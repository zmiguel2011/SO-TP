#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MAX_SIZE 256

int create_pipes(char n){
    char name[] = "pipextoy";
    for(int x = 1; x < n - '0'; x++){
        name[4] = x + '0';
        name[7] = (x + 1) + '0';
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
    return 0;
}