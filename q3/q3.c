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
    create_pipes(*argv[2]);
    int temp;
    pid_t pid;
    for(int y = 1; y <= *argv[2] - '0'; y++){
        pid_t pid1 = fork();
        pid = pid1;
        if(pid1 == 0) {
            temp = y;
            break;
        }
    }
    if(pid == 0){
    char pipeToRead[] = "pipextoy";
    char pipeToWrite[] = "pipextoy";
    if(temp == *argv[2] - '0'){
        pipeToWrite[4] = temp + '0';
        pipeToWrite[7] = '1';
        pipeToRead[4] = (temp - 1) + '0';
        pipeToRead[7] = temp + '0';
    }
    else{
        pipeToWrite[4] = temp + '0';
        pipeToWrite[7] = (temp +1) + '0';
        pipeToRead[4] = (temp - 1) + '0';
        pipeToRead[7] = temp + '0';   
    }
    printf("id: %d;\ntoWrite: %s;\ntoRead: %s;\n", getpid(), pipeToWrite, pipeToRead);
    return 0;
    }
    wait(NULL);
    return 0;
    /*while(read(antecessor)){
        ... (check a chance)
        write(sucessor)
    }*/
}