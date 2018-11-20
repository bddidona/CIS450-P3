#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>



//Global stuff here


void *tc_thread(void *arg) {


}

int main(int argh, char *argv[]) {

	pthread_t thread;
	
	tc = pthread_create(&thread, NULL, tc_thread);
	pthread_join(&thread);
	
	return 0;
}