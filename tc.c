#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//hardcoded car values
const char arrival[] = "NNNSSNEW";
const char destination[] = "NNWSENNN";
const float times[] = {1.1,2.0,3.3,3.5,4.2,4.4,5.7,5.9};

clock_t timer;

#define cTime (clock() - timer)/(float)CLOCKS_PER_SEC

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
int t_red = 10;

int left = 3;
int right = 1;
int straight = 2;

enum color {Green, Yellow, Red};

struct light{
	int NS;
	int EW;
}Light;

//Global stuff here
typedef struct _car {
	char dir_original;
	char dir_target;
	float time;
	int id;
	int status;
} car;

void sleep(float time){
	float temp = clock();
	while(temp+time*CLOCKS_PER_SEC > clock());
}

void ArriveIntersection(car thisCar){
	while(timer + thisCar.time*CLOCKS_PER_SEC > clock());
	printf("Time %.1f: Car %d (->%c ->%c) Arriving\n",cTime,thisCar.id,thisCar.dir_original, thisCar.dir_target);
	///delay for start time
	//Determines where car arrives, and waits if quadrant of intersection is locked
	/*
		Wait here until head of line is unlocked (for arrival direction)
		then lock head of line in the direction, and determine what the center quadrant will be
		depending on center quadrant the car can now call cross intersection

	*/
	///It should determine if it can go depending on the light
	switch(thisCar.dir_original){
		case 'N':
		case 'S':
				while(Light.NS != Green);///replace when turning is firgured out
				break;
		case 'E':
		case 'W':
				while(Light.EW != Green);///see above
				break;
	}
	///Then aquires the semaphores for the move it's making
}
void CrossIntersection(car thisCar){
	printf("Time %.1f: Car %d (->%c ->%c) Crossing\n",cTime,thisCar.id,thisCar.dir_original, thisCar.dir_target);
	///Release Head of line lock
	
	///I think that once the car starts moving it would aquire all locks for its lane
	///ie once striaght traffic starts a car can't turn right into it's lane
	///this would result in a car stopping in the intersection
	///though it's more of a minor detail at this point
	//Arrives to middle, waits until exit lock becomes free
	/*
		Car has arrived and has the lock for the head of line
		Now waits for lock of center quadrant to be unlocked (determined by destination vs arrival)
		lock quadrant of center which car will be in
		arrival can now be unlocked!
		Now depending on destination the car will call exit
		status is "crossing"
	*/
	///this select how long a car should sleep base on direction;
	///there is a library with a proper function but for now this one i made will do
	switch(thisCar.dir_original){
		case 'N':switch(thisCar.dir_target){
				case 'N':sleep(straight);break;
				case 'E':sleep(right);break;
				case 'W':sleep(left);break;
			}break;
		case 'S':switch(thisCar.dir_target){
				case 'S':sleep(straight);break;
				case 'E':sleep(left);break;
				case 'W':sleep(right);break;
			}break;
		case 'E':switch(thisCar.dir_target){
				case 'S':sleep(right);break;
				case 'E':sleep(straight);break;
				case 'N':sleep(left);break;
			}break;
		case 'W':switch(thisCar.dir_target){
				case 'S':sleep(left);break;
				case 'N':sleep(right);break;
				case 'W':sleep(straight);break;
			}break;
	}
	///After this either nothing is needed or
	///we wait on the exit lock
	///it all depends on how we give the cars the locks
}
void ExitIntersection(car thisCar){
	///Arrives at exit and leaves, unlocks mid/exit locks
	///all this function does is release all remaining locks
	/*
		Here is where the car will pass through the destination and exit
		Waits for lock of destination to be unlocked
		lock exit, car will "exit" and then unlock both middle and exit
		Car has now passed through - status is now 0
	*/
	printf("Time %.1f: Car %d (->%c ->%c) Exiting\n",cTime,thisCar.id,thisCar.dir_original, thisCar.dir_target);
}

void *Car(void * vid) {
	car thisCar;
	int id = *((int*)vid);		//make value easier to uses
	thisCar.dir_original = arrival[id];
	thisCar.dir_target = destination[id];
	thisCar.time = times[id];
	thisCar.id = id;
	thisCar.status = 1;
	
	ArriveIntersection(thisCar);
	CrossIntersection(thisCar);
	ExitIntersection(thisCar);
}

void *tc_thread(void * id) {
	Light.NS = Green;
	sleep(t_green);
	printf("Light Change NS->Yellow\n");
	Light.NS = Yellow;
	sleep(t_yellow);
	printf("Light Change NS->Red\n");
	Light.NS = Red;
	Light.EW = Green;
	sleep(t_red);
	printf("Traffic Finished\n");
}

int main(int argh, char *argv[]) {

	pthread_t thread[9];
	int i;
	int ids[8];
	timer = clock(); ///used for delays in car arrival
	
	Light.NS = Red; ///initalize the lights
	Light.EW = Red;
	
	pthread_create(&thread[8], NULL, *tc_thread,NULL); ///thread for lights
	for(i=0;i<8;i++) ///creates threads for each car
	{
		ids[i] = i;
		pthread_create(&thread[i], NULL, *Car,&ids[i]);
	}
	
	for(i=0;i<8;i++) //waits for all cars to finish
		pthread_join(thread[i],NULL);
	pthread_join(thread[8],NULL);	//wait until lights/simulation finish
	return 0;
}