#ifndef STANDARD_H
#define STANDARD_H
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
#endif

#ifndef STRBUF_H
#define STRBUF_H
#include "strbuf.h"
#endif

typedef struct LL
{
   strbuf_t *key;
   strbuf_t *value;
   struct LL *next;
}LLNode;

typedef struct LL* LLptr;

LLptr LLNodeInit(LLptr,char*,char*);
void LLPrint(LLptr);
int LLLength(LLptr);
LLptr SelectionSort(LLptr);
void FreeLL(LLptr );
