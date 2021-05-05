#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct kld_list {
    char* word;
    double freq;
    struct kld_list* next;
}kld_list;

void insert_kld(kld_list* list, char* word , double freq){
    kld_list* temp  = (kld_list*)malloc(sizeof(struct kld_list));
    kld_list* ptr = list;
    temp->word = word;
    temp->freq = freq;
    while(ptr->next != NULL){
        ptr= ptr->next;
    } 
    ptr->next = temp;
    ptr->next->next = NULL;
}
void print_kld_list(kld_list* list){
    kld_list* ptr = list;
    while(ptr != NULL){
        if(ptr->word != NULL){
             printf("word: %s",ptr->word);
            putchar('|');
            printf("freq: %f",ptr->freq);
            putchar('\n');
        }
        ptr=ptr->next;
    }

}

int search_kld(kld_list* list,char* word){
    kld_list* ptr = list;
    while(ptr != NULL){
        if(ptr->word != NULL){
           if(strcmp(word,ptr->word)==0){
               return 1;
           }
        }
        ptr=ptr->next;
    }
    return 0;
}
double get_f_kld(kld_list* list,char* word){
    kld_list* ptr = list;
    while(ptr != NULL){
        if(ptr->word != NULL){
           if(strcmp(word,ptr->word)==0){
               return ptr->freq;
           }
        }
        ptr=ptr->next;
    }
    return 1.0;
}