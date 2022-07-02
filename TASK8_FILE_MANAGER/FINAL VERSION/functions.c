#include "functions.h"

int nfiles = 0;         //кол-во файлов в директории
int prev_nfiles = 0;    //кол-во файлов в пред.директории
char **name_list;       //массив имен каталога
char **path_list;       //массив полных адресов каталога

int maxx;               //максимальный размер окна по x
int maxy;               //макс. размер окна по y
int key;                //нажатая клавиша
int highlight = 1;      //переменная для подсветки тек. позиции в списке
int print_start = 0;    //перем. для прокрутки экрана
char cur_dir[MAX_PATH]; //тек. директория
char prev_dir[MAX_PATH];//пред. директория

WINDOW * name_field;        //основное окно
WINDOW * sub_name_field;    //внутреннее окно для отображения текста

void allocate() 
{
    //-------Allocate memory---------
    if (!(name_list = (char**)malloc(nfiles * sizeof(char*)))) {
        fprintf(stderr, "Allocation error! name_list\n");
        exit(1);
    }
    if (!(path_list = (char**)malloc(nfiles * sizeof(char*)))) {
        fprintf(stderr, "Allocation error! name_list\n");
        exit(1);
    }
    for (int i = 0; i < nfiles; i++) 
        if (!(name_list[i] = (char*)malloc(MAX_PATH*sizeof(char)))) {
            fprintf(stderr, "Allocation error! name_list[i]\n");
            exit(1);
        }
    for (int i = 0; i < nfiles; i++) 
        if (!(name_list[i] = (char*)malloc(MAX_NAME*sizeof(char)))) {
            fprintf(stderr, "Allocation error! name_list[i]\n");
            exit(1);
        }
}
void free_mem()
{
    for (int i = 0; i < nfiles; i++) {
        free(name_list[i]);
        free(path_list[i]);
    }
    free(name_list);
    free(path_list);
}
void empty_stdin() 
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void dirwalk(char *dir)
{
    char path[MAX_PATH];
    char name[MAX_NAME];
    nfiles = 0;
    struct dirent *dp;
    DIR *dfd;

    if ((dfd = opendir(dir)) == NULL) {
    	printf("dirwalk: не могу открыть %s\n", dir);
        // fprintf(stderr, "dirwalk: не могу открыть %s\n", dir);
        return;
    }
    while ((dp = readdir(dfd)) != NULL) {           //пока есть файлы в каталоге
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;                          //пропустить себя и родителя
        if (strlen(dir) + strlen(dp->d_name) + 2 > sizeof(path))
            fprintf(stderr, "dirwalk: слишком длинное имя %s/%s\n", 
                dir, dp->d_name);
        else {
            nfiles++;                                   //подсчет кол-ва файлов
            sprintf(path, "%s/%s", dir, dp->d_name);    //форматирование строки адреса
            strcpy(name, dp->d_name);                   //сохранение имени файла
            name_list = resize_list(name_list);         //увеличение массива
            strcpy(name_list[nfiles - 1], name);        //сохранение в массив
            path_list = resize_list(path_list);
            strcpy(path_list[nfiles - 1], path);
            prev_nfiles = nfiles;                       //сохранение пред. кол-ва файлов
        }
    }
   	closedir(dfd);                                      //закрыть директорию
}
char **resize_list(char **list)
{
    if (!(list = (char**)realloc(list, nfiles * sizeof(char*)))) {
        fprintf(stderr, "Reallocation error! name_list\n");
        exit(1);
    }
    if (prev_nfiles < nfiles)                           //если файлов стало больше - добавить записи
        for (int i = prev_nfiles; i < nfiles; i++) 
        if (!(list[i] = (char*)malloc(MAX_PATH*sizeof(char)))) {
            fprintf(stderr, "Allocation error! name_list\n");
            exit(1);
        }
    else if (prev_nfiles > nfiles) {                    //если меньше - освободить память
        for (int i = nfiles; i < prev_nfiles; i++) 
            free(name_list[i]);
    }
    return list;  
}

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получение новых размеров
	resizeterm(size.ws_row, size.ws_col);              //изменение размера терминала
	maxx = size.ws_col;                                //сохранение переменных
	maxy = size.ws_row;
	windows_resize();                                  //обновление размеров окон
    print(print_start);                                //обновить текст
	windows_refresh();                                 //обновить окна
}
void windows_init()
{
	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();                          //частичный контроль над клавой
	getmaxyx(stdscr, maxy, maxx);      //получить макс.размеры окна
	curs_set(FALSE);                   //откл. курсор
	start_color();                     //иниц. цвета
	refresh();                         //обновить полностью
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);    //новая цветовая пара текст/фон
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	attron(COLOR_PAIR(1) | A_BOLD);            //применить цвет. пару фоновому окну
//--------------NAME FIELD--------------------
	name_field = newwin(maxy, maxx, 0, 0);     //создание окна
	wbkgd(name_field, COLOR_PAIR(2));          //фоновые цвета
	box(name_field, '|', '-');                 //рамка

	sub_name_field = derwin(name_field, maxy - 1, maxx - 1, 0, 1); //суб-окно, чтобы текст не переписывал рамку
	keypad(sub_name_field, true);                  //включить клаву
	wbkgd(sub_name_field, COLOR_PAIR(2));

	getmaxyx(stdscr, maxy, maxx);
	//wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
	wrefresh(sub_name_field);
	wrefresh(name_field);
}

void windows_del()
{
	delwin(sub_name_field);
	delwin(name_field);
    endwin();
}
void windows_resize()
{
	wclear(name_field);
	wclear(sub_name_field);
	wresize(name_field, maxy, maxx);
	wresize(sub_name_field, maxy - 1, maxx - 1);
	box(name_field, '|', '-');
    refresh();
	
}
void windows_refresh()
{
	//wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
	wrefresh(sub_name_field);
	wrefresh(name_field);
}
void print(int start)
{
    int row = 0;                    //переменная для хранение №тек. строки
    wclear(sub_name_field);
    wattron(sub_name_field, COLOR_PAIR(1) | A_BOLD);    //желтая строка с тек. адресом
    mvwprintw(sub_name_field, 1, 1, "CURRENT DIRECTORY: %s; Files: %i",cur_dir, nfiles);
    wattroff(sub_name_field, A_BOLD);
    wattron(sub_name_field, COLOR_PAIR(2));             //вернуть цвет обратно
 //   mvwprintw(sub_name_field, 1, 50, "n = %i; s = %i; y = %i; h = %i", nfiles, start, maxy, highlight);
    for (int i = start; i < nfiles; i++) {          //вывод списка и подсветка
            if (row == highlight - 1)
                wattron(sub_name_field, A_REVERSE);
            mvwprintw(sub_name_field, row + 2, 1, name_list[i]);
            wattroff(sub_name_field, A_REVERSE);
            row++;
    }
    windows_refresh();
}