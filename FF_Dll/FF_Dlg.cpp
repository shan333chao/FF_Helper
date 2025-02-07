﻿// FF_Dlg.cpp: 实现文件
//

#include "pch.h"
#include "FF_Dll.h"
#include "FF_Dlg.h"
#include "afxdialogex.h"
#include "TIMER_EVENT.h"
#include <TlHelp32.h>
#include <thread>
#include "ff_offset.h"
#include <vector>
#include <map>

#include "CHookUtil.h"
#include <string>
#pragma comment(lib, "Lib/BeaEngineHook.lib")
#define WM_UPDATEDATA 0x70029
// FF_Dlg 对话框

IMPLEMENT_DYNAMIC(FF_Dlg, CDialogEx)
std::vector<std::pair<CString, DWORD> > warrior_v;
std::vector<std::pair<CString, DWORD> > xinshou_v;
std::vector<std::pair<CString, DWORD> > magic_v;
std::vector<std::pair<CString, DWORD> > doctor_v;
std::map<DWORD, CString> equipments;
DWORD m_s_iSpeedTimes;
std::map<DWORD, std::vector<std::pair<CString, DWORD>>> jobMap;
std::map<DWORD, CString> jobNameMap;
std::map<DWORD, CString> fubenPickUpMap;
BOOL checkGM = false;
float pre_multiSpeed = 1;
BOOL first_GetMessageTimed = FALSE;
BOOL first_timeGetTimed = FALSE;
BOOL first_GetTickCounted = FALSE;
BOOL first_QueryPerformanceCountered = FALSE;
int cannot_exit = 0;
DWORD First_GetMessageTime = 0;
DWORD Pre_GetMessageTime = 0;
DWORD New_First_GetMessageTime = 0;
DWORD New_Pre_GetMessageTime = 0;
BOOL Has_Pre_GetMessageTime = FALSE;
DWORD First_timeGetTime = 0;
DWORD Pre_timeGetTime = 0;
DWORD New_First_timeGetTime = 0;
DWORD New_Pre_timeGetTime = 0;
BOOL Has_Pre_timeGetTime = FALSE;
DWORD First_GetTickCount = 0;
DWORD Pre_GetTickCount = 0;
DWORD New_First_GetTickCount = 0;
DWORD New_Pre_GetTickCount = 0;
BOOL Has_Pre_GetTickCount = FALSE;
LARGE_INTEGER First_QueryPerformanceCounter;
LARGE_INTEGER Pre_QueryPerformanceCounter;
LARGE_INTEGER New_First_QueryPerformanceCounter;
LARGE_INTEGER New_Pre_QueryPerformanceCounter;
BOOL Has_Pre_QueryPerformanceCounter = FALSE;

//----------特征码变量区-----------
DWORD dw_SURRDOUND_BASE = SURRDOUND_BASE;
DWORD dw_PICK_TARGET_BASE = 0;
DWORD dw_SURROUND_TMP_1 = 0;
DWORD dw_SURROUND_TMP_2 = 0;
DWORD dw_ACTION_PARAM1 = 0;
DWORD dw_ACTION_CALL = 0;

DWORD dw_SKILL_PARAM1 = 0;
DWORD dw_SKILL_CALL = 0;

DWORD dw_NO_COLLISIOR = 0;
DWORD dw_TEAM_BUFF_BASE = 0;

DWORD dw_TEAM_POINT = 0;
DWORD dw_TEAM_LEVEL = 0;

DWORD dw_SKILL_REMOTE_PARAM1 = 0;
DWORD dw_SKILL_REMOTE_CALL = 0;
DWORD dw_SOCKET_SEND_PARAM1 = 0;
DWORD dw_SOCKET_SEND_CALL = 0;


DWORD dw_HOOK_WSASEND = HOOK_WSASEND;
DWORD dw_TEAM_SKILL_PARAM1 = 0;
DWORD dw_FLY_ATK_CALL = 0;
CHookUtil inlinehook;
CWinThread* pthread;
DWORD pindex;
DWORD send_control;
DWORD filter_len = 0;
DWORD methodAddrBase = dw_SOCKET_SEND_CALL;
DWORD content;
DWORD len;
DWORD flags;
DWORD callAddr;
DWORD go_on1 = methodAddrBase + 0x1F;
DWORD go_on2 = methodAddrBase + 0x6;
FF_Dlg::FF_Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FF, pParent)

	, closeToMonster(0)
	, m_edit_logs(_T(""))
	, select_skill_index(9)
	, window_title(_T(""))

	, m_edit_monster_lvl(0)
{
	this->player_instance = new player();
	this->player_instance->init();
	this->pkg = new Package();
	this->surround = new Surround();
	this->FillJobSkillLvl();

	m_s_iSpeedTimes = 0;


}

FF_Dlg::~FF_Dlg()
{

}



DWORD WINAPI ThreadDetectGM(LPVOID lpThreadParameter) {

	Surround surround;
	DWORD maxMember = 0;
	DWORD assertTmp0 = 0;
	DWORD assertTmp1 = 0;
	DWORD assertTmp2 = 0;
	DWORD assertTmp3 = 0;
	DWORD assertTmp4 = 0;
	CString info;
	USES_CONVERSION;
	DWORD gm = 0;
	DWORD member = 0;
	CString tmp;
	PCHAR name = NULL;
	DWORD local_dw_ACTION_PARAM1 = dw_ACTION_PARAM1;
	DWORD local_dw_ACTION_CALL = dw_ACTION_CALL;
	while (true)
	{
		if (checkGM)
		{
			maxMember = surround.GetMaxMembers();

			for (size_t i = 0; i < maxMember; i++)
			{
				assertTmp0 = *((LPDWORD)(dw_PICK_TARGET_BASE));
				member = *((LPDWORD)(i * 4 + dw_SURRDOUND_BASE));
				if (member == 0 || member == -1)
				{
					continue;
				}
				assertTmp1 = *((LPDWORD)(assertTmp0 + 0xCC));
				if (assertTmp1 == 0)
				{
					continue;
				}
				assertTmp2 = *((LPDWORD)(member + 0x170));
				if (assertTmp2 != 0x5)
				{
					continue;
				}
				assertTmp3 = *((LPDWORD)(dw_SURROUND_TMP_1));//39 3D ?? ?? ?? ?? 75 0C 39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF 
				assertTmp4 = *((LPDWORD)(dw_SURROUND_TMP_2));//39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF D9 05 ?? ?? ?? ?? D9 5D E0 
				if (assertTmp3 == member && assertTmp4 == 0)
				{
					continue;
				}
				gm = *((LPDWORD)(member + PLAY_OFFSET_GM));


				if (gm == 90)
				{
					//回城call
					//004FDC57 | 8B81 EC000000 | mov eax, dword ptr ds : [ecx + 0xEC] | eax来源 239293B0
					//004FDC5D | 85C0 | test eax, eax |
					//004FDC5F | 74 10 | je 0x4FDC71 |
					//004FDC61 | 6A 00 | push 0x0 |
					//004FDC63 | FF30 | push dword ptr ds : [eax] |  动作技能索引
					//004FDC65 | B9 C84C9600 | mov ecx, neuz.964CC8 | 964CC8 : "T﹛" == &"i H"
					//004FDC6A | 6A 03 | push 0x3 |
					//004FDC6C | E8 1444F8FF | call 0x482085 | 回城call
					__asm {
						pushad
						pushfd
						push 0x0
						push 0x19
						mov ecx, local_dw_ACTION_PARAM1  //B9 ?? ?? ?? ?? 6A 03 E8 ?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ?? 
						push 0x3
						mov eax, local_dw_ACTION_CALL //E8 ?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ?? B9 ?? ?? ?? ?? 6A 0A 
						call eax
						popfd
						popad
					}
					MessageBox(0, L"发现gm", L"发现gm", 0);
					//ExitProcess(998);
				}
			}
		}
		Sleep(1000);
	}
	return 0;
}
void FF_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Hp_Per, m_edit_hp);
	DDX_Text(pDX, IDC_EDIT_Mp_Per, m_edit_mp);
	DDX_Text(pDX, IDC_EDIT_Fp_per, m_edit_fp);
	DDX_Control(pDX, IDC_CHECK_AutoHpMpFp, m_btn_is_auto_eat);
	DDX_Control(pDX, IDC_STATIC_HP_VAL, m_static_hp);
	DDX_Control(pDX, IDC_STATIC_MP_VAL, m_static_mp);
	DDX_Control(pDX, IDC_STATIC_FP_VAL, m_static_fp);
	DDX_Text(pDX, IDC_EDIT_LOGS, m_edit_logs);

	DDX_Control(pDX, IDC_CHECK_Auto_Skill, m_btn_autoskill);
	DDX_Control(pDX, IDC_CHECK_CombineSkill, m_btn_combineskill);
	DDX_Control(pDX, IDC_CHECK_COMMON_ATK, m_check_common_atk);
	DDX_Control(pDX, IDC_CHECK_AUTO_SELL, m_btn_auto_sell);
	DDX_Control(pDX, IDC_COMBO_SKILLS, m_combox_skills);

	DDX_Control(pDX, IDC_CHECK_GMONLINE, m_checkbox_gmstatus);
	DDX_Text(pDX, IDC_EDIT_MONSTER_LVL, m_edit_monster_lvl);
	DDX_Control(pDX, IDC_CHECK_NoCollisior, m_checkbox_nocollisior);
	DDX_Control(pDX, IDC_CHECK_SURROUND_EXT, m_checkbox_surround_ext);
	DDX_Control(pDX, IDC_CHECK_FRAME_SKILL, m_checkbox_frameskip);
	DDX_Control(pDX, IDC_EDIT_PICK_KEYWORD, m_edit_item_keywords);
	DDX_Control(pDX, IDC_CHECK_SILENT, m_checkbox_silent);
	DDX_Control(pDX, IDC_CHECK_ATK, m_checkbox_atk);

	DDX_Control(pDX, IDC_CHECK_GET, m_checkbox_get);
	DDX_Control(pDX, IDC_EDIT_SPEED, m_edit_move_speed);
	DDX_Control(pDX, IDC_CHECK_TEAM_DRAW, m_checkbox_draw);
	DDX_Control(pDX, IDC_CHECK_TEAM_LUCKY, m_checkbox_team_luck);
	DDX_Control(pDX, IDC_CHECK_TEAMWORK, m_checkbox_team_work);
	DDX_Control(pDX, IDC_CHECK_KUANG, m_checkbox_caikuang);
	DDX_Control(pDX, IDC_LIST_Sends, m_ListCtrl_sends);
	DDX_Control(pDX, IDC_EDIT_Buffers, m_edit_buffers);
	DDX_Control(pDX, IDC_CHECK_HOOK_ATK, m_checkbox_hook_atk);
	DDX_Control(pDX, IDC_EDIT_Len, m_edit_filter_len);
	DDX_Control(pDX, IDC_CHECK_OPEN_START, m_checkbox_start_stop);
	DDX_Control(pDX, IDC_CHECK_FLY_ATK, m_checkbox_flypick);
}


