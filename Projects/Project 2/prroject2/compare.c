#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <regex.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include "wsdstruct.h"
#include <pthread.h>
#include "kld_list.h"

#include <math.h>

#define BUFF_LENGTH (256)
#define BYTES_READ (256)
#define DEBUG (1)


const char* delims = " \n\t\v\f\r";
char* prefix;
typedef struct counter_t {
  int value;
  pthread_mutex_t lock;
}count_dir_thread;

count_dir_thread* counter_dir;

void init_counter() {
  counter_dir->value = 0;
  pthread_mutex_init(&counter_dir->lock, NULL);
}
void increment() {
  pthread_mutex_lock(&counter_dir->lock);
  counter_dir->value += 1;
  pthread_mutex_unlock(&counter_dir->lock);
}
void decrement(){
    pthread_mutex_lock(&counter_dir->lock);
    counter_dir->value -= 1;
    pthread_mutex_unlock(&counter_dir->lock);
}

void debug(char* msg, char* msg2){
    if(DEBUG == 0){
        printf("message: %s , %s\n",msg ,msg2);
    }
}
typedef struct jsd
{
    char* f1;
    char* f2;
    double jsd;
    int combined_count;
}jsd;
typedef struct wsd_repo{
   wsd* list;
   struct wsd_repo* next;
   pthread_mutex_t lock;

}node;

node* main_ptr;

void edit_list(node* list){
    main_ptr = list;
}
void insert_lists(wsd* a){
  
   pthread_mutex_lock(&main_ptr->lock);
   debug("lock",""); 
   node* temp = (node*)malloc(sizeof(struct wsd_repo));
   temp->list = a;
   if(main_ptr!=NULL){ 
      temp->next =main_ptr;
   }
   main_ptr = temp;
   pthread_mutex_unlock(&main_ptr->lock);
   debug("unlock",""); 
}
void print_lists(){
   node* temp = main_ptr;
   while(temp !=NULL){
      if(temp->list != NULL){
        print_list(temp->list);

      }
      temp = temp->next;
   }
}
int print_repo_length(){
   node* temp = main_ptr;
   int size = 0;
   while(temp !=NULL){
      if(temp->list != NULL){
        get_list(temp->list);
        size++;
      }
      
      temp = temp->next;
   }
   return size;
}

void rev(){
   node* temp = main_ptr;
   node* prev = NULL;
   node*  next;
   while(temp!=NULL){
      next = temp->next;
      temp->next = prev;
      prev = temp;
      temp = next;
   }
   main_ptr = prev;
}

void* get_wsd_file(void* f){
    char* buff;
    debug("processing file", (char*) f);
    int fd;
    int size = BYTES_READ; 
    fd = open((char*)f,O_RDONLY);
    if(fd == -1){
        perror("flie_cant");
        return NULL;
    }
    buff  = (char*)malloc(sizeof(char)*size); 
    int bytes_read = read(fd,buff,size);
    int str = bytes_read;
    if(bytes_read ==str && fd !=0){
		while(bytes_read ==  str){
			close(fd);
			size = size*2;
			fd = open((char*)f,O_RDONLY);
			read(fd,buff,size);
			bytes_read = read(fd,buff,size);
			str = strlen(buff); 
		}
	}
   // printf(" bytes read : %d\n", bytes_read);
    char* tokens;
    tokens = strtok(buff,delims);
    wsd* list = (wsd*)malloc(sizeof(wsd));
    
    set_fname(list,f);
    close(fd);
    
    while(tokens != NULL){
        insert_list(tokens,list);
        tokens = strtok(NULL, delims);
    }
    
    //reverse(list);
  //  list = sort(list);
    insert_lists(list);
    buff = NULL;
    debug("done proccessing file", f);
    return NULL; 
}
// end of wsd_repo code

#ifndef QSIZE
#define QSIZE 200
#endif

typedef struct queue_t{
	char* data[QSIZE];
	unsigned count;
	unsigned head;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} queue_t;

int init(queue_t *Q)
{
	Q->count = 0;
	Q->head = 0;
	pthread_mutex_init(&Q->lock, NULL);
	pthread_cond_init(&Q->read_ready, NULL);
	pthread_cond_init(&Q->write_ready, NULL);
	
	return 0;
}

int destroy(queue_t *Q)
{
	pthread_mutex_destroy(&Q->lock);
	pthread_cond_destroy(&Q->read_ready);
	pthread_cond_destroy(&Q->write_ready);

	return 0;
}


