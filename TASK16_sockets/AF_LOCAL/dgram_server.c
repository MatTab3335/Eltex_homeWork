//server for AF_LOCAL - DGRAM

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MY_SOCK_PATH "/tmp/my_socket_server"
#define LISTEN_BACKLOG 5

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int sfd, cfd;
    struct sockaddr_un my_addr, client_addr;
    socklen_t client_addr_size = 0;
    char in_buf[256] = {};

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        handle_error("socket");

    memset(&my_addr, 0, sizeof(struct sockaddr_un));
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, MY_SOCK_PATH,
            sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_un)) == -1)
        handle_error("bind");

    client_addr_size = sizeof(struct sockaddr_un);

    int bytes = recvfrom(sfd, in_buf, sizeof(in_buf), MSG_WAITALL, 
                (struct sockaddr *) &client_addr, &client_addr_size);

    printf("[MSG]: %s\n", in_buf);

    int n = sendto(sfd, in_buf, bytes, 0,
                (struct sockaddr *) &client_addr, client_addr_size);

    printf("n = %i\n", n);

    unlink(MY_SOCK_PATH);
}