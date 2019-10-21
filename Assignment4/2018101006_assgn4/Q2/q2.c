#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<limits.h>
#include<sys/shm.h>
#include<unistd.h>
#define for1(i,l) for(i=l;i!=-1;i--)
int M, N, K;
pthread_mutex_t mutex_chefs[1000];
pthread_mutex_t mutex_tables[1000];
pthread_t cheftids[1000];
pthread_t tabletids[1000];
pthread_t studenttids[1000];
int num_students;

typedef struct chef{
    int ind;
    int time;
    int num_portion_each;
    int num_vessel_each;
}chefo;
chefo chefs[1000];
typedef struct table{
    int ind;
    int num_slots;
    int available_portion;
}tablo; 
tablo tables[1000];

typedef struct student{
    int ind;
    int status;
}stud ;
stud students[1000];

int generator(int left, int right){
    int num = rand()%(right - left + 1);
    num=num+left;
    return num;
}

void biryani_ready(int ind);
void *chef_init(void *args);
void ready_to_serve_table(int num_slots, int ind);
void *table_init(void *args);
void student_in_slot(int ind, int j);
void wait_for_slots(int ind);


void *student_init(void *args){
    int ind = *(int *)args;
    printf("Student %d has arrived.\n", ind);
    wait_for_slots(ind);
}
void launch(){
    num_students = K;
   int i;
    for1(i,M-1){
        usleep(100);
      
        chefs[M-i].ind = M-i;
        pthread_create(&cheftids[M-i], NULL, chef_init, (void *)&chefs[M-i].ind);
    }
    
   
    for1(i,N-1){
        usleep(100);
      
        tables[N-i].ind = N-i;
        pthread_create(&tabletids[N-i], NULL, table_init, (void *)&tables[N-i].ind);
    }
   
    for1(i,K-1){
        usleep(100);  
        students[K-i].ind = K-i;
        pthread_create(&studenttids[K-i], NULL, student_init, (void *)&students[K-i].ind);
    }
    for1(i,K-1)
        pthread_join(studenttids[K-i],NULL);
}
int main(void){
    srand(time(NULL));
    printf("Give the no of Robot Chefs ,Serving Tables ,Students:\n");
    scanf("%d %d %d", &M,&N,&K);
    launch();
    sleep(1);
    printf("Job Done\n");
    return 0;
}
void biryani_ready(int ind){
    while(chefs[ind].num_vessel_each){
        int j;
        for1(j,N-1){
            if(!pthread_mutex_trylock(&mutex_tables[N-j])){
                if(!tables[N-j].available_portion){
                printf("Robot Chef %d is refilling Serving Container of Serving Table %d\n", chefs[ind].ind, tables[N-j].ind);
                chefs[ind].num_vessel_each=chefs[ind].num_vessel_each-1;
                tables[N-j].available_portion = chefs[ind].num_portion_each;
                printf("Serving Container of Table %d is refilled by Robot Chef %d; Table %d resuming serving now\n", tables[N-j].ind, chefs[ind].ind, tables[N-j].ind);
                sleep(1);
                }
                pthread_mutex_unlock(&mutex_tables[N-j]);
            if(chefs[ind].num_vessel_each==0)     break;
            }
        }
    }
    printf("All the vessels prepared by Robot Chef %d are emptied. Resuming cooking now.\n", chefs[ind].ind);
}
void *chef_init(void *args){
    int check = 0;
    int ind = *(int *)args;
    while(check==0){
        if(num_students!=0){
            chefs[ind].num_vessel_each = generator(1, 10);
            chefs[ind].time = generator(2, 5);
            printf("Robot Chef %d is preparing %d vessels of Biryani\n",chefs[ind].ind, chefs[ind].num_vessel_each);
            chefs[ind].num_portion_each = generator(25, 50);
            sleep(chefs[ind].time);
            printf("Robot Chef %d has prepared %d vessels of Biryani. Waiting for all the vessels to be emptied to resume cooking\n", chefs[ind].ind, chefs[ind].num_vessel_each); 
            biryani_ready(ind);
        }
        else  break;
    }
}


void student_in_slot(int ind, int j){
    tables[j].available_portion=tables[j].available_portion-1;
    if(!tables[j].available_portion)
        printf("Serving Container of Table %d is empty, waiting for refill\n", tables[j].ind);
    num_students-=1;
    tables[j].num_slots-=1;
    pthread_mutex_unlock(&mutex_tables[j]);
    while(num_students!=0 && tables[j].num_slots!=0){    
    }
    printf("Student %d on serving table %d has been served\n", students[ind].ind, tables[j].ind);
}
void *table_init(void *args){
    int ind = *(int *)args;
    int check = 0;   
    while(check==0){
        if(num_students==0)
            break;
        if(tables[ind].available_portion!=0){
            int num_slots = generator(1, 10);
            if(tables[ind].available_portion<num_slots)
                tables[ind].num_slots = tables[ind].available_portion;
            else tables[ind].num_slots = num_slots;
            
            printf("%d slots are ready to be served by Table %d\n", tables[ind].num_slots, tables[ind].ind);
            ready_to_serve_table(tables[ind].num_slots, tables[ind].ind);
        }}
    }
void ready_to_serve_table(int num_slots, int ind){
    int check = 0;
    while(check==0){
        if(pthread_mutex_trylock(&mutex_tables[ind])==0){
            if(num_students==0 || !tables[ind].num_slots ){
                pthread_mutex_unlock(&mutex_tables[ind]);
                printf("Serving table %d entering Serving Phase\n", tables[ind].ind);
               
                break;
            }
            pthread_mutex_unlock(&mutex_tables[ind]);
        }}
}


void wait_for_slots(int ind){
    int check = 0;
    printf("Student %d is waiting to be allocated a slot on the serving table\n", ind);
    while(check==0){
        int j = N;
        while(j--){
            if(!pthread_mutex_trylock(&mutex_tables[N-j])){
                if(tables[N-j].num_slots!=0){
                    printf("Student %d assigned a slot on the serving table %d and waiting to be served.\n", ind, N-j);
                    check = 1;
                    student_in_slot(ind, N-j);
                    pthread_mutex_unlock(&mutex_tables[N-j]);
                    break;
                }
                pthread_mutex_unlock(&mutex_tables[N-j]);
            }
        }
    }
}

