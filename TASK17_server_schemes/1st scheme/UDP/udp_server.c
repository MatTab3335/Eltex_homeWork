/* udp server - scheme #1
 * For each client main server vreats thread = server to 
 * communicate with client
*/ 
#include "functions.h"

int main(int argc, char *argv[])
{
    // sockets variables
    int udp_s_s, udp_s_c;
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
    udp_s_s = create_server(AF_INET, SOCK_DGRAM, MAIN_SERVER_PORT);
 
    while (1) {

        int thread_id = 0;
        int recv_bytes = recvfrom(udp_s_s, in_buf, sizeof(in_buf), 0, 
                (struct sockaddr *) &client_addr, &client_addr_size);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Main server is closed\n");
                exit(1);
        }

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
        if (sendto(udp_s_s, out_buf, sizeof(out_buf), 0,
                (struct sockaddr *) &client_addr, client_addr_size) == -1)
            handle_error("main send error");
        perm_send = 0;

        memset(&client_addr, 0, sizeof(struct sockaddr_in));
            client_addr_size = sizeof(struct sockaddr_in);
        
        //close(udp_s_c);
    }

    exit(0);
}
