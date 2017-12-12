#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "heap_time.h"

static void flow_up(HeapTimer *ht, Timer *_timer)
{
	if (ht && _timer)
	{
		if (ht->size == 0)
		{
			return;
		}
		else if (ht->size == 1)
		{
			memcpy(&(ht->timer[ht->size]), _timer, sizeof(Timer));
			_timer = NULL;
			return;
		}
		int temp = ht->size;
		while(temp)
		{
			if (ht->timer[temp/2].timeout > (_timer->timeout))
			{
				ht->timer[temp].timeout = ht->timer[temp/2].timeout;
				ht->timer[temp].data = ht->timer[temp/2].data;
				ht->timer[temp].persist = ht->timer[temp/2].persist;
				ht->timer[temp].timegap = ht->timer[temp/2].timegap;
			}else{
				break;
			}
			temp = temp/2;
		}
		memcpy(&(ht->timer[temp]), _timer, sizeof(Timer));
		_timer = NULL;
	}
}

static void flow_down(HeapTimer *ht)
{
	unsigned int start = 1;
	unsigned int end = ht->size;
	unsigned int current = start;
	unsigned int l = 2*current;
	while(l <= end)
	{
		if (l < end && (ht->timer[l+1].timeout < ht->timer[l].timeout))
		{
			l++;
		}
		if (ht->timer[end].timeout < ht->timer[l].timeout)
		{
			break;
		}
		else{
			ht->timer[current].timeout = ht->timer[l].timeout;
			ht->timer[current].data = ht->timer[l].data;
			ht->timer[current].persist = ht->timer[l].persist;
			ht->timer[current].timegap = ht->timer[l].timegap;
			current = l;
			l = 2*current;
		}
	}
	ht->timer[current].timeout = ht->timer[end].timeout;
	ht->timer[current].data = ht->timer[end].data;
	ht->timer[current].persist = ht->timer[end].persist;
	ht->timer[current].timegap = ht->timer[end].timegap;
}

HeapTimer * initHeapTimer(size_t s)
{
	if (s <= (size_t)0)
	{
		return NULL;
	}
	HeapTimer * ht = (HeapTimer *)malloc(sizeof(HeapTimer));
	if (!ht)
	{
		return NULL;
	}
	ht->size = 0;
	ht->capacity  = s+1;
	ht->timer = (Timer *)malloc(sizeof(Timer) * (s+1));
	if (ht->timer)
	{
		memset(ht->timer, 0, sizeof(Timer)*(s+1));
		return ht;
	}
	return NULL;
}

void min_heap_push(HeapTimer * ht, Timer * _timer)
{
	if (ht && _timer)
	{
		if (ht->size == ht->capacity)
		{
			size_t need = ht->size + (ht->size)/3;
			Timer *temp = (Timer *)malloc(sizeof(Timer)*(need));
			if(temp){
				memset(temp, 0, sizeof(Timer)*(need));
				memcpy(temp, ht->timer, sizeof(ht->size));
				ht->capacity = need;
				free(ht->timer);
				ht->timer = temp;
				temp = NULL;
			}
		}
		(ht->size)++;
		flow_up(ht, _timer);
	}
}

void min_heap_pop(HeapTimer * ht, Timer * _timer)
{
	if ( ht && ht->size)
	{
		memset(_timer, 0, sizeof(Timer));
		memcpy(_timer, &(ht->timer[1]), sizeof(Timer));
		ht->timer[1].timeout = ht->timer[ht->size].timeout;
		ht->timer[1].data = ht->timer[ht->size].data;
		ht->timer[1].persist = ht->timer[ht->size].persist;
		ht->timer[1].timegap = ht->timer[ht->size].timegap;
		if (ht->size > 1)
		{
			flow_down(ht);
		}
		ht->timer[ht->size].timeout = 0;
		ht->timer[ht->size].data = NULL;
		ht->timer[ht->size].persist = 0;
		ht->timer[ht->size].timegap = 0;
		(ht->size)--;
	}
}

int check_had_timeout(HeapTimer *ht, time_t now)
{
	if (ht)
	{
		if (ht->size > (size_t)0)
		{
			return (ht->timer[1]).timeout < now;
		}
	}
	return 0;
}

int main()
{
	HeapTimer *timer = initHeapTimer(50);
	Timer t1;
	t1.timegap = 3;
	t1.timeout = time(NULL) + t1.timegap;
	t1.persist = 1;
	Timer t2;
	t2.timegap = 6;
	t2.timeout = time(NULL) + 6;
	t2.persist = 0;
	min_heap_push(timer, &t1);
	min_heap_push(timer, &t2);
	while(1)
	{
		if (check_had_timeout(timer, time(NULL)))
		{
			Timer temp;
			min_heap_pop(timer, &temp);
			if (temp.persist)
			{
				temp.timeout = time(NULL) + temp.timegap;
				min_heap_push(timer, &temp);
			}
			printf("timeout  %u\n", timer->size);
		}
		usleep(100);
	}
	return 0;
}
