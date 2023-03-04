#include "pch.h"
#include "Package.h"
#include "ff_offset.h"

DWORD Package::InitPackage(DWORD base)
{

	this->packageBase = base;
	this->packageIndexMax = *((LPDWORD)(base + PLAY_PACKAGE_MAX_INDEX));
	if (this->packageIndexMax == NULL)
	{
		return 0;
	}

	return 	this->packageIndexMax;
}

DWORD Package::getPackageItem(DWORD index)
{
	if (index > this->packageIndexMax)
	{
		return 0;
	}
	DWORD dw_index = index;
	DWORD dw_packageBase = this->packageBase;
	DWORD ret_eax{};
	__asm {
		pushad
		pushfd
		mov eax, index
		mov ecx, dw_packageBase
		add ecx, PACKAGE_OFFSET
		imul eax, eax, 0xf8
		add eax, [ecx + 0x10]
		mov ecx, eax
		mov eax, [ecx + 0xc]
		neg eax
		sbb eax, eax
		and eax, ecx
		mov ret_eax, eax
		popfd
		popad
	}
	return ret_eax;
 
}

DWORD Package::QuickUse(DWORD index)
{
	int arr[9] = { 0x0, 0xa4,0x148 ,0x1ec,0x290 ,0x334,0x3d8,0x47c,0x520 };

	DWORD F_KEY = arr[index];
	//
	//DWORD param1= *((LPDWORD)0x0096353C);
	//param1= *((LPDWORD)param1 + 0x5584);;
	//param1 += F_KEY;
	//方法特征  使用快捷键特征码
	//8B B3 ?? ?? ?? ?? 03 75 BC 83 7E ?? 05 75 2F FF 76 ?? 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 C0 74 1D 8B C8 E8 ?? ?? ?? ?? 83 B8 ?? ?? ?? ?? FF 74 0D 8B 45 C0 C7 80 ?? ?? ?? ?? 01 00 00 00 56 8B CF E8 ?? ?? ?? ?? 
	DWORD local_dw_QUICK_USE_PARAM1 = dw_QUICK_USE_PARAM1;
	DWORD local_dw_QUICK_USE_PARAM2 = dw_QUICK_USE_PARAM2;
	DWORD local_dw_QUICK_USE_PARAM3 = dw_QUICK_USE_PARAM3;
	DWORD local_dw_QUICK_USE_CALL = dw_QUICK_USE_CALL;
	__asm {
		pushad
		pushfd
		mov esi, local_dw_QUICK_USE_PARAM1   //A1 ?? ?? ?? ?? 8B 80 ?? ?? ?? ?? 3B C7 7C 3C 
		mov esi, [esi] 
		mov esi,[esi+ 0x55b4]
		add esi, F_KEY //esi==18606470
		push esi //18606514
		mov ecx, local_dw_QUICK_USE_PARAM2 //BF ?? ?? ?? ?? 85 C0 74 5E 8D 45 C0 50 8D 45 B8 50 8D 45 BC 50 B9 ?? ?? ?? ?? 
		mov eax, local_dw_QUICK_USE_CALL //E8 ?? ?? ?? ?? 8B 45 C0 8B 44 05 D4 C6 80 ?? ?? ?? ?? 00 81 45 BC A4 00 00 00 83 45 C0 04 81 7D BC 68 06 00 00 0F 8C ?? ?? ?? ?? 8B CB 
		call eax
		popfd
		popad

	}

	return 0;
}

DWORD Package::GetItemName(DWORD itemObj)
{
	//00569B60 | FF77 0C | push dword ptr ds : [edi + 0xC] |
	//00569B63 | BE 78E18400 | mov esi, neuz.84E178 | esi:"\\Yx" == &"H`F"
	//00569B68 | 8BCE | mov ecx, esi | esi:"\\Yx" == &"H`F"
	//00569B6A | E8 3EB8E9FF | call 0x4053AD |

	DWORD item = itemObj;
	DWORD nameAddr = {};
	DWORD local_dw_GET_ITEM_NAME_PARAM1 = dw_GET_ITEM_NAME_PARAM1;
	DWORD local_dw_GET_ITEM_NAME_CALL = dw_GET_ITEM_NAME_CALL;
	__asm {
		pushad
		pushfd
		mov edi, itemObj
		mov edi, [edi + 0xc]
		push edi
		mov ecx, local_dw_GET_ITEM_NAME_PARAM1  //BE ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? 
		mov eax, local_dw_GET_ITEM_NAME_CALL //E8 ?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? 83 65 FC 00 83 7B ?? 78 C7 45 F0 01 00 00 00 75 53 8B 8F ?? ?? ?? ?? 
		call eax
		mov nameAddr, eax
		popfd
		popad
	}
	return nameAddr;
}


