#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
 
int main(int argc, char* argv[]) {
    FILE* file;
    int n = 0, m = 0, offset = 0;
    long bound;

    if (argc <= 3) {
        printf("usage: samples file numberfrags maxfragsize\n");
        return -1;
    }

    char *p;
    int errno = 0;
    long conv = strtol(argv[2], &p, 10);

    // Check for errors: e.g., the string does not represent an integer
    // or the integer is larger than int
    // then saves the numberfrags in n
    if (errno != 0 || *p != '\0' || conv > INT_MAX || conv < INT_MIN) {
        printf("usage: samples file numberfrags (n) maxfragsize \n");
        printf("ERROR: input for n is invalid\n");
        return EXIT_FAILURE;
    } else {
        n = conv;
    }

    errno = 0;
    conv = strtol(argv[3], &p, 10);

    // Check for errors: e.g., the string does not represent an integer
    // or the integer is larger than int
    // then saves the maxfragsize in m
    if (errno != 0 || *p != '\0' || conv > INT_MAX || conv < INT_MIN) {
        printf("usage: samples file numberfrags (n) maxfragsize \n");
        printf("ERROR: input for m is invalid\n");
        return EXIT_FAILURE;
    } else {
        m = conv;
    }

    char* output = malloc(sizeof(char) * m + 2);
    char c;
    output[0] = '>'; output[m+1] = '<'; output[m+2] = 0;

    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("ERROR: %s could not be opened\n", argv[1]);
        return EXIT_FAILURE;
    }
    fseek(file, 0, SEEK_END);
    bound = ftell(file) - m;

    srandom(0);
    for (int i = 0; i < n; i++) {
        offset = (rand() % (bound - 0 + 1)) + 0; 
        fseek(file, offset, SEEK_SET);
        for (int j = 1; j < m + 1; j++) {
            c = fgetc(file);
            if (c == '\n') {
                j--;
                continue;
            }
            else output[j] = c;
        }
        printf("%s \n", output);
    }

    fclose(file);
    free(output);
    return EXIT_SUCCESS;
}