#pragma once
#include <Windows.h>

bool readDword(HKEY, LPCWSTR, LPCWSTR, DWORD*);
bool writeDword(HKEY, LPCWSTR, LPCWSTR, DWORD);
bool toggleDword(HKEY, LPCWSTR, LPCWSTR);