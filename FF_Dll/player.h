#pragma once
#include "ff_offset.h"
#include "TIMER_EVENT.h"

class player
{
private:
	DWORD dw_MAIN_OFFSET_BASE = MAIN_OFFSET_BASE;
 
	DWORD dw_TEAM_SKILL_PARAM1 = 0x0;
	DWORD dw_TEAM_SKILL_CALL = 0x0;
	DWORD hp{};
	DWORD mp{};
	DWORD fp{};
	DWORD lvl{};
	DWORD potential{};
	DWORD id{};
	FLOAT x{};
	FLOAT z{};
	FLOAT y{};
	CString name{};
	DWORD job_type{};

public:
	DWORD moduleBase{};
	player() { 
		this->moduleBase = (DWORD)GetModuleHandle(MODULE_NAME);
	}
	DWORD PlayBase{};
	bool init();
	DWORD GetHp();
	DWORD GetMp();
	DWORD GetFp();
 
	DWORD GetJobType();
	void UpdatePos();
	CString GetPlayName();
	FLOAT GetPosX();
	FLOAT GetPosZ();
	FLOAT GetPosY();
	DWORD GetId();
	DWORD GetLvl();
	void AddPoint(DWORD pointType);

	void TeamSkill(TEAM_GROUP_SKILL skillIndex);
};

