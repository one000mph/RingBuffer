#include <pthread.h>


// struct message given in lab writeup
struct message
{
	int value; /* Value to be passed to consumer */
	int consumer_sleep; /* Time (in ms) for consumer to sleep */
	int line; /* Line number in input file */
	int print_code; /* Output code; see below */
	int quit; /* NZ if consumer should exit */
};

typdef struct  {
	int *buf;
	int n;
	int front;
	int rear;
	pthread_mutex_t buf_mutex; //protect buffer from modification by more than one thread at a time
	pthread_mutex_t filled;    // keep track of number of filled slots in buf
	pthread_mutex_t empty;     // keep track of number of empty slots in buf

} ringbuf_t;


