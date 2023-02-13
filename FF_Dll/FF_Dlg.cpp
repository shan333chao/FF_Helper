// FF_Dlg.cpp: 实现文件
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
#include "hooked_def.h"
#include "include/MinHook.h"
#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif
#include "SignaturesCodeTool.h"
#include "SignaturesDataSingleton.h"



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
DWORD dw_SURRDOUND_BASE = SignaturesDataSingleton::getInstance().GetAddressByKey("SURRDOUND_BASE");
DWORD dw_PICK_TARGET_BASE = SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_TARGET_BASE");
DWORD dw_SURROUND_TMP_1 = SignaturesDataSingleton::getInstance().GetAddressByKey("SURROUND_TMP_1");
DWORD dw_SURROUND_TMP_2 = SignaturesDataSingleton::getInstance().GetAddressByKey("SURROUND_TMP_2");
DWORD dw_ACTION_PARAM1 = SignaturesDataSingleton::getInstance().GetAddressByKey("ACTION_PARAM1");
DWORD dw_ACTION_CALL = SignaturesDataSingleton::getInstance().GetAddressByKey("ACTION_CALL");

DWORD dw_SKILL_PARAM1 = SignaturesDataSingleton::getInstance().GetAddressByKey("SKILL_PARAM1");
DWORD dw_SKILL_CALL = SignaturesDataSingleton::getInstance().GetAddressByKey("SKILL_CALL");

DWORD dw_NO_COLLISIOR = SignaturesDataSingleton::getInstance().GetAddressByKey("NO_COLLISIOR");
DWORD dw_TEAM_BUFF_BASE = SignaturesDataSingleton::getInstance().GetAddressByKey("TEAM_BUFF_BASE");

DWORD dw_TEAM_POINT = SignaturesDataSingleton::getInstance().GetAddressByKey("TEAM_POINT");
DWORD dw_TEAM_LEVEL = SignaturesDataSingleton::getInstance().GetAddressByKey("TEAM_LEVEL");





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
	this->GetOldFunAddress();
	m_s_iSpeedTimes = 0;
	MH_Initialize();

}

FF_Dlg::~FF_Dlg()
{
	MH_Uninitialize();
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
	DDX_Control(pDX, IDC_CHECK_COLLECT, m_checkbox_collect);
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
}


