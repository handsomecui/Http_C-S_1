#ifndef __LINKPOOL_H__
#define __LINKPOOL_H__
#define BUF_SIZE 1024
#include <time.h>
typedef struct Link{
	int connfd;
	char recvbuf[BUF_SIZE];
	time_t last_linktime;
}Link;

int link_init();
void link_finish();
void* link_timeout(void* arg);
void link_delete(int connfd);
int link_add(int connfd);
void link_update(int connfd);
Link* getlink(int connfd);

#endif
