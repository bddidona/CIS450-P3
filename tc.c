#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"


///hardcoded car values
const char arrival[] = "NNNSSNEW";
const char destination[] = "NNWSENNN";
const double times[] = {1.1,2.0,3.3,3.5,4.2,4.4,5.7,5.9};

///used for timing and print outs
double timer;
#define cTime (float)(GetTime() - timer)

///ALL THE LOCKS
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

///adjustable timing, could just be hardcoded
int t_green = 18;
int t_yellow = 2;
int t_red = 10;

int left = 3;
int right = 1;
int straight = 2;

///Used for readability and ease
enum color {Green, Yellow, Red};

///Used for telling number of cars for releasing locks
int northn = 0;
int northe = 0;
int northw = 0;

int souths = 0;
int southe = 0;
int southw = 0;

int westw = 0;
int westn = 0;
int wests = 0;

int easte = 0;
int eastn = 0;
int easts = 0;

///Light struct for checking 
struct light{
	int NS;
	int EW;
}Light;

///Car struct info for passing into functions
typedef struct _car {
	char dir_original;
	char dir_target;
	double time;
	int id;
	int status;
} car;

void ArriveIntersection(car thisCar){
	while(thisCar.time >= cTime);
	///delay for start time
	printf("Time %.1f: Car %d (->%c ->%c) Arriving\n",cTime,thisCar.id,thisCar.dir_original, thisCar.dir_target);
	//Determines where car arrives, and waits if quadrant of intersection is locked
	/*
		Wait here until head of line is unlocked (for arrival direction)
		then lock head of line in the direction, and determine what the center quadrant will be
		depending on center quadrant the car can now call cross intersection

	*/
	///claim lock needed for the desired direction
	switch(thisCar.dir_original){
		case 'N':pthread_mutex_lock(&head_of_line_n);
			switch(thisCar.dir_target){
				case 'N':while(Light.NS != Green);
					if(northn++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_ne);
						pthread_mutex_lock(&exit_intersection_n);
						if(northe == 0)
							pthread_mutex_lock(&mid_intersection_se);
					}
					break;
				case 'W':while(Light.NS != Green);
					if(northw++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_sw);
						pthread_mutex_lock(&exit_intersection_w);
					}
					break;
				case 'E':
					if(northe++ == 0){
						pthread_mutex_lock(&exit_intersection_e);
						if(northn == 0)
							pthread_mutex_lock(&mid_intersection_se);
					}
					break;
				}
			break;
		case 'S':pthread_mutex_lock(&head_of_line_s);
			switch(thisCar.dir_target){
				case 'S':while(Light.NS != Green);
					if(souths++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_sw);
						pthread_mutex_lock(&exit_intersection_s);
						if(southw == 0)
							pthread_mutex_lock(&mid_intersection_nw);
					}
					break;
				case 'E':while(Light.NS != Green);
					if(southe++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_se);
						pthread_mutex_lock(&exit_intersection_e);
					}
					break;
				case 'W':
					if(southw++ == 0){
						pthread_mutex_lock(&exit_intersection_w);
						if(souths == 0)
							pthread_mutex_lock(&mid_intersection_nw);
					}
					break;
				}
			break;
		case 'E':pthread_mutex_lock(&head_of_line_e);
			switch(thisCar.dir_target){
				case 'E':while(Light.EW != Green);
					if(easte++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_se);
						pthread_mutex_lock(&exit_intersection_e);
						if(easts == 0)
							pthread_mutex_lock(&mid_intersection_sw);
					}
					break;
				case 'N':while(Light.EW != Green);
					if(eastn++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_nw);
						pthread_mutex_lock(&exit_intersection_n);
					}
					break;
				case 'S':
					if(easts++ == 0)
					{
						pthread_mutex_lock(&exit_intersection_s);
						if(easte == 0)
							pthread_mutex_lock(&mid_intersection_sw);
					}
					break;
				}
			break;
		case 'W':pthread_mutex_lock(&head_of_line_w);
			switch(thisCar.dir_target){
				case 'W':while(Light.EW != Green);
					if(westw++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_nw);
						pthread_mutex_lock(&exit_intersection_w);
						if(westn == 0)
							pthread_mutex_lock(&mid_intersection_ne);
					}
					break;
				case 'S':while(Light.EW != Green);
					if(wests++ == 0)
					{
						pthread_mutex_lock(&mid_intersection_sw);
						pthread_mutex_lock(&exit_intersection_s);
					}
					break;
				case 'N':
					if(westn++ == 0)
					{
						pthread_mutex_lock(&exit_intersection_n);
						if(easte == 0)
							pthread_mutex_lock(&mid_intersection_ne);
					}
					break;
				}
			break;
	}
}

