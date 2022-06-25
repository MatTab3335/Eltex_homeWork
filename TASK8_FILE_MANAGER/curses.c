#include "cur_functions.h"


int main(int argc, char ** argv)
{
	windows_init();
	int key;
	char names[][10] = {"One", "Two", "Three"};
	unsigned int highlight = 0;

	while(1) {
		for (int i = 0; i < 3; i++) {
			if (i == highlight)
				wattron(sub_name_field, A_REVERSE);
			mvwprintw(sub_name_field, i+2, 1, names[i]);
			wattroff(sub_name_field, A_REVERSE);
		}

		key = wgetch(sub_name_field);
		switch(key) {
			case KEY_DOWN: 
				highlight++;
				break;
			case KEY_UP:
				highlight--;
				break;
			default:
				break;
		}
	}
	windows_del();
	wmove(stdscr, 8, 1);
	printw("Press any key to continue...");
	refresh();
	getch();
	endwin();
	exit(EXIT_SUCCESS);
}

