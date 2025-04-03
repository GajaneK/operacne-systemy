#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024
typedef struct {
	char data[BUFFER_SIZE];
	int count;
	int read_index;
	int write_index;
	pthread_mutex_t mutex;
	pthread_cond_t can_produce;
	pthread_cond_t can_consume;
	int fin;
} buffer_t;

buffer_t buffer = {
	.count = 0,
	.read_index = 0,
	.write_index = 0,
	.fin = 0,
	.mutex = PTHREAD_MUTEX_INITIALIZER,
	.can_produce = PTHREAD_COND_INITIALIZER,
	.can_consume = PTHREAD_COND_INITIALIZER
};

void *reader_thread() {
	char tmp[BUFFER_SIZE];
	ssize_t bytes_read;
	while ((bytes_read = read(STDIN_FILENO, tmp, BUFFER_SIZE)) > 0){
		pthread_mutex_lock(&buffer.mutex);
		for (ssize_t i = 0; i < bytes_read; i++) {
			while (buffer.count == BUFFER_SIZE) {
				pthread_cond_wait(&buffer.can_produce, &buffer.mutex);
			}
			buffer.data[buffer.write_index] = tmp[i];
			buffer.write_index = (buffer.write_index +1) % BUFFER_SIZE;
			buffer.count++;
			pthread_cond_signal(&buffer.can_consume);
		}
		pthread_mutex_unlock(&buffer.mutex);
	}
	pthread_mutex_lock(&buffer.mutex);
	buffer.fin = 1;
	pthread_cond_signal(&buffer.can_consume);
	pthread_mutex_unlock(&buffer.mutex);
	return NULL;
}

void *writer_thread() {
	while(1) {
		pthread_mutex_lock(&buffer.mutex);
		while (buffer.count == 0 && !buffer.fin) {
			pthread_cond_wait(&buffer.can_consume, &buffer.mutex);
		}
		if (buffer.count == 0 && buffer.fin) {
			pthread_mutex_unlock(&buffer.mutex);
			break;
		}
		char c = buffer.data[buffer.read_index];
		buffer.read_index = (buffer.read_index +1) % BUFFER_SIZE;
		buffer.count--;
		pthread_cond_signal(&buffer.can_produce);

		pthread_mutex_unlock(&buffer.mutex);
		write(STDOUT_FILENO, &c, 1);
	}
	return NULL;
}

int main() {
	pthread_t reader, writer;
	pthread_create(&reader, NULL, reader_thread, NULL);
	pthread_create(&writer, NULL, writer_thread, NULL);

	pthread_join(reader, NULL);
	pthread_join(writer, NULL);
	return 0;
}


