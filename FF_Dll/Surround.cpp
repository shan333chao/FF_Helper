#include "pch.h"
#include "Surround.h"
#include "ff_offset.h"
#include "SignaturesDataSingleton.h"

DWORD Surround::GetMaxMembers()
{
	
	return *((LPDWORD)(dw_SURROUND_MAX_MEMBERS));
}

DWORD Surround::GetCloseToMonster(DWORD monsterLvl)
{
	int maxMembers = GetMaxMembers();
	DWORD assertTmp0 = 0;
	DWORD assertTmp1 = 0;
	DWORD assertTmp2 = 0;
	DWORD assertTmp3 = 0;
	DWORD assertTmp4 = 0;
	CString info;
	USES_CONVERSION;
	DWORD index = 0;
	DWORD member = 0;
	FLOAT closeTo = 9999;
	DWORD mp = 999;
	DWORD lvl = 99;
	//FLOAT tmpfarAway = 0;
	DWORD closeToMonster = 0;
	DWORD hp = 999;
	DWORD fp = 999;
	FLOAT z = 0;
	DWORD id = 0;
	DWORD gm = 0;

	

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

		//PCHAR name = (PCHAR)(member + PLAY_OFFSET_NAME);
		hp = *((LPDWORD)(member + PLAY_OFFSET_HP));
		mp = *((LPDWORD)(member + PLAY_OFFSET_MP));
		fp = *((LPDWORD)(member + PLAY_OFFSET_FP));
		id = *((LPDWORD)(member + PLAY_OFFSET_ID));

		//z = *((PFLOAT)(member + PLAY_OFFSET_X));
		//FLOAT y = *((PFLOAT)(member + PLAY_OFFSET_Y));  
		//gm = *((LPDWORD)(member + PLAY_OFFSET_GM));
		lvl = *((LPDWORD)(member + PLAY_OFFSET_LEVEL));
		if (lvl == 1 || mp <= 1 || fp <= 1 || hp <= 0 || id > 1)
		{
			continue;
		}

		if (monsterLvl > 0 && lvl != monsterLvl)
		{
			continue;
		}
		//tmpfarAway = *((PFLOAT)(member + SURROUND_FARAWAY));
		//if (tmpfarAway < closeTo)
		//{
		//	closeTo = tmpfarAway;
		//	closeToMonster = member;
		//}
		closeToMonster = member;
	}
	return closeToMonster;
}

DWORD Surround::PickMonster(DWORD monster)
{
	if (monster == NULL || *((LPDWORD)(monster)) == 0)
	{
		return 0;
	}
	//| push 0x1      | 0x1
	//| push esi		| 怪物对象
	//| mov ecx, edi  | [00848CCC]
	//| call 0x5B4F55 | 选中

	DWORD tmpMonster = monster;
	DWORD result{};
	DWORD local_dw_PICK_TARGET_BASE = dw_PICK_TARGET_BASE;
	DWORD local_dw_PICK_MONSTER_CALL = dw_PICK_MONSTER_CALL;
	__asm {
		pushad
		pushfd
		push 0x1
		push tmpMonster
		mov ecx, local_dw_PICK_TARGET_BASE
		mov ecx, [ecx]
		mov eax, local_dw_PICK_MONSTER_CALL //E8 ?? ?? ?? ?? 8B C6 5F 5E 5B C2 08 00 55 8B EC 83 EC 1C A1 ?? ?? ?? ?? 
		call eax
		mov result, eax
		popfd
		popad
	}
	return result;
}




DWORD Surround::GetPicked()
{
	DWORD basePicked = *((LPDWORD)(dw_PICK_TARGET_BASE));

	basePicked = *((LPDWORD)(basePicked + PICK_TARGET_OFFSET));
	return basePicked;
}

DWORD Surround::PickItem(DWORD itemObj,DWORD playerObj)
{
	//00491797 | 57 | push edi | 地面物品对象
	//00491798 | 8BCE | mov ecx, esi | 20C06028
	//0049179A | E8 EEB0FFFF | call 0x48C88D | 拾取call
	if (itemObj == NULL)
	{
		return 0;
	}
	DWORD result = 0;
	DWORD local_dw_PICK_ITEM_CALL = dw_PICK_ITEM_CALL;
	__asm {
		pushad
		pushfd
		mov ecx, playerObj
		push itemObj
		mov eax, local_dw_PICK_ITEM_CALL //E8 ?? ?? ?? ?? 8A 43 ?? 88 43 ?? 33 C0 40 8B 4D FC 5F 5E 33 CD 5B E8 ?? ?? ?? ?? 
		call eax
		popfd
		popad
	}
	return result;
}




