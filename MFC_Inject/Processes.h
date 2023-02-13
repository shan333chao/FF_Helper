#pragma once
#include <Windows.h>
#include <TlHelp32.h>
typedef void (*Function)(DWORD);
/// <summary>
/// 提升调试权限
/// </summary>
BOOL EnableDebugPrivilege();
void ShowError(PCHAR pszText);
BOOL ZwCreateThreadExInjectDll(DWORD PID, PCHAR pszDllFileName, PDWORD dllImageBase);
/// <summary>
///  根据进程名获取PID
/// </summary>
/// <param name="processName">进程名</param>
/// <param name="ignoreCase">是否忽略大小写</param>
/// <returns>第一个找到的id</returns>
DWORD getProcessByName(WCHAR* processName, BOOL ignoreCase);
/// <summary>
/// 按进程名查找所有进程id
/// </summary>
/// <param name="processName">进程名</param>
/// <param name="ignoreCase">是否忽略大小写</param>
/// <param name="foundCount">找到的进程数量</param>
/// <returns>所有进程id</returns>
void getAllProcessIdsByName(WCHAR* processName, BOOL ignoreCase, PDWORD foundCount, PDWORD pids);

/// <summary>
/// 启动进程
/// </summary>
/// <param name="pePath">pe文件路径</param>
/// <returns>进程id</returns>
DWORD createMyProcess(LPSTR pePath);

/// <summary>
/// 打开进程
/// </summary>
/// <param name="pid">进程id</param>
/// <returns>进程句柄</returns>
HANDLE openMyProcess(DWORD pid);

DWORD GetFuncAddr(PCHAR pLibName, PCHAR pFuncName);
/// <summary>
/// pid终止进程
/// </summary>
/// <param name="pid">进程id</param>
void KillProcessById(DWORD pid);


void KillEachProcessByName(WCHAR* processName, Function pFunc);

/// <summary>
/// 进程名终止进程
/// </summary>
/// <param name="pid">进程id</param>

void KillProcessByName(WCHAR* processName);


/// <summary>
/// 挂起进程
/// </summary>
/// <param name="pid"></param>
void SuspendProcess(DWORD pid);

/// <summary>
/// 恢复进程
/// </summary>
/// <param name="pid"></param>
void ResumeProcess(DWORD pid);


DWORD ReadRemoteProcessMemory(DWORD pid, LPVOID baseAddr, LPVOID lpBuffer, DWORD size, SIZE_T* lpNumberOfBytesRead);

DWORD WriteRemoteProcessMemory(DWORD pid, LPVOID baseAddr, LPVOID lpBuffer, DWORD size, SIZE_T* lpNumberOfBytesWritten);

/// <summary>
/// 远程线程注入
/// </summary>
/// <param name="dwPid">进程id</param>
/// <param name="szDllPath">dll path</param>
/// <param name="exitAddr">dll baseimage</param>
/// <returns></returns>
BOOL  RemoteThreadInject(DWORD dwPid, PCHAR szDllPath, PDWORD exitAddr);


DWORD GetProcessIdByWindowName(LPCWSTR lpClassName, LPCWSTR lpWindowName);



DWORD FreeInjectDll(DWORD dwPID, PWCHAR pDllPath);


DWORD FreeInjectDllByExitAddr(DWORD dwPID, DWORD imageBaseAddr);
/// <summary>
/// 劫持线程注入
/// </summary>
/// <param name="dwPID">进程id</param>
/// <param name="pDllPath">DLL path</param>
/// <returns></returns>
//DWORD HijackThreadInject(DWORD dwPID, PCHAR pDllPath);



/// <summary>
/// 劫持进程注入
/// </summary>
/// <param name="dwPID">进程id</param>
/// <param name="pDllPath">DLL path</param>
/// <returns></returns>
DWORD HjackProcessInject(PCHAR peFilePath, PCHAR pDllPath, PDWORD exitAddr);


/// <summary>
/// APC 注入
/// </summary>
/// <param name="dwPID"></param>
/// <param name="pDllPath"></param>
/// <returns></returns>
DWORD ApcInject(DWORD dwPID, PCHAR pDllPath);



HWND  GetWindowHwndByPID(DWORD dwProcessID, PCHAR szClassName);


BOOL ModuleExist(IN DWORD dwPID, IN PWCHAR ModuleName, OUT PDWORD moduleBase);


void GetRunningFolderFile(IN PCHAR szFileName, OUT PCHAR FilePath);