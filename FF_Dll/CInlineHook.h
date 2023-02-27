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

	FARPROC m_pFnOrign; // ҪHook�ĺ�����ַ
	BYTE m_bOld[6]; // ҪHook�ĺ��� ǰ5���ֽ�
	BYTE m_bNew[6]; // ҪHook�ĺ��� �޸ĺ��5���ֽ�
	BYTE m_nop[6];//���nop
};