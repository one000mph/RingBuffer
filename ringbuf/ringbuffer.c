#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define BUF_SIZE 10


// struct message given in lab writeup
typedef struct
{
	int value; /* Value to be passed to consumer */
	int consumer_sleep; /* Time (in ms) for consumer to sleep */
	int line; /* Line number in input file */
	int print_code; /* Output code; see below */
	int quit; /* Non-zero if consumer should exit */
} message_t;

static typedef struct  {
	message buf_array[BUF_SIZE];
	void* input; // where to produce
	void* output; // where to consume
	pthread_mutex_t buf_mutex; //protect buffer from modification by more than one thread at a time
	pthread_cond_t allow_produce;    // allow when empty slots in buffer
	pthread_cond_t allow_consume;     // allow when message exist in the buffer

} ringbuf_t;

void sleep(int sleep_time) {
	struct timespec time1, time2;
	if (sleep_time > 999) {
		time1.tv_sec = 1;
		sleep_time -= 1000;
	}
}


// Attribution: Found online at http://stackoverflow.com/questions/7684359/using-nanosleep-in-c
// This is exactly what we want. We took time to understand this code - T & H
int nsleep(long milliseconds)
{
   struct timespec req, rem;

   if(milliseconds > 999)
   {   
        req.tv_sec = (int)(milliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (milliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }   
   else
   {   
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = milliseconds * 1000000;    /* Must be in range of 0 to 999999999 */
   }   

   return nanosleep(&req , &rem);
}


void buf_init(ringbuf_t* buffer) {
	buffer->input = 0;
	buffer->output = 0;
	// initialize mutex and conditions things here
}

message_t msg_init(int* line, int linenumber) {
	return message_t msg = {
		.value = line[0],
		.consumer_sleep = line[2],
		.line = linenumber,
		.print_code = line[3],
		.quit = 0
	};
}

void producer() {
	int line[4];
	int lineNumber = 1;

	// while unread content do stuff
	while (scanf("%d %d %d %d", &line[0], &line[1], &line[2], &line[3]) > 0) {
		int producer_sleep = line[1];
		message_t msg = msg_init(line, lineNumber);

		lineNumber++;

		// sleep for the given amount of time
		int sleep_return = nsleep(line[1]);

	}

	// if 0, write message with non-zero quit field

}

void consumer() {

}

int main (int argc, char *argv[]) {
	setlinebuf(stdout);



	// pthread_t producer_thread;
	// pthread_t consumer_thread;
	// pthread_create(&producer_thread, NULL, producer, (void*)&buf);
	// pthread_create(&consumer_thread, NULL, consumer, (void*)&buf);

	// pthread_join(producer_thread, NULL);
	// pthread_join(consumer_thread, NULL);

}


