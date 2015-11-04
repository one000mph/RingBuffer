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
	int input; 						 // where to produce
	int output; 					 // where to consume
	int count;						 // count things in buffer
	pthread_mutex_t buf_mutex; 		 // protect buffer from modification by more than one thread at a time
	pthread_cond_t allow_produce;    // allow when empty slots in buffer
	pthread_cond_t allow_consume;    // allow when message exist in the buffer

} ringbuf_t;


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


void* producer(void* ringbuf) {
	int line[4];
	int lineNumber = 1;

	ringbuf_t buffer = *((ringbuf_t) ringbuf);

	// while unread content read a line
	while (scanf("%d %d %d %d", &line[0], &line[1], &line[2], &line[3]) > 0) {
		int message = line[0];
		int producer_sleep = line[1];
		int print_code = line[3];

		// initialize the message struct
		message_t msg = msg_init(line, lineNumber, 0);

		// sleep for the given amount of time
		int sleep_return = nsleep(producer_sleep);

		// check if the buffer is full, if so wait for the consumer to signal
		pthread_mutex_lock(&buffer->buf_mutex);
		if (buffer->count == BUF_SIZE) pthread_cond_wait(&buffer->allow_produce, &buffer->buf_mutex);
		pthread_mutex_unlock(&buffer->buf_mutex);

		// populate the ring buffer with the message and iterate the count index
		pthread_mutex_lock(&buffer->buf_mutex);
		buffer->buf_array[buffer->input] = msg;
		pthread_cond_signal(&buffer->allow_consume);
		buffer->count++;
		pthread_mutex_unlock(&buffer->buf_mutex);

		// print the status message if needed
		if((print_code == 1) | (print_code == 3)){
			printf("Produced %d from input line %d\n", message, lineNumber);
		}
		lineNumber++;

		// set input to the next valid buf_array index
		if (buffer->input == (BUF_SIZE - 1)) {
			// reset to zero if currently at the last array index
			buffer->input = 0;
		} else {
			(buffer->input)++;
		}
	}

	// handle EOF
	// enter one more message without sleeping first
	// we use whatever happens to be in line since all
	// fields will be ignored except for quit
	message_t msg = msg_init(line, lineNumber, 1);
	// add to buffer array
	buffer->buf_array[buffer->input] = msg;
}

void* consumer(void* ringbuf) {
	//running sum
	int sum = 0;
	ringbuf_t buffer = *((ringbuf_t) ringbuf);

	// consume is possible
	while(buffer->output != buffer->input){

		// read in the message
		message_t message = buffer->buf_array[buffer->output];

		// allow producer to continue producing
		pthread_cond_signal(&buffer->allow_produce);

		if(message.quit != 0){
			printf("The Final Sum is %d", sum);
			return;
			//Thread terminates here
		}

		// sleep for the appropriate amount of time
		int sleep_return = nsleep(message.consumer_sleep);

		// add value to accumulator
		sum += message.value;

		// print consumer status when needed
		if((message.print_code == 2) | (message.print_code == 3)){
			printf("Consumed %d from input line %d: sum = %d\n", message.value, message.line, sum);
		}

		// handle the ring wraparound 
		if (buffer->output == (BUF_SIZE -1)){
			buffer->output = 0; 
		}
		else{
			(buffer->output)++;
		}

	}
}

int main (int argc, char *argv[]) {

	setlinebuf(stdout);

	// create and initialize ringbuffer
	ringbuf_t buffer = {
		.input = 0,
		.output = 0,
		.count = 0
	};
	pthread_mutex_init(&buffer.buf_mutex, NULL);
	pthread_cond_init(&buffer.allow_produce, NULL);
	pthread_cond_init(&buffer.allow_consume, NULL);

	pthread_t producer_thread;
	pthread_t consumer_thread;

	pthread_create(&consumer_thread, NULL, consumer, (void*)&buffer);
	// pthread_create(&producer_thread, NULL, producer, (void*)&buffer);

	producer(&buffer);
	// pthread_join(producer_thread, NULL);
	pthread_join(consumer_thread, NULL);

}


