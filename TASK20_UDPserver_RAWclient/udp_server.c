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
    char in_buf[4096] = {};
    char *ptr;

    udp_s_s = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_s_s == -1)
        handle_error("socket");

    memset(&my_addr, 0, sizeof(struct sockaddr_in));
                        
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr("192.168.0.106");
    my_addr.sin_port = htons(SERVER_PORT);


    if (bind(udp_s_s, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_in)) == -1)
        handle_error("bind");



    client_addr_size = sizeof(struct sockaddr_in);
                
    while(1) {
        recvfrom(udp_s_s, in_buf, sizeof(in_buf), 0, 
                (struct sockaddr *) &client_addr, &client_addr_size);
        ptr = in_buf;
        printf("[MSG]: %s\n", ptr);
        sendto(udp_s_s, "Accepted", sizeof("Accepted"), 0, 
                (struct sockaddr *) &client_addr, client_addr_size);
    }

}
