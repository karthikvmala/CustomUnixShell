#ifndef STANDARD_H
#define STANDARD_H

#include <errno.h>
#include <string.h>  
#include <unistd.h>  
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>  
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include "pwd.h"
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>


#define white "\033[0;37m"
#define red "\033[0;31m"
#define green "\033[0;32m"
#define yellow "\033[0;33m"
#define blue "\033[0;34m"
#define magenta "\033[0;35m"
#define orange "\033[0;36m"
#define reset "\033[0m"
#define max_path 4096
#define his_no 15

typedef struct backgroundProcess {
    pid_t pidofprocess;
    char name[max_path];
}backgroundProcess;

extern int currentbackgroundprocess;
extern backgroundProcess* bg;

int min(int a, int b);
void set_word(char* word);
char* add_string(char* b, char* a, int offset);
void tokenize(char** buffer, int* n, char* input, const char* delim);
#endif