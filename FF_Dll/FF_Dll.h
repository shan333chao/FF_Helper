// FF_Dll.h: FF_Dll DLL 的主标头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif
#include "afxdialogex.h"
#include "resource.h"		// 主符号


// CFFDllApp
// 有关此类实现的信息，请参阅 FF_Dll.cpp
//

class CFFDllApp : public CWinApp
{
public:
	CFFDllApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
