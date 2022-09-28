// functions for 2nd scheme tcp
#include "functions.h"

int n_of_clients = 0;
int n_of_threads = 0;
//thread variablses
pthread_t *thread_list = NULL;
int perm_send = 0;
char *serv_stat;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;


pthread_t *allocate(pthread_t *list, int init_value)    //alloc list of threads
{        
    if (!(list = (pthread_t *)malloc(init_value * sizeof(pthread_t)))) {
        handle_error("Allocation error!");
        exit(1);
    }
        return list;
}
pthread_t *reallocate(pthread_t *list, int size){      //change size
    if (!(list = (pthread_t *)realloc(list, size * sizeof(pthread_t)))) {
        handle_error("Reallocation error!");
        exit(1);
    }
    return list;
}
void SignalHandler(int signal)
{
    printf("Finish server\n");
    for (int i = 0; i < n_of_threads; i++) {
        pthread_cancel(thread_list[i]);
    }
    free(thread_list);
    exit(0);
}
void SignalHandlerClient(int signal)
{   
    // close(fd);
    printf("Finish client\n");
    exit(0);
}
int create_server(int domain, int type, int server_port)
{
	static int tcp_s;
    struct sockaddr_in my_addr;
    
    /* Clear structure */
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    
    tcp_s = socket(domain, type, 0);
    if (tcp_s == -1)
        handle_error_int("socket", server_port);
    
    my_addr.sin_family = domain;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(server_port);

    if (bind(tcp_s, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1)
        handle_error_int("bind", server_port);

    if (listen(tcp_s, LISTEN_BACKLOG) == -1)
        handle_error_int("listen", server_port);
        
    printf("Server is listening on port %i\n", server_port);
    
    return tcp_s;
}
int connect_server(int domain, int type, int server_port)
{
    struct sockaddr_in server_addr;
    int tcp_s;
    
    tcp_s = socket(domain, type, 0);
    if (tcp_s == -1)
        handle_error_int("socket", server_port);

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = domain;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(domain, "127.0.0.1", &server_addr.sin_addr) == -1)
        handle_error("inet_pton");
    
    if (connect(tcp_s, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)))
        handle_error_int("connect error", server_port);
       
    return tcp_s;
}
void *thr_func(void *input)      //function for message processing thread
{
    int my_tcp_s, tcp_s_c;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    char in_buf[256] = {};
    char my_path[256] = {};

    int id = *((int *)input);
    int server_port = id + 1 + MAIN_SERVER_PORT;

    client_addr_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    my_tcp_s = create_server(AF_INET, SOCK_STREAM, server_port);
        
    perm_send = 1;      //permit sent to client info about me

    while (1) {
        while(1) {        
    		tcp_s_c = accept(my_tcp_s, (struct sockaddr *) &client_addr,
    						 &client_addr_size);
    		if (tcp_s_c == -1) 
    			handle_error("accept");

    		while(1) {
    			int recv_bytes = recv(tcp_s_c, in_buf, sizeof(in_buf), 0);
    			if (recv_bytes == -1 || recv_bytes == 0) {
    					printf("Thread %i is free\n", id);
    					serv_stat[id] = 0;
    					break;
    			}
    			printf("[MSG%i]: %s\n", id, in_buf);
    			if (send(tcp_s_c, in_buf, sizeof(in_buf), MSG_NOSIGNAL) == -1)
    				handle_error("send error");
    		}
            close(tcp_s_c);
    	}
    }
    return NULL;
}
void *thr_func_client(void *input)
{
    int tcp_s;
    char in_buf[256] = {};
    char server_path[256] = {};
    int server_port = 0;

    int id = *((int *)input);
    
    printf("Thread #%i is created\n", id);
    
    
    //ont thread per time to connect to main server        
    pthread_mutex_lock(&m1);
    tcp_s = connect_server(AF_INET, SOCK_STREAM, MAIN_SERVER_PORT);
    pthread_mutex_unlock(&m1);
    //get new server id
    int recv_bytes = recv(tcp_s, in_buf, sizeof(in_buf), 0);
    if (recv_bytes == -1 || recv_bytes == 0)
        handle_error_int("1st recv error", id);
        
    printf("Client %i: %s\n", id, in_buf);
    close(tcp_s);  // close coonection to main server
    // make new server port
    server_port = MAIN_SERVER_PORT + atoi(in_buf) +1;

    // connect to new server
    tcp_s = connect_server(AF_INET, SOCK_STREAM, server_port);

    if (send(tcp_s, "Hi", sizeof("Hi"), MSG_NOSIGNAL) == -1)
        handle_error_int("2nd send error", id); 

    while (1) {
        int recv_bytes = recv(tcp_s, in_buf, sizeof(in_buf), 0);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Client №%i is closed\n", id);
                break;
        }
        printf("[MSG]: %s\n", in_buf);
    }
    close(tcp_s);
    exit(0);
}
void handle_error_int(char *msg, int id)
{
    char temp[256] = {};
    sprintf(temp, "id[%i]: %s", id, msg);
    perror(temp); exit(EXIT_FAILURE);
}
void handle_error_char(char *msg, char *info)
{
    char temp[256] = {};
    sprintf(temp, "<%s> : %s", info, msg);
    perror(temp); 
    exit(EXIT_FAILURE);
}
