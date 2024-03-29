#pragma once
#include "SignaturesDataSingleton.h"
class Surround
{
public:
	DWORD SurroundBase{};
	DWORD GetMaxMembers(); 

	DWORD GetCloseToMonster(DWORD monsterLvl);

	DWORD PickMonster(DWORD monster);
	DWORD PickMonsterFly(DWORD monster);

	DWORD GetPicked();


	DWORD PickItem(DWORD itemObj, DWORD playerObj);
	DWORD PickItem2(DWORD itemObj, DWORD playerObj);
private:
	DWORD dw_SURROUND_MAX_MEMBERS= SignaturesDataSingleton::getInstance().GetAddressByKey("SURROUND_MAX_MEMBERS");
	DWORD dw_SURRDOUND_BASE = SignaturesDataSingleton::getInstance().GetAddressByKey("SURRDOUND_BASE");
	DWORD dw_PICK_TARGET_BASE = SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_TARGET_BASE");
	DWORD dw_SURROUND_TMP_1 = SignaturesDataSingleton::getInstance().GetAddressByKey("SURROUND_TMP_1");
	DWORD dw_SURROUND_TMP_2 = SignaturesDataSingleton::getInstance().GetAddressByKey("SURROUND_TMP_2");
	DWORD dw_PICK_MONSTER_CALL = SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_MONSTER_CALL");
	DWORD dw_PICK_ITEM_CALL= SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_ITEM_CALL");
	DWORD dw_PICK_REMOTE_PARAM1 = SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_REMOTE_PARAM1");
	DWORD dw_PICK_REMOTE1_CALL = SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_REMOTE1_CALL");
	DWORD dw_PICK_REMOTE2_PARAM2 = SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_REMOTE2_PARAM2");
	DWORD dw_PICK_REMOTE2_CALL = SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_REMOTE2_CALL");
	DWORD dw_TEAM_SKILL_PARAM1 = SignaturesDataSingleton::getInstance().GetAddressByKey("TEAM_SKILL_PARAM1");
	DWORD dw_PICK_FLY_CALL= SignaturesDataSingleton::getInstance().GetAddressByKey("PICK_FLY_CALL");



};

