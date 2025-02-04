#pragma once
#include "pch.h"
struct HookContext
{
	DWORD ELF;
	DWORD EDI;
	DWORD ESI;
	DWORD EBP;
	DWORD ESP;
	DWORD EBX;
	DWORD EDX;
	DWORD ECX;
	DWORD EAX;
};
 
typedef void(_stdcall* InlineFunc)(HookContext*);

class CHookUtil
{
public:
	CHookUtil(void);
	~CHookUtil(void);

	bool Hook(DWORD pTarget, InlineFunc func);

	bool UnHook();
 
private:
	DWORD m_pFnOrign; // 要Hook的函数地址
	BYTE m_OldCode[5];
 
};

 