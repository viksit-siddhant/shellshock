# Shellshock
Shellshock is an implementation of the Linux shell, written using C. 
## Installation
- `cd` into `shellshock`
- Run `make`
- Run `./shellshock`
## Working
Internal commands such as `pwd` and `cd` are handled by the shell binary itself. For external commands, the shell looks up the appropriate binary in the `./bin/` directory and then executes it in a separate process. By default this is handled by `fork()` and the `execl` family of functions. However, the user can make the shell use the POSIX Pthread library by appending `&t` to the end of the command, for example,`ls -a ~ &t`. Features of options:
- Options can be passed in any order
- Supports multiple options at one

## Commands
### Internal Commands
#### echo
Echo is a command to print text to stdout. 
Example:
```
echo Hello World!
```
Implemented options are-
- `-n` doesn't print the trailing newline after the text. Example:
```
sid@sids-laptop:/home/sid/Projects/shellshock$ echo -n Hello!
Hello! sid@sids-laptop:/home/sid/Projects/shellshock$ 
```
- `-e` handles special characters such as *"\n"* or *"\t"*, converting them to special characters while printing. Example:
```sh
echo -e \tHello!
```
will print
```sh
    Hello!
```

Edge cases handled:
- If no argument is passed, 
- If `-e` is passed but the special characters used aren't implemented, the program will write them verbatim to stdout.

#### pwd
pwd prints the current working directory of the shell. Example:
```sh
pwd
```
Implemented options are:
- `-L` uses the `PWD` environment variable to get the working directory
- `-P` uses the `getcwd()` system call on Linux instead. (Default behaviour)

Edge cases handled:
- Returns an error if it is unable to get the working directory
- Gives a warning if `-L` and `-P` are both used

#### cd
cd changes to the directory specified to it, or to the home directory if none is specified. Example:
```sh
cd -L ~
```
Implemented options are:
- `-L` doesn't resolves symlinks and handles `..` components logically
- `-P` resolves symlinks and handles `..` components physically 

If neither is specified, `-L` will be used 

Edge cases handled:
- Changes directory to the home directory if no other directory is passed to it
- Returns an error if given an invalid path

### External commands
#### mkdir
mkdir makes an empty directory in the path specified. Usage:
```sh
mkdir -pm 777 temp1 temp2/temp3 temp4
```
Options implemented are:
- `-p`: If unable to create the directory directly, make parent directories as needed
- `-m [MODE]`: Sets the permissions of the directory created. See `man chmod` for information on file permissions.  

Edge cases handled:
- If not passed any arguments, it will show how to use the command
- If unable to create a directory, it throws an error

#### date
date prints the system date and time, by default, the local timezone. Usage:
```sh
date -u
```
Options implemented are:
- `-I`: Prints the date in ISO-8601 format
- `-u`: Prints the UTC datetime

Edge cases handled:
- Prints an error if unable to get time
- Warns the user if they try to input an excess number of argumente

#### ls
ls prints all the directories and files in the specified directory. Example:
```sh
ls -a1 ~
```
Options handled are:
- `-a` prints hidden files and directories as well
- `-1` transforms the list from space-separated to line-separated

Edge cases handled:
- If no directory is specified, it defaults to the current working directory
- It returns an error if it's unable to open any specified directory

#### rm
rm deletes all the specified files. Usage:
```sh
$ mkdir -p temp1/temp2 temp3
$ rm -ri temp1 temp3
```
Options handled are:
- `-r` recursively deletes all files and subdirectories 
- `-i` asks the user before all deletions 

Edge cases handled:
- It returns an error if it's unable to delete any file or directory
- Prints the usage if not passed any arguments

#### cat
cat concatenates the contents of all given files and prints it to stdout.
```sh
cat -nE ~/shellshock/src/cat.c
```
Options handled are:
- `-n` Numbers all the lines 
- `-E` adds a `$` to the end to every line 

Edge cases handled:
- It returns an error if it's unable to open any file
- Buffering the file input in case the file is too large to fit into the buffer
