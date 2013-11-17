#pragma once
#include "ExitDll.h"

int WINAPI DllMain(HINSTANCE hinstance, DWORD dwreason, PVOID pvreserved)
{
	bool ballwentwell=true;
	switch (dwreason) 
	{
	case DLL_PROCESS_ATTACH: // Инициализация процесса. 
		break;
	case DLL_THREAD_ATTACH: // Инициализация потока.
		break;
	case DLL_THREAD_DETACH: // Очистка структур потока.
		break;
	case DLL_PROCESS_DETACH: // Очистка структур процесса.
		break;
	}
	if(ballwentwell) return true;
	else return false;
}


__declspec(dllexport) void Exit(SPKPool *spkPool)
{
	Task *task = new Task();
	task->taskName = observerDestroyTask;
	spkPool->AddTask(task);
	WaitForSingleObject(spkPool->observerThread, INFINITE);

	spkPool->isSPKPoolExit = true;
	WaitForSingleObject(spkPool->threadManager, INFINITE);

	delete spkPool;
}


void SPKPool::AddTask(Task *task)
{
	WaitForSingleObject(taskSemaphore, INFINITE);
	tasks.push(task);
	ReleaseSemaphore(taskSemaphore, 1, NULL);
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