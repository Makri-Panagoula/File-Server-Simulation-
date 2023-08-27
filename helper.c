#include "helper.h"

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

//Count number of lines in the first file (all of the files in the system must have the same number of lines)
int count_lines() {
    FILE*  file = fopen("SERVER_FILES/file_1.txt","r");
    char c ;
    int lines = 1;
    // Extract characters from file and store in character c
    for (c = getc(file); c != EOF; c = getc(file))
        if (c == '\n') // Increment count if this character is newline
            lines = lines + 1;
    fclose(file);    
    return lines;
}

//Returns current time in milliseconds.
long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

char* attach_num(char* string,int num) {

    char* to_return = malloc(50 * sizeof(char));
    strcpy(to_return ,string);
    char str_num[20];
    sprintf(str_num,"%d",num);
    strcat(to_return,str_num);    
    return to_return;
}
//Allocates memory and copies the current request
request* request_t(request* cur) {

    request* new_request = (request*) malloc(sizeof(request));
    new_request->file_num = cur->file_num;
    new_request->start = cur->start;
    new_request->end = cur->end;
    new_request->shm_id = cur->shm_id;
    new_request->served = cur->served;
    new_request->process_sem = cur->process_sem;
    new_request->thread_sem = cur->thread_sem;
    return new_request;
}