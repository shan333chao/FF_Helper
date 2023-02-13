#include "pch.h"
#include "helper.h"
#include <stdio.h>
#include <string>
#include <atlconv.h>
#include <iostream>
#include <stdlib.h>  // for wcstombs
#include <wctype.h>  // for towlower
#include <wchar.h>   // for wchar_t
using namespace std;

string get_last_error(DWORD errCode)
{
	string err("");
	if (errCode == 0) errCode = GetLastError();
	LPTSTR lpBuffer = NULL;
	if (0 == FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, //��־λ���������˵��lpSource������dwFlags�ĵ�λָ����δ����й����������������Ҳ��������ȵĸ�ʽ�������,��ѡ������
		NULL,//����dwFlags��־������
		errCode,//�������Ϣ�ı�ʶ������dwFlags��־ΪFORMAT_MESSAGE_FROM_STRINGʱ�ᱻ���ԡ�
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//�������Ϣ�����Ա�ʶ����
		(LPTSTR)&lpBuffer,//���մ�����Ϣ�����Ļ�����ָ�롣
		0,//���FORMAT_MESSAGE_ALLOCATE_BUFFER��־û�б�ָ���������������ָ��Ϊ����������Ĵ�С�����ָ��ֵΪ0���������ָ��Ϊ������������������С����
		NULL//�����ʽ����Ϣ�еĲ���ֵ��һ�����顣
	))
	{//ʧ��
		char tmp[100] = { 0 };
		sprintf_s(tmp, "{δ�����������(%d)}", errCode);
		err = tmp;
	}
	else//�ɹ�
	{
		USES_CONVERSION;
		err = W2A(lpBuffer); 
		LocalFree(lpBuffer);
	}
	return err;
}


int wcscasecmp(const wchar_t* cs, const wchar_t* ct)
{
	while (towlower(*cs) == towlower(*ct))
	{
		if (*cs == 0)
			return 0;
		cs++;
		ct++;
	}
	return towlower(*cs) - towlower(*ct);
}


string rand_str(int len)  /*����Ϊ�ַ����ĳ���*/
{
	srand(time(0));
	/*��ʼ��*/
	string str;                 /*����������������ַ�����str*/
	char c;                     /*�����ַ�c����������������ɵ��ַ�*/
	int idx;                    /*����ѭ���ı���*/
	/*ѭ�����ַ��������������ɵ��ַ�*/
	for (idx = 0; idx < len; idx++)
	{
		/*rand()%26��ȡ�࣬����Ϊ0~25����'a',������ĸa~z,���asc���*/
		c = rand() % 64 + rand() % 64;
		str.push_back(c);       /*push_back()��string��β�庯���������������ַ�c*/
	}
	return str;                 /*�������ɵ�����ַ���*/
}