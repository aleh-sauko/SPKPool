#pragma once
#include "../SPKPool/SPKPool.h"
#include "../SPKPool/SPKPool.cpp"
#include "../SPKPool/Main.cpp"

__declspec(dllexport) DWORD Copy(SPKPool *spkPool, std::vector<std::string> arguments);