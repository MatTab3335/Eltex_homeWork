#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME   "/name"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

int main (int argc, char **argv)
{
    char client_queue_name [64];
    mqd_t qdes_server, qdes_client;   // queue descriptors
    struct mq_attr attr;
    char in_buffer [MSG_BUFFER_SIZE];
    char temp_buf [MSG_BUFFER_SIZE];

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // create the client queue for receiving messages from server
    sprintf (client_queue_name, "/name-%d", getpid ());

    if ((qdes_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: mq_open (client)");
        exit (1);
    }

    if ((qdes_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }

    // send info about myself to server
    if (mq_send (qdes_server, client_queue_name, strlen (client_queue_name) + 1, 0) == -1) {
        perror ("Client: Not able to send message to server");
        exit(1);
    }

    printf ("Enter a message: ");

    while (fgets (temp_buf, MSG_BUFFER_SIZE, stdin)) {

        // send message to server
        if (mq_send (qdes_server, temp_buf, strlen (temp_buf) + 1, 0) == -1) {
            perror ("Client: Not able to send message to server");
            continue;
        }

        // receive response from server
        if (mq_receive (qdes_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        // display token received from server
        printf ("Client: Message received from server: %s\n\n", in_buffer);

        if (!strcmp(in_buffer, "close\n"))    
            break;

        printf ("Enter a message: ");
    }


    if (mq_close (qdes_client) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }
    printf ("Client: Queue is closed\n");

    if (mq_unlink (client_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    printf ("Client: Queue is removed\n");

    exit (0);
}