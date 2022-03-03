#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//Representing keywords as -1(proc), -2(sleep), -3(stop)
struct node {
    int data;
    struct node *next;
    struct node *prev;
};

struct stats {
    clock_t start;
    clock_t lastReady;
    int totalWait;
    struct stats *next;
};

struct node *init() {
    struct node *new;
    new = malloc(sizeof(struct node));
    new->next = NULL;
    new->prev = NULL;
    new->data = -4;
    return new;
};

//globals
pthread_mutex_t readyQLock;
pthread_mutex_t IOQLock;
pthread_mutex_t statLock;
struct node *readyQ;
struct node *ioQ;
struct stats *timerList;
int schedulingAlgorithm;
int quantum = 0;

void print(struct node *list) {
    struct node *currentnode = list;
    struct node *nextnode = currentnode->next;

    while(currentnode->next != NULL) {
        printf("currentnode->data: %d\n", currentnode->data);
        // printf("currentnode->next->data: %d\n", nextnode->data);
        currentnode = nextnode;
        nextnode = currentnode->next;
    }
}

void insert(struct node *list, int data) {
    struct node *currentnode = list;
    struct node *newnode;

    //If list is empty, prev node will be NULL
    if(currentnode->data == -4) {
      currentnode->data = data;
      currentnode->prev = NULL;
      currentnode->next = NULL;
      return;
    }

    while(1) {
        if(data == 0) {
            break;
        }
        if(currentnode->next == NULL) {
            printf("Inserting %d\n", data);
            newnode = malloc(sizeof(struct node));
            newnode->data = data;
            currentnode->next = newnode;
            newnode->prev = currentnode;
            newnode->next = NULL;
            break;
        }
        else {
            currentnode = currentnode->next;
        }
    }
}

void delete(struct node *process) {
    struct node *currentnode = process;

    //If only element left in list
    if(currentnode->prev == NULL && currentnode->next == NULL) {
        free(currentnode);
    } //If last element in list
    else if(currentnode->next == NULL) {
        currentnode->prev->next = NULL;
        free(currentnode);
    } //If first element in list
    else if(currentnode->prev == NULL) {
        currentnode->next->prev = NULL;
        free(currentnode);
    }
    else {
        currentnode->next->prev = currentnode->prev;
        currentnode->prev->next = currentnode->next;
        free(currentnode);
    }
}

void pull(struct node *process, int flag) {
    //remove node from list
    struct node *firstNode = process;
    struct node *lastNode = process->next;

    int listHead = 0, listTail = 0;

    if(firstNode->prev == NULL) {//if process is first in list
      listHead = 1;
    }

    while(lastNode->data > 0) {//find the first node of the next command
      if(lastNode->next == NULL) {//if process is last in list
        listTail = 1;
        break;
      }
      lastNode = lastNode->next;
    }

    if(listTail && listHead) {
      if(flag) {readyQ = NULL;}
      else {ioQ = NULL;}
    }
    else if(listTail && !listHead) {
      firstNode->prev->next = NULL;
      firstNode->prev = NULL;
    }
    else if(!listTail && listHead) {
      lastNode->prev->next = NULL;
      lastNode->prev = NULL;
      if(flag) {readyQ = lastNode;}
      else {ioQ = lastNode;}
    }
    else {//process is in middle of list
      lastNode->prev->next = NULL;
      lastNode->prev = firstNode->prev;
      firstNode->prev->next = lastNode;
      firstNode->prev = NULL;
    }

}

void put(struct node *process, struct node *list) {
  //if list is empty
  if(list == NULL) {
    list = process;
    process->prev = NULL;
    return;
  }

  //else normal insert
  struct node *temp = list;
  while(temp->prev != NULL) {
    temp = temp->prev;
  }
    temp->prev = process;
    temp->prev->next = temp;
    temp->prev->prev = NULL;
}