BEGIN_MESSAGE_MAP(FF_Dlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_AutoHpMpFp, &FF_Dlg::OnBnClickedCheckAutohpmpfp)
	ON_BN_CLICKED(IDC_CHECK_Auto_Skill, &FF_Dlg::OnBnClickedCheckAutoSkill)
	ON_WM_TIMER()
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_BUTTON_ATK, &FF_Dlg::OnBnClickedButtonAtk)
	ON_BN_CLICKED(IDC_BUTTON_GetItemObj, &FF_Dlg::OnBnClickedButtonGetitemobj)

	ON_BN_CLICKED(IDC_BUTTON_Surround, &FF_Dlg::OnBnClickedButtonSurround)

	ON_BN_CLICKED(IDC_BUTTON_GM, &FF_Dlg::OnBnClickedButtonGm)
	ON_BN_CLICKED(IDC_CHECK_AUTO_SELL, &FF_Dlg::OnBnClickedCheckAutoSell)


	ON_BN_CLICKED(IDC_CHECK_GMONLINE, &FF_Dlg::OnBnClickedCheckGmonline)
	ON_BN_CLICKED(IDC_CHECK_NoCollisior, &FF_Dlg::OnBnClickedCheckNocollisior)
	ON_BN_CLICKED(IDC_CHECK_SURROUND_EXT, &FF_Dlg::OnBnClickedCheckSurroundExt)
	ON_BN_CLICKED(IDC_CHECK_FRAME_SKILL, &FF_Dlg::OnBnClickedCheckFrameSkill)
	ON_BN_CLICKED(IDC_BUTTON_ShowInfo, &FF_Dlg::OnBnClickedButtonShowinfo)
	ON_BN_CLICKED(IDC_CHECK_SILENT, &FF_Dlg::OnBnClickedCheckSilent)



	ON_BN_CLICKED(IDC_CHECK_COMMON_ATK, &FF_Dlg::OnBnClickedCheckCommonAtk)
	ON_BN_CLICKED(IDC_CHECK_GET, &FF_Dlg::OnBnClickedCheckGet)

	ON_BN_CLICKED(IDC_BUTTON_KA_SKILL, &FF_Dlg::OnBnClickedButtonKaSkill)
	ON_BN_CLICKED(IDC_CHECK_TEAM_DRAW, &FF_Dlg::OnBnClickedCheckTeamDraw)
	ON_BN_CLICKED(IDC_CHECK_TEAM_LUCKY, &FF_Dlg::OnBnClickedCheckTeamDraw)
	ON_BN_CLICKED(IDC_CHECK_TEAMWORK, &FF_Dlg::OnBnClickedCheckTeamDraw)
	ON_BN_CLICKED(IDC_BUTTON_FUBEN, &FF_Dlg::OnBnClickedButtonFuben)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &FF_Dlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_GET_KUANG, &FF_Dlg::OnBnClickedButtonGetKuang)
	ON_BN_CLICKED(IDC_CHECK_KUANG, &FF_Dlg::OnBnClickedCheckKuang)
	ON_BN_CLICKED(IDC_BUTTON_HOOK, &FF_Dlg::OnBnClickedButtonHook)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &FF_Dlg::OnBnClickedButtonClear)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_Sends, &FF_Dlg::OnNMDblclkListSends)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &FF_Dlg::OnBnClickedButtonSend)
	//	ON_EN_CHANGE(IDC_EDIT_Buffers, &FF_Dlg::OnEnChangeEditBuffers)
	ON_BN_CLICKED(IDC_BUTTON_FILTER, &FF_Dlg::OnBnClickedButtonFilter)
	ON_BN_CLICKED(IDC_CHECK_OPEN_START, &FF_Dlg::OnBnClickedCheckOpenStart)
	ON_BN_CLICKED(IDC_BUTTON_UNHOOK, &FF_Dlg::OnBnClickedButtonUnhook)

	ON_BN_CLICKED(IDC_BUTTON_Collect_MONSTER, &FF_Dlg::OnBnClickedButtonCollectMonster)
END_MESSAGE_MAP()



/// <summary>
/// 自动补充气血
/// </summary>
void FF_Dlg::OnBnClickedCheckAutohpmpfp()
{
	UpdateData(TRUE);

	if (this->m_btn_is_auto_eat.GetCheck() == BST_CHECKED)
	{

		AddLog(L"开启自动补血");
		SetTimer(1, 300, NULL);
	}
	else {
		AddLog(L"停止自动补血");
		KillTimer(1);
	}
	UpdateData(false);

}
void FF_Dlg::AddLog(CString text)
{

	m_edit_logs.Append(text);
	m_edit_logs.Append(_T("\r\n"));
	this->GetDlgItem(IDC_EDIT_LOGS)->PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);
	UpdateData(FALSE);
}

DWORD lastHP = 1;
DWORD lastMP = 1;
DWORD lastFP = 1;
void FF_Dlg::EatHpMpFp()
{
	if (!this->player_instance->init())
	{
		return;
	}
	if (this->m_edit_hp > this->player_instance->GetHp() && lastHP != this->player_instance->GetHp())
	{
		lastHP = this->player_instance->GetHp();
		this->pkg->QuickUse(0);
	}

	if (this->m_edit_mp > this->player_instance->GetMp() && this->player_instance->GetMp() != lastMP)
	{
		lastMP = this->player_instance->GetMp();
		this->pkg->QuickUse(1);

	}

	if (this->m_edit_fp > this->player_instance->GetFp() && lastFP != this->player_instance->GetFp())
	{
		lastFP = this->player_instance->GetFp();
		this->pkg->QuickUse(2);

	}


}
/// <summary>
/// 自动补篮
/// </summary>

void FF_Dlg::OnBnClickedCheckAutoSkill()
{

	UpdateData(TRUE);
	if (this->m_btn_autoskill.GetCheck() == BST_CHECKED)
	{
		AddLog(L"开启自动攻击");

		this->closeToMonster = 0;
		SetTimer(AUTO_ATK, 350, NULL);
	}
	else {
		AddLog(L"停止自动攻击");
		KillTimer(AUTO_ATK);
		this->closeToMonster = 0;
	}
	UpdateData(FALSE);

}



void FF_Dlg::RefreshPlayInfo()
{
	if (!this->player_instance->init())
	{
		return;
	}
	this->window_title.Format(L"%d", this->player_instance->GetHp());
	this->m_static_hp.SetWindowTextW(this->window_title);
	this->window_title.Format(L"%d", this->player_instance->GetMp());
	this->m_static_mp.SetWindowTextW(this->window_title);
	this->window_title.Format(L"%d", this->player_instance->GetFp());
	this->m_static_fp.SetWindowTextW(this->window_title);
	this->player_instance->UpdatePos();
	this->window_title.Format(L"%s(%d) [%s]   Lvl：%d  坐标：(%.1f ,%.1f, %.1f)", this->player_instance->GetPlayName(), this->player_instance->GetId(), jobNameMap.at(this->player_instance->GetJobType()), this->player_instance->GetLvl(), this->player_instance->GetPosX(), this->player_instance->GetPosZ(), this->player_instance->GetPosY());
	this->SetWindowTextW(this->window_title);

}





