#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "helper.h"
#include <TlHelp32.h>
#include <Shlwapi.h>
#include "Processes.h"
#pragma comment(lib,"shlwapi.lib")

using namespace std;

typedef void (*Function)(DWORD);

DWORD GetFuncAddr(PCHAR pLibName, PCHAR pFuncName) {
	HMODULE hModule_kernal32 = LoadLibraryA(pLibName);
	if (hModule_kernal32 == NULL)
	{
		cout << "LoadLibraryA failed" << get_last_error(0) << endl;
		return 0;
	}
	return (DWORD)GetProcAddress(hModule_kernal32, pFuncName);
}
// 提权函数
BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);

		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;

}
void ShowError(const char* pszText)
{
	char szError[MAX_PATH] = { 0 };
	sprintf_s(szError, "%s Error[%d]\n", pszText, GetLastError());
	MessageBoxA(NULL, szError, "ERROR", MB_OK);
}

// 使用 ZwCreateThreadEx 实现远线程注入
BOOL ZwCreateThreadExInjectDll(DWORD PID, PCHAR pszDllFileName, PDWORD dllImageBase)
{
	HANDLE hProcess = NULL;
	SIZE_T dwSize = 0;
	LPVOID pDllAddr = NULL;
	FARPROC pFuncProcAddr = NULL;
	HANDLE hRemoteThread = NULL;
	DWORD dwStatus = 0;

	EnableDebugPrivilege();

	// 打开注入进程，获取进程句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (hProcess == NULL)
	{
		printf("OpenProcess - Error!\n\n");
		return -1;
	}
	// 在注入的进程申请内存地址

	dwSize = strlen(pszDllFileName) + 1;
	pDllAddr = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);

	if (NULL == pDllAddr)
	{
		ShowError("VirtualAllocEx - Error!\n\n");
		return FALSE;
	}
	//写入内存地址

	if (FALSE == WriteProcessMemory(hProcess, pDllAddr, pszDllFileName, dwSize, NULL))
	{
		ShowError("WriteProcessMemory - Error!\n\n");
		return FALSE;
	}
	//加载ntdll
	HMODULE hNtdllDll = LoadLibraryA("ntdll.dll");

	if (NULL == hNtdllDll)
	{
		ShowError("LoadLirbary");
		return FALSE;
	}
	// 获取LoadLibraryA函数地址

	pFuncProcAddr = (FARPROC)GetFuncAddr((PCHAR)"Kernel32.dll", (PCHAR)"LoadLibraryA");
	if (NULL == pFuncProcAddr)
	{
		ShowError("GetProcAddress_LoadLibraryA - Error!\n\n");
		return FALSE;
	}

#ifdef _WIN64
	typedef DWORD(WINAPI* typedef_ZwCreateThreadEx)(
		PHANDLE ThreadHandle,
		ACCESS_MASK DesiredAccess,
		LPVOID ObjectAttributes,
		HANDLE ProcessHandle,
		LPTHREAD_START_ROUTINE lpStartAddress,
		LPVOID lpParameter,
		ULONG CreateThreadFlags,
		SIZE_T ZeroBits,
		SIZE_T StackSize,
		SIZE_T MaximumStackSize,
		LPVOID pUnkown);
#else
	typedef DWORD(WINAPI* typedef_ZwCreateThreadEx)(
		PHANDLE ThreadHandle,
		ACCESS_MASK DesiredAccess,
		LPVOID ObjectAttributes,
		HANDLE ProcessHandle,
		LPTHREAD_START_ROUTINE lpStartAddress,
		LPVOID lpParameter,
		BOOL CreateSuspended,
		DWORD dwStackSize,
		DWORD dw1,
		DWORD dw2,
		LPVOID pUnkown);
