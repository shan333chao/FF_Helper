#pragma once
#include "SignaturesDataSingleton.h"
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
	DWORD dw_QUICK_USE_PARAM1= SignaturesDataSingleton::getInstance().GetAddressByKey("QUICK_USE_PARAM1");
	DWORD dw_QUICK_USE_PARAM2= SignaturesDataSingleton::getInstance().GetAddressByKey("QUICK_USE_PARAM2");
	DWORD dw_QUICK_USE_PARAM3 = SignaturesDataSingleton::getInstance().GetAddressByKey("QUICK_USE_PARAM3");
	
	DWORD dw_QUICK_USE_CALL= SignaturesDataSingleton::getInstance().GetAddressByKey("QUICK_USE_CALL");

	DWORD dw_GET_ITEM_NAME_PARAM1 = SignaturesDataSingleton::getInstance().GetAddressByKey("GET_ITEM_NAME_PARAM1");
	DWORD dw_GET_ITEM_NAME_CALL = SignaturesDataSingleton::getInstance().GetAddressByKey("GET_ITEM_NAME_CALL");

	DWORD dw_SELL_ITEM_PARAM1 = SignaturesDataSingleton::getInstance().GetAddressByKey("SELL_ITEM_PARAM1");
	DWORD dw_SELL_ITEM_CALL = SignaturesDataSingleton::getInstance().GetAddressByKey("SELL_ITEM_CALL");


 
};