void FF_Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!this->player_instance->init())
	{
		return;
	}
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		//自动吃血
	case AUTO_HPMPFP: {

		EatHpMpFp();
		break;
	};
					//自动攻击
	case AUTO_ATK: {
		OnBnClickedButtonAtk();
		break;
	};

	case AUTO_SELL: {

		OnBnClickedButtonGetitemobj();
		break;
	}

	case AUTO_COLLECT_MONSTER: {


		break;
	}
	case AUTO_REFRESH_INFO: {

		RefreshPlayInfo();
		break;
	}
	case AUTO_EARCH_PICK: {

		OnBnClickedButtonShowinfo();
		break;
	}
	case AUTO_TEAM_SKILL: {
		AutoTeamSkill();
		break;
	}
	case AUTO_CAI_KUANG: {
		OnBnClickedButtonGetKuang();
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void FF_Dlg::OnClose()
{


	this, m_checkbox_gmstatus.SetCheck(0x0);
	OnBnClickedButtonUnhook();
	OnBnClickedCheckGmonline();
	CDialogEx::OnClose();
}



void FF_Dlg::OnBnClickedButtonAtk()
{
	if (!this->player_instance->init())
	{
		return;
	}
	DWORD picked = this->surround->GetPicked();
	DWORD targetMonster = 0;
	if (this->m_check_common_atk.GetCheck() != BST_CHECKED && picked <= 0)
	{

		targetMonster = this->surround->GetCloseToMonster(this->m_edit_monster_lvl);
		DWORD baseSurroundExt = *(PDWORD)(dw_PICK_TARGET_BASE);
		PSHORT dw_LOD_OBJECT = (PSHORT)(baseSurroundExt + PICK_TARGET_SURROUND);
		if (targetMonster == 0)
		{
			*dw_LOD_OBJECT = BST_UNCHECKED;
			return;
		}
		*dw_LOD_OBJECT = BST_CHECKED;
		if (this->closeToMonster <= 0 || (this->closeToMonster > 0 && *((LPDWORD)(this->closeToMonster)) == 0) || targetMonster != this->closeToMonster)
		{
			DWORD currentMember = *((LPDWORD)(targetMonster));
			if (currentMember <= 0 || this->closeToMonster == targetMonster)
			{
				return;
			}
			DWORD hp = *((LPDWORD)(targetMonster + PLAY_OFFSET_HP));
			if (hp == 0)
			{
				this->closeToMonster = 0;
				return;
			}
			if (this->m_checkbox_flypick.GetCheck() == BST_CHECKED)
			{
				picked = this->surround->PickMonsterFly(targetMonster);
			}
			else {
				picked = this->surround->PickMonster(targetMonster);
			}
			this->closeToMonster = targetMonster;
		}
		DWORD hp = *((LPDWORD)(this->closeToMonster + PLAY_OFFSET_HP));
		if (hp == 0)
		{
			this->closeToMonster = 0;
			return;
		}

	}
	picked = this->surround->GetPicked();
	if (picked <= 0 || picked == this->player_instance->PlayBase)
	{
		return;
	}

	//if (this->player_instance->GetFp() < this->m_edit_fp)
	//{
	//	return;
	//}



	if (this->m_checkbox_atk.GetCheck() == BST_CHECKED)
	{

		if (m_checkbox_hook_atk.GetCheck() == BST_CHECKED && picked > 0) {

			DWORD monster_real = *(PDWORD)(picked + 0x2f0);
			DWORD local_dw_TEAM_SKILL_PARAM1 = dw_TEAM_SKILL_PARAM1;
			DWORD local_dw_FLY_ATK_CALL = dw_FLY_ATK_CALL;
			__asm {
				pushad
				pushfd
				push 0
				push 0
				push monster_real
				mov ecx, local_dw_TEAM_SKILL_PARAM1
				push 0x1d
				mov eax, local_dw_FLY_ATK_CALL
				call eax
				popfd
				popad
			}

		}
		else {
			DWORD local_dw_ACTION_PARAM1 = dw_ACTION_PARAM1;
			DWORD local_dw_ACTION_CALL = dw_ACTION_CALL;
			//动作技能实现普工
			__asm {
				pushad
				pushfd
				push 0x0
				push 0x1b
				mov ecx, local_dw_ACTION_PARAM1  //B9 ?? ?? ?? ?? 6A 03 E8 ?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ?? 
				push 0x3
				mov eax, local_dw_ACTION_CALL //E8 ?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ?? B9 ?? ?? ?? ?? 6A 0A 
				call eax
				popfd
				popad
			}

		}



		return;
	}


	if (this->m_btn_combineskill.GetCheck() == BST_CHECKED)
	{

		this->pkg->QuickUse(4);
	}
	else {

		DWORD skill_index = this->m_combox_skills.GetCurSel();
		if (skill_index == -1)
		{

			AddLog(L"请选择一个技能 或勾选普工");
			this->m_btn_autoskill.SetCheck(BST_UNCHECKED);
			OnBnClickedCheckAutoSkill();
			return;
		}


		if (m_checkbox_hook_atk.GetCheck() == BST_CHECKED && picked > 0)
		{
			DWORD monster_real = *(PDWORD)(picked + 0x2f0);
			DWORD local_dw_SKILL_REMOTE_PARAM1 = dw_SKILL_REMOTE_PARAM1;
			DWORD local_dw_SKILL_REMOTE_CALL = dw_SKILL_REMOTE_CALL;
			__asm {

				pushad
				pushfd
				mov ecx, local_dw_SKILL_REMOTE_PARAM1
				push 0x0
				push 0x0
				push monster_real
				push skill_index
				push 0x0
				mov eax, local_dw_SKILL_REMOTE_CALL
				call eax
				popfd
				popad

			}
		}
		else {

			/// <summary>
			/// 单体技能
			/// </summary>
			//00481A7D | FF73 10 | push dword ptr ds : [ebx + 0x10] | 技能索引
			//00481A80 | 8BCF | mov ecx, edi | 00964CC8
			//00481A82 | 50 | push eax |
			//00481A83 | E8 FB2FFFFF | call 0x474A83 | 使用技能call
			DWORD local_dw_SKILL_PARAM1 = dw_SKILL_PARAM1;
			DWORD local_dw_SKILL_CALL = dw_SKILL_CALL;
			__asm {
				pushad
				pushfd
				push skill_index
				mov ecx, local_dw_SKILL_PARAM1 //3B 3D ?? ?? ?? ?? 75 0A B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 47 ?? 2B 47 ?? 53 33 DB 
				mov ecx, [ecx]
				push 0
				mov eax, local_dw_SKILL_CALL //E8 ?? ?? ?? ?? E9 ?? ?? ?? ?? 83 F8 0B 0F 85 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 70 ?? E8 ?? ?? ?? ?? FF 73 ?? 8B 48 ?? E8 ?? ?? ?? ?? 8B 4B ?? 83 C1 FB 83 F9 01 
				call eax
				popfd
				popad

			}
		}



	}

}


void FF_Dlg::OnBnClickedButtonGetitemobj()
{
	if (!this->player_instance->init())
	{
		return;
	}
	USES_CONVERSION;
	this->pkg->InitPackage(this->player_instance->PlayBase);
	DWORD item = 0;
	PDWORD packages = (PDWORD)this->pkg->packageBase;
	for (size_t i = 0; i < this->pkg->packageIndexMax; ++i)
	{
		item = packages[4] + 0x160 * i;
		if (item == 0)
		{
			continue;
		}


		DWORD itemId = *(PDWORD)(item + 0xc);
		WORD count = *(PWORD)(item + 0xc8);
		if (itemId == 0 || count > 9999)
		{
			continue;
		}
		DWORD nameAddr = this->pkg->GetItemName(itemId);
		//result.Format(L"%d %x %s[%d] %d,   %x      lvl:%d  ", i, ret, A2W((PCHAR)(nameAddr)), count, itemId, nameObj, itemLvl);

		CString result;
		result.Format(L"%08x  %s id:%d count:%d   index:%d ", item, A2W((PCHAR)(nameAddr + 4)), itemId, count, i);
		AddLog(result);
	}
}




void FF_Dlg::OnBnClickedButtonSurround()
{
	m_edit_logs.Empty();

	UpdateData(FALSE);
	int maxMembers = this->surround->GetMaxMembers();

	CString info;
	USES_CONVERSION;
	DWORD index = 0;
	DWORD member = 0;

	PDWORD members = (PDWORD)dw_SURRDOUND_BASE;
	for (size_t i = 0; i < maxMembers; i++)
	{
		CString tmp;
		try
		{
			//member = *((LPDWORD)(i * 4 + dw_SURRDOUND_BASE));	
			member = members[i];
			PDWORD currentMember = (PDWORD)member;
			if (member == 0 || member == -1)
			{
				continue;
			}

			//排除自身
			if (member == this->player_instance->PlayBase)
			{
				continue;
			}
			DWORD	type = *((LPDWORD)(member + 0x3ac));//未知类型分析

			if (!(type == 0 || type == 999))
			{
				continue;
			}

			DWORD memberId = *((LPDWORD)(member + SURROUND_MEMEBER_ID));
			if (memberId == 0)
			{
				continue;
			}

			PCHAR name = (PCHAR)(member + PLAY_OFFSET_NAME);



			CString testName(name);
			type = *((LPDWORD)(member + 0x3ac));//未知类型分析
			bool isok = false;
			if (testName.IsEmpty() && type == 0) ///物品对象
			{
				isok = true;
			}
			else if (!testName.IsEmpty() && type == 999) //有名字并且类别是999 就是 有生命体的对象
			{
				isok = true;
			}
			if (!isok)
			{
				continue;
			}


			SHORT hp = *((PSHORT)(member + PLAY_OFFSET_HP));
			SHORT mp = *((PSHORT)(member + PLAY_OFFSET_MP));
			SHORT fp = *((PSHORT)(member + PLAY_OFFSET_FP));
			//FLOAT x = *((PFLOAT)(member + PLAY_OFFSET_X));
			//FLOAT y = *((PFLOAT)(member + PLAY_OFFSET_Y));
			//FLOAT z = *((PFLOAT)(member + PLAY_OFFSET_Z));
			DWORD lvl = *((LPDWORD)(member + PLAY_OFFSET_LEVEL));
			//FLOAT far_away = *((PFLOAT)(member + SURROUND_FARAWAY));
			FLOAT far_away = this->surround->GetFarAway(this->player_instance->PlayBase, member);
			DWORD gm = *((LPDWORD)(member + PLAY_OFFSET_GM));
			//DWORD monster_sign = *((LPDWORD)(member + 0x2f0));

			DWORD goodid = *((LPDWORD)(member + 0x174));
			if (mp < -1 || mp>9999)
			{
				continue;
			}

			if (type == 0)//物品
			{
				if (!(hp == mp && mp == fp && fp == 0))
				{
					continue;
				}
				DWORD nameAddr = this->pkg->GetItemName(goodid);
				if (nameAddr == 0)
				{
					continue;
				}
				DWORD itemId = *(PDWORD)nameAddr;
				if (itemId == 0)
				{
					continue;
				}
				PBYTE B_id = (PBYTE)(member + SURROUND_MEMEBER_ID);
				tmp.Format(L"物品 %x  %08x  %s(%d) %02x%02x%02x%02x   id:%d  %d far:%.1f", member, memberId, A2W((PCHAR)(nameAddr + 4)), lvl, B_id[0], B_id[1], B_id[2], B_id[3], type, goodid, far_away);
			}
			else if (type == 999) {//怪物
				if ((hp == 1 && mp == -1) || hp <= 0)
				{
					//过滤NPC           
					continue;
				}
				/*	 2a4f414 | (null) | Ꞔ | 0 | 3418 | 100 | 397 | 8493.9 | 3356.7*/
				tmp.Format(L"怪物 %x  %08x  %s(%d)h:%d m:%d f:%d   %d   %d far:%.1f", member, memberId, A2W(name), lvl, hp, mp, fp, type, goodid, far_away);
			}
			else if (type != 999 && type != 0)
			{
				//其他东西
				tmp.Format(L"其他 %x  %08x  (%d)h:%d m:%d f:%d   %d   %d", member, memberId, lvl, hp, mp, fp, type, goodid);
			}

			if (!tmp.IsEmpty())
			{
				info.Append(tmp);
				info.Append(_T("\r\n"));
			}
			index++;
		}
		catch (const std::exception&)
		{
			continue;
		}


	}
	AddLog(info);
	info.Format(L"对象数量%d 共：%d", maxMembers, index);
	AddLog(info);

}









void FF_Dlg::OnBnClickedButtonGm()
{
	PDWORD gm = (PDWORD)(this->player_instance->PlayBase + PLAY_OFFSET_GM);
	if (*gm == 90)
	{
		*gm = 70;
	}
	else {
		*gm = 90;
	}


}


void FF_Dlg::OnBnClickedCheckAutoSell()
{
	if (this->m_btn_auto_sell.GetCheck() == BST_CHECKED)
	{
		AddLog(L"开启自动卖垃圾");
		SetTimer(AUTO_SELL, 5000 * 60, NULL);
	}
	else {
		AddLog(L"关闭自动卖垃圾");
		KillTimer(AUTO_SELL);
	}

}

void FF_Dlg::FillJobSkillLvl()
{





	xinshou_v.push_back(std::make_pair(L"1.弧月斩", 4));
	xinshou_v.push_back(std::make_pair(L"2.疾风突刺", 6));
	xinshou_v.push_back(std::make_pair(L"3.摘星斩", 11));

	jobMap.insert(make_pair(0, xinshou_v));
	jobNameMap.insert(std::make_pair(0, L"初心者"));

	warrior_v.push_back(std::make_pair(L"1.弧月斩", 4));
	warrior_v.push_back(std::make_pair(L"2.疾风突刺", 6));
	warrior_v.push_back(std::make_pair(L"3.摘星斩", 11));
	warrior_v.push_back(std::make_pair(L"4.逆手之回旋", 16));
	warrior_v.push_back(std::make_pair(L"5.战斧之光", 18));
	warrior_v.push_back(std::make_pair(L"6.光芒圣剑", 18));
	warrior_v.push_back(std::make_pair(L"7.坚硬之磐石", 19));
	warrior_v.push_back(std::make_pair(L"8.切裂之风轮", 21));
	warrior_v.push_back(std::make_pair(L"9.纠缠之藤蔓", 24));
	warrior_v.push_back(std::make_pair(L"10.毁灭之狂气", 27));
	warrior_v.push_back(std::make_pair(L"11.血之光辉", 30));
	warrior_v.push_back(std::make_pair(L"12.战神附体", 41));


	jobNameMap.insert(std::make_pair(1, L"战士"));
	jobMap.insert(make_pair(1, warrior_v));



	magic_v.push_back(std::make_pair(L"1.弧月斩", 4));
	magic_v.push_back(std::make_pair(L"2.疾风突刺", 6));
	magic_v.push_back(std::make_pair(L"3.摘星斩", 11));
	magic_v.push_back(std::make_pair(L"4.炽热焰星", 16));
	magic_v.push_back(std::make_pair(L"5.极地冰柱", 17));
	magic_v.push_back(std::make_pair(L"6.冰之锁链", 18));
	magic_v.push_back(std::make_pair(L"7.魔电召唤", 18));
	magic_v.push_back(std::make_pair(L"8.暴裂突岩", 19));
	magic_v.push_back(std::make_pair(L"9.螺旋烈风", 19));
	magic_v.push_back(std::make_pair(L"10.地狱火花", 20));
	magic_v.push_back(std::make_pair(L"11.空间转换", 22));
	magic_v.push_back(std::make_pair(L"12.神迹回复", 23));
	magic_v.push_back(std::make_pair(L"13.龙卷飓风", 24));
	magic_v.push_back(std::make_pair(L"14.火神爆焰", 27));
	magic_v.push_back(std::make_pair(L"15.神之诅咒", 30));
	magic_v.push_back(std::make_pair(L"16.灵波狂想", 35));
	magic_v.push_back(std::make_pair(L"17.分光化影", 41));

	jobMap.insert(make_pair(4, magic_v));
	jobNameMap.insert(std::make_pair(4, L"法师"));

	doctor_v.push_back(std::make_pair(L"1.弧月斩", 4));
	doctor_v.push_back(std::make_pair(L"2.疾风突刺", 6));
	doctor_v.push_back(std::make_pair(L"3.摘星斩", 11));
	doctor_v.push_back(std::make_pair(L"4.治疗术", 16));
	doctor_v.push_back(std::make_pair(L"5.圣光破", 16));
	doctor_v.push_back(std::make_pair(L"6.天马行空", 17));
	doctor_v.push_back(std::make_pair(L"7.命中术", 18));
	doctor_v.push_back(std::make_pair(L"8.强身术", 19));
	doctor_v.push_back(std::make_pair(L"9.智力术", 20));
	doctor_v.push_back(std::make_pair(L"10.神圣之光", 21));
	doctor_v.push_back(std::make_pair(L"11.躲避术", 21));
	doctor_v.push_back(std::make_pair(L"12.力量术", 22));
	doctor_v.push_back(std::make_pair(L"13.圣光大地", 22));
	doctor_v.push_back(std::make_pair(L"14.敏捷术", 24));
	doctor_v.push_back(std::make_pair(L"15.昏厥术", 25));
	doctor_v.push_back(std::make_pair(L"16.生命之泉", 27));
	doctor_v.push_back(std::make_pair(L"17.光芒圣歌", 30));
	doctor_v.push_back(std::make_pair(L"18.神光破", 31));
	doctor_v.push_back(std::make_pair(L"19.神之庇护", 33));
	doctor_v.push_back(std::make_pair(L"20.复仇之光", 36));


	jobMap.insert(make_pair(3, doctor_v));
	jobNameMap.insert(std::make_pair(3, L"圣职"));

}










FF_Dlg* ff_dlg;
BOOL FF_Dlg::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	ff_dlg = this;
	// TODO:  在此添加额外的初始化

	this->m_edit_fp = 150;
	this->m_edit_hp = 870;
	this->m_edit_mp = 150;

	UpdateData(false);

	if (this->player_instance->init())
	{
		DWORD packageMax = this->pkg->InitPackage(this->player_instance->PlayBase);
		CString packageInfo;
		packageInfo.Format(L"人物对象:%x 背包格子总数： %d", this->player_instance->PlayBase, packageMax);
		AddLog(packageInfo);
		DWORD jobType = this->player_instance->GetJobType();
		std::vector<std::pair<CString, DWORD> > current = jobMap.at(jobType);
		this->m_combox_skills.ResetContent();
		m_ListCtrl_sends.InsertColumn(0, _T("index"), LVCFMT_LEFT, 40);
		m_ListCtrl_sends.InsertColumn(1, _T("len"), LVCFMT_LEFT, 40);
		m_ListCtrl_sends.InsertColumn(2, _T("content"), LVCFMT_LEFT, 290);
		m_ListCtrl_sends.InsertColumn(3, _T("client"), LVCFMT_LEFT, 90);
		m_ListCtrl_sends.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
		for (auto item : current)
		{
			this->m_combox_skills.AddString(item.first);
		}
		this->m_combox_skills.SetCurSel(3);
		SetTimer(AUTO_REFRESH_INFO, 300, NULL);
	}
	else {
		AddLog(L"请选择角色后 重新启动 外挂");
		CWnd* pwndctrl = GetDlgItem(IDC_CHECK_AutoHpMpFp);
		for (int i = 0; i < 99 && pwndctrl != NULL; i++)
		{
			pwndctrl->EnableWindow(FALSE);
			pwndctrl = GetNextDlgTabItem(pwndctrl);
		}
	}
	return TRUE;
}
HANDLE gmDetectThread = NULL;
void FF_Dlg::OnBnClickedCheckGmonline()
{

	if (this->m_checkbox_gmstatus.GetCheck() == BST_CHECKED)
	{
		AddLog(L"开启检测 GM 在周围时游戏自动退出");
		checkGM = true;
		if (gmDetectThread == NULL)
		{
			gmDetectThread = ::CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(ThreadDetectGM), NULL, NULL, NULL);
		}
	}
	else {
		AddLog(L"关闭检测GM检测");
		checkGM = false;
		if (gmDetectThread != NULL)
		{
			//CloseHandle(gmDetectThread);
			TerminateThread(gmDetectThread, 999);
			gmDetectThread == NULL;
		}

	}

	// TODO: 在此添加控件通知处理程序代码
}