#endif

	//获取ZwCreateThreadEx函数地址
	typedef_ZwCreateThreadEx ZwCreateThreadEx = (typedef_ZwCreateThreadEx)GetProcAddress(hNtdllDll, "ZwCreateThreadEx");

	if (NULL == ZwCreateThreadEx)
	{
		ShowError("GetProcAddress_ZwCreateThread - Error!\n\n");
		return FALSE;
	}
	// 使用 ZwCreateThreadEx 创建远线程, 实现 DLL 注入
	dwStatus = ZwCreateThreadEx(&hRemoteThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)pFuncProcAddr, pDllAddr, 0, 0, 0, 0, NULL);
	if (dwStatus)
	{
		ShowError("ZwCreateThreadEx - Error!\n\n");
		return FALSE;
	}
	WaitForSingleObject(hRemoteThread, INFINITE);
	GetExitCodeThread(hRemoteThread, dllImageBase);
	// 关闭句柄
	::CloseHandle(hProcess);
	::FreeLibrary(hNtdllDll);
	return TRUE;
}
DWORD getProcessByName(WCHAR* processName, BOOL ignoreCase) {


	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD pid = 0;
	if (hSnap == INVALID_HANDLE_VALUE)
	{

		cout << "进程快照 失败：" << get_last_error(0) << endl;
		return pid;
	}
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet = Process32First(hSnap, &pe32);
	while (bRet)
	{
		int cmpRes = ignoreCase ? wcscasecmp(processName, pe32.szExeFile) : wcscmp(processName, pe32.szExeFile);
		if (cmpRes == 0)
		{
			pid = pe32.th32ProcessID;
			wprintf(L"%s \t %d \n", pe32.szExeFile, pe32.th32ProcessID);
			break;
		}
		bRet = Process32Next(hSnap, &pe32);
	}
	CloseHandle(hSnap);
	return pid;
}

void getAllProcessIdsByName(WCHAR* processName, BOOL ignoreCase, PDWORD foundCount, PDWORD pids) {

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap == INVALID_HANDLE_VALUE)
	{
		cout << "进程快照 失败：" << get_last_error(0) << endl;
		return;
	}
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet = Process32First(hSnap, &pe32);
	int matched = 0;
	while (bRet)
	{
		int cmpRes = ignoreCase ? wcscasecmp(processName, pe32.szExeFile) : wcscmp(processName, pe32.szExeFile);
		if (cmpRes == 0)
		{
			pids[matched] = pe32.th32ProcessID;
			matched++;
			wprintf(L"%s \t %d \n", pe32.szExeFile, pe32.th32ProcessID);
		}
		bRet = Process32Next(hSnap, &pe32);
	}
	*foundCount = matched;
	CloseHandle(hSnap);
}


DWORD createMyProcess(LPSTR pePath) {
	DWORD pid = 0;
	STARTUPINFOA start = { 0 };
	start.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = { 0 };
	CreateProcessA(pePath, NULL, NULL, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &start, &pi);
	if (GetLastError())
	{
		cout << "创建进程：" << get_last_error(0) << endl;
		return pid;
	}
	pid = pi.dwProcessId;
	return pid;
}

HANDLE openMyProcess(DWORD pid) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (hProcess == NULL)
	{
		cout << "打开进程：" << get_last_error(0) << endl;
	}

	return hProcess;
}

void KillProcessById(DWORD pid) {
	HANDLE hProcess = openMyProcess(pid);
	TerminateProcess(hProcess, 999);
	cout << "结束进程：[%d] \t" << pid << get_last_error(0) << endl;
	DWORD exitCode = -1;
	GetExitCodeProcess(hProcess, &exitCode);
	cout << "ExitCode：" << exitCode << endl;
}

void KillEachProcessByName(WCHAR* processName, Function killProcessFunc) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD pid = 0;
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		cout << "KillProcessByName进程快照 失败：" << get_last_error(0) << endl;
		return;
	}
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet = Process32First(hSnap, &pe32);
	while (bRet)
	{
		if (wcscmp(processName, pe32.szExeFile) == 0)
		{
			killProcessFunc(pe32.th32ProcessID);
			wprintf(L"kill process %s \t %d \n", pe32.szExeFile, pe32.th32ProcessID);
		}
		bRet = Process32Next(hSnap, &pe32);
	}
	CloseHandle(hSnap);
}
void KillProcessByName(WCHAR* processName) {
	KillEachProcessByName(processName, &KillProcessById);
}



