#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#ifndef WWPATH
#define WWPATH "/ilab/users/rmc326/Systems Programming/Project 1/ww"
#endif
 
int isdirect(char *name) {
	    struct stat data;
	    int err = stat(name, &data);
	    if(err!=0) {
	       perror(name);
	       return 2;
	    }
    	      return S_ISDIR(data.st_mode);
}

int main(int argc, char* argv[]){
	char* i = argv[1];
	for(int j = 2; j < argc; j++){
	   int fd[2]; // fd[0] -read, fd[1] - write
	   if (pipe(fd) == -1) {
		printf("An error ocurred opening the pipe \n");
		return 1;
	    }
	    int id = fork();
	    if (id < 0) {
		fprintf(stderr, "Fork Failed");
		return 1;
	    }
	
	    if (id == 0 && isdirect(argv[j]) == 0) {  //child process
		    close(fd[0]);
		    write(fd[1], argv[j], sizeof(int));
		    execl(WWPATH, WWPATH, i, argv[j], NULL);
		    close(fd[1]);
		    wait(NULL);
	
	    } else {
		    close(fd[1]);
		    read(fd[0], argv[j] , sizeof(int));
		    close(fd[0]);
		    wait(NULL);
		    fprintf(stdout, "\n");
	    }

	}
	return 0;
}
