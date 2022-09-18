//server for AF_INET - SOCK_DGRAM

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

/*struct sockaddr_in {
    sa_family_t    sin_family; // address family: AF_INET 
    in_port_t      sin_port;   // port in network byte order 
    struct in_addr sin_addr;   // internet address 
};

// Internet address. 
struct in_addr {
    uint32_t       s_addr;     // address in network byte order 
};*/


int main(int argc, char *argv[])
{
    int udp_s_s, udp_s_c;
    struct sockaddr_in my_addr, client_addr;
    int client_addr_size = 0;
    char in_buf[256] = {};

    udp_s_s = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_s_s == -1)
        handle_error("socket");

    memset(&my_addr, 0, sizeof(struct sockaddr_in));
                        
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(SERVER_PORT);


    if (bind(udp_s_s, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_in)) == -1)
        handle_error("bind");



    client_addr_size = sizeof(struct sockaddr_in);

    recvfrom(udp_s_s, in_buf, sizeof(in_buf), MSG_WAITALL, 
                (struct sockaddr *) &client_addr, &client_addr_size);
    printf("[MSG]: %s\n", in_buf);
    
    sendto(udp_s_s, in_buf, sizeof(in_buf), MSG_CONFIRM,
                (struct sockaddr *) &client_addr, client_addr_size);

}