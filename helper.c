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

char* attach_num(char* string,int num) {
    
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