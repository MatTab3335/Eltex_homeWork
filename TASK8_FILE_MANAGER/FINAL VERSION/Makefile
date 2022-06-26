all: file_manager

file_manager: main.o functions.o
	gcc main.o functions.o -o file_manager -lncursesw

main.o: main.c
	gcc -c main.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 file_manager
