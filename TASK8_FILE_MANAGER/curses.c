#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>

int maxx;
int maxy;

WINDOW * name_field;
WINDOW * sub_name_field;
WINDOW * size_field;
WINDOW * sub_size_field;
WINDOW * date_field;
WINDOW * sub_date_field;

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
	resizeterm(size.ws_row, size.ws_col);
	maxx = size.ws_col;
	maxy = size.ws_row;
	wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
	wrefresh(sub_name_field);
	wrefresh(name_field);
	
}

int main(int argc, char ** argv)
{


	int h_name_field = 100;
	int w_name_field = 50;
	int h_date_field = 100;
	int w_date_field = 30; 
	int h_size_field = 100;
	int w_size_field = 15;

	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(FALSE);
	start_color();
	refresh();
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	attron(COLOR_PAIR(1) | A_BOLD);
//--------------NAME FIELD--------------------
	name_field = newwin(h_name_field, w_name_field, 0, 0);
	wbkgd(name_field, COLOR_PAIR(2));
	box(name_field, '|', '-');

	sub_name_field = derwin(name_field, h_name_field - 5, w_name_field - 5, 1, 2);
	wbkgd(sub_name_field, COLOR_PAIR(2) | A_BOLD);

	getmaxyx(stdscr, maxy, maxx);
	wprintw(sub_name_field, "%i, %i\n", maxy, maxx);
	wrefresh(sub_name_field);
	wrefresh(name_field);
//--------------DATE FIELD--------------------
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
	wrefresh(size_field);

	while(1);

	delwin(sub_name_field);
	delwin(name_field);
	delwin(sub_date_field);
	delwin(date_field);
	delwin(sub_size_field);
	delwin(size_field);
	wmove(stdscr, 8, 1);
	printw("Press any key to continue...");
	refresh();
	getch();
	endwin();
	exit(EXIT_SUCCESS);
}