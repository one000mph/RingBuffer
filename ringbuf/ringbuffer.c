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

typedef struct  {
	message_t buf_array[BUF_SIZE];
	int input; // where to produce
	int output; // where to consume
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

message_t msg_init(int* line, int linenumber, int quit) {
	message_t msg = {
		.value = line[0],
		.consumer_sleep = line[2],
		.line = linenumber,
		.print_code = line[3],
		.quit = quit
	};
	return msg;
}


void producer(ringbuf_t* ringbuf, int num_itr) {
	int line[4];
	int lineNumber = 1;

	int counter = 0;

	// while unread content do stuff
	while (counter < num_itr) {

		if (scanf("%d %d %d %d", &line[0], &line[1], &line[2], &line[3]) > 0){

			int message = line[0];
			int producer_sleep = line[1];
			int print_code = line[3];

			message_t msg = msg_init(line, lineNumber, 0);

			// sleep for the given amount of time
			int sleep_return = nsleep(producer_sleep);

			// populate the ring buffer, iterate the input index
			ringbuf->buf_array[ringbuf->input] = msg;

			if( (print_code == 1) | (print_code == 3 )){
				printf("Produced %d from input line %d\n", message, lineNumber);
			}

			counter++;
			lineNumber++;
		}

		else{
			//enter one more message without sleeping first
			message_t msg = msg_init(line, lineNumber, 1);

			int input_idx = ringbuf->input;
			ringbuf->buf_array[input_idx] = msg;
		}

		// handle the ring wraparound 
		if (ringbuf->input == (BUF_SIZE -1)){
			ringbuf->input = 0; 
		}
		else{
			(ringbuf->input)++;
		}
	}

}

void consumer(ringbuf_t* ringbuf, int num_itr) {
	//running sum
	int sum = 0;

	while(ringbuf->output != ringbuf->input){
		// read in the message
		message_t message = ringbuf->buf_array[ringbuf->output];

		if(message.quit != 0){
			printf("The Final Sum is %d", sum);
			return;
			//Thread terminates here
		}

		int sleep_return = nsleep(message.consumer_sleep);

		sum += message.value;

		if((message.print_code == 2) | (message.print_code == 3)){
			printf("Consumed %d from input line %d: sum = %d\n", message.value, message.line, sum);
		}

		// handle the ring wraparound 
		if (ringbuf->output == (BUF_SIZE -1)){
			ringbuf->output = 0; 
		}
		else{
			(ringbuf->output)++;
		}

	}
}

int main (int argc, char *argv[]) {

	setlinebuf(stdout);

	// create and initialize ringbuffer
	ringbuf_t buffer = {
		.input = 0,
		.output = 0
	};

	// FOR NOW produce some stuff
	int num_itr = 6;

	producer(&buffer, num_itr);
	consumer(&buffer, num_itr);

	producer(&buffer, num_itr);
	consumer(&buffer, num_itr);

	// pthread_t producer_thread;
	// pthread_t consumer_thread;
	// pthread_create(&producer_thread, NULL, producer, (void*)&buf);
	// pthread_create(&consumer_thread, NULL, consumer, (void*)&buf);

	// pthread_join(producer_thread, NULL);
	// pthread_join(consumer_thread, NULL);

}


