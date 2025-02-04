#pragma once
#include <map>
#include <string>
class PatternSingleton
{
public:
    ~PatternSingleton();
    DWORD GetAddressByKey(std::string key);
    static PatternSingleton& getInstance();
    std::map<std::string, DWORD> GetAllSignatureValue();

private:
    std::map<std::string, DWORD> mapSignatureValue;
    bool IsPatternFileExist(PCHAR FilePath);
    std::vector<CString> ReadAllLinesAsCString(const CHAR* filename);
    std::vector<CString> SplitString(const CString& str, TCHAR delimiter);
    void InitSignatureDataAddr();
    PatternSingleton();
};

