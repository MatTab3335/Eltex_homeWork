#!/учеба си/Eltex_homeWork/TASK5_Calculator
#Скрипт для создания динамической библиотеки,компилирования файла калькулятора и создания исполняемого файла

gcc functions.c -fPIC -c
gcc -shared functions.o -o libfunctionsdyn.so

#добавить каталог с библиотекой:
#LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH ./Calculator_dyn