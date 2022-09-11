#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>

#include "windows.h"

#define SERVER_QUEUE_NAME   "/server_chat"
#define QUEUE_PERMISSIONS 0660      //-rw
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#define MAX_CLIENTS 3

//Template for regular msgs: "clients_queue_name*message"
/*Template for service msgs:   
    * "/cmd add_user username" - in/out msg
    * "/cmd del_user username" - in/out msg
    * "/cmd refresh_clients" - output msg
    * "/cmd end_refresh_clients" - output msg
    * "/cmd close" - output (close extra client_)
*/                  
void process_msgs (void);
int del_el_double_ar (char array[][MSG_BUFFER_SIZE], char *element, int size);
void del_el_mqd_t (int *array, int idx, int size);
void send_to_all(char *out_buffer);
void refresh_clients();
void close_myself();
void SignalHandler(int signal);
void printClients();

mqd_t qdes_server, qdes_client[MAX_CLIENTS];   // queue descriptors
struct mq_attr attr;
char out_buffer [MSG_BUFFER_SIZE] = {};
char clients_info [MAX_CLIENTS][MSG_BUFFER_SIZE] = {};
int n_of_client = 0;

int main (int argc, char **argv)
{
    windows_init();

    // printf ("Server: I'm server!\n");

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    //----register signals----
    signal(SIGINT, SignalHandler);

    //create server queue
    if ((qdes_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    while (1)
        process_msgs();
    
    close_myself();

    exit (0);
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
    return -1;
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
void send_to_all(char *out_buffer)
{
    //-----Send to all users-----
    for (int i = 0; i < n_of_client; i++)
        if (mq_send (qdes_client[i], out_buffer, strlen (out_buffer) + 1, 0) == -1) {
            perror ("Server: Not able to send message to client");
                return;
        }
}
void refresh_clients()
{
    char integer[5];
    sprintf(integer, "%i", n_of_client);
    send_to_all("/cmd refresh_clients");
    send_to_all(integer);

    for (int i = 0; i < n_of_client; i++)
        send_to_all(clients_info[i]);
    
    send_to_all("/cmd end_refresh_clients");
}
void process_msgs ()
{
    char in_buffer [MSG_BUFFER_SIZE] = {};
    char temp[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];
    char name[128];
    char cmd[128];
    char arg[128];
    char *token;                 //to store tokens of string
    char perm_to_join = 1;

    while(1) {
        int p = 0;      //to operate tokens of string
        
        if (mq_receive (qdes_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }
        sprintf(temp, "[MSG]: %s\n", in_buffer);
        print_auto_row (chat_window, temp);
        //if receives service message
        if (!strncmp ("/cmd", in_buffer, 4)) {
            token = strtok (in_buffer, " ");
            //split cmd and args
            while (token != NULL) {
                switch (p) {
                  default: break;
                  case 1: strcpy (cmd, token);
                  case 2: strcpy (arg, token);  
                } 
                token = strtok (NULL, " ");
                p++;
            }
            p = 0;

            //----ADD user----
            if (perm_to_join == 1 && !strcmp ("add_user", cmd)) {
                strcpy(clients_info[n_of_client], arg);
                sprintf(temp, "User %s has joined the chat!\n", clients_info[n_of_client]);
                print_auto_row(chat_window, temp);
                // get clients descriptor
                if ((qdes_client[n_of_client] = mq_open (clients_info[n_of_client], O_WRONLY)) == -1) {
                    perror ("Server: Not able to open client queue");
                    continue;
                }
                n_of_client++;
                //---refresh list of clients in clients---
                refresh_clients();
                printClients();
            }
            //----if clients buffer is full - send cmd to close new user
            else if (perm_to_join == 0 && !strcmp ("add_user", cmd)) {
                mqd_t qdes;
                if ((qdes = mq_open (arg, O_WRONLY)) == -1) {
                    perror ("Server: Not able to open client queue");
                    continue;
                } 
                if (mq_send (qdes, "/cmd close", strlen ("/cmd close") + 1, 0) == -1) {
                    perror ("Server: Not able to send message to client");
                    continue;
                }
            }
            //----DELETE user----
            else if (!strcmp ("del_user", cmd)) {
                int idx = 0;
                // if this user exists in list
                if ((idx = del_el_double_ar(clients_info, arg, n_of_client)) >= 0) {
                    del_el_mqd_t(qdes_client, idx, n_of_client);
                    sprintf(temp, "User %s has left the chat!\n", arg);
                    print_auto_row(chat_window, temp);
                    n_of_client --;
                    refresh_clients();
                    printClients();
                }
            }
            //----check number of clients----
            if (n_of_client >= MAX_CLIENTS) {
                sprintf(temp, "Number of clients >= %i. New user can't join\n", MAX_CLIENTS);
                print_auto_row(chat_window, temp);
                perm_to_join = 0;
            } else perm_to_join = 1;  
        } else {                            //if regular message
            //split name and message
            token = strtok (temp, "*");
            while (token != NULL) {
                if (p == 0) 
                    strcpy (name, token);
                else 
                    strcpy (msg, token);
                token = strtok (NULL, "*");
                p++;
            }
            p = 0;

            //----make out message----
            char temp1[strlen(msg)];
            strcpy (temp1, msg);
            sprintf(out_buffer, "%s: %s", name, temp1);

            //-----Send to all users-----
            send_to_all(out_buffer);

        }
    }       
}
void close_myself()
{
    if (mq_close (qdes_server) == -1) {
        perror ("Server: mq_close");
        exit (1);
    }
    print_auto_row(chat_window, "Server: Queue is closed");

    if (mq_unlink (SERVER_QUEUE_NAME) == -1) {
        perror ("Server: mq_unlink");
        exit (1);
    }
    print_auto_row(chat_window, "Server: Queue is removed");
    windows_del();
}
void SignalHandler(int sig)
{
    //tell clients to close
    send_to_all("/cmd close");
    close_myself();
    exit(0);
    // printf("Server is closed\n");
    signal(sig, SIG_DFL);    //call default function
}
void printClients()
{
    wclear(clients_window);
    box(clients_window, '|', '-');
    wmove(clients_window, 1, 1);

    for (int i = 0; i < n_of_client; i++) {
        print(clients_window, clients_info[i], i);
    }
    wrefresh(clients_window);
}