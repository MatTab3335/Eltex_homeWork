all: 1st_scheme_udp_server

1st_scheme_udp_server: functions.o udp_server.o 
	gcc functions.o udp_server.o -o 1st_scheme_udp_server -lpthread

udp_server.o: udp_server.c
	gcc -c udp_server.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 1st_scheme_udp_server
