#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int fd;
ssize_t nr;
const char buf_out[] = "Hello!!!";
char buf_in[10];

int main(void)
{
    fd = creat("file.txt", 0644);
    if (fd == -1)
        printf("Error! Can't creat a file!\n");
    fd = open("file.txt", O_RDWR);
    if (fd == -1)
        printf("Error! Can't open a file!\n");
    nr = write(fd, buf_out, strlen(buf_out));
    if (nr == -1)
        printf("Error! Can't read to a file!\n");
    lseek(fd, 0, 0);
    nr = read(fd, buf_in, 10);
    if (nr == -1)
        printf("Error! Can't readt a file!\n");

    puts(buf_in);
    return 0;
}
