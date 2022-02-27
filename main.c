#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//Representing keywords as 01(proc), 02(sleep), 03(stop)
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
}

void insert(struct node *list, int data) {
    struct node *currentnode = list;
    struct node *nextnode;
    struct node *prevnode;
    struct node *newnode;

    prevnode = currentnode->prev;
    nextnode = currentnode->next;
    if(currentnode->next == NULL) {
        newnode = malloc(sizeof(struct node));
        newnode->data = data;
        currentnode->next = newnode;
        newnode->next = NULL;
    }
    printf("currentnode = %d\n", data);
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
void *fileRead(struct node *list, char *filename) {
    FILE *fp;
    int i = 0;
    int data;
    int counter = 0;
    char *token;
    char lineBuffer;
    char line[256][256];

    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Error: Cannot open file\n");
        exit(1);
    }

    //Get number of lines from input file
    while(1) {
        lineBuffer = fgetc(fp);

        if(lineBuffer == '\n') {
            counter++;
            printf("counter = %d\n", counter);
        }

        if(lineBuffer == EOF) {
            printf("EOF reached\n");
            fclose(fp);
            break;
        }
    }

    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Error: Cannot open file\n");
        exit(1);
    }
    while(fgets(line[i], 256, fp) != NULL) {
        line[i][strlen(line[i]) - 1] = '\0';
        // printf("line[%d]: %s\n", i, line[i]);

        //Read file into doubly linked list
        //Check keyword
        token = strtok(line[i], " ");
        if(strcmp(token, "proc") == 0) {
            //Create new process
            printf("proc found!\n");
            insert(list, 01);
            while(strcmp(token, "\n") != 0) {
                token = strtok(NULL, " ");
                data = atoi(token);
                insert(list, data);
            }
        }
        else if(strcmp(token, "sleep") == 0) {
            printf("sleep found!\n");
            insert(list, 02);
            while(strcmp(token, "\n") != 0) {
                token = strtok(NULL, " ");
                data = atoi(token);
                insert(list, data);
            }
        }
        else if(strcmp(token, "stop") == 0) {
            printf("stop found!\n");
            insert(list, 03);
            while(strcmp(token, "\n") != 0) {
                token = strtok(NULL, " ");
                data = atoi(token);
                insert(list, data);
            }
        }
        else {
            printf("Error: Unknown keyword\n");
            exit(1);
        }

        int j = 0;
        while(token != NULL) {
            printf("token[%d]: %s\n", j, token);
            token = strtok(NULL, " ");
            j++;
        }
        i++;
    }
}

void cpuScheduler(int schedulingAlgorithm) {
  //FCFS = 0
  //SJF = 1
  //PR = 2
  //RR = 3

  //while still processes
    //if ready queue has process
      //pick process
      //sleep for burst time
      //put process in IO queue
}

void ioSystem() {
  //while still processes
    //if IO queue has process
      //sleep for IO burst time
      //put process in ready queue
}

void handleThreads(struct node *list, char *filename) {
    pthread_t FileRead_thread;
    pthread_t CPUScheduler_thread;
    pthread_t IOSystem_thread;

    if((pthread_create(&FileRead_thread, NULL, fileRead(list, filename), NULL)) != 0) {
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
            printf("argv[%d] = %s\n", i, argv[i]);
        }
    }

    struct node *list = init();

    //Call function to create threads based on provided command line argument
    handleThreads(list, argv[6]);

    return 0;
}
