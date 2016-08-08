#include "linkpool.h"
#include "common.h"
extern int TIMEOUT;
map<int, Link*>connpool;

int link_init(){
	return 1;
}
void link_finish(){
	map<int, Link*>::iterator iter;
	for(iter = connpool.begin(); iter != connpool.end(); iter++){
		close(iter->first);
		free(connpool[iter->first]);
		connpool.erase(iter->first);
	}
}
void* link_timeout(void* arg){
	map<int, Link*>::iterator iter;	
	while(1){
		for(iter = connpool.begin(); iter != connpool.end(); iter++){
			if(difftime(time(NULL), iter->second->last_linktime) >= TIMEOUT){
				printf("connfd %d: Timeout!\n", iter->first);
				close(iter->first);
				free(connpool[iter->first]);
				connpool.erase(iter->first);
			}
		}
	}
}
void link_error(){
	return;
}
int link_add(int connfd){
	Link *newLink = new Link;
	newLink->connfd = connfd;
	newLink->last_linktime = time(NULL);
	//printf("add connfd %d\n ", connfd);
	connpool.insert(make_pair(connfd, newLink));
	return 1;
}
void link_update(int connfd){
	connpool[connfd]->last_linktime = time(NULL);
}


