#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

#include "windows.h"


#define SERVER_QUEUE_NAME   "/server_chat"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#define MAX_CLIENTS 3

int del_el_double_ar (char array[][MSG_BUFFER_SIZE], char *element, int size);
void del_el_mqd_t (int *array, int idx, int size);
void *send_msg(void *args);
void *get_msg(void *args);
void empty_stdin(void);
void close_myself(void);
void SignalHandler(int signal);
void printClients();

//Template for regular msgs: "clients_queue_name*message"
/*Template for service msgs:   
    * "/cmd add_user username" - output msg
    * "/cmd del_user" - output msg
    * "/cmd refresh_clients" - input msg
    * "/cmd close" - input - close myself
*/
mqd_t qdes_server, my_qdes;   // queue descriptors
char my_queue_name [64];
char in_buffer [MSG_BUFFER_SIZE];
char out_buffer [MSG_BUFFER_SIZE];
char del_cmd [MSG_BUFFER_SIZE];
char add_cmd [MSG_BUFFER_SIZE];
char perm_flag = 1;
char clients_info [MAX_CLIENTS][MSG_BUFFER_SIZE] = {};
int n_of_clients = MAX_CLIENTS;

int main (int argc, char **argv)
{
    struct mq_attr attr;
    pthread_t thr_send, thr_get;
    int *ret_val;

    windows_init();
    //----register signals----
    signal(SIGINT, SignalHandler);

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
    if (mq_send (qdes_server, add_cmd, strlen (add_cmd) + 1, 0) == -1) {
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
    close_myself();

    exit (0);
}

void *get_msg(void *args)
{
    char name[128];
    char cmd[128];
    char arg[128];
    char temp[MSG_BUFFER_SIZE];
    int p = 0;

    while (perm_flag) {
        // receive response from server
        if (mq_receive (my_qdes, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        // display msg received from server
        print_auto_row (chat_window, in_buffer);
        //----If received msg = refresh clients
        if (!strncmp(in_buffer, "/cmd refresh_clients", 20)) {
            while (1) {                     //get n_of_ckients and array of clients
                if (mq_receive (my_qdes, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                    perror ("Client: mq_receive");
                    exit (1);
                }
                if (!strncmp(in_buffer, "/cmd end_refresh_clients", 24)) break;

                if (p == 0) n_of_clients = atoi (in_buffer);
                else strcpy (clients_info[p - 1], in_buffer);
                //printf("n_of_clients = %i\n", n_of_clients);
                p++;
            }
            p = 0;
            //---print current users---
            sprintf(temp, "Now in chat %i users:\n", n_of_clients);
            print_auto_row (chat_window, temp);
            printClients();

            for (int i = 0; i < n_of_clients; i++)
                print_auto_row (chat_window, clients_info[i]);
        }
        // if server tell me close - close  
        else if (!strncmp(in_buffer, "/cmd close", 10)) 
            close_myself(); 
    }
}
void *send_msg(void *args)
{
    char temp_buf [MSG_BUFFER_SIZE];
    int del = 0;        //flag to delete user

    while(1) {
        wgetstr(msg_window, temp_buf);
        wclear(msg_window);
        box(msg_window, '|', '-');
        wmove(msg_window, 1, 1);
        wrefresh(msg_window);

        char temp[strlen(temp_buf)];
        strcpy(temp, temp_buf);

        if (!strncmp("/cmd del_user", temp_buf, 13)) {
            sprintf(out_buffer, "%s %s", "/cmd del_user", my_queue_name);
            del = 1;
        }
        else sprintf(out_buffer, "%s*%s", my_queue_name, temp);

        // send message to server
        if (mq_send (qdes_server, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
            perror ("Client: Not able to send message to server");
            continue;
        }
        //if cmd = delete user - quit and close queues
        if (del) {
            perm_flag = 0;
            break;
        }   
            
        
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
void empty_stdin() 
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}
void close_myself()
{
    if (mq_close (my_qdes) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }
    print_auto_row (chat_window, "Client: Queue is closed");

    if (mq_unlink (my_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    print_auto_row (chat_window, "Client: Queue is removed");
    windows_del();
}
void SignalHandler(int sig)
{
    //tell server to remove me from user list
    if (mq_send (qdes_server, del_cmd, strlen (del_cmd) + 1, 0) == -1)
            perror ("Client: Not able to send message to server");
    close_myself();
    exit(0);
    signal(sig, SIG_DFL);    //call default function
}
void printClients()
{
    wclear(clients_window);
    box(clients_window, '|', '-');
    wmove(clients_window, 1, 1);

    for (int i = 0; i < n_of_clients; i++) {
        print(clients_window, clients_info[i], i);
    }
    wrefresh(clients_window);
}