void FF_Dlg::OnBnClickedCheckNocollisior()
{
	PSHORT no_collisior = (PSHORT)(dw_NO_COLLISIOR);
	// TODO: 在此添加控件通知处理程序代码
	if (this->m_checkbox_nocollisior.GetCheck() == BST_CHECKED)
	{
		*no_collisior = BST_UNCHECKED;
		AddLog(L"开启穿墙");
	}
	else {
		*no_collisior = BST_CHECKED;
		AddLog(L"关闭穿墙");
	}

}


void FF_Dlg::OnBnClickedCheckSurroundExt()
{
	DWORD baseSurroundExt = *(PDWORD)(dw_PICK_TARGET_BASE);
	PSHORT no_collisior = (PSHORT)(baseSurroundExt + PICK_TARGET_SURROUND);
	// TODO: 在此添加控件通知处理程序代码
	if (this->m_checkbox_surround_ext.GetCheck() == BST_CHECKED)
	{
		*no_collisior = BST_UNCHECKED;
		AddLog(L"开启扩大寻怪范围");
	}
	else {
		*no_collisior = BST_CHECKED;
		AddLog(L"关闭扩大寻怪范围");
	}
}


void FF_Dlg::OnBnClickedCheckFrameSkill()
{

	PDWORD playSpeed = (PDWORD)(this->player_instance->PlayBase + PLAY_OFFSET_MOVE_SPEED);
	// TODO: 在此添加控件通知处理程序代码
	if (this->m_checkbox_frameskip.GetCheck() == BST_CHECKED)
	{
		CString speedValue;
		this->m_edit_move_speed.GetWindowTextW(speedValue);
		*playSpeed = _ttoi(speedValue);
		AddLog(L"开启加速");
	}
	else {
		*playSpeed = 0;
		AddLog(L"关闭加速");
	}


	// TODO: 在此添加控件通知处理程序代码
}


