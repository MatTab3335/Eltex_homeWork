all: 2nd_scheme_udp

2nd_scheme_udp: udp_server.o functions.o
	gcc udp_server.o functions.o -o 2nd_scheme_udp -lpthread

udp_server.o: udp_server.c
	gcc -c udp_server.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 2nd_scheme_udp
