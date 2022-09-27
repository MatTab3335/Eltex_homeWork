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
    
    char in_buf[256];
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);
	//allocate pull
    thread_list = allocate(thread_list, INIT_THR_NUM);
    //create main server
    sfd = create_server(AF_UNIX, SOCK_STREAM, MY_SOCK_PATH);
	
	memset(&client_addr, 0, sizeof(struct sockaddr_un)); //clear
    client_addr_size = sizeof(struct sockaddr_un);

    while (1) {
        int thread_id = 0;
        char out_buf[256] = {};
        struct args *arg = (struct args*)malloc(sizeof(struct args));

        cfd = accept(sfd, (struct sockaddr *) &client_addr,
                     &client_addr_size);
        if (cfd == -1) handle_error("accept");

        printf("Client № %i \n", thread_id));
        
        // increase number of servers
        if (thread_id >= INIT_THR_NUM)
            thread_list = reallocate(thread_list, n_of_clients + 10);
		// fill args to pass to server
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
