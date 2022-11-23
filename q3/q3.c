#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 256

//create 1 pipe
const char* makePipe(int x, int n){
    char *result = malloc (sizeof (char) * 26);
    if(x == n){
        sprintf(result, "pipe%dto%d", x, 1);   
    }
    else{
    sprintf(result, "pipe%dto%d", x, x+1);
    }
    mkfifo(result, 0777);
    return result;
}

//creates pipes returns array of names
const char** create_pipes(int n){
    const char **pipeNames;
    pipeNames = malloc(n * sizeof(char*));
    for(int i = 1; i <= n; i++){
        pipeNames[i-1] = makePipe(i, n);
    }
    return pipeNames;
}

//n->número de processos p->probabilidade de esperar t->tempo de espera
int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("usage: n p t\n");
        return -1;
    }
    float p = atof(argv[2]);
    int n = atoi(argv[1]);
    const char** pipeNames = create_pipes(n);
    pid_t pid;
    for(int y = 1; y <= n; y++){
        pid_t pid1 = fork();
        pid = pid1;
        if(pid == 0){
            srandom(y);
            int fileDescriptorRead;
            int fileDescriptorWrite;
            if(y == n){
                fileDescriptorRead = open(pipeNames[y-1], O_RDONLY);
                fileDescriptorWrite = open(pipeNames[0], O_WRONLY);
            }
            else{
                fileDescriptorRead = open(pipeNames[y-1], O_RDONLY);
                fileDescriptorWrite = open(pipeNames[y], O_WRONLY);
            }
            int number, size = 1;
            while((size = read(fileDescriptorRead, &number, sizeof(int))) > 0){
                if(random() % 100 < p * 100){
                    printf("lock on token (val = %d) %d\n", number, y);
                    sleep(*argv[3] - '0');
                    printf("unlock token\n");
                }
                number++;
                write(fileDescriptorWrite, &number, sizeof(int));
            }
            printf("reached this %d\n", y);
            return 0;
        }
    }
    int fileDescriptor = open("pipe1to2", O_WRONLY); 
    int first = 0;
    write(fileDescriptor, &first, sizeof(int));
    close(fileDescriptor);
    wait(NULL);
    return 0;
}