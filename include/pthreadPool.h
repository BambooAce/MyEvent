#ifndef PTHREADPOOL_H
#define PTHREADPOOL_H
int initThreadPool(int threadnum, TASKLIST * tl, int detach);
int joinAllThreads();
#endif
