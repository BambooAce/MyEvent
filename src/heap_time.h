#include <time.h>
typedef struct 
{
	int persist;
	time_t timeout;
	time_t timegap;
	void * data;
}Timer;
typedef struct 
{
	Timer *timer;
	size_t size;
	size_t capacity;
}HeapTimer;

HeapTimer * initHeapTimer(size_t s);
void min_heap_push(HeapTimer * ht, Timer * _timer);
void min_heap_pop(HeapTimer * ht, Timer * _timer);
int check_had_timeout(HeapTimer *ht, time_t now);