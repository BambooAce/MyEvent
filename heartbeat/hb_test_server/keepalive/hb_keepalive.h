#ifndef HB_SOCKOPT_SERVER_H
#define HB_SOCKOPT_SERVER_H
int initListenfd(char *addr, int port);
void setkeepalive(int lisfd, unsigned int begin, unsigned int cnt, unsigned int intvl);
void loop(int lisfd);

#endif // HB_SOCKOPT_SERVER_H
