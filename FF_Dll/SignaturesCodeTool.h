#pragma once
#include "ff_offset.h"
#include <string>
#include <vector>
class SignaturesCodeTool
{
public :
	DWORD m_TargetModuleBase;
 	DWORD m_RAVModuleBase;
	DWORD m_RAVModuleSize;

	SignaturesCodeTool() {
		this->m_TargetModuleBase = (DWORD)GetModuleHandle(MODULE_NAME);
		this->m_RAVModuleBase = 0x0;
		this->m_RAVModuleSize = 0x0;
	}

	DWORD InitModuleTextSize();
	PVOID64 Search(const std::string& hexStringData);
	PVOID64 Search(PVOID64 startAddress, size_t size, const std::string& hexStringData);
	PVOID64 Search(PVOID64 startAddress, PVOID64 endAddress, const std::string& hexStringData);
 
private:
	enum class SignatureElementType {
		kNone,
		kWhole,
		kVague
	};

	struct SignatureElement {
		SignatureElementType type;
		size_t length;
		std::vector<unsigned char> data;
	};
	static int __cdecl memcmp_ex(void const* _Buf1, void const* _Buf2, size_t _Size);
	static unsigned int DecStringToUInt(const std::string& str, size_t* i = nullptr, const unsigned char* endCharArr = nullptr, size_t endCharArrSize = 0);
	/*
	* ���������ַ���ת��ΪElement
	* ��׼��ʽʾ���� "48 &?? ?? 65*20 88"
	* &��ʾ����ʱ�Ļ��Դ��ֽ�Ϊ��ʼ��ַ�������ֽ�ǰ�漴�ɣ�ʾ���м�ƫ��Ϊ1
	*	�����һ��&Ϊ׼
	* ??��ʾģ��ƥ����ֽ�
	* *xx��ʾ��һ���ֽڵ��ظ�������ʾ�������ظ�0x65 20�Σ���ʮ����
	*/
	size_t StringToElement(const std::string& hexStringData, std::vector<SignatureElement>& signature, size_t& offset);
};

