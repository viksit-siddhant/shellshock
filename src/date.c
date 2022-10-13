#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    char *s = malloc(100);
    int options['z'+1];
    memset(options, 0, sizeof(options));
    int opt;
    while ((opt = getopt(argc, argv, "Iu")) != -1) {
        options[opt] = 1;
    }
    time_t t = time(NULL);
    struct tm *tm;
    if (options['u']) tm = gmtime(&t);
    else tm = localtime(&t);
    if (tm == NULL) {
        printf("Error: Could not get time\n");
        exit(1);
    }
    if (optind != argc) {
        printf("Warning: date doesn't take any additional arguments");
    }
    if (options['I']) strftime(s, 100, "%Y-%m-%d", tm);
    else strftime(s, 100, "%a %b %d %H:%M:%S %Z %Y", tm);
    printf("%s\n", s);
    return 0;
}