void FF_Dlg::OnBnClickedButtonShowinfo()
{
	int maxMembers = this->surround->GetMaxMembers();
	DWORD assertTmp0 = 0;
	DWORD assertTmp1 = 0;
	DWORD assertTmp2 = 0;
	DWORD assertTmp3 = 0;
	DWORD assertTmp4 = 0;
	CString info;
	USES_CONVERSION;

	DWORD member = 0;

	CString tmp;

	for (size_t i = 0; i < maxMembers; i++)
	{

		assertTmp0 = *((LPDWORD)(dw_PICK_TARGET_BASE));
		member = *((LPDWORD)(i * 4 + dw_SURRDOUND_BASE));
		if (member == 0 || member == -1)
		{
			continue;
		}

		assertTmp1 = *((LPDWORD)(assertTmp0 + 0xCC));
		if (assertTmp1 == 0)
		{

			continue;
		}
		assertTmp2 = *((LPDWORD)(member + 0x170));
		if (assertTmp2 != 0x4)
		{
			continue;
		}
		assertTmp3 = *((LPDWORD)(dw_SURROUND_TMP_1));//39 3D ?? ?? ?? ?? 75 0C 39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF 
		assertTmp4 = *((LPDWORD)(dw_SURROUND_TMP_2));//39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF D9 05 ?? ?? ?? ?? D9 5D E0 
		if (assertTmp3 == member && assertTmp4 == 0)
		{
			continue;
		}
		DWORD  nameObj = this->pkg->GetItemName2(member);
		DWORD id = *((LPDWORD)(member + 0x174));
		FLOAT x = *((PFLOAT)(member + PLAY_OFFSET_X));
		FLOAT y = *((PFLOAT)(member + PLAY_OFFSET_Y));
		FLOAT z = *((PFLOAT)(member + PLAY_OFFSET_Z));
		FLOAT far_away = *((PFLOAT)(member + SURROUND_FARAWAY));
		tmp.Format(L"%x %s ,%d    x:%.1f z:%.1f y%.1f s:%.1f ", member, A2W((PCHAR)(nameObj + 0x4)), id, x, z, y, far_away);
		info.Append(tmp);
		info.Append(_T("\r\n"));
		AddLog(info);
		this->surround->PickItem2(member, this->player_instance->PlayBase);

	}

}


void FF_Dlg::OnBnClickedCheckSilent()
{

	DWORD UI_BASE_ADDR = *((LPDWORD)(dw_PICK_TARGET_BASE));

	PDWORD pObjects = (PDWORD)(UI_BASE_ADDR + UI_OBJECTS);
	PDWORD pWATER = (PDWORD)(UI_BASE_ADDR + UI_WATER);
	PDWORD pTerrain = (PDWORD)(UI_BASE_ADDR + UI_TERRAIN);
	PDWORD pWeather = (PDWORD)(UI_BASE_ADDR + UI_WEATHER);
	// TODO: 在此添加控件通知处理程序代码
	if (this->m_checkbox_silent.GetCheck() == BST_CHECKED)
	{
		*pObjects = BST_UNCHECKED;
		*pWATER = BST_UNCHECKED;
		*pTerrain = BST_UNCHECKED;
		*pWeather = BST_UNCHECKED;
		AddLog(L"开启静默模式");
	}
	else {
		*pObjects = BST_CHECKED;
		*pWATER = BST_CHECKED;
		*pTerrain = BST_CHECKED;
		*pWeather = BST_CHECKED;
		AddLog(L"关闭静默模式");
	}
}










void FF_Dlg::OnBnClickedCheckCommonAtk()
{

	DWORD UI_BASE_ADDR = *((LPDWORD)(dw_PICK_TARGET_BASE));

	PDWORD pLoadObjects = (PDWORD)(UI_BASE_ADDR + UI_LOD_OBJECT);
	if (this->m_check_common_atk.GetCheck() == BST_CHECKED)
	{
		*pLoadObjects = BST_UNCHECKED;
	}
	else {
		*pLoadObjects = BST_CHECKED;
	}
}


