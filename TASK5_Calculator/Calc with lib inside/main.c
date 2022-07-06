#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main()
{
    void *handle;
    char *error;
    int (*func)(int, int);
    double (*double_func)(double, double);
    void (*void_func)();

    handle = dlopen("/home/matvey/Рабочий стол/Ссылка на Eltex_homeWork/TASK5_Calculator/libfunctionsdyn.so"
        , RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();  /* Clear any existing error */
    while(1) {
        char command;
        char integer[10];
        int a = 0;
        int b = 0; 

        printf("\n-----------What do you wanna do?------------\n");
        printf("List of commands:\n");
        printf("'+' - add two numbers;\n'-' - subtract two numbers;\n");
        printf("'*' - multiply two numbers;\n'/' - divide two numbers;\n");
        printf("'q' - to quit\n");
        printf("Your command: ");
        command = getchar();
        void_func = dlsym(handle, "empty_stdin"); 
        (*void_func)();
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
            case 43:
                func = dlsym(handle, "add"); 
                printf("%i + %i = %i", a, b, (*func)(a, b));
                break;
            case 45: 
                func = dlsym(handle, "subtract"); 
                printf("%i - %i = %i", a, b, (*func)(a, b));
                break;
            case 42: 
                func = dlsym(handle, "multiply");
                printf("%i * %i = %i", a, b, (*func)(a, b));
                break;
            case 47: 
                double_func = dlsym(handle, "divide");
                printf("%i / %i = %lf", a, b, (*double_func)(a, b));
                break;
        }
    }
    dlclose(handle);
    return 0;
}
