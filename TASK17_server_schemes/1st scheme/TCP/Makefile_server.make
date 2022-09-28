all: 1st_scheme_tcp

1st_scheme_tcp: functions.o tcp_server.o 
	gcc functions.o tcp_server.o -o 1st_scheme_tcp -lpthread

tcp_server.o: tcp_server.c
	gcc -c tcp_server.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 1st_scheme_tcp
