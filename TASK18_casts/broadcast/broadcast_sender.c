//UDP sender for BROADCAST

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define SERVER_PORT 7777
#define IP "255.255.255.255"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int udp_s_s;
    struct sockaddr_in endpoint_addr;
    int endpoint_addr_size = sizeof(struct sockaddr_in);
    char out_buf[256] = {};

    udp_s_s = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_s_s == -1)
        handle_error("socket");

    int flag = 1;
    if (setsockopt(udp_s_s, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) < 0)
        handle_error("setsockopt");

    memset(&endpoint_addr, 0, sizeof(struct sockaddr_in));
                        
    endpoint_addr.sin_family = AF_INET;
    endpoint_addr.sin_addr.s_addr = inet_addr(IP);
    endpoint_addr.sin_port = htons(SERVER_PORT);

    while (1) {

        static int i = 0;
        int sent_bytes = 0;

        sprintf(out_buf, "%i", i);

        sent_bytes = sendto(udp_s_s, out_buf, sizeof(out_buf), 0,
                (struct sockaddr *) &endpoint_addr, endpoint_addr_size);
        if (sent_bytes < 0) handle_error("send error");

        printf("%i\n", i);
        i++;
        usleep(500000);
    }
    
}