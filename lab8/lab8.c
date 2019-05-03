// Z'vonty Flugence (zaf17)
#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <pthread.h>

#define streq(a, b) (strcmp((a), (b)) == 0)

void exit_main_thread();
void show_status();
void set_alarm(long duration);

void get_line(char* input, int size) {
	fgets(input, size, stdin);
	int len = strlen(input);
	if(len > 0)
		input[len - 1] = '\0';
}

int main(int argc, char** argv) {
	while(true) {
		printf("> ");
		char buf[100];
		get_line(buf, sizeof(buf));

		char* saveptr;
		char* command = strtok_r(buf, " ", &saveptr);

		if(command == NULL) {
			continue;
		} else if(streq(command, "exit")) {
			exit_main_thread();
		} else if(streq(command, "status")) {
			show_status();
		} else if(streq(command, "alarm")) {
			char* duration = strtok_r(NULL, " ", &saveptr);

			if(duration == NULL) {
				printf("please give a duration in seconds.\n");
			} else {
				set_alarm(atoi(duration));
			}
		}
	}
	return 0;
}

int num_threads = 0;
pthread_mutex_t num_threads_mutex = PTHREAD_MUTEX_INITIALIZER;

void change_thread_counter(int delta) {
	pthread_mutex_lock(&num_threads_mutex);
	num_threads += delta;
	pthread_mutex_unlock(&num_threads_mutex);
}

void exit_main_thread() {
	if(num_threads > 0) {
		printf("still %d alarm(s) pending.\n", num_threads);
	}
	pthread_exit(NULL);
}

void show_status() {
	printf("%d alarm(s) pending.\n", num_threads);
}

void* alarm_main(void* ctx) {
	long duration = (long)ctx;
	sleep(duration);
	printf("(RING RING!)\n");
	change_thread_counter(-1);
	return NULL;
}

void set_alarm(long duration) {
	if(duration <= 0) {
		printf("Duration must be greater than zero!\n");
	} else {
		pthread_t alarm;
		pthread_create(&alarm, NULL, &alarm_main, (void*)duration);
		change_thread_counter(1);
	}
}