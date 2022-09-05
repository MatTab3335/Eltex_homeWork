//Server program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "path"
#define PROJECT_ID 'A'
#define QUEUE_PERMISSIONS 0660      //-rw

struct message_text {
    int qid;
    char buf [200];
};

struct message {
    long message_type;
    struct message_text message_text;
};

int main ()
{
    key_t msg_queue_key;        //server queue key
    int myqid;                    //message queue id
    struct message message;

    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }
    //create server queue
    if ((myqid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (1);
    }

    printf ("Server: I'm server\n");

    while (1) {
        // read an incoming message
        if (msgrcv (myqid, &message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("msgrcv");
            exit (1);
        }

        printf ("Server: message received.\n");

        int client_qid = message.message_text.qid;
        message.message_text.qid = myqid;

        // send reply message to client
        if (msgsnd (client_qid, &message, sizeof (struct message_text), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }
        printf ("Server: response sent to client.\n");

        if (!strcmp(message.message_text, "close") break;               
    }
    if (msgctl (myqid, IPC_RMID, NULL) == -1) {
        perror ("server: msgctl");
        exit (1);
    }
    printf("Server queue is closed, finished.\n");
    exit(0);
}