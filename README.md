General admissions made:
The command line arguments must be given in the following order : N (number of client-processes) , L (number of requests per client process) , K (number of files accessible to the file server) and lamda (parameter for the exponentially distributed time algorithm).The client submits its request in the following format : file number,beginning line, ending line along with a shared memory id with the size of a line(block).We return the ending line in the request as well.For checking accuracy in requests and viewing their metrics, we create an output folder for every child process in the format output_i.txt , i in [1,N], containing this information. Requests in this file are stored in the format : Beginning Line - Ending Line from File Number and at the end of the file the metrics can be found.The metrics include total lines returned from server,unique files retrieved, total delay between submitting and getting response to a request and average time between consecutive requests.The system files from where the information will be retrieved must be stored in the format file_i.txt , i in [1,K].For simplicity, we work with 10 files each having 100 lines,but a different number of files or lines wouldn't make any difference.The maximum line size (aka BLOCK_SIZE) is arbitrarily chosen in 1024 ,which is a big enough number for every reasonable sentence.Each of the K files must have the same number of lines.

Synchronizing & Shared Memory:
In order to ensure mutual exclusion in the request object shared memory and proper communication between parent(reader) and children(writers), we use the named semaphores : "server" initialized in 0 and "clients" initialized in 1. We,also, need to synchronize every process (reader in shared memory) with its responding serving thread(writer in shared memory) , therefore we create two semaphore arrays (of size N) : "process" and "thread" initialized in 0.The semaphores corresponing to each process are passed as arguments in child function and they are saved in the request struct so that the thread can access them as well.The thread serves only one line at a time until client has all the requested ones. The server merely reads the requests and copies this information to a pointer transferring it to the thread which will do all the actual work.
All semaphores are initialized in parent process (server.c) before the fork.

Disclaimer : 4 is the minimum grade I need in the assignment to pass the class, so please keep that in mind :).