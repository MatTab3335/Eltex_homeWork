@CHCP 1251>NUL
gcc -c main.c
gcc -c functions.c
gcc main.o functions.o -o calculator
