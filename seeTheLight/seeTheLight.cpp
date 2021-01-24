#include <Windows.h>
#include <iostream>
#include "seeTheLight.h"

using namespace std;

bool readDword(HKEY hKeyParent, LPCWSTR lpczSubkey, LPCWSTR lpczValueName, DWORD* readData) {
	HKEY hKey;
	DWORD dwReturn;

	// check if key exists (it should)
	dwReturn = RegOpenKeyExW(
		hKeyParent, // parent key
		lpczSubkey, // subkey
		0, // options
		KEY_READ, // permissions desired
		&hKey // data result
		);

	if (dwReturn == ERROR_SUCCESS) {
		DWORD dwData, dwLen = sizeof(DWORD);

		dwReturn = RegQueryValueExW(
			hKey, // key to query
			lpczValueName, // value name
			NULL, // not reserged
			NULL, // null type
			(LPBYTE)(&dwData),
			&dwLen
		);

		if (dwReturn == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			(*readData) = dwData; // write data read
			return true; // success
		}

		RegCloseKey(hKey);
		return false;
	}

	else {
		return false;
	}
}

bool writeDword(HKEY hKeyParent, LPCWSTR lpczSubkey, LPCWSTR lpczValueName, DWORD dwData) {
	HKEY hKey;
	DWORD dwReturn;

	dwReturn = RegOpenKeyExW(
		hKeyParent, // parent key
		lpczSubkey, // subkey
		0, // options
		KEY_WRITE, // permissions desired
		&hKey // data result
	);

	if (dwReturn == ERROR_SUCCESS) {
		if (ERROR_SUCCESS != RegSetValueExW(
			hKey, // parent key
			lpczValueName, // value name
			0, // not reserved
			REG_DWORD, // data type
			reinterpret_cast<BYTE*>(&dwData),
			sizeof(DWORD)
		)) {
			RegCloseKey(hKey);
			return false;
		}

		RegCloseKey(hKey);
		return true;
	}

	return false;
}

bool toggleDword(HKEY hKeyParent, LPCWSTR lpczSubkey, LPCWSTR lpczValueName) {
	DWORD dwCurrentValue;
	bool bSuccess;

	bSuccess = readDword(hKeyParent, lpczSubkey, lpczValueName, &dwCurrentValue);
	
	if (!bSuccess) {
		cerr << "read of " << hKeyParent << lpczSubkey << lpczValueName << "failed!" << endl;
		return false;
	}
	else {
		if (dwCurrentValue == 1) {
			writeDword(hKeyParent, lpczSubkey, lpczValueName, (DWORD)0);
			return true;
		}
		else if (dwCurrentValue == 0) {
			writeDword(hKeyParent, lpczSubkey, lpczValueName, (DWORD)1);
			return true;
		}
		else {
			cerr << "current DWORD value is non-binary!" << endl;
			return false;
		}
	}
}


int wmain(int argc, wchar_t** argv) {
	LPCWSTR lpczSubkey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
	LPCWSTR lpAppsKey = L"AppsUseLightTheme";
	LPCWSTR lpSystemKey = L"SystemUsesLightTheme";
	DWORD dwDarkModeApps, dwDarkModeSystem;
	bool bSuccess, bVerbose = false, bUndo = false;

	for (int i = 0; i < argc; i++) {
		if (lstrcmpiW(argv[i], L"verbose") == 0) {
			bVerbose = true;
		}
		if (lstrcmpiW(argv[i], L"undo") == 0) {
			bUndo = true;
		}
	}

	bSuccess = readDword(HKEY_CURRENT_USER, lpczSubkey, lpAppsKey, &dwDarkModeApps);

	if (!bSuccess) {
		cerr << "key read failed from AppsUseLightTheme" << endl;
		return 1;
	}

	bSuccess = readDword(HKEY_CURRENT_USER, lpczSubkey, lpSystemKey, &dwDarkModeSystem);

	if (!bSuccess) {
		cerr << "key read failed from SystemUsesLightTheme" << endl;
		return 1;
	}
	
	if (bVerbose) {
		cout << "Apps Light Mode: " << dwDarkModeApps << endl;
		cout << "System Light Mode: " << dwDarkModeSystem << endl;
	}
	
	if (bUndo && dwDarkModeApps == 1) {
		bSuccess = toggleDword(HKEY_CURRENT_USER, lpczSubkey, lpAppsKey);
	}
	else if (dwDarkModeApps == 0) {
		bSuccess = toggleDword(HKEY_CURRENT_USER, lpczSubkey, lpAppsKey);
	}
	

	if (!bSuccess) {
		cerr << "setting AppsUseLightTheme failed!" << endl;
		return 1;
	}

	if (bUndo && dwDarkModeSystem == 1) {
		bSuccess = toggleDword(HKEY_CURRENT_USER, lpczSubkey, lpSystemKey);
	}
	else if (dwDarkModeSystem == 0) {
		bSuccess = toggleDword(HKEY_CURRENT_USER, lpczSubkey, lpSystemKey);
	}

	if (!bSuccess) {
		cerr << "setting SystemUsesLightTheme failed!" << endl;
		return 1;
	}

	return 0;
}