#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

char *strip(char *str) {
    int len = strlen(str);
    int ptr = 0;
    while (ptr < len && isWhitespace(str[ptr])) ptr++;
    int ptr2 = len-1;
    while (ptr2 >= 0 && isWhitespace(str[ptr2])) {
        str[ptr2] = '\0';
        ptr2--;
    }
    return str + ptr;
}
char *path;
void *exec_thread(void *arg) {
    char *argv = (char *)arg;
    char bin_path[100];
    memset(bin_path, 0, 100);
    strcpy(bin_path, path);
    strcat(bin_path,argv);
   // printf("Executing %s\n", bin_path);
    if (system(bin_path) != 0) {
        printf("Error: Command not found: %s", argv);
    }
    pthread_exit(NULL);
}

char *replace_back(char *str) {
    int len = strlen(str);
    char *new_str = malloc(len+1);
    memset(new_str, 0, len+1);
    int ptr1 = 0, ptr2 = 0;
    while (ptr1 < len) {
        if (str[ptr1] == '\\' && ptr1 < len-1) {
            if (str[ptr1+1] == 'b') new_str[ptr2++] = '\b';
            else if (str[ptr1+1]=='n') new_str[ptr2++] = '\n';
            else if (str[ptr1+1]=='t') new_str[ptr2++] = '\t';
            else if (str[ptr1+1]=='v') new_str[ptr2++] = '\v';
            else if (str[ptr1+1]=='r') new_str[ptr2++] = '\r';
            else if (str[ptr1+1]=='f') new_str[ptr2++] = '\f';
            else if (str[ptr1+1]=='a') new_str[ptr2++] = '\a';
            else if (str[ptr1+1]=='\\') new_str[ptr2++] = '\\';
            else {
                new_str[ptr2++] = str[ptr1];
                new_str[ptr2++] = str[ptr1+1];
            }
            ptr1 += 2;
        }
        else {
            new_str[ptr2++] = str[ptr1++];
        }
    }
    return new_str;
}

int main() {
    char *buf = malloc(100);
    path = malloc(100);
    getcwd(path, 100);
    path = strcat(path, "/bin/");
    while (1) {
        char *user = getlogin();
        char *prompt = strcat(user, "@");
        gethostname(buf, 100);
        prompt = strcat(prompt, buf);
        prompt = strcat(prompt, ":");
        getcwd(buf, 100);
        prompt = strcat(prompt, buf);
        prompt = strcat(prompt, "$ ");
        printf("%s", prompt);
        char cmd[100], line[100];
        memset(line, 0, 100);
        fgets(cmd, 100, stdin);
        char **args = malloc(100*sizeof(char*));
        memset(args,0,100*sizeof(char*));
        int i = 0;
        char *arg = strtok(cmd, " ");
        while (arg != NULL) {
            if (arg[0] == '~') {
                char *home = getenv("HOME");
                arg = strcat(home, arg+1);
            }
            arg = strip(arg);
            if (strlen(arg) == 0) {
                arg = strtok(NULL, " ");
                continue;
            }
            args[i] = arg;
            strcat(line, arg);
            strcat(line, " ");
            i++;
            arg = strtok(NULL, " ");
        }
        line[strlen(line)-1] = '\0';
        int num_args = i;
        int create_thread = 0;
        optind = 1;
        if (strcmp(args[num_args-1],"&t") == 0) {
            create_thread = 1;
            args[num_args-1] = NULL;
            num_args--;
            for (int j = 0;j<3;j++) {
                line[strlen(line)-1] = '\0';
            }
        }
        if (strcmp(args[0], "exit") == 0) {
            exit(0);
        }
        else if (strcmp(args[0], "cd") == 0) {
            int opt;
            int flag_l = 1;
            while ((opt = getopt(num_args, args, "LP")) != -1) {
                if (opt == 'L') {
                    if (!flag_l) {
                        printf("Warning: -L and -P are exclusive\n");
                    }
                    flag_l = 1;

                }
                else if (opt == 'P') {
                    flag_l = 0;
                }
            }
            if (optind == num_args) {
                if (chdir(getenv("HOME")) != 0) {
                    printf("Error: Could not change directory to home directory\n");
                }
            }
            else {
                char filename[100];
                memset(filename, 0, 100);
                strcpy(filename, args[1]);
                if (!flag_l) {
                    realpath(filename,args[optind]);
                }
                if (chdir(args[optind]) != 0) {
                    printf("cd: %s: No such file or directory\n", args[1]);
                }
            }
        }
        else if (strcmp(args[0], "pwd") == 0) {
            int opt;
            int flag_l = 0;
            while ((opt = getopt(num_args, args, "LP")) != -1) {
                if (opt == 'L') flag_l = 1;
                else if (opt == 'P') {
                    if (flag_l) {
                        printf("Warning: -L and -P are mutually exclusive\n");
                    }
                    flag_l = 0;
                }
            }
            if (flag_l) {
                buf = getenv("PWD");
            }
            else getcwd(buf, 100);
            if (buf == NULL) {
                printf("Error: Could not get current working directory\n");
            }
            else printf("%s\n", buf);
        }
        else if (strcmp(args[0], "echo") == 0) {
            int opt;
            int flag_n = 0, flag_e = 0;
            while ((opt = getopt(num_args, args, "ne")) != -1) {
                if (opt == 'n') flag_n = 1;
                else if (opt == 'e') flag_e = 1;
            }
            for (int j = optind; j < num_args; j++) {
                if (flag_e != 0) args[j] = replace_back(args[j]);
                printf("%s ", args[j]);
            }
            if (flag_n == 0) printf("\n");
        }
        else {
            if (create_thread == 0) {
                if (fork() == 0) {
                    char bin_path[100];
                    memset(bin_path, 0, 100);
                    strcpy(bin_path, path);
                    strcat(bin_path,args[0]);
                    if (execvp(bin_path, args) == -1) {
                        printf("Error: Command not found\n");
                        exit(-1);
                    }
                    exit(0);
                }
                else {
                    wait(NULL);               
                }
            }
            else {
                pthread_t t;
                pthread_create(&t, NULL, exec_thread, line);
                pthread_join(t, NULL);
            }
        }
    }
}