//functions for 1st scheme udp
#include "functions.h"

int n_of_clients = 0;
int n_of_threads = 0;
//thread variablses
pthread_t *thread_list = NULL;
int perm_send = 0;
static pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

pthread_t *allocate(pthread_t *list, int init_value){        //alloc list of threads

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
    for (int i = 0; i < n_of_clients; i++) {
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
    static int udp_s;
    struct sockaddr_in my_addr;
    
    /* Clear structure */
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    
    udp_s = socket(domain, type, 0);
    if (udp_s == -1)
        handle_error_int("socket", server_port);
    
    my_addr.sin_family = domain;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(server_port);

    if (bind(udp_s, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1)
        handle_error_int("bind", server_port);
        
    printf("Server is ready on port %i\n", server_port);
    
    return udp_s;
}
int connect_server(int domain, int type, int server_port)
{
    struct sockaddr_in server_addr;
    int udp_s;
    int server_addr_size = sizeof(server_addr);
    
    udp_s = socket(domain, type, 0);
    if (udp_s == -1)
        handle_error_int("socket", server_port);

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = domain;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(domain, "127.0.0.1", &server_addr.sin_addr) == -1)
        handle_error("inet_pton");

    sendto(udp_s, "connect", sizeof("connect"), 0,
                (struct sockaddr *) &server_addr, server_addr_size);
    
    if (connect(udp_s, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)))
        handle_error_int("connect error", server_port);
    printf("Connected to server on port %i\n", server_port);
       
    return udp_s;
}
void *thr_func(void *input)      //function for message processing thread (server)
{
    int my_udp_s, udp_s_c;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    char in_buf[256] = {};
    char my_path[256] = {};

    int id = *((int *)input);
    int server_port = id + MAIN_SERVER_PORT;

    client_addr_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    my_udp_s = create_server(AF_INET, SOCK_DGRAM, server_port);
        
    perm_send = 1;      //permit sent to client info about me
    

    while(1) {
        int recv_bytes = recvfrom(my_udp_s, in_buf, sizeof(in_buf), 0, 
                (struct sockaddr *) &client_addr, &client_addr_size);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Thread %i is closed\n", id);
                break;
        }
        printf("[MSG]: %s\n", in_buf);
        if (sendto(my_udp_s, in_buf, sizeof(in_buf), 0,
                (struct sockaddr *) &client_addr, client_addr_size) == -1)
            handle_error_int("send error", server_port);
    }
    close(udp_s_c);
}
void *thr_func_client(void *input)
{
    int udp_s;
    char in_buf[256] = {};
    char server_path[256] = {};
    int server_port = 0;

    int id = *((int *)input);
    
    printf("Thread #%i is created\n", id);
    
    
    //ont thread per time to connect to main server        
    pthread_mutex_lock(&m1);
    udp_s = connect_server(AF_INET, SOCK_DGRAM, MAIN_SERVER_PORT);
    pthread_mutex_unlock(&m1);
    //get new server id
    int recv_bytes = recv(udp_s, in_buf, sizeof(in_buf), 0);
    if (recv_bytes == -1 || recv_bytes == 0)
        handle_error_int("1st recv error", id);
        
    printf("Client %i: %s\n", id, in_buf);
    close(udp_s);  // close coonection to main server
    // make new server port
    server_port = MAIN_SERVER_PORT + atoi(in_buf);

    // connect to new server
    udp_s = connect_server(AF_INET, SOCK_DGRAM, server_port);

    if (send(udp_s, "Hi", sizeof("Hi"), MSG_NOSIGNAL) == -1)
        handle_error_int("2nd send error", id); 

    while (1) {
        int recv_bytes = recv(udp_s, in_buf, sizeof(in_buf), 0);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Client №%i is closed\n", id);
                break;
        }
        printf("[MSG%i]: %s\n", id, in_buf);
    }
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