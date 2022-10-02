//receiver for MULTICAST

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define PORT 7777
#define IP "224.0.0.1"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int udp_s;
    struct sockaddr_in endpoint_addr;
    struct ip_mreqn mreqn;
    int endpoint_addr_size = sizeof(struct sockaddr_in);
    char in_buf[256] = {};

    udp_s = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_s == -1)
        handle_error("socket");

    memset(&endpoint_addr, 0, sizeof(struct sockaddr_in));

    endpoint_addr.sin_family = AF_INET;
    endpoint_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    endpoint_addr.sin_port = htons(PORT);

    if (bind(udp_s, (struct sockaddr *) &endpoint_addr,
            sizeof(struct sockaddr_in)) == -1)
        handle_error("bind");

    mreqn.imr_multiaddr.s_addr = inet_addr(IP);
    mreqn.imr_address.s_addr = htonl(INADDR_ANY);
    mreqn.imr_ifindex = 0;

    if (setsockopt(udp_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn)) < 0)
        handle_error("setsockopt");

    while(1) {

    	recvfrom(udp_s, in_buf, sizeof(in_buf), 0, 
                (struct sockaddr *) &endpoint_addr, &endpoint_addr_size);

    	printf("[MSG]: %s\n", in_buf);
    }

    

}