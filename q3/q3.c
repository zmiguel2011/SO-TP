#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>
#include <signal.h>

#define MAX_PIPE_NAME 26            // Longest pipename, for the biggest N values, for N <= INT_MAX in pipeXtoY, X,Y <= N

typedef enum {false = 0, true = 1} bool;
bool keepAtIt = true;


void sigint_handler() {
    keepAtIt = false;
}

/* 
* Create a single pipe
* @param x is the first number used in the name
* @param n is the total of pipes being created in the program
* @return char* with name of pipe created
*/
const char* makePipe(int x, int n){
    char *result = malloc(sizeof (char) * MAX_PIPE_NAME);
    if(x == n){
        sprintf(result, "pipe%dto%d", x, 1);   
    }
    else{
    sprintf(result, "pipe%dto%d", x, x+1);
    }
    mkfifo(result, 0777);
    return result;
}

/* 
* Create all pipes
* @param n is the total of pipes being created in the program
* @return char*+ array with the name of all pipes created
*/
const char** create_pipes(int n){
    const char **pipeNames;
    pipeNames = malloc(n * sizeof(char*));
    for(int i = 1; i <= n; i++){
        pipeNames[i-1] = makePipe(i, n);
    }
    return pipeNames;
}

int main(int argc, char* argv[]) {
    if (argc <= 3) {
        printf("usage: n (number of processes) p (chances of blocking) t (number of seconds the block takes)\n");
        return EXIT_FAILURE;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("WARNING: cannot install handler for SIGINT\n");
    }

    int n = 0, t = 0;
    float p = 0.0;

    char *pointer;
    int errno = 0;
    long conv = strtol(argv[1], &pointer, 10);

    // Check for errors: e.g., the string does not represent an integer
    // or the integer is larger than INT_MAX or smaller than INT_MIN
    // then saves the number of processes in n
    if (errno != 0 || *pointer != '\0' || conv > INT_MAX || conv < INT_MIN) {
        printf("usage: n (number of processes) p (chances of blocking) t (number of seconds the block takes)\n");
        printf("ERROR: input for n (number of processes) is invalid\n");
        return EXIT_FAILURE;
    } else {
        n = conv;
    }

    errno = 0;
    float conv_f = strtof(argv[2], NULL);

    // Check for errors: e.g., the string does not represent a float
    // or the float is larger than 1 or smaller/equal to 0.0 
    // then saves the chances of blocking in p
    if (errno != 0 || conv_f <= 0.0 || conv_f > 1) {
        printf("usage: n (number of processes) p (chances of blocking) t (number of seconds the block takes)\n");
        printf("ERROR: input for p (chances of blocking) is invalid\n");
        return EXIT_FAILURE;
    } else {
        p = conv_f;
    }

    errno = 0;
    conv = strtol(argv[3], &pointer, 10);

    // Check for errors: e.g., the string does not represent an integer
    // or the integer is larger than INT_MAX or smaller than INT_MIN
    // then saves the number of seconds the block takes in t
    if (errno != 0 || *pointer != '\0' || conv > INT_MAX || conv < INT_MIN) {
        printf("usage: n (number of processes) p (chances of blocking) t (number of seconds the block takes)\n");
        printf("ERROR: input for t (number of seconds the block takes) is invalid\n");
        return EXIT_FAILURE;
    } else {
        t = conv;
    }


    const char** pipeNames = create_pipes(n);
    pid_t pid;
    int fileDescriptorRead;
    int fileDescriptorWrite;
    for(int y = 1; y <= n; y++){
        pid_t pid1 = fork();
        pid = pid1;
        if(pid == 0){
            srandom(time(0)*y);
            if(y == n){
                fileDescriptorRead = open(pipeNames[y-1], O_RDONLY);
                fileDescriptorWrite = open(pipeNames[0], O_WRONLY);
            }
            else{
                fileDescriptorRead = open(pipeNames[y-1], O_RDONLY);
                fileDescriptorWrite = open(pipeNames[y], O_WRONLY);
            }
            sleep(1);
            int number, size = 1;
            while((size = read(fileDescriptorRead, &number, sizeof(int))) > 0 && keepAtIt){
                if(random() % 100 < p * 100){
                    printf("lock on token (val = %d)\n", number);
                    sleep(t);
                    printf("unlock token\n");
                }
                number++;
                write(fileDescriptorWrite, &number, sizeof(int));
            }
            close(fileDescriptorRead);
            close(fileDescriptorWrite);

            return EXIT_SUCCESS;
        }
    }
    int fileDescriptor = open("pipe1to2", O_WRONLY); 
    int first = 0;
    write(fileDescriptor, &first, sizeof(int));
    close(fileDescriptor);
    wait(NULL);

    for(int i = 0; i < n; i++) 
        unlink(pipeNames[i]);

    return EXIT_SUCCESS;
}