#pragma once
#include <string>
#include <vector>
//计划加入几种不同的搜索模式，为了方便管理所以定义了一个特征码搜索的类
class CFeatureCode
{
public:
 
	~CFeatureCode(void);
    DWORD m_TargetModuleBase;
    DWORD m_RAVModuleBase;
    DWORD m_RAVModuleSize;
    CFeatureCode(LPCWSTR lpModuleName) {
        this->m_TargetModuleBase = (DWORD)GetModuleHandle(lpModuleName);
        this->m_RAVModuleBase = 0x0;
        this->m_RAVModuleSize = 0x0;
    }
	void Init();
    //Sunday算法搜索 网上抄来的 感谢网友  之所以定义一个类是因为还想加入其他的扫描算法。。。
    //2020.8.9代码更新，修复通配符的小BUG，该BUG会有极低几率搜索错误，现已修复
    //2022.4.4代码更新，添加一句代码，修复起始地址的小BUG，该BUG会有小概率搜不到想要的内存

    /*
    FindMatchingCode() 参数说明：
    1) hProcess		要打开的进程句柄
    2) markCode		特征码,支持通配符（??），如: 56 8B 01 FF 90 ?? ?? ?? ?? 85 C0 74
    3) memBeginAddr		起始搜索地址
    4) memEndAddr		结束搜索地址
    5) retAddr[]		记录找到的地址,传入这个参数前一定要清0，如 DWORD retAddr[32] = {0};  或者 DWORD *retAddr = new DWORD[32]();
    6) deviation		特征码地址离目标地址的偏移距离，上负下正
    7) isCall		是否为找CALL的跳转地址，true 则 retAddr[] 返回的是CALL跳转的地址
    8) isAll		是否查找所有符合的地址，false找到第一个符合的地址后就结束搜索，true继续搜索，直到搜索地址大于结束地址（memEndAddr）
    return返回值		找到的地址总数
    搜不到内存可能是保护属性没有选对
    */
    DWORD FindMatchingCode(HANDLE hProcess, std::string strMarkCode, DWORD dwMemBeginAddr, DWORD dwMemEndAddr, DWORD dwRetAddr[], int nDeviation, bool bIsCall, bool bIsAll = false);

    DWORD FindPatternCode(std::string strMarkCode, DWORD dwRetAddr[], int nDeviation, bool bIsCall, bool bIsAll = false);
    /// <summary>
    /// 初始化搜索范围
    /// </summary>
    /// <returns></returns>
    DWORD InitModuleTextSize();

    DWORD SearchPattern(std::string markCode, DWORD memBeginAddr, DWORD memEndAddr, std::vector<DWORD>& retAddr, int deviation, bool isCall, bool isAll);

    DWORD SearchPatternE(std::string markCode, std::vector<DWORD>& retAddr, int deviation, bool isCall, bool isAll);

 

};

