#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include<pwd.h>

//for allocating space to array
#define buffer_size 1000
int bg;
//for making tokens
#define COMMAND_DELIM ";"
#define TOK_DELIM " \n\t\a\r"
char *actual_home;
//below function is used to allocate space to a pointer
char *allocation(){
    char *newp = (char*)malloc(buffer_size*sizeof(char));
    return newp;
}
void check();
char *path;
int bgarr[1000];
int bgflag[1000];
int bgC=0;
//following function is used to make seperate tokens when multiple commands are entered at once 
char **splitting(char *line,int flag)
{
    int pos=0;
    char **tokens=malloc(1000*sizeof(char*));
    char *token;
    if(flag) token = strtok(line, COMMAND_DELIM);
    else     token = strtok(line, TOK_DELIM);
    while(token !=NULL){
        tokens[pos++] = token;
        if(flag) token = strtok(NULL, COMMAND_DELIM);
        else     token = strtok(NULL, TOK_DELIM);
    }
    tokens[pos] = NULL;
    free(token);
    return tokens;
}
int cd(char **args)
{    if (!args[1] || (strcmp("~", args[1]) == 0) ||(strcmp("&", args[1]) == 0) ) chdir(actual_home);
    else { 	
        if (strncmp("~", args[1], 1) == 0){
	        char* fpath = (char*)malloc(1024);
            strcpy(fpath, actual_home); 
		    strcat(fpath, &args[1][1]);
		    if(chdir(fpath)!=-1){
            path=fpath;
            chdir(path);
           }
           else{
            	printf("Error: could not change directory\n");
           }
        }
        else{
            char* fpath=allocation();
             getcwd(fpath,1024);
            strcat(fpath,"/");
            strcat(fpath,args[1]);
           if(chdir(fpath)!=-1){
            path=fpath;
            chdir(path);
           }
           else{
            	printf("Error: could not change directory\n");
           }
        }
    }
    return 1;
}
int pwd(char **args)
{   
    char *pth = allocation();
    getcwd(pth,1024);
    printf("%s\n",pth);
    free(pth);
    return 1;
}
int echo(char **args)
{
    for(int i=1;args[i]!=NULL;i++){
        printf("%s ",args[i]);
    } 
    printf("\n");
    return 1;
}
int ls(char **args)
{   
    int all,hidden;
    char *path = allocation();
    path = ".";
    all=hidden=0;
    int k=1;
    while(args[k]!=NULL){
        if(args[k][0]=='-'){
            if(args[k][1] == 'l') all = 1;
            if(args[k][1] == 'a') hidden =1;
            if(args[k][2] == 'l' && args[k][2]!='\0')    all =1;
            if(args[k][2] == 'a' && args[k][2]!='\0')    hidden =1;
        }
        else if(strcmp(args[k],"&")!=0)     path = args[k];
    k++;    
        
    } 
    struct stat check;
    char *name = path;
    struct dirent *dir;
    DIR *tempdir = opendir(path);
    if(tempdir == NULL){
        int isFile = stat(path,&check);
        if(!isFile){
            if(all == 1){
            int pos=0;
            int fileMode = check.st_mode;
            char permission[11];
            char *token = strtok(ctime(&check.st_mtime),TOK_DELIM);
            char **tokens = malloc(buffer_size*sizeof(char*));
            struct group  *gr = getgrgid(check.st_gid);
            struct passwd *pw = getpwuid(check.st_uid);
            
            for(int i=0;i<10;i++)permission[i]='-';
            if (fileMode & S_IRUSR)    permission[1] = 'r';
            if (fileMode & S_IWUSR)    permission[2] = 'w';
            if (fileMode & S_IXUSR)    permission[3] = 'x';
            if (fileMode & S_IRGRP)    permission[4] = 'r';
            if (fileMode & S_IWGRP)    permission[5] = 'w';
            if (fileMode & S_IXGRP)    permission[6] = 'x';
            if (fileMode & S_IROTH)    permission[7] = 'r';
            if (fileMode & S_IWOTH)    permission[8] = 'w';       
            if (fileMode & S_IXOTH)    permission[9] = 'x';
            while(token!=NULL){
                tokens[pos++]=token;
                token = strtok(NULL," ");
            }
            printf("%s\t%s\t%s\t%ld\t%s %s\t%s ",permission,pw->pw_name,gr->gr_name,check.st_size,tokens[1],tokens[2],tokens[3]);
            }
            printf("%s\n",name);
            return 1;
        }     
    }
    while( NULL != (dir = readdir(tempdir)) ){
        if(all==0){
            if(hidden==0)
            {       if (dir->d_name[0] != '.')
                    printf("%s\t",dir->d_name);
            }
        
            else            printf("%s\t",dir->d_name);
        }
        else{
            int pos=0;
            struct stat attrib;
            char *fname = dir->d_name;
            stat(fname,&attrib);
            int fileMode = attrib.st_mode;
            char permission[11];
            for(int i=0;i<10;i++)permission[i]='-';
            char *token = strtok(ctime(&attrib.st_mtime),TOK_DELIM);
            char **tokens = malloc(buffer_size*sizeof(char*));
            struct group  *gr = getgrgid(attrib.st_gid);
            struct passwd *pw = getpwuid(attrib.st_uid);
            
            if(S_ISDIR(fileMode))    permission[0]='d';
            if (fileMode & S_IRUSR)    permission[1] = 'r';
            if (fileMode & S_IWUSR)    permission[2] = 'w';
            if (fileMode & S_IXUSR)    permission[3] = 'x';
            if (fileMode & S_IRGRP)    permission[4] = 'r';
            if (fileMode & S_IWGRP)    permission[5] = 'w';
            if (fileMode & S_IXGRP)    permission[6] = 'x';
            if (fileMode & S_IROTH)    permission[7] = 'r';
            if (fileMode & S_IWOTH)    permission[8] = 'w';       
            if (fileMode & S_IXOTH)    permission[9] = 'x';
            
            while(token!=NULL){
                tokens[pos++]=token;
                token = strtok(NULL," ");
            }
            if(hidden==1)
                
                printf("%s\t%s\t%s\t%ld\t%s %s\t%s %s\n",permission,pw->pw_name,gr->gr_name,attrib.st_size,tokens[1],tokens[2],tokens[3],dir->d_name);
                
            else{
                if(dir->d_name[0] !='.')
                   printf("%s\t%s\t%s\t%ld\t%s %s\t%s %s\n",permission,pw->pw_name,gr->gr_name,attrib.st_size,tokens[1],tokens[2],tokens[3],dir->d_name); 
            }
        }
    }
     closedir(tempdir);
    free(dir);
    printf("\n");
    return 1;
}
int pinfo(char **args){
    char *process = allocation();
    if(args[1] && strcmp(args[1],"&")!=0){
        process = args[1];
    }
    else{
        int n,r, length = 0;
        int  num=getpid();
        n = num;
        while (n != 0){
        length++;
            n/= 10;
        }
        for (int i=0;i<length;i++){
            r=num%10;
            num=num/10;
            process[length-(i+1)]=r+48;
        }
        process[length] = '\0';
    }
    char pathStat[buffer_size] = "/proc/";
    strcat(pathStat,process);
    strcat(pathStat,"/status");
    FILE * status;
    char *line =NULL;
    status = fopen(pathStat,"r");
    if(status == NULL)
    {
        perror("No such process\n");
        return 1;
    }
    size_t len = 100;
    ssize_t rd;
    printf("pid -- %s\n",process);
    while((rd = getline(&line,&len,status))!= -1){       
        char **argv = splitting(line,0);
        if(strcmp(argv[0],"State:")==0)
            printf("Process Status -- %s\n",argv[1]);
        if(strcmp(argv[0],"VmSize:")==0)
            printf("memory --  %s\n",argv[1]);
    }
    char exeStat[buffer_size] = "/proc/";
    strcat(exeStat,process);
    strcat(exeStat,"/exe");
    char *exePath = allocation();
    rd = readlink(exeStat,exePath,buffer_size-1);
    if(rd==0){
        perror("Executable Path doesn't exist\n");
        return 1;
    }
    printf("Executable Path -- %s\n",exePath);
    return 1;
}
typedef int (*command)(char**);
char *built[]={"cd","pwd","echo","ls","pinfo"};
command builtin[] = {&cd,&pwd,&echo,&ls,&pinfo};

