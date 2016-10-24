#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define CENTRAL_MAILBOX 3539565100
#define CENTRAL_MAILBOX2 3539565200    //Central Mailbox number 
#define NUM_PROCESSES 4           //Total number of external processes
#define MAX_THREADS 2
#define HEADER_SIZE 30

//Declare function
void * stabilise(void * args);

struct mailbox{
	long priority;         //message priority
	int temp;             //temperature
	int pid;                //process id
	int stable;            //boolean for temperature stability
}; 

struct thread{
	int temp;
	int mailboxNumber;
	char header[HEADER_SIZE];
};

//MAIN function
int main(int argc, char *argv[]) {

    int returnStatus = 0;
    pthread_t * threadsArray[MAX_THREADS];
    struct thread * thread1 = NULL;
    struct thread * thread2 = NULL;

   	//Validate that a temperature was given via the command line
   	if(argc != 5) {
      		printf("USAGE: Too few arguments --./central.out Temp");
      		exit(0);
   	}

    //Setup structs
    thread1 = malloc(sizeof(struct thread));
    thread2 = malloc(sizeof(struct thread));
    threadsArray[0] = malloc(sizeof (pthread_t));
    threadsArray[1] = malloc(sizeof (pthread_t));

    // set up variables
    thread1->temp = atoi(argv[1]);
    if(strcmp(argv[3],"1200") == 0)
	{
		thread1->mailboxNumber = CENTRAL_MAILBOX;
	}
    strcpy(thread1->header, "[THREAD 1]");

    thread2->temp = atoi(argv[2]);
    if(strcmp(argv[4],"1300") == 0)
    {
	 thread2->mailboxNumber = CENTRAL_MAILBOX2;
    }
    strcpy(thread2->header, "[THREAD 2]");


   	printf("\nStarting Server\n");

    // Creates two threads
    //Thread 1 (handles first 4 process')
    returnStatus = pthread_create(threadsArray[0], NULL, stabilise, (void *) thread1);
    if(returnStatus != 0)
    {
        fprintf(stderr, "ERROR: Thread_1 failed to initialise\n");
	//Free up memory
        free(thread1);
        free(thread2);
        free(threadsArray[0]);
        free(threadsArray[1]);
        exit(0);
    }

    //Thread 2 (Handles second set of process' (5-8))
    returnStatus = pthread_create(threadsArray[1], NULL, stabilise, (void *) thread2);
    if(returnStatus != 0)
    {
        fprintf(stderr, "ERROR: Thread_2 failed to initialise\n");
        free(thread1);
        free(thread2);
        free(threadsArray[0]);
        free(threadsArray[1]);
        exit(0);
    }

    //Wait threads finish
    pthread_join(*threadsArray[0], NULL);
    pthread_join(*threadsArray[1], NULL);
    //Free up memory, Final free up
    free(thread1);
    free(thread2);
    free(threadsArray[0]);
    free(threadsArray[1]);

    printf("ShuttingDownProgram..\n");

 	return 0;
}

void * stabilise(void * args) {
	struct timeval t1, t2;
    	double elapsedTime;
	struct mailbox msgp;
	struct mailbox cmbox;

   	// start timer
   	gettimeofday(&t1, NULL);

   	struct thread * arguments = (struct thread *) args;

   	//Set up local variables
   	int i,result,length,status;             //counter for loops
   	int uid = 0;                               //central process ID
   	int initTemp = 0;        //starting temperature
   	int msqid[NUM_PROCESSES];       //mailbox IDs for all processes
   	int unstable = 1;          //boolean to denote temp stability
  	int tempAry[NUM_PROCESSES];   //array of process temperatures
	
	int mailboxKey = 0;
	char processHeader[50] = {0};
	initTemp = arguments->temp;
	mailboxKey = arguments->mailboxNumber;
	strcpy(processHeader, arguments->header);

   	//Create the Central Mailbox
   	printf("%sthread Key %d",processHeader,mailboxKey);
   	int msqidC = msgget(mailboxKey, 0600 | IPC_CREAT);

   	//Create the mailboxes for the other processes and store their IDs
   	for(i = 1; i <= NUM_PROCESSES; i++){
      		msqid[(i-1)] = msgget((mailboxKey + i), 0600 | IPC_CREAT);
   	}

  	//Initialize the message to be sent
  	msgp.priority = 1;
   	msgp.pid = uid;
   	msgp.temp = initTemp;
   	msgp.stable = 1;

   	/* The length is essentially the size of the structure minus sizeof(mtype) */
   	length = sizeof(msgp) - sizeof(long);

   	//While the processes have different temperatures
   	while(unstable == 1){
      		int sumTemp = 0;        //sum up the temps as we loop
      		int stable = 1;            //stability trap

      		// Get new messages from the processes
      		for(i = 0; i < NUM_PROCESSES; i++){
         			result = msgrcv( msqidC, &cmbox, length, 1, 0);

         			/* If any of the new temps are different from the old temps then we are still unstable. Set the 
				new temp to the corresponding process ID in the array */
         			if(tempAry[(cmbox.pid - 1)] != cmbox.temp) {
            				stable = 0;
            				tempAry[(cmbox.pid - 1)] = cmbox.temp;
        		 	}

         			//Add up all the temps as we go for the temperature algorithm
         			sumTemp += cmbox.temp;
      		}

      		/*When all the processes have the same temp twice: 1) Break the loop 2) Set the messages stable field 
		to stable*/
      		if(stable){
         			printf("%s Temperature Stabilized: %d\n",processHeader, msgp.temp);
         			unstable = 0;
         			msgp.stable = 0;
      		}
      		else { //Calculate a new temp and set the temp field in the message
         			int newTemp = (msgp.temp + 1000*sumTemp) / (1000*NUM_PROCESSES + 1);
         			usleep(100000);
				msgp.temp = newTemp;
				printf("%s The new temp in central is %d\n",processHeader, newTemp);
      		}

      		/* Send a new message to all processes to inform of new temp or stability */
     		for(i = 0; i < NUM_PROCESSES; i++){
         			result = msgsnd( msqid[i], &msgp, length, 0);
     	 	}
   	}

   	printf("\n%sShutting down Server...\n",processHeader);

   	//Remove the mailbox
   	status = msgctl(msqidC, IPC_RMID, 0);

   	//Validate nothing when wrong when trying to remove mailbox
  	 if(status != 0){
      		printf("\nERROR closing mailbox\n");
   	}

  	// stop timer
   	gettimeofday(&t2, NULL);

    	// compute and print the elapsed time in millisec
    	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
	printf("%s The elapsed time is %fms\n",processHeader, elapsedTime);

	pthread_exit(NULL);
}


