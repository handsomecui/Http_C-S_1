#include "linkpool.h"
#include "common.h"
#include "threadpool.h"
extern int TIMEOUT;
map<int, Link*>connpool;
extern struct threadpool *pool;
int link_init(){
	return 1;
}
void link_finish(){
	map<int, Link*>::iterator iter;
	//pthread_mutex_lock(&(pool->conn_mutex));
	for(iter = connpool.begin(); iter != connpool.end(); iter++){
		close(iter->first);
		if(iter->second != NULL){
			delete iter->second;
			iter->second = NULL;
		}
		connpool.erase(iter->first);
	}
	//pthread_mutex_unlock(&(pool->conn_mutex));
}
void* link_timeout(void* arg){
	map<int, Link*>::iterator iter;	
	//这里枷锁了就不能添加连接等接下来的操作，因为他们都在等着unlock，然而不会发生的
	//pthread_mutex_lock(&(pool->conn_mutex));
	while(1){
		for(iter = connpool.begin(); iter != connpool.end(); iter++){
			if(difftime(time(NULL), iter->second->last_linktime) >= TIMEOUT){
				printf("connfd %d: Timeout!\n", iter->first);
				close(iter->first);
				if(iter->second != NULL){
					delete iter->second;
					iter->second = NULL;
				}
				connpool.erase(iter->first);
			}
		}
	}
	//pthread_mutex_unlock(&(pool->conn_mutex));
}
void link_delete(int connfd){
	//pthread_mutex_lock(&(pool->conn_mutex));
	if(connpool.count(connfd) == 0){
		printf("link_delete error!\n");
		close(connfd);
		//pthread_mutex_unlock(&(pool->conn_mutex));
		return;
	}
	delete connpool[connfd];
	connpool[connfd] = NULL;
	connpool.erase(connfd);
	close(connfd);
	//pthread_mutex_unlock(&(pool->conn_mutex));
	return;
}
int link_add(int connfd){
	//pthread_mutex_lock(&(pool->conn_mutex));
	if(connpool.count(connfd) != 0){
		printf("link_add error!\n");
		//Link *newLink = getlink(connfd);
		//newLink->last_linktime = time(NULL);
		close(connfd);
		//pthread_mutex_unlock(&(pool->conn_mutex));
		return 0;
	}
	Link *newLink = (Link *)malloc(sizeof(Link));
	newLink->connfd = connfd;
	newLink->last_linktime = time(NULL);
	printf("add connfd %d\n ", connfd);
	connpool.insert(make_pair(connfd, newLink));
	//pthread_mutex_unlock(&(pool->conn_mutex));
	return 1;
}
void link_update(int connfd){
	//pthread_mutex_lock(&(pool->conn_mutex));
	if(connpool.count(connfd) == 0){
		printf("link_update error!\n");
		//pthread_mutex_unlock(&(pool->conn_mutex));
		return;
	}
	map<int, Link*>::iterator iter;
	for(iter = connpool.begin(); iter != connpool.end(); iter++){
		if(iter->first == connfd)
			iter->second->last_linktime = time(NULL);
	}	
	//pthread_mutex_unlock(&(pool->conn_mutex));
	//connpool[connfd]->last_linktime = time(NULL);
}

Link* getlink(int connfd){
	//pthread_mutex_lock(&(pool->conn_mutex));
	if(connpool.count(connfd) == 0){
		printf("getlink error! \n");
		//pthread_mutex_unlock(&(pool->conn_mutex));
		return NULL;
	}
	//pthread_mutex_unlock(&(pool->conn_mutex));
	return connpool[connfd];
}

