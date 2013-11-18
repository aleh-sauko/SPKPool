#pragma once
#include "SizeDll.h"
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

__declspec(dllexport) DWORD Size(SPKPool *spkPool, std::vector<std::string> arguments)
{
	DWORD FILE_SIZE = 0; 

	if (arguments.size() != 1)
	{
		spkPool->out << "\nError: wrong count of parameters.\n";
		return FILE_SIZE;
	}

	std::wstring temp = std::wstring(arguments[0].begin(), arguments[0].end()) + L"\\*.*";
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

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				spkPool->AddTask(task);
			}
			else
			{
				FILE_SIZE += fd.nFileSizeLow;
				delete task;
			}
		} while(::FindNextFile(hFind, &fd));
		::FindClose(hFind);
		
	} 
	else
	{
		temp = std::wstring(arguments[0].begin(), arguments[0].end());
		root = temp.c_str();
		hFind=::FindFirstFile(root, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FILE_SIZE += fd.nFileSizeLow;
		} 
		else 
		{
			spkPool->out << "\nError : Invalid dir " << arguments[0] << ".\n" << std::cout;
		}
	}

	spkPool->out << std::endl << "Size of " << arguments[0] << " " << FILE_SIZE << " bytes." << std::endl;

	return FILE_SIZE;
}