#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <ctype.h>


char* args[2][1024];
int firstTimeOpen = 1;

void dirCommand(){
	struct dirent *de;
	DIR *dr;
	dr = opendir("./");
	if(dr == NULL){
		printf("file could not be open");
	}
	while((de = readdir(dr)) != NULL){
		printf("%s	",de->d_name);
	}
	printf("\n");
	closedir(dr);
}

void uniqCommand(char* s){
	char* file;
	char* command;
	char* cmd = strtok(s," ");
	char* argument = strtok(NULL," ");
	int iArg = 0;
	int uArg = 0;
	int dArg = 0;
	if(strcmp(argument,"-i") == 0){ iArg = 1;}
	if(strcmp(argument,"-d") == 0){ dArg = 1;}
	if(strcmp(argument,"-u") == 0){ uArg = 1;}
	printf("%d",iArg);
	if(iArg == 0 && dArg == 0 && uArg == 0){
		FILE *fp;
		fp=fopen(argument,"r");
		if(argument == NULL) printf("null value in argument");
		printf("the text file from which to uniq is:%s\n",argument);
		char x[1024] = "";
		char prev[1024] = "";
		while(fgets(x,sizeof(x),fp) != 0){
		if(strcmp(x,prev) != 0)
	 		printf("%s",x);
			strcpy(prev,x);
	}
	fclose(fp);
	}
	if(iArg == 1){
		char* file = strtok(NULL," ");
		printf("file is: %s",file);
		FILE *fp;
		int i = 0;
		fp = fopen(file,"r");
		char x[1024] = "";
                char prev[1024] = "";
                while(fgets(x,sizeof(x),fp) != 0){
		while(x[i]){
			x[i] = tolower(x[i]);
			i++;
		}
		i = 0;
                if(strcmp(x,prev) != 0)
                        printf("%s",x);
                        strcpy(prev,x);
		}

	}
	if(dArg == 1){
		char* file = strtok(NULL," ");
        	printf("file is: %s",file);
        	FILE *fp;
        	int i = 0;
        	fp = fopen(file,"r");
        	char x[1024] = "";
        	char prev[1024] = "";
		while(fgets(x,sizeof(x),fp) != 0){
                if(strcmp(x,prev) == 0)
                        printf("%s",x);
                        strcpy(prev,x);
                }

	}
	if(uArg == 1){
                char* file = strtok(NULL," ");
                printf("file is: %s",file);
                FILE *fp;
                int i = 0;
                fp = fopen(file,"r");
                char x[1024] = "";
                char prev[1024] = "";
		char firstLine[1024]= "";
		int first = 0;
                while(fgets(x,sizeof(x),fp) != 0){
		if(first == 0){
			strcpy(prev,x);
			first = 1;
		}else
                if(strcmp(x,prev) != 0)
                        printf("%s",x);
                        strcpy(prev,x);
                }

        }

}

void rmCommand(char* s){

	struct dirent *d;
	DIR *dir;
        char buf[256];
        dir = opendir("./");
	char* cmd = strtok(s," ");
	char* argument = strtok(NULL," ");
	char* file = strtok(NULL," ");
	int iArg = 0;
        int rArg = 0;
        int vArg = 0;
        if(strcmp(argument,"-i") == 0){ iArg = 1;}
        if(strcmp(argument,"-r") == 0){ rArg = 1;}
        if(strcmp(argument,"-v") == 0){ vArg = 1;}
	if(iArg == 0 && rArg == 0 && vArg == 0){
		remove(argument);
	}
	if(iArg == 1){
		printf("are you sure you want to remove this file?");
		printf("y or n?");
		char a;
		scanf("%c",&a);
		if(a == 'y') remove(file);
		if(a == 'n') printf("the file will not be removed");
		if(a != 'y' && a != 'n') printf("invalid response");
	}
	if(vArg == 1){
                remove(file);
		printf("\n");
		printf("removed '%s'\n",file);
        }

}