typedef DWORD(WINAPI* SuspendProcessFunc)(HANDLE);

void SuspendProcess(DWORD pid) {

	HMODULE hModule = LoadLibraryA("ntdll.dll");
	if (hModule == NULL)
	{
		cout << "LoadLibraryA failed " << endl;
		return;
	}
	SuspendProcessFunc FuncAddr = (SuspendProcessFunc)GetProcAddress(hModule, "ZwSuspendProcess");
	HANDLE hProcess = openMyProcess(pid);
	FuncAddr(hProcess);
	CloseHandle(hProcess);
}

typedef DWORD(WINAPI* ResumeProcessFunc)(HANDLE);
void ResumeProcess(DWORD pid) {

	HMODULE hModule = LoadLibraryA("ntdll.dll");
	if (hModule == NULL)
	{
		cout << "LoadLibraryA failed " << endl;
		return;
	}
	ResumeProcessFunc FuncAddr = (ResumeProcessFunc)GetProcAddress(hModule, "ZwResumeProcess");
	HANDLE hProcess = openMyProcess(pid);
	FuncAddr(hProcess);
	CloseHandle(hProcess);
}

DWORD ReadRemoteProcessMemory(DWORD pid, LPVOID baseAddr, LPVOID lpBuffer, DWORD size, SIZE_T* lpNumberOfBytesRead) {

	HANDLE hProcess = openMyProcess(pid);
	if (hProcess == NULL)
	{
		cout << "openMyProcess failed:" << get_last_error(0) << endl;
		return 0;
	}
	//DWORD oldAttr = 0;
	//VirtualProtect(baseAddr,size,PAGE_READWRITE,&oldAttr);
	BOOL ret = ReadProcessMemory(hProcess, baseAddr, lpBuffer, size, lpNumberOfBytesRead);
	if (!ret)
	{
		cout << "ReadProcessMemory failed" << get_last_error(0) << endl;
	}
	CloseHandle(hProcess);
	return ret;


}


DWORD WriteRemoteProcessMemory(DWORD pid, LPVOID baseAddr, LPVOID lpBuffer, DWORD size, SIZE_T* lpNumberOfBytesWritten) {

	HANDLE hProcess = openMyProcess(pid);
	if (hProcess == NULL)
	{
		cout << "openMyProcess failed:" << get_last_error(0) << endl;
		return 0;

	}
	DWORD oldAttr = 0;
	VirtualProtect(baseAddr, size, PAGE_READWRITE, &oldAttr);
	BOOL ret = WriteProcessMemory(hProcess, baseAddr, lpBuffer, size, lpNumberOfBytesWritten);
	if (!ret)
	{
		cout << "WriteProcessMemory failed" << get_last_error(0) << endl;
	}
	CloseHandle(hProcess);
	return ret;
}



