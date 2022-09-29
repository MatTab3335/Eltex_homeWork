all: client

client: udp_client.o functions.o
	gcc udp_client.o functions.o -o client -lpthread

udp_client.o: udp_client.c
	gcc -c udp_client.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 client
