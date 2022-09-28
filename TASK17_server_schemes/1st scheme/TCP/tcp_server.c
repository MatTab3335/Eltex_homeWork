/* tcp server - scheme #1
 * For each client main server vreats thread = server to 
 * communicate with client
*/ 
#include "functions.h"

int main(int argc, char *argv[])
{
    // sockets variables
    int tcp_s_s, tcp_s_c;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    char in_buf[256] = {};
    char out_buf[256] = {};
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    //allocate thread list
    thread_list = allocate(thread_list, INIT_THR_NUM);
    // clear client structure
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    client_addr_size = sizeof(struct sockaddr_in);
    //create main server
    tcp_s_s = create_server(AF_INET, SOCK_STREAM, MAIN_SERVER_PORT);
 
    while (1) {

        int thread_id = 0;
        tcp_s_c = accept(tcp_s_s, (struct sockaddr *) &client_addr,
                     &client_addr_size);
        if (tcp_s_c == -1) handle_error("accept");

        n_of_clients++;

        printf("Client № %i \n", n_of_clients);
        
        if (n_of_clients >= INIT_THR_NUM)
            thread_list = reallocate(thread_list, n_of_clients + 10);

        thread_id = n_of_clients;
        pthread_create(&thread_list[thread_id], NULL, thr_func, (void *) &thread_id);
        

        sprintf(out_buf, "%i", thread_id);      
        //wait while thread bind
        while(!perm_send)
            usleep(1000);
        if (send(tcp_s_c, out_buf, sizeof(out_buf), 0) == -1)
            handle_error("main send error");
        perm_send = 0;
        
        //shutdown(tcp_s_c, SHUT_RDWR);
        close(tcp_s_c);
    }

    exit(0);
}
