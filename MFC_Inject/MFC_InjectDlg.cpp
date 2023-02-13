
// MFC_InjectDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFC_Inject.h"
#include "MFC_InjectDlg.h"
#include "afxdialogex.h"
#define INJECT_DLL_NAME "FF_Dll.dll"
#define MAIN_MODULE_NAME _T("Neuz.exe")
#define UI_BASE 0x0084FCEC //A1 ?? ?? ?? ?? 8B 40 ?? 85 C0 0F 84 ?? ?? ?? ?? 8B 91 ?? ?? ?? ?? F6 42 ?? 08 
#define UI_OBJECTS 0xc0
#define UI_WATER 0xc8
#define UI_TERRAIN 0xbc
#define UI_WEATHER 0xb8
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCInjectDlg 对话框



CMFCInjectDlg::CMFCInjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_INJECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCInjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_list_process);
}

BEGIN_MESSAGE_MAP(CMFCInjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &CMFCInjectDlg::OnBnClickedButtonReload)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESS, &CMFCInjectDlg::OnNMDblclkListProcess)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CMFCInjectDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_SILENT, &CMFCInjectDlg::OnBnClickedButtonSilent)
 
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_ALL, &CMFCInjectDlg::OnBnClickedButtonCloseAll)
	ON_BN_CLICKED(IDC_BUTTON_MINWND, &CMFCInjectDlg::OnBnClickedButtonMinwnd)
END_MESSAGE_MAP()


// CMFCInjectDlg 消息处理程序

BOOL CMFCInjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);
	m_list_process.InsertColumn(0, _T("PID"), LVCFMT_LEFT, 100);
	m_list_process.InsertColumn(1, _T("进程名"), LVCFMT_LEFT, 90);
	m_list_process.InsertColumn(2, _T("窗口名"), LVCFMT_LEFT, 190);
	m_list_process.InsertColumn(3, _T("状态"), LVCFMT_LEFT, 90);
	m_list_process.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// TODO: 在此添加额外的初始化代码
	SetTimer(1, 500, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCInjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCInjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void doInject(DWORD pid) {
	CHAR pzTarget[MAX_PATH] = { 0 };
	GetRunningFolderFile((PCHAR)INJECT_DLL_NAME, pzTarget);
	DWORD  exitAddr = 0;
	USES_CONVERSION;
	BOOL isok = RemoteThreadInject(pid, pzTarget, &exitAddr);
}

typedef struct _INJECTTARGET
{
	DWORD pid;
	char name[0x256];
	char title[0x256];
	BOOL status;
}INJECTTARGET, * PINJECTTARGET;

INJECTTARGET targets[20] = { 0 };
DWORD targetCount = 0;
clock_t start = clock();
CString newTitle;
int str_start = 0;
int str_end = 0;
int found = 0;
void GetAllTargets(PINJECTTARGET targets) {
	double counter = ((double)(clock() - start)) / CLOCKS_PER_SEC;
	if (counter <= 1)
	{
		return;
	}
	start = clock();
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		::MessageBox(0, (LPCWSTR)get_last_error(0).c_str(), 0, 0);
		return;
	}

	USES_CONVERSION;
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnap, &pe32))
	{
		int i = 0;
		do
		{
			str_start = 0;
			str_end = 0;
			found = 0;
			if (!wcscmp(MAIN_MODULE_NAME, pe32.szExeFile))
			{
				HWND hgameWin = GetWindowHwndByPID(pe32.th32ProcessID, (PCHAR)"D3D Window");
				char titleText[MAX_PATH] = { 0 };
				if (hgameWin != NULL)
				{
					::GetWindowTextA(hgameWin, titleText, MAX_PATH);
					newTitle = A2W(titleText);
					found = newTitle.Find(L"客户端", 0);
					str_start = newTitle.Find(L"[", 0);
					str_end = newTitle.Find(L")", 0);
					if (found > 1 && str_start > 1 && str_end > 0)
					{
						newTitle = newTitle.Mid(str_start + 1, str_end - str_start);
						::SetWindowTextW(hgameWin, newTitle);
					}
				}
				DWORD outAddr{};
				BOOL status = ModuleExist(pe32.th32ProcessID, (PWCHAR)_T(INJECT_DLL_NAME), &outAddr);
				USES_CONVERSION;
				INJECTTARGET target = { 0 };
				target.pid = pe32.th32ProcessID;
				target.status = status;
				strcpy_s(target.name, W2A(pe32.szExeFile));
				strcpy_s(target.title, titleText);
				//memcpy(target.title, titleText, sizeof(titleText));

				targets[i] = target;

				i++;
			}
		} while (Process32Next(hSnap, &pe32));
		targetCount = i;
	}
	CloseHandle(hSnap);
}

