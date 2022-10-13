#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    int options['z'+1];
    memset(options, 0, sizeof(options));
    int opt;
    while ((opt = getopt(argc, argv, "a1")) != -1) {
        options[opt] = 1;
    }
    char *name;
    if (argc == optind) {
        name = malloc(100);
        getcwd(name, 100);
        DIR *dir = opendir(name);
        if (dir == NULL) {
            printf("Error: Could not open directory %s\n",name);
            exit(0);
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') {
                if (options['a']) printf("%s", entry->d_name);
                continue;
            }
            printf("%s", entry->d_name);
            if (entry->d_type == DT_DIR) printf("/");
            if(options['1']) printf("\n");
            else printf(" ");
        }
        printf("\n");
    }
    else {
        for (int i = optind; i < argc; i++) {
            name = argv[i];
            DIR *dir = opendir(name);
            if (dir == NULL) {
                printf("Error: Could not open directory %s\n",name);
                exit(0);
            }
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] == '.') {
                    if (options['a']) printf("%s", entry->d_name);
                    continue;
                }
                printf("%s", entry->d_name);
                if (entry->d_type == DT_DIR) printf("/");
                if(options['1']) printf("\n");
                else printf(" ");
            }
            printf("\n");
        }
    }
    return 0;
}