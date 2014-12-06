#pragma once

#include <iostream>
#include <stdint.h>

#include "MoGuiGame.h"

namespace MoGui
{
namespace MoGuiXY
{
namespace GameXY
{
namespace DeZhou
{
	using namespace MoGui::Game::DeZhou;

	const short  GameXYID_Start                         = 0;

	const short  GameXYID_Ready	  					    = GameXYID_Start+1;
	const short  GameXYID_GameStart                     = GameXYID_Start+2;
	const short  GameXYID_GameState                     = GameXYID_Start+3;
	const short  GameXYID_BankerInfo                    = GameXYID_Start+4;
	const short  GameXYID_StartInfo                     = GameXYID_Start+5;
	const short  GameXYID_HandPai                       = GameXYID_Start+6;
	const short  GameXYID_TableRule                     = GameXYID_Start+7;

	const short  GameXYID_GamePlayerInfo                = GameXYID_Start+10;           //和当前局游戏紧密相关
	const short  GameXYID_TablePai                      = GameXYID_Start+11;
	const short  GameXYID_SitPlayerInfo                 = GameXYID_Start+12;           //玩家属性和游戏数据关系不大
	const short  GameXYID_WashOutPlayerInfo             = GameXYID_Start+13;
	
	const short  GameXYID_ReqAddMoney                   = GameXYID_Start+14;
	const short  GameXYID_RespAddMoney                  = GameXYID_Start+15;
	const short  GameXYID_TableMoney                    = GameXYID_Start+16;
	const short  GameXYID_GameMoney                     = GameXYID_Start+17;
	const short  GameXYID_AddMoneyInfo                  = GameXYID_Start+18;
	
	const short  GameXYID_ReqAddChip                    = GameXYID_Start+21;
	const short  GameXYID_RespAddChip                   = GameXYID_Start+22;
	const short  GameXYID_AddChipInfo                   = GameXYID_Start+23;	
	const short  GameXYID_PlayerTurn                    = GameXYID_Start+24;           //轮到谁了
	const short  GameXYID_PlayerPower                   = GameXYID_Start+25;           //权限是什么

	const short  GameXYID_ReqShowPai                    = GameXYID_Start+27;
	const short  GameXYID_ShowPai                       = GameXYID_Start+28;

	const short  GameXYID_MoneyPool                     = GameXYID_Start+30;
	const short  GameXYID_Result                        = GameXYID_Start+31;
	const short  GameXYID_ReLink                        = GameXYID_Start+32;

	//比赛部分
	const short  GameXYID_MatchBlind                    = GameXYID_Start+41;
	const short  GameXYID_MatchResult                   = GameXYID_Start+42;
	const short  GameXYID_MatchWaitTable                = GameXYID_Start+43;           //等待配桌
	const short  GameXYID_PlayerRank                    = GameXYID_Start+44;

	const short  GameXYID_Flag                          = GameXYID_Start+51;

	const short  GameXYID_TableChat                     = GameXYID_Start+101;
	const short  GameXYID_PrivateChat                   = GameXYID_Start+102;
	const short  GameXYID_TableQuickChat                = GameXYID_Start+103;

	const short  GameXYID_ReqShowFace                   = GameXYID_Start+111;
	const short  GameXYID_RespShowFace                  = GameXYID_Start+112;
	const short  GameXYID_ReqAddFriend                  = GameXYID_Start+116;
	const short  GameXYID_RespAddFriend                 = GameXYID_Start+117;
	const short  GameXYID_AddFriendInfo                 = GameXYID_Start+118;

	const short  GameXYID_ReqSendMoney                  = GameXYID_Start+121;
	const short  GameXYID_RespSendMoney                 = GameXYID_Start+122;

	const short  GameXYID_SendGift                      = GameXYID_Start+131;
	const short  GameXYID_RespSendGift                  = GameXYID_Start+132;
	
	const short  GameXYID_ReqAddTableTime               = GameXYID_Start+136;
	const short  GameXYID_RespAddTableTime              = GameXYID_Start+137;
	const short  GameXYID_LeftTime                      = GameXYID_Start+138;
	const short  GameXYID_TableEnd                      = GameXYID_Start+139;

	const short  GameXYID_TimePromote                   = GameXYID_Start+141;
	const short  GameXYID_ReqTimePromote                = GameXYID_Start+142;
	const short  GameXYID_RespTimePromote               = GameXYID_Start+143;

	const short  GameXYID_TableWinLoss                  = GameXYID_Start+151;

	const short  GameXYID_ReqJoinJuBaoPeng              = GameXYID_Start+161;
	const short  GameXYID_RespJoinJuBaoPeng             = GameXYID_Start+162;
	const short  GameXYID_JoinJuBaoPengInfo             = GameXYID_Start+163;
	const short  GameXYID_WinJuBaoPeng                  = GameXYID_Start+164;

	const short  GameXYID_ReqTailInfo                   = GameXYID_Start+201;

	const short  GameXYID_End                           = GameXYID_Start+250;

	struct GameDeZhou_Ready
	{
		enum { XY_ID = GameXYID_Ready };

		unsigned char       m_MySitID;

		GameDeZhou_Ready() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_Ready& rd )
		{
			bos << rd.m_MySitID;			
			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_Ready& rd )
		{
			rd.ReSet();
			
			bis >> rd.m_MySitID;

			return bis;
		}
	};

	struct GameDeZhou_Start
	{
		enum { XY_ID = GameXYID_GameStart };

		GameDeZhou_Start() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const GameDeZhou_Start& st )
		{
			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_Start& st )
		{
			return bis;
		}
	};

	struct msgStartPlayerInfo
	{
		unsigned char          m_SitID;
		unsigned short         m_Action;
		GS::stHandPai          m_HandPai;
		SBigNumber             m_nServiceMoney;
		SBigNumber             m_nChip;
		SBigNumber             m_nTableMoney;

		msgStartPlayerInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const msgStartPlayerInfo& spi )
		{
			bos << spi.m_SitID;
			bos << spi.m_Action;
			bos << spi.m_HandPai.m_Pai[0];
			bos << spi.m_HandPai.m_Pai[1];
			bos << spi.m_nServiceMoney;
			bos << spi.m_nChip;
			bos << spi.m_nTableMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, msgStartPlayerInfo& spi )
		{
			spi.ReSet();

			bis >> spi.m_SitID;
			bis >> spi.m_Action;
			bis >> spi.m_HandPai.m_Pai[0];
			bis >> spi.m_HandPai.m_Pai[1];
			bis >> spi.m_nServiceMoney;
			bis >> spi.m_nChip;
			bis >> spi.m_nTableMoney;

			return bis;
		}
	};

	struct GameDeZhou_StartInfo
	{
		enum { XY_ID = GameXYID_StartInfo };

		mutable BYTE                 m_nPlayerCount;
		msgStartPlayerInfo           m_PlayerStartInfo[MAX_PALYER_ON_TABLE];

