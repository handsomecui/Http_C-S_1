#include "Connect.h"
#include "common.h"
#include "threadpool.h"
#include "work.h"
#include "linkpool.h"
#include "mAtomic.h"
#include <dlfcn.h>
#define MAX_LEN 1024

extern struct threadpool *pool;
char *buf;
extern int conn_epollfd;
void (*worker)(void *arg);
extern struct epoll_event conn_events[MAX_SOCKET_NUMBERS];
void *Connect(void *arg){
	conn_init();
	int ret, i;
	while(1){
		ret = epoll_wait(conn_epollfd, conn_events, MAX_SOCKET_NUMBERS, 1);
		//printf("ret = %d\n", ret);
		show_err(ret < 0, "work_epoll等候失败\r\n");
		for(i = 0; i < ret; i++){
			if(conn_events[i].events & EPOLLIN){
				(*receive_data)((void *)&conn_events[i]);
				//threadpool_add_job(pool, receive_data, (void *)&conn_events[i]);
			}
			else if(conn_events[i].events & EPOLLOUT){
				(*send_data)((void *)&conn_events[i]);
				//threadpool_add_job(pool, send_data, (void *)&conn_events[i]);
			}
			else
				printf("Connect do other things!\n");
		}
	}
	conn_finish();
}

void *receive_data(void *arg){
	pthread_mutex_lock(&(pool->conn_mutex));
	struct epoll_event *ent = (struct epoll_event *)arg;
	int connfd = (ent->data).fd;
	int ret;
	int recvnum = 0;
	//由于add，与下面send中的get不在同一线程，有时候会造成get失败
	puts("**");
	if(link_add(connfd) == 0){
		
	};
	puts("--");
	Link *iolink = getlink(connfd);
	puts("==");
	if(iolink == NULL){
		printf("getlink connfd error!\n");
		link_delete(connfd);
		epoll_ctl(conn_epollfd, EPOLL_CTL_DEL, connfd, ent);
		pthread_mutex_unlock(&(pool->conn_mutex));
		return NULL;
	}
	
		//pthread_mutex_lock(&(pool->mutex));
		ret = recv(connfd, iolink->recvbuf + recvnum, MAX_LEN, MSG_NOSIGNAL);
		//pthread_mutex_unlock(&(pool->mutex));

		if(ret < 0){
		    link_delete(connfd);
			epoll_ctl(conn_epollfd, EPOLL_CTL_DEL, connfd, ent);
			pthread_mutex_unlock(&(pool->conn_mutex));	
			//break;
		}
		else if(ret == 0){
			//addfd(conn_epollfd, connfd, 1);
		    link_delete(connfd);
			epoll_ctl(conn_epollfd, EPOLL_CTL_DEL, connfd, ent);
			pthread_mutex_unlock(&(pool->conn_mutex));	
			//break;
		}
		else
		{
			link_update(connfd);
			recvnum += ret;
			buf[recvnum] = '\0';

	printf("recv_data %d: %s\n", connfd, iolink->recvbuf);
	/*
	if(!check_complete(iolink->recvbuf)){
		link_delete(connfd);
		epoll_ctl(conn_epollfd, EPOLL_CTL_DEL, ent->data.fd, ent);
		//需要发送不完整报文
		


		return NULL;
	}
	*/
	//puts(buf);
	
	ent->events = EPOLLET | EPOLLOUT;
	epoll_ctl(conn_epollfd, EPOLL_CTL_MOD, ent->data.fd, ent);
	pthread_mutex_unlock(&(pool->conn_mutex));
		}
}

void *send_data(void * arg){
	pthread_mutex_lock(&(pool->conn_mutex));
	struct epoll_event *ent = (struct epoll_event *)arg;
	ent->events = EPOLLIN | EPOLLET;
	int connfd = (ent->data).fd;
	
	Link *iolink = getlink(connfd);
	if(iolink == NULL){
		printf("send : getlink error !  connfd = %d\n", connfd);
		link_delete(connfd);
		epoll_ctl(conn_epollfd, EPOLL_CTL_DEL, ent->data.fd, ent);
		pthread_mutex_unlock(&(pool->conn_mutex));
		return NULL;
	}
	epoll_ctl(conn_epollfd, EPOLL_CTL_DEL, ent->data.fd, ent);
	wjob warg;
	warg.fd = connfd;
	warg.buf = iolink->recvbuf;
	printf("send_data %d: %s\n", connfd, warg.buf);
	(*(work))((void *)&warg);
	link_delete(connfd);
	//close(connfd);
	printf("send over\n");
	pthread_mutex_unlock(&(pool->conn_mutex));
}

int check_complete(const char *buf){
	int len = strlen(buf);
	const char *method = strstr(buf, "GET");
	if(method && len > 74){
		return 1;
	}
	else{
		printf("request is invalid\n");
		return 0;
	}

}

int conn_init(){
	buf = (char *)malloc(MAX_LEN);
}
void conn_finish(){
	free(buf);
}
