#pragma once
#include "ExitDll.h"
#include "../SPKPool/MapProc.h"
#include "../SPKPool/SPKPool.cpp"

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