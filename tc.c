#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//hardcoded car values
char arrival[] = "NNNSSNEW";
char destination[] = "NNWSENNN";
float times[] = {1.1,2.0,3.3,3.5,4.2,4.4,5.7,5.9};

clock_t timer;

int t_green = 18;
int t_yellow = 2;
int t_red = 20;

int left = 3;
int right = 2;
int straight = 1;


//Global stuff here
typedef struct _car {
	char dir_original;
	char dir_target;
	float time;
	int id;
} car;

void ArriveIntersection(car thisCar){

}
void CrossIntersection(car thisCar){

}
void ExitIntersection(car thisCar){

}

void *Car(void * vid) {
	car thisCar;
	int id = *((int*)vid);		//make value easier to uses
	thisCar.dir_original = arrival[id];
	thisCar.dir_target = destination[id];
	thisCar.time = times[id];
	thisCar.id = id;
	
	ArriveIntersection(thisCar);
	CrossIntersection(thisCar);
	ExitIntersection(thisCar);
	
	//testing of timings and value filling	
	while(timer + thisCar.time*CLOCKS_PER_SEC > clock());//this moves to start of arrive later
	printf("Car Id:%d Time:%.1f Start:%c End:%c \n",thisCar.id,thisCar.time, thisCar.dir_original, thisCar.dir_target);
}

void *tc_thread(void * id) {
	int * temp = (int*)id;
	return 0;

}

int main(int argh, char *argv[]) {

	pthread_t thread[8];
	int i;
	int ids[8];
	timer = clock(); //used for delays in car arrival

	for(i=0;i<8;i++) //creates threads for each car
	{
		ids[i] = i;
		pthread_create(&thread[i], NULL, *Car,&ids[i]);
	}
	
	for(i=0;i<8;i++) //waits for all cars to finish
	{
		pthread_join(thread[i],NULL);
	}
	
	
	return 0;
}
