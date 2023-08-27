#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <pthread.h> 
#include <errno.h> 
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>        
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/wait.h>
#include<sys/time.h>
#pragma once

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define BLOCK_SIZE 1024

typedef struct{
    //Selected file
    int file_num;
    //Beginning line in selected file
    int start;
    //Ending line in selected file    
    int end;
    //Id of client-created shared memory where server should write the requested lines
    int shm_id;
    //Number of child processes that have terminated
    int served;
    //Semaphores for IPC between the client-process and the serving thread
    sem_t* process_sem;
    sem_t* thread_sem;    
} request;

void perror_exit(char *message);

//Count number of lines in the first file (all of the files in the system must have the same number of lines)
int count_lines() ;

//Allocates memory and copies the current request
request* request_t(request* cur) ;

//Returns current time in milliseconds.
long long timeInMilliseconds(void) ;

//Allocates memory for a string that concatenates string with num
char* attach_num(char* string,int num);
