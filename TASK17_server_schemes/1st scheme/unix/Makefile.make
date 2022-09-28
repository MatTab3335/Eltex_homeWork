all: 1st_scheme_unix

1st_scheme_unix: server_functions.o unix_server.o 
	gcc server_functions.o unix_server.o -o 1st_scheme_unix -lpthread

unix_server.o: unix_server.c
	gcc -c unix_server.c

server_functions.o: server_functions.c
	gcc -c server_functions.c

clean:
	rm -rf *.0 1st_scheme_unix
