#include <stdio.h>
#include <string.h>
#include <pthread.h>

struct node {
    int data;
    int key;
    struct node *next;
    struct node *prev;
};

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

    pthread_t FileRead_thread;
    pthread_t CPUScheduler_thread;
    pthread_t IOSystem_thread;

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

    //Get number of lines from input file
    fp = fopen(argv[6], "r");
    if(fp == NULL) {
        printf("Error: Cannot open file\n");
        return 1;
    }

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

    fp = fopen(argv[6], "r");
    if(fp == NULL) {
        printf("Error: Cannot open file\n");
        return 1;
    }
    //Read each line of input file
    while(fgets(line[i], 256, fp) != NULL) {
        line[i][strlen(line[i]) - 1] = '\0';
        // printf("line[%d]: %s\n", i, line[i]);
        token = strtok(line[i], " ");
        int j = 0;
        while(token != NULL) {
            printf("token[%d]: %s\n", j, token);
            token = strtok(NULL, " ");
            j++;
        }
        i++;
    }
    
    return 0;
}