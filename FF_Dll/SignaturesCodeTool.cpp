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
		//��������˽����ַ������������ַ�������һ�ɲ���
		else if (endCharArr) {
			for (size_t j = 0; j < endCharArrSize; ++j) {
				if (c == endCharArr[j]) return sum;
			}
		}
		//����Ҫ-1����Ϊ����������Ҫ��������1
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

	//�����ַ�
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
				// ���*�ź��滹���ַ������������ظ�����
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

			// ��Ч�ַ�������Ҫ�������
			validChar = false;
			goto _PushChar;
		}

		if (oldType == SignatureElementType::kNone) {
			// ���������δ��ʼ�����������͸�ֵ��������
			oldType = newType;
		}

		// ���ַ����ͺ����ַ����Ͳ�ͬʱ����Ҫ����µ�ƥ�䵥Ԫ
		else if (oldType != newType) {
			tempSignatureElement.type = oldType;
			totalLength += tempSignatureElement.length;
			signature.push_back(tempSignatureElement);

			oldType = newType;
			tempSignatureElement.length = 0;
			tempSignatureElement.data.clear();
		}

	_PushChar:
		// ���ԭ�������Ǿ�ȷƥ�䣬������ַ�
		if (oldType == SignatureElementType::kWhole) {
			if (first && validChar) {
				sum = c << 4;
				first = false;
			}
			else if (!first) {
				first = true;
				// �������Ч�ַ���˵�������߲�δ�ṩ������������Ч�ַ�����������һ����Ч�ַ���ֵ
				validChar ? sum += c : sum >>= 4;
				tempSignatureElement.data.push_back(sum);
				++tempSignatureElement.length;
			}

			// ���һ��������ǣ���δ��ʼƴ���ֽڣ�������Ч�ַ���ֱ�����Ӽ���
		}

		// ģ��ƥ�䣬�ǵڶ������ž�ֱ�ӵ�������
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

	//�����δ��ɵ��ַ���������
	if (!first) {
		if (oldType == SignatureElementType::kWhole) {
			tempSignatureElement.data.push_back(sum >> 4);
		}
		++tempSignatureElement.length;
	}

	//��ƥ�䵥Ԫ������
	if (tempSignatureElement.length > 0 || tempSignatureElement.data.size() > 0) {
		tempSignatureElement.type = oldType;
		totalLength += tempSignatureElement.length;
		signature.push_back(tempSignatureElement);
	}

	return totalLength;
}