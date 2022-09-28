all: udp_client

udp_client: server_functions.o udp_client.o 
	gcc server_functions.o udp_client.o -o udp_client -lpthread

udp_client.o: udp_client.c
	gcc -c udp_client.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 udp_client