void insertStat(clock_t time) {
  if(timerList == NULL) {
    timerList == malloc(sizeof(struct stats));
    timerList->start = time;
    timerList->lastReady = time;
    timerList->totalWait = 0;
    timerList->next = NULL;
  }else {
    struct stats *temp = timerList;
    while(temp->next != NULL){
      temp = temp->next;
    }
    temp->next == malloc(sizeof(struct stats));
    temp->next->start = time;
    temp->next->lastReady = time;
    temp->next->totalWait = 0;
    temp->next->next = NULL;
  }
}

void test(struct node *list) {
    struct node *currentnode = list;

    while(currentnode != NULL) {
        if(currentnode->data == -1) {
            printf("Pulling...\n");
            pull(currentnode, 1);
            break;
        }
        else {
            currentnode = currentnode->next;
        }
    }
    print(readyQ);
}

void *fileRead(struct node *readyQ, char *filename) {
    FILE *fp;
    int i = 0;
    int data;
    int counter = 0;
    int timerCounter;
    char *token;
    char *lastToken;
    char lineBuffer;
    char line[256][256];
    clock_t first_t;

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

    int procCounter = -1;
    //Loop to tokenize each line and call functions accordingly
    while(counter < i) {
        token = strtok(line[counter], " ");
        printf("token[%d]: %s\n", counter, token);

        if(strcmp(token, "proc") == 0) {
            printf("proc found!\n");
            while(pthread_mutex_lock(&readyQLock) != 0) {}
            insert(readyQ, procCounter);
            procCounter--;
            timerCounter = 0;
            while(token != NULL) {
                if(timerCounter == 1) {
                    first_t = clock();
                    insertStat(first_t);
                    token = strtok(NULL, " ");
                }
                timerCounter++;
                token = strtok(NULL, " ");
                if(token == NULL) {
                    break;
                }
                data = atoi(token);
                insert(readyQ, data);
                printf("insert(readyQ, %d)\n", data);
            }
            if(pthread_mutex_unlock(&readyQLock) == 0) {
                printf("Successfuly unlocked readyQ\n");
            }
        }
        else if(strcmp(token, "sleep") == 0) {
            printf("sleep found!\n");
            while(token != NULL) {
                token = strtok(NULL, " ");
                if(token == NULL) {
                    break;
                }
                data = atoi(token);
                fflush(stdout);
                sleep(data);
            }
        }
        else if(strcmp(token, "stop\n") == 0) {
            printf("stop found!\n");
            printf("Exiting thread FileRead_thread\n");
            pthread_exit(NULL);
        }
        else {
            // print(readyQ);
            printf("Error: Unknown keyword\n");
            // printf("Calling Test()\n");
            // test(readyQ);
            exit(1);
        }
        counter++;
        // printf("Incrementing counter: %d\n", counter);
    }
}

void cpuScheduler() {
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
  	pthread_mutex_init(&statLock, NULL);

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
    pthread_mutex_destroy(&statLock);

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
    int filenameLoc = 4;

    if(argc > 7 || argc < 5) {
        print_usage();
        return 1;
    }

    if(strcmp(argv[2], "RR") == 0) {
        int time = atoi(argv[4]);
        if(!time > 0) {
            printf("Quantum time must be greater than zero\n");
            return 1;
        }
        quantum = time;
        schedulingAlgorithm = 3;
        filenameLoc = 6;
    }
    else {
        if(argc != 5) {
            print_usage();
            return 1;
        }
        else if(strcmp(argv[2], "PR") == 0) {
            schedulingAlgorithm = 2;
        }
        else if(strcmp(argv[2], "SJF") == 0) {
            schedulingAlgorithm = 1;
        }
        else if(strcmp(argv[2], "FIFO") == 0) {
            schedulingAlgorithm = 0;
        }
    }

    readyQ = init();
    ioQ = init();
    timerList = NULL;
    
    handleThreads(readyQ, argv[filenameLoc]);

    return 0;
}
