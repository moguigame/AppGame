#pragma once

#include <string.h>

namespace GameFunc
{
	typedef unsigned int					UINT;
	typedef unsigned long					DWORD;
	typedef	char							CHAR;
	typedef short							SHORT;
	typedef long							LONG;
	typedef int								BOOL;

	typedef unsigned char                   BYTE;
	typedef unsigned short					WORD;

	typedef signed char                     INT8;
	typedef signed short                    INT16;
	typedef signed int                      INT32;
	typedef signed long long                INT64;

	typedef unsigned char                   UINT8;
	typedef unsigned short                  UINT16;
	typedef unsigned int                    UINT32;
	typedef unsigned long long              UINT64;

	extern bool           IsRightBotLevel(int BotLevel);
	extern long long      GetBotLevelMoney(int nLevel);
}