void FF_Dlg::OnBnClickedCheckGet()
{
	// TODO: 在此添加控件通知处理程序代码
	if (this->m_checkbox_get.GetCheck() == BST_CHECKED)
	{
		AddLog(L"开启按关键字拾取");
		SetTimer(AUTO_EARCH_PICK, 200, NULL);
	}
	else {
		AddLog(L"停止开启按关键字拾取");
		KillTimer(AUTO_EARCH_PICK);
	}
}




void FF_Dlg::OnBnClickedButtonKaSkill()
{

	if (this->player_instance->GetJobType() == 1)
	{
		PDWORD nishou = (PDWORD)(this->player_instance->PlayBase + PLAY_OFFSET_NISHOU_ID);
		PDWORD fenglun = (PDWORD)(this->player_instance->PlayBase + PLAY_OFFSET_FENGLUN_ID);
		PDWORD kuangqi = (PDWORD)(this->player_instance->PlayBase + PLAY_OFFSET_KUANGQI_ID);
		*nishou = 11;
		*fenglun = 11;
		*kuangqi = 11;
		AddLog(L"修改战士技能成功，请拖到组合技能栏");
	}
}

BOOL TEAM_SKILL_START = FALSE;
void FF_Dlg::OnBnClickedCheckTeamDraw()
{
	DWORD team_level = *(PDWORD)(dw_TEAM_LEVEL);
	if (team_level < 40)
	{
		AddLog(L"剧团等级小于40级 请提升剧团等级");
		return;
	}

	DWORD startCounter = this->m_checkbox_team_luck.GetCheck() + this->m_checkbox_team_work.GetCheck() + this->m_checkbox_draw.GetCheck();
	if (startCounter == 0)
	{
		KillTimer(AUTO_TEAM_SKILL);
		TEAM_SKILL_START = FALSE;
		AddLog(L"停止自动团技能");
	}
	else if (!TEAM_SKILL_START) {
		SetTimer(AUTO_TEAM_SKILL, 200, NULL);
		TEAM_SKILL_START = TRUE;
		AddLog(L"启动自动团技能");
	}
}

void FF_Dlg::AutoTeamSkill()
{
	DWORD picked = this->surround->GetPicked();
	if (picked <= 0 || picked == this->player_instance->PlayBase)
	{
		return;
	}
	DWORD hp = *((LPDWORD)(picked + PLAY_OFFSET_HP));
	DWORD mp = *((LPDWORD)(picked + PLAY_OFFSET_MP));
	DWORD fp = *((LPDWORD)(picked + PLAY_OFFSET_FP));
	DWORD id = *((LPDWORD)(picked + PLAY_OFFSET_ID));
	DWORD lvl = *((LPDWORD)(picked + PLAY_OFFSET_LEVEL));
	if (lvl <= 1 || mp <= 1 || fp <= 1 || hp <= 0 || id > 1)
	{
		return;
	}
	DWORD point = *(PDWORD)(dw_TEAM_POINT);
	if (this->m_checkbox_team_work.GetCheck() == BST_CHECKED)
	{
		DWORD WORK_STATUS = *(PDWORD)(dw_TEAM_BUFF_BASE + TEAM_BUFF_WORK);
		if (WORK_STATUS == 0 && point > 1)
		{
			point -= 1;
			this->player_instance->TeamSkill(WORK);
		}
	}
	if (this->m_checkbox_draw.GetCheck() == BST_CHECKED)
	{
		DWORD DRAW_STATUS = *(PDWORD)(dw_TEAM_BUFF_BASE + TEAM_BUFF_DRAW);
		if (DRAW_STATUS == 0 && point > 5)
		{
			point -= 5;
			this->player_instance->TeamSkill(DRAW);
		}
	}
	if (this->m_checkbox_team_luck.GetCheck() == BST_CHECKED)
	{
		DWORD LUCK_STATUS = *(PDWORD)(dw_TEAM_BUFF_BASE + TEAM_BUFF_LUCK);
		if (LUCK_STATUS == 0 && point >= 8)
		{
			this->player_instance->TeamSkill(LUCK);
		}
	}



}




void FF_Dlg::OnBnClickedButtonFuben()
{
	int maxMembers = this->surround->GetMaxMembers();
	DWORD assertTmp0 = 0;
	DWORD assertTmp1 = 0;
	DWORD assertTmp2 = 0;
	DWORD assertTmp3 = 0;
	DWORD assertTmp4 = 0;
	CString info;
	USES_CONVERSION;
	DWORD index = 0;
	DWORD member = 0;

	CString tmp;

	for (size_t i = 0; i < maxMembers; i++)
	{

		assertTmp0 = *((LPDWORD)(dw_PICK_TARGET_BASE));
		member = *((LPDWORD)(i * 4 + dw_SURRDOUND_BASE));
		if (member == 0 || member == -1)
		{
			continue;
		}

		assertTmp1 = *((LPDWORD)(assertTmp0 + 0xCC));
		if (assertTmp1 == 0)
		{

			continue;
		}
		assertTmp2 = *((LPDWORD)(member + 0x170));
		if (assertTmp2 != 0x4)
		{
			continue;
		}
		assertTmp3 = *((LPDWORD)(dw_SURROUND_TMP_1));//39 3D ?? ?? ?? ?? 75 0C 39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF 
		assertTmp4 = *((LPDWORD)(dw_SURROUND_TMP_2));//39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF D9 05 ?? ?? ?? ?? D9 5D E0 
		if (assertTmp3 == member && assertTmp4 == 0)
		{
			continue;
		}
		DWORD  nameObj = this->pkg->GetItemName2(member);
		DWORD id = *((LPDWORD)(member + 0x174));
		FLOAT x = *((PFLOAT)(member + PLAY_OFFSET_X));
		FLOAT y = *((PFLOAT)(member + PLAY_OFFSET_Y));
		FLOAT z = *((PFLOAT)(member + PLAY_OFFSET_Z));
		FLOAT far_away = *((PFLOAT)(member + SURROUND_FARAWAY));
		DWORD itemLvl = *(PDWORD)(nameObj + 0x230);
		if (itemLvl != 0xffffffff || fubenPickUpMap.count(id) == 1)
		{
			tmp.Format(L"%x %s ,%d  lvl: %d   x:%.1f z:%.1f y%.1f s:%.1f  副本拾取 \r\n", member, A2W((PCHAR)(nameObj + 0x4)), id, itemLvl, x, z, y, far_away);
			AddLog(tmp);
			this->surround->PickItem2(member, this->player_instance->PlayBase);
		}

	}

}


void FF_Dlg::OnBnClickedButtonTest()
{
	/*

		004C282D   | 8D45 F4                   | lea eax,dword ptr ss:[ebp-0xC]                              |
		004C2830   | 64:A3 00000000            | mov dword ptr fs:[0],eax                                    |
		004C2836   | 898D 30FEFFFF             | mov dword ptr ss:[ebp-0x1D0],ecx                            |
		004C283C   | FF75 08                   | push dword ptr ss:[ebp+0x8]                                 | 技能id
		004C283F   | B9 509AA000               | mov ecx,neuz.A09A50                                         | 文件资源信息  这个参数和 物品名称call  一样 GET_ITEM_NAME_PARAM1
		004C2844   | E8 A71CF5FF               | call 0x4144F0                                               | 获取技能信息call

	*/

	USES_CONVERSION;

	for (size_t i = 1; i < 350; i++)
	{
		DWORD nameAddr = { 0 };
		__asm {
			pushad
			pushfd
			push i
			mov ecx, GET_ITEM_NAME_PARAM1
			mov eax, GET_SKILL_NAME_BY_ID
			call eax
			mov nameAddr, eax
			popfd
			popad
		}
		if (nameAddr == NULL || IsBadReadPtr((PVOID)nameAddr, 0x8) || nameAddr == 0xcccccccc)
		{
			continue;
		}

		DWORD skillId = *(PDWORD)(nameAddr);
		if (skillId == 0)
		{
			continue;
		}

		CString result;
		result.Format(L"id:%d  %s ", skillId, A2W((PCHAR)(nameAddr + 4)));
		AddLog(result);
	}


}

