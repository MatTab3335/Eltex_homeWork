all: client

client: tcp_client.o functions.o
	gcc tcp_client.o functions.o -o client -lpthread -lrt

tcp_client.o: tcp_client.c
	gcc -c tcp_client.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 client