int enqueue(queue_t *Q,  char* item)
{
   
	pthread_mutex_lock(&Q->lock);
	
	while (Q->count == QSIZE) {
		pthread_cond_wait(&Q->write_ready, &Q->lock);
	}
	
	unsigned i = Q->head + Q->count;
	if (i >= QSIZE) i -= QSIZE;
	
	Q->data[i] = item;
	++Q->count;
    
	
	pthread_cond_signal(&Q->read_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return 0;
}


char* dequeue(queue_t *Q)
{
	pthread_mutex_lock(&Q->lock);
	
	while (Q->count == 0) {
		pthread_cond_wait(&Q->read_ready, &Q->lock);
	}
	

  
    char* item = Q->data[Q->head];
    
	--Q->count;
	++Q->head;
	if (Q->head == QSIZE) Q->head = 0;
	
	pthread_cond_signal(&Q->write_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return item;
}

int is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

queue_t* queue_file;
queue_t* queue_dir;



void listFilesDir(char *basePath,queue_t* dir_q,queue_t* file_queue)
{
    struct dirent *dp;
    
    
    increment();
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            char path[100];
            char* pathptr = path;
            strcat(pathptr,basePath);
            strcat(pathptr, "/");
            strcat(pathptr, dp->d_name);
            debug("dir or file",pathptr);
            if(is_dir(pathptr)!=0 ){
                //debug("path",pathptr);
                char* pathptrn = (char*)malloc(sizeof(char)*strlen(pathptr));
                pathptrn = (char*)malloc(sizeof(char)*strlen(pathptr));
                
                strcpy(pathptrn,pathptr);
               // debug("path_new",pathptrn);
                enqueue(dir_q,pathptrn);
               
            }else{
               if(strstr(dp->d_name,prefix)){
               
                    char* pathptrn = (char*)malloc(sizeof(char)*strlen(pathptr));
                    pathptrn = (char*)malloc(sizeof(char)*strlen(pathptr));

                    strcpy(pathptrn,pathptr);
                 // debug("path_new",pathptrn);
                    enqueue(file_queue,pathptrn);
                 
               }
            }
            strcpy(pathptr,"");
        }
        
    }
     

    decrement();
    closedir(dir);
} 

// add item to end of queue

void dir_thread_process(void* ptr){
    
    char* basepath = dequeue(queue_dir);
    listFilesDir(basepath,queue_dir,queue_file);
    
}
int all_done = 0;
pthread_mutex_t lock_file_thread;
void* file_thread_process2(void*ptr){
    pthread_mutex_lock(&lock_file_thread);
    while(all_done != 1){
        if(counter_dir->value == 0 && queue_dir->count == 0 && queue_file->count == 0 ){
            all_done = 1;
            break;
        }
        debug("dequeing ...","");
        char* name = dequeue(queue_file);
        if(strcmp(name,"")!=0){
            get_wsd_file(name);
        }
        
    }
    pthread_mutex_unlock(&lock_file_thread);
    debug("end of processing file","***************************");
    printf("counter :%d\n", counter_dir->value);
    return NULL;
}

typedef struct arg_s{
    queue_t* file;
    queue_t* dir;
}arg_s;
void* dir_queue(void* args){
    
    debug("processing","dir");
    while(1){
        
        dir_thread_process(NULL);
    }
}

jsd jsd_calc(wsd* f1,wsd* f2){
 
   wsd* ptrf1 = sort(f1);
   wsd* ptrf2 = sort(f2);
   ptrf1 = ptrf1->next;
   ptrf2 = ptrf2->next;
   prefix = ".txt";
   
   kld_list* kld = (kld_list*)malloc(sizeof(struct kld_list));
   struct jsd values;
  while(ptrf2 != NULL && ptrf1 != NULL){
       if(strcmp(ptrf1->data,ptrf2->data)== 0){
           
           insert_kld(kld,ptrf1->data,(ptrf1->frequency+ptrf2->frequency)/2);
           ptrf1 = ptrf1->next;
           ptrf2 = ptrf2->next;
       }else if(strcmp(ptrf1->data,ptrf2->data)<0){
            insert_kld(kld,ptrf1->data,(ptrf1->frequency)/2);
            ptrf1 = ptrf1->next;
       }else{
           insert_kld(kld,ptrf2->data,(ptrf2->frequency)/2);
            ptrf2 = ptrf2->next;
       }
       
   } 
   
   if(ptrf1 != NULL){
       while(ptrf1 != NULL){
            insert_kld(kld,ptrf1->data,(ptrf1->frequency)/2);
            ptrf1 = ptrf1->next;
       }
   }
   if(ptrf2 != NULL){
       while(ptrf2!=NULL){
            insert_kld(kld,ptrf2->data,(ptrf2->frequency)/2);
            ptrf2 = ptrf2->next;
       }
   }
  double kld1 = 0;
  ptrf1 = f1->next;
  while(ptrf1!=NULL){
      if(search_kld(kld,ptrf1->data) == 1){
        
          double in_log = ptrf1->frequency/get_f_kld(kld,ptrf1->data);
          double val = (ptrf1->frequency)*(log(in_log)/log(2));
         
          kld1 += val;
        
      }
      ptrf1 = ptrf1->next;
  }
 
  double kld2 = 0;
  ptrf2 = f2->next;

  
  while(ptrf2!=NULL){
      if(search_kld(kld,ptrf2->data) == 1){
          
          double in_log = ptrf2->frequency/get_f_kld(kld,ptrf2->data);
          double val = (ptrf2->frequency)*(log(in_log)/log(2));
          kld2 += val;
      }
      ptrf2 = ptrf2->next;
  }
  
  kld1 = kld1/2;
  
  kld2 = kld2/2;
  
  double jsd = sqrt(kld1+kld2);
  values.f1 = f1->filename; 
  values.f2 = f2->filename;
  values.jsd = jsd;
  values.combined_count = length(f1)+length(f2);
  
  return values;
}
jsd* values;
node* repo;
int size;
void* get_jsd(void* ptr){
    node* ptr1 = repo;
    node* ptr2 = ptr1->next;
    int comparisons = 0;
    jsd* val = values;
    int size = size;
    while(ptr1 != NULL){
         ptr2 = ptr1->next;
        while(ptr2 !=  NULL){
            if(ptr1->list != NULL && ptr2->list != NULL && comparisons < size){
                val[comparisons] = jsd_calc(ptr1->list,ptr2->list);
                comparisons++;
            }
            ptr2 = ptr2->next;
        }
        ptr1 = ptr1->next;
    }
    printf("comp %d\n",comparisons);
    return NULL;
}


