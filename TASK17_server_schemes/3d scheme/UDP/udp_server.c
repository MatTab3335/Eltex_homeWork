/* tcp server - scheme #2 - main server creates a pull of
 * thread servers to process clients - if number of clients > pull =>
 * create new servers and delete them if its not used. When thread is free
 * it tells main server about that and next client will use thi thread
*/
#include "functions.h"


int main(int argc, char *argv[])
{
    // sockets variables
    int udp_s_s, udp_s_с;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    char in_buf[256] = {};
    char out_buf[256] = {};
    //
    n_of_clients = 0;
    n_of_threads = INIT_THR_NUM;
    int *serv_idx;
 
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
    // allocate servers status array
    serv_stat = (char *)malloc(n_of_threads * sizeof(char));
    for (int i = 0; i < n_of_threads; i++)
		serv_stat[i] = 0;
    // allocate servers index array
    serv_idx = (int *)malloc(INIT_THR_NUM * sizeof(int));
	// allocate pull
    thread_list = allocate(thread_list, n_of_threads);
    // create main server
    udp_s_s = create_server(AF_INET, SOCK_DGRAM, MAIN_SERVER_PORT);
	
	memset(&client_addr, 0, sizeof(struct sockaddr_in)); //clear
    client_addr_size = sizeof(struct sockaddr_in);

    //create pull of server/threads
    for (int i = 0; i < INIT_THR_NUM; i++) {
        serv_idx[i] = i;
        pthread_create(&thread_list[i], NULL, thr_func, (void *) &serv_idx[i]);
        usleep(10000);
    }
    printf("Pull is ready\n");

    while (1) {     
        // get msg from new client
		int recv_bytes = recvfrom(udp_s_s, in_buf, sizeof(in_buf), 0, 
                (struct sockaddr *) &client_addr, &client_addr_size);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Main server is closed\n");
                exit(1);
        }

        n_of_clients++;
        printf("Client № %i \n", n_of_clients - 1);
        
        // increase number of servers
        if (n_of_clients >= n_of_threads) {
            n_of_threads += INIT_THR_NUM;
            thread_list = reallocate(thread_list, n_of_threads);
            serv_stat = realloc(serv_stat, n_of_threads * sizeof(char));
            serv_idx = realloc(serv_idx, n_of_threads * sizeof(int));
            //increase pull of server/threads
			for (int i = n_of_clients; i < n_of_threads; i++) {
				serv_idx[i] = i;
				// printf("idx = %i\n", serv_idx[i]);
				pthread_create(&thread_list[i], NULL, thr_func, (void *) &serv_idx[i]);
				usleep(10000);
			}
            printf("N of servers increased: %i\n", n_of_threads);
        }

        // check what server is not busy, then send to new client id of this server
        for (int i = 0; i < n_of_threads; i++) {
            if (serv_stat[i] == 0) {
				serv_stat[i] = 1;
                sprintf(out_buf, "%i", i);
                if (sendto(udp_s_s, out_buf, sizeof(out_buf), 0,
                    (struct sockaddr *) &client_addr, client_addr_size) == -1)
                    handle_error("main send error");
                break;
            }
        } 
        memset(&client_addr, 0, sizeof(struct sockaddr_in));
            client_addr_size = sizeof(struct sockaddr_in);   
        
    }
    
    exit(0);
}
