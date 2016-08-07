#ifndef __WORK_H__
#define __WORK_H__
void *work(void *arg);
void winit();
void wfinish();
void getpath(char *buf);
void work_List(int connfd);
void work_Get(int connfd, char *buf);
void work_Put(int connfd, char *buf);
int get_size(char *str, int &i);
typedef struct wjob{
	int fd;
	char *buf;
}wjob;

#endif
