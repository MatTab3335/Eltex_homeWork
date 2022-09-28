all: udp_client

udp_client: functions.o udp_client.o 
	gcc functions.o udp_client.o -o udp_client -lpthread

udp_client.o: udp_client.c
	gcc -c udp_client.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 udp_client
