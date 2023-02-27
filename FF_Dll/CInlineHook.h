#pragma once

#include <Windows.h>

class CInlineHook
{
public:
	CInlineHook(void);
	~CInlineHook(void);

	bool Hook(FARPROC TargetFnAddr, FARPROC MyFunc);

	bool UnHook();
	bool ReHook();
private:

	FARPROC m_pFnOrign; // 要Hook的函数地址
	BYTE m_bOld[6]; // 要Hook的函数 前5个字节
	BYTE m_bNew[6]; // 要Hook的函数 修改后的5个字节
	BYTE m_nop[6];//填充nop
};