# File-Server-Simulation

Our goal in this assignment is to develop a system of processes to simulate the functionality of a file server such that the client processes will randomly choose a file out of the available ones in the system and demand a certain number of lines and upon the completion of all of its requests metrics regarding the system's service are presented.
## General Admissions

- Each of the N clients submits L requests.
- FS creates a thread for every request it receives.
- The thread returns only one line at a time until client has all the requested ones.
- The server merely reads the requests and creates a thread which will do all the actual work.
- The submission of the request to the FS is done through a shared memory segment which is created once in the beginning and deleted only upon completion of the program.
- Before submitting the request the client process creates another temporary shared memory segment through which it will get the wanted passage.This space is fixed and equal to the maximum size of a line.This segment is deleted upon completion of request by client.
 - Between the submission of each client's request there is an exponentially distributed time given a parameter λ.
- The K available files have the same number of lines and are saved in the SERVER_FILES directory in the format file_i.txt , i in [1,K] . 
- Each output_i file for every child process i stores every request information in the format:
  Beginning Line - Ending Line from File Number . At the end of the file the total lines returned from server,unique files retrieved, total delay between submitting and getting response to a request and average time between consecutive requests are stored.
- N,K,L,λ are command line arguments.

## Synchronizing & Shared Memory
 To ensure mutual exclusion we use the named semaphores : "server" ,initialized in 0 and "clients" ,initialized in 1. We,also, need to synchronize every child process (reader in shared memory) with its responding serving thread(writer in shared memory) , therefore we create two named semaphore arrays (of size N) : "process" and "thread" initialized in 0. 
All semaphores are initialized in parent process (server.c) before the fork.

## Usage
Run with make run. Arguments must be provided in the order; N L K λ
