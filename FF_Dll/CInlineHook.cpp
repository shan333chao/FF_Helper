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
	// ���汻Hook������ǰ5���ֽ�
	if (ReadProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bOld, 5, &numByte) == 0)
		goto end;
 
 

	WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_nop, 6, &numByte);

	// ����jmpָ��:jmp Ŀ���ַ����Ӧ�Ļ�����:e9 4�ֽڵ�ƫ����
	m_bNew[0] = 0xe9;
	// ʣ��4�ֽ� ��ƫ����
	*(DWORD*)(&m_bNew[0] + 1) = (DWORD)MyFunc - (DWORD)m_pFnOrign - 5;

	// �޸ı�Hook������ǰ5���ֽ� �ı���ִ������
	if (WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bNew, 5, &numByte) == 0)
		goto end;
	ret = true;
end:
	return ret;
}

bool CInlineHook::UnHook()
{
	bool ret = false;
	// ж�ع��ӣ�ʵ���ϼ��ǽ����ĵ�5���ֽڻ�ԭ
	SIZE_T numByte;
	if (m_pFnOrign != NULL && WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bOld, 5, &numByte) != 0)
		ret = true;
	return ret;
}

bool CInlineHook::ReHook()
{
	bool ret = true;
	// �ٴ�װ���ӣ�����ҪHook�ĺ�����ǰ5�ֽ� ������ת��Ŀ�꺯��
	SIZE_T numByte;
	if (m_pFnOrign != NULL && WriteProcessMemory(GetCurrentProcess(), m_pFnOrign, m_bNew, 5, &numByte) != 0)
		ret = true;
	return ret;
}