BOOL  RemoteThreadInject(DWORD dwPid, PCHAR szDllPath, PDWORD exitAddr) {

	BOOL isok = FALSE;
	if (!PathFileExistsA(szDllPath))
	{
		cout << "PathFile is Not Exists" << endl;
		return isok;
	}
	LPVOID pDllPath = 0;
	DWORD dwDllPathSize = 0;
	HMODULE hModule_kernal32 = 0;
	HANDLE hProcess = openMyProcess(dwPid);

	if (hProcess == NULL)
	{
		return isok;
	}
	dwDllPathSize = strlen(szDllPath) + 1;
	pDllPath = VirtualAllocEx(hProcess, NULL, dwDllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pDllPath)
	{
		cout << "VirtualAllocEx failed" << get_last_error(0) << endl;
		return isok;
	}
	if (!WriteProcessMemory(hProcess, pDllPath, szDllPath, dwDllPathSize, NULL))
	{
		cout << "WriteProcessMemory failed" << get_last_error(0) << endl;
		return isok;
	};

	DWORD LoadLibraryFuncAddr = GetFuncAddr((PCHAR)"Kernel32.dll", (PCHAR)"LoadLibraryA");
	if (!LoadLibraryFuncAddr)
	{
		cout << "LoadLibraryFuncAddr failed" << get_last_error(0) << endl;
		return isok;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryFuncAddr, pDllPath, 0, NULL);
	if (hThread == NULL)
	{
		cout << "CreateRemoteThread failed" << get_last_error(0) << endl;
		return isok;
	}
	WaitForSingleObject(hThread, INFINITE);
	DWORD exitCode = 0;
	GetExitCodeThread(hThread, &exitCode);
	*exitAddr = exitCode;
	VirtualFreeEx(hProcess, pDllPath, dwDllPathSize, MEM_COMMIT);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}


DWORD GetProcessIdByWindowName(LPCWSTR lpClassName, LPCWSTR lpWindowName) {
	HWND wHandle = FindWindow(lpClassName, lpWindowName);
	if (wHandle == NULL)
	{
		cout << "FindWindow failed " << get_last_error(0) << endl;
		return 0;
	}
	DWORD pid = 0;
	GetWindowThreadProcessId(wHandle, &pid);
	return pid;
}



DWORD FreeInjectDll(DWORD dwPID, PWCHAR pDllPath) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnap == NULL)
	{
		cout << "CreateToolhelp32Snapshot failed " << get_last_error(0) << endl;
		return 0;
	}
	MODULEENTRY32 module = { 0 };
	module.dwSize = sizeof(module);
	BOOL isFoundDll = FALSE;
	if (Module32First(hSnap, &module))
	{
		do
		{
			if (wcscmp(pDllPath, module.szModule) == 0)
			{
				isFoundDll = TRUE;
				break;
			}
		} while (Module32Next(hSnap, &module));
	}
	if (!isFoundDll)
	{
		cout << pDllPath << "not found" << endl;
		return 0;
	}
	HANDLE hProcess = openMyProcess(dwPID);
	if (hProcess == NULL)
	{
		return 0;
	}

	DWORD FreeLibraryFuncAddr = GetFuncAddr((PCHAR)"Kernel32.dll", (PCHAR)"FreeLibrary");
	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)FreeLibraryFuncAddr, module.hModule, 0, 0);
	if (hThread == NULL)
	{
		cout << "CreateRemoteThread failed" << get_last_error(0) << endl;
		return 0;
	}
	WaitForSingleObject(hThread, INFINITE);
	DWORD exit = 0;
	GetExitCodeThread(hThread, &exit);
	CloseHandle(hThread);
	CloseHandle(hSnap);
	CloseHandle(hProcess);
	return 1;

}