		GameDeZhou_StartInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const GameDeZhou_StartInfo& si )
		{
			si.m_nPlayerCount = 0;
			for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
			{
				if ( si.m_PlayerStartInfo[Sit].m_SitID > 0 )
				{
					si.m_nPlayerCount++;
				}
			}
			bos << si.m_nPlayerCount;
			for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
			{
				if ( si.m_PlayerStartInfo[Sit].m_SitID > 0 )
				{
					bos << si.m_PlayerStartInfo[Sit];
				}
			}
			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_StartInfo& si )
		{
			bis >> si.m_nPlayerCount;
			if ( si.m_nPlayerCount > MAX_PALYER_ON_TABLE )
			{
				throw agproexception(agproexception::rangeerror);
			}			
			assert(si.m_nPlayerCount<=MAX_PALYER_ON_TABLE);
			for (int Sit=0;Sit<MAX_PALYER_ON_TABLE&&Sit<si.m_nPlayerCount;Sit++)
			{
				bis >> si.m_PlayerStartInfo[Sit];
			}
			return bis;
		}
	};

	struct GameDeZhou_HandPai
	{
		enum { XY_ID = GameXYID_HandPai };

		unsigned char       m_SitID;
		GS::stHandPai       m_HandPai;

		GameDeZhou_HandPai() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_HandPai& src)
		{
			bos << src.m_SitID;
			bos << src.m_HandPai.m_Pai[0];
			bos << src.m_HandPai.m_Pai[1];
		
			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_HandPai& src )
		{
			src.ReSet();

			bis >> src.m_SitID;
			bis >> src.m_HandPai.m_Pai[0];
			bis >> src.m_HandPai.m_Pai[1];

			return bis;
		}
	};

	struct GameDeZhou_TableRule
	{
		enum { XY_ID = GameXYID_TableRule };

		std::string         m_strRule;

		GameDeZhou_TableRule() { ReSet(); }
		void ReSet()
		{
			m_strRule = "";
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_TableRule& src )
		{
			InString(bos,src.m_strRule,MAX_RULE_SIZE);
			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_TableRule& src )
		{
			src.ReSet();
			OutString(bis,src.m_strRule,MAX_RULE_SIZE);
			return bis;
		}
	};

	struct GameDeZhou_BankerInfo
	{
		enum { XY_ID = GameXYID_BankerInfo };

		unsigned char       m_BankerSitID;
		unsigned char       m_BigBlindSitID;
		unsigned char       m_SmallBlindSitID;

		GameDeZhou_BankerInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_BankerInfo& bi )
		{
			bos << bi.m_BankerSitID;
			bos << bi.m_BigBlindSitID;
			bos << bi.m_SmallBlindSitID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_BankerInfo& bi )
		{
			bi.ReSet();

			bis >> bi.m_BankerSitID;
			bis >> bi.m_BigBlindSitID;
			bis >> bi.m_SmallBlindSitID;

			return bis;
		}
	};

	struct GameDeZhou_Flag
	{
		enum { XY_ID = GameXYID_Flag };
		enum
		{
			Success,
			ShowPai = 1,                  //通知玩家可以亮牌
			TakeMoneyOutTime = 3,         //玩家带钱超时
			NoMoneyForMatch = 5,          //没有足够的报名费
		};

		unsigned char       m_Flag;

		GameDeZhou_Flag() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_Flag& fl )
		{
			bos << fl.m_Flag;			
			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_Flag& fl )
		{
			fl.ReSet();
			bis >> fl.m_Flag;
			return bis;
		}
	};

	struct GameDeZhou_GameState
	{
		enum { XY_ID = GameXYID_GameState };

		unsigned char       m_GameST;

		GameDeZhou_GameState() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_GameState& gs )
		{
			bos << gs.m_GameST;			
			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_GameState& gs )
		{
			gs.ReSet();
			bis >> gs.m_GameST;
			return bis;
		}
	};
	
	struct msgGamePlayerInfo
	{
		unsigned char          m_SitID;
		unsigned char          m_GameState;
		unsigned short         m_Action;
		GS::stHandPai          m_HandPai;
		SBigNumber             m_nTableMoney;
		SBigNumber             m_nChip;

		msgGamePlayerInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const msgGamePlayerInfo& gpi )
		{
			bos << gpi.m_SitID;
			bos << gpi.m_GameState;
			bos << gpi.m_Action;
			bos << gpi.m_HandPai.m_Pai[0];
			bos << gpi.m_HandPai.m_Pai[1];
			bos << gpi.m_nTableMoney;
			bos << gpi.m_nChip;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, msgGamePlayerInfo& gpi )
		{
			gpi.ReSet();

			bis >> gpi.m_SitID;
			bis >> gpi.m_GameState;
			bis >> gpi.m_Action;
			bis >> gpi.m_HandPai.m_Pai[0];
			bis >> gpi.m_HandPai.m_Pai[1];
			bis >> gpi.m_nTableMoney;
			bis >> gpi.m_nChip;

			return bis;
		}

		msgGamePlayerInfo( const msgGamePlayerInfo& gpi)
		{
			Copy(gpi);
		}

		msgGamePlayerInfo& operator=( const msgGamePlayerInfo& gpi)
		{
			if(this == &gpi)
			{
				return *this;
			}

			Copy(gpi);

			return *this;
		}

		void Copy( const msgGamePlayerInfo& gpi )
		{
			this->m_SitID         = gpi.m_SitID;
			this->m_GameState     = gpi.m_GameState;
			this->m_Action        = gpi.m_Action;
			this->m_HandPai       = gpi.m_HandPai;
			this->m_nTableMoney   = gpi.m_nTableMoney;
			this->m_nChip         = gpi.m_nChip;
		}
	};

	struct GameDeZhou_GamePlayerInfo
	{
		enum { XY_ID = GameXYID_GamePlayerInfo };
		
		mutable BYTE        m_nPlayerCount;
		msgGamePlayerInfo   m_GamePlayerInfo[MAX_PALYER_ON_TABLE];

		GameDeZhou_GamePlayerInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_GamePlayerInfo& gpi )
		{			
			gpi.m_nPlayerCount = 0;
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if( gpi.m_GamePlayerInfo[i].m_SitID > 0 )
				{
					gpi.m_nPlayerCount++;
				}
			}
			bos << gpi.m_nPlayerCount;
			for(int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if( gpi.m_GamePlayerInfo[i].m_SitID > 0 )
				{					
					bos << gpi.m_GamePlayerInfo[i];
				}
			}

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_GamePlayerInfo& gpi )
		{
			gpi.ReSet();
			
			bis >> gpi.m_nPlayerCount;
			if ( gpi.m_nPlayerCount > MAX_PALYER_ON_TABLE )
			{
				throw agproexception(agproexception::rangeerror);
			}
			assert( gpi.m_nPlayerCount <= MAX_PALYER_ON_TABLE );
			for(int i=0;i<gpi.m_nPlayerCount && i<MAX_PALYER_ON_TABLE;i++)
			{
				bis >> gpi.m_GamePlayerInfo[i];
			}

			return bis;
		}
	};

	struct GameDeZhou_TablePai
	{
		enum { XY_ID = GameXYID_TablePai };
		enum { FLOP,TURN,RIVER };

        unsigned char       m_PaiCount;
		unsigned char       m_TablePai[TABLE_CARDS];

		GameDeZhou_TablePai() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const GameDeZhou_TablePai& tp )
		{
			assert( tp.m_PaiCount && tp.m_PaiCount <= TABLE_CARDS );
			
			bos << tp.m_PaiCount;
			for(int i=0;i<tp.m_PaiCount&&i<TABLE_CARDS;i++)
			{
				bos << tp.m_TablePai[i];
			}	

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_TablePai& tp )
		{
			tp.ReSet();
			
			bis >> tp.m_PaiCount;
			for(int i=0;i<tp.m_PaiCount&&i<TABLE_CARDS;i++)
			{
				bis >> tp.m_TablePai[i];
			}

			return bis;
		}
	};

	struct GameDeZhou_SitPlayerInfo
	{
		enum { XY_ID = GameXYID_SitPlayerInfo };
		
		unsigned char              m_SitID;
		short                      m_GiftID;
		unsigned int               m_PID;		

		SBigNumber                 m_nJF;                                     //积分
		SBigNumber                 m_nExperience;                             //这个可以用消费数的计量，也用于等级的计算
	
		SBigNumber                 m_nWinTimes;
		SBigNumber                 m_nLossTimes;
		long long                  m_MaxPai;

		BYTE                       m_GameLevel;
		BYTE                       m_VipLevel;
		unsigned char              m_Sex;

		std::string                m_HomePageURL;
	
		GameDeZhou_SitPlayerInfo() { ReSet(); }
		void ReSet()
		{
			m_SitID  = 0;
			m_GiftID  = 0;
			m_PID  = 0;

			m_nJF  = 0;
			m_nExperience  = 0;

			m_nWinTimes  = 0;
			m_nLossTimes  = 0;
			m_MaxPai  = 0;

			m_GameLevel = 0;
			m_VipLevel = 0;
			m_Sex = 0;

			m_HomePageURL  = "";
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_SitPlayerInfo& spi )
		{
			bos << spi.m_SitID;
			bos << spi.m_GiftID;
			bos << spi.m_PID;

			bos << spi.m_nJF;
			bos << spi.m_nExperience;
			
			bos << spi.m_nWinTimes;
			bos << spi.m_nLossTimes;
			bos << spi.m_MaxPai;

			bos << spi.m_GameLevel;
			bos << spi.m_VipLevel;
			bos << spi.m_Sex;

			InString(bos,spi.m_HomePageURL,MAX_URL_SIZE);

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_SitPlayerInfo& spi )
		{
			spi.ReSet();
			
			bis >> spi.m_SitID;
			bis >> spi.m_GiftID;
			bis >> spi.m_PID;			

			bis >> spi.m_nJF;
			bis >> spi.m_nExperience;

			bis >> spi.m_nWinTimes;
			bis >> spi.m_nLossTimes;
			bis >> spi.m_MaxPai;

			bis >> spi.m_GameLevel;
			bis >> spi.m_VipLevel;
			bis >> spi.m_Sex;

			OutString(bis,spi.m_HomePageURL,MAX_URL_SIZE);

			return bis;
		}
	};

	struct GameDeZhou_WashOutPlayerInfo
	{
		enum { XY_ID = GameXYID_WashOutPlayerInfo };

		unsigned char             m_SitID;
		unsigned int              m_PID;
		short                     m_Position;
		std::string               m_NickName;
		std::string               m_HeadPicURL;

		GameDeZhou_WashOutPlayerInfo() { ReSet(); }
		void ReSet()
		{
			m_SitID = 0;
			m_PID = 0;
			m_Position = 0;
			m_NickName = "";
			m_HeadPicURL = "";
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_WashOutPlayerInfo& src )
		{
			bos << src.m_SitID;
			bos << src.m_PID;
			bos << src.m_Position;
			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicURL,MAX_URL_SIZE);

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_WashOutPlayerInfo& src )
		{
			src.ReSet();

			bis >> src.m_SitID;
			bis >> src.m_PID;
			bis >> src.m_Position;
			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicURL,MAX_URL_SIZE);

			return bis;
		}
	};

	struct GameDeZhou_ReqAddMoney
	{
		enum { XY_ID = GameXYID_ReqAddMoney };

		unsigned char             m_SitID;
		SBigNumber                m_MinAdd;
		SBigNumber                m_MaxAdd;

		GameDeZhou_ReqAddMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqAddMoney& am )
		{
			bos << am.m_SitID;
			bos << am.m_MinAdd;
			bos << am.m_MaxAdd;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqAddMoney& am )
		{
			am.ReSet();

			bis >> am.m_SitID;
			bis >> am.m_MinAdd;
			bis >> am.m_MaxAdd;

			return bis;
		}
	};
	struct GameDeZhou_RespAddMoney
	{
		enum { XY_ID = GameXYID_RespAddMoney };

		unsigned char             m_SitID;
		SBigNumber                m_AddMoney;

		GameDeZhou_RespAddMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_RespAddMoney& am )
		{
			bos << am.m_SitID;
			bos << am.m_AddMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_RespAddMoney& am )
		{
			am.ReSet();

			bis >> am.m_SitID;
			bis >> am.m_AddMoney;

			return bis;
		}
	};
	struct GameDeZhou_AddMoneyInfo
	{
		enum { XY_ID = GameXYID_AddMoneyInfo };
		enum 
		{
			SUCCESS,
			UNSUCCESS,
			OUTMONEY,      //游戏币不足
			FULLMONEY,     //你不需要再加游戏币
			PLAYING,       //游戏中
			MATCH,         //比赛中
			TAKELESSMONEY, //加的钱太少了
		};

		unsigned char             m_Flag;
		SBigNumber                m_AddMoney;
		SBigNumber                m_TableMoney;
		SBigNumber                m_GameMoney;

		GameDeZhou_AddMoneyInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_AddMoneyInfo& src )
		{
			bos << src.m_Flag;
			if ( src.m_Flag == SUCCESS )
			{
				bos << src.m_AddMoney;
				bos << src.m_TableMoney;
				bos << src.m_GameMoney;
			}

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_AddMoneyInfo& src )
		{
			src.ReSet();

			bis >> src.m_Flag;
			if (src.m_Flag == SUCCESS)
			{
				bis >> src.m_AddMoney;
				bis >> src.m_TableMoney;
				bis >> src.m_GameMoney;
			}
			
			return bis;
		}
	};

	struct GameDeZhou_TableMoney
	{
		enum { XY_ID = GameXYID_TableMoney };

		unsigned char             m_SitID;
		SBigNumber                m_nTableMoney;

		GameDeZhou_TableMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_TableMoney& lm )
		{
			bos << lm.m_SitID;
			bos << lm.m_nTableMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_TableMoney& lm )
		{
			lm.ReSet();

			bis >> lm.m_SitID;
			bis >> lm.m_nTableMoney;

			return bis;
		}
	};

	struct GameDeZhou_GameMoney
	{
		enum { XY_ID = GameXYID_GameMoney };

		unsigned char             m_SitID;
		SBigNumber                m_nGameMoney;

		GameDeZhou_GameMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_GameMoney& gm )
		{
			bos << gm.m_SitID;
			bos << gm.m_nGameMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_GameMoney& gm )
		{
			gm.ReSet();

			bis >> gm.m_SitID;
			bis >> gm.m_nGameMoney;

			return bis;
		}
	};

	struct GameDeZhou_MoneyPool
	{
		enum { XY_ID = GameXYID_MoneyPool };

		unsigned char       m_MoneyPoolCount;
		SBigNumber          m_MoneyPools[MAX_PALYER_ON_TABLE];

		GameDeZhou_MoneyPool() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_MoneyPool& mp )
		{
			bos << mp.m_MoneyPoolCount;
			for ( int i=0;i<mp.m_MoneyPoolCount&&i<MAX_PALYER_ON_TABLE;i++)
			{
				bos << mp.m_MoneyPools[i];
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_MoneyPool& mp )
		{
			mp.ReSet();

			bis >> mp.m_MoneyPoolCount;
			for (int i=0;i<mp.m_MoneyPoolCount&&i<MAX_PALYER_ON_TABLE;i++)
			{
				bis >> mp.m_MoneyPools[i];
			}
			return bis;
		}
	};

	struct GameDeZhou_ReqAddChip
	{
		enum { XY_ID = GameXYID_ReqAddChip };

		unsigned char       m_CurPlayerSitID;
		unsigned char       m_ChipAction;

		SBigNumber          m_nCurChipMoney;

		GameDeZhou_ReqAddChip() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqAddChip& ac )
		{
			bos << ac.m_CurPlayerSitID;
			bos << ac.m_ChipAction;
			bos << ac.m_nCurChipMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqAddChip& ac )
		{
			ac.ReSet();

			bis >> ac.m_CurPlayerSitID;
			bis >> ac.m_ChipAction;
			bis >> ac.m_nCurChipMoney;
		
			return bis;
		}
	};

	struct GameDeZhou_RespAddChip
	{
		enum { XY_ID = GameXYID_RespAddChip };

		unsigned char       m_Flag;

		GameDeZhou_RespAddChip() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_RespAddChip& ac )
		{
			bos << ac.m_Flag;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_RespAddChip& ac )
		{
			ac.ReSet();

			bis >> ac.m_Flag;

			return bis;
		}
	};

	struct GameDeZhou_AddChipInfo
	{
		enum { XY_ID = GameXYID_AddChipInfo };

		unsigned char       m_CurPlayerSitID;
		unsigned short      m_ChipAction;
		SBigNumber          m_nCurChipMoney;
		SBigNumber          m_nTableMoney;

		GameDeZhou_AddChipInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_AddChipInfo& aci )
		{
			bos << aci.m_CurPlayerSitID;
			bos << aci.m_ChipAction;
			bos << aci.m_nCurChipMoney;
			bos << aci.m_nTableMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_AddChipInfo& aci )
		{
			aci.ReSet();

			bis >> aci.m_CurPlayerSitID;
			bis >> aci.m_ChipAction;
			bis >> aci.m_nCurChipMoney;
			bis >> aci.m_nTableMoney;

			return bis;
		}
	};
	
	struct GameDeZhou_PlayerTurn
	{
		enum { XY_ID = GameXYID_PlayerTurn };

		unsigned char      m_SitID;
		unsigned char      m_LeftTime;

		GameDeZhou_PlayerTurn() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_PlayerTurn& pt )
		{
			bos << pt.m_SitID;
			bos << pt.m_LeftTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_PlayerTurn& pt )
		{
			pt.ReSet();

			bis >> pt.m_SitID;
			bis >> pt.m_LeftTime;

			return bis;
		}
	};

	struct GameDeZhou_PlayerPower
	{
		enum { XY_ID = GameXYID_PlayerPower };

		unsigned char      m_ActionFlag;
		SBigNumber         m_nMaxChip;
		SBigNumber         m_nMinAddChip;
		SBigNumber         m_nTopChip;

		GameDeZhou_PlayerPower() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_PlayerPower& pp )
		{
			bos << pp.m_ActionFlag;
			bos << pp.m_nMaxChip;
			bos << pp.m_nMinAddChip;
			bos << pp.m_nTopChip;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_PlayerPower& pp )
		{
			pp.ReSet();

			bis >> pp.m_ActionFlag;
			bis >> pp.m_nMaxChip;
			bis >> pp.m_nMinAddChip;
			bis >> pp.m_nTopChip;

			return bis;
		}
	};	

	struct msgPlayerWinMoney
	{
		unsigned char           m_SitID;
		SBigNumber              m_nWinMoney;

		msgPlayerWinMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		void SetPlayerWinMoney(BYTE sitid,long long winmoney)
		{
			if (sitid>0 && sitid<=MAX_PALYER_ON_TABLE && winmoney>=0)
			{
				m_SitID     = sitid;
				m_nWinMoney = winmoney;
			}
			else
			{
				printf_s("msgPlayerWinMoney Error \n");				
			}
		}
	};

	struct msgMoneyPool
	{
		SBigNumber      		m_nMoney;
		mutable BYTE            m_nPlayerCount;
		msgPlayerWinMoney       m_PlayersInPool[MAX_PALYER_ON_TABLE];

		msgMoneyPool() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const msgMoneyPool& mp )
		{
			bos << mp.m_nMoney;

			mp.m_nPlayerCount = 0;
			for(int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if (mp.m_PlayersInPool[i].m_SitID > 0)
				{
					mp.m_nPlayerCount++;
				}
			}
			assert(mp.m_nPlayerCount>0);			
			bos << mp.m_nPlayerCount;
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if( mp.m_PlayersInPool[i].m_SitID > 0 )
				{
					bos << mp.m_PlayersInPool[i].m_SitID;
					bos << mp.m_PlayersInPool[i].m_nWinMoney;
				}
			}

			return bos;
		}

		friend bistream& operator>>( bistream& bis, msgMoneyPool& mp )
		{
			mp.ReSet();

			bis >> mp.m_nMoney;
			bis >> mp.m_nPlayerCount;
			assert(mp.m_nPlayerCount<=MAX_PALYER_ON_TABLE);
			for(int i=0;i<mp.m_nPlayerCount&&i<MAX_PALYER_ON_TABLE;i++)
			{
				bis >> mp.m_PlayersInPool[i].m_SitID;
				bis >> mp.m_PlayersInPool[i].m_nWinMoney;
			}

			return bis;
		}

		msgMoneyPool( const msgMoneyPool& mp)
		{
			Copy(mp);
		}

		msgMoneyPool& operator=( const msgMoneyPool& mp)
		{
			if(this == &mp)
			{
				return *this;
			}

			Copy(mp);

			return *this;
		}

		void Copy( const msgMoneyPool& mp )
		{
			this->m_nMoney              = mp.m_nMoney;
			this->m_nPlayerCount        = mp.m_nPlayerCount;
			memcpy(m_PlayersInPool,mp.m_PlayersInPool,sizeof(m_PlayersInPool));
		}
	};

	struct msgResultPai
	{
		unsigned char           m_SitID;
		unsigned char           m_HandPai[2];
		unsigned char           m_PaiType[6];
		unsigned char           m_bWin;

		msgResultPai() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}
		bool IsShowPai()const
		{
			if ( m_HandPai[0]>PAI_VALUE_NONE && m_HandPai[0]<PAI_VALUE_BACK 
				&& m_HandPai[1]>PAI_VALUE_NONE && m_HandPai[1]<PAI_VALUE_BACK )
			{
				return true;
			}
			return false;
		}
		void SetPai(BYTE pai1,BYTE pai2)
		{
			m_HandPai[0] = pai1;
			m_HandPai[1] = pai2;
		}
		friend bostream& operator<<( bostream& bos, const msgResultPai& rp )
		{
			bos << rp.m_SitID;
			bos << rp.m_HandPai[0];
			bos << rp.m_HandPai[1];
			for (int i=0;i<6;i++)
			{
				bos << rp.m_PaiType[i];
			}
			bos << rp.m_bWin;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, msgResultPai& rp )
		{
			rp.ReSet();

			bis >> rp.m_SitID;
			bis >> rp.m_HandPai[0];
			bis >> rp.m_HandPai[1];
			for (int i=0;i<6;i++)
			{
				bis >> rp.m_PaiType[i];
			}
			bis >> rp.m_bWin;

			return bis;
		}

		msgResultPai( const msgResultPai& src)
		{
			Copy(src);
		}
		msgResultPai& operator=( const msgResultPai& src)
		{
			if(this == &src)
			{
				return *this;
			}

			Copy(src);

			return *this;
		}
		void Copy( const msgResultPai& src )
		{
			this->m_SitID           = src.m_SitID;
			this->m_bWin            = src.m_bWin;
			memcpy(m_HandPai,src.m_HandPai,sizeof(m_HandPai));
			memcpy(m_PaiType,src.m_PaiType,sizeof(m_PaiType));
		}
	};

	struct GameDeZhou_Result
	{
		enum { XY_ID = GameXYID_Result };
		
		mutable BYTE        m_PlayerCount;
		msgResultPai        m_PlayerHandPai[MAX_PALYER_ON_TABLE];

		mutable BYTE        m_MoneyPoolCount;
		msgMoneyPool        m_MoneyPools[MAX_PALYER_ON_TABLE];

		GameDeZhou_Result() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		void TidyUp()
		{
			m_PlayerCount = 0;
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if (m_PlayerHandPai[i].IsShowPai())
				{
					m_PlayerCount++;
				}
			}

			int StartIdx = -1;
			int EndIdx = -1;
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if ( m_PlayerHandPai[i].IsShowPai() == false )
				{
					StartIdx = i;
				}
				else
				{
					continue;
				}

				for (int j=i+1;j<MAX_PALYER_ON_TABLE;j++)
				{
					if ( m_PlayerHandPai[j].IsShowPai() )
					{
						EndIdx = j;
						break;
					}
				}
				if ( StartIdx != -1)
				{
					if ( EndIdx != -1 )
					{
						m_PlayerHandPai[StartIdx] = m_PlayerHandPai[EndIdx];
						m_PlayerHandPai[EndIdx].ReSet();

						StartIdx = EndIdx = -1;
					}
					else
					{
						break;
					}
				}
			}

			m_MoneyPoolCount = 0;
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if (m_MoneyPools[i].m_nMoney>0)
				{
					m_MoneyPoolCount++;
				}
			}
			StartIdx = EndIdx = -1;
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if ( m_MoneyPools[i].m_nMoney == 0 )
				{
					StartIdx = i;
				}
				else
				{
					continue;
				}

				for (int j=i+1;j<MAX_PALYER_ON_TABLE;j++)
				{
					if ( m_MoneyPools[j].m_nMoney > 0 )
					{
						EndIdx = j;
						break;
					}
				}
				if ( StartIdx != -1)
				{
					if ( EndIdx != -1 )
					{
						m_MoneyPools[StartIdx] = m_MoneyPools[EndIdx];
						m_MoneyPools[EndIdx].ReSet();

						StartIdx = EndIdx = -1;
					}
					else
					{
						break;
					}
				}
			}            
		}

		friend bostream& operator<<( bostream& bos, GameDeZhou_Result& res )
		{
			bos << res.m_PlayerCount;
			for(int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if (res.m_PlayerHandPai[i].IsShowPai())
				{
					bos << res.m_PlayerHandPai[i];
				}
			}

			bos << res.m_MoneyPoolCount;
			for(int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if (res.m_MoneyPools[i].m_nMoney>0)
				{
					bos << res.m_MoneyPools[i];
				}				
			}

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_Result& res )
		{
			res.ReSet();
			
			bis >> res.m_PlayerCount;
			for(int i=0;i<res.m_PlayerCount&&i<MAX_PALYER_ON_TABLE;i++)
			{
				bis >> res.m_PlayerHandPai[i];
			}
			bis >> res.m_MoneyPoolCount;
			for(int i=0;i<res.m_PlayerCount&&i<MAX_PALYER_ON_TABLE;i++)
			{
				bis >> res.m_MoneyPools[i];
			}

			return bis;
		}
	};

	struct GameDeZhou_ReLink
	{
		enum { XY_ID = GameXYID_ReLink };
		enum { Start = 1,End };

		unsigned char        m_Flag;

		GameDeZhou_ReLink() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReLink& rl )
		{
			bos << rl.m_Flag;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_ReLink& rl )
		{
			rl.ReSet();
			bis >> rl.m_Flag;
			return bis;
		}
	};

	struct GameDeZhou_ReqShowPai
	{
		enum { XY_ID = GameXYID_ReqShowPai };

		unsigned char           m_SitID;

		GameDeZhou_ReqShowPai() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqShowPai& rsp )
		{
			bos << rsp.m_SitID;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqShowPai& rsp )
		{
			rsp.ReSet();
			bis >> rsp.m_SitID;
			return bis;
		}
	};

	struct GameDeZhou_ShowPai
	{
		enum { XY_ID = GameXYID_ShowPai };
		
		unsigned char           m_SitID;
		unsigned char           m_HandPai[2];		

		GameDeZhou_ShowPai() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ShowPai& sp )
		{
			bos << sp.m_SitID;
			bos << sp.m_HandPai[0];
			bos << sp.m_HandPai[1];		

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_ShowPai& sp )
		{
			sp.ReSet();

			bis >> sp.m_SitID;
			bis >> sp.m_HandPai[0];
			bis >> sp.m_HandPai[1];

			return bis;
		}
	};

	struct GameDeZhou_MatchBlind
	{
		enum { XY_ID = GameXYID_MatchBlind };

		int                 m_BigBlind;
		int                 m_SmallBlind;

		GameDeZhou_MatchBlind() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_MatchBlind& src )
		{
			bos << src.m_BigBlind;
			bos << src.m_SmallBlind;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_MatchBlind& src )
		{
			src.ReSet();

			bis >> src.m_BigBlind;
			bis >> src.m_SmallBlind;

			return bis;
		}
	};

	struct GameDeZhou_MatchResult
	{
		enum { XY_ID = GameXYID_MatchResult };

		int                 m_MatchID;
		int                 m_Position;
		SBigNumber          m_nAwardMoney;
		SBigNumber          m_nGameMoney;
		SBigNumber          m_nAwardJF;
		SBigNumber          m_nMatchJF;

		GameDeZhou_MatchResult() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_MatchResult& src )
		{
			bos << src.m_MatchID;
			bos << src.m_Position;
			bos << src.m_nAwardMoney;
			bos << src.m_nGameMoney;
			bos << src.m_nAwardJF;
			bos << src.m_nMatchJF;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_MatchResult& src )
		{
			src.ReSet();

			bis >> src.m_MatchID;
			bis >> src.m_Position;
			bis >> src.m_nAwardMoney;
			bis >> src.m_nGameMoney;
			bis >> src.m_nAwardJF;
			bis >> src.m_nMatchJF;

			return bis;
		}
	};

	struct GameDeZhou_MatchWaitTable
	{
		enum { XY_ID = GameXYID_MatchWaitTable };

		unsigned short      m_TableID;

		GameDeZhou_MatchWaitTable() { ReSet(); }
		void ReSet()
		{
			m_TableID = 0;
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_MatchWaitTable& src )
		{
			bos << src.m_TableID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_MatchWaitTable& src )
		{
			src.ReSet();

			bis >> src.m_TableID;

			return bis;
		}
	};

	struct GameDeZhou_PlayerRank
	{
		enum { XY_ID = GameXYID_PlayerRank };

		short      m_LeftPlayerCount;
		short      m_MyRank;

		GameDeZhou_PlayerRank() { ReSet(); }
		void ReSet()
		{
			m_LeftPlayerCount = 0;
			m_MyRank = 0;
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_PlayerRank& src )
		{
			bos << src.m_LeftPlayerCount;
			bos << src.m_MyRank;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_PlayerRank& src )
		{
			src.ReSet();

			bis >> src.m_LeftPlayerCount;
			bis >> src.m_MyRank;

			return bis;
		}
	};

	struct GameDeZhou_TableChat
	{
		enum { XY_ID = GameXYID_TableChat };

		unsigned int        m_PID;
		std::string         m_ChatMsg;

		GameDeZhou_TableChat() { ReSet(); }
		void ReSet()
		{
			m_PID = 0;
			m_ChatMsg = "";
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_TableChat& tc )
		{
			bos << tc.m_PID;
			InString(bos,tc.m_ChatMsg,MAX_TABLECHAT_SIZE);

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_TableChat& tc )
		{
			tc.ReSet();

			bis >> tc.m_PID;
			OutString(bis,tc.m_ChatMsg,MAX_TABLECHAT_SIZE);

			return bis;
		}
	};

	struct GameDeZhou_PrivateChat
	{
		enum { XY_ID = GameXYID_PrivateChat };

		unsigned int       m_SendPID;
		unsigned int       m_ToPID;
		std::string        m_ChatMsg;

		GameDeZhou_PrivateChat() { ReSet(); }
		void ReSet()
		{ 
			m_SendPID = 0;
			m_ToPID = 0;
			m_ChatMsg = "";
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_PrivateChat& pc )
		{		
			bos << pc.m_SendPID;
			bos << pc.m_ToPID;
			InString(bos,pc.m_ChatMsg,MAX_PRIVATECHAT_SIZE);

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_PrivateChat& pc )
		{
			pc.ReSet();

			bis >> pc.m_SendPID;
			bis >> pc.m_ToPID;
			OutString(bis,pc.m_ChatMsg,MAX_PRIVATECHAT_SIZE);

			return bis;
		}
	};

	struct GameDeZhou_TableQuickChat
	{
		enum { XY_ID = GameXYID_TableQuickChat };

		unsigned int        m_PID;
		unsigned char       m_Idx;		

		GameDeZhou_TableQuickChat() { ReSet(); }
		void ReSet()
		{
			m_PID = 0;
			m_Idx = 0;
		}
		friend bostream& operator<<( bostream& bos, const GameDeZhou_TableQuickChat& tc )
		{
			bos << tc.m_PID;
			bos << tc.m_Idx;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_TableQuickChat& tc )
		{
			tc.ReSet();

			bis >> tc.m_PID;
			bis >> tc.m_Idx;

			return bis;
		}
	};

	struct GameDeZhou_ReqShowFace
	{
		enum { XY_ID = GameXYID_ReqShowFace };

		unsigned char       m_SitID;
		unsigned char       m_FaceID;

		GameDeZhou_ReqShowFace() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqShowFace& sf )
		{
			bos << sf.m_SitID;
			bos << sf.m_FaceID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqShowFace& sf )
		{
			sf.ReSet();

			bis >> sf.m_SitID;
			bis >> sf.m_FaceID;

			return bis;
		}
	};

	struct GameDeZhou_RespShowFace
	{
		enum { XY_ID = GameXYID_RespShowFace };

		unsigned char       m_SitID;
		unsigned char       m_FaceID;

		SBigNumber          m_PayMoney;
		SBigNumber          m_TableMoney;

		GameDeZhou_RespShowFace() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_RespShowFace& sf )
		{
			bos << sf.m_SitID;
			bos << sf.m_FaceID;
			bos << sf.m_PayMoney;
			bos << sf.m_TableMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_RespShowFace& sf )
		{
			sf.ReSet();

			bis >> sf.m_SitID;
			bis >> sf.m_FaceID;
			bis >> sf.m_PayMoney;
			bis >> sf.m_TableMoney;

			return bis;
		}
	};

	struct GameDeZhou_ReqAddFriend
	{
		enum { XY_ID = GameXYID_ReqAddFriend };
		enum { Req,Accept,Refuse };

		unsigned char      m_Flag;
		unsigned int       m_FromPID;
		unsigned int       m_ToPID;

		GameDeZhou_ReqAddFriend() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqAddFriend& af )
		{
			bos << af.m_Flag;
			bos << af.m_FromPID;
			bos << af.m_ToPID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqAddFriend& af )
		{
			af.ReSet();

			bis >> af.m_Flag;
			bis >> af.m_FromPID;
			bis >> af.m_ToPID;

			return bis;
		}
	};

	struct GameDeZhou_RespAddFriend
	{
		enum { XY_ID = GameXYID_RespAddFriend };

		unsigned int       m_LeftPID;
		unsigned int       m_RightPID;

		GameDeZhou_RespAddFriend() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_RespAddFriend& af )
		{
			bos << af.m_LeftPID;
			bos << af.m_RightPID;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_RespAddFriend& af )
		{
			af.ReSet();

			bis >> af.m_LeftPID;
			bis >> af.m_RightPID;

			return bis;
		}
	};

	struct GameDeZhou_AddFriendInfo
	{
		enum { XY_ID = GameXYID_AddFriendInfo };
		enum { MAX_COUNT = 500 };

		mutable short                  m_nCount;
		std::vector<unsigned int>      m_vecReqPID;

		GameDeZhou_AddFriendInfo() { ReSet(); }
		void ReSet()
		{
			m_nCount = 0;
			m_vecReqPID.clear();
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_AddFriendInfo& af )
		{
			af.m_nCount = min( short(af.m_vecReqPID.size()),MAX_COUNT );

			bos << af.m_nCount;
			for (int i=0;i<af.m_nCount;i++)
			{
				bos << af.m_vecReqPID.at(i);
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_AddFriendInfo& af )
		{
			af.ReSet();

			bis >> af.m_nCount;
			if ( af.m_nCount > MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}
			for (int i=0;i<af.m_nCount;i++)
			{
				int PID;
				bis >> PID;
				af.m_vecReqPID.push_back(PID);
			}		

			return bis;
		}
	};

	struct GameDeZhou_ReqSendMoney
	{
		enum { XY_ID = GameXYID_ReqSendMoney };

		unsigned int        m_SendPID;
		unsigned int        m_RecvPID;

		SBigNumber          m_nSendMoney;	

		GameDeZhou_ReqSendMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqSendMoney& sm )
		{
			bos << sm.m_SendPID;
			bos << sm.m_RecvPID;
			bos << sm.m_nSendMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqSendMoney& sm )
		{
			sm.ReSet();

			bis >> sm.m_SendPID;
			bis >> sm.m_RecvPID;
			bis >> sm.m_nSendMoney;

			return bis;
		}
	};

	struct GameDeZhou_RespSendMoney
	{
		enum { XY_ID = GameXYID_RespSendMoney };
		enum 
		{
			SUCCESS = 0,
			UNSUCCESS = 1,                //赠送失败
			ReSend = 2,                   //一局只能赠送一次
			OUTMONEY = 3,                 //您当前游戏币较少，不能赠送
			CantSend = 7,                 //当前不能相互赠送游戏币
		};

		unsigned char       m_Flag;

		unsigned int        m_SendPID;
		SBigNumber          m_SendPlayerTableMoney;
		unsigned int        m_RecvPID;
		SBigNumber          m_RecvPlayerTableMoney;
		SBigNumber          m_nSendMoney;

		GameDeZhou_RespSendMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_RespSendMoney& sm )
		{
			bos << sm.m_Flag;
			if ( sm.m_Flag == SUCCESS )
			{
				bos << sm.m_SendPID;
				bos << sm.m_SendPlayerTableMoney;
				bos << sm.m_RecvPID;
				bos << sm.m_RecvPlayerTableMoney;
				bos << sm.m_nSendMoney;
			}
			
			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_RespSendMoney& sm )
		{
			sm.ReSet();

			bis >> sm.m_Flag;
			if ( sm.m_Flag == SUCCESS )
			{
				bis >> sm.m_SendPID;
				bis >> sm.m_SendPlayerTableMoney;
				bis >> sm.m_RecvPID;
				bis >> sm.m_SendPlayerTableMoney;
				bis >> sm.m_nSendMoney;
			}

			return bis;
		}
	};

	struct GameDeZhou_SendGift
	{
		enum { XY_ID = GameXYID_SendGift };
		enum { MAX_COUNT = 250 };

		unsigned short      m_GiftID;
		unsigned int        m_SendPID;
		VectorPID           m_vecToPID;

		GameDeZhou_SendGift() { ReSet(); }
		void ReSet()
		{
			m_GiftID = 0;
			m_SendPID = 0;
			m_vecToPID.clear();
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_SendGift& sg )
		{
			bos << sg.m_GiftID;
			bos << sg.m_SendPID;
			InVector(bos,sg.m_vecToPID,sg.MAX_COUNT);
			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_SendGift& sg )
		{
			sg.ReSet();

			bis >> sg.m_GiftID;
			bis >> sg.m_SendPID;
			OutVector(bis,sg.m_vecToPID,sg.MAX_COUNT);

			return bis;
		}
	};

	struct GameDeZhou_RespSendGift
	{
		enum { XY_ID = GameXYID_RespSendGift };
		enum { MAX_COUNT = 250 };
		enum
		{
			SUCCESS = 0,
			NOMONEY = 1,
			NOPLAYER = 2,
			NOPRICE = 3,
			NOMOGUI = 4,
			Failed
		};
		
		unsigned char       m_Flag;
		unsigned short      m_GiftID;
		unsigned int        m_SendPID;
		VectorPID           m_vecToPID;

		GameDeZhou_RespSendGift() { ReSet(); }
		void ReSet()
		{
			m_Flag = Failed;
			m_GiftID = 0;
			m_SendPID = 0;
			m_vecToPID.clear();
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_RespSendGift& sg )
		{
			bos << sg.m_Flag;
			if ( sg.m_Flag == sg.SUCCESS )
			{
				bos << sg.m_GiftID;
				bos << sg.m_SendPID;
				InVector(bos,sg.m_vecToPID,sg.MAX_COUNT);
			}			

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_RespSendGift& sg )
		{
			sg.ReSet();

			bis >> sg.m_Flag;
			if ( sg.m_Flag == sg.SUCCESS )
			{
				bis >> sg.m_GiftID;
				bis >> sg.m_SendPID;
				OutVector(bis,sg.m_vecToPID,sg.MAX_COUNT);
			}
			
			return bis;
		}
	};

	struct GameDeZhou_ReqAddTableTime
	{
		enum { XY_ID = GameXYID_ReqAddTableTime };

		unsigned int	   m_PID;
		short              m_RoomID;
		short              m_TableID;
		int                m_AddTime;

		GameDeZhou_ReqAddTableTime(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_ReqAddTableTime& src)
		{
			bos << src.m_PID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_AddTime;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_ReqAddTableTime& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_AddTime;

			return bis;
		}
	};

	struct GameDeZhou_RespAddTableTime
	{
		enum { XY_ID = GameXYID_RespAddTableTime };
		enum 
		{
			SUCCESS,
			UNSUCCESS,
			OUTMONEY,
			NOROOM,
			NOTABLE,
			TABLEERROR,
		};

		unsigned char      m_Flag;
		unsigned int	   m_PID;
		int                m_AddTime;
		int                m_nLeftTime;

		GameDeZhou_RespAddTableTime(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_RespAddTableTime& src)
		{
			bos << src.m_Flag;
			if (src.m_Flag == SUCCESS )
			{
				bos << src.m_PID;
				bos << src.m_AddTime;
				bos << src.m_nLeftTime;
			}
			
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_RespAddTableTime& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			if (src.m_Flag == SUCCESS)
			{
				bis >> src.m_PID;
				bis >> src.m_AddTime;
				bis >> src.m_nLeftTime;
			}			

			return bis;
		}
	};

	struct GameDeZhou_TableLeft
	{
		enum { XY_ID = GameXYID_LeftTime };

		int                m_LeftTime;

		GameDeZhou_TableLeft(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_TableLeft& src)
		{			
			bos << src.m_LeftTime;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_TableLeft& src)
		{
			src.ReSet();

			bis >> src.m_LeftTime;

			return bis;
		}
	};

	struct GameDeZhou_TableEnd
	{
		enum { XY_ID = GameXYID_TableEnd };

		GameDeZhou_TableEnd(){ ReSet(); }
		void ReSet() {  }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_TableEnd& src)
		{
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_TableEnd& src)
		{
			return bis;
		}
	};

	struct GameDeZhou_ReqJoinJuBaoPeng
	{
		enum { XY_ID = GameXYID_ReqJoinJuBaoPeng };

		BYTE               m_SitID;
		int                m_nZhuChip;          //玩家每次投注的游戏戏币数量
		int                m_nCount;

		GameDeZhou_ReqJoinJuBaoPeng(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_ReqJoinJuBaoPeng& src)
		{
			bos << src.m_SitID;
			bos << src.m_nZhuChip;
			bos << src.m_nCount;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_ReqJoinJuBaoPeng& src)
		{
			src.ReSet();
			bis >> src.m_SitID;
			bis >> src.m_nZhuChip;
			bis >> src.m_nCount;
			return bis;
		}
	};
	struct GameDeZhou_RespJoinJuBaoPeng
	{
		enum { XY_ID = GameXYID_RespJoinJuBaoPeng };
		enum { SUCCESS,AREADYIN,MatchTable,Failed,NOTJOIN,NOMONEY,};

		BYTE               m_Flag;
		int                m_nZhuChip;
		int                m_nCount;

		GameDeZhou_RespJoinJuBaoPeng(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_RespJoinJuBaoPeng& src)
		{
			bos << src.m_Flag;
			bos << src.m_nZhuChip;
			bos << src.m_nCount;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_RespJoinJuBaoPeng& src)
		{
			src.ReSet();
			bis >> src.m_Flag;
			bis >> src.m_nZhuChip;
			bis >> src.m_nCount;
			return bis;
		}
	};
	struct GameDeZhou_JoinJuBaoPengInfo
	{
		enum { XY_ID = GameXYID_JoinJuBaoPengInfo };
		enum { JoinJuBaoPeng=1,ExitJuBaoPeng=2, };

		BYTE               m_SitID;
		BYTE               m_Flag;

		GameDeZhou_JoinJuBaoPengInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_JoinJuBaoPengInfo& src)
		{
			bos << src.m_SitID;
			bos << src.m_Flag;			
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_JoinJuBaoPengInfo& src)
		{
			src.ReSet();
			bis >> src.m_SitID;
			bis >> src.m_Flag;
			return bis;
		}
	};

	struct msgWinJuBaoPeng
	{
		unsigned int       m_PID;
		BYTE               m_PaiType;         //玩家的牌型
		SBigNumber         m_nZhuChip;        //下注游戏币数量
		SBigNumber         m_nBeiShu;         //获赔的倍数
		SBigNumber         m_WinMoney;        //奖励金额

		//SBigNumber         m_TableMoney;      //获注后玩家桌面游戏币数量
		//SBigNumber         m_GameMoney;       //获注后玩家总游戏币数量

		msgWinJuBaoPeng(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const msgWinJuBaoPeng& src)
		{
			bos << src.m_PID;
			bos << src.m_PaiType;
			bos << src.m_nZhuChip;
			bos << src.m_nBeiShu;
			bos << src.m_WinMoney;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,msgWinJuBaoPeng& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_PaiType;
			bis >> src.m_nZhuChip;
			bis >> src.m_nBeiShu;
			bis >> src.m_WinMoney;

			return bis;
		}
	};

	struct GameDeZhou_WinJuBaoPeng
	{
		enum { XY_ID = GameXYID_WinJuBaoPeng };

		std::vector<msgWinJuBaoPeng>     m_listWin;

		GameDeZhou_WinJuBaoPeng(){ ReSet(); }
		void ReSet() { m_listWin.clear(); }

		friend bostream& operator<<(bostream& bos,const GameDeZhou_WinJuBaoPeng& src)
		{
			InVector(bos,src.m_listWin,10);
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameDeZhou_WinJuBaoPeng& src)
		{
			src.ReSet();
			OutVector(bis,src.m_listWin,10);
			return bis;
		}
	};

	struct stTablePlayerWinLoss
	{
		UINT32           m_PID;
		SBigNumber       m_WinLossMoney;

		stTablePlayerWinLoss() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }

		friend bostream& operator<<( bostream& bos, const stTablePlayerWinLoss& src )
		{
			bos << src.m_PID;
			bos << src.m_WinLossMoney;
			return bos;
		}

		friend bistream& operator>>( bistream& bis, stTablePlayerWinLoss& src )
		{
			src.ReSet();
			bis >> src.m_PID;
			bis >> src.m_WinLossMoney;
			return bis;
		}
	};

	struct GameDeZhou_TableWinLoss
	{
		enum { XY_ID = GameXYID_TableWinLoss };
		enum { MaxWinLossCount = 100 };

		std::vector<stTablePlayerWinLoss>       m_vectorWinLoss;

		GameDeZhou_TableWinLoss() { ReSet(); }
		void ReSet()
		{
			m_vectorWinLoss.clear();
		}

		friend bostream& operator<<( bostream& bos, const GameDeZhou_TableWinLoss& src )
		{
			InVector(bos,src.m_vectorWinLoss,src.MaxWinLossCount);
			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_TableWinLoss& src )
		{
			src.ReSet();
			OutVector(bis,src.m_vectorWinLoss,src.MaxWinLossCount);
			return bis;
		}
	};

	struct GameDeZhou_ReqTailInfo
	{
		enum { XY_ID = GameXYID_ReqTailInfo };
		
		BYTE                m_Flag;
		UINT32              m_nValue;

		GameDeZhou_ReqTailInfo() { ReSet(); }
		void ReSet()
		{
			m_Flag = 0;
			m_nValue = 0;
		}
		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqTailInfo& pti )
		{
			bos << pti.m_Flag;
			bos << pti.m_nValue;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqTailInfo& pti )
		{
			pti.ReSet();
			bis >> pti.m_Flag;
			bis >> pti.m_nValue;
			return bis;
		}
	};

	struct GameDeZhou_PromoteTime
	{
		enum { XY_ID = GameXYID_TimePromote };

		BYTE             m_Step;
		int              m_LeftTime;
		int              m_AwardMoney;

		GameDeZhou_PromoteTime() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const GameDeZhou_PromoteTime& PT )
		{
			bos << PT.m_Step;
			bos << PT.m_LeftTime;
			bos << PT.m_AwardMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_PromoteTime& PT )
		{
			PT.ReSet();

			bis >> PT.m_Step;
			bis >> PT.m_LeftTime;
			bis >> PT.m_AwardMoney;

			return bis;
		}
	};

	struct GameDeZhou_ReqPromoteTime
	{
		enum { XY_ID = GameXYID_ReqTimePromote };

		BYTE             m_Step;		

		GameDeZhou_ReqPromoteTime() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const GameDeZhou_ReqPromoteTime& PT )
		{
			bos << PT.m_Step;	

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_ReqPromoteTime& PT )
		{
			PT.ReSet();

			bis >> PT.m_Step;

			return bis;
		}
	};

	struct GameDeZhou_RespPromoteTime
	{
		enum { XY_ID = GameXYID_RespTimePromote };

		BYTE             m_Step;
		SBigNumber       m_PrizeMoney;

		BYTE             m_NextStep;
		SBigNumber       m_NextMoney;
		int              m_NextTime;

		GameDeZhou_RespPromoteTime() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const GameDeZhou_RespPromoteTime& PT )
		{
			bos << PT.m_Step;
			bos << PT.m_PrizeMoney;

			bos << PT.m_NextStep;
			bos << PT.m_NextMoney;
			bos << PT.m_NextTime;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameDeZhou_RespPromoteTime& PT )
		{
			PT.ReSet();

			bis >> PT.m_Step;
			bis >> PT.m_PrizeMoney;

			bis >> PT.m_NextStep;
			bis >> PT.m_NextMoney;
			bis >> PT.m_NextTime;

			return bis;
		}
	};
}
}
}
}