void execPipedCommand(char* s,char ***localCommand){
	int p[2];
	pid_t pid;
	int fd_in = 0;
	int i =0;
	while(*localCommand != NULL){
		pipe(p);
		if((pid = fork()) == -1){
			printf("failed to fork");
		}else if(pid == 0){
			dup2(fd_in,0);
			if(*(localCommand+1) != NULL) {
				dup2(p[1],1);
			}
			close(p[0]);
			if(execvp((*localCommand)[0],*localCommand) < 0) printf("failed\n");
		}else{
			wait(NULL);
			close(p[1]);
			fd_in = p[0];
			localCommand++;
		}
	}
	close(p[0]);
	close(p[1]);
}

void formatPipeCommand(char *s){
	int i = -1;
	char *a = "|";
	char *b = " ";
	char* c;
	char* arg;
	char **cmd[1024];
	char* found;
	while((found = strsep(&s,a)) != NULL){
		i++;
		if(found[0] == ' '){
			found++;
		}
		c = strsep(&found,b);
		arg = strsep(&found,b);
		args[i][0] = c;
		args[i][1] = arg;
		args[i][2] = NULL;
		cmd[i] = args[i];
	}

	cmd[i+1] = NULL;
	execPipedCommand(s,cmd);
}

void execCommand(char* s){
	
	char* cmd;
	char* arg;
	char* a = " ";
	cmd = strsep(&s,a);
	arg = strsep(&s,a);
	char* args[2];
	args[0]=(char*) cmd;
	args[1]=(char*) arg;
	args[2]=NULL;
	pid_t p;
	p = fork();
	if(p == 0){
		if(execvp(args[0],args) < 0){
		printf("failed to execute command\n");
		}
		exit(0);
	}else {
		wait(NULL);
		return;
	}
}

int checkRedirect(char* s){
        if(strchr(s,'>') != NULL){
                return 1;
        }else {
                return 0;
        }
}


int checkPipe(char* s){
        if(strchr(s,'|') != NULL){
                return 1;
        }else {
                return 0;
        }
}


int processCommand(char* s){
	char* cat[]={"cat","test.txt",NULL};
	char* grep[]={"grep","contents",NULL};
	char **command[]={cat,grep,NULL};
	int hasPipes;
	hasPipes = checkPipe(s);
	if(hasPipes){
		formatPipeCommand(s);
	}else {
		execCommand(s);
	}
}
char* user;
int input(char *s){
	char *buffer;
	char cwd[1024];
	char* c = ":";
	if(firstTimeOpen){
		user = getenv("USER");
		getcwd(cwd,sizeof(cwd));
		strcat(user,c);
		strcat(user,cwd);
		strcat(user,"$ ");
		firstTimeOpen = 0;
	}
	buffer = readline(user);
	if(strlen(buffer) != 0){
		add_history(buffer);
		strcpy(s,buffer);
		return 0;
	}else {
		return 1;
	}
}

int main(){

	int n;
	char exitCommand[] = "exit";
	char dirCommandText[] = "dir";
	char uniqCommandText[] = "uniq";
	char rmCommandText[] = "rm";
	char line[1024];
	char buffer[1024];
	int i=0;
	int fd;
	int saved = dup(1);
	char* token = "";
	do{
		if(!input(line)){
			strcpy(buffer,line);
			if(checkRedirect(buffer)){
				char buffer2[1024];
				strcpy(buffer2,buffer);
				char* trash = strtok(buffer2,">");
				token = strtok(NULL,">");
				for(int i = 0; i<strlen(token); i++) token[i] = token[i+1];
				printf("%s\n", token);
				fd = open(token, O_WRONLY | O_APPEND | O_CREAT);
				if(fd < 0 ) return 1;
				if(dup2(fd,1) < 0) { close(fd);return 1;}
			}
			i++;
			if(strcmp(buffer,"dir") == 0){
				dirCommand();
			}else if(strcmp(buffer,"uniq") >= 0){
				uniqCommand(buffer);
			}else if(strcmp(buffer,"rm") >= 0){
				rmCommand(buffer);
			}else
			processCommand(buffer);
		}
		close(fd);
		dup2(saved,1);
		close(saved);
	}while(strcmp(exitCommand,line) != 0);
}


