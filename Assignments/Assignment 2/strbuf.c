#include <stdlib.h>
#include <stdio.h>
#include "strbuf.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int sb_init(strbuf_t *L, size_t length)
{
    L->data = malloc(sizeof(int) * length);
    if (!L->data) return 1;

    L->length = length;
    L->used   = 0;

    return 0;
}

void sb_destroy(strbuf_t *L)
{
    free(L->data);
}


int sb_append(strbuf_t *L, char item)
{
    if (L->used == L->length) {
	size_t size = L->length * 2;
	char *p = realloc(L->data, sizeof(int) * size);
	if (!p) return 1;

	L->data = p;
	L->length = size;

	if (DEBUG) printf("Increased size to %lu\n", size);
    }

    L->data[L->used] = item;
    ++L->used;

    return 0;
}


int sb_remove(strbuf_t *L, char *item)
{
    if (L->used == 0) return 1;

    --L->used;

    if (item) *item = L->data[L->used];

    return 1;

}

int sb_insert(strbuf_t *list, int index, char item) {

	int i;
	int length;
	length = sizeof(list);
	if(index > length) {
		if((sizeof(list->data)*2) > (sizeof(list->data)+index)){
			char *p =(char *)realloc(list, 2 * sizeof(int));
			list->data = p;
			return sb_append(list, item);
		} else {
			char *p = (char *)realloc(list, (sizeof(int) + index));
			list->data = p;
			return sb_append(list, item);
		}
	}
	for(i = length - 1; i >= index; i--) {
		list->data[i] = list->data[i - 1];
	}
	list->data[index - 1] = item;
	/*print updated array*/
	for(i = 0; i < length; i++) {
		printf("%c", list->data[i]);
		printf("\n");
	}
	return 0;
}

int sb_concat(strbuf_t *sb, char *str){
	int i;
        int length;
        length = sizeof(sb);
        if(index > length) {
                if((sizeof(sb->data)*2) > (sizeof(sb->data)+index)){
                        cahr *p =(char *)realloc(sb, 2 * sizeof(int));
                        sb->data = p;
                        return sb_append(sb, item);
                } else {
                        char *p = (char *)realloc(sb, (sizeof(int) + index));
                        sb->data = p;
                        return sb_append(sb, item);
                }
        }
        for(i = 0; i <= length; i++) {
                sb->data[i] = sb->data[i + 1];
        }
	sb->data[i] = str;
	if(*str[length] = '\0') {	
		return 0;
	} else {
		return 1;
	}
}

/* Going to be honest, I did not really know how to do this assignment.
 * Hoping I can get some partial credit for attempting it.
 * Sorry for wasteing your time. Hopefully I can learn and understand
 * how this is done in the near future. */
