#include "Connect.h"
#include "common.h"
#include "threadpool.h"
#include "work.h"
#include "linkpool.h"
#include <dlfcn.h>
#define MAX_LEN 1024000

struct threadpool *pool;
char *buf;
extern int conn_epollfd;
void (*worker)(void *arg);
extern struct epoll_event conn_events[MAX_SOCKET_NUMBERS];
void *Connect(void *arg){
	conn_init();
	int ret, i;
	while(1){
		ret = epoll_wait(conn_epollfd, conn_events, MAX_SOCKET_NUMBERS, -1);
		show_err(ret < 0, "work_epoll等候失败\r\n");
		for(i = 0; i < ret; i++){
			if(conn_events[i].events & EPOLLIN){
				threadpool_add_job(pool, receive_data, (void *)&conn_events[i]);
			}
			else
				printf("Connect do other things!\n");
		}
	}
	conn_finish();
}

void *receive_data(void *arg){
	struct epoll_event *ent = (struct epoll_event *)arg;
	int connfd = (ent->data).fd;
	int ret;
	int recvnum = 0;
	buf[recvnum] = '\0';
	while(1){
		pthread_mutex_lock(&(pool->mutex));
		ret = recv(connfd, buf + recvnum, MAX_LEN, 0);
		pthread_mutex_unlock(&(pool->mutex));

		if(ret < 0){
			if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
				printf("read later \r\n");
			}
		    	break;
		}
		else if(ret == 0){
			//addfd(conn_epollfd, connfd, 1);
			break;
		}
		else
		{
			link_update(connfd);
			recvnum += ret;
			buf[recvnum] = '\0';
		}
	}
	//puts(buf);
	//printf("receive_data : connfd = %d buf = %s\n", ent->data.fd, buf);
	wjob warg;
	warg.fd = connfd;
	warg.buf = buf;
	(*(work))((void *)&warg);
	close(connfd);
	epoll_ctl(conn_epollfd, EPOLL_CTL_DEL, ent->data.fd, NULL);
}

int conn_init(){
	buf = (char *)malloc(MAX_LEN);
	pool = threadpool_init(10, 20);
}
void conn_finish(){
	free(buf);
	threadpool_destroy(pool);
}
