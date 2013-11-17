#pragma once
#include "SPKPool.h"

SPKPool::SPKPool(int minCountOfThread, int maxCountOfThread, double timeLife)
{
	this->minCountOfThread = minCountOfThread;
	this->maxCountOfThread = maxCountOfThread;
	this->timeLife = timeLife;
	startedCountOfThread = 0;
	isSPKPoolExit = true;

	threads = new HANDLE[maxCountOfThread];
	startThreadSemaphore = new HANDLE[maxCountOfThread];
	workThreadSemaphore = new HANDLE[maxCountOfThread];
	
	isThreadCreated = new bool[maxCountOfThread];
	isThreadBusy = new bool[maxCountOfThread];
	
	taskOfThread = new Task*[maxCountOfThread];
	timeOfLastActiveThread = new time_t[maxCountOfThread];

	for (int i = 0; i < maxCountOfThread; i++)
	{
		threads[i] = NULL;
		startThreadSemaphore[i] = CreateSemaphore(NULL, 0, 1, NULL);
		workThreadSemaphore[i] = CreateSemaphore(NULL, 1, 1, NULL);
		isThreadCreated[i] = false;
		isThreadBusy[i] = false;
		taskOfThread[i] = NULL;
		timeOfLastActiveThread[i] = NULL;
	}

	taskSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	observerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ObserverThreadProc,
		(LPVOID *)this, 0, NULL);
	threadManager = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadManagerProc,
		(LPVOID *)this, 0, NULL);

	out.open("d:\\result.txt");
}

SPKPool::~SPKPool(void) 
{
	for (int i = 0; i < maxCountOfThread; i++)
	{
		if (isThreadCreated[i])
		{
			WaitForSingleObject(workThreadSemaphore[i], INFINITE);
			DestroyThreadWithId(i);
		}
	}

	CloseHandle(taskSemaphore);
	CloseHandle(threadManager);
	CloseHandle(observerThread);

	for (int i = 0; i < maxCountOfThread; i++)
	{
		CloseHandle(startThreadSemaphore[i]);
		CloseHandle(workThreadSemaphore[i]);
	}

	delete[] threads;
	delete[] taskOfThread;
	delete[] isThreadCreated;
	delete[] isThreadBusy;
	delete[] timeOfLastActiveThread;
	delete[] startThreadSemaphore;
	delete[] workThreadSemaphore;

	out.close();
}

DWORD WINAPI SPKPool::ThreadManagerProc(LPVOID *lpSPKPool)
{
	SPKPool *spkPool = (SPKPool *) lpSPKPool;

	time_t currentTime;
	DWORD waitResult;
	while (!(spkPool->isSPKPoolExit))
	{
		while (spkPool->startedCountOfThread <= spkPool->minCountOfThread)
		{
			if (spkPool->isSPKPoolExit)
			{
				return 0;
			}
			Sleep(1000);
		}
		for (int i = 0; i < spkPool->maxCountOfThread; i++)
		{
			if (spkPool->timeOfLastActiveThread[i] == NULL)
			{
				continue;
			}
			waitResult = WaitForSingleObject(spkPool->workThreadSemaphore[i], 0L);
			if (waitResult == WAIT_OBJECT_0)
			{
				if (!(spkPool->isThreadBusy[i]))
				{
					currentTime = time(NULL);
					if (difftime(currentTime, spkPool->timeOfLastActiveThread[i]) > spkPool->timeLife)
					{
						spkPool->DestroyThreadWithId(i);
						if (spkPool->startedCountOfThread <= spkPool->minCountOfThread) break;
						else continue;
					}
				}	
				ReleaseSemaphore(spkPool->workThreadSemaphore[i], 1, NULL);
			}
		}
		Sleep(1000);
	}
	return 0;
}

DWORD WINAPI SPKPool::ObserverThreadProc(LPVOID *lpSPKPool)
{
	SPKPool *spkPool = (SPKPool *) lpSPKPool;

	Task *task;
	while (true)
	{
		WaitForSingleObject(spkPool->taskSemaphore, INFINITE);
		if (spkPool->tasks.empty())
		{
			ReleaseSemaphore(spkPool->taskSemaphore, 1, NULL);
			Sleep(100);
			continue;
		}
		
		task = spkPool->tasks.front();
		spkPool->tasks.pop();
		ReleaseSemaphore(spkPool->taskSemaphore, 1, NULL);

		if (task->taskName == observerDestroyTask)
		{
			delete task;
			return 0;
		}

		int threadId = spkPool->GetFreeThreadId();
		spkPool->taskOfThread[threadId] = task;
		spkPool->isThreadBusy[threadId] = true; 
		ReleaseSemaphore(spkPool->startThreadSemaphore[threadId], 1, NULL);
		ReleaseSemaphore(spkPool->workThreadSemaphore[threadId], 1, NULL);
	}
	return 0;
}

