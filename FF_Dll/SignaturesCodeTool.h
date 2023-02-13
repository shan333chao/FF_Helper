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
	* 将特征码字符串转换为Element
	* 标准格式示例： "48 &?? ?? 65*20 88"
	* &表示返回时的会以此字节为起始地址，加在字节前面即可，示例中即偏移为1
	*	以最后一个&为准
	* ??表示模糊匹配此字节
	* *xx表示上一个字节的重复次数，示例就是重复0x65 20次，是十进制
	*/
	size_t StringToElement(const std::string& hexStringData, std::vector<SignatureElement>& signature, size_t& offset);
};

