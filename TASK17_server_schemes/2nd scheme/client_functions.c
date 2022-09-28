#include "client_functions.h"

static pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

void SignalHandler(int signal)
{	
	// close(fd);
	printf("Finish client\n");
	exit(0);
}
int connect_to_serv(int domain, int type, char *path)
{
	struct sockaddr_un server_addr;
    int fd;
    
    fd = socket(domain, type, 0);
    if (fd == -1)
        handle_error_info("1st socket", path);

    memset(&server_addr, 0, sizeof(struct sockaddr_un));

    server_addr.sun_family = domain;
    strncpy(server_addr.sun_path, path,
            sizeof(server_addr.sun_path) - 1);
    
    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)))
    	handle_error_info("1st connect error", path);
       
    return fd;
}
void *thread_func(void *input)
{
    int id = *((int *)input);
	
	int fd;
    char in_buf[256] = {};
    char server_path[256] = {};
    
    printf("Thread #%i is created\n", id);
    
    //ont thread per time to connect to main server        
    pthread_mutex_lock(&m1);
    fd = connect_to_serv(AF_UNIX, SOCK_STREAM, MAIN_SERVER_PATH);
    pthread_mutex_unlock(&m1);
        
    int recv_bytes = recv(fd, in_buf, sizeof(in_buf), 0);
    if (recv_bytes == -1 || recv_bytes == 0)
    	handle_error("1st recv error", id);
        
    printf("Client %i: %s\n", id, in_buf);
    close(fd);
    
    sprintf(server_path, "/tmp/tcp_server_%i", atoi(in_buf));
    // printf("server_path = %s\n", server_path);
    fd = connect_to_serv(AF_UNIX, SOCK_STREAM, server_path);
    
    if (send(fd, "Hi", sizeof("Hi"), MSG_NOSIGNAL) == -1)
    	handle_error("2nd send error", id); 
    	
    while (1) {
        int recv_bytes = recv(fd, in_buf, sizeof(in_buf), 0);
        if (recv_bytes == -1 || recv_bytes == 0) {
                printf("Client №%i is closed\n", id);
                break;
        }
        printf("[MSG]: %s\n", in_buf);
    }
    close(fd);
    exit(0);
}
void handle_error(char *msg, int id)
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
