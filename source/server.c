#include "../headers/helper.h"

void child(int ,int ,int ,int ,int ,sem_t*,sem_t* ,sem_t*,sem_t* ,request* ) ;
void* process_request(void* );

int main (int argc, char* argv[]) {

    if (argc < 5)
        perror_exit("Not enough command line arguments!Try again!");
    
    int N = atoi(argv[1]);
    int L = atoi(argv[2]);
    int K = atoi(argv[3]);  
    int lambda = atoi(argv[4]);
    //Number of lines in each file
    int lines = count_lines();  

    //Create the shared memory for requests
    int request_id = shmget(IPC_PRIVATE,sizeof(request),0666);
    if (request_id == -1) 
        perror_exit("Creation of request shared memory object failed!");
    
    //Attach the shared memory to server
    request* request_mem = (request*) shmat(request_id, (void*)0, 0);
    if (request_mem == (void*)-1) 
        perror_exit("Attachment of request shared memory object failed!");
    request_mem->served = 0;

    //Create named semaphores to protect the request shared memory object
    sem_t* server = sem_open("/server", O_CREAT, SEM_PERMS, 0);
    if(server == SEM_FAILED)
        perror_exit("Server sem_open failed!"); 

    sem_t* clients = sem_open("/clients", O_CREAT, SEM_PERMS, 1);
    if(clients == SEM_FAILED)
        perror_exit("Clients sem_open failed!");  

    //Initialize a named semaphore for each child process and for the thread that will serve its request for IPC
	sem_t** process = malloc(N*sizeof(sem_t*));
	sem_t** thread = malloc(N*sizeof(sem_t*));
    char** process_name = malloc(N*sizeof(char*));
    char** thread_name = malloc(N*sizeof(char*));

    for(int i = 0; i < N; i++) {

        process_name[i] = attach_num("/Process",i);
        thread_name[i] = attach_num( "/Thread",i);        

        if((process[i] = sem_open(process_name[i], O_CREAT, SEM_PERMS, 0)) == SEM_FAILED)
            perror_exit("Sem_open for child process failed!"); 

        if((thread[i] = sem_open(thread_name[i], O_CREAT, SEM_PERMS, 0)) == SEM_FAILED)
            perror_exit("Sem_open for thread failed!");                    
    }

    //Create N clients - child processes
    pid_t pids[N];
    pthread_t t_ids[N*L];
    int thread_num = 0;

    for(int i = 0; i < N; i++) {

        if((pids[i] = fork()) < 0 )         
            perror_exit("Fork failed!");
        else if (pids[i] == 0 )                 //Child Process
            child(lines,K,L,lambda,i+1,clients,server,process[i],thread[i],request_mem);
    }

    //Repeat until you have served all the clients
    while( request_mem->served < N ) {
        //Wait until you get a request from a client
        sem_wait(server);      
        //Copy the shared memory's information into a new pointer
        request* thread_arg = request_t(request_mem);   
        //Create thread
        pthread_t new_thread;
        if((t_ids[thread_num++] = pthread_create(&new_thread, NULL, process_request , (void*) thread_arg))) 
            perror_exit("Error in server's pthread_create"); 
        //Current request is read we can move on to reading the next one => allow next client to write on shared memory         
        sem_post(clients);
    }  

    int status;
    //Collect children that have finished
    for(int i = 0; i < N; i++)
        wait(&status);
        
    //Make sure threads have exited
    for(int i = 0; i < N*L; i++)
        pthread_join(t_ids[i],NULL);

    // Detach request shared memory object
    if(shmdt((void*)request_mem) == -1)
       perror_exit("Failed to detach request shared memory object!");

    // Delete request shared memory object
    if ( shmctl(request_id , IPC_RMID , 0) == -1 ) 
        perror_exit("Failed to delete request shared memory object!");

    //Close and unlink all the named semaphores
    if(sem_close(clients) < 0)
        perror_exit("Clients sem_close failed!");

    if(sem_unlink("/clients") < 0)
        perror_exit("Clients sem_unlink failed!");

    if(sem_close(server) < 0)
        perror_exit("Server sem_close failed!");

    if(sem_unlink("/server") < 0)
        perror_exit("Server sem_unlink failed!");    

    for(int i = 0; i < N; i++) {

        if(sem_close(process[i]) < 0)
            perror_exit("Sem_close failed!");  

        if(sem_unlink(process_name[i]) < 0)
            perror_exit("Sem_unlink for process_name failed!");        

        if(sem_close(thread[i]) < 0)
            perror_exit("Sem_close failed!");  

        if(sem_unlink(thread_name[i]) < 0) 
            perror_exit("Sem_unlink for thread_name failed!");                

        free(process_name[i]);
        free(thread_name[i]);
    }
    //Deallocate memory
    free(process_name);
    free(thread_name);    
    free(process);
    free(thread);
    return 0;
}

void* process_request(void* arg) {         

    request* request_t = (request*) arg;
    //Attach child's shared memory to parent
    char* passage = (char*) shmat(request_t->shm_id, (void*)0, 0);
    if (passage == (void*)-1) 
        perror_exit((char*)"Attachment of passage failed in server!");

    //Open wanted file given file number
    char* file_name = attach_num("SERVER_FILES/file_",request_t->file_num);
    strcat(file_name,".txt");
    FILE*  file = fopen(file_name,"r");
    char * copy = NULL;
    size_t len = 0;

    //Read until you reach wanted section
    for (int i = 1; i < request_t->start;  i++)           
        getline(&copy,&len,file);

    for(int i = request_t->start-1; i <= request_t->end; i++) {
        //Notify client line is ready to be consumed
        sem_post(request_t->process_sem);         
        //Wait until client notifies that it has consumed the line to move on to the next one
        sem_wait(request_t->thread_sem);                  
        getline(&copy,&len,file);
        strcpy(passage,copy);                       
    }   
    free(file_name);     
    free(copy);
    fclose(file);  
    // Detach passage shared memory object since we are done with copying
    if(shmdt((void*)passage) == -1)
        perror_exit((char*)"Failed to detach passage in server!");    

    //Request is processed => deallocate the memory
    free(request_t);
    //Notify client that the shared memory can be detached and deleted
    sem_post(request_t->process_sem);
    //Exit thread
    pthread_exit(NULL); 
}