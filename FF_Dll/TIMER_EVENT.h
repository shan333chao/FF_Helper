#pragma once
enum TimerEvent
{
	AUTO_HPMPFP = 1,
	AUTO_ATK = 2,
	AUTO_SELL = 3,
	AUTO_COLLECT_MONSTER = 4,
	AUTO_REFRESH_INFO = 5,
	AUTO_EARCH_PICK = 6,
	AUTO_TEAM_SKILL=7,
	AUTO_CAI_KUANG=8
};
enum POTENTIAL_TYPE
{
	STRONG = 0x64,
	VITALITY = 0x65,
	AGILE = 0x66,
	INTELLECT = 0x67

};

enum TEAM_GROUP_SKILL {
	WORK=0x5,
	DRAW=0x6,
	LUCK=0x8 
};