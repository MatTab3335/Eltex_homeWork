all: client

client: server_functions.o tcp_client.o 
	gcc server_functions.o tcp_client.o -o client -lpthread

tcp_client.o: tcp_client.c
	gcc -c tcp_client.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 client
