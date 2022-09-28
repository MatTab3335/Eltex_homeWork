all: client

client: tcp_client.o client_functions.o
	gcc tcp_client.o client_functions.o -o client -lpthread

tcp_client.o: tcp_client.c
	gcc -c tcp_client.c

client_functions.o: client_functions.c
	gcc -c client_functions.c

clean:
	rm -rf *.0 client
