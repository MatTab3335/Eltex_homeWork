// functions for 1st scheme
#include "server_functions.h"

int n_of_clients = 0;
int n_of_threads = 0;
//thread variablses
pthread_t *thread_list = NULL;
int perm_send = 0;
char *serv_stat;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;


pthread_t *allocate(pthread_t *list, int init_value)    //alloc list of threads
{        
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
        unlink_servers();
        for (int i = 0; i < n_of_threads; i++) {
                pthread_cancel(thread_list[i]);
        }
        unlink(MY_SOCK_PATH);
        free(thread_list);
        exit(0);
}
void handle_error_info(char *msg, char *info)
{
    char temp[256] = {};
    sprintf(temp, "<%s> : %s", info, msg);
    perror(temp); 
    exit(EXIT_FAILURE);
}
int create_server(int domain, int type, char *path)
{
	static int sfd;
    struct sockaddr_un my_addr;
    
    /* Clear structure */
    memset(&my_addr, 0, sizeof(struct sockaddr_un));
    
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        handle_error_info("socket", path);
    
	my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, path, sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1)
        handle_error_info("bind", path);

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        handle_error_info("listen", path);
        
    printf("Server %s is listening\n", path);
    
    return sfd;
}
void *thr_func(void *input)      //function for message processing thread
{
    int my_fd, cfd;
    struct sockaddr_un my_addr, client_addr;
    socklen_t client_addr_size;
    char in_buf[256] = {};
    char my_path[256] = {};

    client_addr_size = sizeof(struct sockaddr_un);
    memset(&client_addr, 0, sizeof(struct sockaddr_un));

    int id = *((int *)input);

    sprintf(my_path, "/tmp/tcp_server_%i", id);
    //printf("Thread path: %s\n", my_path);

    my_fd = create_server(AF_UNIX, SOCK_STREAM, my_path);
    printf("Server #%i is ready\n", id);
    // grant permission to send client info about me
    perm_send = 1;
    
    while(1) {        
		cfd = accept(my_fd, (struct sockaddr *) &client_addr,
						 &client_addr_size);
		if (cfd == -1) 
			handle_error("accept");

		while(1) {
			int recv_bytes = recv(cfd, in_buf, sizeof(in_buf), 0);
			if (recv_bytes == -1 || recv_bytes == 0) {
					printf("Thread %i is free\n", id);
					serv_stat[id] = 0;
					break;
			}
			printf("[MSG%i]: %s\n", id, in_buf);
			if (send(cfd, in_buf, sizeof(in_buf), MSG_NOSIGNAL) == -1)
				handle_error("send error");
		}
	}
    unlink(my_path);
}
void unlink_servers()
{
	char path[256] = {};
	for (int i = 0; i < n_of_threads; i++) {
		sprintf(path, "/tmp/tcp_server_%i", i);
		unlink(path);
	}
	printf("All servers are closed\n");
}
