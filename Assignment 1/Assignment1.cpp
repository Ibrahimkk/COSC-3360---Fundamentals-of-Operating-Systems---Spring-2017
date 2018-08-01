#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>

using namespace std;


enum processStatus { ready = 0, running = 1, blocked = 2, terminated = 3, notStarted = 4};

struct processes //a struct containing the variables I need for every process, such as start time, completion time, the process type and the process time, as well as it's current status.
{
	int startTime;
	int processNumber;
	int completionTime;
	queue<string> processType;
	queue<int> processTime;
	processStatus currentStatus;
};

void checkstatus(processes temp)
{
	cout << "Process " << temp.processNumber << " " << "Started at " << temp.startTime << " " << "MS" << " " << "and is: ";
	if (temp.currentStatus == 0)
	{
		cout << "Ready." << endl;
	}
	else if (temp.currentStatus == 1)
	{
		cout << "Running." << endl;
	}
	else if (temp.currentStatus == 2)
	{
		cout << "Blocked." << endl;
	}
	else if (temp.currentStatus == 3)
	{
		cout << "Terminated." << endl;
	}
}

//Global variables so I can access them from anywhere.
int cores;
int timeSlice;
int processIndex = -1;
int processDone = -1;


int main(int argc, char* argv[])
{
	//The variables, and the queue's/deque's i'm going to be using for reading in the input and for the round robin.
	string eventType;
	string event;
	int time = 0;
	int currentTime = 0;	
	deque<processes> readyQueue;
	deque<processes> diskQueue;
	deque<processes> blockedQueue;
	deque<processes> terminatedQueue;
	queue<processes> disk;
	queue<processes> theProcesses;
	vector<processes> processor;


	if (cin.fail())
	{
		cout << "Invalid input file! Please try again with a valid input file." << endl;
	}
	cin >> event;
	if (event == "NCORES")
	{
		cin >> cores;
	}
	cin >> event;
	if (event == "SLICE")
	{
		cin >> timeSlice;
	}

	while (!cin.eof()) //Reading in the input and putting it in a queue while it is not end of file yet.
	{
		cin >> event;
		cin >> time;
		eventType = event;
		if (event == "NEW") //initalizing all the new processes and setting their start time, current status, their process number, and their completion time.
		{
			processIndex++;
			theProcesses.push(processes());
			theProcesses.back().startTime = time;
			theProcesses.back().completionTime = time;
			theProcesses.back().currentStatus = notStarted;
			theProcesses.back().processNumber = processIndex;
		}
		else
		{
			theProcesses.back().processType.push(eventType);
			theProcesses.back().processTime.push(time);
		}
	}

	while (processDone != processIndex) //this will go on until the processes completed = the number of processes read in initially.
	{
		while (!theProcesses.empty()) //while there is still some processes in the inital queue that I put everything into while reading in the file.
		{
			if (currentTime == theProcesses.front().startTime) //it is putting all the new processes in the right queue depending on the process type, whenever their start time is reached. 
			{
				if (theProcesses.front().processType.front() == "CORE")
				{
					theProcesses.front().currentStatus = ready;
					readyQueue.push_back(theProcesses.front());
					theProcesses.pop();
				}
				else if (theProcesses.front().processType.front() == "DISK")
				{
					theProcesses.front().currentStatus = blocked;
					diskQueue.push_back(theProcesses.front());
					theProcesses.pop();
				}
				else if (theProcesses.front().processType.front() == "DISPLAY" || theProcesses.front().processType.front() == "INPUT")
				{
					theProcesses.front().currentStatus = blocked;
					theProcesses.front().completionTime = currentTime + theProcesses.front().processTime.front();
					blockedQueue.push_back(theProcesses.front());
					theProcesses.pop();
				}
			}
			else //if the start time of the processes is not = to the current time, then the currentTime is set to the smallest start time, so that the first process can go through.
			{
				currentTime = min(currentTime, theProcesses.front().startTime);
				break;
			}
		}

		while (!readyQueue.empty() && processor.size() != cores) //while the ready queue is not empty and the processor is not full, it is changing the status of the process and then filling up the processor, as well as updating the completion time, depending on the process time of the process.
		{
			readyQueue.front().currentStatus = running;
			if (readyQueue.front().processTime.front() > timeSlice)
			{
				readyQueue.front().completionTime = currentTime + timeSlice;
			}
			else
			{
				readyQueue.front().completionTime = currentTime + readyQueue.front().processTime.front();
			}
			processor.push_back(readyQueue.front());
			readyQueue.pop_front();
		}

		if (!diskQueue.empty()) //this is pushing a process into the disk if the disk queue is not empty and the disk isn't currently doing something on a process, it is also updating completion time of that process
			{
				if (disk.empty())
				{
					diskQueue.front().completionTime = currentTime + diskQueue.front().processTime.front();
					disk.push(diskQueue.front());
					diskQueue.pop_front();
				}
			}

		if (!terminatedQueue.empty()) //if the terminated queue is not empty, means a process has terminated, and it is time to output the current time and the status of all the processes available.
		{
			{
				cout << "THE CURRENT STATE OF THE SYSTEM AT t = " << currentTime << endl;
				if (processor.empty())
				{
					cout << "Number of busy cores: " << "0" << endl;
				}
				else
				{
					cout << "Number of busy cores: " << processor.size() << endl;
				}

				if (readyQueue.empty())
				{
					cout << "READY QUEUE: " << endl;
					cout << "empty" << endl;
				}
				else
				{
					cout << "READY QUEUE: " << endl;
					for (int pNum = 0; pNum < readyQueue.size(); pNum++)
					{
						cout << "process" << readyQueue.at(pNum).processNumber << endl;
					}
				}
				if (diskQueue.empty())
				{
					cout << "DISK QUEUE: " << endl;
					cout << "empty" << endl;
				}
				else
				{
					cout << "DISK QUEUE: " << endl;
					for (int pNum = 0; pNum < diskQueue.size(); pNum++)
					{
						cout << "process" << diskQueue.at(pNum).processNumber << endl;
					}
				}
				cout << "PROCESS TABLE: " << endl;
				{
					if (!terminatedQueue.empty())
					{
						for (int j = 0; j < terminatedQueue.size(); j++)
						checkstatus(terminatedQueue.at(j));
					}
					if (!processor.empty())
					{
						for (int j = 0; j < processor.size(); j++)
						checkstatus(processor.at(j));
					}
					if (!diskQueue.empty())
					{
						for (int j = 0; j < diskQueue.size(); j++)
						checkstatus(diskQueue.at(j));
					}
					if (!readyQueue.empty())
					{
						for (int j = 0; j < readyQueue.size(); j++)
						checkstatus(readyQueue.at(j));
					}
					if (!blockedQueue.empty())
					{
						for (int j = 0; j < blockedQueue.size(); j++)
						checkstatus(blockedQueue.at(j));
					}
					if (!disk.empty())
					{
						checkstatus(disk.front());
					}
				}
			}
			cout << endl;
			terminatedQueue.pop_front();
			processDone++;
		}
		currentTime = 0x7FFFFFFF; //currentTime is set to an extremely large number, because if it isn't, there is a possibility that it is smaller than the smallest completion time, and the currentTime will get stuck at said number, so it has to be huge for this to work.
		if (!theProcesses.empty())
		{
			currentTime = min(currentTime, theProcesses.front().completionTime);
		}
		if (!disk.empty())
		{ 	
			currentTime = min(currentTime, disk.front().completionTime);
		}
		if (!processor.empty())
		{
			for (int i = 0; i < processor.size(); i++)
			{
			currentTime = min(currentTime, processor.at(i).completionTime);	
			}
		}
		if (!blockedQueue.empty())
		{
			for (int i = 0; i < blockedQueue.size(); i++)
			{
			currentTime = min(currentTime, blockedQueue.at(i).completionTime);
			}
		}
	
		if (!processor.empty()) //if the processor is not empty, then depending on whether the processtime is greater than the timeSlice or less than it, it will either pop the processtime and move onto the next queue or terminated queue. Or it will decrement the processtime by the timeSlice and push it back into the readyQueue to wait for more processor time.
		{
			for (int i = 0; i<processor.size(); i++)
			{
				if (processor.at(i).processTime.front() > timeSlice)
				{
					if (processor.at(i).completionTime == currentTime)
					{
						processor.at(i).processTime.front() -= timeSlice;
						processor.at(i).currentStatus = ready;
						readyQueue.push_back(processor.at(i));
						processor.erase(processor.begin() + i);
						i--;
					}
				}
				else
				{
					if (processor.at(i).completionTime == currentTime)
					{
						processor.at(i).processType.pop();
						processor.at(i).processTime.pop();
						if (processor.at(i).processType.empty())
						{
							processor.at(i).currentStatus = terminated;
							terminatedQueue.push_back(processor.at(i));
							processor.erase(processor.begin() + i);
							i--;
						}
						else if (processor.at(i).processType.front() == "DISK")
						{
							processor.at(i).currentStatus = blocked;
							diskQueue.push_back(processor.at(i));
							processor.erase(processor.begin() + i);
							i--;
						}
						else if (processor.at(i).processType.front() == "INPUT" || processor.at(i).processType.front() == "DISPLAY")
						{
							processor.at(i).currentStatus = blocked;
							processor.at(i).completionTime = currentTime + processor.at(i).processTime.front();
							blockedQueue.push_back(processor.at(i));
							processor.erase(processor.begin() + i);
							i--;
						}
						else if (processor.at(i).processType.front() == "CORE")
						{
							processor.at(i).currentStatus = ready;
							readyQueue.push_back(processor.at(i));
							processor.erase(processor.begin() + i);
							i--;
						}
					}
				}
			}
		}
		if (!disk.empty()) //if the disk is not empty then it will wait until completionTime of that process and pop it from the disk, update the current status, and move it to the next queue or terminated queue.
		{
			if (disk.front().completionTime == currentTime)
			{
				disk.front().processType.pop();
				disk.front().processTime.pop();
				if (disk.front().processType.empty())
				{
					disk.front().currentStatus = terminated;
					terminatedQueue.push_back(disk.front());
					disk.pop();
				}
				else if (disk.front().processType.front() == "INPUT" || disk.front().processType.front() == "DISPLAY")
				{
					disk.front().completionTime = currentTime + disk.front().processTime.front();
					blockedQueue.push_back(disk.front());
					disk.pop();
				}
				else if (disk.front().processType.front() == "DISK")
				{
					diskQueue.push_back(disk.front());
					disk.pop();
				}
				else if (disk.front().processType.front() == "CORE")
				{
					disk.front().currentStatus = ready;
					readyQueue.push_back(disk.front());
					disk.pop();
				}
			}
		}
		if (!blockedQueue.empty()) //if the blockedQueue is not empty, it will for all the processes inside, one by one, wait for the completion time and then pop the processtype and time, and then update the status and either move it to the terminated queue or the next queue depending on the processtype.
		{
			for (int i = 0; i < blockedQueue.size(); i++)
			{
				if (blockedQueue.at(i).completionTime == currentTime)
				{	
						blockedQueue.at(i).processType.pop();
						blockedQueue.at(i).processTime.pop();
					if (blockedQueue.at(i).processType.empty())
					{
						blockedQueue.at(i).currentStatus = terminated;
						terminatedQueue.push_back(blockedQueue.at(i));
						blockedQueue.erase(blockedQueue.begin()+i);
						i--;
					}
					else if (blockedQueue.at(i).processType.front() == "CORE")
					{
						blockedQueue.at(i).currentStatus = ready;
						readyQueue.push_back(blockedQueue.at(i));
						blockedQueue.erase(blockedQueue.begin()+i);
						i--;
					}
					else if (blockedQueue.at(i).processType.front() == "DISK")
					{
						diskQueue.push_back(blockedQueue.at(i));
						blockedQueue.erase(blockedQueue.begin()+i);
						i--;
					}
					else if (blockedQueue.at(i).processType.front() == "DISPLAY" || blockedQueue.at(i).processType.front() == "INPUT")
					{
						blockedQueue.at(i).completionTime = currentTime + blockedQueue.at(i).processTime.front();
						blockedQueue.push_back(blockedQueue.at(i));
						blockedQueue.erase(blockedQueue.begin()+i);
						i--;
					}
				}
			}
		}
	} // end of the round robin.
	return 0;
} // end of program