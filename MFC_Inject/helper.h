#pragma once
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <atlconv.h>
#include <iostream>
#include <stdlib.h>  // for wcstombs
#include <wctype.h>  // for towlower
#include <wchar.h>   // for wchar_t
#include <stdio.h>   
std::string get_last_error(DWORD errCode);


int wcscasecmp(const wchar_t* cs, const wchar_t* ct);
std::string rand_str(const int len);