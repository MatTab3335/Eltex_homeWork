/* udp server - scheme #1
 * For each client main server vreats thread = server to 
 * communicate with client
*/ 
#include "server_functions.h"

int main(int argc, char *argv[])
{
    // sockets variables
    int s_sock, c_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    char in_buf[256];
    char out_buf[256] = {};
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    //allocate thread list
    thread_list = allocate(thread_list, INIT_THR_NUM);
    // clear client structure
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    client_addr_size = sizeof(struct sockaddr_in);
    //create main server
    s_sock = create_server(AF_UNIX, SOCK_STREAM, MY_SOCK_PATH);
 
    while (1) {

        int thread_id = 0;
        cfd = accept(s_sock, (struct sockaddr *) &client_addr,
                     &client_addr_size);
        if (c_sock == -1) handle_error("accept");

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
        if (send(c_sock, out_buf, sizeof(out_buf), 0) == -1)
            handle_error("main send error");
        perm_send = 0;
        
        //shutdown(c_sock, SHUT_RDWR);
        close(c_sock);
    }

    exit(0);
}
