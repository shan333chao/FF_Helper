#include "pch.h"
#include "FeatureCode.h"

using namespace std;

CFeatureCode::~CFeatureCode(void)
{}

//Sunday�㷨���� ���ϳ����� ��л����  ֮���Զ���һ��������Ϊ�������������ɨ���㷨������
//2020.8.9������£��޸�ͨ�����СBUG����BUG���м��ͼ����������������޸�
//2022.4.4������£����һ����룬�޸���ʼ��ַ��СBUG����BUG����С�����Ѳ�����Ҫ���ڴ�

/*
FindMatchingCode() ����˵����
1) hProcess		Ҫ�򿪵Ľ��̾��
2) markCode		������,֧��ͨ�����??������: 56 8B 01 FF 90 ?? ?? ?? ?? 85 C0 74
3) memBeginAddr		��ʼ������ַ
4) memEndAddr		����������ַ
5) retAddr[]		��¼�ҵ��ĵ�ַ,�����������ǰһ��Ҫ��0���� DWORD retAddr[32] = {0};  ���� DWORD *retAddr = new DWORD[32]();
6) deviation		�������ַ��Ŀ���ַ��ƫ�ƾ��룬�ϸ�����
7) isCall		�Ƿ�Ϊ��CALL����ת��ַ��true �� retAddr[] ���ص���CALL��ת�ĵ�ַ
8) isAll		�Ƿ�������з��ϵĵ�ַ��false�ҵ���һ�����ϵĵ�ַ��ͽ���������true����������ֱ��������ַ���ڽ�����ַ��memEndAddr��
return����ֵ		�ҵ��ĵ�ַ����
�Ѳ����ڴ�����Ǳ�������û��ѡ��
*/
DWORD CFeatureCode::FindMatchingCode(HANDLE hProcess, string markCode, DWORD memBeginAddr, DWORD memEndAddr, DWORD retAddr[], int deviation, bool isCall, bool isAll)
{
	//----------------------����������----------------------//
	//ȥ�����пո�
	if (!markCode.empty())
	{
		int index = 0;
		while ((index = markCode.find(' ', index)) >= 0)
		{
			markCode.erase(index, 1);
		}
		index = 0;
		while (true)
		{
			//ɾ��ͷ��ͨ���
			index = markCode.find("??", index);
			if (index == 0)
			{
				markCode.erase(index, 2);
			}
			else
			{
				break;
			}
		}
	}


	//�����볤�Ȳ���Ϊ����
	if (markCode.length() % 2 != 0) return 0;

	//�����볤��
	int len = markCode.length() / 2;

	//Sunday�㷨ģ������ĳ���
	int nSundayLen = len;

	//��������ת����byte��
	BYTE* pMarkCode = new BYTE[len];
	BYTE* pWildcard = new BYTE[len];
	for (int i = 0; i < len; i++)
	{
		string tempStr = markCode.substr(i * 2, 2);
		if (tempStr == "??")
		{
			pWildcard[i] = 0xFF;
			if (nSundayLen == len)
			{
				nSundayLen = i;	//��¼��һ��ͨ�����������������Խ����Ч��Խ��
			}
		}
		else
		{
			pWildcard[i] = 0x00;
		}
		pMarkCode[i] = strtoul(tempStr.c_str(), 0, 16);
	}
	//--------------------------end-------------------------//

	//Sunday�㷨ģ�����鸳ֵ��+1��ֹ���������FFʱԽ��
	int aSunday[0xFF + 1] = { 0 };
	for (int i = 0; i < nSundayLen; i++)
	{
		aSunday[pMarkCode[i]] = i + 1;
	}

	//��ʼ��ַ
	const DWORD dwBeginAddr = memBeginAddr;
	//������ַ
	const DWORD dwEndAddr = memEndAddr;
	//��ǰ��ȡ���ڴ���ַ
	DWORD dwCurAddr = dwBeginAddr;
	//����ڴ����ݵĻ�����
	BYTE* pMemBuffer = NULL;
	//�������retAddr[]����ĳ��ȣ��ò�������ǰһ��Ҫ��0
	int nArrayLength = 0;
	for (int i = 0;; i++)
	{
		if (*(retAddr + i) != 0)
		{
			nArrayLength = i;
			break;
		}
	}
	//ƫ����
	int nOffset;
	//�����±꣺�ڴ桢�����롢���ص�ַ
	int i = 0;
	int j = 0;
	int nCount = 0;

	//�ڴ���Ϣ
	MEMORY_BASIC_INFORMATION mbi;

	//ɨ���ڴ�
	while (dwCurAddr < dwEndAddr)
	{
		//��ѯ��ַ�ռ����ڴ��ַ����Ϣ
		memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));
		if (::VirtualQueryEx(hProcess, (LPCVOID)dwCurAddr, &mbi, sizeof(mbi)) == 0)
		{
			goto end;
		}

		//���һ����룬�޸ĵ�ǰ��ַ������ڴ�ҳ����ʼ��ַ���༭��2022.4.4��û�������и����Ѳ�����Ҫ���ڴ�
		dwCurAddr = (DWORD_PTR)mbi.BaseAddress;
		//-----------------------------------------------------//

		//�����ڴ�ռ�, �����ڴ��״̬�ͱ������Խ��й���
		//һ��ɨ�裨��д��ִ�У����ɣ��ٶȼ��죬ɨ�����Ļ��ڳ�����ӣ���д����һ����
		if (MEM_COMMIT == mbi.State &&			//�ѷ���������ڴ�
			MEM_PRIVATE == mbi.Type ||		//˽���ڴ棬�����������̹���
			MEM_IMAGE == mbi.Type &&
			PAGE_READONLY == mbi.Protect ||	//ֻ��
			PAGE_EXECUTE_READ == mbi.Protect ||	//����ִ��
			PAGE_READWRITE == mbi.Protect ||	//��д
			PAGE_EXECUTE_READWRITE == mbi.Protect)	//��д��ִ��
		{
			//���붯̬�ڴ�
			if (pMemBuffer)
			{
				delete[] pMemBuffer;
				pMemBuffer = NULL;
			}
			pMemBuffer = new BYTE[mbi.RegionSize];
			//��ȡ�����ڴ�
			ReadProcessMemory(hProcess, (LPCVOID)dwCurAddr, pMemBuffer, mbi.RegionSize, 0);
			i = 0;
			j = 0;
			while (j < len)
			{
			nextAddr:
				if (pMemBuffer[i] == pMarkCode[j] || pWildcard[j] == 0xFF)
				{
					i++;
					j++;
				}
				else
				{
					nOffset = i - j + nSundayLen;
					//�ж�ƫ�����Ƿ���ڻ�����
					if (nOffset > mbi.RegionSize - len)
						break;
					//�ж� aSundayģ������ ����û�� �ڴ�ƫ�ƺ��ֵ��������ݣ�����+1
					if (aSunday[pMemBuffer[nOffset]])
					{
						i = nOffset - aSunday[pMemBuffer[nOffset]] + 1;
						j = 0;
					}
					else
					{
						i = nOffset + 1;
						j = 0;
					}
				}
			}

			if (j == len)
			{
				//�����ҵ���Ŀ���ַ��
				//�������ַ = ��ǰ�ڴ���ַ + iƫ�� - �����볤��
				//Ŀ���ַ = �������ַ + ƫ�ƾ���
				//CALL��E8����ת�ĵ�ַ = E8ָ������4���ֽڵ�ַ + ��һ��ָ���ַ(Ҳ����Ŀ���ַ + 5)
				retAddr[nCount] = dwCurAddr + i - len + deviation;
				if (isCall)
				{
					DWORD temp;
					memcpy(&temp, &pMemBuffer[i - len + deviation + 1], 4);
					retAddr[nCount] += 5;
					retAddr[nCount] += temp;
				}

				if (++nCount >= nArrayLength)
				{
					//����������±�Խ��ͽ�������
					goto end;
				}

				if (isAll)
				{
					i = i - len + 1;
					j = 0;
					goto nextAddr;
				}
				else
				{
					goto end;
				}
			}
			dwCurAddr += mbi.RegionSize; //ȡ��һ���ڴ��ַ
		}
		else
		{
			dwCurAddr += mbi.RegionSize;
		}
	}


