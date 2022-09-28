all: client

client: server_functions.o unix_client.o 
	gcc server_functions.o unix_client.o -o client -lpthread

unix_client.o: unix_client.c
	gcc -c unix_client.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 client
