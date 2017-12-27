#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

typedef struct 
{
	void ** buffer;
	size_t size;
	int capacity;
	int length;
} RingBuffer;

RingBuffer * newRingBuffer(int , size_t);

int isfullRingBuffer(RingBuffer *);

int isemptyRingBuffer(RingBuffer *);

void writeRingBuffer(RingBuffer * , void * );

void printRingBuffer(RingBuffer *, void (* print)(void*));

void printDouble(void * );

#endif