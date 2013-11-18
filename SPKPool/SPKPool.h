#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <map>

const std::string exitTask = "Exit";
const std::string statusTask = "Status";
const std::string threadDestroyTask = "SYS_THREAD_DESTROY";
const std::string observerDestroyTask = "SYS_OBSERVER_DESTROY";

class SPKPool;
struct Task;

typedef void (*PoolProc)(SPKPool *spkPool);
typedef DWORD (*ThreadProc)(std::vector<std::string> arguments);
typedef DWORD (*MultipalThreadProc)(SPKPool *spkPool, std::vector<std::string> arguments);
typedef std::map <std::string, ThreadProc>* (__stdcall *MYPROC)(void);

extern std::map <std::string, ThreadProc> threadProcs;
extern std::map <std::string, MultipalThreadProc> multipalThreadProcs;

class SPKPool 
{
public:

	SPKPool(int minCountOfThread, int maxCountOfThread, double timeLife);
	~SPKPool(void);

	void AddTask(Task *task);

	HANDLE threadManager;
	HANDLE observerThread;

	int minCountOfThread;
	int maxCountOfThread;
	double timeLife;

	bool *isThreadCreated;
	bool *isThreadBusy;

	bool isSPKPoolExit;

	std::ofstream out;

private:

	static DWORD WINAPI ObserverThreadProc(LPVOID *spkPool);
	static DWORD WINAPI ThreadManagerProc(LPVOID *spkPool);
	static DWORD WINAPI TaskHandlerProc(LPVOID *threadId);

	int GetFreeThreadId(void);
	void CreateThreadWithId(int id);
	void DestroyThreadWithId(int id);

	HANDLE *threads;
	HANDLE *startThreadSemaphore;
	HANDLE *workThreadSemaphore;

	HANDLE taskSemaphore;
	std::queue<Task *> tasks;
	Task **taskOfThread;

	time_t *timeOfLastActiveThread;

	int startedCountOfThread;
};
	
struct Task
{
	std::string taskName;
	std::vector<std::string> arguments;
};

struct InfoThread
{
	SPKPool *spkPool;
	int threadId;
};