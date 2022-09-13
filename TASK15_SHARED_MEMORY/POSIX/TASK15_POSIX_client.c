
#include "settings.h"

// to short error msgs processing
void error (char *msg);
void SignalHandler(int sig);
void del_el_double_ar(char array[][MAX_MSG_SIZE], int id, int size); //delete array element and shift others
void *get_msg();
void *send_msg();

sem_t *buf_count_sem, *msgs_count_sem;
struct sh_mem *sh_mem_ptr;
int my_name_id = 0;

int main ()
{
    int fd_shm;
    pthread_t thr_send, thr_get;
    char in_buffer [256];
    int *ret_val; 

    //---------------INITIALIZATION---------------
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    // Get shared memory 
    if ((fd_shm = shm_open(SHARED_MEM_NAME, O_RDWR, 0)) == -1)
        error("shm_open");

    if ((sh_mem_ptr = mmap(NULL, sizeof(struct sh_mem), PROT_READ | PROT_WRITE, MAP_SHARED,
            fd_shm, 0)) == MAP_FAILED)
        error("mmap");

    // counting semaphore, indicating the number of available buffers.
    if ((buf_count_sem = sem_open(SEM_BUFFER_COUNT_NAME, 0, 0, 0)) == SEM_FAILED)
        error ("sem_open");

    // counting semaphore, indicating the number of strings to be printed. Initial value = 0
    if ((msgs_count_sem = sem_open(SEM_MSGS_COUNT_NAME, 0, 0, 0)) == SEM_FAILED)
        error ("sem_open");

    //---------------END OF INITIALIZATION---------------

    //----Create threads to send and receive msgs----
    pthread_create (&thr_get, NULL, get_msg, NULL);
    pthread_create (&thr_send, NULL, send_msg, NULL);
    //----wait threads----
    pthread_join(thr_get, (void **) &ret_val);
    pthread_join(thr_send, (void **) &ret_val);

 
    exit(0);
}

void *get_msg()
{
    int prev_print_idx = 0;
    prev_print_idx = sh_mem_ptr->buf_print_idx;

    while(1) {
        //if no new msg
        if (prev_print_idx == sh_mem_ptr->buf_print_idx)
            usleep(500000); //0.5sec
        else {
            prev_print_idx = sh_mem_ptr->buf_print_idx;
            printf("[MSG]: %s\n", sh_mem_ptr->buf[sh_mem_ptr->buf_print_idx - 1]);
        }
    }
}
void *send_msg()
{
    char temp [MAX_MSG_SIZE], *cp;

    printf("Please type a message: ");

    while (fgets(temp, MAX_MSG_SIZE - 2, stdin)) {
        //to once send to server my name
        static char once_flag = 1;

        // remove newline from string
        int length = strlen (temp);
        if (temp [length - 1] == '\n')
           temp [length - 1] = '\0';
        // get a buffer:
        if (sem_wait(buf_count_sem) == -1)
            error("sem_wait: buf_count_sem");
            //save name
            if (once_flag){
                char t[128];
                once_flag = 0;
                sprintf(t, "%d", getpid());
                strcpy(sh_mem_ptr->names[sh_mem_ptr->name_id], t);
                my_name_id = sh_mem_ptr->name_id;
                sh_mem_ptr->name_id++;
                if (sh_mem_ptr->name_id >= MAX_BUF_SIZE) sh_mem_ptr->name_id = 0;
            }

            time_t now = time(NULL);
            cp = ctime(&now);
            int len = strlen (cp);
            if (*(cp + len -1) == '\n')
                *(cp + len -1) = '\0';

            sprintf (sh_mem_ptr->buf[sh_mem_ptr->buf_idx], "%d: %s %s\n", getpid(), 
                     cp, temp);
            (sh_mem_ptr->buf_idx)++;
            if (sh_mem_ptr->buf_idx == MAX_BUF_SIZE)
                sh_mem_ptr->buf_idx = 0;
    
       // Tell server that there is a string to print
        if (sem_post(msgs_count_sem) == -1)
            error("sem_post: (msgs_count_sem");
    }
}
// Print system error and exit
void error (char *msg)
{
    perror(msg);
    exit(1);
}
void del_el_double_ar(char array[][MAX_MSG_SIZE], int id, int size)
{
    for (int y = id; y < size - 1; y++)
        strcpy (array[y], array[y + 1]);
    memset (array[size - 1], 0, strlen (array[size - 1]));
}
void SignalHandler(int sig)
{

    //по идее надо поставить для удаления юзера еще один семафор, но лень
    del_el_double_ar(sh_mem_ptr->names, my_name_id, MAX_BUF_SIZE);
    sh_mem_ptr->name_id--;

    if (munmap(sh_mem_ptr, sizeof (struct sh_mem)) == -1)
        error ("munmap");
    printf("Memory unmapped\n");
    if (sem_close(buf_count_sem) == -1)
        error("sem unlink");
    if (sem_close(msgs_count_sem) == -1)
        error("sem unlink"); 
    printf("Semaphores are closed\n");
    //exit(1);
    signal(sig, SIG_DFL);    //call default function
    exit(0);
}