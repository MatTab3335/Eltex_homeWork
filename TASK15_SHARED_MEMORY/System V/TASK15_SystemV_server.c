/* Сервер будет использовать два семафора: один для подсчета входящих сообщений, второй для
    подсчета свободных буферов для записи сообщений */
#include "settings.h"

// to short error msgs processing
void error (char *msg);
void SignalHandler(int signal);

struct sh_mem *sh_mem_ptr;
int shm_id;
int buf_count_sem, msgs_count_sem;

int main ()
{
    
    key_t s_key;
    union semun  
    {
        int val;
        struct semid_ds *buf;
        ushort array [1];
    } sem_attr;
    
    char in_buffer[MAX_MSG_SIZE];
    
    //---------------INITIALIZATION---------------
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    // Get shared memory 
    if ((s_key = ftok(SHARED_MEM_KEY, PROJECT_ID)) == -1)
        error ("ftok");    
    if ((shm_id = shmget(s_key, sizeof(struct sh_mem), 0660 | IPC_CREAT)) == -1)
        error ("shmget");
    if ((sh_mem_ptr = (struct sh_mem *) shmat(shm_id, NULL, 0)) 
         == (struct sh_mem *) -1) 
        error ("shmat");
    // Initialize the shared memory
    sh_mem_ptr->buf_idx = sh_mem_ptr->buf_print_idx = sh_mem_ptr->name_id = 0;

    // counting semaphore, indicating the number of available buffers.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok(SEM_BUFFER_COUNT_NAME, PROJECT_ID)) == -1)
        error ("ftok");
    if ((buf_count_sem = semget(s_key, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // giving initial values
    sem_attr.val = MAX_BUF_SIZE;    // MAX_BUF_SIZE are available
    if (semctl (buf_count_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");

    // counting semaphore, indicating the number of msgs to be printed.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok (SEM_MSGS_COUNT_NAME, PROJECT_ID)) == -1)
        error ("ftok");
    if ((msgs_count_sem = semget (s_key, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // giving initial values
    sem_attr.val = 0;    // 0 strings are available initially.
    if (semctl (msgs_count_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");

    struct sembuf asem [1];

    asem [0].sem_num = 0;
    asem [0].sem_op = 0;
    asem [0].sem_flg = 0;
    //---------------END OF INITIALIZATION---------------


    while (1) { 
        // Is there a string to print?
        asem [0].sem_op = -1;
        if (semop(msgs_count_sem, asem, 1) == -1)
            perror ("semop: spool_signal_sem");

        strcpy(in_buffer, sh_mem_ptr->buf[sh_mem_ptr->buf_print_idx]);

        (sh_mem_ptr->buf_print_idx)++;
        if (sh_mem_ptr -> buf_print_idx == MAX_BUF_SIZE)
           sh_mem_ptr -> buf_print_idx = 0;

        /* Contents of one buffer has been printed.
           One more buffer is available for use by clients.
           Release buffer: V (buf_count_sem);  */
        asem [0].sem_op = 1;
        if (semop(buf_count_sem, asem, 1) == -1)
            perror ("semop: buffer_count_sem");
        
        printf("[MSG]: %s\n", in_buffer);

        printf("Users: ");
        for (int i = 0; i < sh_mem_ptr->name_id; i++)
            printf("%s, ", sh_mem_ptr->names[i]);
        printf("\n");
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
    if (shmdt(sh_mem_ptr) == -1)
        error ("shmdetach");
    printf("Memory detached\n");

    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
        error("memory delete");
    printf("Memory object is deleted\n");

    if (semctl(buf_count_sem, 0, IPC_RMID) == -1)
        error ("semctl IPC_RMID");
    if (semctl(msgs_count_sem, 0, IPC_RMID) == -1)
        error ("semctl IPC_RMID");
    printf("Semaphores are deleted\n");
    //exit(1);
    signal(sig, SIG_DFL);    //call default function
    exit(0);
}