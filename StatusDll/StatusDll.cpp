#pragma once
#include "StatusDll.h"

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

__declspec(dllexport) void Status(SPKPool *spkPool)
{
	int workCountOfThread = 0;
	int waitCountOfThread = 0;
	for (int i = 0; i < spkPool->maxCountOfThread; i++)
	{
		if (spkPool->isThreadCreated[i])
		{
			if (spkPool->isThreadBusy[i])
			{
				workCountOfThread++;
			}
			else
			{
				waitCountOfThread++;
			}
		}
	}

	std::cout << std::endl << "Work count of thread " << workCountOfThread << "." << std::endl;
	std::cout << "Wait count of thread " << waitCountOfThread << "." << std::endl << std::endl;

	return;
}
