all: client

client: functions.o tcp_client.o 
	gcc functions.o tcp_client.o -o client -lpthread

tcp_client.o: tcp_client.c
	gcc -c tcp_client.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 client
