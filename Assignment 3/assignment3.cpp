#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <unistd.h>
#include <queue>
#include <sstream>

using namespace std;

static int currentWeight;
static int maxweight;
static pthread_mutex_t mylock;
static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;
int counter = 0;

struct vehicles
{
	string vid;
	int arrivalDelay = 0;
	int crossingTime = 0;
	int vehicleWeight = 0;
};

bool enterBridge(int weight, string vehicleid)
{
	if (weight > maxweight)
	{
		return false;
	}
	pthread_mutex_lock(&mylock);
	while ((weight + currentWeight) > maxweight)
	{
		pthread_cond_wait(&ok, &mylock);
	}
	currentWeight += weight;
	cout << "Vehicle: " << vehicleid << " " << "Is crossing the bridge." << " The current bridge load is: " << currentWeight << endl;
	pthread_cond_signal(&ok);
	pthread_mutex_unlock(&mylock);
	return true;
}

void leaveBridge(int weight, string vehicleid)
{
	pthread_mutex_lock(&mylock);
		currentWeight -= weight;
		cout << "Vehicle: " << vehicleid << " " << "Has crossed the bridge." << " The current bridge load is: " << currentWeight << endl;
		pthread_cond_signal(&ok);
	pthread_mutex_unlock(&mylock);
}

void *vehicle(void *arg)
{
	vehicles v = *((vehicles*)arg);
	cout << "Vehicle: " << v.vid << " " << "Has arrived at the bridge." << " The current bridge load is: " << currentWeight << endl;
	if (enterBridge(v.vehicleWeight, v.vid) == false)
	{
		cout << "Vehicle: " << v.vid << " " << "Weight exceeds the max bridge weight." << endl;
	}
	else
	{
		sleep(v.crossingTime);
		leaveBridge(v.vehicleWeight, v.vid);
	}

	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	pthread_mutex_init(&mylock, NULL);
	string input;
	deque<vehicles> allcars;

	if (argc < 2)
	{
		cout << "You did not enter enough parameters, please try again" << endl;
		return 0;
	}

	maxweight = atoi(argv[1]);

	cout << "The Maximum Bridge Load is: " << maxweight << " tons." << endl;


	while(getline(cin, input) && input.length() != 0)
	{
		vehicles v;
		
		stringstream ss(input);
		
		ss >> v.vid;
		ss >> v.arrivalDelay;
		ss >> v.vehicleWeight;
		ss >> v.crossingTime;

		allcars.push_back(v);
		counter++;
	}

	deque<pthread_t> children;

	for (int k = 0; k < allcars.size(); k++)
	{
	sleep(allcars.at(k).arrivalDelay);
	pthread_t tid;
	pthread_create(&tid, NULL, vehicle, (void *) &allcars.at(k));
	children.push_back(tid);
	}


	for (int n = 0; n < children.size(); n++)
	{
		pthread_join(children[n],NULL);
	}

	cout << "The total number of vehicles is: " << counter << endl;

	return 0;
}