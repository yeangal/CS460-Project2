#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

//Representing keywords as -1(proc), -2(sleep), -3(stop)
struct node {
    int data;
    struct node *next;
    struct node *prev;
};

struct node *init() {
    struct node *new;
    new = malloc(sizeof(struct node));
    new->next = NULL;
    new->prev = NULL;
    return new;
};

//globals
pthread_mutex_t readyQLock;
pthread_mutex_t IOQLock;
struct node *readyQ;
struct node *ioQ;
struct node *head;

void print(struct node *list) {
    struct node *currentnode = list;
    struct node *nextnode = currentnode->next;

    while(currentnode->next != NULL) {
        printf("%d ", nextnode->data);
        currentnode = nextnode;
        nextnode = nextnode->next;
    }
}

void insert(struct node *list, int data) {
    // struct node *currentnode = list;

    // if(currentnode == NULL) {
    //     printf("Error: Unable to allocate memory\n");
    //     exit(1);
    // }

    // if(list == NULL) {
    //     currentnode->next = NULL;
    //     currentnode->prev = NULL;
    //     head = currentnode;
    // }
}

void pull(struct node *process){
  //remove node from list
}
/*
Called from the threadHandler function in response to the creation of the FileRead_thread
Reads the filename passed, gets the number of lines for possible later usage.
Parses each line into tokens... need to store each token into an array, ex:
token[0] = proc, token[1] = priority level, token[2] = num of bursts, token[3..'\n'] = burst times
token[0] = sleep, token[1] = sleep time
token[0] = stop
Don't know if this is the right way to do it but it's my thought
*/
void *fileRead(struct node *readyQ, char *filename) {
    FILE *fp;
    int i = 0;
    int data;
    int counter = 0;
    char *token;
    char *lastToken;
    char lineBuffer;
    char line[256][256];

    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Error: Cannot open file\n");
        exit(1);
    }

    //Get number of lines from input file
    // while(1) {
    //     lineBuffer = fgetc(fp);

    //     if(lineBuffer == '\n') {
    //         counter++;
    //         // printf("counter = %d\n", counter);
    //     }

    //     if(lineBuffer == EOF) {
    //         printf("EOF reached\n");
    //         fclose(fp);
    //         break;
    //     }
    // }

    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Error: Cannot open file\n");
        exit(1);
    }

    //Loop to store each line of file into 2D array
    while(fgets(line[i], 256, fp) != NULL) {
        line[i][strlen(line[i])] = '\0';
        printf("line[%d]: %s\n", i, line[i]);
        i++;
    }
    fclose(fp);

    //Loop to tokenize each line and call functions accordingly
    while(counter < i) {
        token = strtok(line[counter], " ");
        printf("token[%d]: %s\n", counter, token);
        if(strcmp(token, "proc") == 0) {
            printf("proc found!\n");
            insert(readyQ, -1);
            while(token != NULL) {
                token = strtok(NULL, " ");
                if(token == NULL) {
                    break;
                }
                data = atoi(token);
                insert(readyQ, data);
                printf("insert(readyQ, %d)\n", data);
            }
        }
        else if(strcmp(token, "sleep") == 0) {
            printf("sleep found!\n");
            insert(readyQ, -2);
            while(token != NULL) {
                token = strtok(NULL, " ");
                if(token == NULL) {
                    break;
                }
                data = atoi(token);
                insert(readyQ, data);
                printf("insert(readyQ, %d)\n", data);
            }
        }
        else if(strcmp(token, "stop") == 0) {
            printf("stop found!\n");
            insert(readyQ, -3);
            while(token != NULL) {
                token = strtok(NULL, " ");
                if(token == NULL) {
                    break;
                }
                data = atoi(token);
                insert(readyQ, data);
                printf("insert(readyQ, %d)\n", data);
            }
        }
        else {
            printf("Error: Unknown keyword\n");
            exit(1);
        }
        printf("Incrementing counter: %d\n", counter);
        counter++;
    }
}

void cpuScheduler(int schedulingAlgorithm) {
  int burstTime;
  struct node *process;

  while(readyQ != NULL) {
    while(pthread_mutex_lock(&readyQLock) != 0){}//wait for queue access
    //pick process
      //FCFS
      if(schedulingAlgorithm == 0) {
        //get first in q
        //process = readyQ head
      }

      //SJF
      else if(schedulingAlgorithm == 1) {
        //get shortest time left in q
        while(process->next != NULL) {
          //if process->next burstTime is shorter than process burstTime
            //process = process->next
        }
      }

      //PR
      else if(schedulingAlgorithm == 2) {
        //get highest priority in q
        while(process->next != NULL) {
          //if process->next priority is shorter than process priority
            //process = process->next
        }
      }

      //RR
      else if(schedulingAlgorithm == 3) {

      }

      //pull process from readyQ
      //pull(process)
      sleep(burstTime);
      //put process in IO queue
      while(pthread_mutex_lock(&IOQLock) != 0){}//wait for queue access
      //ioqInsert(process)
  }

}

void ioSystem() {
  //while still processes
    while(readyQ != NULL) {
        while(pthread_mutex_lock(&readyQLock) != 0){}//wait for queue access
    //get IO burst time
    //sleep for IO burst time
    //put process in ready queue
    }
}

void handleThreads(struct node *readyQ, char *filename) {
    pthread_t FileRead_thread;
    pthread_t CPUScheduler_thread;
    pthread_t IOSystem_thread;

    //init mutexes
  	pthread_mutex_init(&readyQLock, NULL);
  	pthread_mutex_init(&IOQLock, NULL);

    if((pthread_create(&FileRead_thread, NULL, fileRead(readyQ, filename), NULL)) != 0) {
        printf("Failed to create thread: FileRead_thread\n");
        exit(1);
    }
    // if((pthread_create(&CPUScheduler_thread, NULL, cpuScheduler, NULL)) != 0) {
    //     printf("Failed to create thread: CPUScheduler_thread\n");
    //     exit(1);
    // }
    // if((pthread_create(&IOSystem_thread, NULL, ioSystem, NULL)) != 0) {
    //     printf("Failed to create thread: IOSystem_thread\n");
    //     exit(1);
    // }

    //Join threads
    if(pthread_join(FileRead_thread, NULL) != 0) {
        printf("Failed to join thread: FileRead_thread\n");
        exit(1);
    }
    if(pthread_join(CPUScheduler_thread, NULL) != 0) {
        printf("Failed to join thread: CPUScheduler_thread\n");
        exit(1);
    }
    if(pthread_join(IOSystem_thread, NULL) != 0) {
        printf("Failed to join thread: IOSystem_thread\n");
        exit(1);
    }

    pthread_mutex_destroy(&readyQLock);
    pthread_mutex_destroy(&IOQLock);

}

void print_usage() {
    printf("Usage: prog -alg [FIFO|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]\n");
    printf("---CPU Scheduling Algorithms---\n");
    printf("-FIFO: First Come First Serve\n");
    printf("-SJF: Shortest Job First\n");
    printf("-PR: Priority\n");
    printf("-RR: Round Robin\n");
}

int main(int argc, char *argv[]) {
    //Checking arguments
    if(argc > 7 || argc < 7) {
        print_usage();
        return 1;
    }
    else {
        for(int i = 0; i < argc; i++) {
            // printf("argv[%d] = %s\n", i, argv[i]);
        }
    }

    readyQ = init();
    ioQ = init();

    //Call function to create threads based on provided command line argument
    handleThreads(readyQ, argv[6]);

    return 0;
}
