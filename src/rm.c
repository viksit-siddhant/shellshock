#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

void rec_walk(char *path, int *options) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Error: Could not open directory %s",path);
        exit(0);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char *new_path = malloc(strlen(path)+strlen(entry->d_name)+2);
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, entry->d_name);
        if (entry->d_name[0] == '.') continue;
        if (entry->d_type == DT_DIR) {
            if (options['i']) {
                printf("Descend into %s?(y/n)\n", new_path);
                char mode[5];
                scanf("%s", mode);
                if (mode[0] != 'y') continue;
            }
            rec_walk(new_path,options);
        }
        if (options['i']) {
            if (entry->d_type == DT_DIR) printf("Remove %s/?(y/n)\n", new_path);
            else printf("Remove %s?(y/n)\n", new_path);
            char mode[5];
            scanf("%s", mode);
            if (strcmp(mode, "y") != 0) continue;
        }
        if (remove(new_path) != 0) {
            printf("Error: Could not remove %s\n", new_path);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: rm [options] <files>\n");
        exit(0);
    }
    int options['z'+1];
    memset(options, 0, sizeof(options));
    int opt;
    while ((opt = getopt(argc, argv, "ir")) != -1) {
        options[opt] = 1;
    }
    for (int i = optind; i < argc;i++) {
        char mode[5];
        if (options['r']) {
            rec_walk(argv[i], options);
        }
        if (options['i']) {
            printf("rm: Remove %s?(y/n)", argv[i]);
            scanf("%s", mode);
            if (strcmp(mode, "y") != 0) continue;
        }
        if (remove(argv[i]) != 0) {
            printf("Error: Could not remove file %s\n", argv[i]);
        }
    }
    return 0;
}