//tcp clients - scheme #1
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define MAIN_SERVER_PATH "/tmp/tcp_server"
#define LISTEN_BACKLOG 5

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void SignalHandler(int signal);

int fd;

int main(int argc, char *argv[])
{
    
    struct sockaddr_un server_addr;
    char in_buf[256] = {};
    char server_path[256] = {};

    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);

    // sprintf(sock_path, "%s%i", MY_SOCK_PATH, getpid());
    // printf("%s\n", sock_path);

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        handle_error("socket");

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, MAIN_SERVER_PATH,
            sizeof(server_addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)))
    	handle_error("connect error");

    if (recv(fd, in_buf, sizeof(in_buf), MSG_WAITALL) == -1)
    	handle_error("recv error");
    printf("Client %i: %s\n", fd, in_buf);

    close(fd);

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        handle_error("socket");

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    server_addr.sun_family = AF_UNIX;

    sprintf(server_path, "/tmp/tcp_server_%i", atoi(in_buf));
    strncpy(server_addr.sun_path, server_path,
            sizeof(server_addr.sun_path) - 1);
    printf("server_path = %s\n", server_path);

    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)))
    	handle_error("connect error");

    if (send(fd, "Hi", sizeof("Hi"), 0) == -1)
    	handle_error("send error"); 
    while (1) {
	    if (recv(fd, in_buf, sizeof(in_buf), 0) == -1)
	    	handle_error("recv error"); 
	    printf("[MSG]: %s\n", in_buf);
    }
    

}

void SignalHandler(int signal)
{	
	close(fd);
	printf("Finish client\n");
	exit(0);
}