//自动采矿
void FF_Dlg::OnBnClickedButtonGetKuang()
{
	//if (!this->player_instance->init())
	//{
	//	return;
	//}
	//DWORD picked;
	//DWORD lingshou = this->surround->GetCloseToMonster(15);
	//if (lingshou == NULL)
	//{
	//	return;
	//}
	//DWORD hp = *((LPDWORD)(lingshou + PLAY_OFFSET_HP));
	//DWORD lvl = *((LPDWORD)(lingshou + PLAY_OFFSET_LEVEL));
	//if (lvl != 15)
	//{
	//	return;
	//}
	//picked = this->surround->PickMonster(lingshou);
	//if (picked <= 0)
	//{
	//	return;
	//}
	//picked = this->surround->GetPicked();
	//if (hp > 0)
	//{
	//	//先穿装备F8
	//	this->UseWeapon(7, 183);
	//	DWORD local_dw_SKILL_PARAM1 = dw_SKILL_PARAM1;
	//	DWORD local_dw_SKILL_CALL = dw_SKILL_CALL;
	//	DWORD skill_index = 9;
	//	for (size_t i = 0; i < 100; i++)
	//	{
	//		hp = *((LPDWORD)(lingshou + PLAY_OFFSET_HP));
	//		picked = this->surround->GetPicked();
	//		if (hp == 0 || picked == NULL)
	//		{
	//			break;
	//		}
	//		__asm {
	//			pushad
	//			pushfd
	//			push skill_index
	//			mov ecx, local_dw_SKILL_PARAM1 //3B 3D ?? ?? ?? ?? 75 0A B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 47 ?? 2B 47 ?? 53 33 DB 
	//			mov ecx, [ecx]
	//			push 0
	//			mov eax, local_dw_SKILL_CALL //E8 ?? ?? ?? ?? E9 ?? ?? ?? ?? 83 F8 0B 0F 85 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 70 ?? E8 ?? ?? ?? ?? FF 73 ?? 8B 48 ?? E8 ?? ?? ?? ?? 8B 4B ?? 83 C1 FB 83 F9 01 
	//			call eax
	//			popfd
	//			popad
	//		}
	//		Sleep(1000);
	//	}
	//	Sleep(2000);
	//	picked = this->surround->PickMonster(lingshou);
	//	Sleep(1000);
	//	this->UseWeapon(5, 183);
	//	Sleep(1000);
	//	this->pkg->QuickUse(6);
	//	Sleep(1000);
	//	picked = this->surround->GetPicked();
	//	this->pkg->QuickUse(6);
	//	Sleep(1000);
	//	this->pkg->QuickUse(6);
	//	while (picked != NULL)
	//	{
	//		picked = this->surround->GetPicked();
	//		Sleep(2000);
	//	}
	//}


	// TODO: 在此添加控件通知处理程序代码
}

void FF_Dlg::UseWeapon(DWORD useIndex, DWORD packageIndex)
{
	if (useIndex == 7)
	{
		this->pkg->QuickUse(5);
		Sleep(1000);
		this->pkg->QuickUse(7);
	}
	else {
		this->pkg->QuickUse(7);
		Sleep(1000);
		this->pkg->QuickUse(5);
	}



}




void FF_Dlg::OnBnClickedCheckKuang()
{
	if (this->m_checkbox_caikuang.GetCheck() == BST_CHECKED)
	{

		AddLog(L"开启自动采矿");
		SetTimer(AUTO_CAI_KUANG, 1000, NULL);
	}
	else {
		AddLog(L"停止自动采矿");
		KillTimer(AUTO_CAI_KUANG);
	}
}






static UINT SendMsgThread(LPVOID lparam) {
	FF_Dlg* m_ff_Dlg = (FF_Dlg*)lparam;
	CString cs_info;
	CString cs_content;
	CString cs_buffer;
	while (send_control)
	{
		if (flags)
		{
			if (len == 0)
			{
				continue;
			}
			if (filter_len > 0 && len != filter_len)
			{
				continue;
			}

			cs_info.Format(_T("%d"), pindex);
			m_ff_Dlg->m_ListCtrl_sends.InsertItem(pindex, cs_info);
			cs_info.Format(_T("%d"), len);
			m_ff_Dlg->m_ListCtrl_sends.SetItemText(pindex, 1, cs_info);
			for (size_t i = 0; i < len; i++)
			{
				cs_buffer.Format(_T("%02x"), ((PBYTE)content)[i]);
				cs_content += cs_buffer;
			}
			m_ff_Dlg->m_ListCtrl_sends.SetItemText(pindex, 2, cs_content);
			cs_info.Format(_T("%08x"), callAddr);
			m_ff_Dlg->m_ListCtrl_sends.SetItemText(pindex, 3, cs_info);
			pindex++;

			m_ff_Dlg->m_ListCtrl_sends.PostMessageW(WM_UPDATEDATA, FALSE, NULL);
			m_ff_Dlg->m_ListCtrl_sends.PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);
			PostMessage(m_ff_Dlg->m_hWnd, WM_UPDATEDATA, FALSE, NULL);
			flags = 0;
			cs_content = "";
		}


	}
	return true;


}
void _stdcall WsaLog(HookContext* context) {
	/*
		0074E0C0 | 55 | push ebp |
		0074E0C1 | 8BEC | mov ebp, esp |
		0074E0C3 | 51 | push ecx |
		0074E0C4 | 894D FC | mov dword ptr ss : [ebp - 0x4] , ecx |
		0074E0C7 | 8B45 FC | mov eax, dword ptr ss : [ebp - 0x4] | eax = [9fc508 + 0x4]
		0074E0CA | 8378 04 00 | cmp dword ptr ds : [eax + 0x4] , 0x0 |
		0074E0CE | 74 1F | je 0x74E0EF |                        要Hook的地址
		0074E0D0 | FF75 10 | push dword ptr ss : [ebp + 0x10] | 固定值1
		0074E0D3 | FF75 0C | push dword ptr ss : [ebp + 0xC] | 包长
		0074E0D6 | FF75 08 | push dword ptr ss : [ebp + 0x8] | 包内容
		0074E0D9 | 8B45 FC | mov eax, dword ptr ss : [ebp - 0x4] | eax = [9fc508 + 0x4]
		0074E0DC | 8B40 04 | mov eax, dword ptr ds : [eax + 0x4] | eax = [[9fc508 + 0x4]+ 0x4]
		0074E0DF | 8B4D FC | mov ecx, dword ptr ss : [ebp - 0x4] | ecx = [9fc508 + 0x4]
		0074E0E2 | 8B49 04 | mov ecx, dword ptr ds : [ecx + 0x4] | ecx = [[9fc508 + 0x4]+ 0x4]
		0074E0E5 | 8B00 | mov eax, dword ptr ds : [eax] | eax = [[[9fc508 + 0x4]+ 0x4] ]
		0074E0E7 | FF50 28 | call dword ptr ds : [eax + 0x28] | 发包封装函数 [[[9fc508 + 0x4]+ 0x4] ] + 0x28
		0074E0EA | 33C0 | xor eax, eax |
		0074E0EC | 40 | inc eax |
		0074E0ED | EB 02 | jmp 0x74E0F1 |
		0074E0EF | 33C0 | xor eax, eax |
		0074E0F1 | C9 | leave |
		0074E0F2 | C2 0C00 | ret 0xC |
	*/
	if (context->EBP == 0)
	{
		return;
	}
	len = *(PDWORD)(context->EBP + 0xc); //包长
	if (len > 0)
	{
		PVOID contentAddr = (PVOID) * (PDWORD)(context->EBP + 0x8); //包内容 
		callAddr = context->EAX;
		memmove((PVOID)content, contentAddr, len);
		flags = 1;
	}

}
void FF_Dlg::OnBnClickedButtonHook()
{
	DWORD dw_old;
	content = (DWORD)VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!content)
	{
		return;
	}
	memset((PVOID)content, 0xcc, 0x1000);
	//0x005DC834
	bool hookRet = inlinehook.Hook(dw_HOOK_WSASEND, WsaLog);
	if (!hookRet)
	{
		AddLog(L"hook failed");
		return;
	}
	this->m_checkbox_start_stop.SetCheck(BST_CHECKED);
	OnBnClickedCheckOpenStart();
}




void FF_Dlg::OnBnClickedButtonClear()
{
	pindex = 0;
	m_ListCtrl_sends.DeleteAllItems();
	m_edit_logs.Empty();
	// TODO: 在此添加控件通知处理程序代码
}


