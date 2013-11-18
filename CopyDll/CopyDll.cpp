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
		std::cout << "\nError: wrong count of parameters.\n";
		return STATUS+1;
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
			std::string from = arguments[0] + "\\" + std::string(temp.begin(), temp.end());
			std::string to = arguments[1] + "\\" + std::string(temp.begin(), temp.end());

			if (temp == L".." || temp == L".")
				continue;

			std::wstring temp1 = std::wstring(from.begin(), from.end());
			LPCWSTR path = temp1.c_str();

			std::wstring temp2 = std::wstring(to.begin(), to.end());
			LPCWSTR destination = temp2.c_str();

			arg.push_back(from);
			arg.push_back(to);
				
			Task *task = new Task();
			task->taskName = "Copy";
			task->arguments = arg;


			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				STATUS = CreateDirectory(destination, NULL);
				EnterCriticalSection(&spkPool->outSection);
				if (STATUS == 0)
					spkPool->out << "(Create Dir) Error (" << GetLastError() <<") : " << to << std::endl;
				else
					spkPool->out << "(Create Dir) Success : " << to << std::endl;
				LeaveCriticalSection(&spkPool->outSection);
				spkPool->AddTask(task);
			}
			else
			{
				STATUS = CopyFile(path, destination, FALSE);
				EnterCriticalSection(&spkPool->outSection);
				if (STATUS == 0)
					spkPool->out << "(Copy) Error (" << GetLastError() <<") : " << from << " to " << to << std::endl;
				else
					spkPool->out << "(Copy) Success : " << from << " to " << to << std::endl;
				LeaveCriticalSection(&spkPool->outSection);
				delete task;
			}
		} while(::FindNextFile(hFind, &fd));
		::FindClose(hFind);
		
	} 
	else 
	{	
		EnterCriticalSection(&spkPool->outSection);
		spkPool->out << "\nError : Invalid dir " << arguments[0] << ".\n" << std::cout;
		LeaveCriticalSection(&spkPool->outSection);
	}
	
	return STATUS;
}