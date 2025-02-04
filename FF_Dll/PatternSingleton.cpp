#include "pch.h"
#include "PatternSingleton.h"
#include "FeatureCode.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <Shlwapi.h>    // ���������ͷ�ļ�
#pragma comment(lib, "Shlwapi.lib") // ��Ҫ���� Shlwapi ��
#define MODULE_NAME  _T("Neuz.exe")
#define PATTERN_FILE "Neuz.bp" // ͳһʹ�� ANSI �ַ���

bool PatternSingleton::IsPatternFileExist(PCHAR FilePath)
{


	CHAR curDirPath[MAX_PATH] = { 0 };
	// ��ȡ��ǰ��ִ���ļ�·��
	if (GetModuleFileNameA(NULL, curDirPath, MAX_PATH) == 0) {
		// ������GetLastError()
		return false;
	}
	// ȥ���ļ�������Ŀ¼
	if (!PathRemoveFileSpecA(curDirPath)) {
		// ������Ŀ¼����ʧ��
		return false;
	}

	// ��ȫ���·��
	if (!PathCombineA(FilePath, curDirPath, PATTERN_FILE)) {
		// ������·�����ʧ��
		return false;
	}
	return PathFileExistsA(FilePath) && !PathIsDirectoryA(FilePath);
}

void GetPatternContent(PCHAR filePath, PCHAR fileBuffer) {

	std::string strPath = CStringA(filePath);
	FILE* pFile = nullptr;
	fopen_s(&pFile, strPath.c_str(), "rb");
	if (pFile == nullptr)
	{
		MessageBoxA(NULL, filePath, "Pattern File OpenFailed", MB_OK);
		return;
	}

	fseek(pFile, 0, SEEK_END);//���ļ�ָ���ƶ����ļ�β
	long fileSize = ftell(pFile);//��ȡ�ļ���С
	char* szMarkCode = new char[fileSize + 2];//�����ڴ�
	memset(szMarkCode, 0, fileSize + 2);
	rewind(pFile);

	//��ȡ�ļ�����ʾ
	fread(szMarkCode, 1, fileSize, pFile);
	szMarkCode[fileSize] = '\0';
	memcpy(fileBuffer, szMarkCode, fileSize+2);
	delete szMarkCode;
	fclose(pFile);

}
//��ȡ�ļ�����
std::vector<CString> ReadAllLinesAsCString(const CHAR* filename) {
	std::vector<CString> lines;
	std::ifstream file(filename, std::ios::binary);  // ����ԭʼ��������

	if (!file.is_open()) {
		CStringA errMsg;
		errMsg.Format("�޷����ļ�: %s (������: 0x%08X)", filename, GetLastError());
		throw std::runtime_error(errMsg.GetString());
	}

	// һ���Զ�ȡȫ������
	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string& content = buffer.str();

	// ��Ч�ָ��У��Զ����� \r\n �� \n��
	size_t pos = 0;
	const size_t len = content.length();
	const char* data = content.data();

	while (pos < len) {
		size_t newline_pos = pos;

		// ���һ��з�
		while (newline_pos < len &&
			data[newline_pos] != '\r' &&
			data[newline_pos] != '\n') {
			++newline_pos;
		}

		// ֱ�ӹ��� CString�������м� string��
		lines.emplace_back(
			data + pos,                            // ��ʼָ��
			static_cast<int>(newline_pos - pos)    // �ַ�����
		);

		// �������з�
		pos = newline_pos;
		if (pos < len) {
			if (data[pos] == '\r') ++pos;
			if (pos < len && data[pos] == '\n') ++pos;
		}
	}

	return lines;
}
//���ÿһ�е������� ���Ÿ���
std::vector<CString>  SplitString(const CString& str, TCHAR delimiter)
{
	//���ݴ�������delimiter�ָ��ַ��� ���洢��vector���ٷ���
	std::vector<CString> tokens;
	int start = 0;
	for (int i = 0; i < str.GetLength(); i++)
	{
		if (str[i] == delimiter)
		{
			CString token = str.Mid(start, i - start);
			tokens.push_back(token);
			start = i + 1;
		}
	}
	CString token = str.Mid(start);
	tokens.push_back(token);
	return tokens;
}

void PatternSingleton::InitSignatureDataAddr()
{
	CHAR patternFilePath[MAX_PATH] = { 0 };
	if (IsPatternFileExist(patternFilePath))
	{
		MessageBoxA(NULL, patternFilePath, "Pattern File Path", MB_OK);
		return;
	}
	auto lines = ReadAllLinesAsCString(patternFilePath);
	CFeatureCode sct(MODULE_NAME);
	for (const auto& line : lines) {
		if (line.IsEmpty())
		{
			continue;
		}
		//��ȡ ע�� ������ ƫ�� ��ȡ���� ��ȡ��ʽ(��ַ ƫ�� CALL)
		std::vector<CString> vecMarkCodeLine = SplitString(line, _T(',')); 
		std::string strMarkCode = CStringA(vecMarkCodeLine[2]);
		std::vector<DWORD> dwRetAddr;
		bool isCall = vecMarkCodeLine[5].CompareNoCase(_T("CALL")) == 0;
		bool isBase = vecMarkCodeLine[4].CompareNoCase(_T("��ַ")) == 0;
		bool isOffset = vecMarkCodeLine[4].CompareNoCase(_T("ƫ��")) == 0;
		DWORD foundCount= sct.SearchPatternE(strMarkCode, dwRetAddr, _ttoi(vecMarkCodeLine[3]), isCall, false);
		if (foundCount>0)
		{
			if (true)
			{

			}
			DWORD addr = *(PDWORD)dwRetAddr[0];
			mapSignatureValue.insert(std::make_pair(CStringA(vecMarkCodeLine[1]), addr));
		}
	}
}

PatternSingleton::PatternSingleton()
{
	InitSignatureDataAddr();
}
PatternSingleton::~PatternSingleton()
{
}

DWORD PatternSingleton::GetAddressByKey(std::string key)
{
	if (mapSignatureValue.count(key) > 0)
	{
		return mapSignatureValue.at(key);
	}
	return NULL;
}

PatternSingleton& PatternSingleton::getInstance()
{
	static PatternSingleton instance;
	return instance;
}

std::map<std::string, DWORD> PatternSingleton::GetAllSignatureValue()
{
	return this->mapSignatureValue;
}