//below two function are used to get the host name and user name
char *finalPath(char * name)
{
    getcwd(path,1024);
  // printf("path :%s\n actual_home : %s\n",path,actual_home);
    int path_length = strlen(path);
    int actual_home_length = strlen(actual_home);

    //printf("%d %d",path_length,actual_home_length);
    if(path_length <= actual_home_length)
        return path;
    else if(path[actual_home_length]!='/')
        return path;
    else 
    {   char *finalpath = allocation();
        int j = 1;
        finalpath[0]='~';
        //finalpath[1]='/';
        for(int i=actual_home_length;path[i]!='\0';i++)
        {
          //  printf("j==%d i==%d\n",j,i);
            finalpath[j++] = path[i]; 
        }
        finalpath[j]='\0';
      //  printf("\n%s\n",finalpath); 
           return finalpath;
    }
}
//below command is used to display the username,hostname and current directory
void display()
{
    uid_t uid;
    struct passwd *pw;
    uid = geteuid();
    pw = getpwuid(uid);
    char *sysname=allocation();
    char*name=allocation();
    
    name = pw->pw_name;
    gethostname(sysname,1000);
    path=finalPath(name);
    if(strcmp(path,actual_home)==0)
        path="~";
    printf("<%s@%s:%s>",name,sysname,path);
    
}
// below function is used to read the input from the user
char *read_line()
{   int pos = 0,valid=1,c;
    char *buffer=allocation();
    while(valid){
        c = getchar();
        if(c==EOF || c == '\n'){
            buffer[pos]='\0';
            valid =0;
            return buffer;
        }
        else{
            buffer[pos] = c;
        }
        pos++;
        
    }
    return buffer;
}
// below command is used to process the command
void launch(char **args)
{   check();
        
    bg=0;
       long long int count = sizeof(builtin)/sizeof(int*);
          int u=0;
          while(args[u]!=NULL){
              if (strcmp(args[u],"&")==0) bg=1;
              u++;
              }

         for(long long int i=0;i<count;i++){
        if(strcmp(args[0],built[i])==0){
            builtin[i](args);
            return;
               }
        }
        pid_t pid,wpid;
        int status;
        pid =fork();
        if (pid==0){
            if (execvp(args[0],args)==-1){
                perror("Not valid Command\n");
            }
            exit(EXIT_FAILURE);
        }
        else{
        if(bg==0){
            do{
                wpid=waitpid(pid,&status,WUNTRACED);
            
            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
       
        }
        else{
            printf("child bg process running with pid : %d\n",pid);
                bgarr[bgC]=pid;
                bgflag[bgC++]=1;
        }
    }

}
void check(){
    char  pathStat[buffer_size] = "/proc/";
char * process =allocation();
     int n,r, length = 0;
    for (int l=0;l<bgC;l++){
        length=0;
        if(bgflag[l]!=2){
        int  num=bgarr[l];
        n = num;
        while (n != 0){
        length++;
            n/= 10;
        }
        for (int i=0;i<length;i++){
            r=num%10;
            num=num/10;
            process[length-(i+1)]=r+48;
        }
        process[length] = '\0';
    strcat(pathStat,process);
    strcat(pathStat,"/status");
    FILE * status;
    status = fopen(pathStat,"r");
    size_t len = 0;
    ssize_t rd;
    char *line=NULL;
    
    if(status == NULL){
        printf("terminated process with pid : %d\n",bgarr[l]);
        bgflag[l]=2;
        
            }
    else{   
        while((rd = getline(&line,&len,status))!= -1){       
        char **argv = splitting(line,0);
        if(strcmp(argv[0],"State:")==0)
            if(strcmp(argv[1],"Z")==0){
                 printf("terminated process with pid : %d\n",bgarr[l]);
                bgflag[l]=2;
             }
     }
        }
    }
}
}

int main(){
    char **cmds;    
    actual_home=allocation();
    path = allocation();
   
    system("clear");

    getcwd(actual_home,buffer_size); 
    while(1){
    char *line =allocation();
    //if (bgC >0) printf("%d",bgarr[bgC-1]);
    display();
    line = read_line();
    cmds=splitting(line,1);
    char **argv = malloc(10000);
        for(int i=0;cmds[i]!=NULL;i++){
                argv=splitting(cmds[i],0);
                    launch(argv);
        }
     free(cmds);
    }
}

