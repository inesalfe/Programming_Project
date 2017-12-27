#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "circularbuffer.h"

RingBuffer * newRingBuffer(int capacity, size_t size){

	RingBuffer * rb = (RingBuffer *) malloc(sizeof(RingBuffer));

	rb->buffer 		= malloc(capacity * size);
	rb->head 		= 0;
	rb->tail		= 0;
	rb->size 		= size;
	rb->capacity 	= capacity;
	rb->length 		= 0;

	return rb;
}

int isfullRingBuffer(RingBuffer * rb){
	if(rb->length == rb->capacity)
		return 1;
	else
		return 0;
}

int isemptyRingBuffer(RingBuffer * rb){
	if(rb->length == 0)
		return 1;
	else
		return 0;
}

void writeRingBuffer(RingBuffer * rb, void * item){
	if (isfullRingBuffer(rb) == 1)
		rb->head = (rb->head + 1) % rb->capacity;
	else
		rb->length ++;
	memcpy(&rb->buffer[rb->tail], item, rb->size);
	rb->tail = (rb->tail + 1) % rb->capacity;
}

void printRingBuffer(RingBuffer * rb, void (* print)(void*)){

	int i, idx;
	for(i=0, idx = rb->head; i < rb->length; i++, idx = (idx + 1) % rb->capacity){
		printf("[ buf[%d] =", i);
		print((char*)&(rb->buffer[idx]));
		printf("]  ");
		if(i%3 == 2) printf("\n");
	}
	printf("\n");

}

void printDouble(void * x)
{
    double * tmp = (double *) x;
    printf(" %lf ", * tmp);
}

void printInt(void * x)
{
    int * tmp = (int *) x;
    printf(" %d ", * tmp);
}

// Testing
int main(){

	int a;
	RingBuffer * rb = newRingBuffer(3, sizeof(int));
	a = 1;
	writeRingBuffer(rb, &a);
	printRingBuffer(rb, printInt);
	printf("--------\n");
	a = 2;
	writeRingBuffer(rb, &a);	
	printRingBuffer(rb, printInt);
	printf("--------\n");
	a = 3;
	writeRingBuffer(rb, &a);	
	printRingBuffer(rb, printInt);
	printf("--------\n");
	a = 4;
	writeRingBuffer(rb, &a);	
	printRingBuffer(rb, printInt);
	a = 5;
	writeRingBuffer(rb, &a);	
	printRingBuffer(rb, printInt);

	// printf("%lf\n", *((double *) (&rb->buffer[1])));
	printf("%d\n", *((int *) (&rb->buffer[1])));

}
