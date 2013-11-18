#pragma once
#include "ExitDll.h"
#include "../SPKPool/MapProc.h"
#include "../SPKPool/SPKPool.cpp"

int WINAPI DllMain(HINSTANCE hinstance, DWORD dwreason, PVOID pvreserved)
{
	bool ballwentwell=true;
	switch (dwreason) 
	{
	case DLL_PROCESS_ATTACH: // ������������� ��������. 
		break;
	case DLL_THREAD_ATTACH: // ������������� ������.
		break;
	case DLL_THREAD_DETACH: // ������� �������� ������.
		break;
	case DLL_PROCESS_DETACH: // ������� �������� ��������.
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