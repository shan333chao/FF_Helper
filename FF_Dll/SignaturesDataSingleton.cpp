#include "pch.h"
#include "SignaturesDataSingleton.h"
#include "SignaturesCodeTool.h"


void SignaturesDataSingleton::InitSignatureDataAddr()
{
	mapSignatureDesc.insert(std::make_pair("MAIN_OFFSET_BASE", "A1 &?? ?? ?? ?? 33 F6 3B C6 74 54 8B 88 ?? ?? ?? ?? 3B CE 74 4A 39 B1 ?? ?? ?? ?? 74 42 83 B8 ?? ?? ?? ?? 4C"));
	mapSignatureDesc.insert(std::make_pair("SURROUND_MAX_MEMBERS", "8B 0D &?? ?? ?? ?? 89 75 E8 3B CE 0F 8E ?? ?? ?? ?? 8B 45 E8 8B 3C 85 ?? ?? ?? ?? B8 FF 00 00 00 66 39 87 ?? ?? ?? ?? 0F 83 ?? ?? ?? ?? 39 B3 ?? ?? ?? ?? 74 68 8B 87 ?? ?? ?? ?? 39 B0 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("SURRDOUND_BASE", "8B 3C 85 &?? ?? ?? ?? B8 FF 00 00 00 66 39 87 ?? ?? ?? ?? 0F 83 ?? ?? ?? ?? 39 B3 ?? ?? ?? ?? 74 68 8B 87 ?? ?? ?? ?? 39 B0 ?? ?? ?? ?? 74 5A"));
	mapSignatureDesc.insert(std::make_pair("SURROUND_TMP_1", "39 3D &?? ?? ?? ?? 75 0C 39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF"));
	mapSignatureDesc.insert(std::make_pair("SURROUND_TMP_2", "39 35 &?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF D9 05 ?? ?? ?? ?? D9 5D E0"));
	mapSignatureDesc.insert(std::make_pair("PICK_TARGET_BASE", "A1 &?? ?? ?? ?? 8B 40 ?? 85 C0 0F 84 ?? ?? ?? ?? 8B 91 ?? ?? ?? ?? F6 42 ?? 08"));
	mapSignatureDesc.insert(std::make_pair("NO_COLLISIOR", "39 05 &?? ?? ?? ?? 0F 94 C0 A3 ?? ?? ?? ?? E9 ?? ?? ?? ?? A1 ?? ?? ?? ?? 33 C9 39 88 ?? ?? ?? ?? 0F 94 C1 89 88 ?? ?? ?? ?? E9 ?? ?? ?? ?? 33 C0 33 F6 39 35 ?? ?? ?? ?? 68 6A 02 00 00"));
	mapSignatureDesc.insert(std::make_pair("TEAM_BUFF_BASE", "89 3C B5 &?? ?? ?? ?? E8 ?? ?? ?? ?? 89 5D FC 33 C0 2B F0 89 45 E8 0F 84 ?? ?? ?? ?? 4E 0F 84 ?? ?? ?? ?? 4E 0F 84 ?? ?? ?? ?? 4E"));
	mapSignatureDesc.insert(std::make_pair("TEAM_POINT", "FF 35 &?? ?? ?? ?? 8D 45 F0 57 50 E8 ?? ?? ?? ?? 83 C4 0C 68 4A 02 00 00 8B CE E8 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("TEAM_LEVEL", "8B 0D &?? ?? ?? ?? 2B 8F ?? ?? ?? ?? 78 29 8B 4D D0 50 68 00 00 00 FF 83 C7 04 57 8B 7D DC 57 53 E8 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("QUICK_USE_PARAM1", "A1 &?? ?? ?? ?? 8B 80 ?? ?? ?? ?? 3B C7 7C 3C "));
	mapSignatureDesc.insert(std::make_pair("QUICK_USE_PARAM2", "BF &?? ?? ?? ?? 85 C0 74 5E 8D 45 C0 50 8D 45 B8 50 8D 45 BC 50 B9 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("QUICK_USE_CALL", "E8 &?? ?? ?? ?? 8B 45 C0 8B 44 05 D4 C6 80 ?? ?? ?? ?? 00 81 45 BC A4 00 00 00 83 45 C0 04 81 7D BC 68 06 00 00 0F 8C ?? ?? ?? ?? 8B CB"));
	mapSignatureDesc.insert(std::make_pair("GET_ITEM_NAME_PARAM1", "BE &?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? "));
	mapSignatureDesc.insert(std::make_pair("GET_ITEM_NAME_CALL", "E8 &?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? 83 65 FC 00 83 7B ?? 78 C7 45 F0 01 00 00 00 75 53 8B 8F ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("SELL_ITEM_PARAM1", "B9 &?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 00 6A 01 6A 00 6A 00 6A 04 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 1F"));
	mapSignatureDesc.insert(std::make_pair("SELL_ITEM_CALL", "E8 &?? ?? ?? ?? 6A 00 6A 01 6A 00 6A 00 6A 04 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 1F E8 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("PICK_MONSTER_CALL", "E8 &?? ?? ?? ?? 8B C6 5F 5E 5B C2 08 00 55 8B EC 83 EC 1C A1 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("PICK_ITEM_CALL", "E8 &?? ?? ?? ?? 8A 43 ?? 88 43 ?? 33 C0 40 8B 4D FC 5F 5E 33 CD 5B E8 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("ACTION_PARAM1", "B9 &?? ?? ?? ?? 6A 03 E8 ?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ??"));
	mapSignatureDesc.insert(std::make_pair("ACTION_CALL", "E8 &?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ?? B9 ?? ?? ?? ?? 6A 0A"));
	mapSignatureDesc.insert(std::make_pair("SKILL_PARAM1", "3B 3D &?? ?? ?? ?? 75 0A B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 47 ?? 2B 47 ?? 53 33 DB"));
	mapSignatureDesc.insert(std::make_pair("SKILL_CALL", "E8 &?? ?? ?? ?? E9 ?? ?? ?? ?? 83 F8 0B 0F 85 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 70 ?? E8 ?? ?? ?? ?? FF 73 ?? 8B 48 ?? E8 ?? ?? ?? ?? 8B 4B ?? 83 C1 FB 83 F9 01"));
	mapSignatureDesc.insert(std::make_pair("ADD_POINT_PARAM1", "B9 &?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 00 8B CE E8 ?? ?? ?? ?? 5E C3 6A 04"));
	mapSignatureDesc.insert(std::make_pair("ADD_POINT_CALL", "E8 &?? ?? ?? ?? 6A 00 8B CE E8 ?? ?? ?? ?? 5E C3 6A 04 B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F1 89 75 F0 E8 ?? ?? ?? ?? 83 65 FC 00"));
	mapSignatureDesc.insert(std::make_pair("TEAM_SKILL_PARAM1", "B9 &?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 43 ?? 8B 0D ?? ?? ?? ?? 8B 55 F0 89 8C 82 ?? ?? ?? ?? E9 ?? ?? ?? ?? 51 51 68 E2 07 00 00"));
	mapSignatureDesc.insert(std::make_pair("TEAM_SKILL_CALL", "E8 &?? ?? ?? ?? 8B 43 ?? 8B 0D ?? ?? ?? ?? 8B 55 F0 89 8C 82 ?? ?? ?? ?? E9 ?? ?? ?? ?? 51 51 68 E2 07 00 00"));
 

	SignaturesCodeTool sct;
	sct.InitModuleTextSize();
	PVOID64 ADDR_RET;
	DWORD addr;
	for (auto signature : mapSignatureDesc)
	{
		ADDR_RET = sct.Search(signature.second);
		addr = *(PDWORD)ADDR_RET;
		if (signature.first.find("CALL") != std::string::npos)
		{
			addr = addr + (DWORD)ADDR_RET + 4;
			mapSignatureValue.insert(std::make_pair(signature.first, addr));
		}
		else {
			mapSignatureValue.insert(std::make_pair(signature.first, addr));
		}
	}
}

SignaturesDataSingleton::SignaturesDataSingleton()
{
	InitSignatureDataAddr();
}
SignaturesDataSingleton::~SignaturesDataSingleton()
{
}

DWORD SignaturesDataSingleton::GetAddressByKey(std::string key)
{
	if (mapSignatureValue.count(key) > 0)
	{
		return mapSignatureValue.at(key);
	}
	return NULL;
}

SignaturesDataSingleton& SignaturesDataSingleton::getInstance()
{
	static SignaturesDataSingleton instance;
	return instance;
}

std::map<std::string, DWORD> SignaturesDataSingleton::GetAllSignatureValue()
{
	return this->mapSignatureValue;
}
