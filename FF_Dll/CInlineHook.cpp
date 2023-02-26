#include "pch.h"
#include "CInlineHook.h"

CInlineHook::CInlineHook(void)
{
	m_pFnOrign = NULL;
	memset(m_bOld, 0, 5);
	memset(m_bNew, 0, 5);
}


CInlineHook::~CInlineHook(void)
{
	UnHook();
}

bool CInlineHook::Hook(FARPROC TargetFnAddr, FARPROC MyFunc)
{
	bool ret = false;

	m_pFnOrign = TargetFnAddr;

	BYTE m_nop[6] = { 0x90 ,0x90 ,0x90 ,0x90 ,0x90 ,0x90 };
	SIZE_T numByte;
	// 保存被Hook函数的前5个字节
	if (ReadProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bOld, 5, &numByte) == 0)
		goto end;
 
 

	WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_nop, 6, &numByte);

	// 构造jmp指令:jmp 目标地址，对应的机器码:e9 4字节的偏移量
	m_bNew[0] = 0xe9;
	// 剩余4字节 放偏移量
	*(DWORD*)(&m_bNew[0] + 1) = (DWORD)MyFunc - (DWORD)m_pFnOrign - 5;

	// 修改被Hook函数的前5个字节 改变其执行流程
	if (WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bNew, 5, &numByte) == 0)
		goto end;
	ret = true;
end:
	return ret;
}

bool CInlineHook::UnHook()
{
	bool ret = false;
	// 卸载钩子，实际上即是将更改的5个字节还原
	SIZE_T numByte;
	if (m_pFnOrign != NULL && WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bOld, 5, &numByte) != 0)
		ret = true;
	return ret;
}

bool CInlineHook::ReHook()
{
	bool ret = true;
	// 再次装钩子，更改要Hook的函数的前5字节 让其跳转到目标函数
	SIZE_T numByte;
	if (m_pFnOrign != NULL && WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bNew, 5, &numByte) != 0)
		ret = true;
	return ret;
}