BEGIN_MESSAGE_MAP(FF_Dlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_AutoHpMpFp, &FF_Dlg::OnBnClickedCheckAutohpmpfp)
	ON_BN_CLICKED(IDC_CHECK_Auto_Skill, &FF_Dlg::OnBnClickedCheckAutoSkill)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_LoadDll, &FF_Dlg::OnBnClickedButtonLoaddll)
	ON_BN_CLICKED(IDC_BUTTON_ATK, &FF_Dlg::OnBnClickedButtonAtk)
	ON_BN_CLICKED(IDC_BUTTON_GetItemObj, &FF_Dlg::OnBnClickedButtonGetitemobj)
	ON_BN_CLICKED(IDC_BUTTON_USE, &FF_Dlg::OnBnClickedButtonUse)
	ON_BN_CLICKED(IDC_BUTTON_Surround, &FF_Dlg::OnBnClickedButtonSurround)

	ON_BN_CLICKED(IDC_BUTTON_GM, &FF_Dlg::OnBnClickedButtonGm)
	ON_BN_CLICKED(IDC_CHECK_AUTO_SELL, &FF_Dlg::OnBnClickedCheckAutoSell)

	ON_BN_CLICKED(IDC_CHECK_COLLECT, &FF_Dlg::OnBnClickedCheckCollect)
	ON_BN_CLICKED(IDC_CHECK_GMONLINE, &FF_Dlg::OnBnClickedCheckGmonline)
	ON_BN_CLICKED(IDC_CHECK_NoCollisior, &FF_Dlg::OnBnClickedCheckNocollisior)
	ON_BN_CLICKED(IDC_CHECK_SURROUND_EXT, &FF_Dlg::OnBnClickedCheckSurroundExt)
	ON_BN_CLICKED(IDC_CHECK_FRAME_SKILL, &FF_Dlg::OnBnClickedCheckFrameSkill)
	ON_BN_CLICKED(IDC_BUTTON_ShowInfo, &FF_Dlg::OnBnClickedButtonShowinfo)
	ON_BN_CLICKED(IDC_CHECK_SILENT, &FF_Dlg::OnBnClickedCheckSilent)
	ON_BN_CLICKED(IDC_BUTTON_AGI_ADD, &FF_Dlg::OnBnClickedButtonAgiAdd)
	ON_BN_CLICKED(IDC_BUTTON_STR_ADD, &FF_Dlg::OnBnClickedButtonStrAdd)
	ON_BN_CLICKED(IDC_BUTTON_INT_ADD, &FF_Dlg::OnBnClickedButtonIntAdd)
	ON_BN_CLICKED(IDC_BUTTON_VIT_ADD, &FF_Dlg::OnBnClickedButtonVitAdd)


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

		CollectMonster();
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
	OnBnClickedCheckGmonline();
	CDialogEx::OnClose();
}
UINT_PTR WINAPI MySetTimer(__in_opt HWND hWnd, __in UINT_PTR nIDEvent, __in UINT uElapse, __in_opt TIMERPROC lpTimerFunc)
{
	UINT uMyElapse = (UINT)((double)uElapse / m_s_iSpeedTimes);

	return oldSetTimer(hWnd, nIDEvent, uMyElapse, lpTimerFunc);
}
LONG WINAPI MyGetMessageTime(VOID)
{
	if (m_s_iSpeedTimes != pre_multiSpeed)
	{
		first_GetMessageTimed = FALSE;
		first_timeGetTimed = FALSE;
		first_GetTickCounted = FALSE;
		first_QueryPerformanceCountered = FALSE;
		pre_multiSpeed = m_s_iSpeedTimes;
	}
	if (first_GetMessageTimed == FALSE)
	{
		if (Has_Pre_GetMessageTime == FALSE)
		{
			First_GetMessageTime = oldGetMessageTime();
			New_First_GetMessageTime = First_GetMessageTime;
			New_Pre_GetMessageTime = First_GetMessageTime;
			Has_Pre_GetMessageTime = TRUE;
		}
		else
		{
			First_GetMessageTime = Pre_GetMessageTime;
			New_First_GetMessageTime = New_Pre_GetMessageTime;
		}
		first_GetMessageTimed = TRUE;
	}
	Pre_GetMessageTime = oldGetMessageTime();
	New_Pre_GetMessageTime = LONG(double(Pre_GetMessageTime - First_GetMessageTime) * m_s_iSpeedTimes) + New_First_GetMessageTime;
	return New_Pre_GetMessageTime;
}

void WINAPI MySleep(DWORD dwMilliseconds)
{
	cannot_exit++;
	DWORD myInterval = (DWORD)((double)dwMilliseconds / m_s_iSpeedTimes);
	oldSleep(myInterval);
	cannot_exit--;
	return;
}

DWORD WINAPI  NewTimeGetTime2(void)
{
	static DWORD fake = 0;
	static DWORD last_real = 0;
	DWORD now = (*oldTimeGetTime)();
	DWORD result;

	if (last_real == 0)
	{
		result = fake = last_real = now;
	}
	else
	{
		result = fake + m_s_iSpeedTimes * (now - last_real);
		fake = result;
		last_real = now;
	}

	return result;
}

DWORD WINAPI  NewGetTickCount2(void)
{
	static DWORD fake = 0;
	static DWORD last_real = 0;
	DWORD now = (*oldGetTickCount)();
	DWORD result;

	if (last_real == 0)
	{
		result = fake = last_real = now;
	}
	else
	{
		result = fake + m_s_iSpeedTimes * (now - last_real);
		fake = result;
		last_real = now;
	}

	return result;
}

