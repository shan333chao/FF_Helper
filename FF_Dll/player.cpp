#include "pch.h"
#include "player.h"
#include "ff_offset.h"
 

bool player::init()
{

	this->PlayBase = *((LPDWORD)(dw_MAIN_OFFSET_BASE));

	return this->PlayBase != NULL;

}
DWORD player::GetJobType()
{
	this->job_type = *((LPDWORD)(this->PlayBase + PLAY_OFFSET_JOB));
	return this->job_type;
}

DWORD player::GetHp()
{
	this->hp = *((LPDWORD)(this->PlayBase + PLAY_OFFSET_HP));
	return this->hp;
}

DWORD player::GetMp()
{
	this->mp = *((LPDWORD)(this->PlayBase + PLAY_OFFSET_MP));
	return this->mp;
}

DWORD player::GetFp()
{
	this->fp = *((LPDWORD)(this->PlayBase + PLAY_OFFSET_FP));
	return this->fp;
}
 


void player::UpdatePos()
{
	this->x = *((PFLOAT)(this->PlayBase + PLAY_OFFSET_X));
	this->z = *((PFLOAT)(this->PlayBase + PLAY_OFFSET_Z));
	this->y = *((PFLOAT)(this->PlayBase + PLAY_OFFSET_Y));
	this->lvl = *((LPDWORD)(this->PlayBase + PLAY_OFFSET_LEVEL));
	this->id = *((LPDWORD)(this->PlayBase + PLAY_OFFSET_ID));

}



CString player::GetPlayName()
{
	USES_CONVERSION;
	PCHAR nameCache = (PCHAR)(this->PlayBase + PLAY_OFFSET_NAME);
	this->name.Format(_T("%s"), A2W(nameCache));
	return this->name;
}




FLOAT player::GetPosX()
{
	return this->x;
}

FLOAT player::GetPosZ()
{
	return this->z;
}

FLOAT player::GetPosY()
{
	return this->y;
}

DWORD player::GetId()
{
	return this->id;
}

DWORD player::GetLvl()
{
	return this->lvl;
}

void player::AddPoint(DWORD pointType)
{
 

}

void player::TeamSkill(TEAM_GROUP_SKILL skillIndex)
{
	DWORD index = skillIndex;
	DWORD local_dw_TEAM_SKILL_PARAM1 = dw_TEAM_SKILL_PARAM1;
	DWORD local_dw_TEAM_SKILL_CALL = dw_TEAM_SKILL_CALL;
	__asm {
		pushad
		pushfd
		push  index
		mov ecx, local_dw_TEAM_SKILL_PARAM1 //B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 43 ?? 8B 0D ?? ?? ?? ?? 8B 55 F0 89 8C 82 ?? ?? ?? ?? E9 ?? ?? ?? ?? 51 51 68 E2 07 00 00 
		mov eax, local_dw_TEAM_SKILL_CALL //E8 ?? ?? ?? ?? 8B 43 ?? 8B 0D ?? ?? ?? ?? 8B 55 F0 89 8C 82 ?? ?? ?? ?? E9 ?? ?? ?? ?? 51 51 68 E2 07 00 00 
		call eax
		popfd
		popad
	}
}
