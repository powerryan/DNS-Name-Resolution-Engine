#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "util.h"

#define MAX_INPUT_FILES 10
#define MAX_PARSE_THREADS 20
#define MAX_CONVERT_THREADS 10
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

int isFull();
int isEmpty();
int addToBuffer(char**buff, char*add);
char* removeFromBuffer(char**buff);
void* parse(void *tnumber);
void* convert();
int main(int argc, char *argv[]);
