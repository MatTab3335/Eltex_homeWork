//tcp clients - scheme #1
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define MAIN_SERVER_PATH "/tmp/tcp_server"
#define LISTEN_BACKLOG 5
#define THR_N 1000



void SignalHandler(int signal);
void *thread_func(void *input);
void handle_error(char *msg, int id);

int fd;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
    pthread_t thr_list[THR_N];
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);

    for (int i = 0; i < THR_N; i++) {
        pthread_create(&thr_list[i], NULL, thread_func, (void *) &i);
        usleep(70000);
    }
}

void SignalHandler(int signal)
{	
	close(fd);
	printf("Finish client\n");
	exit(0);
}
void *thread_func(void *input)
{
    pthread_mutex_lock(&m1);
    int id = *((int *)input);
    pthread_mutex_unlock(&m1);
    
    printf("Thread #%i is created\n", id);
    
    struct sockaddr_un server_addr;
    char in_buf[256] = {};
    char server_path[256] = {};
    
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        handle_error("1st socket", id);

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, MAIN_SERVER_PATH,
            sizeof(server_addr.sun_path) - 1);
    
    //ont thread per time to connect to main server        
    pthread_mutex_lock(&m2);
    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)))
    	handle_error("1st connect error", id);
    pthread_mutex_unlock(&m2);
        
    int recv_bytes = recv(fd, in_buf, sizeof(in_buf), 0);
    if (recv_bytes == -1 || recv_bytes == 0)
    	handle_error("1st recv error", id);
        
    printf("Client %i: %s\n", id, in_buf);
    close(fd);

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        handle_error("2nd socket", id);

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    server_addr.sun_family = AF_UNIX;

    sprintf(server_path, "/tmp/tcp_server_%i", atoi(in_buf));
    strncpy(server_addr.sun_path, server_path,
            sizeof(server_addr.sun_path) - 1);
    //printf("server_path = %s\n", server_path);

    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)))
    	handle_error("2nd connect error", id);

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
    exit(0);
}
void handle_error(char *msg, int id)
{
    char temp[256] = {};
    sprintf(temp, "id[%i]: %s", id, msg);
    perror(temp); exit(EXIT_FAILURE);
}
