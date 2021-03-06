#ifndef STANDARD_H
#define STANDARD_H
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#endif

#ifndef STRBUF_H
#define STRBUF_H
#include "strbuf.h"
#endif

typedef struct LLNode
{
    strbuf_t *key;
    strbuf_t *value;
    struct LLNode*next;
}LLNode;

struct connection {
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

typedef struct LLNode* LLptr;

LLptr LLNodeInit(char*,char*);
void LLPrint(LLptr);
int LLLength(LLptr);
LLptr SelectionSort(LLptr);
void FreeLL();

void siginthandler(int);
