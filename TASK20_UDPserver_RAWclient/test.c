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
	int s = socket (PF_INET, SOCK_RAW, IPPROTO_UDP);
	if(s == -1)
		handle_error("socket");
	
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
	iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(data);
	iph->id = htonl(54321);	//Id of this packet
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;		//Set to 0 before calculating checksum
	iph->saddr = inet_addr(source_ip);	//Spoof the source ip address
	iph->daddr = sin.sin_addr.s_addr;
	
	//Ip checksum
	iph->check = csum ((unsigned short *) datagram, 20);
	
	//UDP Header
	udph->source = htons(5005);
	udph->dest = htons(SERVER_PORT);
	udph->len = sizeof(struct udphdr) + strlen(data);
	printf("UDP header length = %i\n", udph->uh_ulen);
	udph->check = 0;
	
	//IP_HDRINCL to tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;
	
	if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
		handle_error("Error setting IP_HDRINCL");

	while (1)
	{
		//Send the packet
		if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
			perror("sendto failed");
		else
			printf ("Packet Send. Length : %d \n" , iph->tot_len);
        // sleep for 1 seconds
        sleep(1);
	}
	
	return 0;
}