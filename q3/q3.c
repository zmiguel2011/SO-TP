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

//creates pipes
int create_pipes(char n){
    char name[] = "pipextoy";
    for(int i = 1; i < n - '0'; i++){
        name[4] = i + '0';
        name[7] = (i + 1) + '0';
        mkfifo(name, 0777);
    }
    name[4] = n;
    name[7] = '1';
    mkfifo(name, 0777);
    return 0;
}

//n->número de processos p->probabilidade de esperar t->tempo de espera
int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("usage: n p t\n");
        return -1;
    }
    float p = atof(argv[2]);
    int n = atoi(argv[1]);
    create_pipes(*argv[1]);
    pid_t pid;
    for(int y = 1; y <= *argv[1] - '0'; y++){
        pid_t pid1 = fork();
        pid = pid1;
        if(pid == 0){
            srandom(y);
            char pipeToRead[] = "pipextoy";
            char pipeToWrite[] = "pipextoy";
            if(y == *argv[1] - '0'){
                pipeToWrite[4] = y + '0';
                pipeToWrite[7] = '1';
                pipeToRead[4] = (y - 1) + '0';
                pipeToRead[7] = y + '0';
            }
            else{
                pipeToWrite[4] = y + '0';
                pipeToWrite[7] = (y +1) + '0';
                if(y == 1){
                    pipeToRead[4] = *argv[1];
                    pipeToRead[7] = 1 + '0';
                }
                else{
                    pipeToRead[4] = (y - 1) + '0';
                    pipeToRead[7] = y + '0';
                }   
            }
        int fileDescriptorRead = open(pipeToRead, O_RDONLY);
        int fileDescriptorWrite = open(pipeToWrite, O_WRONLY);
        int number, size = 1;
        sleep(1);
        while((size = read(fileDescriptorRead, &number, sizeof(int))) > 0){
            if(random() % 100 < p * 100){
                printf("lock on token (val = %d)\n", number);
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