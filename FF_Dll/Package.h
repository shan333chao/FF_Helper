#pragma once
#include "ff_offset.h"
class Package
{

public:
	DWORD packageBase{};
	DWORD packageIndexMax;
	DWORD InitPackage(DWORD base);
	DWORD getPackageItem(DWORD index);
	DWORD QuickUse(DWORD index);
	DWORD GetItemName(DWORD itemObj);
	DWORD GetItemName2(DWORD itemObj);
	DWORD SellItem(DWORD itemObj);
private:
	DWORD dw_QUICK_USE_PARAM1= 0;
	DWORD dw_QUICK_USE_PARAM2= 0;
	DWORD dw_QUICK_USE_PARAM3 = 0;
	
	DWORD dw_QUICK_USE_CALL= 0;

	DWORD dw_GET_ITEM_NAME_PARAM1 = GET_ITEM_NAME_PARAM1;
	DWORD dw_GET_ITEM_NAME_CALL = GET_ITEM_NAME_CALL;

	DWORD dw_SELL_ITEM_PARAM1 =0;
	DWORD dw_SELL_ITEM_CALL = 0;


 
};

