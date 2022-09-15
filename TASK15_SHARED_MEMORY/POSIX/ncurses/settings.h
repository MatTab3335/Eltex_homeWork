#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>

#include "windows.h"

#define MAX_CLIENTS 10
#define MAX_BUF_SIZE 10
#define MAX_MSG_SIZE 256

#define SEM_BUFFER_COUNT_NAME   "/sem-buffer-count"
#define SEM_MSGS_COUNT_NAME     "/sem-msgs-count"
#define SHARED_MEM_NAME         "/posix-shared-mem"

struct sh_mem {
    char buf [MAX_BUF_SIZE][MAX_MSG_SIZE];
    char names [MAX_BUF_SIZE][MAX_MSG_SIZE];
    int name_id;
    int buf_idx;
    int buf_print_idx;
};