all: manager

manager: manager.o functions.o
	gcc manager.o functions.o -o manager

main.o: manager.c
	gcc -c manager.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 manager