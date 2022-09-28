all: 2nd_scheme_tcp

2nd_scheme_tcp: tcp_server.o functions.o
	gcc tcp_server.o functions.o -o 2nd_scheme_tcp -lpthread

tcp_server.o: tcp_server.c
	gcc -c tcp_server.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 2nd_scheme_tcp