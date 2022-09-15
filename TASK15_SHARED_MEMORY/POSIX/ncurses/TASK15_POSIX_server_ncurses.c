/* Сервер будет использовать два семафора: один для подсчета входящих сообщений, второй для
    подсчета свободных буферов для записи сообщений */
#include "settings.h"

// to short error msgs processing
void error (char *msg);
void SignalHandler(int signal);

struct sh_mem *sh_mem_ptr;

int main ()
{
    sem_t *buf_count_sem, *msgs_count_sem;
    int fd_shm;
    char in_buffer[MAX_MSG_SIZE];
    
    //---------------INITIALIZATION---------------
    windows_init();
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    // Get shared memory 
    if ((fd_shm = shm_open(SHARED_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660)) == -1)
        error("shm_open");

    if (ftruncate(fd_shm, sizeof (struct sh_mem)) == -1)
       error("ftruncate");
    
    if ((sh_mem_ptr = mmap(NULL, sizeof(struct sh_mem), PROT_READ | PROT_WRITE, MAP_SHARED,
            fd_shm, 0)) == MAP_FAILED)
       error("mmap");
    // Initialize the shared memory
    sh_mem_ptr->buf_idx = sh_mem_ptr->buf_print_idx = sh_mem_ptr->name_id = 0;

    // counting semaphore, indicating the number of available buffers. Initial value = MAX_BUF_SIZE
    if ((buf_count_sem = sem_open(SEM_BUFFER_COUNT_NAME, O_CREAT | O_EXCL, 0660, MAX_BUF_SIZE)) == SEM_FAILED)
        error("sem_open");

    // counting semaphore, indicating the number of msgs to be printed. Initial value = 0
    if ((msgs_count_sem = sem_open(SEM_MSGS_COUNT_NAME, O_CREAT | O_EXCL, 0660, 0)) == SEM_FAILED)
        error("sem_open");
    //---------------END OF INITIALIZATION---------------


    while (1) { 
        // Is there a string to print?
        if (sem_wait(msgs_count_sem) == -1)
            error("sem_wait: msgs_count_sem");

        strcpy(in_buffer, sh_mem_ptr->buf[sh_mem_ptr->buf_print_idx]);

        (sh_mem_ptr->buf_print_idx)++;
        if (sh_mem_ptr -> buf_print_idx == MAX_BUF_SIZE)
           sh_mem_ptr -> buf_print_idx = 0;

        /* Contents of one buffer has been printed.
           One more buffer is available for use by clients.
           Release buffer: V (buf_count_sem);  */
        if (sem_post(buf_count_sem) == -1)
            error ("sem_post: buf_count_sem");
        
        print_auto_row(chat_window, in_buffer);
        // printf("[MSG]: %s\n", in_buffer);

        // printf("Users: ");
        for (int i = 0; i < sh_mem_ptr->name_id; i++) {
            print(clients_window, sh_mem_ptr->names[i], i);
            // printf("%s, ", sh_mem_ptr->names[i]);
        }
        // printf("\n");
    }
}

// Print system error and exit
void error (char *msg)
{
    perror (msg);
    exit (1);
}
void SignalHandler(int sig)
{
    windows_del();
    if (munmap(sh_mem_ptr, sizeof (struct sh_mem)) == -1)
        error ("munmap");
    printf("Memory unmapped\n");
    if (shm_unlink(SHARED_MEM_NAME) == -1)
        error("memory unlink");
    printf("Memory object is deleted\n");
    if (sem_unlink(SEM_MSGS_COUNT_NAME) == -1)
        error("sem unlink");
    if (sem_unlink(SEM_BUFFER_COUNT_NAME) == -1)
        error("sem unlink"); 
    printf("Semaphores are deleted\n");
    //exit(1);
    signal(sig, SIG_DFL);    //call default function
    exit(0);
}