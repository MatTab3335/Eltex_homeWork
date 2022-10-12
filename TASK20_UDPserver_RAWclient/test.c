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

unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

int main (void)
{
	//Create a raw socket
	int s = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);
	if(s == -1)
		handle_error("socket");
		
	//IP_HDRINCL to tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;
	
	if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
		handle_error("Error setting IP_HDRINCL");
	
	//Datagram to represent the packet
	char datagram[4096] , source_ip[32] , *data , *pseudogram;
	
	//zero out the packet buffer
	memset (datagram, 0, 4096);
	
	//IP header
	struct iphdr *iph = (struct iphdr *) datagram;
	
	//UDP header
	struct udphdr *udph = (struct udphdr *)(datagram + sizeof (struct iphdr));
	struct sockaddr_in sin;

	//Data part
	data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
	strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	
	//some address resolution
	strcpy(source_ip , "127.0.0.1");
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.s_addr = inet_addr ("127.0.0.1");
	
	//Fill in the IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = 0; 	//Set to 0 to let system fill it
	iph->id = 0;		//Set to 0 to let system fill it
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;		//Set to 0 to let system fill it
	iph->saddr = 0;		//Set to 0 to let system fill it
	iph->daddr = sin.sin_addr.s_addr;
	
	
	//UDP Header
	udph->source = htons(5005);
	udph->dest = htons(SERVER_PORT);
	udph->len = sizeof(struct udphdr) + strlen(data);
	printf("UDP header length = %i\n", udph->uh_ulen);
	udph->check = 0;
	
	

	while (1)
	{
		//Send the packet
		if (sendto (s, datagram, sizeof(datagram),	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
			perror("sendto failed");
		else
			printf ("Packet Send. Length : %d \n" , iph->tot_len);
        // sleep for 1 seconds
        sleep(1);
	}
	
	return 0;
}
