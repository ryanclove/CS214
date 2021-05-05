#include <stdlib.h>
#include <stdio.h>
#include "arraylist.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int al_init(arraylist_t *L, size_t length)
{
    L->data = malloc(sizeof(int) * length);
    if (!L->data) return 1;

    L->length = length;
    L->used   = 0;

    return 0;
}

void al_destroy(arraylist_t *L)
{
    free(L->data);
}


int al_append(arraylist_t *L, int item)
{
    if (L->used == L->length) {
	size_t size = L->length * 2;
	int *p = realloc(L->data, sizeof(int) * size);
	if (!p) return 1;

	L->data = p;
	L->length = size;

	if (DEBUG) printf("Increased size to %lu\n", size);
    }

    L->data[L->used] = item;
    ++L->used;

    return 0;
}


int al_remove(arraylist_t *L, int *item)
{
    if (L->used == 0) return 1;

    --L->used;

    if (item) *item = L->data[L->used];

    return 1;
}

int al_insert(arraylist_t *list, int index, int item) {

        int i;
        int length;
        length = sizeof(list);
        if(index > length) {
		if((sizeof(list->data)*2) > (sizeof(list->data)+index)) {
			int *p = (int * )realloc(list, 2 * sizeof(int));
			list->data = p;
			return al_append(list, item);
		} else {
			int *p = (int *)realloc(list, (sizeof(int) + index));
			list->data = p;
			return al_append(list, item);
		}
	}
	for(i = length - 1; i >= index; i--) {
                list->data[i] = list->data[i - 1];
        }
        list->data[index - 1] = item;
        /*print updated array*/
        for(i = 0; i < length; i++)
                printf("%d ", list->data[i]);
        printf("\n");
	
	return 0;
}
