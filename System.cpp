#include <iostream>
#include <math.h>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <queue>
#include <vector>

struct page
{
	int arrivalT, pID, num, endT, start, end;
};

struct process
{
	std::vector<int> addVector;
	int life_time, spaceNum, memReq, pid, arrival_time, pagesReq;
};

void printMemory(std::vector<page>, int);
void printQueue(std::queue<process>);

int main()
{
	int availableMem, page_Size, memory_Size;

	std::string fileName;
	std::queue<process> Queued, waitingQueue;

	std::cout << "Memory Size: ";

	std::cin >> memory_Size;

	std::cout << "Page Size(1:100, 2:200, 3:400): ";

	std::cin >> page_Size;

	while ((page_Size != 3) && (page_Size != 2) && (page_Size != 1))
	{
		std::cout << "Invalid page size selected!\nEnter a valid number corresponding with the Page Size you would like to choose!\n";
		std::cin >> page_Size;
	}

	if (page_Size == 1)
	{
		page_Size = 100;
	}
	else if (page_Size == 2)
	{
		page_Size = 200;
	}
	else if (page_Size == 3)
	{
		page_Size == 400;
	}

	std::cin.ignore();
	std::cout << "Please enter the name the file: ";

	std::getline(std::cin, fileName);
	std::fstream fin(fileName, std::fstream::in);

	int TProc;
	availableMem = memory_Size / page_Size;

	/*reads and stores information into waitingQueue*/

	if (fin.is_open())
	{
		fin >> TProc;

		//reads information to temp process and then push to Queued

		for (int i = 0; i < TProc; i++)
		{
			process temp;
			temp.memReq = 0;
			fin >> temp.pid >> temp.arrival_time >> temp.life_time >> temp.spaceNum;

			//read space into process address vector and store their total sum in memReq.

			for (int j = 0; j < temp.spaceNum; j++)
			{
				int addr;
				fin >> addr;
				temp.memReq = addr+ temp.memReq;
				temp.addVector.push_back(addr);
			}
			int entire_process = (temp.memReq / page_Size);

			int left_over = 0;

			double leftover_process = temp.memReq % page_Size;

			if (leftover_process > 0)
			{
				left_over = 1;
			}
			temp.pagesReq = left_over + entire_process;
			if (temp.memReq <= memory_Size)
			{							 //ignore processes that need memory
				waitingQueue.push(temp); // larger than all memory
			}

		} //end forloop
		fin.close();
	}

	/*stars clock and enqueues processes in waitqueue into the input queue at when clock hits arrival time. 
	Move process into memory if memory avaiable and keeps track of them.*/
	page initial;
	std::vector<page> memMap;

	int pages = memory_Size / page_Size;
	long t = 0;

	int endID = waitingQueue.back().pid;
	std::vector<int> turnaroundTime;

	//initalize needed pages in memory map with start & end addressees

	for (int k = 0; k < pages; k++)
	{
		initial.num = 0;
		initial.pID = -1;
		initial.start = page_Size*k;
		initial.endT = -1;
		initial.end = ( page_Size*(k + 1)) - 1;
		memMap.push_back(initial);
	}

	int time_limit = 100000;

	while (t != time_limit)
	{

		bool printMap = false;

		bool output = true;

		bool Waiting = false;
		//prints arrival time of process, then puts them into queue and prints queue
		if (!waitingQueue.empty())
		{
			Waiting = (waitingQueue.front().arrival_time == t);
		}
		while (Waiting)
		{

			std::cout << "t = " << t << std::endl;
			std::cout << "\t\tProcess " << waitingQueue.front().pid << " arrives\n";
			Queued.push(waitingQueue.front());
			waitingQueue.pop();
			printQueue(Queued);
			printMap = true;
			output = false;

			if (!waitingQueue.empty())
			{
				Waiting = (waitingQueue.front().arrival_time == t);
			}
			else
			{
				Waiting = false;
			}
		}

		bool processes_p = true;

		//clears out finished processes
		for (int i = 0; i < pages; i++)
		{
			int p = 0;
			if (memMap[i].endT == t)
			{

				p = memMap[i].pID;
				memMap[i].endT = 0;
				memMap[i].num = 0;
				memMap[i].pID = -1;

				availableMem++;
			}
			if ((p != 0) && (processes_p))
			{
				if (output)
				{
					std::cout << "t = " << t << std::endl;
					output = false;
				}
				std::cout << "\t\tProcess " << p << " completes\n";
				processes_p = false;
				printMap = true;
			}
		}

		//checks available memory

		while ((!Queued.empty()) && (availableMem >= Queued.front().pagesReq))
		{
			if (output)
			{
				std::cout << "t = " << t << std::endl;
				output = false;
			}

			std::cout << "\t\tMM moves Process " << Queued.front().pid << " to memory\n";
			printMap = true;
			int endTime = Queued.front().life_time + t;
			int pnum = 1;
			int i = 0;

			if (memMap[i].pID == endID)
			{
				time_limit = endTime;
			}

			memMap[i].arrivalT = Queued.front().arrival_time;
			turnaroundTime.push_back(memMap[i].endT - memMap[i].arrivalT);

			while ( (i < pages) &&(Queued.front().pagesReq != 0) )
			{

				if (memMap[i].pID == -1)
				{
					memMap[i].pID = Queued.front().pid;
					memMap[i].endT = endTime;
					memMap[i].num = pnum;
					pnum =+1;
					availableMem -=1;
					Queued.front().pagesReq -=1;
				}
				i +=1;
			}
			Queued.pop();
		} //end queue

		//output memory map.
		if (printMap)
		{
			printMemory(memMap, pages);
		}
		t++;
	} //close frame

	//calculates the average turnaround time
	int sum = 0;
	for (int i = 0; i < turnaroundTime.size(); i++)
	{
		sum += turnaroundTime[i];
	}
	double turnaround = sum / turnaroundTime.size();

	std::cout << "\nAverage Turnaround Time: " << std::fixed << std::setprecision(2) << turnaround << "\n" << std::endl;

	return 0;
}

//prints memory map status
void printMemory(std::vector<page> mMap, int E_R)
{

	int last = 0;
	bool blank = true;

	std::cout << "\t\tMemory Map: \n";
	int begin = 0;

	for (int i = 0; i < E_R; i++)
	{
		page a = mMap[i];

		if (a.pID == -1)
		{
			if (blank)
			{
				begin = a.start;
				blank = false;
			}
			last = a.end;
			if (i == (E_R - 1))
			{
				std::cout << "\t\t\t" << begin << "-" << last << ": ";

				std::cout << "\tFree Frame(s)\n";
			}
		}
		else
		{
			if (last != 0)
			{
				std::cout << "\t\t\t" << begin << "-" << last << ": ";
				if (i == 1)
				{
					std::cout << "\t";
				}
				std::cout << "\tFree Frame(s)\n";
				blank = true;
				last = 0;
				begin = 0;
				std::cout << "\t\t\t" << a.start << "-" << a.end << ": ";
				std::cout << "\tProcess " << a.pID << ", \tPage " << a.num << std::endl;
			}
			else
			{
				std::cout << "\t\t\t" << a.start << "-" << a.end << ": ";
				if (i == 0)
				{
					std::cout << "\t";
				}
				std::cout << "\tProcess " << a.pID << ", \tPage " << a.num << std::endl;
			}
		}
	}

	std::cout << std::endl;
}

//prints input queue
void printQueue(std::queue<process> input)
{
	std::cout << "\t\tInput Queue: [ ";
	while (!input.empty())
	{
		std::cout << input.front().pid << " ";
		input.pop();
	}
	std::cout << "]\n";
}
