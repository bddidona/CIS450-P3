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
	/*
		Wait here until head of line is unlocked (for arrival direction)
		then lock head of line in the direction, and determine what the center quadrant will be
		depending on center quadrant the car can now call cross intersection

	*/
}
void CrossIntersection(car thisCar){
	//Arrives to middle, waits until exit lock becomes free
	/*
		Car has arrived and has the lock for the head of line
		Now waits for lock of center quadrant to be unlocked (determined by destination vs arrival)
		lock quadrant of center which car will be in
		arrival can now be unlocked!
		Now depending on destination the car will call exit
		status is "crossing"
	*/
}
void ExitIntersection(car thisCar){
	//Arrives at exit and leaves, unlocks mid/exit locks
	/*
		Here is where the car will pass through the destination and exit
		Waits for lock of destination to be unlocked
		lock exit, car will "exit" and then unlock both middle and exit
		Car has now passed through
	*/
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