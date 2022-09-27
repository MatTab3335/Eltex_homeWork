all: 2nd_scheme_tcp

2nd_scheme_tcp: tcp_server.o server_functions.o
	gcc tcp_server.o server_functions.o -o 2nd_scheme_tcp -lpthread

tcp_server.o: tcp_server.c
	gcc -c tcp_server.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 2nd_scheme_tcp