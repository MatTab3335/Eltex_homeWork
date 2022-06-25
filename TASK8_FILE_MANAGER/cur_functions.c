#include "cur_functions.h"

int maxx;
int maxy;
int flag_refresh;

WINDOW * name_field;
WINDOW * sub_name_field;
WINDOW * size_field;
WINDOW * sub_size_field;
WINDOW * date_field;
WINDOW * sub_date_field;

int flag_refresh = 0;

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
	resizeterm(size.ws_row, size.ws_col);
	maxx = size.ws_col;
	maxy = size.ws_row;
	windows_resize();
	windows_refresh();
}
void windows_init()
{
	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	getmaxyx(stdscr, maxy, maxx);
	curs_set(FALSE);
	start_color();
	refresh();
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	attron(COLOR_PAIR(1) | A_BOLD);
//--------------NAME FIELD--------------------
	name_field = newwin(maxy, maxx, 0, 0);
	wbkgd(name_field, COLOR_PAIR(2));
	box(name_field, '|', '-');

	sub_name_field = derwin(name_field, maxy - 5, maxx - 5, 1, 2);
	keypad(sub_name_field, true);
	wbkgd(sub_name_field, COLOR_PAIR(2) | A_BOLD);

	getmaxyx(stdscr, maxy, maxx);
	wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
	wrefresh(sub_name_field);
	wrefresh(name_field);
/*//--------------DATE FIELD--------------------
	date_field = newwin(h_date_field, w_date_field, 0, w_name_field);
	wbkgd(date_field, COLOR_PAIR(2));
	box(date_field, '|', '-');

	sub_date_field = derwin(date_field, h_date_field - 5, w_date_field - 5, 1, 2);
	wbkgd(sub_date_field, COLOR_PAIR(2) | A_BOLD);

	wprintw(sub_date_field, "Date field\n");
	wrefresh(sub_date_field);
	wrefresh(date_field);
//--------------SIZE FIELD--------------------
	size_field = newwin(h_size_field, w_size_field, 0, w_name_field + w_date_field);
	wbkgd(size_field, COLOR_PAIR(2));
	box(size_field, '|', '-');

	sub_size_field = derwin(size_field, h_size_field - 5, w_size_field - 5, 1, 2);
	wbkgd(sub_size_field, COLOR_PAIR(2) | A_BOLD);

	wprintw(sub_size_field, "Size field\n");
	wrefresh(sub_size_field);
	wrefresh(size_field);*/
}

void windows_del()
{
	delwin(sub_name_field);
	delwin(name_field);
/*	delwin(sub_date_field);
	delwin(date_field);
	delwin(sub_size_field);
	delwin(size_field);*/
}
void windows_resize()
{
	wclear(name_field);
	wclear(sub_name_field);
	wresize(name_field, maxy, maxx);
	wresize(sub_name_field, maxy - 5, maxx - 5);
	box(name_field, '|', '-');
	wrefresh(name_field);
	wrefresh(sub_name_field);
	
}
void windows_refresh()
{
	//wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
	wrefresh(sub_name_field);
	wrefresh(name_field);
	flag_refresh = 0;
}