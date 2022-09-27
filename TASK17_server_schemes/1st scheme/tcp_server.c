//tcp server - scheme #1
#include "functions.h"

/* service msg format: "<srvc>message" */

void sigpipe_handler(int unused) {}

int main(int argc, char *argv[])
{
    // sockets variables
    int sfd, cfd;
    struct sockaddr_un my_addr, client_addr;
    socklen_t client_addr_size;
    char in_buf[256];
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    //ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN); 

    thread_list = allocate(thread_list, INIT_THR_NUM);

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        handle_error("socket");

    /* Clear structure */
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    memset(&my_addr, 0, sizeof(struct sockaddr_un));
                        
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, MY_SOCK_PATH,
            sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_un)) == -1)
        handle_error("bind");

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        handle_error("listen");
    printf("listen\n");

    /* Now we can accept incoming connections one
       at a time using accept(2) */

    client_addr_size = sizeof(struct sockaddr_un);

    while (1) {

        pthread_t thr;
        int thread_id = 0;
        char out_buf[256] = {};
        struct args *arg = (struct args*)malloc(sizeof(struct args));

        cfd = accept(sfd, (struct sockaddr *) &client_addr,
                     &client_addr_size);
        if (cfd == -1) handle_error("accept");

        printf("Client № %i \n", n_of_clients);
        
        if (n_of_clients >= INIT_THR_NUM)
            thread_list = reallocate(thread_list, n_of_clients + 10);

        thread_id = n_of_clients;
        arg->cfd = cfd;
        arg->id = thread_id;
        // printf("cfd = %i, id = %i\n", arg->cfd, arg->id);

        pthread_create(&thread_list[thread_id], NULL, get_msg, (void *) arg);


        n_of_clients++;

        sprintf(out_buf, "%i", thread_id);
        
        //wait while thread bind
        while(!perm_send)
            usleep(1000);
        if (send(cfd, out_buf, sizeof(out_buf), 0) == -1)
            handle_error("main send error");
        perm_send = 0;
        

        //shutdown(cfd, SHUT_RDWR);
        close(cfd);
    }

    exit(0);
}
