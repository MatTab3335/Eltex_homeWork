#!/учеба си/Eltex_homeWork/TASK5_Calculator
#Скрипт для создания статической библиотеки,компилирования файла калькулятора и создания исполняемого файла

ar rc libfunctions.a functions.o
ranlib libfunctions.a
gcc main.c -L. -lfunctions -o Calculator_static