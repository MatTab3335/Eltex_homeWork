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
void print(WINDOW *window, int start);      //вывод содержимого массивов в окно

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
//--------------init windows--------------------
    clients_window = newwin(maxy, maxx / 3, 0, 0);     //создание окна
    wbkgd(clients_window, COLOR_PAIR(2));          //фоновые цвета
    box(clients_window, '|', '-');                 //рамка

    sub_clients_window = derwin(clients_window, maxy - 1, (maxx / 3) - 1, 0, 1); //суб-окно, чтобы текст не переписывал рамку
    keypad(sub_name_field, false);                  //выключить клаву
    wbkgd(sub_name_field, COLOR_PAIR(2));

    chat_window = newwin(maxy, 2*maxx/3, 0, maxx/3);     //создание окна
    wbkgd(chat_window, COLOR_PAIR(2));          //фоновые цвета
    box(clients_window, '|', '-');                 //рамка

    sub_chat_window = derwin(chat_window, maxy - 1, (2*maxx/3) - 1, 0, 1); //суб-окно, чтобы текст не переписывал рамку
    keypad(sub_chat_window, false);                  //выключить клаву
    wbkgd(sub_chat_window, COLOR_PAIR(2));

    msg_window = newwin(2, 2*maxx/3, maxy - 2, maxx/3);     //создание окна
    wbkgd(msg_window, COLOR_PAIR(2));          //фоновые цвета
    box(msg_window, '|', '-');                 //рамка

    sub_msg_window = derwin(msg_window, 2, (2*maxx/3) - 1, 0, 1); //суб-окно, чтобы текст не переписывал рамку
    keypad(sub_msg_window, true);                  //включить клаву
    wbkgd(sub_msg_window, COLOR_PAIR(2));

    getmaxyx(stdscr, maxy, maxx);
    //wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
    windows_refresh();
}

void windows_del()
{
    delwin(sub_name_field);
    delwin(name_field);
    endwin();
}
void windows_clear()
{
    wclear(clients_window);
    wclear(sub_clients_window);
    wclear(chat_window);
    wclear(sub_chat_window);
    wclear(msg_window);
    wclear(sub_msg_window); 
}
void windows_resize()
{
    windows_clear();
    wresize(clients_window, maxy, maxx / 3);
    wresize(sub_clients_window, maxy - 1, (maxx / 3) - 1);
    wresize(chat_window, maxy, 2*maxx/3);
    wresize(sub_chat_window, maxy - 1, (2*maxx/3) - 1);
    wresize(msg_window, 2, 2*maxx/3);
    wresize(sub_msg_window, 2, (2*maxx/3) - 1);

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
void print(WINDOW *window, int start)
{
    int row = 0;                    //переменная для хранение №тек. строки
    wclear(window);
    //wattron(window, COLOR_PAIR(1) | A_BOLD);    //желтая строка с тек. адресом
    //mvwprintw(sub_name_field, 1, 1, "CURRENT DIRECTORY: %s; Files: %i",cur_dir, nfiles);
    //wattroff(sub_name_field, A_BOLD);
    //wattron(sub_name_field, COLOR_PAIR(2));             //вернуть цвет обратно
 //   mvwprintw(sub_name_field, 1, 50, "n = %i; s = %i; y = %i; h = %i", nfiles, start, maxy, highlight);

    mvwprintw(window, row + 2, 1, "Hello");
    row++;

    windows_refresh();
}