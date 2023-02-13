#pragma once
#include <Windows.h>
#include <TlHelp32.h>
typedef void (*Function)(DWORD);
/// <summary>
/// ��������Ȩ��
/// </summary>
BOOL EnableDebugPrivilege();
void ShowError(PCHAR pszText);
BOOL ZwCreateThreadExInjectDll(DWORD PID, PCHAR pszDllFileName, PDWORD dllImageBase);
/// <summary>
///  ���ݽ�������ȡPID
/// </summary>
/// <param name="processName">������</param>
/// <param name="ignoreCase">�Ƿ���Դ�Сд</param>
/// <returns>��һ���ҵ���id</returns>
DWORD getProcessByName(WCHAR* processName, BOOL ignoreCase);
/// <summary>
/// ���������������н���id
/// </summary>
/// <param name="processName">������</param>
/// <param name="ignoreCase">�Ƿ���Դ�Сд</param>
/// <param name="foundCount">�ҵ��Ľ�������</param>
/// <returns>���н���id</returns>
void getAllProcessIdsByName(WCHAR* processName, BOOL ignoreCase, PDWORD foundCount, PDWORD pids);

/// <summary>
/// ��������
/// </summary>
/// <param name="pePath">pe�ļ�·��</param>
/// <returns>����id</returns>
DWORD createMyProcess(LPSTR pePath);

/// <summary>
/// �򿪽���
/// </summary>
/// <param name="pid">����id</param>
/// <returns>���̾��</returns>
HANDLE openMyProcess(DWORD pid);

DWORD GetFuncAddr(PCHAR pLibName, PCHAR pFuncName);
/// <summary>
/// pid��ֹ����
/// </summary>
/// <param name="pid">����id</param>
void KillProcessById(DWORD pid);


void KillEachProcessByName(WCHAR* processName, Function pFunc);

/// <summary>
/// ��������ֹ����
/// </summary>
/// <param name="pid">����id</param>

void KillProcessByName(WCHAR* processName);


/// <summary>
/// �������
/// </summary>
/// <param name="pid"></param>
void SuspendProcess(DWORD pid);

/// <summary>
/// �ָ�����
/// </summary>
/// <param name="pid"></param>
void ResumeProcess(DWORD pid);


DWORD ReadRemoteProcessMemory(DWORD pid, LPVOID baseAddr, LPVOID lpBuffer, DWORD size, SIZE_T* lpNumberOfBytesRead);

DWORD WriteRemoteProcessMemory(DWORD pid, LPVOID baseAddr, LPVOID lpBuffer, DWORD size, SIZE_T* lpNumberOfBytesWritten);

/// <summary>
/// Զ���߳�ע��
/// </summary>
/// <param name="dwPid">����id</param>
/// <param name="szDllPath">dll path</param>
/// <param name="exitAddr">dll baseimage</param>
/// <returns></returns>
BOOL  RemoteThreadInject(DWORD dwPid, PCHAR szDllPath, PDWORD exitAddr);


DWORD GetProcessIdByWindowName(LPCWSTR lpClassName, LPCWSTR lpWindowName);



DWORD FreeInjectDll(DWORD dwPID, PWCHAR pDllPath);


DWORD FreeInjectDllByExitAddr(DWORD dwPID, DWORD imageBaseAddr);
/// <summary>
/// �ٳ��߳�ע��
/// </summary>
/// <param name="dwPID">����id</param>
/// <param name="pDllPath">DLL path</param>
/// <returns></returns>
//DWORD HijackThreadInject(DWORD dwPID, PCHAR pDllPath);



/// <summary>
/// �ٳֽ���ע��
/// </summary>
/// <param name="dwPID">����id</param>
/// <param name="pDllPath">DLL path</param>
/// <returns></returns>
DWORD HjackProcessInject(PCHAR peFilePath, PCHAR pDllPath, PDWORD exitAddr);


/// <summary>
/// APC ע��
/// </summary>
/// <param name="dwPID"></param>
/// <param name="pDllPath"></param>
/// <returns></returns>
DWORD ApcInject(DWORD dwPID, PCHAR pDllPath);



HWND  GetWindowHwndByPID(DWORD dwProcessID, PCHAR szClassName);


BOOL ModuleExist(IN DWORD dwPID, IN PWCHAR ModuleName, OUT PDWORD moduleBase);


void GetRunningFolderFile(IN PCHAR szFileName, OUT PCHAR FilePath);