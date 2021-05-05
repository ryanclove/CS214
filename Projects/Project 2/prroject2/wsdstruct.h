#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct wsd{
   char* data;
   char* filename;
   struct wsd* next;
   double frequency;
   int quant;
}wsd;

char* lower(char* a){
  for(int i = 0; a[i]; i++){
      a[i] = tolower(a[i]);
   }
   return a;
}


int check(char* a, wsd* ref){
   wsd* ptr = ref;
   while(ptr != NULL){
      if(ptr->data){
        if(strcmp(ptr->data,lower(a)) == 0){
           ptr->quant++;
           return 1;
        }
      }
        ptr = ptr->next;
   }
   return 0;
}
int length(wsd* ref){
   wsd* temp = ref;
   int length = 0;
   while(temp !=NULL){
      length += temp->quant;
      temp = temp->next;
   }
   return length;
}
void set_frequencies(wsd* ref){

   wsd* ptr = ref;
   while(ptr != NULL){
      if(ptr->data){
         double q = (double)length(ref);
         double freq = (double)(ptr->quant/q);
         ptr->frequency = freq;
      }
        ptr = ptr->next;
   }

}
void set_fname(wsd* ref, char* f){
   ref->filename = f;
}
 void filter(char *s) {
  for (char *p = s; *p; ++p)
    if (isalpha(*p)|| isdigit(*p))
      *s++ = *p;
  *s = '\0';
}
 
void insert_list(char* a,wsd* ref){
   wsd* temp = (wsd*)malloc(sizeof(struct wsd));
   filter(a);
  
   if(a == NULL){
      return;
   }
   temp->quant++;
   temp->data = a;
   wsd* ptr = ref;
   if(check(a,ref)==1){
      return;
   }
   while(ptr->next != NULL){
      ptr = ptr->next;
   }
   ptr->next = temp;
   ptr->next->next = NULL;
   
}

void delete_list(int n,wsd* ref){
   wsd* temp = ref;
   if(n==0){
      ref = temp->next;
      free(temp);
      return;
   }
   for(int i = 0; i<n-1;i++){
      temp = temp->next;
   }
   wsd* temp2 = temp->next;
   temp->next = temp2->next;
   
   free(temp2);
}
wsd* print_list(wsd*ref){
   set_frequencies(ref);
  
   wsd* temp = ref;
   wsd* print = temp;
   printf("Filename :%s\n",temp->filename);
   while(temp !=NULL){
      if(temp->data){
         printf("Word: %s", temp->data);
         putchar('|');
         printf(" Quanitity: %d", temp->quant);
         putchar('|');
       printf(" Freq: %lf", temp->frequency);
       putchar('\n');
         
      }
      temp = temp->next;
   }
   return print;
}
void reverse(wsd* ref){
   wsd* temp = ref;
   wsd* prev = NULL;
   wsd*  next;
   while(temp!=NULL){
      next = temp->next;
      temp->next = prev;
      prev = temp;
      temp = next;
   }
   ref = prev;
}
void insert_all(char* a,int quant,double freq,wsd* ref){
   wsd* temp = (wsd*)malloc(sizeof(struct wsd));

   temp->quant = quant;
   temp->data = a;
   temp->frequency = freq;
   wsd* ptr = ref;
   if(check(a,ref)==1){
      return;
   }
   if(ptr == NULL){
      ref = temp;
      return;
   }
   while(ptr->next != NULL){
      ptr = ptr->next;
   }
   ptr->next = temp;
   ptr->next->next = NULL;
   
}

int size_list(wsd* ref){
   wsd* ptr = ref;
   int size = 0;
   while(ptr != NULL){
      if(ptr->data){
         size++;
      }
        ptr = ptr->next;
   }
   return size;
}

void search_list(char* word,wsd*list,wsd*in_list){
   wsd* ptr = list;
   while(ptr != NULL){
      if(ptr->data != NULL){
         if(strcmp(ptr->data,word) == 0){
         
            insert_all(ptr->data,ptr->quant,ptr->frequency,in_list);
            return;
         }
      }
        ptr = ptr->next;
   }
   return;
}
wsd* sort(wsd* list){
  if(list == NULL){
     return NULL;
  }
  int asize = size_list(list);
 
  char** array = malloc(sizeof(char*)*asize);
  int i = 0;
  char* filename = list->filename;
  
  wsd* ptr = list->next;
  while(ptr != NULL && i<asize){
     array[i] = ptr->data;
     i++;
     ptr = ptr->next;
  }
  //char* temp;
  for(int z =0;z<asize;z++)
      for(int g =z+1; g<asize;g++){
         if (strcmp(array[z], array[g]) > 0) {
            char* temp = array[z]; 
            array[z] = array[g]; 
            array[g] = temp; 
         }
      }
  
  wsd* new_list = (wsd*)malloc(sizeof(struct wsd));
  new_list->filename = filename;
  for(int f = 0; f<asize; f++){
    search_list(array[f],list,new_list);
  }
  return new_list;
}

wsd* get_list(wsd* ref){
   set_frequencies(ref);
   return ref;
}