typedef struct a_thread{
    jsd* val;
    int size;
    node* repo;
}a_thread;
int main(int argc,char* argv[]){

 queue_file = (queue_t*)malloc(sizeof(struct queue_t));
 queue_dir  = (queue_t*)malloc(sizeof(struct queue_t));
   repo = (node*)malloc(sizeof(struct wsd_repo));
  pthread_mutex_init(&repo->lock, NULL);
  prefix = ".txt";
  int dir_threads = 1;
  int file_threads = 1;
  edit_list(repo);
  init(queue_file);
  init(queue_dir);
 
       int index = 1;
       while(argv[index]!= NULL){
            
            if(is_dir(argv[index])){
                enqueue(queue_dir,argv[index]);
            }
            if(is_file(argv[index]) && argv[index][0]!='-'){
                enqueue(queue_file,argv[index]);
            }
            if(strstr(argv[index],"-d")){
                if(strlen(argv[index]) != 2 ){
                    int threads = 0;
                    char* arg = argv[index];
                    if(isdigit(arg[2]) == 0){
                        return EXIT_FAILURE;
                    }
                    threads = arg[2]-'0';
                    dir_threads = threads;
                    printf("file_threads %d\n",threads);
                }else{
                    perror("no argument for dir_thread");
                    return EXIT_FAILURE;
                }
            }
            if(strstr(argv[index],"-f")){
                if(strlen(argv[index]) != 2 ){
                    int threads = 0;
                    char* arg = argv[index];
                    threads = arg[2]-'0';
                    file_threads = threads;
                    //printf("dir_threads %d\n",threads);
                }else{
                    perror("no argument for file_thread");
                    return EXIT_FAILURE;
                }
            }
            if(strstr(argv[index],"-s")){
                char suffix[10];
                int add = 0;
                int start = 2;

                if(argv[index][start] == '.'){
                while(argv[index][start]!= '\0'){
                     suffix[add] = argv[index][start];
                     add++;
                     start++;
                }
                    char* pass_suff = (char*)malloc(sizeof(char)*add);
                    for(int i = 0; i< add; i++){
                        pass_suff[i] = suffix[i];
                        }
                    prefix = pass_suff;
                }else{
                    perror(" no good suffix");
                    return EXIT_FAILURE;
                }
            }
           index++;
       }
       

       //exit(0);
    
     
     
     arg_s* ar = malloc(sizeof(struct arg_s));
     
     ar->dir = queue_dir;
     ar->file = queue_file;
     counter_dir = (count_dir_thread*)malloc(sizeof(struct counter_t));
     init_counter();
     
    pthread_t* d_threads = (pthread_t*)malloc(sizeof(pthread_t)*dir_threads);
    for(int i = 0; i<dir_threads; i++){
        pthread_create(&d_threads[i],NULL,dir_queue,(void*)ar);
    }
    debug("created dir threads ","");
    
    pthread_t* f_threads = (pthread_t*)malloc(sizeof(pthread_t)*file_threads);
    

    for(int n = 0; n<file_threads; n++){
        pthread_create(&f_threads[n],NULL,file_thread_process2,NULL);
    }
     for(int j = 0; j<file_threads;j++){
        pthread_join(f_threads[j],NULL);
    }
   
    debug("create and joined file threads ","");  
    print_lists();
    repo = main_ptr;
    
    values= (jsd*)malloc(sizeof(struct jsd)*size);
    
    size  = (print_repo_length())*((print_repo_length()-1));
    size = size/2;
    
    debug("___________________","______________________");
    
    pthread_t a_thread;
    pthread_create(&a_thread,NULL,get_jsd,NULL);
    pthread_join(a_thread,NULL);
    for(int s = 0; s< size; s++){
        for(int h = s+1; h<size; h++){
            if(values[s].combined_count < values[h].combined_count ){
                jsd temp = values[s];
                values[s] = values[h];
                values[h] = temp;
            }
        }
    }
    for(int i = 0; i<size; i++){
        printf("%f",values[i].jsd);
        printf("%s"," ");
        printf("%s",values[i].f1);
        printf("%s"," ");
        printf("%s",values[i].f2);
        printf("%c",'\n'); 
    }
    
   return EXIT_SUCCESS;
}