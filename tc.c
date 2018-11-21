#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//hardcoded car values
const char arrival[] = "NNNSSNEW";
const char destination[] = "NNWSENNN";
const float times[] = {1.1,2.0,3.3,3.5,4.2,4.4,5.7,5.9};

clock_t timer;
pthread_mutex_t head_of_line_n;
pthread_mutex_t head_of_line_s;
pthread_mutex_t head_of_line_w;
pthread_mutex_t head_of_line_e;
pthread_mutex_t mid_intersection_nw;
pthread_mutex_t mid_intersection_se;
pthread_mutex_t mid_intersection_ne;
pthread_mutex_t mid_intersection_sw;
pthread_mutex_t exit_intersection_n;
pthread_mutex_t exit_intersection_s;
pthread_mutex_t exit_intersection_w;
pthread_mutex_t exit_intersection_e;

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
	//Determines where car arrives, and waits if quadrant of intersection is locked
}
void CrossIntersection(car thisCar){
	//Arrives to middle, waits until exit lock becomes free
}
void ExitIntersection(car thisCar){
	//Arrives at exit and leaves, unlocks mid/exit locks
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