end:
	//�ͷ��ڴ�
	if (pMemBuffer)
	{
		delete[] pMemBuffer;
		pMemBuffer = NULL;
	}
	delete[] pMarkCode;
	pMarkCode = NULL;
	delete[] pWildcard;
	pWildcard = NULL;
	return nCount;
}

DWORD CFeatureCode::FindPatternCode(std::string strMarkCode, DWORD dwRetAddr[], int nDeviation, bool bIsCall, bool bIsAll)
{
	return this->FindMatchingCode(GetCurrentProcess(), strMarkCode, this->m_RAVModuleBase, this->m_RAVModuleBase + this->m_RAVModuleSize, dwRetAddr, nDeviation, bIsCall, bIsAll);

}

DWORD CFeatureCode::InitModuleTextSize()
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
		if (pSectionHeader->Characteristics == 0x60000020)
		{
			this->m_RAVModuleBase = imageBase + pSectionHeader->VirtualAddress;
			this->m_RAVModuleSize = pSectionHeader->SizeOfRawData;
			return TRUE;
		}
		pSectionHeader++;
	}
	return FALSE;
}


// ������ʵ�֣��ؼ��޸ĵ���ע��˵����
/*
FindMatchingCode() ����˵����
1) markCode		������,֧��ͨ�����??������: 56 8B 01 FF 90 ?? ?? ?? ?? 85 C0 74
2) memBeginAddr		��ʼ������ַ
3) memEndAddr		����������ַ
4) vector<DWORD>& retAddr		��¼�ҵ��ĵ�ַ,�����������ǰһ��Ҫ��0���� DWORD retAddr[32] = {0};  ���� DWORD *retAddr = new DWORD[32]();
5) deviation		�������ַ��Ŀ���ַ��ƫ�ƾ��룬�ϸ�����
6) isCall		�Ƿ�Ϊ��CALL����ת��ַ��true �� retAddr[] ���ص���CALL��ת�ĵ�ַ
7) isAll		�Ƿ�������з��ϵĵ�ַ��false�ҵ���һ�����ϵĵ�ַ��ͽ���������true����������ֱ��������ַ���ڽ�����ַ��memEndAddr��
return����ֵ		�ҵ��ĵ�ַ����
�Ѳ����ڴ�����Ǳ�������û��ѡ��
*/
DWORD CFeatureCode::SearchPattern(std::string markCode, DWORD memBeginAddr, DWORD memEndAddr, std::vector<DWORD>& retAddr, int deviation, bool isCall, bool isAll)
{
	//------------------------ ������Ԥ���� ------------------------//
	// ȥ�����пո�
	if (!markCode.empty()) {
		int index = 0;
		while ((index = markCode.find(' ', index)) >= 0) {
			markCode.erase(index, 1);
		}
		// ����ͷ��ͨ���
		index = 0;
		while (true) {
			index = markCode.find("??", index);
			if (index == 0) {
				markCode.erase(index, 2);
			}
			else {
				break;
			}
		}
	}

	// ��������Ч�Լ��
	if (markCode.length() % 2 != 0) return 0;
	const int len = markCode.length() / 2;

	// ת��������Ϊ�ֽ�����
	vector<BYTE> pMarkCode(len);
	vector<BYTE> pWildcard(len, 0x00);
	int nSundayLen = len;
	for (int i = 0; i < len; i++) {
		string tempStr = markCode.substr(i * 2, 2);
		if (tempStr == "??") {
			pWildcard[i] = 0xFF;
			if (nSundayLen == len) {
				nSundayLen = i;  // Sunday�㷨�Ż���
			}
		}
		else {
			pMarkCode[i] = static_cast<BYTE>(strtoul(tempStr.c_str(), nullptr, 16));
		}
	}

	//------------------------ Sunday�㷨׼�� ------------------------//
	int aSunday[256] = { 0 };
	for (int i = 0; i < nSundayLen; i++) {
		aSunday[pMarkCode[i]] = i + 1;
	}

	//------------------------ �ڴ�ɨ���߼� ------------------------//
	DWORD dwCurAddr = memBeginAddr;
	MEMORY_BASIC_INFORMATION mbi;
	DWORD nCount = 0;

	while (dwCurAddr < memEndAddr) {
		// ��ѯ�������ڴ���Ϣ��ʹ��VirtualQuery�滻VirtualQueryEx��
		if (::VirtualQuery((LPCVOID)dwCurAddr, &mbi, sizeof(mbi)) == 0) {
			break;
		}

		dwCurAddr = (DWORD)mbi.BaseAddress;  // ���뵽�ڴ����ʼ��ַ

		// ���˿ɶ��ڴ����򣨸�����������������ԣ�
		if (mbi.State == MEM_COMMIT &&
			(mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)))
		{
			const BYTE* pMem = reinterpret_cast<BYTE*>(dwCurAddr);  // ֱ�ӷ����ڴ�
			const SIZE_T regionSize = mbi.RegionSize;

			for (SIZE_T offset = 0; offset <= regionSize - len; ) {
				bool found = true;
				// ������ƥ����
				for (int j = 0; j < len; j++) {
					if (pWildcard[j] != 0xFF &&
						pMem[offset + j] != pMarkCode[j]) {
						found = false;
						break;
					}
				}

				if (found) {
					// ����Ŀ���ַ
					DWORD targetAddr = dwCurAddr + offset + deviation;
					if (isCall) {
						// ֱ�Ӷ�ȡ�ڴ棨����ReadProcessMemory��
						DWORD callOffset = 0;
						memcpy(&callOffset, reinterpret_cast<const void*>(targetAddr + 1), 4);
						targetAddr = targetAddr + 5 + callOffset;
					}
					retAddr.push_back(targetAddr);
					nCount++;

					if (!isAll) return nCount;
					offset += len;  // ����������һ��
				}
				else {
					// Sunday�㷨��ת
					const BYTE nextChar = pMem[offset + nSundayLen];
					offset += (nSundayLen - aSunday[nextChar] + 1);
				}
			}
		}
		dwCurAddr += mbi.RegionSize;  // �ƶ�����һ���ڴ��
	}
	return nCount;
}


DWORD CFeatureCode::SearchPatternE(std::string markCode, std::vector<DWORD>& retAddr, int deviation, bool isCall, bool isAll) {

	return	SearchPattern(markCode, this->m_RAVModuleBase, this->m_RAVModuleBase + this->m_RAVModuleSize, retAddr, deviation, isCall, isAll);

};