BOOL WINAPI  NewQueryPerformanceCounter(__out LARGE_INTEGER* lpPerformanceCount)
{
	BOOL ret = (*oldQueryPerformanceCounter)(lpPerformanceCount);
	if (!ret) return ret;

	static LARGE_INTEGER fake = { 0 };
	static LARGE_INTEGER last_real = { 0 };
	LARGE_INTEGER now = *lpPerformanceCount;

	if (last_real.QuadPart == 0)
	{
		fake = last_real = now;
	}
	else
	{
		lpPerformanceCount->QuadPart = fake.QuadPart + m_s_iSpeedTimes * (now.QuadPart - last_real.QuadPart);
		fake = *lpPerformanceCount;
		last_real = now;
	}

	return ret;
}

void FF_Dlg::OnBnClickedButtonLoaddll()
{

	if (m_s_iSpeedTimes > 0)
	{
		m_s_iSpeedTimes = 0;
		MH_DisableHook(&GetTickCount);
		MH_DisableHook(&timeGetTime);
		MH_DisableHook(&QueryPerformanceCounter);
		MH_DisableHook(&oldSetTimer);
		MH_DisableHook(&oldGetMessageTime);
		MH_DisableHook(&oldSleep);
		//MH_DisableHook(&oldTimeSetEvent);
	}
	else {
		m_s_iSpeedTimes = 5;

		MH_EnableHook(&GetTickCount);
		MH_EnableHook(&timeGetTime);
		MH_EnableHook(&QueryPerformanceCounter);
		MH_EnableHook(&oldSetTimer);
		MH_EnableHook(&oldGetMessageTime);
		MH_EnableHook(&oldSleep);
		//MH_EnableHook(&oldTimeSetEvent);
	}


}


