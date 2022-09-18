//client for AF_LOCAL - SOCK_STREAM

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MY_SOCK_PATH "/tmp/my_socket_server"
#define LISTEN_BACKLOG 5

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_un server_addr;
    char in_buf[256];

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        handle_error("socket");

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, MY_SOCK_PATH,
            sizeof(server_addr.sun_path) - 1);

    connect(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un));

    send(fd, "hi", 2, 0);
    recv(fd, in_buf, sizeof(in_buf), 0);
    printf("[MSG]: %s\n", in_buf);

    /* Code to deal with incoming connection(s)... */

    /* When no longer required, the socket pathname, MY_SOCK_PATH
       should be deleted using unlink(2) or remove(3) */
}