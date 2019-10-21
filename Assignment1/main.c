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
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>

//for allocating space to array
#define buffer_size 1000
int bgFlag;
int currpid;
int killy,bgy;
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
char bgnames[1000][30];
int bgC=0;
//following function is used to make seperate tokens when multiple commands are entered at once 
char **splitting(char *line,int flag)
{
    int pos=0;
    char **tokens=malloc(1000*sizeof(char*));
    char *token;
    char *t1, *t2;
    if(flag) token = strtok_r(line, COMMAND_DELIM, &t1);
    else     token = strtok_r(line, TOK_DELIM, &t2);
    while(token !=NULL){
        strcat(token, "");
        tokens[pos++] = token;
        if(flag) token = strtok_r(NULL, COMMAND_DELIM, &t1);
        else     token = strtok_r(NULL, TOK_DELIM, &t2);
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
int setenvfunction(char **args){
	if(args[0]==NULL || args[3]!=NULL)   printf("Invalid argument \n");
    if (args[2] == NULL){
        args[2] = " ";
    }
	if (setenv(args[1],args[2],1) != 0)    perror("shell ");
	return 1;	
}
int unsetenvfunction(char **args){
	if(args[0]==NULL || args[3]!=NULL)  printf("Invalid argument \n");
    if (unsetenv(args[1]) != 0) perror("shell");
	return 1;
}
int getenvfunction(char **args){
    if(getenv(args[1])==0)
    {
        fprintf(stderr,"Please check the format\n");
    }
    else{
        printf("%s\n",getenv(args[1]));
    }
}
int quit(){
    exit(0);
}
  void ctrlcignore(int sig_num) { 
      signal(SIGINT, ctrlcignore); 
     fflush(stdout); 
} 
 void exithandler(int sig_num){  
    kill(currpid,9);
    fflush(stdout);
}
void  killo(int sig_num){
    // printf("ENTERED CTRZ\n");
       bgy=1;
}

int overkill(char **args) {
    union sigval sval;
    sval.sival_int = 1;
    for (int k = 0; k < bgC; k++) {
        if (bgflag[k]!=0 && bgflag[k]!=2){
            printf("pid  :  %d killed \n",bgarr[k]);
            sigqueue(bgarr[k], SIGKILL, sval);
            bgflag[k]=2;
            }
    }
    return 1;
}

int kjob(char **args){
    if(args[1]==NULL || args[2]==NULL)
    {
        fprintf(stderr,"Format kjobs <job num> <signal num>\n");
        return 1;
    }
    int jobpid;
     int jobnum = atoi(args[1]);
    int sig=atoi(args[2]);
    _Bool flag=0; 
    for (int i=0;i<bgC;i++){
        if ( bgflag[i]!=0 && bgflag[i]!=2){
            if(--jobnum==0){
            jobpid=bgarr[i];
            flag=1;
            break;
            }
        }
    }
    if (flag==1)    kill(jobpid,sig);
    else        fprintf(stderr,"Job doesn't exist\n"); 
}
int bg(char ** args){
      if(args[1]==NULL){
        fprintf(stderr,"Please print the job number");
        return 1;
    }
    _Bool flag=0;
    int jobPid;
    int jobnum = atoi(args[1]);
    for (int i=0;i<bgC;i++){
        if ( bgflag[i]!=0 && bgflag[i]!=2){
            if(--jobnum==0){
            jobPid=bgarr[i];
            flag=1;
            break;
            }
        }
    }
    if(jobPid < 0){
        fprintf (stderr,"Job %s doesnot exist!\n", args[1]);
        return 1;
    }
    
    if(flag && kill(jobPid,SIGCONT)<0){
        fprintf(stderr,"Job %s doesnt exist! \n",args[1]);
        return 1;
    }
}
int fg(char **args){
          if(args[1]==NULL){
        fprintf(stderr,"Please print the job number");
        return 1;}
   _Bool flag=0;
    int jobPid;
    int jobnum = atoi(args[1]);
    for (int i=0;i<bgC;i++){
        if ( bgflag[i]!=0 && bgflag[i]!=2){
            if(--jobnum==0){
            jobPid=bgarr[i];
            flag=1;
            break;
            }
        }
    }

      if(jobPid < 0 || flag==0){
        fprintf (stderr,"Job  doesn't exist!\n");
        return 1;
    }
     kill(jobPid,SIGCONT);
        int pid,status,wpid;
        //pid =getpid();
        signal(SIGINT, exithandler); 
                  signal(SIGTSTP,killo);  
        pid=jobPid;
        currpid=jobPid;
       do{
        // currpid = bgarr[bgC-1];
        //  printf(" job pid %d bgy is %d\n",jobPid,bgy);
               
            if(bgy == 1){
                     setpgid(pid,pid);
                    kill(pid,SIGTSTP);
                    bgy=0;
                    break;
                
            }
                int tp = waitpid(jobPid, &status, WUNTRACED | WNOHANG);
                if(tp == jobPid) {
                    // printf("EXITED HERE\n");
                    break;
                }

            // }while(!WIFEXITED(status) && !WIFSIGNALED(status));
            }while(1);
       
}
int jobs(char ** args){
      int j=1;
      for (int i = 0; i < bgC; i++) {
    //  printf(" curr %d bgF= %d\n",bgarr[i],bgflag[i]);
        if (bgflag[i]!=2 ){
        char path[buffer_size] = "/proc/";
        char *pidNo =allocation();
        int n,r, length = 0;
        int  num=bgarr[i];
        n = num;
        while (n != 0){
        length++;
            n/= 10;
        }
        for (int i=0;i<length;i++){
            r=num%10;
            num=num/10;
            pidNo[length-(i+1)]=r+48;
        }
        pidNo[length] = '\0';
            strcat(path,pidNo);
            strcat(path,"/status");
            FILE * status;
                char *line =NULL;
                size_t len = 0;
                ssize_t read;
                status = fopen(path,"r");
             while((read = getline(&line,&len,status))!= -1)
                {
                    char **argv = splitting(line,0);
                    if(strcmp(argv[0],"State:")==0){
                        if(strcmp(argv[1],"T")==0)
                            printf("[%d] Stopped %s [%d]\n",j++,bgnames[i], bgarr[i]);
                        else 
                            printf("[%d] Running %s [%d]\n",j++,bgnames[i], bgarr[i]);
                        
                        break;
                    }
                }
            fclose(status);
        }
    }

}
int cronjob(char ** args){
    int j=0,k=2;
while(strcmp(args[k],"-p")!=0)  k++;    
int total = atoi(args[k+1]);
k=2;
while(strcmp(args[k],"-t")!=0)   k++;    
int interval= atoi(args[k+1]);
char **cmd=malloc(10);
for(int i=0;i<10;i++) cmd[i]=(char *)malloc(20);
k=2;
while(strcmp(args[k],"-t")!=0){
    strcpy(cmd[j++],args[k++]);
}
 cmd[j]=NULL;
pid_t pid;
    while(total>0){
        sleep(interval);
        total-=interval;
        
   pid =fork();
        if (pid==0){
             if (execvp(cmd[0],cmd)==-1){
                fprintf(stderr, "Not valid Command\n");
            }
            exit(EXIT_FAILURE);       
           }
        else{
        wait(NULL);
        }
    
    }
 }

// predefined commands
typedef int (*command)(char**);
char *built[]={"cd","pwd","echo","ls","pinfo","quit","setenv","unsetenv","getenv","overkill","kjob","bg","fg","jobs","cronjob"};
command builtin[] = {&cd,&pwd,&echo,&ls,&pinfo,&quit,&setenvfunction,&unsetenvfunction,&getenvfunction,&overkill,&kjob,&bg,&fg,&jobs,&cronjob};
int killy,bgy;
void redirection(char ** args){
 //   for(int i=0;args[i]!=NULL;i++){
   // printf("%s  ",args[i]);}
   char *input=allocation();
   char *output=allocation();
    _Bool fg1=0,fg2=0,fg3=0;
    int fd1,fd2,savein,saveout;
    for(int i=0;args[i]!=NULL;i++){
        if(strcmp(args[i],"<")==0)    fg1=1;
        if(strcmp(args[i],">")==0)    fg2=1;
        if(strcmp(args[i],">>")==0)    fg3=1;
    }
    
    if (fg1) {
        for(int i=0;args[i]!=NULL;i++) if(strcmp(args[i],"<")==0) strcpy(input,args[i+1]);
        
    }
    if(fg2==1 || fg3==1)
        for(int i=0;args[i]!=NULL;i++) if(strcmp(args[i],">")==0 ||strcmp(args[i],">>")==0)  strcpy(output,args[i+1]); 
           
    for(int  k=0;args[k]!=NULL;k++){
       if(strcmp(args[k],">")==0 || strcmp(args[k],"<")==0 || strcmp(args[k],">>")==0 ){
        args[k]=NULL;
         }
    }
    if(fg1==1){ 
		struct stat file;
		savein=dup(0);
        if(stat(input,&file)==-1){
			perror("file does not exist\n");
		}
   		fd1 = open(input,O_RDONLY, 0644);
		dup2(fd1,0);
        close(fd1);
	}
    if( fg2==1 || fg3==1){
       saveout=dup(1);
        if(fg2==1)  fd2 = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else        fd2 = open(output, O_WRONLY | O_CREAT | O_APPEND, 0644);
		dup2(fd2,1);
        close(fd2);
	}
    pid_t pid;
	pid=fork();
	if(pid==-1){
		perror("error forking : ");
		return ;
	}
	else if(pid==0){
		if(execvp(args[0],args)==-1)		{
			perror("invalid command :");
			exit(EXIT_FAILURE);
		}
    }
	else{
		wait(NULL);
		dup2(savein,0);
		dup2(saveout,1);
    }
   
}
void piping(char ** args){
    //     int pipes=0;
    //   for( int k=0;args[k]!=NULL;k++) if (strcmp(args[k],"|")==0) pipes++;
	int i=0,j=0,finish=0;
	int cnt=0;
	char *cur_com[1000];
	int frd;
	while(1)
	{
		if(args[i]==NULL)break;
		if(finish==1)break;
		int p=0;
		while(1){
			if(args[j]==NULL)
			{
				finish=1;
				break;
			}
			if(strcmp(args[j],"|")!=0)
			{
				cur_com[p]=args[j];
				// printf("%s\n",cur_com[p]);
				p++;
			}
			else break;
			if(args[j]==NULL)
			{
				finish=1;
				break;
			}
			j++;
		}
		cnt++;
		j++;
		cur_com[p]=NULL;
		int z=0;
		int fd[2];
		int in,out;
		// printf("%lld\n",finish );
		if(cnt==1)
		{	
			if(pipe(fd)==-1)
			{
				perror("piping error :");
				return;
			}
			int z=0;
			char infile[1000];
			while(cur_com[z]!=NULL)
			{
				// printf("%s\n",cur_com[z] );
				if(strcmp(cur_com[z],"<")==0)
				{
					cur_com[z]=NULL;
					strcpy(infile,cur_com[z+1]);
					// int zz=0;
					// while(cur_com[zz]!=NULL)
					// {
						// printf("%s\n",cur_com[zz] );
						// zz++;
					// }
					in=dup(0);
			   		frd = open(infile,O_RDONLY, 0644);
					dup2(frd,0);
				}
				z++;				
			}
			out=dup(1);
			dup2(fd[1],1);
			close(fd[1]);
		}
		else if(finish==1)	
		{
			// redirect
			int z=0;
			char outfile[1000];
			int flagg=0;
			while(cur_com[z]!=NULL)
			{
				// printf("%s\n",cur_com[z] );
				if(strcmp(cur_com[z],">")==0)
				{
					cur_com[z]=NULL;
					strcpy(outfile,cur_com[z+1]);
					out=dup(1);
					frd= open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					dup2(frd,1);
					close(frd);
					flagg=1;
				}
				else if(strcmp(cur_com[z],">>")==0)
				{
					cur_com[z]=NULL;
					strcpy(outfile,cur_com[z+1]);
					out=dup(1);
					frd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
					dup2(frd,1);
					close(frd);
					flagg=1;
				}
				z++;				
			}
			in = dup(0);
			dup2(fd[0],0);
			close(fd[0]);
		}
		else
		{
			in = dup(0);
			dup2(fd[0],0);
			if(pipe(fd)==-1)
			{
				perror("piping error :");
				return;
			}
			out=dup(1);
			dup2(fd[1],1);
			close(fd[1]);
		}
		pid_t pid;
		pid=fork(); 

		if(pid==-1)
		{
			perror("fork error : ");
			return;
		}
		else if(!pid)
		{
			if(execvp(cur_com[0],cur_com)==-1)
			{
				perror("command not found");
				exit(EXIT_FAILURE);
			}
		} 
		else
		{
			wait(NULL);
			dup2(in,0);
			dup2(out,1);
		}

	}
}

//below two function are used to get the host name and user name
char *finalPath(char * name)
{
    getcwd(path,1024);
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
void display(char *store)
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
    sprintf(store, "<%s@%s:%s>",name,sysname,path);
    
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
void launch(char **args){
    check();
    bgFlag=0;
    int u=0;
       long long int count = sizeof(builtin)/sizeof(int*);
          while(args[u]!=NULL){ if(strcmp(args[u],"|")==0){
                    piping(args);
                    return;
                    }
            u++;
          }
          u=0;
          while(args[u]!=NULL){
              if (strcmp(args[u],"&")==0) bgFlag=1;
              if  (strcmp(args[u],"<")==0 || strcmp(args[u],">")==0 || strcmp(args[u],">>")==0){
                  redirection(args);
                  return;
              }
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
                fprintf(stderr, "Not valid Command\n");
            }
            exit(EXIT_FAILURE);
        }
        else if (pid>0){
            
        if(bgFlag==0){
                  signal(SIGINT, exithandler); 
                  signal(SIGTSTP,killo);  
            do{
                currpid=pid;
                if(bgy == 1){
                    if(strcmp(args[0],"fg")==0)
                        break;
                    setpgid(pid,pid);
                    bgarr[bgC] = pid;
                   strcpy(bgnames[bgC],args[0]);
                    bgflag[bgC++]=1;
                    kill(pid,SIGSTOP);
                    bgy=0;
                    break;
                
                }
                     wpid=waitpid(pid,&status,WUNTRACED);
    
            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
       
        }
        else{ 
            setpgid(pid,pid);
            printf("child bg process running with pid : %d\n",pid);
                bgarr[bgC]=pid;
                strcpy(bgnames[bgC],args[0]);
                bgflag[bgC++]=1;
                
        }
    }

}
void check(){
  
    for (int l=0;l<bgC;l++){
    char  pathStat[buffer_size] = "/proc/";
char * process =allocation();
     int n,r, length = 0;

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
        printf("recently terminated process with pid : %d\n",bgarr[l]);
        bgflag[l]=2;
               }
     else{  
        while((rd = getline(&line,&len,status))!= -1){       
        char **argv = splitting(line,0);
        if(strcmp(argv[0],"State:")==0)
            if(strcmp(argv[1],"Z")==0){
                 printf("recently terminated process with pid : %d\n",bgarr[l]);
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
    using_history(); 
    while(1){
    
     signal(SIGINT, ctrlcignore); 
    char *line =allocation();
    char prmpt[100];
    display(prmpt);
    line = readline(prmpt);
    // while(strlen(line) == 0) {
    //     line = readline(prmpt);
    // }
    add_history(line);
    cmds=splitting(line,1);
    char **argv = malloc(10000);
        for(int i=0;cmds[i]!=NULL;i++){

                argv=splitting(cmds[i],0);
                    launch(argv);
        
        }
    free(argv);
    free(cmds);
    //free(line);
    }

}

