#include <time.h>
typedef struct 
{
	time_t timeout;
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
int check_have_timeout(HeapTimer *ht, time_t now);