all: 1st_scheme_udp_server

1st_scheme_udp_server: server_functions.o udp_server.o 
	gcc server_functions.o udp_server.o -o 1st_scheme_udp_server -lpthread

udp_server.o: udp_server.c
	gcc -c udp_server.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 1st_scheme_udp_server
