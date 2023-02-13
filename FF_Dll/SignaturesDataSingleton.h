#pragma once
#include <map>
#include <string>
class SignaturesDataSingleton
{
public:
    ~SignaturesDataSingleton();
    DWORD GetAddressByKey(std::string key);
    static SignaturesDataSingleton& getInstance();
    std::map<std::string, DWORD> GetAllSignatureValue();

private:
    std::map<std::string, std::string> mapSignatureDesc;
    std::map<std::string, DWORD> mapSignatureValue;
    void InitSignatureDataAddr();
    SignaturesDataSingleton();
};

