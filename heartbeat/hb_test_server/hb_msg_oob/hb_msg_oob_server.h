#ifndef HB_SOCKOPT_SERVER_H
#define HB_SOCKOPT_SERVER_H
#include <time.h>
enum 
{
	CONNECTED = 1,
	OFFLINE = 2
};

struct Event
{
	int fd;
	//Heartbeat
	time_t idle;
	unsigned int times;
	unsigned int interval;
	int flags;
};
struct BaseEvent
{
	struct Event *cli_map;
	unsigned int cliNum;
};

int initListenfd(char *addr, int port);
void loop(int lisfd);

#endif // HB_SOCKOPT_SERVER_H