DWORD WINAPI SPKPool::TaskHandlerProc(LPVOID *data)
{
	InfoThread *infoThread = (InfoThread *) data;
	SPKPool *spkPool = infoThread->spkPool;
	int threadId = infoThread->threadId;

	while (true)
	{
		WaitForSingleObject(spkPool->startThreadSemaphore[threadId], INFINITE);
		spkPool->timeOfLastActiveThread[threadId] = time(NULL);
		WaitForSingleObject(spkPool->workThreadSemaphore[threadId], INFINITE);
		Task *task = spkPool->taskOfThread[threadId];
		if (task->taskName == threadDestroyTask)
		{
			delete task;
			return 0;
		}
		ThreadProc threadProc = threadProcs[task->taskName];
		if (threadProc)
		{
			threadProc(task->arguments);
		}
		else 
		{
			MultipalThreadProc multipalThreadProc = multipalThreadProcs[task->taskName];
			if (multipalThreadProc)
			{
				multipalThreadProc(spkPool, task->arguments);
			}
			else
			{
				std::cout << std::endl << "Error: task " << task->taskName << " not found." << std::endl;
			}
		}
		delete task;
		spkPool->timeOfLastActiveThread[threadId] = time(NULL);
		spkPool->isThreadBusy[threadId] = false;
		ReleaseSemaphore(spkPool->workThreadSemaphore[threadId], 1, NULL);
	}
	return 0;
}


int SPKPool::GetFreeThreadId(void)
{
	DWORD waitResult;
	while (true)
	{
		for (int i = 0; i < maxCountOfThread; i++)
		{
			waitResult = WaitForSingleObject(workThreadSemaphore[i], 0L);
			if (waitResult == WAIT_OBJECT_0 && !isThreadBusy[i] && isThreadCreated[i])
			{
				return i;
			}
			else if (waitResult == WAIT_OBJECT_0)
			{
				ReleaseSemaphore(workThreadSemaphore[i], 1, NULL);
			}
		}

		for (int i = 0; i < maxCountOfThread; i++)
		{
			waitResult = WaitForSingleObject(workThreadSemaphore[i], 0L);
			if (waitResult == WAIT_OBJECT_0 && !isThreadBusy[i])
			{
				if (!isThreadCreated[i])
				{
					CreateThreadWithId(i);
				}
				return i;
			}
			else if (waitResult == WAIT_OBJECT_0) 
			{
				ReleaseSemaphore(workThreadSemaphore[i], 1, NULL);
			}
		}
		Sleep(100);
	}
}


void SPKPool::CreateThreadWithId(int threadId)
{
	InfoThread *infoThread = new InfoThread();
	infoThread->spkPool = this;
	infoThread->threadId = threadId;
	
	threads[threadId] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->TaskHandlerProc,
		(LPVOID *)infoThread, 0, NULL);
	isThreadCreated[threadId] = true;
	startedCountOfThread++;
}

void SPKPool::DestroyThreadWithId(int threadId)
{
	Task *destroyTask = new Task();
	destroyTask->taskName = threadDestroyTask;
	taskOfThread[threadId] = destroyTask;
	isThreadBusy[threadId] = true; 
	ReleaseSemaphore(startThreadSemaphore[threadId], 1, NULL);
	ReleaseSemaphore(workThreadSemaphore[threadId], 1, NULL);
	WaitForSingleObject(threads[threadId], INFINITE);

	CloseHandle(threads[threadId]);

	threads[threadId] = NULL;
	taskOfThread[threadId] = NULL;
	isThreadBusy[threadId] = false;
	isThreadCreated[threadId] = false;
	timeOfLastActiveThread[threadId] = NULL;
	ReleaseSemaphore(workThreadSemaphore[threadId], 1, NULL);
	startedCountOfThread--;
}


void SPKPool::AddTask(Task *task)
{
	if (task->taskName == statusTask)
	{
		HINSTANCE statusDll;
		if((statusDll=LoadLibrary(L"../Debug/StatusDll"))==NULL)
		{
			std::cout << "Can`t load StatusDll.";
			return;
		}

		PoolProc status;
		status=(PoolProc)GetProcAddress(statusDll, (LPCSTR)MAKEINTRESOURCE(1));
		status(this);
		FreeLibrary(statusDll);
		return;
	}

	WaitForSingleObject(taskSemaphore, INFINITE);
	tasks.push(task);
	ReleaseSemaphore(taskSemaphore, 1, NULL);
}