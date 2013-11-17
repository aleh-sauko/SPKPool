#pragma once
#include "CopyDll.h"
#include <tchar.h>

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


__declspec(dllexport) DWORD Copy(SPKPool *spkPool, std::vector<std::string> arguments)
{
	DWORD STATUS = 0; 

	if (arguments.size() != 2)
	{
		spkPool->out << "\nError: wrong count of parameters.\n";
		return STATUS+1;
	}

	std::wstring temp = std::wstring(arguments[0].begin(), arguments[0].end()) + L"\\*.*";
	LPCWSTR root = temp.c_str();

	temp = std::wstring(arguments[1].begin(), arguments[1].end()) + L"\\*.*";
	LPCWSTR root = temp.c_str();

	WIN32_FIND_DATA fd;
	HANDLE hFind=::FindFirstFile(root, &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
		do{
			std::vector<std::string> arg;
			std::wstring temp(fd.cFileName);

			if (temp == L".." || temp == L".")
				continue;

			arg.push_back(arguments[0] + "\\" + std::string(temp.begin(), temp.end()));
				
			Task *task = new Task();
			task->taskName = "Size";
			task->arguments = arg;

			//spkPool->GetFreeThreadId;

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				spkPool->AddTask(task);
				//FILE_SIZE += Size(spkPool, arg);
			}
			else
			{
				FILE_SIZE += fd.nFileSizeLow;
			}
		} while(::FindNextFile(hFind, &fd));
		::FindClose(hFind);
		
	} else spkPool->out << "\nError : Invalid dir " << arguments[0] << ".\n" << std::cout;

	spkPool->out << std::endl << "Size of " << arguments[0] << " " << FILE_SIZE << " bytes." << std::endl;

	return FILE_SIZE;
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