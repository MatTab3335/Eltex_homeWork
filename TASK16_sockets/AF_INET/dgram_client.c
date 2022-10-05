//client for AF_INET - SOCK_DGRAM

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define SERVER_PORT 5000
#define LISTEN_BACKLOG 5

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_in server_addr;
    int server_addr_size = 0;
    char in_buf[256] = {};

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        handle_error("socket");

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    server_addr_size = sizeof(server_addr);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == -1)
        handle_error("inet_pton");

    sendto(fd, "Hi from client", sizeof("Hi from client"), MSG_CONFIRM,
                (struct sockaddr *) &server_addr, server_addr_size);

    recvfrom(fd, in_buf, sizeof(in_buf), MSG_WAITALL, 
                (struct sockaddr *) &server_addr, &server_addr_size);

    printf("[MSG]: %s\n", in_buf);
    
    while(1) {
        static int i = 0;
        char out_buf[256] = {};
        
        sprintf(out_buf, "%i", i);
        sendto(fd, out_buf, sizeof(out_buf), 0,
                (struct sockaddr *) &server_addr, server_addr_size);
        usleep(500000);
        i++;
    }

    /* Code to deal with incoming connection(s)... */

    /* When no longer required, the socket pathname, MY_SOCK_PATH
       should be deleted using unlink(2) or remove(3) */
}
