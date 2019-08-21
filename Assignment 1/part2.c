#include <stdio.h> 
#include <unistd.h> 
#include<stdlib.h>
#include<sys/stat.h>
 #include <fcntl.h> 
#include <sys/types.h>
#include<string.h>
#define ll long long
void yes() { write(1, "Yes\n", 5); }
void no() { write(1, "No\n", 4); }
int main(int argc,char** argv)
{
    ll f1,f2;
    ll flag=0;
    ll m1,pointer;
    f1=open(argv[1],O_RDONLY);
    f2=open(argv[2],O_RDONLY);
    if(f1<0)
    {
        perror("Old file don't exist");
        
    }
    if(f2<0)
    {
        perror("New file don't exist");
    }
    ll sz_f1=lseek(f1,0,SEEK_END);
    ll sz_f2=lseek(f2,0,SEEK_END);
    lseek(f1,0,SEEK_SET);
     write(1, "Whether file contents are reversed in newfile: ", 48);
        
    ll SZE=sz_f1;
    if(sz_f1!=sz_f2)
    {
        no();
    }
    else
    {   char *bf=(char*)malloc(100000*sizeof(char));
        char *cf=(char*)malloc(100000*sizeof(char));
            pointer=lseek(f2,0,SEEK_END);  
            while(pointer-100000>=0)
            {
            pointer=lseek(f2,-100000,SEEK_CUR);

            read(f2,cf,100000);
            pointer=lseek(f2,-100000,SEEK_CUR);
            read(f1,bf,100000);            
            for(int i=0;i<100000;i++)
            {
            if(bf[i]!=cf[99999-i])
             {   flag=1;
                break;

            }}
            }
            if(pointer>0)
            {   
        char *df,*ef; 
        df =(char*)malloc(pointer*sizeof(char));
        ef=(char*)malloc(pointer*sizeof(char));

                lseek(f2,-pointer,SEEK_CUR);
                read(f2,ef,pointer);
                read(f1,df,pointer);
                    for(int i=0;i<pointer;i++)
                    {
                     if(df[i]!=ef[pointer-1-i])
                        flag=1;
                    }
            
            }
        close(f2);
        close(f1);
        if(flag == 0){
            yes();
          }
        else{
            no();   
        }
    }
    struct stat buffer;
      write(1, "newfile is created: ", 21);
    if (stat(argv[2], &buffer) != 0) no();
    else
    {
        yes();
        write(1, "User has read permissions for newfile: ", 39);
        if (buffer.st_mode & S_IRUSR) yes();    else no();
        write(1, "User has write permission for newfile: ", 39);
        if (buffer.st_mode & S_IWUSR) yes();    else no();
        write(1, "User has execute permission for newfile: ", 41);
        if (buffer.st_mode & S_IXUSR) yes();	else no();
        write(1, "Group has read permissions for newfile: ", 39);
        if (buffer.st_mode & S_IRGRP) yes();	else no();
        write(1, "Group has write permission for newfile: ", 39);
        if (buffer.st_mode & S_IWGRP) yes();	else no();
        write(1, "Group has execute permission for newfile: ", 41);
        if (buffer.st_mode & S_IXGRP) yes();	else no();
        write(1, "Others has read permissions for newfile: ", 39);
        if (buffer.st_mode & S_IROTH) yes();	else no();
        write(1, "Others has write permission for newfile: ", 39);
        if (buffer.st_mode & S_IWOTH) yes();	else no();
        write(1, "Others has execute permission for newfile: ", 43);
        if (buffer.st_mode & S_IXOTH) yes();	else no();
    }
    write(1, "oldfile is created: ", 21);
    if (stat(argv[1], &buffer) != 0) no();
    else
    {
        yes();
        write(1, "User has read permissions for oldfile: ", 39);
        if (buffer.st_mode & S_IRUSR) yes();	else no();
        write(1, "User has write permission for oldfile: ", 39);
        if (buffer.st_mode & S_IWUSR) yes();	else no();
        write(1, "User has execute permission for oldfile: ", 41);
        if (buffer.st_mode & S_IXUSR) yes();	else no();
        write(1, "Group has read permissions for oldfile: ", 39);
        if (buffer.st_mode & S_IRGRP) yes();	else no();
        write(1, "Group has write permission for oldfile: ", 39);
        if (buffer.st_mode & S_IWGRP) yes();	else no();
        write(1, "Group has execute permission for oldfile: ", 41);
        if (buffer.st_mode & S_IXGRP) yes();	else no();
        write(1, "Others has read permissions for oldfile: ", 39);
        if (buffer.st_mode & S_IROTH) yes();	else no();
        write(1, "Others has write permission for oldfile: ", 39);
        if (buffer.st_mode & S_IWOTH) yes();	else no();
        write(1, "Others has execute permission for oldfile: ", 43);
        if (buffer.st_mode & S_IXOTH) yes();	else no();
    }
    write(1, "Directory is created: ", 23);
    if (stat(argv[3], &buffer) != 0 || !S_ISDIR(buffer.st_mode))
        no();
    else
    {
        yes();
        write(1, "User has read permissions for directory: ", 41);
        if (buffer.st_mode & S_IRUSR) yes();	else no();
        write(1, "User has write permission for directory: ", 41);
        if (buffer.st_mode & S_IWUSR) yes();	else no();
        write(1, "User has execute permission for directory: ", 43);
        if (buffer.st_mode & S_IXUSR) yes();	else no();
        write(1, "Group has read permissions for directory: ", 41);
        if (buffer.st_mode & S_IRGRP) yes();	else no();
        write(1, "Group has write permission for directory: ", 41);
        if (buffer.st_mode & S_IWGRP) yes();	else no();
        write(1, "Group has execute permission for directory: ", 43);
        if (buffer.st_mode & S_IXGRP) yes();	else no();
        write(1, "Others has read permissions for directory: ", 41);
        if (buffer.st_mode & S_IROTH) yes();	else no();
        write(1, "Others has write permission for directory: ", 41);
        if (buffer.st_mode & S_IWOTH) yes();	else no();
        write(1, "Others has execute permission for directory: ", 45);
        if (buffer.st_mode & S_IXOTH) yes();	else no();
    }
  
    return 0;
}

        