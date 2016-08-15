#include "main.h"
#include "common.h"
#include "log.h"
#include "Connect.h"
#include "linkpool.h"
#include "threadpool.h"
#define CONFIG_PATH "Config.ini"

char Sip[16];
int Sport;
int MAXLOGLEN;
int TIMEOUT;
struct sockaddr_in address;
int listenfd, lis_epollfd, conn_epollfd;
struct epoll_event lis_events[MAX_SOCKET_NUMBERS], conn_events[MAX_SOCKET_NUMBERS];
pthread_t conn_pid, lis_pid, link_pid;

struct threadpool *pool;

int main(){
	ginit();
	gstart();
	link_init();
	pthread_join(lis_pid, NULL);
	//pthread_join(conn_pid, NULL);
	//pthread_join(link_pid, NULL);
	gdestroy();
	link_finish();
}

void gstart(){
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, Sip, &address.sin_addr);
	address.sin_port = htons(Sport);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	show_err(listenfd < 0, "create socket error!");
	int on = 1;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, (socklen_t)sizeof(on)) < 0)
	{
		perror("set error");
	}
	int ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
	show_err(ret == -1, "bind error!");
	ret = listen(listenfd, 5);
	show_err(ret == -1, "listen error!");
	lis_epollfd = epoll_create(5);
	show_err(lis_epollfd == -1, "create listen epoll error!");
	addfd(lis_epollfd, listenfd, 1);
	conn_epollfd = epoll_create(5);
	show_err(conn_epollfd == -1, "create connect epoll error!");
	for(int i = 0; i < 1; i++){
		threadpool_add_job(pool, Connect, NULL);
	}
	ret = pthread_create(&lis_pid, NULL, listener, NULL);
	show_err(ret != 0, "create listen pthread error!");
	//ret = pthread_create(&conn_pid, NULL, Connect, NULL);
	//show_err(ret != 0, "create connect pthread error!");
	//ret = pthread_create(&link_pid, NULL, link_timeout, NULL);
	//show_err(ret != 0, "create connect pthread error!");
}


int ginit(){
	Getconfig();	
	pool = threadpool_init(6, 20);
}

void gdestroy(){
	threadpool_destroy(pool);
}

void *listener(void *arg){
	int ret, i;
	int connfd;
	struct sockaddr_in client_address;
	socklen_t client_addresslen = sizeof(client_address);
	while(1){
		//printf("before wait\n");
		ret = epoll_wait(lis_epollfd, lis_events, MAX_SOCKET_NUMBERS, 10);
		//printf("lis ret = %d\n", ret);
		show_err(ret < 0, "lis_epoll等候失败\r\n");
		for(;ret;){
			//	pthread_mutex_lock(&(pool->conn_mutex));
				connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addresslen);
			//	pthread_mutex_unlock(&(pool->conn_mutex));
				if(connfd < 0)
					break;
				//link_add(connfd);
				addfd(conn_epollfd, connfd, 1);
		}
	}
}

void Getconfig(){
	FILE *fp = fopen(CONFIG_PATH, "r");
	show_err(fp == NULL, "Can't open config file!");
	char buf[250];
	int i;
	while(fgets(buf, sizeof(buf), fp)){
		i = 0;
		while(isspace(buf[i]))i++;
		if(buf[i] == '\0')
			continue;
		char *p = strchr(buf, '=');
		*p = '\0';
		char *key = buf;
		char *value = p + 1;
		if(strcmp( key, "IP") == 0)
			sprintf(Sip, "%s", value);
		else if(strcmp(key, "PORT") == 0)
			sscanf(value, "%d", &Sport);
		else if(strcmp(key, "MAXLOGLEN") == 0)
			sscanf(value, "%d", &MAXLOGLEN);
		else if(strcmp(key, "TIMEOUT") == 0)
			sscanf(value, "%d", &TIMEOUT);
		else
			WriteSysLog("Error: unknow config item\n");
	}
	fclose(fp);
}

void addfd(int epollfd, int fd, int flag){
	struct epoll_event event;
	//memset(&event, 0x00, sizeof(event));
	event.data.fd = fd;
	event.events = EPOLLIN;
	if(flag){
		event.events |= EPOLLET;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd);
}

