/* tcp server - scheme #2 - main server creates a pull of
 * thread servers to process clients - if number of clients > pull =>
 * create new servers and delete them if its not used. When thread is free
 * it tells main server about that and next client will use thi thread
*/
#include "server_functions.h"


int main(int argc, char *argv[])
{
    // sockets variables
    int cfd, sfd;
    struct sockaddr_un client_addr;
    socklen_t client_addr_size;
    //
    n_of_clients = 0;
    n_of_threads = INIT_THR_NUM;
    int *serv_idx;
    char out_buf[256] = {};
    
    char in_buf[256];
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
    sfd = create_server(AF_UNIX, SOCK_STREAM, MY_SOCK_PATH);
	
	memset(&client_addr, 0, sizeof(struct sockaddr_un)); //clear
    client_addr_size = sizeof(struct sockaddr_un);

    //create pull of server/threads
    for (int i = 0; i < INIT_THR_NUM; i++) {
        serv_idx[i] = i;
        pthread_create(&thread_list[i], NULL, thr_func, (void *) &serv_idx[i]);
        usleep(10000);
    }
    printf("Pull is ready\n");

    while (1) {     
		   
		// nprintf("Waiting for accept\n");
        cfd = accept(sfd, (struct sockaddr *) &client_addr,
                     &client_addr_size);
        if (cfd == -1) 
            handle_error("accept");

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
				printf("idx = %i\n", serv_idx[i]);
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
                if (send(cfd, out_buf, sizeof(out_buf), 0) == -1)
                    handle_error("main send id error");
                break;
            }
        }    
        close(cfd);
    }
    
	unlink_servers();
    unlink(MY_SOCK_PATH);
    exit(0);
}
