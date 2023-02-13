
// MFC_InjectDlg.h: 头文件
//

#pragma once


// CMFCInjectDlg 对话框
class CMFCInjectDlg : public CDialogEx
{
// 构造
public:
	CMFCInjectDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_INJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void RefreshProcessList();
public:
	CListCtrl m_list_process;
	afx_msg void OnBnClickedButtonReload();
	afx_msg void OnNMDblclkListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonSilent();
 
	afx_msg void OnBnClickedButtonCloseAll();
	afx_msg void OnBnClickedButtonMinwnd();
};
