#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "ww.h"


#define BUFFSIZE 2

int wrap(int fd_read, size_t length, int fd_write) {
    char* buff = malloc(BUFFSIZE);
    strbuf_t* currentword = malloc(sizeof(strbuf_t));
    sb_init(currentword, length);
    int num_read = 1;
    int offset = 0;
    int count = 0;
    int newlineflag = 0;
    int started = 0;
    int firstword = 1;
    int whitespaceflag = 0;
    int fail=0;
    while(num_read > 0) {
        // reads char from file into buffer (size macro)
	    num_read = pread(fd_read, buff, BUFFSIZE, offset);
	    offset += num_read;
	    // Iterate buffer
	    for(int i=0; i < num_read; i++) {
            //adds nonspace char to current word strbuf 
            read_word(currentword, buff[i], &started);
            //space character
            if(isspace(buff[i])) {
                //checks for newline
                if(buff[i] == '\n') {
                    newlineflag++;
                }
                else {
                    whitespaceflag++;
                }
                //checks if first word of file
                if(started != 0) {
                    //writes word to output when only encountering 1 space character
                    if((whitespaceflag==1 || newlineflag==1)) {
                        write_word(fd_write, currentword, &count, length, newlineflag, started, firstword, &fail, 0);
                    }
                    firstword = 0;
                    sb_destroy(currentword);
                    sb_init(currentword, length);
                }
            }
            //normal char
            else {
                //checks and writes paragraph breaks to output file
                if(newlineflag >= 2) {
                    write_word(fd_write, currentword, &count, length, newlineflag, started, firstword, &fail, 1);
                }
                newlineflag=0;
                whitespaceflag=0;
            }
        }
    }
    //writes last word to output file and frees memory
    newlineflag = 0;
    write_word(fd_write, currentword, &count, length, newlineflag, started, firstword, &fail, 0);
    write(fd_write, "\n", 1);
    sb_destroy(currentword);
    free(buff);
    free(currentword);
    return fail;
}

strbuf_t* read_word(strbuf_t* currentword, char currentletter, int *started) {
    if(!isspace(currentletter)) {
        *started=1;
        sb_append(currentword, currentletter);
    }
    return currentword;
}

void write_word(int fd_write, strbuf_t* currentword, int *count, size_t limit, int newlineflag, int started, int firstword, int *fail, int elsecall) {    
    int sizewritten = *count;
    int currentsize = currentword->used;
    int added=1;
    //break paragraph
    if(newlineflag >= 2 && elsecall == 1) {
        char parabuf[2];
        parabuf[0] = '\n';    
        parabuf[1] = '\n';
        write(fd_write, parabuf, 2);
        *count = 0;
        return;
    }
    if(sizewritten==0 || firstword==1 || currentword->used==0){
        added=0;
    }
    //word fits on line
    if(sizewritten + currentsize + added <= limit) {
        if(sizewritten!=0 && firstword==0 && currentword->used!=0) {
            //putting prior space
            char tempspace[1];
            tempspace[0] = ' ';
            write(fd_write, tempspace, 1);
        }
        //writing word
        char *tempword = currentword->data;
        write(fd_write, tempword, currentsize);
        *count = sizewritten + currentsize + added;
    }
    //doesn't fit
    else {
        //skip line
        if(newlineflag!=2 && sizewritten!=0 && currentsize!=0) {
            char tempnewline[1];
            tempnewline[0] = '\n';
            write(fd_write, tempnewline, 1);
        }
        // word > line length then flag and own line
        if(currentsize > limit) {
            *fail = 1;
            char *tempword = currentword->data;
            write(fd_write, tempword, currentsize);
            *count=currentsize;
            return;
        }
        //writing word
        char *tempword = currentword->data;
        write(fd_write, tempword, currentsize);
        *count = currentsize;
    }
}

//returns 0 if file, 1 if directory, 2 if file/directory not found
int isdirect(char *name) {
    struct stat data;
    int err=stat(name,&data);
    if(err!=0) {
       perror(name);
       return 2;
    }
    return S_ISDIR(data.st_mode);
}

int main(int argc, char* argv[argc+1]) {
    //exits if 1 or 2 arguments not given
    int num_arg = argc-1;
    if(num_arg != 1 && num_arg != 2) {
        return EXIT_FAILURE;
    }
    //length is not a number or <= to 0
    int line_length = atoi(argv[1]);
    if(line_length <= 0) {
        printf("First argument must be > 0 \n");
        return EXIT_FAILURE;
    }
    //If 1 argument provided read from stdin 
    if(num_arg==1) {
        int fd = 0;
        int returnval = wrap(fd, line_length, 1);
            if(returnval == 1) {
                return EXIT_FAILURE;
            }
    }
    //If 2 arguments provided search files/directory
    else {
        //checks if valid
        int argtype = isdirect(argv[2]);
        //file/direc not found
        if(argtype == 2) {
            return EXIT_FAILURE;
        }
        //normal file
        if(argtype == 0) {
            int file;
            file = open(argv[2], O_RDONLY);
            if(file == -1) {
                perror("Error: ");
                return EXIT_FAILURE;
            }
            int returnval = wrap(file, line_length, 1);
            if(returnval == 1) {
                return EXIT_FAILURE;
            }
            close(file);
        }
        //directory
        else if(argtype == 1) {
            strbuf_t* dir= malloc(sizeof(strbuf_t));
            sb_init(dir, 10);
            sb_concat(dir, argv[2]);
            DIR * directptr = opendir(dir->data);
            if(directptr==NULL) {
                perror("Error: ");
                return EXIT_FAILURE;
            }
            struct dirent *d;
            while ((d = readdir(directptr))) { 
                char type = d->d_type;
                if(type == DT_REG) {
                    //open file
                    char *inputfile = d->d_name;
                    strbuf_t* current_read = malloc(sizeof(strbuf_t));
                    strbuf_t* current_write = malloc(sizeof(strbuf_t));
                    char* addon = "wrap.";
                    sb_init(current_read,10);
                    sb_init(current_write,10);
                    sb_concat(current_write, dir->data);
                    sb_concat(current_read, dir->data);
                    sb_concat(current_write, addon);
                    if(*inputfile == '.' || strstr(inputfile,addon) != NULL) {
                        sb_destroy(current_write);
                        sb_destroy(current_read);
                        free(current_write);
                        free(current_read);
                        continue;
                    }
                    sb_concat(current_write, inputfile);
                    sb_concat(current_read, inputfile);
                    int fd_read = open(current_read->data, O_RDONLY);
                    //create new file
                    int fd_write = open(current_write->data,O_RDWR | O_CREAT | O_TRUNC, 0777);
                    //call write
                    wrap(fd_read, line_length, fd_write);
                    close(fd_read);
                    close(fd_write);
                    sb_destroy(current_write);
                    sb_destroy(current_read);
                    free(current_write);
                    free(current_read);
                }
                //else ignore subdirectories
            }
            sb_destroy(dir);
            free(dir);
            closedir(directptr);
        }
    }
  return EXIT_SUCCESS;
}
