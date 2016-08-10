#include "mAtomic.h"
#include "common.h"
#include "work.h"
#include "threadpool.h"
#include<dirent.h>
#define MAXLEN 1024000
char *path;
char *sendbuf;
extern struct threadpool *pool;
void *work(void *arg){
	//puts("***");
	winit();
	int connfd = ((wjob * )arg)->fd;
	char *buf = ((wjob *)arg)->buf;
	//printf("fd %d: %s\n", connfd, buf);
	getpath(buf);
	int fd, nbyte;
	if((fd = open(path, O_RDONLY | O_APPEND)) < 0){
		printf("fail to open %s\n", path);
		return NULL;
	}
	
	//pthread_mutex_lock(&(pool->mutex));
	while((nbyte = read(fd, buf, MAXLEN)) > 0){
		send(connfd, buf, nbyte,0);
	}
	//pthread_mutex_unlock(&(pool->mutex));
	close(connfd);
	close(fd);
	wfinish();
}



void getpath(char *buf){
	int i, j = 0;	
	path[j] = '\0';
	for(i = 0; buf[i]; i++){
		if(buf[i] == '/')
			path[j++] = buf[i];
		else if(buf[i] == ' ' && j)
			break;
		else if(j)
			path[j++] = buf[i];
	}
	path[j] = '\0';
	printf("path: %s\n", path);
}


void winit(){
	path = (char *)malloc(100);
	sendbuf = (char *)malloc(MAXLEN);
}

void wfinish(){
	free(path);
	free(sendbuf);
}
