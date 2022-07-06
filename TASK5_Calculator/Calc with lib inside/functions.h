#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>

int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
double divide(double a, double b);
void empty_stdin();

extern int a;
extern int b; 
extern char command;
extern char integer[11];

#endif /* FUNCTIONS_H_ */
