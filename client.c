#include "helper.h"
#include <math.h>

//Exponential distribution function : f(x;λ) = λ * e ^(-λx).X is the number of request. 
int Exp(int lambda,int X) {
    if ( !X)                   //First request of each client should be happening instantly
        return(0);
    return (lambda * exp(X*(-lambda)));
}

//Updates files array with new file if it isn't already retrieved and value of number acccordingly
void update_files(int files[],int* number ,int requested) {

    for(int i = 0; i < *number; i++) {
        if(files[i] == requested)
            return;
    }
    files[*number] = requested;
    *number = *number + 1;
}

void child(int lines,int K,int L,int lambda,int process_num, sem_t* clients,sem_t* server,sem_t* process_sem,sem_t* thread_sem,request* request_mem) {

    unsigned int curtime = time(NULL);      
    srand((unsigned int) (curtime + getpid()));
    //Open output file for given process
    char output_file[] = "output_";
    char str_num[20];
    sprintf(str_num,"%d",process_num);
    strcat(output_file,str_num);
    strcat(output_file,".txt");    
    FILE*  output = fopen(output_file,"w");    
    //Number of lines the client has retrieved until the end of all of his requests
    int returned_lines = 0;
    //Number of unique files from where information was drawn
    int files_num = 0;
    //Array with unique files we have retrieved
    int files[L];
    long long total_delay = 0;
    //Total time between consecutive requests
    long long time_cons = 0;
    long long prev_submit;
    fprintf(output,"---REQUESTS---");

    for(int i = 0; i < L; i++) {

        //Pick randomly a file number , a beginning line and the number of lines in the passage
        int start = rand() % (lines - 1) + 1;
        int wanted = rand() % (lines - start) + 1;
        int ending = start + wanted - 1;
        int file_num = rand() % K + 1;
        //Update metrics
        returned_lines += wanted;
        update_files(files,&files_num,file_num);

        //Create shared memory where the request is gonna be copied
        int passage_id = shmget(IPC_PRIVATE,BLOCK_SIZE *sizeof(char),0666);
        if (passage_id == -1)
            perror_exit("Creation of passage shared memory object failed!");

        //Attach the shared memory to client
        char* passage = (char*) shmat(passage_id, (void*)0, 0);
        if (passage == (void*)-1) 
            perror_exit("Attachment of passage failed in client!");         

        //Time between submitting the requests follows exponential distribution
        sleep(Exp(lambda,i));

        //Exclude other clients' access to the shared memory object while we update its value
        sem_wait(clients);
       
        //Create request
        request_mem->file_num = file_num;
        request_mem->start = start;
        request_mem->end = ending;
        request_mem->shm_id = passage_id;      
        request_mem->process_sem = process_sem;
        request_mem->thread_sem = thread_sem;      
        //If this is client's last request, server has received all requests of this client 
        if(i == L-1)
            request_mem->served++;
        //Keep track of the submitting time
        long long submit = timeInMilliseconds();
        //Update total time between consecutive requests
        if( i > 0 )
            time_cons += submit - prev_submit;
        prev_submit = submit;
        //Submit request to server    
        sem_post(server);

        //Keep track of requests in output file    
        fprintf(output,"\n%d - %d from %d\n",start,ending,file_num);
        fflush(output);
        //Repeat until thread serves all wanted lines
        for(int j = 0; j <= wanted; j++) {            
            //Wait for response from thread
            sem_wait(process_sem);           
            //Save current line in output file
            fprintf(output,"%s",passage);
            fflush(output);
            //Thread can continue its execution now that we have consumed the line
            sem_post(thread_sem);            
        }

        //Keep track of delays
        long long response = timeInMilliseconds();
        total_delay += response-submit;

        //Wait until thread is done with shared memory to delete it
        sem_wait(process_sem); 

        // Detach passage shared memory object
        if(shmdt((void*)passage) == -1)
            perror_exit("Failed to detach passage in client!");

        // Delete passage shared memory object
        if ( shmctl(passage_id , IPC_RMID , 0) == -1 ) 
            perror_exit("Failed to delete passage!"); 
    }
    //Save statistics in output folder
    long long avg_time = time_cons / L ;              //Average time between consecutive requests
    fprintf(output,"\n---METRICS---\n");
    fprintf(output,
            "Total lines returned : %d.\nTotal files retrieved : %d.\nTotal delay between submitting and receiving requests : %lld msec.\nAverage time between consequtive requests: %lld msec.\n",
            returned_lines,files_num,total_delay,avg_time);
    fclose(output);
    exit(0);
}