void CMFCInjectDlg::RefreshProcessList()
{
	GetAllTargets(targets);
	CString format;
	USES_CONVERSION;
	for (size_t i = 0; i < targetCount; i++)
	{

		format.Format(_T("%d [%x]"), targets[i].pid, targets[i].pid);
		m_list_process.InsertItem(LVIF_TEXT | LVIF_STATE, i, format, 0, 0, 0, 1);
		format.Format(_T("%s"), A2W(targets[i].name));
		m_list_process.SetItemText(i, 1, format);
		format.Format(_T("%s"), A2W(targets[i].title));
		m_list_process.SetItemText(i, 2, format);
		format.Format(_T("%s"), targets[i].status ? L"已启动" : L"未启动");
		m_list_process.SetItemText(i, 3, format);
	}

}



void CMFCInjectDlg::OnBnClickedButtonReload()
{
	m_list_process.DeleteAllItems();
	RefreshProcessList();
}


void CMFCInjectDlg::OnNMDblclkListProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nIndex = m_list_process.GetSelectionMark();   //获取选中行的行号

	CString status = m_list_process.GetItemText(nIndex, 3);
	if (!wcscmp(L"已启动", status))
	{
		return;
	}
	// TODO: 在此添加控件通知处理程序代码

	CString pidText = m_list_process.GetItemText(nIndex, 0);   //获取第0列内容
	int pid = _ttoi(pidText);
	doInject(pid);
	m_list_process.DeleteAllItems();
	RefreshProcessList();
	*pResult = 0;
}


void CMFCInjectDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);

	switch (nIDEvent)
	{

	case 1: {
		OnBnClickedButtonReload();
		break;
	};
	}

}


void CMFCInjectDlg::OnBnClickedButtonExit()
{
	exit(0);
}

DWORD OPEN_UI = 0x00000001;
void CMFCInjectDlg::OnBnClickedButtonSilent()
{

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		::MessageBox(0, (LPCWSTR)get_last_error(0).c_str(), 0, 0);
		return;
	}
	OPEN_UI = !OPEN_UI;
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnap, &pe32))
	{
		int i = 0;
		do
		{
			if (!wcscmp(MAIN_MODULE_NAME, pe32.szExeFile))
			{

				DWORD outAddr{};
				BOOL status = ModuleExist(pe32.th32ProcessID, (PWCHAR)MAIN_MODULE_NAME, &outAddr);
				if (status)
				{
					DWORD addr = 0;
					DWORD NumberOfBytesRead = 0;
					ReadRemoteProcessMemory(pe32.th32ProcessID, (LPVOID)(outAddr + UI_BASE), &addr, sizeof(DWORD), &NumberOfBytesRead);

					WriteRemoteProcessMemory(pe32.th32ProcessID, (LPVOID)(addr + UI_OBJECTS), &OPEN_UI, sizeof(DWORD), &NumberOfBytesRead);
					WriteRemoteProcessMemory(pe32.th32ProcessID, (LPVOID)(addr + UI_WATER), &OPEN_UI, sizeof(DWORD), &NumberOfBytesRead);
					WriteRemoteProcessMemory(pe32.th32ProcessID, (LPVOID)(addr + UI_TERRAIN), &OPEN_UI, sizeof(DWORD), &NumberOfBytesRead);
					WriteRemoteProcessMemory(pe32.th32ProcessID, (LPVOID)(addr + UI_WEATHER), &OPEN_UI, sizeof(DWORD), &NumberOfBytesRead);
				}


				i++;
			}
		} while (Process32Next(hSnap, &pe32));
		targetCount = i;
	}
	CloseHandle(hSnap);
}


 


void CMFCInjectDlg::OnBnClickedButtonCloseAll()
{
	KillProcessByName(MAIN_MODULE_NAME);
}


void CMFCInjectDlg::OnBnClickedButtonMinwnd()
{
	// TODO: 在此添加控件通知处理程序代码
}
