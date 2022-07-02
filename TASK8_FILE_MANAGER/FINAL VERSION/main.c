#include "functions.h"

int main()
{
    struct stat info;                   //переменная для хранения инфы о каталоге
    allocate();                         //разместить в памяти массив для хранения имен и адресов
    //-------Init curses----------
    setlocale(LC_ALL, "ru_RU.UTF-8");
    windows_init();                     //инициализация окон

	//-------Go walk through directories--------
    strcpy(cur_dir, "/home/matvey");
    dirwalk(cur_dir);                   //зайти в директорию home/matvey
    
    while(1) {
        char temp[MAX_PATH] = {};       //вспомогательная переменная для обработки адреса возврата
        int i = 0;                      //вспомогательная переменная для обработки адреса возврата
        int remove = 0;                 //вспомогательная переменная для обработки адреса возврата
        print(print_start);             //напечатать содержимое каталога, начиная с print_start
        key = wgetch(sub_name_field);   //получение нажатой клавиши
        switch(key) {
            case KEY_DOWN: 
                highlight++;            //подсветить текущую позицию
                if ((highlight >= maxy - 2) || (highlight >= nfiles)   //если курсор стоит на последней записи на экране 
                    || (highlight >= (nfiles - print_start))) {
                    print_start++;                                      //прокрутка списка
                    highlight = (maxy - 2 > nfiles) ? nfiles:(maxy - 2); //чтобы подсветка не убегала с последней записи
                    highlight = (highlight > nfiles - print_start) 
                    ? nfiles - print_start:highlight;
                }
                break;
            case KEY_UP:
                highlight--;            //подсветить текущую позицию
                if (highlight <= 1) {   //если курсор на первой записи - оставить его там же
                    highlight = 1;
                    print_start--;
                    if (print_start <= 0)
                        print_start = 0;
                }   
                break;
            case KEY_RIGHT:
                stat(path_list[print_start + highlight - 1], &info);    //прежде чем зайти внутрь файла, проверить не является ли он обычным файлом, если да, то пропустить
                if (S_ISREG(info.st_mode))
                    break;
                strcpy(cur_dir, path_list[print_start + highlight - 1]);    //копирование текущего адреса в cur_dir для его отображения вверху жкрана
                dirwalk(path_list[print_start + highlight - 1]);            //зайти в директорию
                highlight = 1;
                print_start = 0;
                break;
            case KEY_LEFT:
                i = strlen(cur_dir);            //удаление из текущего адреса последнего имени, чтобы вернуться назад
                remove = 0;
                while (cur_dir[i] != '/') {
                    remove++;
                    i--;
                }
                strncpy(temp, cur_dir, strlen(cur_dir) - (size_t)remove);
                strcpy(cur_dir, temp);
                dirwalk(cur_dir);           //перейти в пред. директорию
                highlight = 1;
                print_start = 0;
                break;
            default:
                break;
        }
        

            
                
    }
    free_mem();                 //освободить память
    windows_del();              //освободить окна
    exit(EXIT_SUCCESS);
}

