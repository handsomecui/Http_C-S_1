#include "common.h"

void show_err(int err, const char *str){
	if(err){
		perror(str);
		exit(1);
	}
}

int setnonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return new_option;
}
