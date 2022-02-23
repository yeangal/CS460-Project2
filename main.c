#include <stdio.h>
#include <string.h>
#include <pthread.h>

struct node {
    int data;
    int key;
    struct node *next;
    struct node *prev;
};

char fileRead(char filename) {
    FILE *fp;
    int i = 0;
    int counter = 0;
    char token;
    char lineBuffer;
    char line[256][256];

    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Error: Cannot open file\n");
        return 1;
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
        return 1;
    }
    while(fgets(line[i], 256, fp) != NULL) {
        line[i][strlen(line[i]) - 1] = '\0';
        // printf("line[%d]: %s\n", i, line[i]);
        
        //Check keyword
        token = strtok(line[i], " ");
        if(strcmp(token, "proc") == 0) {
            //Create new process
            
        }
        else if(strcmp(token, "sleep") == 0) {

        }
        else if(strcmp(token, "stop") == 0) {

        }
        else {
            printf("Error: Unknown keyword\n");
            return 1;
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

void cpuScheduler() {

}

void ioSystem() {

}

void handleThreads(char filename) {
    pthread_t FileRead_thread;
    pthread_t CPUScheduler_thread;
    pthread_t IOSystem_thread;

    if((pthread_create(&FileRead_thread, NULL, fileRead(filename), NULL)) != 0) {
        printf("Failed to create thread: FileRead_thread\n");
        return 1;
    }
    if((pthread_create(&CPUScheduler_thread, NULL, cpuScheduler, NULL)) != 0) {
        printf("Failed to create thread: CPUScheduler_thread\n");
        return 1;
    }
    if((pthread_create(&IOSystem_thread, NULL, ioSystem, NULL)) != 0) {
        printf("Failed to create thread: IOSystem_thread\n");
        return 1;
    }

    //Join threads
    if(pthread_join(FileRead_thread, NULL) != 0) {
        printf("Failed to join thread: FileRead_thread\n");
    }
    if(pthread_join(CPUScheduler_thread, NULL) != 0) {
        printf("Failed to join thread: CPUScheduler_thread\n");
    }
    if(pthread_join(IOSystem_thread, NULL) != 0) {
        printf("Failed to join thread: IOSystem_thread\n");
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
    FILE *fp;
    int i = 0;
    int counter = 0;
    char *token;
    char lineBuffer;
    char line[256][256];

    struct node *head = NULL;
    struct node *tail = NULL;
    struct node *current = NULL;

    //Checking arguments
    if(argc > 7 || argc < 7) {
        print_usage();
        return 1;
    }
    else {
        for(i = 0; i < argc; i++) {
            printf("argv[%d] = %s\n", i, argv[i]);
        }
    }
    
    handleThreads(argv[6]);
    
    return 0;
}