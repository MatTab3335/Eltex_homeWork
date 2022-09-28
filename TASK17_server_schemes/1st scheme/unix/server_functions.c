//functions for 1st scheme unix
#include "server_functions.h"

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
        char temp[256] = {};
        sprintf(temp, "/tmp/tcp_server_%i", i);
        unlink(temp);
        pthread_cancel(thread_list[i]);
    }
    unlink(MY_SOCK_PATH);
    free(thread_list);
    exit(0);
}
void SignalHandlerClient(int signal)
{   
    // close(fd);
    printf("Finish client\n");
    exit(0);
}
int create_server(int domain, int type, char *path)
{
    static int sfd;
    struct sockaddr_un my_addr;
    
    /* Clear structure */
    memset(&my_addr, 0, sizeof(struct sockaddr_un));
    
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        handle_error_info("socket", path);
    
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, path, sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1)
        handle_error_info("bind", path);

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        handle_error_info("listen", path);
        
    printf("Server %s is listening\n", path);
    
    return sfd;
}
int connect_server(int domain, int type, char *path)
{
    struct sockaddr_un server_addr;
    int fd;
    
    fd = socket(domain, type, 0);
    if (fd == -1)
        handle_error_info("socket", path);

    memset(&server_addr, 0, sizeof(struct sockaddr_un));

    server_addr.sun_family = domain;
    strncpy(server_addr.sun_path, path,
            sizeof(server_addr.sun_path) - 1);
    
    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)))
        handle_error_info("connect error", path);
       
    return fd;
}
void *thr_func(void *input)      //function for message processing thread
{
    int my_fd, cfd;
    struct sockaddr_un client_addr;
    socklen_t client_addr_size;
    char in_buf[256] = {};
    char my_path[256] = {};

    int id = *((int *)input);

    client_addr_size = sizeof(struct sockaddr_un);
    memset(&client_addr, 0, sizeof(struct sockaddr_un));

    sprintf(my_path, "/tmp/tcp_server_%i", id);

    my_fd = create_server(AF_UNIX, SOCK_STREAM, my_path);
        
    perm_send = 1;      //permit sent to client info about me
    
    cfd = accept(my_fd, (struct sockaddr *) &client_addr,
                     &client_addr_size);

    while(1) {
        int recv_bytes = recv(cfd, in_buf, sizeof(in_buf), 0);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Thread %i is closed\n", id);
                break;
        }
        printf("[MSG]: %s\n", in_buf);
        if (send(cfd, in_buf, sizeof(in_buf), MSG_NOSIGNAL) == -1)
            handle_error("send error");
    }
    unlink(my_path);
}
void *thr_func_client(void *input)
{
    int fd;
    char in_buf[256] = {};
    char server_path[256] = {};

    int id = *((int *)input);
    
    printf("Thread #%i is created\n", id);
    
    
    //ont thread per time to connect to main server        
    pthread_mutex_lock(&m1);
    fd = connect_server(AF_UNIX, SOCK_STREAM, MAIN_SERVER_PATH);
    pthread_mutex_unlock(&m1);
    //get new server id
    int recv_bytes = recv(fd, in_buf, sizeof(in_buf), 0);
    if (recv_bytes == -1 || recv_bytes == 0)
        handle_error_client("1st recv error", id);
        
    printf("Client %i: %s\n", id, in_buf);
    close(fd);  // close coonection to main server
    // make new server path
    sprintf(server_path, "/tmp/tcp_server_%i", atoi(in_buf));

    // connect to new server
    fd = connect_server(AF_UNIX, SOCK_STREAM, server_path);

    if (send(fd, "Hi", sizeof("Hi"), MSG_NOSIGNAL) == -1)
        handle_error_client("2nd send error", id); 
    while (1) {
        int recv_bytes = recv(fd, in_buf, sizeof(in_buf), 0);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Client №%i is closed\n", id);
                break;
        }
        printf("[MSG]: %s\n", in_buf);
    }
    exit(0);
}
void handle_error_client(char *msg, int id)
{
    char temp[256] = {};
    sprintf(temp, "id[%i]: %s", id, msg);
    perror(temp); exit(EXIT_FAILURE);
}
void handle_error_info(char *msg, char *info)
{
    char temp[256] = {};
    sprintf(temp, "<%s> : %s", info, msg);
    perror(temp); 
    exit(EXIT_FAILURE);
}
