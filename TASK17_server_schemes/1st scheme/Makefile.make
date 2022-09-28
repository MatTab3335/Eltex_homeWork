all: 1st_scheme_tcp

1st_scheme_tcp: tcp_server.o functions_server.o
	gcc tcp_server.o functions_server.o -o 1st_scheme_tcp -lpthread

tcp_server.o: tcp_server.c
	gcc -c tcp_server.c

functions_server.o: functions_server.c
	gcc -c functions_server.c

clean:
	rm -rf *.0 1st_scheme_tcp