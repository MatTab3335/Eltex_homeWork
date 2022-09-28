all: 1st_scheme_tcp

1st_scheme_tcp: server_functions.o tcp_server.o 
	gcc server_functions.o tcp_server.o -o 1st_scheme_tcp -lpthread

tcp_server.o: tcp_server.c
	gcc -c tcp_server.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 1st_scheme_tcp
