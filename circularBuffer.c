#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

RingBuffer * newRingBuffer(int capacity, size_t size){

	RingBuffer * rb = (RingBuffer *) malloc(sizeof(RingBuffer));

	rb->buffer 		= malloc(capacity * sizeof(double));
	rb->head   		= rb->buffer;
	rb->tail   		= rb->buffer;
	rb->buffer_end 	= (char *)rb->buffer + capacity * sizeof(double);
	rb->size 		= size;
	rb->capacity 	= capacity;
	rb->count 		= 0;
	return rb;
}

int isfullRingBuffer(RingBuffer * rb){
	if(rb->count == rb->capacity)
		return 1;
	else
		return 0;
}

int isemptyRingBuffer(RingBuffer * rb){
	if(rb->count == 0)
		return 1;
	else
		return 0;
}

void pushRingBuffer(RingBuffer * rb, void * item){
	
	if (isfullRingBuffer(rb) == 1)
		return;
	memcpy(rb->tail, item, rb->size);
	rb->tail = (char *) rb->tail + rb->size;
	if(rb->tail == rb->buffer_end)
		rb->tail = rb->buffer;
	rb->count ++;
}

	void * popRingBuffer(RingBuffer * rb){

	void * item;
	if(isemptyRingBuffer(rb) == 1)
		return NULL;
	item = rb->head;
	// memcpy(item, rb->head, rb->size);
	rb->head = (char *) rb->head + rb->size;
	if(rb->head == rb->buffer_end)
		rb->head = rb->buffer;
	rb->count--;
	return item;
}

void printRingBuffer(RingBuffer * rb, void (* print)(void*)){

	int i = 0;
	void *tmp = (char*) rb->head;
	if (isemptyRingBuffer(rb) == 1)
		return;
	do{
		printf("[ buf[%d] =", i);
		print((char*)tmp);
		printf("]  ");
		tmp = (char*) tmp + rb->size;
		if(tmp == rb->buffer_end)
			tmp = rb->buffer;
		i++;
		if(i%5 == 0) printf("\n");
	}while(tmp != rb->tail);
	printf("\n");

}

void printDouble(void * x)
{
    double * tmp = (double *) x;
    printf(" %lf ", * tmp);
}

// Testing
int main(){

	double a;
	RingBuffer * rb = newRingBuffer(3, sizeof(double));
	a = 1.0;
	pushRingBuffer(rb, &a);
	printRingBuffer(rb, printDouble);
	a = 2.0;
	pushRingBuffer(rb, &a);	
	printRingBuffer(rb, printDouble);
	a = 3.0;
	pushRingBuffer(rb, &a);	
	printRingBuffer(rb, printDouble);
	a = *((double *)popRingBuffer(rb));
	printf("%lf\n", a);
	printRingBuffer(rb, printDouble);
}
