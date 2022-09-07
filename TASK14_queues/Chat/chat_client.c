#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>

#define SERVER_QUEUE_NAME   "/server_chat"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#define MAX_CLIENTS 10

int del_el_double_ar (char array[][MSG_BUFFER_SIZE], char *element, int size);
void del_el_mqd_t (int *array, int idx, int size);

//Template for regular msgs: "clients_queue_name*message"
/*Template for service msgs:   
    * "/cmd add_user username" - output msg
    * "/cmd del_user username" - output msg
    * "/cmd refresh_clients" - input msg
*/
char in_buffer [MSG_BUFFER_SIZE];
char out_buffer [MSG_BUFFER_SIZE];
char temp_buf [MSG_BUFFER_SIZE];
char del_cmd [MSG_BUFFER_SIZE];
char add_cmd [MSG_BUFFER_SIZE];
char perm_flag = 1;
char clients_info [MAX_CLIENTS][MSG_BUFFER_SIZE] = {};
int n_of_clients = MAX_CLIENTS;

int main (int argc, char **argv)
{
    char my_queue_name [64];
    mqd_t qdes_server, my_qdes;   // queue descriptors
    struct mq_attr attr;
    pthread_t thr_send, thr_get;
    int *ret_val;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // create the client queue for receiving messages from server
    sprintf (my_queue_name, "/name-%d", getpid ());
    // create first cmd message to add myself to chat
    sprintf (add_cmd, "/cmd add_user %s", my_queue_name);
    // create cmd message to del myself from chat
    sprintf (del_cmd, "/cmd del_user %s", my_queue_name);

    if ((my_qdes = mq_open (my_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: mq_open (client)");
        exit (1);
    }
    if ((qdes_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }
    // send info about myself to server
    if (mq_send (qdes_server, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
        perror ("Client: Not able to send message to server");
        exit(1);
    }
    //----Create threads to send and receive msgs----
    pthread_create (&thr_get, NULL, get_msg, NULL);
    pthread_create (&thr_send, NULL, send_msg, NULL);
    //----wait threads----
    pthread_join(thr_get, (void **) &ret_val);
    pthread_join(thr_send, (void **) &ret_val);
    //----close mqueue----
    if (mq_close (my_qdes) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }
    printf ("Client: Queue is closed\n");

    if (mq_unlink (my_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    printf ("Client: Queue is removed\n");

    exit (0);
}

void *get_msg()
{
    char name[128];
    char cmd[128];
    char arg[128];

    while (perm_flag) {
        // receive response from server
        if (mq_receive (my_qdes, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        // display msg received from server
        printf ("%s\n", in_buffer);
        //----If received msg = refresh clients
        if (!strncmp(in_buffer, "/cmd refresh_clients", 20)) {
            while (1) {                     //get n_of_ckients and array of clients
                static int p = 0;
                if (mq_receive (my_qdes, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                    perror ("Client: mq_receive");
                    exit (1);
                }
                if (!strncmp(in_buffer, "/cmd end_refresh_clients", 24)) break;

                if (p == 0) n_of_clients = atoi (in_buffer);
                else strcpy (clients_info[i - 1], in_buffer);
                p ++;
            }
            //---print current users---
            printf ("Current users:\n");
            for (int i = 0; i < n_of_clients; i++)
                printf ("%s, ", clients_info[i]);
        }    
    }
}
void *send_msg()
{
    printf ("Enter a message: ");

    while (fgets (temp_buf, MSG_BUFFER_SIZE, stdin)) {
        // send message to server
        if (mq_send (qdes_server, temp_buf, strlen (temp_buf) + 1, 0) == -1) {
            perror ("Client: Not able to send message to server");
            continue;
        }
        //if cmd = delete user - quit and close queues
        if (!strcmp(temp_buf, del_cmd)) {
            perm_flag = 0;
            break;
        }   
            
        printf ("Enter a message: ");
    }
}
int del_el_double_ar (char array[][MSG_BUFFER_SIZE], char *element, int size)
{
    int i = 0;
    for (i = 0; i < size; i++) {
        if (!strcmp (array[i], element)) {
            if (i == size -1) memset (array[i], 0, strlen (array[i]));
            else {
                for (int y = i; y < size - 1; y++)
                    strcpy (array[y], array[y + 1]);
                memset (array[size - 1], 0, strlen (array[size - 1]));
            }
            return i;
        }
    }
}
void del_el_mqd_t (int *array, int idx, int size)
{
    if (idx == size - 1) array[idx] = 0;
    else {
        for (int i = idx; i < size - 1; i++)
            array[i] = array[i + 1];
        array[size - 1] = 0;
    }
    return;
}