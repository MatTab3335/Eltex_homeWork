#include <stdio.h>	
#include <string.h> 
#include <sys/socket.h>	
#include <stdlib.h> 
#include <errno.h> 
#include <linux/if_packet.h>
#include <netinet/udp.h>	//Provides declarations for tcp header
#include <netinet/ip.h>		//Provides declarations for ip header
#include <netinet/ether.h>	//Provides declarations for ethernet header
#include <net/if.h>
#include <arpa/inet.h> 		// inet_addr
#include <unistd.h> 		// sleep()

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define SERVER_PORT 5000

#define MY_IP "192.168.0.105"
#define MY_MAC "00:d8:61:88:7c:4a"

#define SERV_IP "192.168.0.103"
#define SERV_MAC "b8:27:eb:d6:37:21"

int csum(struct iphdr *buf);

int main (void)
{
	//Create a raw socket
	int s = socket (AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(s == -1)
		handle_error("socket");
	
	//Datagram to represent the packet
	char datagram[4096], *data;
	
	//zero out the packet buffer
	memset (datagram, 0, 4096);
	
	//ethernet header
	struct ether_header *ethh = (struct ether_header *)(datagram);
	memset (ethh, 0, sizeof(struct ether_header));
	struct ether_addr *eth_addr;
	//IP header
	struct iphdr *iph = (struct iphdr *)(datagram + sizeof(struct ether_header));
	memset (iph, 0, sizeof(struct iphdr));
	//UDP header
	struct udphdr *udph = (struct udphdr *)(datagram + sizeof(struct ether_header) 
							+ sizeof(struct iphdr));
	memset (udph, 0, sizeof(struct udphdr));
	struct sockaddr_ll addr_ll;
	memset (&addr_ll, 0, sizeof(struct sockaddr_ll));

	
	//Data part
	data = datagram + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr);
	strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	
	//some address resolution
	addr_ll.sll_family = htons(AF_PACKET);
	addr_ll.sll_ifindex = htonl(if_nametoindex("enp3s0"));
	addr_ll.sll_halen = htons(6);
	addr_ll.sll_protocol = htons(ETHERTYPE_IP);
	eth_addr = ether_aton(SERV_MAC);
	for (int i = 0; i < ETH_ALEN; i++) 
		addr_ll.sll_addr[i] = eth_addr->ether_addr_octet[i];

	printf("Address:\n");
	printf("addr_ll.sll_family: %X\n", addr_ll.sll_family);
	printf("addr_ll.sll_ifindex: %X\n", addr_ll.sll_ifindex);
	printf("addr_ll.sll_halen: %X\n", addr_ll.sll_halen);
	printf("addr_ll.sll_protocol: %X\n", addr_ll.sll_protocol);
	printf("addr_ll.sll_addr: ");
	for (int i = 0; i < ETH_ALEN; i++)
		printf("%X", addr_ll.sll_addr[i]);
	
	//Ethernet header
	for (int i = 0; i < ETH_ALEN; i++) 
		ethh->ether_dhost[i] = eth_addr->ether_addr_octet[i];

	eth_addr = ether_aton(MY_MAC);
	for (int i = 0; i < ETH_ALEN; i++) 
		ethh->ether_shost[i] = eth_addr->ether_addr_octet[i];

	ethh->ether_type = htons(ETHERTYPE_IP);

	//Fill in the IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr)); 
	iph->id = htons(1234);
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;		
	iph->saddr = inet_addr(MY_IP);		
	iph->daddr = inet_addr(SERV_IP);
	
	
	//UDP Header
	udph->source = htons(5005);
	udph->dest = htons(SERVER_PORT);
	udph->len = htons(sizeof(struct udphdr) + strlen(data));
	udph->check = 0;		//no need in IPv4
	
	// calculate the checksum for integrity
  	iph->check = csum(iph);
  	printf("\nchecksum = %X\n", iph->check);

	while (1)
	{
		//Send the packet
		if (sendto(s, datagram, strlen(datagram),0, (struct sockaddr *) &addr_ll, 
					sizeof (struct sockaddr_ll)) < 0)
			perror("sendto failed");
		else
			printf ("Packet Send \n");
        // sleep for 1 seconds
        sleep(1);
	}
	
	return 0;
}

int csum(struct iphdr *buf)
{
	int sum = 0;
	short *ptr = (short *)buf;

	for (int i = 0; i < 10; i++) {
		sum = sum + ptr;
		ptr++;
	}
	int tmp = sum >> 16;
	sum = (sum & 0xFFFF) + tmp;
	sum = ~sum;

	return sum;
}