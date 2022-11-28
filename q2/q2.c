#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MAX_SIZE 256


/* 
* Create a child process for each file to convert
* @param - command is the pandoc command 
* @param filename is the file being converted
*/
int fork_n_execute(const char *command, const char *filename) {
    pid_t pid;

    if (command == NULL)
        return EXIT_FAILURE;

    pid = fork();

    if(pid == 0)
    {
        /* This is the child process */
        printf("[pid%d] converting %s ...\n", getpid(), filename);
        system(command); // execute the command
        exit(EXIT_SUCCESS);
    }

    else if(pid > 0)
        wait(NULL);
    
    else {
        printf("ERROR: Unable to create child process.\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
/*
* Creates the command to execute pandoc in shell
* @param filename - the name of the file that will be converted
*/
char* call_pandoc(char *filename) {
    if (filename == NULL) return NULL;

    char* name_txt = filename;
    
    /* Changes the file extension from .txt to .epub and saves it in name_epub */
    char* ptr = filename;
    char* name_epub = malloc(sizeof(char) * (strlen(ptr) - 4));
    for (int i = 0; i < sizeof(ptr); i++)
    {
        if(*ptr == '.') break;

        name_epub[i] = *ptr;
        ptr++;
    }
    strcat(name_epub, ".epub");
    /* Construct system call */
    char* command = malloc(sizeof(char) * (strlen(name_txt) + strlen(name_epub) + 11));
    strcpy(command, "pandoc ");
    strcat(command, name_txt);
    strcat(command, " -o ");
    strcat(command, name_epub);

    free(name_epub);

    return command;
}

/*
*Create the command to create the zip file in shell
*@param files -- array of files to add to the zip
*@param n - number of files to add to the zip
*/
int zip_files(char** files, const int n){
    if (files == NULL) 
        return EXIT_FAILURE;

    /* Allocates space for the zip call (len("zip ebooks.zip "") = 14 + 1 = 15) */
    char* command = malloc(sizeof(char) * 15);
    strcpy(command, "zip ebooks.zip");
    for (int i = 0; i < n; i++) {
        command = realloc(command, strlen(command) + sizeof(char) * (strlen(files[i]) + 1));
        strcat(command," ");
        strcat(command, files[i]);
    }

    /* System call to execute zip */
    system(command);
    free(command);
    return 0;
}


int main(int argc, char *argv[]) {
    char* command;
    char* files[argc - 1];
    if (argc <= 1) {
        printf("usage: txt2epub file1 (file2) (file3) (...)\n");
        return -1;
    }

   
    /* Loop through argv */
    for(int i = 1; i < argc; i++) {
        command = call_pandoc(argv[i]);
        fork_n_execute(command, argv[i]);
        files[i - 1] = malloc(strlen(argv[i]) * sizeof(char));
        /* Changes the file extension from .txt to .epub and saves it in name_epub */
        char* ptr = argv[i];
        char* name_epub = malloc(sizeof(char) * (strlen(ptr) - 4));
        for (int i = 0; i < sizeof(ptr); i++)
        {
            if(*ptr == '.') break;

            name_epub[i] = *ptr;
            ptr++;
        }
        strcat(name_epub, ".epub");
        strcpy(files[i - 1], name_epub);
        free(name_epub);
        free(command);
    }

    /* Create zip file */
    zip_files(files, argc - 1);

    for (int i = 0; i < argc - 1; i++)
    {
        free(files[i]);
    }
    
    return EXIT_SUCCESS;
}
