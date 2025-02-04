#include "pch.h"
#include "PatternSingleton.h"
#include "FeatureCode.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <Shlwapi.h>    // 必须包含该头文件
#pragma comment(lib, "Shlwapi.lib") // 需要链接 Shlwapi 库
#define MODULE_NAME  _T("Neuz.exe")
#define PATTERN_FILE "Neuz.bp" // 统一使用 ANSI 字符集

bool PatternSingleton::IsPatternFileExist(PCHAR FilePath)
{


	CHAR curDirPath[MAX_PATH] = { 0 };
	// 获取当前可执行文件路径
	if (GetModuleFileNameA(NULL, curDirPath, MAX_PATH) == 0) {
		// 错误处理：GetLastError()
		return false;
	}
	// 去除文件名保留目录
	if (!PathRemoveFileSpecA(curDirPath)) {
		// 错误处理：目录操作失败
		return false;
	}

	// 安全组合路径
	if (!PathCombineA(FilePath, curDirPath, PATTERN_FILE)) {
		// 错误处理：路径组合失败
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

	fseek(pFile, 0, SEEK_END);//把文件指针移动到文件尾
	long fileSize = ftell(pFile);//获取文件大小
	char* szMarkCode = new char[fileSize + 2];//分配内存
	memset(szMarkCode, 0, fileSize + 2);
	rewind(pFile);

	//读取文件并显示
	fread(szMarkCode, 1, fileSize, pFile);
	szMarkCode[fileSize] = '\0';
	memcpy(fileBuffer, szMarkCode, fileSize+2);
	delete szMarkCode;
	fclose(pFile);

}
//读取文件内容
std::vector<CString> ReadAllLinesAsCString(const CHAR* filename) {
	std::vector<CString> lines;
	std::ifstream file(filename, std::ios::binary);  // 保持原始参数类型

	if (!file.is_open()) {
		CStringA errMsg;
		errMsg.Format("无法打开文件: %s (错误码: 0x%08X)", filename, GetLastError());
		throw std::runtime_error(errMsg.GetString());
	}

	// 一次性读取全部内容
	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string& content = buffer.str();

	// 高效分割行（自动处理 \r\n 和 \n）
	size_t pos = 0;
	const size_t len = content.length();
	const char* data = content.data();

	while (pos < len) {
		size_t newline_pos = pos;

		// 查找换行符
		while (newline_pos < len &&
			data[newline_pos] != '\r' &&
			data[newline_pos] != '\n') {
			++newline_pos;
		}

		// 直接构造 CString（避免中间 string）
		lines.emplace_back(
			data + pos,                            // 起始指针
			static_cast<int>(newline_pos - pos)    // 字符数量
		);

		// 跳过换行符
		pos = newline_pos;
		if (pos < len) {
			if (data[pos] == '\r') ++pos;
			if (pos < len && data[pos] == '\n') ++pos;
		}
	}

	return lines;
}
//拆分每一行的特征码 逗号隔开
std::vector<CString>  SplitString(const CString& str, TCHAR delimiter)
{
	//根据传进来的delimiter分割字符串 并存储到vector中再返回
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
		//获取 注释 特征码 偏移 读取长度 读取方式(基址 偏移 CALL)
		std::vector<CString> vecMarkCodeLine = SplitString(line, _T(',')); 
		std::string strMarkCode = CStringA(vecMarkCodeLine[2]);
		std::vector<DWORD> dwRetAddr;
		bool isCall = vecMarkCodeLine[5].CompareNoCase(_T("CALL")) == 0;
		bool isBase = vecMarkCodeLine[4].CompareNoCase(_T("基址")) == 0;
		bool isOffset = vecMarkCodeLine[4].CompareNoCase(_T("偏移")) == 0;
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
