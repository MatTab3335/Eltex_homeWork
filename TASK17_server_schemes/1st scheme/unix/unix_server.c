/* unix server - scheme #1
 * For each client main server vreats thread = server to 
 * communicate with client
*/ 
#include "server_functions.h"

int main(int argc, char *argv[])
{
    // sockets variables
    int sfd, cfd;
    struct sockaddr_un client_addr;
    socklen_t client_addr_size;
    char in_buf[256];
    char out_buf[256] = {};
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    //allocate thread list
    thread_list = allocate(thread_list, INIT_THR_NUM);
    // clear client structure
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr_size = sizeof(struct sockaddr_un);
    //create main server
    sfd = create_server(AF_UNIX, SOCK_STREAM, MY_SOCK_PATH);
 
    while (1) {

        int thread_id = 0;
        cfd = accept(sfd, (struct sockaddr *) &client_addr,
                     &client_addr_size);
        if (cfd == -1) handle_error("accept");

        printf("Client № %i \n", n_of_clients);
        
        if (n_of_clients >= INIT_THR_NUM)
            thread_list = reallocate(thread_list, n_of_clients + 10);

        thread_id = n_of_clients;
        pthread_create(&thread_list[thread_id], NULL, thr_func, (void *) &thread_id);
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
