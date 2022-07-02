#include "functions.h"

int main()
{
    while(1) {
        printf("\n-----------What do you wanna do?------------\n");
        printf("List of commands:\n");
        printf("'+' - add two numbers;\n'-' - subtract two numbers;\n");
        printf("'*' - multiply two numbers;\n'/' - divide two numbers;\n");
        printf("'q' - to quit\n");
        printf("Your command: ");
        command = getchar();
        empty_stdin();
        if (command == 'q')
            return 0;
        printf("Enter first number: ");
        fgets(integer, 11, stdin);
        a = atoi(integer);
        printf("Enter second number: ");
        fgets(integer, 11, stdin);
        b = atoi(integer);
        switch (command)
        {
            case 43: printf("%i + %i = %i", a, b, add(a, b));
                break;
            case 45: printf("%i - %i = %i", a, b, subtract(a, b));
                break;
            case 42: printf("%i * %i = %i", a, b, multiply(a, b));
                break;
            case 47: printf("%i / %i = %lf", a, b, divide(a, b));
                break;
        }
    }
    return 0;
}
