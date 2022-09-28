all: client

client: unix_client.o client_functions.o
	gcc unix_client.o client_functions.o -o client -lpthread

unix_client.o: unix_client.c
	gcc -c unix_client.c

client_functions.o: client_functions.c
	gcc -c client_functions.c

clean:
	rm -rf *.0 client