void FF_Dlg::OnBnClickedButtonAtk()
{
	if (!this->player_instance->init())
	{
		return;
	}
	DWORD picked = this->surround->GetPicked();

	if (this->m_check_common_atk.GetCheck() != BST_CHECKED && picked <= 0)
	{

		DWORD targetMonster = this->surround->GetCloseToMonster(this->m_edit_monster_lvl);
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
			DWORD picked = this->surround->PickMonster(targetMonster);
			if (picked <= 0)
			{
				return;
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


void FF_Dlg::OnBnClickedButtonGetitemobj()
{
	if (!this->player_instance->init())
	{
		return;
	}
	this->pkg->InitPackage(this->player_instance->PlayBase);
	CString result;
	WORD count = 0;
	DWORD itemId = 0;
	DWORD itemLvl = -1;
	USES_CONVERSION;
	for (size_t i = 0; i < this->pkg->packageIndexMax; i++)
	{

		if (equipments.count(i) == 1) //过滤穿戴装备
			continue;
		DWORD ret = this->pkg->getPackageItem(i);
		if (ret == 0)
		{
			continue;
		}
		itemLvl = -1;
		count = *((LPWORD)(ret + 0xb8));
		itemId = *((LPWORD)(ret + 0xc));
		DWORD  nameObj = this->pkg->GetItemName(ret);
		if (nameObj == NULL)
		{
			continue;
		}
		itemLvl = *(PDWORD)(nameObj + 0x230);
		if (itemLvl != 0xffffffff && itemLvl > 1 && count == 1)
		{
			this->pkg->SellItem(ret);
			result.Format(L"%d %x %s[%d] %d,   %x      lvl:%d 卖 ", i, ret, A2W((PCHAR)(nameObj + 0x4)), count, itemId, nameObj, itemLvl);
			AddLog(result);
		}
	}


}


void FF_Dlg::OnBnClickedButtonUse()
{



}


void FF_Dlg::OnBnClickedButtonSurround()
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
		//(LPCWCHAR)name,
	/*	 2a4f414 | (null) | Ꞔ | 0 | 3418 | 100 | 397 | 8493.9 | 3356.7*/
		tmp.Format(L"%x %s(%d)h:%d m:%d f:%d  x:%.1f z:%.1f y%.1f s:%.1f   gm:%d", member, A2W(name), lvl, hp, mp, fp, x, z, y, far_away, gm);
		info.Append(tmp);
		info.Append(_T("\r\n"));
		index++;
	}
	AddLog(info);
	info.Format(L"对象数量%d 共：%d", maxMembers, index);
	AddLog(info);

}

void FF_Dlg::CollectMonster()
{


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
	fubenPickUpMap.insert(std::make_pair(60211, L"首饰金片"));
	fubenPickUpMap.insert(std::make_pair(63610, L"祭坛积分"));
	fubenPickUpMap.insert(std::make_pair(10270, L"力量之石"));
	fubenPickUpMap.insert(std::make_pair(10431, L"复活卷轴"));
	fubenPickUpMap.insert(std::make_pair(10218, L"无敌技能券"));
	fubenPickUpMap.insert(std::make_pair(63459, L"黑货券"));

	equipments.insert(std::make_pair(0, L"戒指"));
	equipments.insert(std::make_pair(1, L"戒指"));
	equipments.insert(std::make_pair(42, L"耳环"));
	equipments.insert(std::make_pair(52, L"耳环"));
	equipments.insert(std::make_pair(181, L"项链"));
	equipments.insert(std::make_pair(176, L"眼睛"));
	equipments.insert(std::make_pair(53, L"勋章"));
	equipments.insert(std::make_pair(15, L"飞行器"));

	equipments.insert(std::make_pair(183, L"武器"));
	equipments.insert(std::make_pair(185, L"盾牌"));
	equipments.insert(std::make_pair(187, L"帽子"));
	equipments.insert(std::make_pair(2, L"衣服"));
	equipments.insert(std::make_pair(85, L"披风"));
	equipments.insert(std::make_pair(89, L"手套"));
	equipments.insert(std::make_pair(172, L"鞋子"));
	equipments.insert(std::make_pair(14, L"时装帽子"));
	equipments.insert(std::make_pair(127, L"时装鞋子"));
	equipments.insert(std::make_pair(36, L"时装衣服"));
	equipments.insert(std::make_pair(133, L"时装手套"));



}

void FF_Dlg::OnBnClickedCheckCollect()
{
	if (this->m_checkbox_collect.GetCheck() == BST_CHECKED)
	{

	}
	else {

	}
}






void FF_Dlg::GetOldFunAddress()
{
	oldSetTimer = (SETTIMER)GetProcAddress(GetModuleHandle(TEXT("User32.dll")), "SetTimer");
	oldGetMessageTime = (GETMESSAGETIME)GetProcAddress(GetModuleHandle(TEXT("User32.dll")), "GetMessageTime");
	oldSleep = (TypeSleep)::GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "Sleep");
	oldGetTickCount = (GETTICKCOUNT)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "GetTickCount");
	oldQueryPerformanceCounter = (QUERYPERFORMANCECOUNTER)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "QueryPerformanceCounter");
	//oldTimeSetEvent = (TIMESETEVENT)GetProcAddress(GetModuleHandle(TEXT("Winmm.dll")), "timeSetEvent");
	oldTimeGetTime = (TIMEGETTIME)GetProcAddress(GetModuleHandle(TEXT("Winmm.dll")), "timeGetTime");
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


	MH_CreateHook(&GetTickCount, &NewGetTickCount2, reinterpret_cast<LPVOID*>(&oldGetTickCount));
	MH_CreateHook(&timeGetTime, &NewTimeGetTime2, reinterpret_cast<LPVOID*>(&oldTimeGetTime));
	MH_CreateHook(&QueryPerformanceCounter, &NewQueryPerformanceCounter, reinterpret_cast<LPVOID*>(&oldQueryPerformanceCounter));
	MH_CreateHook(&oldSetTimer, &MySetTimer, reinterpret_cast<LPVOID*>(&oldSetTimer));
	//MH_CreateHook(&oldTimeSetEvent, &MyTimeSetEvent, reinterpret_cast<LPVOID*>(&oldTimeSetEvent));
	MH_CreateHook(&oldSleep, &MySleep, reinterpret_cast<LPVOID*>(&oldSleep));
	MH_CreateHook(&oldGetMessageTime, &MyGetMessageTime, reinterpret_cast<LPVOID*>(&oldGetMessageTime));


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
		this->surround->PickItem(member, this->player_instance->PlayBase);

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


