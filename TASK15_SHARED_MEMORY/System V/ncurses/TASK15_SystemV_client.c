
#include "settings.h"

// to short error msgs processing
void error (char *msg);
void SignalHandler(int sig);
void del_el_double_ar(char array[][MAX_MSG_SIZE], int id, int size); //delete array element and shift others
void *get_msg();
void *send_msg();

int buf_count_sem, msgs_count_sem;
struct sembuf asem [1];
struct sh_mem *sh_mem_ptr;
int shm_id;
int my_name_id = 0;


int main ()
{
    pthread_t thr_send, thr_get;
    char in_buffer [256];
    int *ret_val; 
    key_t s_key;
    union semun  
    {
        int val;
        struct semid_ds *buf;
        ushort array [1];
    } sem_attr;

    //---------------INITIALIZATION---------------
    windows_init();
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    // Get shared memory 
    if ((s_key = ftok(SHARED_MEM_KEY, PROJECT_ID)) == -1)
        error("ftok");    
    if ((shm_id = shmget(s_key, sizeof(struct sh_mem), 0)) == -1)
        error ("shmget");
    if ((sh_mem_ptr = (struct sh_mem *) shmat(shm_id, NULL, 0)) 
         == (struct sh_mem *) -1) 
        error ("shmat");

    // counting semaphore, indicating the number of available buffers.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok(SEM_BUFFER_COUNT_NAME, PROJECT_ID)) == -1)
        error("ftok");
    if ((buf_count_sem = semget(s_key, 1, 0)) == -1)
        error ("semget");

    // counting semaphore, indicating the number of strings to be printed.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok(SEM_MSGS_COUNT_NAME, PROJECT_ID)) == -1)
        error("ftok");
    if ((msgs_count_sem = semget(s_key, 1, 0)) == -1)
        error("semget");
    
    

    asem [0].sem_num = 0;
    asem [0].sem_op = 0;
    asem [0].sem_flg = 0;

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
            print_auto_row(chat_window, sh_mem_ptr->buf[sh_mem_ptr->buf_print_idx - 1]);
            for (int i = 0; i < sh_mem_ptr->name_id; i++)
                print(clients_window, sh_mem_ptr->names[i], i);
        }
    }
}
void *send_msg()
{
    char temp [MAX_MSG_SIZE], *cp;

    printf("Please type a message: ");

    while (1) {
        wgetstr(msg_window, temp);
        wclear(msg_window);
        box(msg_window, '|', '-');
        wmove(msg_window, 1, 1);
        wrefresh(msg_window);
        //to once send to server my name
        static char once_flag = 1;

        // remove newline from string
        int length = strlen (temp);
        if (temp [length - 1] == '\n')
           temp [length - 1] = '\0';
        // get a buffer:
        asem [0].sem_op = -1;
        if (semop(buf_count_sem, asem, 1) == -1)
            error ("semop: buffer_count_sem");
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
        asem [0].sem_op = 1;
        if (semop(msgs_count_sem, asem, 1) == -1)
            error("semop: msgs_count_sem");
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
    windows_del();
    //по идее надо поставить для удаления юзера еще один семафор, но лень
    del_el_double_ar(sh_mem_ptr->names, my_name_id, MAX_BUF_SIZE);
    sh_mem_ptr->name_id--;

    if (shmdt(sh_mem_ptr) == -1)
        error ("shmdetach");
    printf("Memory detached\n");

    //exit(1);
    signal(sig, SIG_DFL);    //call default function
    exit(0);
}