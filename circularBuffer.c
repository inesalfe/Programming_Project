#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "circularbuffer.h"

RingBuffer * newRingBuffer(int capacity, size_t size){

	RingBuffer * rb = (RingBuffer *) malloc(sizeof(RingBuffer));

	rb->buffer 		= malloc(capacity * size);
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
		return;
	memcpy(&rb->buffer[rb->length], item, rb->size);
	rb->length ++;
}

void printRingBuffer(RingBuffer * rb, void (* print)(void*)){

	int i;
	for(i=0; i < rb->length; i++){
		printf("[ buf[%d] =", i);
		print((char*)&(rb->buffer[i]));
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

// Testing
int main(){

	double a, * vec;
	RingBuffer * rb = newRingBuffer(3, sizeof(double));
	a = 1.0;
	writeRingBuffer(rb, &a);
	printRingBuffer(rb, printDouble);
	printf("--------\n");
	a = 2.0;
	writeRingBuffer(rb, &a);	
	printRingBuffer(rb, printDouble);
	printf("--------\n");
	a = 3.0;
	writeRingBuffer(rb, &a);	
	printRingBuffer(rb, printDouble);
	printf("--------\n");
	a = 4.0;
	writeRingBuffer(rb, &a);	
	printRingBuffer(rb, printDouble);

	vec = (double *) rb->buffer;
	printf("%lf\n", vec[1]);
}
