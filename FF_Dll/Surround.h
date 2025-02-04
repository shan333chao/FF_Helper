#pragma once
#include "ff_offset.h"
class Surround
{
public:
	DWORD SurroundBase{};
	DWORD GetMaxMembers(); 

	DWORD GetCloseToMonster(DWORD monsterLvl);
	//选中怪物
	DWORD PickMonster(DWORD monster);
	DWORD PickMonsterFly(DWORD monster);

	DWORD GetPicked();
	//获取人物与周围 对象的实际距离
	FLOAT GetFarAway(DWORD player, DWORD surrountItem);

	DWORD PickItem(DWORD itemObj, DWORD playerObj);
	DWORD PickItem2(DWORD itemObj, DWORD playerObj);
private:
	DWORD dw_SURROUND_MAX_MEMBERS= SURROUND_MAX_MEMBERS;
	DWORD dw_SURRDOUND_BASE = SURRDOUND_BASE;
	DWORD dw_PICK_TARGET_BASE =0;
	DWORD dw_SURROUND_TMP_1 =0;
	DWORD dw_SURROUND_TMP_2 = 0;
	DWORD dw_PICK_MONSTER_CALL =0;
	DWORD dw_PICK_ITEM_CALL= 0;
	DWORD dw_PICK_REMOTE_PARAM1 =0;
	DWORD dw_PICK_REMOTE1_CALL =0;
	DWORD dw_PICK_REMOTE2_PARAM2 = 0;
	DWORD dw_PICK_REMOTE2_CALL = 0;
	DWORD dw_TEAM_SKILL_PARAM1 =0;
	DWORD dw_PICK_FLY_CALL=0;



};

