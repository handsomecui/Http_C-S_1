#ifndef __WORKER_H__
#define __WORKER_H__

int conn_init();
void conn_finish();
void *Connect(void *arg);
void *receive_data(void *arg);
int check_complete(const char *buf);
void *send_data(void *arg);

#endif
