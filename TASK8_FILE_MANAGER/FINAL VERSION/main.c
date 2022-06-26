#include "functions.h"

int main()
{
    struct stat info;
    allocate();
    //-------Init curses----------
    setlocale(LC_ALL, "ru_RU.UTF-8");
    windows_init();

	//-------Go walk through directories--------
    strcpy(cur_dir, "/home/matvey");
    dirwalk(cur_dir);
    
    while(1) {
        char temp[MAX_PATH] = {};
        int i = 0;
        int remove = 0;
        print(print_start);
        key = wgetch(sub_name_field);
        switch(key) {
            case KEY_DOWN: 
                highlight++;
                if ((highlight >= maxy - 2) || (highlight >= nfiles) 
                    || (highlight >= (nfiles - print_start))) {
                    print_start++;
                    highlight = (maxy - 2 > nfiles) ? nfiles:(maxy - 2);
                    highlight = (highlight > nfiles - print_start) 
                    ? nfiles - print_start:highlight;
                }
                break;
            case KEY_UP:
                highlight--;
                if (highlight <= 1) {
                    highlight = 1;
                    print_start--;
                    if (print_start <= 0)
                        print_start = 0;
                }   
                break;
            case KEY_RIGHT:
                stat(path_list[print_start + highlight - 1], &info);
                if (S_ISREG(info.st_mode))
                    break;
                strcpy(cur_dir, path_list[print_start + highlight - 1]);
                dirwalk(path_list[print_start + highlight - 1]);
                highlight = 1;
                print_start = 0;
                break;
            case KEY_LEFT:
                i = strlen(cur_dir);
                remove = 0;
                while (cur_dir[i] != '/') {
                    remove++;
                    i--;
                }
                strncpy(temp, cur_dir, strlen(cur_dir) - (size_t)remove);
                strcpy(cur_dir, temp);
                dirwalk(cur_dir);
                highlight = 1;
                print_start = 0;
                break;
            default:
                break;
        }
        

            
                
    }
    free_mem();
    windows_del();
    exit(EXIT_SUCCESS);
}

