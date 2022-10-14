#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char * getParent (char * path) {
    int l = strlen(path)-1;
    char *path_cpy = malloc(strlen(path));
    strcpy(path_cpy,path);
    while (l >= 0 && path_cpy[l] != '/') {
        path_cpy[l] = '\0';
        l--;
    }
    path_cpy[l] = '\0';
    return path_cpy;
}

int makeParent(char * path, int mode) {
    if (mkdir(path, mode) != 0) {
        char *parent = getParent(path);
        if (strlen(parent) == 0) {
            printf("Error: Could not create directory %s\n", path);
        }
        else {
            makeParent(parent,mode);
            makeParent(path,mode);
        }
    }
}

int strtooct(char *str) {
    int i = 0;
    int oct = 0;
    while (str[i] != '\0') {
        oct = oct * 8 + (str[i] - '0');
        i++;
    }
    return oct;
}


int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: mkdir <directory>\n");
        exit(0);
    }
    int options['z'+1];
    memset(options, 0, sizeof(options));
    int opt;
    char *mode = NULL;
    while ((opt = getopt(argc, argv, "m:p")) != -1) {
        options[opt] = 1;
        if (opt == 'm') mode = optarg;
    }
    int m = 0777;
    if (mode != NULL) m = strtooct(mode);
    for (int i = optind; i < argc; i++) {
        if (argv[i][0] == '-') continue;
        if (mkdir(argv[i], m) != 0) {
            if (options['p']) {
                makeParent(argv[i],m);
            }
            else {
                printf("Error: Could not create directory %s\n", argv[i]);
            }
        }
    }
    return 0;
}
