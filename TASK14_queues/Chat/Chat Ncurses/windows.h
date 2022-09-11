#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

//--------CURSES-------
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>

int maxx;               //максимальный размер окна по x
int maxy;               //макс. размер окна по y
int key;                //нажатая клавиша
int print_start = 0;    //перем. для прокрутки экрана

WINDOW * main_window;        //основное окно
WINDOW * clients_window;    //внутреннее окно для отображения списка клиентов
WINDOW * sub_clients_window;
WINDOW * chat_window;       //внутреннее окно для чата
WINDOW * sub_chat_window;
WINDOW * msg_window;
WINDOW * sub_msg_window;

void windows_init();        //инициализация окон
void windows_del();         //удаление окон
void windows_resize();      //обновление размеров окон
void windows_refresh();     //обновление содержимого окон
void windows_clear();       //очистить все окна
void sig_winch(int signo);  //обработка сигнала по изм-ию размера
void print_auto_row(WINDOW *window, char *s);      //вывод содержимого массивов в окно
void print(WINDOW *window, char *s, int row);

/*int main()
{
    windows_init();
    print(chat_window, "Hi");
    char s[256] = {};
    while(1) {
        wgetstr(msg_window, s);
        wclear(msg_window);
        box(msg_window, '|', '-');
        wmove(msg_window, 1, 1);
        print(chat_window, s);
        //mvwprintw(msg_window, 1, x, c);
    }
    getc(stdin);
    windows_del();
    exit(0);
}*/

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получение новых размеров
    resizeterm(size.ws_row, size.ws_col);              //изменение размера терминала
    maxx = size.ws_col;                                //сохранение переменных
    maxy = size.ws_row;
    windows_resize();                                  //обновление размеров окон
//    print(print_start);                                //обновить текст
    windows_refresh();                                 //обновить окна
}
void windows_init()
{
    int stepy = 0;
    int stepx = 0;

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

    stepx = maxx / 3;
    stepy = maxy;
    //--------------init windows--------------------
    main_window = newwin(maxy, maxx, 0, 0);
    wbkgd(main_window, COLOR_PAIR(2));
    // box(main_window, '|', '-');

    clients_window = derwin(main_window, stepy, stepx, 0, 0);     //создание окна
    wbkgd(clients_window, COLOR_PAIR(2));          //фоновые цвета
    box(clients_window, '|', '-');                 //рамка

    wattron(clients_window, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(clients_window, 1, 1, "User list");
    wattroff(clients_window, A_BOLD);
    wattron(clients_window, COLOR_PAIR(2));

    chat_window = derwin(main_window, stepy - 2, 2*stepx, 0, stepx);     //создание окна
    wbkgd(chat_window, COLOR_PAIR(2));          //фоновые цвета
    box(chat_window, '|', '-');                 //рамка

    wattron(chat_window, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(chat_window, 1, 1, "CHAT");
    wattroff(chat_window, A_BOLD);
    wattron(chat_window, COLOR_PAIR(2));

    msg_window = derwin(main_window, 3, 2*stepx, stepy - 3, stepx);     //создание окна
    wbkgd(msg_window, COLOR_PAIR(2));          //фоновые цвета
    keypad(msg_window, true);                  //включить клаву
    box(msg_window, '|', '-');                 //рамка
    wmove(msg_window, 1, 1);

    getmaxyx(stdscr, maxy, maxx);
    //wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
    windows_refresh();
}

void windows_del()
{
    delwin(main_window);
    delwin(clients_window);
    delwin(chat_window);
    delwin(msg_window);
    endwin();
}
void windows_clear()
{
    wclear(main_window);
    wclear(clients_window);
    wclear(chat_window);
    wclear(msg_window);
}
void windows_resize()
{
    int stepx = maxx / 3;
    int stepy = maxy;

    windows_clear();
    wresize(main_window, maxy, maxx);
    //box(main_window, '|', '-');

    wresize(clients_window, stepy, stepx);
    box(clients_window, '1', '-');
    mvwprintw(clients_window, 1, 1, "%i, %i\n", stepy, stepx);

    wresize(chat_window, stepy - 2, 2*stepx);
    box(chat_window, '|', '-');
    mvwprintw(chat_window, 1, 1, "%i, %i\n", stepy - 2, 2*stepx);
    mvwin(chat_window, 0, stepx);

    wresize(msg_window, 3, 2*stepx);
    box(msg_window, '|', '-');
    mvwprintw(msg_window, 1, 1, "%i, %i\n", 3, 2*stepx);
    mvwin(msg_window, stepy - 2, stepx);

    refresh();    
}
void windows_refresh()
{
    //wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
    wrefresh(clients_window);
    wrefresh(sub_clients_window);
    wrefresh(chat_window);
    wrefresh(sub_chat_window);
    wrefresh(msg_window);
    wrefresh(sub_msg_window);
}
void print_auto_row(WINDOW *window, char *s)
{
    static int row = 0;                    //переменная для хранение №тек. строки

    mvwprintw(window, row + 2, 1, s);
    row++;
    if (row >= (maxy - 4)) {
        wclear(window);
        box(window, '|', '-');
        row = 0;
    }
    windows_refresh();
}
void print(WINDOW *window, char *s, int row)
{
    mvwprintw(window, row + 2, 1, s);
    row++;
    if (row >= (maxy - 4)) {
        wclear(window);
        box(window, '|', '-');
        row = 0;
    }
    windows_refresh();
}

#endif
