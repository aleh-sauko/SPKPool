#pragma once
#include "ListDll.h"
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

__declspec(dllexport) DWORD List(std::vector<std::string> arguments)
{
	if (arguments.size() != 1)
	{
		_tprintf(L"\nError: wrong count of parameters.\n");
		return 1;
	}

	std::wstring temp = std::wstring(arguments[0].begin(), arguments[0].end()) + L"\\*.*";
	LPCWSTR root = temp.c_str();

	WIN32_FIND_DATA fd;
	HANDLE hFind=::FindFirstFile(root, &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
		_tprintf(L"\nFILES :\n");
        do{
            _tprintf(L"\t%s: %s\n", (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? L"Folder" : L"File", fd.cFileName);
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
		_tprintf(L"END FILES.\n\n");
    } else printf("\nError : Invalid dir.\n");

	return 0;
}
