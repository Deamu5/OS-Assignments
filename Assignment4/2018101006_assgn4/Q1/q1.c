#define _POSIX_C_SOURCE 199309L //required for clock
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <inttypes.h>
#include <sys/shm.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void swap(int* a, int* b) { 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 

int * shareMem(size_t size){
    key_t mem_key = IPC_PRIVATE;
    int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
    return (int*)shmat(shm_id, NULL, 0);
}
struct arg{
    int l;
    int r;
    int* arr;    
};

int pivot_partition(int *arr, int l, int r){
   //partition
    int i =l-1;  
    int pivot=arr[r];   
    for (int j=l;j<=r-1;j++) { 
        if (arr[j] < pivot){ 
            i++;    
            swap(&arr[i], &arr[j]); 
        } }
    swap(&arr[i + 1], &arr[r]); 
    return (i + 1);
}
void normal_qsort(int *arr, int l, int r){
    if(l>r) return;

    //insertion sort
    if(r-l+1<5){
        for(int i=l;i<r;i++){
            int j=i+1; 
            for(;j<=r;j++)
                if(arr[j]<arr[i] && j<=r) {
                    int temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
        }
        return;
    }
    int piv=pivot_partition(arr, l, r);

    normal_qsort(arr, l, piv-1);
    normal_qsort(arr, piv+1, r);
    }

void concurrent_qsort(int *arr, int l, int r){
    if(l>r) _exit(1);    
    if(r-l+1<5){
        for(int i=l;i<r;i++){
            int j=i+1; 
            for(;j<=r;j++)
                if(arr[j]<arr[i] && j<=r) {
                    int temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
        }
        return;
    } 
    int piv=pivot_partition(arr, l, r);
    int pid1 = fork();
    int pid2;
    if(pid1==0){
        concurrent_qsort(arr, l, piv-1);
        _exit(1);
    }
    else{
        pid2 = fork();
        if(pid2==0){
            concurrent_qsort(arr,piv+1,r);
            _exit(1);
        }
        else{
            //wait for the right and the left half to get sorted
            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        }
    }
    return;
}

void *threaded_qsort(void* a){
    struct arg *args = (struct arg*) a;
    int l = args->l;
    int r = args->r;
    int *arr = args->arr;
    if(l>r) return NULL;    
    if(r-l+1<5){
        int a[5], mi=INT_MAX, mid=-1;
        for(int i=l;i<r;i++){
            int j=i+1; 
            for(;j<=r;j++)            
                if(arr[j]<arr[i] && j<=r) 
                {
                    int temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
        }
        return NULL;
    }
    int piv= pivot_partition(arr,l,r);
    struct arg a1;
    a1.l = l;
    a1.r = piv-1;
    a1.arr = arr;
    pthread_t tid1;
    pthread_create(&tid1, NULL, threaded_qsort, &a1);  
    //sort right half array
    struct arg a2;
    a2.l = piv+1;
    a2.r = r;
    a2.arr = arr;
    pthread_t tid2;
    pthread_create(&tid2, NULL, threaded_qsort, &a2);
    //wait for the two halves to get sorted
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
 //  int piv= pivot_partition(arr,l,r);
}

void launch(long long int n){
    struct timespec ts;
    //getting shared memory
    int *arr = shareMem(sizeof(int)*(n+1));
    for(int i=0;i<n;i++) scanf("%d", arr+i);

    int brr[n+1];
    for(int i=0;i<n;i++) brr[i] = arr[i];

    printf("Running concurrent_qsort for n = %lld\n", n);
    printf("\nbefore sorting\n");
    for(int i=0;i<n;i++) printf("%d ",arr[i]);
    printf("\n");

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
    //multiprocess concurrent_qsort
    concurrent_qsort(arr, 0, n-1);
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);
    long double t1 = en-st;
    
    printf("\nafter sorting\n");
        for(int i=0;i<n;i++) printf("%d ",arr[i]);
        printf("\n");
     for(int i=0;i<n;i++) arr[i] = brr[i];

    pthread_t tid;
    struct arg a;
    a.l = 0;
    a.r = n-1;
    a.arr = brr;

    printf("Running threaded_concurrent_qsort for n = %lld\n", n);
    printf("\nbefore sorting\n");
    for(int i=0;i<n;i++) printf("%d ",brr[i]);
    printf("\n");

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;

    //multithreaded concurrent_qsort
    pthread_create(&tid, NULL, threaded_qsort, &a);
    pthread_join(tid, NULL);    
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);
    long double t2 = en-st;
        printf("\nafter sorting\n");
        for(int i=0;i<n;i++) printf("%d ",brr[i]);
        printf("\n");
     for(int i=0;i<n;i++) brr[i] = arr[i];
   

    printf("Running normal_qsort for n = %lld\n", n);
     printf("\nbefore sorting\n");
    for(int i=0;i<n;i++) printf("%d ",brr[i]);
    printf("\n");
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
    // normal qsort
    normal_qsort(brr, 0, n-1);    
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);
    long double t3 = en - st;
      printf("\nafter sorting\n");
        for(int i=0;i<n;i++) printf("%d ",brr[i]);
        printf("\n");
    printf("normal_qsort ran:\n\t[ %Lf ] times faster than concurrent_qsort\n\t[ %Lf ] times faster than threaded_concurrent_qsort\n\n\n", t1/t3, t2/t3);
    shmdt(arr);
    return;
}

int main(){
    long long int n;
    scanf("%lld", &n);
    launch(n);
    return 0;
}