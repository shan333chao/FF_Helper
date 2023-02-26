#pragma once
#define MODULE_NAME  _T("Neuz.exe")
#define SECTION_CHARACTERISTICS  0x60000020 
//#define MAIN_OFFSET_BASE 0x44A7E8
//#define MAIN_OFFSET_BASE 0x0084F808 //A1 ?? ?? ?? ?? 33 F6 3B C6 74 54 8B 88 ?? ?? ?? ?? 3B CE 74 4A 39 B1 ?? ?? ?? ?? 74 42 83 B8 ?? ?? ?? ?? 4C 
#define PLAY_OFFSET_LEVEL 0x8b8
#define PLAY_OFFSET_HP 0x8D8
#define PLAY_OFFSET_MP 0x8DC
#define PLAY_OFFSET_FP 0x8E0
#define PLAY_OFFSET_POTENTIAL 0x8e8
#define PLAY_OFFSET_NAME 0x17AC
#define PLAY_OFFSET_X 0x54
#define PLAY_OFFSET_Z 0x58
#define PLAY_OFFSET_Y 0x5C
#define PLAY_OFFSET_ID 0x100c
#define PLAY_OFFSET_GM 0x924
#define SURROUND_FARAWAY 0x2cc
#define PLAY_OFFSET_JOB 0x920
#define PLAY_PACKAGE_MAX_INDEX 0xdd0
#define PLAY_PACKAGE_USED_INDEX 0xdcc
//名字颜色
#define PLAY_OFFSET_NAME_COLOR 0xA14
//决斗值  [决斗名声]
#define PLAY_OFFSET_NAME_FIGHT 0xA0C
//猎杀值 
#define PLAY_OFFSET_NAME_HUNTING 0xA18
//杀戮值 名字变红
#define PLAY_OFFSET_NAME_HUNTING 0xA1C


//移动速度
#define PLAY_OFFSET_MOVE_SPEED 0x12f4

//跳跃等级
#define PLAY_OFFSET_JUMP_LVL 0x13d8

//释放速度
#define PLAY_OFFSET_JUMP_LVL 0x13f4

//释放距离
#define PLAY_OFFSET_JUMP_LVL 0x1430
//攻击速度
#define PLAY_OFFSET_JUMP_LVL 0x1328


//逆手
#define PLAY_OFFSET_NISHOU_ID 0x4dc
//风轮
#define PLAY_OFFSET_FENGLUN_ID 0x51c
//狂气
#define PLAY_OFFSET_KUANGQI_ID 0x53c

//背包偏移
#define PACKAGE_OFFSET 0xdc4

#define GET_PACKAGE_ITEM_CALL  0x0041FAD2 //E8 ?? ?? ?? ?? 83 3D ?? ?? ?? ?? 00 8B D8 74 7D 81 7D ?? A6 00 00 00 74 09 81 7D ?? A7 00 00 00 75 6B 

////周围对象最大值
//#define SURROUND_MAX_MEMBERS 0x00A5DA80 //8B 0D ?? ?? ?? ?? 89 75 E8 3B CE 0F 8E ?? ?? ?? ?? 8B 45 E8 8B 3C 85 ?? ?? ?? ?? B8 FF 00 00 00 66 39 87 ?? ?? ?? ?? 0F 83 ?? ?? ?? ?? 39 B3 ?? ?? ?? ?? 74 68 8B 87 ?? ?? ?? ?? 39 B0 ?? ?? ?? ?? 
////周围对象起始
//#define SURRDOUND_BASE 0xA58C60 //8B 3C 85 ?? ?? ?? ?? B8 FF 00 00 00 66 39 87 ?? ?? ?? ?? 0F 83 ?? ?? ?? ?? 39 B3 ?? ?? ?? ?? 74 68 8B 87 ?? ?? ?? ?? 39 B0 ?? ?? ?? ?? 74 5A 
////周围对象临时变量1
//#define SURROUND_TMP_1 0x009D7CB0//39 3D ?? ?? ?? ?? 75 0C 39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF 
////周围对象临时变量2
//#define SURROUND_TMP_2 0x00A5DB84//39 35 ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 83 4D EC FF D9 05 ?? ?? ?? ?? D9 5D E0 
//

//拾取对象
//#define PICK_TARGET_BASE 0x0084FCEC //A1 ?? ?? ?? ?? 8B 40 ?? 85 C0 0F 84 ?? ?? ?? ?? 8B 91 ?? ?? ?? ?? F6 42 ?? 08 
#define PICK_TARGET_OFFSET 0x20
#define PICK_TARGET_SURROUND 0xe4

//无视障碍物碰撞
//#define NO_COLLISIOR  0x00835814//39 05 ?? ?? ?? ?? 0F 94 C0 A3 ?? ?? ?? ?? E9 ?? ?? ?? ?? A1 ?? ?? ?? ?? 33 C9 39 88 ?? ?? ?? ?? 0F 94 C1 89 88 ?? ?? ?? ?? E9 ?? ?? ?? ?? 33 C0 33 F6 39 35 ?? ?? ?? ?? 68 6A 02 00 00 

#define UI_OBJECTS 0xc0
#define UI_WATER 0xc8
#define UI_TERRAIN 0xbc
#define UI_WEATHER 0xb8
#define UI_LOD_OBJECT 0xE0

