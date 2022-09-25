//server for AF_LOCAL - SOCK_STREAM

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
    socklen_t client_addr_size;
    char in_buf[256];

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        handle_error("socket");

    memset(&my_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, MY_SOCK_PATH,
            sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_un)) == -1)
        handle_error("bind");

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        handle_error("listen");
    printf("listen\n");

    /* Now we can accept incoming connections one
       at a time using accept(2) */

    client_addr_size = sizeof(struct sockaddr_un);
    cfd = accept(sfd, (struct sockaddr *) &client_addr,
                 &client_addr_size);
    if (cfd == -1)
        handle_error("accept");
    printf("new client\n");
    /* Code to deal with incoming connection(s)... */
    while (1) {
        recv(cfd, in_buf, sizeof(in_buf), 0);
        printf("[MSG]: %s\n", in_buf);
        send(cfd, in_buf, sizeof(in_buf), 0);
    }

    unlink(MY_SOCK_PATH);
    /* When no longer required, the socket pathname, MY_SOCK_PATH
       should be deleted using unlink(2) or remove(3) */
}