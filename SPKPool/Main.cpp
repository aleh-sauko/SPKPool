#pragma once
#include "SPKPool.h"

#include "../ExitDll/ExitDll.h"
#pragma comment(lib, "../Debug/ExitDll.lib")

std::map <std::string, ThreadProc> threadProcs;
std::map <std::string, MultipalThreadProc> multipalThreadProcs;

int main(void)
{
	int minCountOfThread;
	int maxCountOfThread;
	int timeLife;
	std::cout << "Write parameters for SPKPool :\n";
	std::cout << "\t1. Min count of thread : ";
	std::cin >> minCountOfThread;
	std::cout << "\t2. Max count of thread : ";
	std::cin >> maxCountOfThread;
	std::cout << "\t3. Time life for thread : ";
	std::cin >> timeLife;

	SPKPool *spkPool = new SPKPool(minCountOfThread, maxCountOfThread, timeLife);

	HINSTANCE listDll;
	if((listDll=LoadLibrary(L"../Debug/ListDll"))==NULL)
	{
		std::cout << "Can`t load ListDll.";
		return 0;
	}

	HINSTANCE sizeDll;
	if((sizeDll=LoadLibrary(L"../Debug/SizeDll"))==NULL)
	{
		std::cout << "Can`t load SizeDll.";
		return 0;
	}

	HINSTANCE copyDll;
	if((copyDll=LoadLibrary(L"../Debug/CopyDll"))==NULL)
	{
		std::cout << "Can`t load CopyDll.";
		return 0;
	}

	ThreadProc list;
	list=(ThreadProc)GetProcAddress(listDll, (LPCSTR)MAKEINTRESOURCE(1));
	threadProcs["List"] = list;

	MultipalThreadProc size;
	size=(MultipalThreadProc)GetProcAddress(sizeDll, (LPCSTR)MAKEINTRESOURCE(1));
	multipalThreadProcs["Size"] = size;

	MultipalThreadProc copy;
	copy=(MultipalThreadProc)GetProcAddress(copyDll, (LPCSTR)MAKEINTRESOURCE(1));
	multipalThreadProcs["Copy"] = copy;

	while (true)
	{
		Task *task = new Task();
		std::cout << "Write Task : ";
		std::cin >> task->taskName;
		std::string arg;
		while (std::cin.peek() != '\n') 
		{
			std::cin >> arg;
			task->arguments.push_back(arg);
		}

		if (task->taskName == exitTask)
		{
			Exit(spkPool);
			break;
		}

		spkPool->AddTask(task);
		Sleep(100);
	}

	FreeLibrary(listDll);
	FreeLibrary(sizeDll);
	FreeLibrary(copyDll);

	std::cout << "SPKPool closed." << std::endl;
	getchar(); getchar();
	return 0;
}