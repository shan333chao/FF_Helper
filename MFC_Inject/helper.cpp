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
	if (0 == FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, //标志位，决定如何说明lpSource参数，dwFlags的低位指定如何处理换行功能在输出缓冲区，也决定最大宽度的格式化输出行,可选参数。
		NULL,//根据dwFlags标志而定。
		errCode,//请求的消息的标识符。当dwFlags标志为FORMAT_MESSAGE_FROM_STRING时会被忽略。
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//请求的消息的语言标识符。
		(LPTSTR)&lpBuffer,//接收错误信息描述的缓冲区指针。
		0,//如果FORMAT_MESSAGE_ALLOCATE_BUFFER标志没有被指定，这个参数必须指定为输出缓冲区的大小，如果指定值为0，这个参数指定为分配给输出缓冲区的最小数。
		NULL//保存格式化信息中的插入值的一个数组。
	))
	{//失败
		char tmp[100] = { 0 };
		sprintf_s(tmp, "{未定义错误描述(%d)}", errCode);
		err = tmp;
	}
	else//成功
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


string rand_str(int len)  /*参数为字符串的长度*/
{
	srand(time(0));
	/*初始化*/
	string str;                 /*声明用来保存随机字符串的str*/
	char c;                     /*声明字符c，用来保存随机生成的字符*/
	int idx;                    /*用来循环的变量*/
	/*循环向字符串中添加随机生成的字符*/
	for (idx = 0; idx < len; idx++)
	{
		/*rand()%26是取余，余数为0~25加上'a',就是字母a~z,详见asc码表*/
		c = rand() % 64 + rand() % 64;
		str.push_back(c);       /*push_back()是string类尾插函数。这里插入随机字符c*/
	}
	return str;                 /*返回生成的随机字符串*/
}