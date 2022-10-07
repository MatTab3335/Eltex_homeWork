#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
	int fd;
	char buf[2056] = {};
	char *ptr = buf;
	struct sockaddr_in addr;
	
	fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (fd == -1)
        handle_error("socket");
        
	memset(&addr, 0, sizeof(struct sockaddr_in));
	int addr_size = sizeof(struct sockaddr_in);
	
	while(1) {
		static int i = 0;
		static int recvbytes;
		recvbytes = recvfrom(fd, buf, sizeof(buf), 0, 
                (struct sockaddr *) &addr, &addr_size);
        if (recvbytes < 0) handle_error("recv");
        ptr = buf + 28;
        printf("Bytes =  %i\n", recvbytes);
        printf("[MSG %i] %s\n", i++, ptr);
	}
	exit(0);
}
