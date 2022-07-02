all: Calculator

Calculator: main.o functions.o
	gcc main.o functions.o -o Calculator

main.o: main.c
	gcc -c main.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm -rf *.0 Calculator
