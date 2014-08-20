
#include "gamefunc.h"

namespace GameFunc
{
	const   INT64 s_BotLevelMoney[] = 
	{
		30000,
		50000,
		100000,
		300000,
		1000000,
		3000000,
		10000000,
		30000000,
		100000000,
		500000000,
		1500000000
	};
	bool IsRightBotLevel(int BotLevel)
	{
		return BotLevel>0 && BotLevel<=10;
	}
	INT64 GetBotLevelMoney(int nLevel)
	{
		if ( nLevel>=0 && nLevel<=10 )
		{
			return s_BotLevelMoney[nLevel];
		}
		return s_BotLevelMoney[0];
	}
}
