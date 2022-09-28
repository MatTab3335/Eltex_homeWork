all: 2nd_scheme_tcp

2nd_scheme_tcp: unix_server.o server_functions.o
	gcc unix_server.o server_functions.o -o 2nd_scheme_tcp -lpthread

unix_server.o: unix_server.c
	gcc -c unix_server.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 2nd_scheme_tcp