#include "functions.h"

int a = 0;
int b = 0; 
char command;
char integer[11];

int add(int a, int b)
{
    return a + b;
}

int subtract(int a, int b)
{
    return a - b;
}

int multiply(int a, int b)
{
    return a * b;
}

double divide(double a, double b)
{
    return a / b;
}
void empty_stdin() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}