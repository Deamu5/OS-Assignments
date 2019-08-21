#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#define ll long long int 
int main(int argc,char *argv[])
{   
     if (argc != 2)
    {
        write(1, "Invalid Format\nValid Format: ./a.out input_filename.txt\n", 100);
        exit(1);
    }
    char *buf=(char*)calloc(1000010,sizeof(char));
    char *newbuf=(char*)calloc(1000010,sizeof(char));
    int ques=open(argv[1],O_RDONLY);
    if (ques < 0)
    {
        write(1,"ERROR:File does not exist\n", 40);
        exit(1);
    }
    mkdir("Assignment", 0700);
    char out[50] = {0};
    strcat(out, "Assignment/");
    strcat(out, argv[1]);
    int ans=open(out,O_CREAT | O_WRONLY,0600);
    ll ptr=lseek(ques,0,SEEK_END);
    //printf("%lld",ptr);
    ll totperc=ptr;
    ll perc=0;
    ll i,x;
    double pcent;
    char* percentage = (char*)malloc(25);
    
    while(ptr-1000010>=0){
    lseek(ques,-1000010,SEEK_CUR);
    read(ques,buf,1000010);
     i=0;
     x=1000010;
    while(x>0){
      newbuf[i++]=buf[--x];
    }
     i=0;
     x=1000010;
    write(ans,newbuf,1000010);    
    perc+=1000010;
    pcent = (perc*100)/totperc;
    sprintf(percentage,"%d \r",(int)pcent);
      fflush(stdout);
      write(1,percentage,strlen(percentage));
    ptr=lseek(ques,-1000010,SEEK_CUR);
    }
    lseek(ques,-ptr,SEEK_CUR);
    read(ques,buf,ptr);
     i=0;
     x=ptr;
    while(x>0){
      newbuf[i++]=buf[--x];
    }
    write(ans,newbuf,ptr);
     perc+=ptr;
    pcent = (perc*100)/totperc;
    sprintf(percentage,"%d",(int)pcent);
      write(1,percentage,strlen(percentage));
  
      
}