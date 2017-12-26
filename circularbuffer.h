#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

typedef struct 
{
	void * buffer;
	void * buffer_end;
	void * head;
	void * tail;
	size_t size;
	int capacity;
	int count;
} RingBuffer;


RingBuffer * newRingBuffer(int , size_t);

int isfullRingBuffer(RingBuffer *);

int isemptyRingBuffer(RingBuffer *);

void pushRingBuffer(RingBuffer * , void * );

void * popRingBuffer(RingBuffer * );

void printRingBuffer(RingBuffer *, void (* print)(void*));

void printDouble(void * );

#endif