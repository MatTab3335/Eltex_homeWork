#include <stdio.h>	
#include <string.h> 
#include <sys/socket.h>	
#include <stdlib.h> 
#include <errno.h> 
#include <netinet/udp.h>	//Provides declarations for tcp header
#include <netinet/ip.h>		//Provides declarations for ip header
#include <arpa/inet.h> 		// inet_addr
#include <unistd.h> 		// sleep()

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define SERVER_PORT 5000

int main (void)
{
	//Create a raw socket
	int s = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);
	if(s == -1)
		handle_error("socket");
	
	//Datagram to represent the packet
	char datagram[4096], *data;
	
	//zero out the packet buffer
	memset (datagram, 0, 4096);
		
	//UDP header
	struct udphdr *udph = (struct udphdr *)datagram;
	struct sockaddr_in sin;

	//Data part
	data = datagram + sizeof(struct udphdr);
	strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	
	//some address resolution
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.s_addr = inet_addr ("127.0.0.1");
		
	//UDP Header
	udph->source = htons(5005);
	udph->dest = htons(SERVER_PORT);
	udph->len = sizeof(struct udphdr) + strlen(data);
	printf("UDP header length = %i\n", udph->len);
	udph->check = 0;		//no need in IPv4
	
	

	while (1)
	{
		//Send the packet
		if (sendto(s, datagram, 20+8+strlen(data),	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
			perror("sendto failed");
		else
			printf ("Packet Send \n");
        // sleep for 1 seconds
        sleep(1);
	}
	
	return 0;
}