DWORD Package::GetItemName2(DWORD itemObj)
{
	//00569B60 | FF77 0C | push dword ptr ds : [edi + 0xC] |
	//00569B63 | BE 78E18400 | mov esi, neuz.84E178 | esi:"\\Yx" == &"H`F"
	//00569B68 | 8BCE | mov ecx, esi | esi:"\\Yx" == &"H`F"
	//00569B6A | E8 3EB8E9FF | call 0x4053AD |

	DWORD item = itemObj;
	DWORD nameAddr = {};
	DWORD local_dw_GET_ITEM_NAME_PARAM1 = dw_GET_ITEM_NAME_PARAM1;
	DWORD local_dw_GET_ITEM_NAME_CALL = dw_GET_ITEM_NAME_CALL;
	__asm {
		pushad
		pushfd
		mov edi, itemObj
		mov edi, [edi + 0x174]
		push edi
		mov ecx, local_dw_GET_ITEM_NAME_PARAM1  //BE ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? 
		mov eax, local_dw_GET_ITEM_NAME_CALL //E8 ?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? 83 65 FC 00 83 7B ?? 78 C7 45 F0 01 00 00 00 75 53 8B 8F ?? ?? ?? ?? 
		call eax
		mov nameAddr, eax
		popfd
		popad
	}
	return nameAddr;
}

DWORD Package::SellItem(DWORD itemObj)
{
	//0047336D | 50 | push eax | 出售数量
	//0047336E | 8B86 68100000 | mov eax, dword ptr ds : [esi + 0x1068] | 物品对象
	//00473374 | 0FB640 04 | movzx eax, byte ptr ds : [eax + 0x4] | eax : "行"
	//00473378 | 50 | push eax |
	//00473379 | B9 400F8400 | mov ecx, neuz.840F40 | 840F40:"hMx"
	//0047337E | E8 78BEFBFF | call 0x42F1FB |
	DWORD item = itemObj;
	DWORD nameAddr = {};
	DWORD count = *((LPWORD)(itemObj + 0xb8));
	DWORD local_dw_SELL_ITEM_PARAM1 = dw_SELL_ITEM_PARAM1;
	DWORD local_dw_SELL_ITEM_CALL = dw_SELL_ITEM_CALL;
	__asm {
		pushad
		pushfd
		push count
		mov edi, itemObj
		mov edi, [edi + 0x4]
		push edi
		mov ecx, local_dw_SELL_ITEM_PARAM1 //B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 00 6A 01 6A 00 6A 00 6A 04 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 1F 
		mov eax, local_dw_SELL_ITEM_CALL //E8 ?? ?? ?? ?? 6A 00 6A 01 6A 00 6A 00 6A 04 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 1F E8 ?? ?? ?? ?? 
		call eax

		popfd
		popad
	}
	return nameAddr;
}

//DWORD Package::GetItemInfo(DWORD itemObj)
//{
//	DWORD item =*(PDWORD)(itemObj + 0xc);
//	DWORD itemLvl = {};
//
//
//	//00476729 | FF73 0C | push dword ptr ds : [ebx + 0xC] | 物品对象 id
//	//0047672C | 8BCE | mov ecx, esi | 0084C168
//	//0047672E | E8 EEEBF8FF | call 0x405321 |
//	__asm {
//		pushad
//		pushfd
//		push item
//		mov ecx, 0x0084C168
//		mov eax, 0x405321
//		call eax
//		mov itemLvl, eax
//		popfd
//		popad
//	}
//	return itemLvl;
//
//}
