#include "common.h"
#include "work.h"
#define MAXLEN 25000
char *path;
char *sendbuf;
void *work(void *arg){
	//puts("***");
	winit();
	int fd = ((wjob * )arg)->fd;
	char *buf = ((wjob *)arg)->buf;
	printf("fd %d: %s\n", fd, buf);
	getpath(buf);
	FILE *fp = fopen(path, "r");
	show_err(fp == NULL, "error: open get file error!\n");
	int ret;
	int curlen = 0;
	sendbuf[curlen] = '\0';
	while((ret = fread(sendbuf + curlen, 1, MAXLEN, fp)) > 0){
		curlen += ret;
		sendbuf[curlen] = '\0';
	}
	fclose(fp);
	puts(sendbuf);
	send(fd, sendbuf, curlen, 0);
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
