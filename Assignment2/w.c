void piping(char **args)
{
	ll i=0,pipes=0;
	//counting number of pipes
	while(args[i]!=NULL)
	{
		if(strcmp(args[i],"|")==0)
		{
			pipes++;
		}
		i++;
	}
	ll sz=i;
	i=0;
	ll j=0,finish=0;
	ll cnt=0;
	char *cur_com[1000];
	ll frd;
	while(1)
	{
		if(args[i]==NULL)break;
		if(finish==1)break;
		ll p=0;
		while(1)
		{
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
		ll z=0;
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
			ll z=0;
			char infile[1000];
			while(cur_com[z]!=NULL)
			{
				// printf("%s\n",cur_com[z] );
				if(strcmp(cur_com[z],"<")==0)
				{
					cur_com[z]=NULL;
					strcpy(infile,cur_com[z+1]);
					// ll zz=0;
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
			ll z=0;
			char outfile[1000];
			ll flagg=0;
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
int redirect(char **args, int num)
{
	ll f1=0,f2=0,f3=0;
	ll prein,preout;
	ll fd[2];
	char *cur_com[1000];
	ll i=0;
	while(args[i]!=NULL)	if(strcmp(args[i++],"<")==0)   f1=1;
	i=0;
	while(args[i]!=NULL)	if(strcmp(args[i++],">")==0)   f2=1;
	i=0;
	while(args[i]!=NULL)    if(strcmp(args[i++],">>")==0)   f3=1;
	i=0;
	char in[1000],out[1000];
	ll x=100000000000;

	if(f1==1)
	{
		i=0;
		while(args[i]!=NULL)
		{
			if(strcmp(args[i],"<")==0)
			{
				if(i-1<x)x=i-1;
				strcpy(in,args[i+1]);
			}
			i++;
		}
	}
	if(f2==1||f3==1)
	{
		i=0;
		while(args[i]!=NULL)	
		{		
			if(strcmp(args[i],">")==0||strcmp(args[i],">>")==0)
			{
				if(i-1<x)x=i-1;
				strcpy(out,args[i+1]);
			}
			i++;
		}
	}
	// printf("%lld %lld %lld \n",f1,f2,f3 );
	args[x+1]=NULL;
	if(f1==1)
	{
		struct stat chk;
		if(stat(in,&chk)==-1)
		{
			perror("file does not exist\n");
		}
		prein=dup(0);
   		fd[0] = open(in,O_RDONLY, 0644);
		dup2(fd[0],0);
	}
	if(f2==1||f3==1)
	{
		preout=dup(1);
		if(f2==1)
	    	fd[1] = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else
	    	fd[1] = open(out, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if(dup2(fd[1],1)==-1)
		{
			perror("dup2 failed");
			return 0;
		}
	}
	pid_t pid;
	pid=fork();
	if(pid==-1)
	{
		close(fd[1]);
		perror("fork error : ");
		return 0;
	}
	else if(!pid)
	{
		if(execvp(args[0],args)==-1)
		{
			perror("command not found");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		wait(NULL);
		dup2(prein,0);
		dup2(preout,1);
		// exit(EXIT_FAILURE);
	}

}