void FF_Dlg::OnBnClickedButtonAgiAdd()
{
	if (this->player_instance->GetPotential() == 0)
	{
		AddLog(L"没有可用潜能点了");
		return;
	}
	this->player_instance->AddPoint(AGILE);
	// TODO: 在此添加控件通知处理程序代码
}


void FF_Dlg::OnBnClickedButtonStrAdd()
{
	if (this->player_instance->GetPotential() == 0)
	{
		AddLog(L"没有可用潜能点了");
		return;
	}
	this->player_instance->AddPoint(STRONG);

}


void FF_Dlg::OnBnClickedButtonIntAdd()
{
	if (this->player_instance->GetPotential() == 0)
	{
		AddLog(L"没有可用潜能点了");
		return;
	}
	this->player_instance->AddPoint(INTELLECT);

}


void FF_Dlg::OnBnClickedButtonVitAdd()
{
	if (this->player_instance->GetPotential() == 0)
	{
		AddLog(L"没有可用潜能点了");
	}
	this->player_instance->AddPoint(VITALITY);
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
			this->surround->PickItem(member, this->player_instance->PlayBase);
		}

	}

}


void FF_Dlg::OnBnClickedButtonTest()
{
	CString item;
	for (auto kv : SignaturesDataSingleton::getInstance().GetAllSignatureValue())
	{
		item.Format(L"%s  %x \n", CString(kv.first.c_str()), kv.second);
		AddLog(item);
	}
}


void FF_Dlg::OnBnClickedButtonGetKuang()
{
	if (!this->player_instance->init())
	{
		return;
	}
	DWORD picked;
	DWORD lingshou = this->surround->GetCloseToMonster(15);
	if (lingshou == NULL)
	{
		return;
	}
	DWORD hp = *((LPDWORD)(lingshou + PLAY_OFFSET_HP));
	DWORD lvl = *((LPDWORD)(lingshou + PLAY_OFFSET_LEVEL));
	if (lvl != 15)
	{
		return;
	}
	picked = this->surround->PickMonster(lingshou);
	if (picked <= 0)
	{
		return;
	}
	picked = this->surround->GetPicked();
	if (hp > 0)
	{
		//先穿装备F8
		this->UseWeapon(7, 183);
		DWORD local_dw_SKILL_PARAM1 = dw_SKILL_PARAM1;
		DWORD local_dw_SKILL_CALL = dw_SKILL_CALL;
		DWORD skill_index = 9;
		for (size_t i = 0; i < 100; i++)
		{
			hp = *((LPDWORD)(lingshou + PLAY_OFFSET_HP));
			picked = this->surround->GetPicked();
			if (hp == 0 || picked == NULL)
			{
				break;
			}
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
			Sleep(1000);
		}
		Sleep(2000);
		picked = this->surround->PickMonster(lingshou);
		Sleep(1000);
		this->UseWeapon(5, 183);
		Sleep(1000); 
		this->pkg->QuickUse(6);
		Sleep(1000);
		picked = this->surround->GetPicked();
		this->pkg->QuickUse(6);
		Sleep(1000);
		this->pkg->QuickUse(6);
		while (picked != NULL)
		{
			picked = this->surround->GetPicked();
			Sleep(2000);
		}
	}


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
