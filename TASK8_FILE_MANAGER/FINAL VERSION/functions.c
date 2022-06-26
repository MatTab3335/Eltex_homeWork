#include "functions.h"

int nfiles = 0;
int prev_nfiles = 0;
char **name_list;
char **path_list;

int maxx;
int maxy;
int key;
int highlight = 1;
int print_start = 0;
char cur_dir[MAX_PATH];
char prev_dir[MAX_PATH];

WINDOW * name_field;
WINDOW * sub_name_field;
WINDOW * size_field;
WINDOW * sub_size_field;
WINDOW * date_field;
WINDOW * sub_date_field;

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
void print_names()
{
    /*struct stat stbuf;
    if (stat(name, &stbuf) == -1) {
        fprintf(stderr, "print_names: нет доступа к %s\n", name);
        return;
    }
    printf("%s\n", name);*/
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
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;                          //пропустить себя и родителя
        if (strlen(dir) + strlen(dp->d_name) + 2 > sizeof(path))
            fprintf(stderr, "dirwalk: слишком длинное имя %s/%s\n", 
                dir, dp->d_name);
        else {
            nfiles++;
            sprintf(path, "%s/%s", dir, dp->d_name);
            strcpy(name, dp->d_name);
            name_list = resize_list(name_list);
            strcpy(name_list[nfiles - 1], name);
            path_list = resize_list(path_list);
            strcpy(path_list[nfiles - 1], path);
            prev_nfiles = nfiles;
        }
    }
   	closedir(dfd);
}
char **resize_list(char **list)
{
    if (!(list = (char**)realloc(list, nfiles * sizeof(char*)))) {
        fprintf(stderr, "Reallocation error! name_list\n");
        exit(1);
    }
    if (prev_nfiles < nfiles)
        for (int i = prev_nfiles; i < nfiles; i++) 
        if (!(list[i] = (char*)malloc(MAX_PATH*sizeof(char)))) {
            fprintf(stderr, "Allocation error! name_list\n");
            exit(1);
        }
    else if (prev_nfiles > nfiles) {
        for (int i = nfiles; i < prev_nfiles; i++) 
            free(name_list[i]);
    }
    return list;  
}

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
	resizeterm(size.ws_row, size.ws_col);
	maxx = size.ws_col;
	maxy = size.ws_row;
	windows_resize();
    print(print_start);
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

	sub_name_field = derwin(name_field, maxy - 1, maxx - 1, 0, 1);
	keypad(sub_name_field, true);
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
    int row = 0;
    wclear(sub_name_field);
    wattron(sub_name_field, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(sub_name_field, 1, 1, "CURRENT DIRECTORY: %s; Files: %i",cur_dir, nfiles);
    wattroff(sub_name_field, A_BOLD);
    wattron(sub_name_field, COLOR_PAIR(2));
 //   mvwprintw(sub_name_field, 1, 50, "n = %i; s = %i; y = %i; h = %i", nfiles, start, maxy, highlight);
    for (int i = start; i < nfiles; i++) {
            if (row == highlight - 1)
                wattron(sub_name_field, A_REVERSE);
            mvwprintw(sub_name_field, row + 2, 1, name_list[i]);
            wattroff(sub_name_field, A_REVERSE);
            row++;
    }
    windows_refresh();
}