DWORD FreeInjectDllByExitAddr(DWORD dwPID, DWORD imageBaseAddr) {
	HANDLE hProcess = openMyProcess(dwPID);
	if (hProcess == NULL)
	{
		return 0;
	}
	DWORD FreeLibraryFuncAddr = GetFuncAddr((PCHAR)"Kernel32.dll", (PCHAR)"FreeLibrary");

	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)FreeLibraryFuncAddr, (LPVOID)imageBaseAddr, 0, 0);
	if (hThread == NULL)
	{
		cout << "CreateRemoteThread failed" << get_last_error(0) << endl;
		return 0;
	}
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	return 1;
}
DWORD HijackThreadInject(DWORD dwPID, PCHAR pDllPath) {

	DWORD status = 0;
	HANDLE hProcess = openMyProcess(dwPID);
	if (hProcess == NULL)
	{

		return 0;
	}

	PCHAR pData = (PCHAR)VirtualAllocEx(hProcess, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pData)
	{
		cout << "VirtualAllocEx failed" << get_last_error(0) << endl;
		return 0;
	}
	char szCode[] = {
		 0x68,0x00,0x00,0x00,0x00,  //PUSH  RET ADDR
		 0x60,						//PUSHAD
		 0x9c,						//PUSHFD
		 0x68,0x00,0x00,0x00,0x00,  //PUSH dllPath
		 0xb8,0x00,0x00,0x00,0x00,  //MOV EAX,[LoadLibraryA]
		 0xff,0xd0,					//CALL EAX
		 0x9d,						//POPFD
		 0x61,						//POPAD
		 0xc3						//RET 
	};

	WriteProcessMemory(hProcess, pData + sizeof(szCode), pDllPath, strlen(pDllPath) + 1, NULL);
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		cout << "CreateToolhelp32Snapshot failed" << get_last_error(0) << endl;
		return 0;
	}
	THREADENTRY32 tEntry = { 0 };
	tEntry.dwSize = sizeof(tEntry);
	HANDLE hThread = 0;
	if (Thread32First(hSnap, &tEntry))
	{
		do
		{
			if (tEntry.th32OwnerProcessID == dwPID)
			{
				hThread = OpenThread(THREAD_ALL_ACCESS, false, tEntry.th32ThreadID);
				if (hThread)
				{
					break;
				}
			}
		} while (Thread32Next(hSnap, &tEntry));
	}
	if (!hThread)
	{
		cout << "thread not found failed" << get_last_error(0) << endl;
		return 0;
	}
	SuspendThread(hThread);
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &ctx);
	//修正返回地址
	*(LPDWORD)&szCode[1] = ctx.Eip;
	//修正loadbirary 参数
	*(LPDWORD)&szCode[8] = (DWORD)(pData + sizeof(szCode));
	//修正函数地址
	*(LPDWORD)&szCode[13] = (DWORD)LoadLibraryA;
	//写入构建代码
	WriteProcessMemory(hProcess, pData, szCode, sizeof(szCode), NULL);
	//修改EIP
	ctx.Eip = (DWORD)pData;
	SetThreadContext(hThread, &ctx);
	ResumeThread(hThread);
	VirtualFreeEx(hProcess, pData, 0x1000, MEM_COMMIT);
	CloseHandle(hThread);
	CloseHandle(hSnap);
	CloseHandle(hProcess);
	return 1;
}

DWORD HjackProcessInject(PCHAR peFilePath, PCHAR pDllPath, PDWORD exitAddr) {

	STARTUPINFOA start = { 0 };
	start.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = { 0 };
	BOOL ret = CreateProcessA(peFilePath, NULL, NULL, NULL, NULL, CREATE_SUSPENDED, NULL, NULL, &start, &pi);
	if (!ret)
	{
		cout << "CreateProcessA failed" << get_last_error(0) << endl;
		return 0;
	}


	if (ZwCreateThreadExInjectDll(pi.dwProcessId, pDllPath, exitAddr))
	{
		cout << "RemoteThreadInject failed" << get_last_error(0) << endl;
		return 0;
	}
	//if (!RemoteThreadInject(pi.dwProcessId, pDllPath, exitAddr))
	//{
	//	cout << "RemoteThreadInject failed" << get_last_error(0) << endl;
	//	return 0;
	//};
	ResumeThread(pi.hThread);
	return 1;
}