//团技能buff状态起始地址  
//#define TEAM_BUFF_BASE 0xA58388  //89 3C B5 ?? ?? ?? ?? E8 ?? ?? ?? ?? 89 5D FC 33 C0 2B F0 89 45 E8 0F 84 ?? ?? ?? ?? 4E 0F 84 ?? ?? ?? ?? 4E 0F 84 ?? ?? ?? ?? 4E 
//团队合作
#define TEAM_BUFF_WORK 0x0
//幕间抽奖
#define TEAM_BUFF_DRAW 0x4
//幸运观众
#define TEAM_BUFF_LUCK 0xc
//剧团点数
//#define TEAM_POINT 0x00A58374 //FF 35 ?? ?? ?? ?? 8D 45 F0 57 50 E8 ?? ?? ?? ?? 83 C4 0C 68 4A 02 00 00 8B CE E8 ?? ?? ?? ?? 
//剧团等级
//#define TEAM_LEVEL 0x00A5836C //8B 0D ?? ?? ?? ?? 2B 8F ?? ?? ?? ?? 78 29 8B 4D D0 50 68 00 00 00 FF 83 C7 04 57 8B 7D DC 57 53 E8 ?? ?? ?? ?? 



//#define QUICK_USE_PARAM1 0x0096A570 //A1 ?? ?? ?? ?? 8B 80 ?? ?? ?? ?? 3B C7 7C 3C 
//#define QUICK_USE_PARAM2 0x00969CE8//BF ?? ?? ?? ?? 85 C0 74 5E 8D 45 C0 50 8D 45 B8 50 8D 45 BC 50 B9 ?? ?? ?? ??
//#define QUICK_USE_CALL 0x00483E60//E8 ?? ?? ?? ?? 8B 45 C0 8B 44 05 D4 C6 80 ?? ?? ?? ?? 00 81 45 BC A4 00 00 00 83 45 C0 04 81 7D BC 68 06 00 00 0F 8C ?? ?? ?? ?? 8B CB 
//
//
//#define  GET_ITEM_NAME_PARAM1 0x853198  //BE ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? 
//#define  GET_ITEM_NAME_CALL 0x004055B1 //E8 ?? ?? ?? ?? 8B 4D 08 8B D8 8D 43 ?? 50 E8 ?? ?? ?? ?? 83 65 FC 00 83 7B ?? 78 C7 45 F0 01 00 00 00 75 53 8B 8F ?? ?? ?? ??
//
//#define SELL_ITEM_PARAM1  0x845F60 //B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 00 6A 01 6A 00 6A 00 6A 04 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 1F 
//#define SELL_ITEM_CALL  0x004307A6 //E8 ?? ?? ?? ?? 6A 00 6A 01 6A 00 6A 00 6A 04 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 1F E8 ?? ?? ?? ?? 
//
//
//#define PICK_MONSTER_CALL 0x005B9EF3 //E8 ?? ?? ?? ?? 8B C6 5F 5E 5B C2 08 00 55 8B EC 83 EC 1C A1 ?? ?? ?? ?? 
//#define PICK_ITEM_CALL 0x0057C7FD //E8 ?? ?? ?? ?? 8A 43 ?? 88 43 ?? 33 C0 40 8B 4D FC 5F 5E 33 CD 5B E8 ?? ?? ?? ?? 
//
//
//#define ACTION_PARAM1 0x969CE8  //B9 ?? ?? ?? ?? 6A 03 E8 ?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ?? 
//#define ACTION_CALL 0x00484CA3 //E8 ?? ?? ?? ?? C2 0C 00 83 89 ?? ?? ?? ?? FF E9 ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 74 14 6A 00 FF B1 ?? ?? ?? ?? B9 ?? ?? ?? ?? 6A 0A 
//
//#define SKILL_PARAM1  0x0096A7CC //3B 3D ?? ?? ?? ?? 75 0A B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 47 ?? 2B 47 ?? 53 33 DB 
//#define SKILL_CALL  0x0047756B //E8 ?? ?? ?? ?? E9 ?? ?? ?? ?? 83 F8 0B 0F 85 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 70 ?? E8 ?? ?? ?? ?? FF 73 ?? 8B 48 ?? E8 ?? ?? ?? ?? 8B 4B ?? 83 C1 FB 83 F9 01 
//
//
//#define ADD_POINT_PARAM1  0x845F60 //B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 00 8B CE E8 ?? ?? ?? ?? 5E C3 6A 04 
//#define ADD_POINT_CALL 0x004312F0 //E8 ?? ?? ?? ?? 6A 00 8B CE E8 ?? ?? ?? ?? 5E C3 6A 04 B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F1 89 75 F0 E8 ?? ?? ?? ?? 83 65 FC 00 
//
//#define TEAM_SKILL_PARAM1  0x845F60 //B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 43 ?? 8B 0D ?? ?? ?? ?? 8B 55 F0 89 8C 82 ?? ?? ?? ?? E9 ?? ?? ?? ?? 51 51 68 E2 07 00 00 
//#define TEAM_SKILL_CALL 0x42D8FC //E8 ?? ?? ?? ?? 8B 43 ?? 8B 0D ?? ?? ?? ?? 8B 55 F0 89 8C 82 ?? ?? ?? ?? E9 ?? ?? ?? ?? 51 51 68 E2 07 00 00 
