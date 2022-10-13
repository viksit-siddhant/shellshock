#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: cat <file>\n");
        exit(0);
    }
    int options['z'+1];
    memset(options, 0, sizeof(options));
    int opt;
    while ((opt = getopt(argc, argv, "nE")) != -1) {
        options[opt] = 1;
    }
    int line_num = 0;
    for (int i = optind; i < argc;i++) {
        if (argv[i][0] == '-') continue;
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) {
            printf("cat: %s: No such file or directory\n", argv[i]);
            continue;
        }
        char *buf = malloc(10000);
        while (fgets(buf, 10000, file) != NULL) {
            if (options['E']) {
                if (buf[strlen(buf)-1] == '\n') {
                    buf[strlen(buf)-1] = '$';
                    buf[strlen(buf)+1] = '\0';
                    buf[strlen(buf)] = '\n';
                }
            }
            if (options['n']) {
                printf("%d ", ++line_num);
            }
            printf("%s", buf);
        }
    }
    printf("\n");
    return 0;
}