DWORD ApcInject(DWORD dwPID, PCHAR pDllPath) {
	HANDLE hProcess = openMyProcess(dwPID);
	DWORD dwDllPathSize = strlen(pDllPath) + 1;
	PCHAR pData = (PCHAR)VirtualAllocEx(hProcess, NULL, dwDllPathSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pData)
	{
		cout << "VirtualAllocEx failed" << get_last_error(0) << endl;
		return 0;
	}
	if (!WriteProcessMemory(hProcess, pData, pDllPath, dwDllPathSize, NULL))
	{
		cout << "WriteProcessMemory failed" << get_last_error(0) << endl;
		return 0;
	}

	DWORD LoadLibraryFuncAddr = GetFuncAddr((PCHAR)"Kernel32.dll", (PCHAR)"LoadLibraryA");
	if (!LoadLibraryFuncAddr)
	{
		cout << "LoadLibraryFuncAddr failed" << get_last_error(0) << endl;
		return 0;
	}
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		cout << "CreateToolhelp32Snapshot failed" << get_last_error(0) << endl;
		return 0;
	}
	THREADENTRY32 tEntry = { 0 };
	tEntry.dwSize = sizeof(tEntry);
	HANDLE hThread = 0;
	if (Thread32First(hSnap, &tEntry))
	{
		do
		{
			if (tEntry.th32OwnerProcessID == dwPID)
			{
				hThread = OpenThread(THREAD_ALL_ACCESS, false, tEntry.th32ThreadID);
				if (hThread)
				{
					QueueUserAPC((PAPCFUNC)LoadLibraryFuncAddr, hThread, (ULONG_PTR)pData);
					CloseHandle(hThread);
					break;
				}
			}
		} while (Thread32Next(hSnap, &tEntry));
	}

	//VirtualFreeEx(hProcess, pData, dwDllPathSize, MEM_COMMIT);
	CloseHandle(hProcess);
	CloseHandle(hSnap);



}

///< 枚举窗口参数
typedef struct
{
	HWND    hwndWindow;     // 窗口句柄
	DWORD   dwProcessID;    // 进程ID
	char className[0x256];
}EnumWindowsArg;

///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	EnumWindowsArg* pArg = (EnumWindowsArg*)lParam;
	DWORD  dwProcessID = 0;
	// 通过窗口句柄取得进程ID
	::GetWindowThreadProcessId(hwnd, &dwProcessID);
	char szClass[0x256] = { 0 };
	GetClassNameA(hwnd, szClass, 0x256);
	if (dwProcessID == pArg->dwProcessID && strcmp(szClass, pArg->className) == 0)
	{
		pArg->hwndWindow = hwnd;
		// 找到了返回FALSE
		return FALSE;
	}
	// 没找到，继续找，返回TRUE
 
	return TRUE;
}

///通过进程ID获取窗口句柄
HWND  GetWindowHwndByPID(DWORD dwProcessID,PCHAR szClassName)
{
	HWND hwndRet = NULL;
	EnumWindowsArg ewa = { NULL,dwProcessID};
	strcpy_s(ewa.className, szClassName);
	EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
	if (ewa.hwndWindow)
	{
		hwndRet = ewa.hwndWindow;
	}
	return hwndRet;
}

BOOL ModuleExist(IN DWORD dwPID, IN PWCHAR ModuleName, OUT PDWORD moduleBase) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	BOOL isFoundDll = FALSE;
	if (hSnap == NULL)
	{
		cout << "CreateToolhelp32Snapshot failed " << get_last_error(0) << endl;
		return isFoundDll;
	}
	MODULEENTRY32 module = { 0 };
	module.dwSize = sizeof(module);

	if (Module32First(hSnap, &module))
	{
		do
		{
			if (wcscmp(ModuleName, module.szModule) == 0)
			{
				*moduleBase = (DWORD)module.modBaseAddr;
				isFoundDll = true;
				break;
			}
		} while (Module32Next(hSnap, &module));
	}
	CloseHandle(hSnap);
	return isFoundDll;

}
void GetRunningFolderFile(IN PCHAR szFileName, OUT PCHAR FilePath) {
	CHAR curDirPath[MAX_PATH];
	//取当前可执行文件的全路径
	GetModuleFileNameA(NULL, curDirPath, MAX_PATH);
	PathRemoveFileSpecA(curDirPath);
	PathCombineA(FilePath, curDirPath, szFileName);

}