void CrossIntersection(car thisCar){
	printf("Time %.1f: Car %d (->%c ->%c) Crossing\n",cTime,thisCar.id,thisCar.dir_original, thisCar.dir_target);
	///Release Head of line lock
	switch(thisCar.dir_original){
		case 'N':pthread_mutex_unlock(&head_of_line_n);
			break;
		case 'S':pthread_mutex_unlock(&head_of_line_s);
			break;
		case 'E':pthread_mutex_unlock(&head_of_line_e);
			break;
		case 'W':pthread_mutex_unlock(&head_of_line_w);
			break;
	}
	///I think that once the car starts moving it would have aquire all locks for its lane
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
	///Determine how long the car "takes" to cross intersection
	switch(thisCar.dir_original){
		case 'N':switch(thisCar.dir_target){
				case 'N':Spin(straight);break;
				case 'E':Spin(right);break;
				case 'W':Spin(left);break;
			}break;
		case 'S':switch(thisCar.dir_target){
				case 'S':Spin(straight);break;
				case 'E':Spin(left);break;
				case 'W':Spin(right);break;
			}break;
		case 'E':switch(thisCar.dir_target){
				case 'S':Spin(right);break;
				case 'E':Spin(straight);break;
				case 'N':Spin(left);break;
			}break;
		case 'W':switch(thisCar.dir_target){
				case 'S':Spin(left);break;
				case 'N':Spin(right);break;
				case 'W':Spin(straight);break;
			}break;
	}
	
	///If using exit locks they would be waited for here.
}

void ExitIntersection(car thisCar){
	///Arrives at exit and leaves
	///unlocks the locks for the direction depending on the number of cars
	switch(thisCar.dir_original){
		case 'N':
			switch(thisCar.dir_target){
				case 'N':
					///increase a counter and lock places only when the first
					if(--northn == 0)
					{
						pthread_mutex_unlock(&mid_intersection_ne);
						pthread_mutex_unlock(&exit_intersection_n);
						if(northe == 0)
							pthread_mutex_unlock(&mid_intersection_se);
					}
					break;
				case 'W':
					///increase a counter and lock places only when the first
					if(--northw == 0)
					{
						pthread_mutex_unlock(&mid_intersection_sw);
						pthread_mutex_unlock(&exit_intersection_w);
					}
					break;
				case 'E':
					///increase a counter and lock places only when the first
					if(--northe == 0)
					{
						pthread_mutex_unlock(&exit_intersection_e);
						if(northn == 0)
							pthread_mutex_unlock(&mid_intersection_se);
					}
					break;
				}
			break;
		case 'S':
			switch(thisCar.dir_target){
				case 'S':
					///increase a counter and lock places only when the first
					if(--souths == 0)
					{
						pthread_mutex_unlock(&mid_intersection_sw);
						pthread_mutex_unlock(&exit_intersection_s);
						if(southw == 0)
							pthread_mutex_unlock(&mid_intersection_nw);
					}
					break;
				case 'E':
					///increase a counter and lock places only when the first
					if(--southe == 0)
					{
						pthread_mutex_unlock(&mid_intersection_se);
						pthread_mutex_unlock(&exit_intersection_e);
					}
					break;
				case 'W':
					///increase a counter and lock places only when the first
					if(--southw == 0)
					{
						pthread_mutex_unlock(&mid_intersection_nw);
						pthread_mutex_unlock(&exit_intersection_w);
						if(souths == 0)
							pthread_mutex_unlock(&mid_intersection_nw);
					}
					break;
				}
			break;
		case 'E':
			switch(thisCar.dir_target){
				case 'E':
					if(--easte == 0)
					{
						pthread_mutex_unlock(&mid_intersection_se);
						pthread_mutex_unlock(&exit_intersection_e);
						if(easts == 0)
							pthread_mutex_unlock(&mid_intersection_sw);
					}
					break;
				case 'N':
					if(--eastn == 0)
					{
						pthread_mutex_unlock(&mid_intersection_nw);
						pthread_mutex_unlock(&exit_intersection_n);
					}
					break;
				case 'S':
					if(--easts == 0)
					{
						pthread_mutex_unlock(&exit_intersection_s);
						if(easte == 0)
							pthread_mutex_unlock(&mid_intersection_sw);
					}
					break;
				}
			break;
		case 'W':
			switch(thisCar.dir_target){
				case 'W':while(Light.EW != Green);
					if(--westw == 0)
					{
						pthread_mutex_unlock(&mid_intersection_nw);
						pthread_mutex_unlock(&exit_intersection_w);
						if(westn == 0)
							pthread_mutex_unlock(&mid_intersection_ne);
					}
					break;
				case 'S':while(Light.EW != Green);
					if(--wests == 0)
					{
						pthread_mutex_unlock(&mid_intersection_sw);
						pthread_mutex_unlock(&exit_intersection_s);
					}
					break;
				case 'N':
					if(--westn == 0)
					{
						pthread_mutex_unlock(&exit_intersection_n);
						if(easte == 0)
							pthread_mutex_unlock(&mid_intersection_ne);
					}
					break;
				}
			break;
	}
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
	Spin(t_green);
	printf("Light Change NS->Yellow %.1f\n",cTime);
	Light.NS = Yellow;
	Spin(t_yellow);
	printf("Light Change NS->Red %.1f\n",cTime);
	Light.NS = Red;
	Light.EW = Green;
	Spin(t_red);
	printf("Traffic Finished %.1f\n",cTime);
}

int main(int argh, char *argv[]) {

	pthread_t thread[9];
	int i;
	int ids[8];
	
	Light.NS = Red; ///pause all directions
	Light.EW = Red;
	
	timer = GetTime();///start global timer
	
	pthread_create(&thread[8], NULL, *tc_thread,NULL); ///start thread for traffic control

	for(i=0;i<8;i++) ///creates threads for each car
	{
		ids[i] = i;
		pthread_create(&thread[i], NULL, *Car,&ids[i]);
	}
	///removed join for cars because if it takes longer than the lights it won't even make it through the lights.
	pthread_join(thread[8],NULL);	//wait until lights/simulation finish
	return 0;
}