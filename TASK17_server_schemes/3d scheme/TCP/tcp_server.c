/* tcp server - scheme #3 - main server creates a pull of
 * thread servers to process clients and queue, in that queue server 
 * puts a descriptor of new client - free server take it, tells main 
 * server that it has taken the client, server tells client new server
 * to connect, finally they start communication
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
    //
    n_of_clients = 0;
    n_of_threads = INIT_THR_NUM;
    int *serv_idx;
    //
    mqd_t qdes_server, qdes_threads;   // queue descriptors
    struct mq_attr attr;
 
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
    tcp_s_s = create_server(AF_INET, SOCK_STREAM, MAIN_SERVER_PORT);
    // fill attributes to queue
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    // open queues
    if ((qdes_server = mq_open (SERVER_QUEUE_NAME, 
        O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) 
        handle_error("mq_open");
    if ((qdes_threads = mq_open (THREADS_QUEUE_NAME, 
        O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) 
        handle_error("mq_open");
	
	memset(&client_addr, 0, sizeof(struct sockaddr_in)); //clear
    client_addr_size = sizeof(struct sockaddr_in);

    //create pull of server/threads
    for (int i = 0; i < INIT_THR_NUM; i++) {
        serv_idx[i] = i;
        pthread_create(&thread_list[i], NULL, thr_func, (void *) &serv_idx[i]);
        usleep(10000);
    }
    printf("Pull is ready, %i threads are ready to accept clients\n", n_of_threads);

    while (1) {     
        char temp[256] = {};
		// accept new client
        tcp_s_c = accept(tcp_s_s, (struct sockaddr *) &client_addr,
                     &client_addr_size);
        if (tcp_s_c == -1) 
            handle_error("accept");
            
        n_of_clients++;
        printf("Client № %i \n", n_of_clients - 1);
        // put to threads queue client info
        sprintf(temp, "%i", tcp_s_c);
        if (mq_send (qdes_threads, temp, strlen(temp) + 1, 0) == -1)
            handle_error("Server: Not able to send message to queue");
        // printf("Descriptor '%s' is sended to queue\n", temp);
        // get the port of server, which accepted client
        if (mq_receive (qdes_server, in_buf, MSG_BUFFER_SIZE, NULL) == -1)
            handle_error("Server: Not able to get message from queue");
        // send to client info about new server
        if (send(tcp_s_c, in_buf, sizeof(in_buf), 0) == -1)
            handle_error("main send id error"); 
             
        close(tcp_s_c);
            
    }
    
    if (mq_close (qdes_server) == -1) 
        handle_error("Main server: mq_close");
    printf ("Server: Queue is closed\n");
    if (mq_unlink (SERVER_QUEUE_NAME) == -1) 
        handle_error("Main server: mq_unlink");
    printf ("Server: Queue is removed\n");
    close(tcp_s_c);
    exit(0);
}
