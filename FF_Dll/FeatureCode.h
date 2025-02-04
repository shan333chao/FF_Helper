#pragma once
#include <string>
#include <vector>
//�ƻ����뼸�ֲ�ͬ������ģʽ��Ϊ�˷���������Զ�����һ����������������
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
    DWORD FindMatchingCode(HANDLE hProcess, std::string strMarkCode, DWORD dwMemBeginAddr, DWORD dwMemEndAddr, DWORD dwRetAddr[], int nDeviation, bool bIsCall, bool bIsAll = false);

    DWORD FindPatternCode(std::string strMarkCode, DWORD dwRetAddr[], int nDeviation, bool bIsCall, bool bIsAll = false);
    /// <summary>
    /// ��ʼ��������Χ
    /// </summary>
    /// <returns></returns>
    DWORD InitModuleTextSize();

    DWORD SearchPattern(std::string markCode, DWORD memBeginAddr, DWORD memEndAddr, std::vector<DWORD>& retAddr, int deviation, bool isCall, bool isAll);

    DWORD SearchPatternE(std::string markCode, std::vector<DWORD>& retAddr, int deviation, bool isCall, bool isAll);

 

};

