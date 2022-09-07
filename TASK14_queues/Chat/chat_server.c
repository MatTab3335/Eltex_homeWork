#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>

#define SERVER_QUEUE_NAME   "/server_chat"
#define QUEUE_PERMISSIONS 0660      //-rw
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#define MAX_CLIENTS 10

//Template for regular msgs: "clients_queue_name*message"
/*Template for service msgs:   
    * "/cmd add_user username" - in/out msg
    * "/cmd del_user username" - in/out msg
    * "/cmd refresh_clients" - output msg
    * "/cmd end_refresh_clients" - output msg
*/                  
void process_msgs (void);
int del_el_double_ar (char array[][MSG_BUFFER_SIZE], char *element, int size);
void del_el_mqd_t (int *array, int idx, int size);
void send_to_all(char *out_buffer);
void refresh_clients();

mqd_t qdes_server, qdes_client[MAX_CLIENTS];   // queue descriptors
struct mq_attr attr;
char in_buffer [MSG_BUFFER_SIZE] = {};
char out_buffer [MSG_BUFFER_SIZE] = {};
char clients_info [MAX_CLIENTS][MSG_BUFFER_SIZE] = {};
int n_of_client = 0;

int main (int argc, char **argv)
{
    printf ("Server: I'm server!\n");

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    //create server queue
    if ((qdes_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    while (1)
        process_msgs();
    

    if (mq_close (qdes_server) == -1) {
        perror ("Server: mq_close");
        exit (1);
    }

    printf ("Server: Queue is closed\n");
    if (mq_unlink (SERVER_QUEUE_NAME) == -1) {
        perror ("Server: mq_unlink");
        exit (1);
    }
    printf ("Server: Queue is removed\n");

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

    for (int i = 0; i < MAX_CLIENTS; i++)
        send_to_all(clients_info[i]);
    
    send_to_all("/cmd end_refresh_clients");
}
void process_msgs ()
{
    char temp[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];
    char name[128];
    char cmd[128];
    char arg[128];
    char *token;                 //to store tokens of string
    char perm_to_join = 1;

    while(1) {
        
        if (mq_receive (qdes_server, temp, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }
        printf ("Received message: %s", temp);
        //if receives service message
        if (!strncmp ("/cmd ", temp, 5)) {
            token = strtok (temp, " ");
            //split cmd and args
            while (token != NULL) {
                static int p = 0;
                switch (p) {
                  case 0: break;
                  case 1: strcpy (cmd, token);
                  case 2: strcpy (arg, token);  
                } 
                token = strtok (NULL, " ");
                p++;
            }
            //----Add user----
            if (perm_to_join && !strcmp ("add_user\n", cmd)) {
                strcpy(clients_info[++ n_of_client], arg);
                printf("User %s has joined the chat!\n", clients_info[n_of_client]);
                // get clients descriptor
                if ((qdes_client[n_of_client] = mq_open (clients_info[n_of_client], O_WRONLY)) == 1) {
                    perror ("Server: Not able to open client queue");
                    continue;
                }
                //---refresh list of clients in clients---
                refresh_clients();
            }
            //----Delete user----
            else if (!strcmp ("del_user\n", cmd)) {
                int idx = del_el_double_ar(clients_info, arg, n_of_client);
                del_el_mqd_t(qdes_client, idx, n_of_client);
                printf("User %s has left the chat!\n", arg);
                n_of_client --;
                refresh_clients();
            }
            //----check number of clients----
            if (n_of_client >= 10) {
                printf("Number of client > %i. New user can't join\n", MAX_CLIENTS);
                perm_to_join = 0;
            } else perm_to_join = 1;    
        } else {                            //if regular message
            //split name and message
            token = strtok (temp, "*");
            while (token != NULL) {
                static int p = 0;
                if (p == 0) 
                    strcpy (name, token);
                else 
                    strcpy (msg, token);
                token = strtok (NULL, "*");
                p++;
            }
            //----make out message----
            char temp1[strlen(msg)];
            strcpy (temp1, msg);
            sprintf(out_buffer, "%s: %s", name, temp1);
            printf("Message: %s", out_buffer);
            //-----Send to all users-----
            send_to_all(out_buffer);
            printf ("Server: responses sent to client.\n\n");
        }
    }       
}