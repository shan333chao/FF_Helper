#include "pch.h"
#include "Surround.h"
#include "ff_offset.h"


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
	/*
		004F2638 | 6A 01 | push 0x1 | 固定值
		004F263A | FF75 F4 | push dword ptr ss : [ebp - 0xC] | 要选的的周围对象
		004F263D | 8B4D FC | mov ecx, dword ptr ss : [ebp - 0x4] | 选中容器[0xA06788 + 0x14]
		004F2640 | E8 2DC92200 | call 0x71EF72 |
		004F2645 | 8B45 F4 | mov eax, dword ptr ss : [ebp - 0xC] |
		004F2648 | C9 | leave |
	*/


	DWORD tmpMonster = monster;
	DWORD result{};
	//选中容器
	DWORD local_dw_PICK_TARGET_BASE = *(PDWORD)(ATK_SELECTED_BASE + 0x14);
	DWORD local_dw_PICK_MONSTER_CALL = PICK_SURROUND_ITEM;
	__asm {
		pushad
		pushfd
		push 0x1
		push tmpMonster
		mov ecx, local_dw_PICK_TARGET_BASE
		mov eax, local_dw_PICK_MONSTER_CALL
		call eax
		mov result, eax
		popfd
		popad
	}
	return result;
}

DWORD Surround::PickMonsterFly(DWORD monster)
{
	DWORD local_dw_PICK_FLY_CALL = dw_PICK_FLY_CALL;
	DWORD local_dw_SURROUND_TMP_1 = dw_SURROUND_TMP_1;
	DWORD local_monster = monster;
	__asm {
		pushad
		pushfd
		push 0x1
		push local_monster
		mov ebx, local_dw_SURROUND_TMP_1
		mov ebx, [ebx]
		mov ecx, [ebx + 0x16c]
		mov eax, local_dw_PICK_FLY_CALL
		call eax
		popfd
		popad
	}


	return 0;
}




DWORD Surround::GetPicked()
{
	DWORD basePicked = *((LPDWORD)(dw_PICK_TARGET_BASE));

	basePicked = *((LPDWORD)(basePicked + PICK_TARGET_OFFSET));
	return basePicked;
}
/// <summary>
/// 计算平方根
/// </summary>
/// <param name="number"></param>
/// <returns></returns>
static float Q_rsqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;            // 浮点数的位模式转为整数
	i = 0x5F3759DF - (i >> 1); // 魔法数初始猜测
	y = *(float*)&i;           // 整数转回浮点数
	y = y * (threehalfs - (x2 * y * y)); // 牛顿迭代
	// y  = y * (threehalfs - (x2 * y * y)); // 可重复迭代提高精度
	return y;
}

// 计算平方根
static float sqrt_fast(float S) {
	if (S == 0) return 0;
	return S * Q_rsqrt(S);
}

// 计算距离
static float CalculateDistance_Fast(
	float x1, float y1, float z1,
	float x2, float y2, float z2
) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	float dz = z2 - z1;
	float sqrSum = dx * dx + dy * dy + dz * dz;
	return sqrt_fast(sqrSum);
}
FLOAT Surround::GetFarAway(DWORD player, DWORD surrountItem)
{
	PFLOAT userPos = (PFLOAT)(player + 0x160);
	PFLOAT item = (PFLOAT)(surrountItem + 0x160);
	float ux = userPos[0];
	float uz = userPos[1];
	float uy = userPos[2];

	float itemx = item[0];
	float itemz = item[1];
	float itemy = item[2];

	return CalculateDistance_Fast(ux,uy,uz,itemx,itemy,itemz);
}

DWORD Surround::PickItem(DWORD itemObj, DWORD playerObj)
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




DWORD Surround::PickItem2(DWORD itemObj, DWORD playerObj)
{

	//远程拾取

	//50 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? BE ?? ?? ?? ?? 85 C0 74 10 83 B8 ?? ?? ?? ?? 00 74 07 8B CE E8 ?? ?? ?? ?? D9 45 ?? 51 D9 1C 24 57 8B CE E8 ?? ?? ?? ?? 
	if (itemObj == NULL)
	{
		return 0;
	}
	DWORD itemSig = *(PDWORD)(itemObj + 0x2f0);
	DWORD result = 0;
	DWORD local_dw_PICK_REMOTE_PARAM1 = dw_PICK_REMOTE_PARAM1;
	DWORD local_dw_PICK_REMOTE1_CALL = dw_PICK_REMOTE1_CALL;
	DWORD local_dw_PICK_REMOTE2_PARAM2 = dw_PICK_REMOTE2_PARAM2;
	DWORD local_dw_PICK_REMOTE2_CALL = dw_PICK_REMOTE2_CALL;

	__asm {
		pushad
		pushfd
		push 0x0
		mov ecx, local_dw_PICK_REMOTE_PARAM1
		mov eax, local_dw_PICK_REMOTE1_CALL
		call eax
		mov esi, local_dw_PICK_REMOTE2_PARAM2
		push ecx
		push itemSig
		mov ecx, esi
		mov eax, local_dw_PICK_REMOTE2_CALL
		call eax
		popfd
		popad
	}
	return result;
}
