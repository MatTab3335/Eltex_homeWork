#include "functions.h"

int n_of_clients = 0;
//thread variablses
pthread_t *thread_list = NULL;

pthread_t *allocate(pthread_t *list, int init_value){        //alloc list of threads

	if (!(list = (pthread_t *)malloc(init_value * sizeof(pthread_t)))) {
		handle_error("Allocation error!");
		exit(1);
	}
	return list;
}
pthread_t *reallocate(pthread_t *list, int size){      //change size
	if (!(list = (pthread_t *)realloc(list, size * sizeof(pthread_t)))) {
		handle_error("Reallocation error!");
		exit(1);
	}
	return list;
}
void SignalHandler(int signal)
{
	printf("Finish server\n");
	for (int i = 0; i < n_of_clients; i++) {
		pthread_cancel(thread_list[i]);
	}
	unlink(MY_SOCK_PATH);
	free(thread_list);
	exit(0);
}
void *get_msg(void *input)
{
	int my_fd;
    struct sockaddr_un my_addr, client_addr;
    socklen_t client_addr_size;
    char in_buf[256] = {};
    char my_path[256] = {};

    client_addr_size = sizeof(struct sockaddr_un);

	int fd = ((struct args *)input)->cfd;
	int id = ((struct args *)input)->id;

	sprintf(my_path, "/tmp/tcp_server_%i", id);
	printf("Thread path: %s\n", my_path);

	my_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (my_fd == -1)
        handle_error("socket");

    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    memset(&my_addr, 0, sizeof(struct sockaddr_un));

    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, my_path,
            sizeof(my_addr.sun_path) - 1);

    if (bind(my_fd, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_un)) == -1)
        handle_error("bind");

    if (listen(my_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen");


    fd = accept(my_fd, (struct sockaddr *) &client_addr,
                     &client_addr_size);

	while(1) {
		int recv_bytes = recv(fd, in_buf, sizeof(in_buf), 0);
		if (recv_bytes == -1 || recv_bytes == 0) {
			printf("Thread %i is closed\n", id);
			break;
		}
		printf("[MSG]: %s\n", in_buf);
        // if (send(fd, in_buf, sizeof(in_buf), MSG_NOSIGNAL) == -1)
        // 	handle_error("send error");
	}
}
