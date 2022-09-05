#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME   "/name"
#define QUEUE_PERMISSIONS 0660      //-rw
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

int main (int argc, char **argv)
{
    mqd_t qdes_server, qdes_client;   // queue descriptors
    int token_number = 1; // next token to be given to client
    struct mq_attr attr;
    char in_buffer [MSG_BUFFER_SIZE];
    char out_buffer [MSG_BUFFER_SIZE];

    printf ("Server: I'm server!\n");

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qdes_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    
    while (1) {
        // get the oldest message with highest priority
        if (mq_receive (qdes_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }

        printf ("Server: message received.\n");

        // send reply message to client
        if ((qdes_client = mq_open (in_buffer, O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
            continue;
        }

        sprintf (out_buffer, "%i", token_number);

        if (mq_send (qdes_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
            perror ("Server: Not able to send message to client");
            continue;
        }

        printf ("Server: response sent to client.\n");
        token_number++;
    }
}