#include "pch.h"
#include "SignaturesCodeTool.h"

DWORD SignaturesCodeTool::InitModuleTextSize()
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)this->m_TargetModuleBase;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(this->m_TargetModuleBase + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((PCHAR)pNTHeader + 4);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((PCHAR)pFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((PCHAR)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	DWORD imageBase = pOptionalHeader->ImageBase;
	DWORD count = pNTHeader->FileHeader.NumberOfSections;
	for (size_t i = 0; i < count; i++)
	{
		if (pSectionHeader->Characteristics == SECTION_CHARACTERISTICS)
		{
			this->m_RAVModuleBase = imageBase + pSectionHeader->VirtualAddress;
			this->m_RAVModuleSize = pSectionHeader->SizeOfRawData;
			return TRUE;
		}
		pSectionHeader++;
	}
	return FALSE;
}

PVOID64 SignaturesCodeTool::Search(const std::string& hexStringData)
{
	return Search((PVOID64)this->m_RAVModuleBase, this->m_RAVModuleSize, hexStringData);
}

PVOID64 SignaturesCodeTool::Search(PVOID64 startAddress, size_t size, const std::string& hexStringData)
{
	std::vector<SignatureElement> signature;
	size_t offset = 0, totalLength = StringToElement(hexStringData, signature, offset);

	size_t SignatureSize = signature.size();
	if (!SignatureSize) return nullptr;

	std::vector<char> buf;
	int64_t base = 0;


	for (size_t i = 0; i < size; ++i) {
		uint64_t currentPos = (uint64_t)startAddress + i;
		uint64_t returnPos = currentPos;
		if (i + totalLength > size) break;
		bool match = true;

		for (size_t j = 0; j < SignatureSize; ++j) {
			size_t length = signature[j].length;

			if (signature[j].type == SignatureElementType::kWhole) {
				int ret = memcmp_ex((void*)currentPos, signature[j].data.data(), length);
				if (ret == 1) {
					match = false;
					break;
				}
				else if (ret == 2) {
					return nullptr;
				}

			}
			currentPos = currentPos + length;
		}
		if (match) {
			return (PVOID)(base + returnPos + offset);
		}

	}
	return nullptr;
}

PVOID64 SignaturesCodeTool::Search(PVOID64 startAddress, PVOID64 endAddress, const std::string& hexStringData)
{
	return Search(startAddress, (uint64_t)endAddress - (uint64_t)startAddress + 1, hexStringData);
}

int __cdecl SignaturesCodeTool::memcmp_ex(void const* _Buf1, void const* _Buf2, size_t _Size)
{
	__try {
		if (memcmp(_Buf1, _Buf2, _Size)) {
			return 1;
		}
		else {
			return 0;
		}

	}
	__except (1) {
		return 2;
	}
}

unsigned int SignaturesCodeTool::DecStringToUInt(const std::string& str, size_t* i, const unsigned char* endCharArr, size_t endCharArrSize)
{
	unsigned int sum = 0;
	if (!i) {
		size_t j;
		i = &j;
	}
	for (*i = 0; *i < str.length(); ++ * i) {
		unsigned char c = str[*i];
		if (c >= 0x30 && c <= 0x39) {
			c -= 0x30;
			sum = sum * 10 + c;
		}
		//如果设置了结束字符，除开结束字符其他的一律不管
		else if (endCharArr) {
			for (size_t j = 0; j < endCharArrSize; ++j) {
				if (c == endCharArr[j]) return sum;
			}
		}
		//不需要-1，因为计数本来就要比索引多1
		else break;

	}
	return sum;
}

size_t SignaturesCodeTool::StringToElement(const std::string& hexStringData, std::vector<SignatureElement>& signature, size_t& offset)
{
	bool first = true;
	unsigned char sum = 0;
	SignatureElement tempSignatureElement;
	tempSignatureElement.length = 0;
	SignatureElementType oldType = SignatureElementType::kNone, newType = SignatureElementType::kNone;
	size_t totalLength = 0;

	//遍历字符
	for (size_t i = 0; i < hexStringData.length(); ++i) {
		unsigned char c = hexStringData[i];
		bool validChar = true;
		if (c >= 0x30 && c <= 0x39) {
			c -= 0x30;
			newType = SignatureElementType::kWhole;
		}
		else if (c >= 0x41 && c <= 0x46) {
			c = c - 0x37;
			newType = SignatureElementType::kWhole;
		}
		else if (c >= 0x61 && c <= 0x66) {
			c = c - 0x57;
			newType = SignatureElementType::kWhole;
		}
		else if (c == '?') {
			newType = SignatureElementType::kVague;
		}
		else {
			if (c == '&') {
				offset = totalLength + tempSignatureElement.length;
			}
			else if (c == '*' && i + 1 < hexStringData.length()) {
				// 如果*号后面还有字符，将其视作重复次数
				size_t countInt;
				unsigned int lenInt = DecStringToUInt(&hexStringData[i] + 1, &countInt) - 1;
				if (countInt) {
					if (oldType == SignatureElementType::kWhole && tempSignatureElement.data.size() > 0) {
						unsigned char repC = tempSignatureElement.data[tempSignatureElement.data.size() - 1];
						for (size_t j = 0; j < lenInt; ++j) {
							tempSignatureElement.data.push_back(repC);
						}
					}
					tempSignatureElement.length += lenInt;
					i += countInt;
				}

			}

			// 无效字符，不需要检测类型
			validChar = false;
			goto _PushChar;
		}

		if (oldType == SignatureElementType::kNone) {
			// 如果旧类型未初始化，将新类型赋值给旧类型
			oldType = newType;
		}

		// 旧字符类型和新字符类型不同时，需要添加新的匹配单元
		else if (oldType != newType) {
			tempSignatureElement.type = oldType;
			totalLength += tempSignatureElement.length;
			signature.push_back(tempSignatureElement);

			oldType = newType;
			tempSignatureElement.length = 0;
			tempSignatureElement.data.clear();
		}

	_PushChar:
		// 如果原先类型是精确匹配，则添加字符
		if (oldType == SignatureElementType::kWhole) {
			if (first && validChar) {
				sum = c << 4;
				first = false;
			}
			else if (!first) {
				first = true;
				// 如果是无效字符，说明调用者并未提供连续的两个有效字符，将修正第一个有效字符的值
				validChar ? sum += c : sum >>= 4;
				tempSignatureElement.data.push_back(sum);
				++tempSignatureElement.length;
			}

			// 最后一种情况就是，即未开始拼接字节，且是无效字符，直接无视即可
		}

		// 模糊匹配，是第二个符号就直接递增长度
		else if (oldType == SignatureElementType::kVague) {
			if (first && validChar) {
				first = false;
			}
			else if (!first) {
				first = true;
				++tempSignatureElement.length;
			}
		}

	}

	//如果有未完成的字符，则推入
	if (!first) {
		if (oldType == SignatureElementType::kWhole) {
			tempSignatureElement.data.push_back(sum >> 4);
		}
		++tempSignatureElement.length;
	}

	//有匹配单元，推入
	if (tempSignatureElement.length > 0 || tempSignatureElement.data.size() > 0) {
		tempSignatureElement.type = oldType;
		totalLength += tempSignatureElement.length;
		signature.push_back(tempSignatureElement);
	}

	return totalLength;
}