#pragma once
#include "player.h"
#include "Package.h"
#include "Surround.h"


// FF_Dlg 对话框

class FF_Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(FF_Dlg)

public:
	FF_Dlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FF_Dlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FF };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	player* player_instance;
	Package* pkg;
	Surround* surround;
	CString window_title;
	DWORD closeToMonster;
	DWORD m_edit_hp;
	DWORD m_edit_mp;
	DWORD m_edit_fp;
	CButton m_btn_is_auto_eat;
	afx_msg void OnBnClickedCheckAutohpmpfp();
	afx_msg void OnBnClickedCheckAutoSkill();
	virtual BOOL OnInitDialog();
	void RefreshPlayInfo();
	CStatic m_static_hp;
	CStatic m_static_mp;
	CStatic m_static_fp;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_edit_logs;
	void AddLog(CString text);
	void EatHpMpFp();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonLoaddll();
	afx_msg void OnBnClickedButtonAtk();

	CButton m_btn_autoskill;
	DWORD select_skill_index;
	afx_msg void OnBnClickedButtonGetitemobj();
	afx_msg void OnBnClickedButtonUse();
	afx_msg void OnBnClickedButtonSurround();
	void CollectMonster();

	CButton m_btn_combineskill;
	CButton m_check_common_atk;
	CSliderCtrl m_slider_view_high;
	afx_msg void OnNMThemeChangedSliderViewHigh(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnBnClickedButtonGm();
	CButton m_btn_auto_sell;
	afx_msg void OnBnClickedCheckAutoSell();
	void FillJobSkillLvl();
	CComboBox m_combox_skills;
	afx_msg void OnBnClickedCheckCollect();

	CButton m_checkbox_collect;
	void GetOldFunAddress();

	afx_msg void OnBnClickedCheckGmonline();
	CButton m_checkbox_gmstatus;
	DWORD m_edit_monster_lvl;
	CButton m_checkbox_nocollisior;
	afx_msg void OnBnClickedCheckNocollisior();
	CButton m_checkbox_surround_ext;
	afx_msg void OnBnClickedCheckSurroundExt();
	afx_msg void OnBnClickedCheckFrameSkill();
	CButton m_checkbox_frameskip;
	afx_msg void OnBnClickedButtonShowinfo();
	CEdit m_edit_item_keywords;
	// 静默模式
	CButton m_checkbox_silent;
	afx_msg void OnBnClickedCheckSilent();
	afx_msg void OnBnClickedButtonAgiAdd();
	afx_msg void OnBnClickedButtonStrAdd();
	afx_msg void OnBnClickedButtonIntAdd();
	afx_msg void OnBnClickedButtonVitAdd();
	CButton m_checkbox_atk;


	afx_msg void OnBnClickedCheckCommonAtk();
	CButton m_checkbox_get;
	afx_msg void OnBnClickedCheckGet();
	// 移动速度修改
	CEdit m_edit_move_speed;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonKaSkill();
	CButton m_checkbox_draw;
	CButton m_checkbox_team_luck;
	CButton m_checkbox_team_work;
	afx_msg void OnBnClickedCheckTeamDraw();
	void AutoTeamSkill();

	afx_msg void OnBnClickedButtonFuben();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonGetKuang();
	void UseWeapon(DWORD useIndex,DWORD packageIndex);
	CButton m_checkbox_caikuang;
	afx_msg void OnBnClickedCheckKuang();
	void SetThreadHook(HANDLE hThread);
	void SetHook();
 
 
	afx_msg void OnBnClickedButtonHook();
	CListCtrl m_ListCtrl_sends;
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnNMDblclkListSends(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonSend();
	CEdit m_edit_buffers;
};
extern FF_Dlg* ff_Dlg;