void FF_Dlg::OnNMDblclkListSends(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_ListCtrl_sends.GetSelectionMark();   //获取选中行的行号

	CString byteStr = m_ListCtrl_sends.GetItemText(nIndex, 2);
	m_edit_buffers.SetWindowTextW(byteStr);
	*pResult = 0;

}
int FF_Dlg::WSASendCall(PUCHAR buffer, int bufferLength) {

	/*  发包原始函数  //89 4D ?? 8B 45 ?? 83 78 ?? 00 74 1F FF 75 ?? FF 75 ?? FF 75 ?? 8B 45 ?? 8B 40 ?? 8B 4D ?? 8B 49 ?? 8B 00 FF 50 ??
		0074E0C0   | 55                       | push ebp                                                                                                                 |
		0074E0C1   | 8BEC                     | mov ebp,esp                                                                                                              |
		0074E0C3   | 51                       | push ecx                                                                                                                 |
		0074E0C4   | 894D FC                  | mov dword ptr ss:[ebp-0x4],ecx                                                                                           |
		0074E0C7   | 8B45 FC                  | mov eax,dword ptr ss:[ebp-0x4]                                                                                           | eax=[9fc508+0x4]
		0074E0CA   | 8378 04 00               | cmp dword ptr ds:[eax+0x4],0x0                                                                                           |
		0074E0CE   | 74 1F                    | je 0x74E0EF   //HOOK 地址                                                                                                |
		0074E0D0   | FF75 10                  | push dword ptr ss:[ebp+0x10]                                                                                             | 固定值1
		0074E0D3   | FF75 0C                  | push dword ptr ss:[ebp+0xC]                                                                                              | 包长
		0074E0D6   | FF75 08                  | push dword ptr ss:[ebp+0x8]                                                                                              | 包内容
		0074E0D9   | 8B45 FC                  | mov eax,dword ptr ss:[ebp-0x4]                                                                                           | eax=[9fc508+0x4]
		0074E0DC   | 8B40 04                  | mov eax,dword ptr ds:[eax+0x4]                                                                                           | eax=[[9fc508+0x4]+0x4]
		0074E0DF   | 8B4D FC                  | mov ecx,dword ptr ss:[ebp-0x4]                                                                                           | ecx=[9fc508+0x4]
		0074E0E2   | 8B49 04                  | mov ecx,dword ptr ds:[ecx+0x4]                                                                                           | ecx=[[9fc508+0x4]+0x4]
		0074E0E5   | 8B00                     | mov eax,dword ptr ds:[eax]                                                                                               | eax=[[[9fc508+0x4]+0x4]]
		0074E0E7   | FF50 28                  | call dword ptr ds:[eax+0x28]                                                                                             | 发包封装函数  [[[9fc508+0x4]+0x4]]+0x28
		0074E0EA   | 33C0                     | xor eax,eax                                                                                                              |
		0074E0EC   | 40                       | inc eax                                                                                                                  |
		0074E0ED   | EB 02                    | jmp 0x74E0F1                                                                                                             |
		0074E0EF   | 33C0                     | xor eax,eax                                                                                                              |
		0074E0F1   | C9                       | leave                                                                                                                    |
		0074E0F2   | C2 0C00                  | ret 0xC                                                                                                                  |
	*/
	unsigned int socketClient = 0x9fc508;
	//9fc508 + 0x4
	unsigned int ClientOffset = socketClient + 0x4;
	//[9fc508 + 0x4]
	unsigned int offset_4 = *(reinterpret_cast<unsigned int*>(ClientOffset));
	//[[9fc508 + 0x4]+ 0x4]
	unsigned int ecxVal = *(reinterpret_cast<unsigned int*>(offset_4 + 0x4));
	//0074E0E5 | 8B00 | mov eax, dword ptr ds : [eax] | eax = [[[9fc508 + 0x4]+ 0x4] ]
	unsigned int eaxVal = *(reinterpret_cast<unsigned int*>(ecxVal));
	//发包封装函数 [[[9fc508 + 0x4]+ 0x4] ] + 0x28
	unsigned int callAddr = *(reinterpret_cast<unsigned int*>(eaxVal + 0x28));
	__asm {
		pushad
		pushfd
		push 0x1
		push bufferLength
		push buffer
		mov eax, callAddr
		mov ecx, ecxVal
		call eax
		popfd
		popad
	}
}
//发包
void FF_Dlg::OnBnClickedButtonSend()
{
	CString  str_content;

	m_edit_buffers.GetWindowTextW(str_content);
	std::string hex_str = CT2A(str_content.GetString());
	BYTE Bytes[0x1000]{};
	DWORD ByteLength = hex_str.length() / 2;
	std::string  str_buffer{};
	for (size_t i = 0; i < ByteLength; i++)
	{
		str_buffer = hex_str.substr(i * 2, 2);
		sscanf_s(str_buffer.c_str(), "%x", &(Bytes[i]));
	}
	DWORD  pStart = (DWORD)&Bytes;
	WSASendCall((PUCHAR)pStart, ByteLength);
	//DWORD local_dw_SOCKET_SEND_PARAM1 = dw_SOCKET_SEND_PARAM1;
	//DWORD local_dw_SOCKET_SEND_CALL = dw_SOCKET_SEND_CALL;
	//__asm {
	//	pushad
	//	pushfd
	//	push 0x1
	//	push ByteLength
	//	push pStart
	//	mov ecx, local_dw_SOCKET_SEND_PARAM1
	//	mov ecx, [ecx + 0x4]
	//	mov eax, local_dw_SOCKET_SEND_CALL
	//	call eax
	//	popfd
	//	popad

	//}
	// TODO: 在此添加控件通知处理程序代码
}




void FF_Dlg::OnBnClickedButtonFilter()
{

	filter_len = GetDlgItemInt(IDC_EDIT_Len, NULL, 1);
	// TODO: 在此添加控件通知处理程序代码
}


void FF_Dlg::OnBnClickedCheckOpenStart()
{

	if (this->m_checkbox_start_stop.GetCheck() == BST_CHECKED)
	{

		if (!send_control)
		{
			send_control = true;
			pthread = AfxBeginThread(SendMsgThread, this);
		}

		AddLog(L"开始抓包");
	}
	else {
		send_control = false;
		flags = 0;
		AddLog(L"停止抓包");

	}
	// TODO: 在此添加控件通知处理程序代码
}


void FF_Dlg::OnBnClickedButtonUnhook()
{
	send_control = false;
	this->m_checkbox_start_stop.SetCheck(BST_UNCHECKED);
	inlinehook.UnHook();
	AddLog(L"卸载hook");
}





void FF_Dlg::OnBnClickedButtonCollectMonster()
{
	if (!this->player_instance->init())
	{
		return;
	}
	DWORD local_dw_TEAM_SKILL_PARAM1 = dw_TEAM_SKILL_PARAM1;
	DWORD local_dw_FLY_ATK_CALL = dw_FLY_ATK_CALL;
	int maxMembers = this->surround->GetMaxMembers();
	DWORD assertTmp0 = 0;
	DWORD assertTmp1 = 0;
	DWORD assertTmp2 = 0;
	DWORD assertTmp3 = 0;
	DWORD assertTmp4 = 0;
	CString info;
	USES_CONVERSION;
	DWORD index = 0;
	DWORD member = 0;
	CString tmp;
	DWORD id = 0;
	for (size_t i = 0; i < maxMembers; i++)
	{
		assertTmp0 = *((LPDWORD)(dw_PICK_TARGET_BASE));
		member = *((LPDWORD)(i * 4 + dw_SURRDOUND_BASE));
		if (member == 0 || member == -1)
		{
			continue;
		}

		assertTmp1 = *((LPDWORD)(assertTmp0 + 0xCC));
		if (assertTmp1 == 0)
		{

			continue;
		}
		assertTmp2 = *((LPDWORD)(member + 0x170));
		if (assertTmp2 != 0x5)
		{
			continue;
		}
		assertTmp3 = *((LPDWORD)(dw_SURROUND_TMP_1));//39 3D ?? ?? ?? ?? 75 0C 39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF 
		assertTmp4 = *((LPDWORD)(dw_SURROUND_TMP_2));//39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF D9 05 ?? ?? ?? ?? D9 5D E0 
		if (assertTmp3 == member && assertTmp4 == 0)
		{
			continue;
		}

		PCHAR name = (PCHAR)(member + PLAY_OFFSET_NAME);
		DWORD hp = *((LPDWORD)(member + PLAY_OFFSET_HP));
		DWORD mp = *((LPDWORD)(member + PLAY_OFFSET_MP));
		DWORD fp = *((LPDWORD)(member + PLAY_OFFSET_FP));
		FLOAT x = *((PFLOAT)(member + PLAY_OFFSET_X));
		FLOAT y = *((PFLOAT)(member + PLAY_OFFSET_Y));
		FLOAT z = *((PFLOAT)(member + PLAY_OFFSET_Z));
		DWORD lvl = *((LPDWORD)(member + PLAY_OFFSET_LEVEL));
		FLOAT far_away = *((PFLOAT)(member + SURROUND_FARAWAY));
		DWORD gm = *((LPDWORD)(member + PLAY_OFFSET_GM));
		DWORD monster_sign = *((LPDWORD)(member + 0x2f0));
		lvl = *((LPDWORD)(member + PLAY_OFFSET_LEVEL));
		if (lvl == 1 || mp <= 1 || fp <= 1 || hp <= 0 || id > 1)
		{
			continue;
		}

		if (this->m_edit_monster_lvl > 0 && lvl != this->m_edit_monster_lvl)
		{
			continue;
		}
		DWORD picked = this->surround->PickMonsterFly(member);
		picked = this->surround->GetPicked();
		if (picked > 0)
		{
			DWORD monster_real = *(PDWORD)(picked + 0x2f0);

			__asm {
				pushad
				pushfd
				push 0
				push 0
				push monster_real
				mov ecx, local_dw_TEAM_SKILL_PARAM1
				push 0x1d
				mov eax, local_dw_FLY_ATK_CALL
				call eax
				popfd
				popad
			}
			tmp.Format(L"%x %x %s(%d)h:%d m:%d f:%d  x:%.1f z:%.1f y%.1f s:%.1f   gm:%d", member, monster_sign, A2W(name), lvl, hp, mp, fp, x, z, y, far_away, gm);
			AddLog(tmp);
		}
		index++;
	}




}
