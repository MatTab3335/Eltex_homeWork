#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSIZE 10

int fd;
ssize_t nr;
off_t ret;
const char buf_out[] = "Hello!!!";
char buf_in[BUFSIZE + 1];
char c;
long offset;

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
    lseek(fd, 0, SEEK_SET);
    nr = read(fd, buf_in, BUFSIZE);
    if (nr == -1)
        printf("Error! Can't readt a file!\n");

    puts(buf_in);

    /***Read in reverse***/
    ret = lseek(fd, -1, SEEK_END);
    if (ret == (off_t)-1)
        printf("Seek error!\n");
    for (int i = 0; i < nr; i++) {
        read(fd, &c, 1);
        buf_in[i] = c;
        lseek(fd, -2, SEEK_CUR);
    }
    puts(buf_in);
    close(fd);
    return 0;
}

