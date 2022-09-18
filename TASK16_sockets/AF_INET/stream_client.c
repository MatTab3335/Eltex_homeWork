//client for AF_INET - SOCK_STREAM

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
    char in_buf[256] = {};

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        handle_error("socket");

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == -1)
        handle_error("inet_pton");

    connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));

    send(fd, "hi", 2, 0);
    recv(fd, in_buf, sizeof(in_buf), 0);
    printf("[MSG]: %s\n", in_buf);

    /* Code to deal with incoming connection(s)... */

    /* When no longer required, the socket pathname, MY_SOCK_PATH
       should be deleted using unlink(2) or remove(3) */
}