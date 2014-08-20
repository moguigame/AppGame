#pragma once

#include <string>
#include <vector>

#include "MoGuiGame.h"

namespace MoGui
{
namespace MoGuiXY
{
namespace GameXY
{
	using namespace std;
	using namespace MoGui::MoGuiXY;
	using namespace MoGui::MoGuiXY::PublicXYData;

	const short  MoGui_GAMELOBBYPLAYERCONNECT                    = MOGUI_FIRST_CLIENT_GAMELOBBY+1;            //连接游戏服务器的认证
	const short  MoGuiXYID_GameLobbyRespPlayerConnect            = MOGUI_FIRST_CLIENT_GAMELOBBY+2;            //认证返回

	const short  MoGui_GAME_FLAG                                 = MOGUI_FIRST_CLIENT_GAMELOBBY+3;            //大厅里的消息提示
	const short  MoGuiXYID_GameRule                              = MOGUI_FIRST_CLIENT_GAMELOBBY+4;            //服务器有关游戏的规则

	const short  MoGuiXYID_PlayerData                            = MOGUI_FIRST_CLIENT_GAMELOBBY+5;            //玩家的完整数据
	const short  MoGuiXYID_PlayerDataEx                          = MOGUI_FIRST_CLIENT_GAMELOBBY+6;            //玩家的扩展数据
	const short  MoGuiXYID_PlayerMatchData                       = MOGUI_FIRST_CLIENT_GAMELOBBY+7;

	const short  MoGui_REQJOINROOM                               = MOGUI_FIRST_CLIENT_GAMELOBBY+11;           //加入房间离开房间等
	const short  MoGui_RESPJOINROOM                              = MOGUI_FIRST_CLIENT_GAMELOBBY+12;
	const short  MoGui_JOINROOM                                  = MOGUI_FIRST_CLIENT_GAMELOBBY+13;
	const short  MoGui_REQLEAVEROOM                              = MOGUI_FIRST_CLIENT_GAMELOBBY+14;
	const short  MoGui_RESPLEAVEROOM                             = MOGUI_FIRST_CLIENT_GAMELOBBY+15;
	const short  MoGui_LEAVEROOM                                 = MOGUI_FIRST_CLIENT_GAMELOBBY+16;
	const short  MoGui_REQROOMINFO                               = MOGUI_FIRST_CLIENT_GAMELOBBY+17;
	const short  MoGui_RESPROOMINFO                              = MOGUI_FIRST_CLIENT_GAMELOBBY+18;
	const short  MoGui_ROOMINFO                                  = MOGUI_FIRST_CLIENT_GAMELOBBY+19;
	const short  MoGui_ROOMINIFOLIST                             = MOGUI_FIRST_CLIENT_GAMELOBBY+20;

	const short  MoGui_REQJOINTABLE                              = MOGUI_FIRST_CLIENT_GAMELOBBY+21;            //请求加入桌子
	const short  MoGui_RESPJOINTABLE                             = MOGUI_FIRST_CLIENT_GAMELOBBY+22;
	const short  MoGui_JOINTABLE                                 = MOGUI_FIRST_CLIENT_GAMELOBBY+23;
	const short  MoGui_REQLEAVETABLE							 = MOGUI_FIRST_CLIENT_GAMELOBBY+24;            //离开桌子
	const short  MoGui_RESPLEAVETABLE						     = MOGUI_FIRST_CLIENT_GAMELOBBY+25;
	const short  MoGui_LEAVETABLE						         = MOGUI_FIRST_CLIENT_GAMELOBBY+26;
	const short  MoGui_REQROOMTABLEINFO                          = MOGUI_FIRST_CLIENT_GAMELOBBY+27;
	const short  MoGui_RESPROOMTABLEINFO                         = MOGUI_FIRST_CLIENT_GAMELOBBY+28;
	const short  MoGui_TABLEINFO                                 = MOGUI_FIRST_CLIENT_GAMELOBBY+29;
	const short  MoGui_TABLEINFOLIST                             = MOGUI_FIRST_CLIENT_GAMELOBBY+30;

	const short  MoGui_REQPLAYERACT                  			 = MOGUI_FIRST_CLIENT_GAMELOBBY+31;           //玩家动作，坐下，站起等
	const short  MoGui_RESPPLAYERACT                 			 = MOGUI_FIRST_CLIENT_GAMELOBBY+32;
	const short  MoGui_PLAYERACT  	                 			 = MOGUI_FIRST_CLIENT_GAMELOBBY+33;           //坐下站起的专们协议通知

	const short  MoGuiXYID_ReqTracePlayer                        = MOGUI_FIRST_CLIENT_GAMELOBBY+34;
	const short  MoGuiXYID_RespTracePlayer                       = MOGUI_FIRST_CLIENT_GAMELOBBY+35;

	const short  MoGuiXYID_ShowTable                             = MOGUI_FIRST_CLIENT_GAMELOBBY+36;           //指示那些桌子可以显示
	const short  MoGuiXYID_ShowTableList                         = MOGUI_FIRST_CLIENT_GAMELOBBY+37;

	const short  MoGuiXYID_ChangePlace                           = MOGUI_FIRST_CLIENT_GAMELOBBY+38;

	const short  MoGui_PLAYERBASEINFO                			 = MOGUI_FIRST_CLIENT_GAMELOBBY+41;
	const short  MoGui_PLAYERGAMEINFO                			 = MOGUI_FIRST_CLIENT_GAMELOBBY+42;
	const short  MoGui_PLAYERSTATEINFO               			 = MOGUI_FIRST_CLIENT_GAMELOBBY+43;
	const short  MoGui_PLAYERTABLEINFO               			 = MOGUI_FIRST_CLIENT_GAMELOBBY+44;
	const short  MoGui_PLAYERINFO                    			 = MOGUI_FIRST_CLIENT_GAMELOBBY+45;
	const short  MoGui_PLAYERINFOLIST                			 = MOGUI_FIRST_CLIENT_GAMELOBBY+46;
	const short  GameLobbyXYID_PlayerStateInfoList               = MOGUI_FIRST_CLIENT_GAMELOBBY+47;

	const short  GameLobbyXYID_PlayerGameMoney                   = MOGUI_FIRST_CLIENT_GAMELOBBY+51;
	const short  GameLobbyXYID_PlayerMoGuiMoney                  = MOGUI_FIRST_CLIENT_GAMELOBBY+52;

	const short  GameLobbyXYID_PlayerHonorAward                  = MOGUI_FIRST_CLIENT_GAMELOBBY+54;          //玩家获得荣誉
	const short  GameLobbyXYID_JuBaoPengMoney                    = MOGUI_FIRST_CLIENT_GAMELOBBY+56;

	const short  GameLobbyXYID_GameLevelAward                    = MOGUI_FIRST_CLIENT_GAMELOBBY+58;

	const short  GameLobbyXYID_ReqTailInfo                  	 = MOGUI_FIRST_CLIENT_GAMELOBBY+60;
	const short  GameLobbyXYID_ReqPlayerGiftInfo              	 = MOGUI_FIRST_CLIENT_GAMELOBBY+61;
	const short  GameLobbyXYID_PlayerGiftInfo              	     = MOGUI_FIRST_CLIENT_GAMELOBBY+62;
	const short  GameLobbyXYID_PlayerGiftInfoList                = MOGUI_FIRST_CLIENT_GAMELOBBY+63;
	const short  GameLobbyXYID_PlayerHonorInfoList               = MOGUI_FIRST_CLIENT_GAMELOBBY+64;
	const short  GameLobbyXYID_PlayerTailInfo                    = MOGUI_FIRST_CLIENT_GAMELOBBY+65;

	const short  GameLobbyXYID_ReqSendGift                       = MOGUI_FIRST_CLIENT_GAMELOBBY+66;
	const short  GameLobbyXYID_RespSendGift                      = MOGUI_FIRST_CLIENT_GAMELOBBY+67;
	const short  GameLobbyXYID_RecvGift                          = MOGUI_FIRST_CLIENT_GAMELOBBY+68;
	const short  GameLobbyXYID_ReqSoldGift                       = MOGUI_FIRST_CLIENT_GAMELOBBY+69;
	const short  GameLobbyXYID_RespSoldGift                      = MOGUI_FIRST_CLIENT_GAMELOBBY+70;
	const short  GameLobbyXYID_ReqChangGift                      = MOGUI_FIRST_CLIENT_GAMELOBBY+71;
	const short  GameLobbyXYID_RespChangGift                     = MOGUI_FIRST_CLIENT_GAMELOBBY+72;

	const short  GameLobbyXYID_ReqChangeBank                     = MOGUI_FIRST_CLIENT_GAMELOBBY+76;
	const short  GameLobbyXYID_RespChangeBank                    = MOGUI_FIRST_CLIENT_GAMELOBBY+77;

	const short  GameLobbyXYID_UserAwardList                     = MOGUI_FIRST_CLIENT_GAMELOBBY+81;
	const short  GameLobbyXYID_UserHuiYuan                       = MOGUI_FIRST_CLIENT_GAMELOBBY+82;
	const short  GameLobbyXYID_UserMoguiExchange                 = MOGUI_FIRST_CLIENT_GAMELOBBY+83;

	const short  GameLobbyXYID_MaxPai                            = MOGUI_FIRST_CLIENT_GAMELOBBY+85;
	const short  GameLobbyXYID_MaxWin                            = MOGUI_FIRST_CLIENT_GAMELOBBY+86;
	const short  GameLobbyXYID_MaxMoney                          = MOGUI_FIRST_CLIENT_GAMELOBBY+87;

	const short  GameLobbyXYID_ReqRoomPlayerBase                 = MOGUI_FIRST_CLIENT_GAMELOBBY+101;
	const short  GameLobbyXYID_ReqRoomPlayerState                = MOGUI_FIRST_CLIENT_GAMELOBBY+102;
	const short  GameLobbyXYID_ReqCityPlayerBase                 = MOGUI_FIRST_CLIENT_GAMELOBBY+103;
	const short  GameLobbyXYID_ReqCityPlayerState                = MOGUI_FIRST_CLIENT_GAMELOBBY+104;
	const short  GameLobbyXYID_ReqTablePlayerBase                = MOGUI_FIRST_CLIENT_GAMELOBBY+105;

	const short  GameLobbyXYID_ChangeUserInfo                    = MOGUI_FIRST_CLIENT_GAMELOBBY+111;          //请求改变昵称
	const short  GameLobbyXYID_ReqPlayerAward                    = MOGUI_FIRST_CLIENT_GAMELOBBY+112;          //请求玩家的奖励
	const short  GameLobbyXYID_RespChangeUserInfo                = MOGUI_FIRST_CLIENT_GAMELOBBY+113;

	const short  GameLobbyXYID_ReqFollowPlayer                   = MOGUI_FIRST_CLIENT_GAMELOBBY+121;
	const short  GameLobbyXYID_RespFollowPlayer                  = MOGUI_FIRST_CLIENT_GAMELOBBY+122;

	const short  MoGui_REQTABLEPLAYERCOUNT	                     = MOGUI_FIRST_CLIENT_GAMELOBBY+131;          //请求各类人数信息
	const short  MoGui_TABLEPLAYERCOUNT	                         = MOGUI_FIRST_CLIENT_GAMELOBBY+132;
	const short  MoGui_TABLEPLAYERCOUNTLIST                      = MOGUI_FIRST_CLIENT_GAMELOBBY+133;

	const short  MoGui_REQROOMPLAYERCOUNT	                     = MOGUI_FIRST_CLIENT_GAMELOBBY+141;
	const short  MoGui_ROOMPLAYERCOUNT	                         = MOGUI_FIRST_CLIENT_GAMELOBBY+142;
	const short  MoGui_ROOMPLAYERCOUNTLIST                       = MOGUI_FIRST_CLIENT_GAMELOBBY+143;

	const short  MoGui_REQGAMEPLAYERCOUNT                        = MOGUI_FIRST_CLIENT_GAMELOBBY+151;
	const short  MoGui_GAMEPLAYERCOUNT                           = MOGUI_FIRST_CLIENT_GAMELOBBY+152;

	const short  MoGui_MatchRoomPlayerCount                      = MOGUI_FIRST_CLIENT_GAMELOBBY+160;          //房间比赛的人数

	const short  MoGui_ReqPlayerWinLoseRank                      = MOGUI_FIRST_CLIENT_GAMELOBBY+170;          //房间排行榜
	const short  MoGui_RespPlayerWinLoseRank                     = MOGUI_FIRST_CLIENT_GAMELOBBY+171;
	const short  MoGui_PlayerWinLoseRankList                     = MOGUI_FIRST_CLIENT_GAMELOBBY+172;
	
	const short  MoGui_STARTGAME		                         = MOGUI_FIRST_CLIENT_GAMELOBBY+200;          //启动客户端

	const short  GameLobbyXYID_ReqSignMatch                      = MOGUI_FIRST_CLIENT_GAMELOBBY+251;
	const short  GameLobbyXYID_RespSignMatch                     = MOGUI_FIRST_CLIENT_GAMELOBBY+252;
	const short  GameLobbyXYID_ReqEnterMatch                     = MOGUI_FIRST_CLIENT_GAMELOBBY+253;

	const short  GameLobbyXYID_MatchTableState                   = MOGUI_FIRST_CLIENT_GAMELOBBY+254;
	const short  GameLobbyXYID_MatchRoomState                    = MOGUI_FIRST_CLIENT_GAMELOBBY+255;

	const short  GameLobbyXYID_ReqRejoinMatch                    = MOGUI_FIRST_CLIENT_GAMELOBBY+256;          //中途付费加入比赛
	const short  GameLobbyXYID_RespRejoinMatch                   = MOGUI_FIRST_CLIENT_GAMELOBBY+257; 

	const short  GameLobbyXYID_AwokeMatch                        = MOGUI_FIRST_CLIENT_GAMELOBBY+258;

	const short  GameLobbyXYID_ReqMatchRank                      = MOGUI_FIRST_CLIENT_GAMELOBBY+261;
	const short  GameLobbyXYID_RespMatchRank                     = MOGUI_FIRST_CLIENT_GAMELOBBY+262;
	const short  GameLobbyXYID_MatchRankInfo                     = MOGUI_FIRST_CLIENT_GAMELOBBY+263;

	const short  GameLobbyXYID_ReqCreateTable                    = MOGUI_FIRST_CLIENT_GAMELOBBY+301;
	const short  GameLobbyXYID_RespCreateTable                   = MOGUI_FIRST_CLIENT_GAMELOBBY+302;
	const short  GameLobbyXYID_CreateTablePID                    = MOGUI_FIRST_CLIENT_GAMELOBBY+303;

	const short  GameLobbyXYID_XieYiList                         = MOGUI_FIRST_CLIENT_GAMELOBBY+480;

	const short  GameLobbyXYID_Test                              = MOGUI_FIRST_CLIENT_GAMELOBBY+499;

	const short  MoGui_GAME_SERVERTOCLIENT                       = MOGUI_FIRST_CLIENT_GAMELOBBY+501;
	const short  MoGui_GAME_CLIENTTOSERVER                       = MOGUI_FIRST_CLIENT_GAMELOBBY+502;

	struct GameLobbyPlayerConnect
	{
		enum { XY_ID = MoGui_GAMELOBBYPLAYERCONNECT };

		unsigned char       m_PlayerType;           //1普通玩家 3游客,
		unsigned char       m_ClientType;           //0PC，1是WEB，2手机，3TV
		unsigned char       m_LoginType;

		unsigned short      m_AID;
		unsigned int        m_PID;

		Session             m_Session;

		GameLobbyPlayerConnect() { ReSet(); }
		void ReSet()
		{
			m_PlayerType = 0;
			m_ClientType = 0;
			m_LoginType = 0;

			m_AID = 0;
			m_PID = 0;

			m_Session.ReSet();
		}
		friend bostream& operator<<( bostream& bos, const GameLobbyPlayerConnect& pc )
		{
			bos << pc.m_PlayerType;
			bos << pc.m_ClientType;
			bos << pc.m_LoginType;
			bos << pc.m_AID;
			bos << pc.m_PID;
			bos << pc.m_Session;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, GameLobbyPlayerConnect& pc )
		{
			pc.ReSet();

			bis >> pc.m_PlayerType;
			bis >> pc.m_ClientType;
			bis >> pc.m_LoginType;
			bis >> pc.m_AID;
			bis >> pc.m_PID;
			bis >> pc.m_Session;

			return bis;
		}
	};

	struct GameLobbyRespPlayerConnect
	{
		enum { XY_ID = MoGuiXYID_GameLobbyRespPlayerConnect };

		enum
		{
			SUCCESS = 0,
			Failed       = 99,
			LOGINSOCKET  = 101,
			LOGINERROR   = 102,
			NOPLAYER     = 103,
			SessionError = 104,
		};
		
		unsigned char       m_Flag;
		short               m_AID;
		unsigned int        m_PID;

		Session             m_Session;
		
		GameLobbyRespPlayerConnect() { ReSet(); }
		void ReSet()
		{
			m_Flag = 0;
			m_AID = 0;
			m_PID = 0;
			m_Session.ReSet();
		}

		friend bostream& operator<<( bostream& bos, const GameLobbyRespPlayerConnect& pd )
		{
			bos << pd.m_Flag;
			bos << pd.m_AID;
			bos << pd.m_PID;
			if ( pd.m_Flag == SUCCESS )
			{
				bos << pd.m_Session;
			}

			return bos;
		}

		friend bistream& operator>>( bistream& bis, GameLobbyRespPlayerConnect& pd )
		{
			pd.ReSet();

			bis >> pd.m_Flag;
			bis >> pd.m_AID;
			bis >> pd.m_PID;

			if ( pd.m_Flag == SUCCESS )
			{
				bis >> pd.m_Session;
			}

			return bis;
		}
	};

	struct Game_PlayerData
	{
		enum { XY_ID = MoGuiXYID_PlayerData };

		short               m_AID;
		unsigned int        m_PID;

		unsigned char       m_Sex;
		unsigned char       m_PlayerType;
		unsigned char       m_ChangeName;
		short               m_Year;

		SBigNumber          m_nBankMoney;
		unsigned char       m_OpenBank;
		SBigNumber          m_nGameMoney;
		SBigNumber          m_nMoGuiMoney;
		SBigNumber          m_nMatchJF;

		SBigNumber          m_nJF;
		SBigNumber          m_nEP;
		SBigNumber          m_nWinTimes;
		SBigNumber          m_nLossTimes;
		SBigNumber          m_nGameTime;

		std::string         m_NickName;
		std::string         m_HeadPicURL;
		std::string         m_HomePageURL;
		std::string         m_City;

		unsigned int        m_JoinTime;
		unsigned int        m_LandTime;

		unsigned short      m_RoomID;
		unsigned short      m_TableID;
		unsigned char       m_SitID;
		unsigned char       m_PlayerState;

		BYTE                m_GameLevel;

		Game_PlayerData() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;

			m_Sex = 0;
			m_PlayerType = 0;
			m_ChangeName = 0;
			m_Year = 1980;

			m_nBankMoney = 0;
			m_OpenBank = 0;
			m_nGameMoney = 0;
			m_nMoGuiMoney = 0;
			m_nMatchJF = 0;

			m_nJF = 0;
			m_nEP = 0;
			m_nWinTimes = 0;
			m_nLossTimes = 0;
			m_nGameTime = 0;

			m_NickName = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";

			m_JoinTime = 0;
			m_LandTime = 0;

			m_RoomID = 0;
			m_TableID = 0;
			m_SitID = 0;
			m_PlayerState = 0;

			m_GameLevel = 0;
		}

		friend bostream& operator<<( bostream& bos, const Game_PlayerData& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Sex;

			bos << src.m_PlayerType;
			bos << src.m_ChangeName;
			bos << src.m_Year;

			bos << src.m_nBankMoney;
			bos << src.m_OpenBank;
			bos << src.m_nGameMoney;
			bos << src.m_nMoGuiMoney;
			bos << src.m_nMatchJF;

			bos << src.m_nJF;
			bos << src.m_nEP;
			bos << src.m_nWinTimes;
			bos << src.m_nLossTimes;
			bos << src.m_nGameTime;

			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicURL,MAX_URL_SIZE);
			InString(bos,src.m_HomePageURL,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			bos << src.m_JoinTime;
			bos << src.m_LandTime;

			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_SitID;
			bos << src.m_PlayerState;

			bos << src.m_GameLevel;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, Game_PlayerData& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Sex;

			bis >> src.m_PlayerType;
			bis >> src.m_ChangeName;
			bis >> src.m_Year;

			bis >> src.m_nBankMoney;
			bis >> src.m_OpenBank;
			bis >> src.m_nGameMoney;
			bis >> src.m_nMoGuiMoney;
			bis >> src.m_nMatchJF;

			bis >> src.m_nJF;
			bis >> src.m_nEP;
			bis >> src.m_nWinTimes;
			bis >> src.m_nLossTimes;
			bis >> src.m_nGameTime;

			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicURL,MAX_URL_SIZE);
			OutString(bis,src.m_HomePageURL,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			bis >> src.m_JoinTime;
			bis >> src.m_LandTime;

			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_SitID;
			bis >> src.m_PlayerState;

			bis >> src.m_GameLevel;

			return bis;
		}
	};

	struct Game_PlayerDataEx
	{
		enum { XY_ID = MoGuiXYID_PlayerDataEx };
		enum { WinTypeNum = 10 };

		short                         m_AID;
		unsigned int                  m_PID;

		INT64                         m_MaxPai;
		UINT32                        m_MaxPaiTime;
		INT64                         m_MaxMoney;
		UINT32                        m_MaxMoneyTime;
		INT64                         m_MaxWin;
		UINT32                        m_MaxWinTime;

		BYTE                          m_VipLevel;
		short                         m_FriendCount;

		Game_PlayerDataEx() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const Game_PlayerDataEx& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_MaxPai;
			bos << src.m_MaxPaiTime;
			bos << src.m_MaxMoney;
			bos << src.m_MaxMoneyTime;
			bos << src.m_MaxWin;
			bos << src.m_MaxWinTime;

			bos << src.m_VipLevel;
			bos << src.m_FriendCount;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, Game_PlayerDataEx& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_MaxPai;
			bis >> src.m_MaxPaiTime;
			bis >> src.m_MaxMoney;
			bis >> src.m_MaxMoneyTime;
			bis >> src.m_MaxWin;
			bis >> src.m_MaxWinTime;

			bis >> src.m_VipLevel;
			bis >> src.m_FriendCount;

			return bis;
		}
	};

	struct Game_PlayerMatchData
	{
		enum { XY_ID = MoGuiXYID_PlayerMatchData };
		enum { WinTypeNum = 10 };

		short                m_AID;
		unsigned int         m_PID;

		INT32                m_TaoJinTimes;
		INT64                m_TaoJinWinMoney;
		INT32                m_TaoJinBest;
		UINT32               m_TaoJinBestTime;

		INT32                m_JingBiaoTimes;
		INT64                m_JingBiaoWinMoney;
		INT32                m_JingBiaoBest;
		UINT32               m_JingBiaoBestTime;

		INT32                m_GuanJunTimes;
		INT64                m_GuanJunWinMoney;
		INT32                m_GuanJunBest;
		UINT32               m_GuanJunBestTime;

		Game_PlayerMatchData() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const Game_PlayerMatchData& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_TaoJinTimes;
			bos << src.m_TaoJinWinMoney;
			bos << src.m_TaoJinBest;
			bos << src.m_TaoJinBestTime;

			bos << src.m_JingBiaoTimes;
			bos << src.m_JingBiaoWinMoney;
			bos << src.m_JingBiaoBest;
			bos << src.m_JingBiaoBestTime;

			bos << src.m_GuanJunTimes;
			bos << src.m_GuanJunWinMoney;
			bos << src.m_GuanJunBest;
			bos << src.m_GuanJunBestTime;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, Game_PlayerMatchData& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_TaoJinTimes;
			bis >> src.m_TaoJinWinMoney;
			bis >> src.m_TaoJinBest;
			bis >> src.m_TaoJinBestTime;

			bis >> src.m_JingBiaoTimes;
			bis >> src.m_JingBiaoWinMoney;
			bis >> src.m_JingBiaoBest;
			bis >> src.m_JingBiaoBestTime;

			bis >> src.m_GuanJunTimes;
			bis >> src.m_GuanJunWinMoney;
			bis >> src.m_GuanJunBest;
			bis >> src.m_GuanJunBestTime;

			return bis;
		}
	};

	struct Game_MsgFlag
	{
		enum { XY_ID = MoGui_GAME_FLAG };
		enum FLAGVALUE
		{
			SUCCESS           = 0,
			LAND_OTHER_PLACE  = 1,
			CheckClientExist  = 2,
			LobbyReady        = 3,

			QuickStartGame    = 101,
		};

		unsigned int        m_Flag;
		unsigned int        m_nValue;

		Game_MsgFlag() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const Game_MsgFlag& MF )
		{
			bos << MF.m_Flag;
			bos << MF.m_nValue;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, Game_MsgFlag& MF )
		{
			MF.ReSet();
			bis >> MF.m_Flag;
			bis >> MF.m_nValue;
			return bis;
		}
	};

	struct Game_MsgRule
	{
		enum { XY_ID = MoGuiXYID_GameRule };

		std::string         m_strRule;

		Game_MsgRule() { ReSet(); }
		void ReSet()
		{
			m_strRule = "";
		}

		friend bostream& operator<<( bostream& bos, const Game_MsgRule& src )
		{
			InString(bos,src.m_strRule,MAX_MSG_LEN);
			return bos;
		}

		friend bistream& operator>>( bistream& bis, Game_MsgRule& src )
		{
			src.ReSet();
			OutString(bis,src.m_strRule,MAX_MSG_LEN);
			return bis;
		}
	};

	struct ReqJoinRoom
	{
		enum { XY_ID = MoGui_REQJOINROOM };

		unsigned short	m_RoomID;
		std::string     m_Password;

		ReqJoinRoom(){ ReSet(); }
		void ReSet() 
		{
			m_RoomID = 0;
			m_Password = "";
		}

		friend bostream& operator<<(bostream& bos,const ReqJoinRoom& jr)
		{		
			bos << jr.m_RoomID;			
			InString(bos,jr.m_Password,ROOM_PASSWORD_SIZE);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqJoinRoom& jr)
		{
			jr.ReSet();

			bis >> jr.m_RoomID;
			OutString(bis,jr.m_Password,ROOM_PASSWORD_SIZE);

			return bis;
		}
	};

	struct RespJoinRoom
	{
		enum { XY_ID = MoGui_RESPJOINROOM };
		enum JOINROOMFLAG 
		{
			SUCCESS           = 0,
			PLAYERFULL        = 1,            //本房间人数已满
			NOROOM            = 2,            //找不到要加入房间的ID
			AREADYINROOM      = 3,            //已经在本房间了
			PLAYERINPLAYING   = 4,            //正在其它房间的游戏桌上游戏中，不能换房间
			PASSWORDERROR     = 5,            //密码错误
		};
		enum JOINROOMTYPE
		{
			JOIN = 0,
			CHANGE = 1,
			ATTACH
		};

		unsigned char	m_Flag;
		unsigned char	m_Type;
		unsigned short	m_RoomID;
		unsigned char   m_ShowFlag;
		string          m_strMsg;

		RespJoinRoom(){ReSet();}
		void ReSet() 
		{
			m_Flag = 0;
			m_Type = 0;
			m_RoomID = 0;
			m_ShowFlag = 0;
			m_strMsg = "";
		}

		friend bostream& operator<<(bostream& bos,const RespJoinRoom& jr)
		{
			bos << jr.m_Flag;
			bos << jr.m_Type;
			bos << jr.m_RoomID;
			bos << jr.m_ShowFlag;
			InString(bos,jr.m_strMsg,250);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespJoinRoom& jr)
		{
			jr.ReSet();
			bis >> jr.m_Flag;
			bis >> jr.m_Type;
			bis >> jr.m_RoomID;
			bis >> jr.m_ShowFlag;
			OutString(bis,jr.m_strMsg,250);

			return bis;
		}
	};

	struct PlayerJoinRoomInfo
	{
		enum {XY_ID = MoGui_JOINROOM };

		unsigned short        m_RoomID;
		unsigned int          m_PID;

		PlayerJoinRoomInfo(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerJoinRoomInfo& jr)
		{
			bos << jr.m_RoomID;
			bos << jr.m_PID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerJoinRoomInfo& jr)
		{
			jr.ReSet();

			bis >> jr.m_RoomID;
			bis >> jr.m_PID;

			return bis;
		}
	};

	struct ReqLeaveRoom
	{
		enum { XY_ID = MoGui_REQLEAVEROOM };

		unsigned short	m_RoomID;		

		ReqLeaveRoom(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqLeaveRoom& rlr)
		{		
			bos << rlr.m_RoomID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqLeaveRoom& rlr)
		{
			rlr.ReSet();

			bis >> rlr.m_RoomID;

			return bis;
		}
	};

	struct RespLeaveRoom
	{
		enum { XY_ID = MoGui_RESPLEAVEROOM };
		enum JOINROOMFLAG 
		{
			SUCCESS           = 0,
			NOROOM            = 1,            //本房间人数已满
			NOTINROOM         = 2,            //找不到要加入房间的ID		
			PLAYERINPLAYING   = 3,            //正在其它房间的游戏桌上游戏中，不能换房间
		};

		unsigned char	m_Flag;		
		unsigned short	m_RoomID;

		RespLeaveRoom(){ ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespLeaveRoom& rlr)
		{
			bos << rlr.m_Flag;			
			bos << rlr.m_RoomID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespLeaveRoom& rlr)
		{
			rlr.ReSet();
			bis >> rlr.m_Flag;			
			bis >> rlr.m_RoomID;

			return bis;
		}
	};

	struct PlayerLeaveRoomInfo
	{
		enum {XY_ID = MoGui_LEAVEROOM };

		unsigned short        m_RoomID;
		unsigned int          m_PID;

		PlayerLeaveRoomInfo(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerLeaveRoomInfo& lr)
		{
			bos << lr.m_RoomID;
			bos << lr.m_PID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerLeaveRoomInfo& lr)
		{
			lr.ReSet();

			bis >> lr.m_RoomID;
			bis >> lr.m_PID;

			return bis;
		}
	};

	struct ReqJoinTable
	{
		enum { XY_ID = MoGui_REQJOINTABLE };

		unsigned short  m_RoomID;
		unsigned short	m_TableID;
		std::string     m_Password;

		ReqJoinTable(){ReSet();}
		void ReSet()
		{
			m_RoomID = 0;
			m_RoomID = 0;
			m_Password = "";
		}

		friend bostream& operator<<(bostream& bos,const ReqJoinTable& jt)
		{
			bos << jt.m_RoomID;
			bos << jt.m_TableID;			
			InString(bos,jt.m_Password,TABLE_PASSWORD_SIZE);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqJoinTable& jt)
		{
			jt.ReSet();

			bis >> jt.m_RoomID;
			bis >> jt.m_TableID;
			OutString(bis,jt.m_Password,TABLE_PASSWORD_SIZE);

			return bis;
		}
	};

	struct RespJoinTable
	{
		enum { XY_ID = MoGui_RESPJOINTABLE };
		enum JOINROOMFLAG 
		{
			SUCCESS        = 0,      //加入桌子成功
			PLAYERFULL     = 1,      //本桌人数已满，包括旁观的也满了
			NOTABLE        = 2,      //找不到要加入的桌子ID号
			INOTHERTABLE   = 3,      //玩家正在其它桌游戏，不能加入本桌
			NOTINROOM      = 4,      //不在要加入的房间内
			AREADYINTABLE  = 5,      //已经在桌子里面了
			PLAYERINPLAYING= 6,      //正在游戏中
			PASSWORDERROR  = 7,      //密码错误
			NOROOM         = 8,
			TABLEEND       = 9,

		};
		enum JOINROOMTYPE
		{
			JOIN = 0,
			CHANGE = 1,
			ATTACH
		};

		unsigned char	m_Flag;
		unsigned char	m_Type;
		unsigned short  m_RoomID;
		unsigned short	m_TableID;
		unsigned char   m_ShowFlag;
		string          m_strMsg;

		RespJoinTable(){ReSet();}
		void ReSet()
		{
			m_Flag = 0;
			m_Type = 0;
			m_RoomID = 0;
			m_TableID = 0;
			m_ShowFlag = 0;
			m_strMsg = "";
		}

		friend bostream& operator<<(bostream& bos,const RespJoinTable& jt)
		{
			bos << jt.m_Flag;
			bos << jt.m_Type;
			bos << jt.m_RoomID;
			bos << jt.m_TableID;
			bos << jt.m_ShowFlag;
			InString(bos,jt.m_strMsg,250);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespJoinTable& jt)
		{
			jt.ReSet();

			bis >> jt.m_Flag;
			bis >> jt.m_Type;
			bis >> jt.m_RoomID;
			bis >> jt.m_TableID;
			bis >> jt.m_ShowFlag;
			OutString(bis,jt.m_strMsg,250);

			return bis;
		}
	};

	struct PlayerJoinTableInfo
	{
		enum { XY_ID = MoGui_JOINTABLE };

		unsigned short  m_RoomID;
		unsigned short	m_TableID;
		unsigned int    m_PID;

		PlayerJoinTableInfo(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerJoinTableInfo& jt)
		{
		
			bos << jt.m_RoomID;
			bos << jt.m_TableID;
			bos << jt.m_PID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerJoinTableInfo& jt)
		{
			jt.ReSet();
		
			bis >> jt.m_RoomID;
			bis >> jt.m_TableID;
			bis >> jt.m_PID;

			return bis;
		}
	};

	struct ReqLeaveTable
	{
		enum {XY_ID = MoGui_REQLEAVETABLE };

		unsigned short               m_RoomID;
		unsigned short               m_TableID;

		ReqLeaveTable(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqLeaveTable& lt)
		{
			bos << lt.m_RoomID;
			bos << lt.m_TableID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqLeaveTable& lt)
		{
			bis >> lt.m_RoomID;
			bis >> lt.m_TableID;

			return bis;
		}
	};
	struct RespLeaveTable
	{
		enum {XY_ID = MoGui_RESPLEAVETABLE };

		unsigned char                m_Flag;
		unsigned short               m_RoomID;
		unsigned short               m_TableID;	

		RespLeaveTable(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespLeaveTable& lt)
		{
			bos << lt.m_Flag;
			bos << lt.m_RoomID;
			bos << lt.m_TableID;			

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespLeaveTable& lt)
		{
			bis >> lt.m_Flag;
			bis >> lt.m_RoomID;
			bis >> lt.m_TableID;		

			return bis;
		}
	};
	struct PlayerLeaveTableInfo
	{
		enum { XY_ID = MoGui_LEAVETABLE };

		unsigned short               m_RoomID;
		unsigned short               m_TableID;
		unsigned int                 m_PID;

		PlayerLeaveTableInfo(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerLeaveTableInfo& lt)
		{
			bos << lt.m_RoomID;
			bos << lt.m_TableID;
			bos << lt.m_PID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerLeaveTableInfo& lt)
		{
			bis >> lt.m_RoomID;
			bis >> lt.m_TableID;
			bis >> lt.m_PID;

			return bis;
		}
	};

	struct StartGameMsg
	{
		enum {XY_ID = MoGui_STARTGAME };

		unsigned char	m_CanStart;

		StartGameMsg(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const StartGameMsg& sg)
		{
			bos << sg.m_CanStart;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,StartGameMsg& sg)
		{
			bis >> sg.m_CanStart;

			return bis;
		}
	};

	struct ReqRoomInfo
	{
		enum { XY_ID = MoGui_REQROOMINFO };

		unsigned short  m_RoomID;

		ReqRoomInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const ReqRoomInfo& rri )
		{
			bos << rri.m_RoomID;
			return bos;
		}

		friend bistream& operator>>( bistream& bis, ReqRoomInfo& rri )
		{
			rri.ReSet();

			bis >> rri.m_RoomID;

			return bis;
		}
	};

	struct RespRoomInfo
	{
		enum { XY_ID = MoGui_RESPROOMINFO };

		enum REQROOMINFOFLAG 
		{
			SUCCESS = 0,
			NOROOM,
			NORIGHT,
			FAILURE
		};

		unsigned char   m_Flag;
		unsigned short  m_RoomID;

		RespRoomInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const RespRoomInfo& rri )
		{
			bos << rri.m_Flag;
			bos << rri.m_RoomID;
			return bos;
		}

		friend bistream& operator>>( bistream& bis, RespRoomInfo& rri )
		{
			rri.ReSet();

			bis >> rri.m_Flag;
			bis >> rri.m_RoomID;

			return bis;
		}
	};

	struct RoomInfo
	{
		enum { XY_ID = MoGui_ROOMINFO };

		unsigned short     m_RoomID;
		std::string        m_RoomName;
		std::string        m_RoomRule;

		unsigned short     m_MaxPlayerInRoom;
		unsigned short     m_CurPlayerInRoom;

		unsigned char      m_Place;                             //指房间摆放的位置
		unsigned char      m_RoomType;                          //排队模式还是自已坐的模式
		unsigned char      m_NeedPassword;                      //是否需要密码访问

		RoomInfo(){ ReSet(); }
		void ReSet()
		{
			m_RoomID = 0;
			m_RoomName = "";
			m_RoomRule = "";

			m_MaxPlayerInRoom = 0;
			m_MaxPlayerInRoom = 0;

			m_Place = 0;
			m_RoomType = 0;
			m_NeedPassword = 0;
		}

		friend bostream& operator<<(bostream& bos,const RoomInfo& ri)
		{
			bos << ri.m_RoomID;
			InString(bos,ri.m_RoomName,MAX_ROOMNAME_SIZE);
			InString(bos,ri.m_RoomRule,MAX_RULE_SIZE);

			bos << ri.m_MaxPlayerInRoom;
			bos << ri.m_CurPlayerInRoom;

			bos << ri.m_Place;
			bos << ri.m_RoomType;			
			bos << ri.m_NeedPassword;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RoomInfo& ri)
		{
			ri.ReSet();

			bis >> ri.m_RoomID;
			OutString(bis,ri.m_RoomName,MAX_ROOMNAME_SIZE);
			OutString(bis,ri.m_RoomRule,MAX_RULE_SIZE);

			bis >> ri.m_MaxPlayerInRoom;
			bis >> ri.m_CurPlayerInRoom;

			bis >> ri.m_Place;
			bis >> ri.m_RoomType;			
			bis >> ri.m_NeedPassword;	

			return bis;
		}

		RoomInfo( const RoomInfo& ri)
		{
			Copy(ri);
		}

		RoomInfo& operator=( const RoomInfo& ri)
		{
			if(this == &ri)
			{
				return *this;
			}

			Copy(ri);

			return *this;
		}

		void Copy( const RoomInfo& ri )
		{
			m_RoomID = ri.m_RoomID;
			m_RoomName = ri.m_RoomName;
			m_RoomRule = ri.m_RoomRule;

			m_MaxPlayerInRoom       = ri.m_MaxPlayerInRoom;
			m_CurPlayerInRoom       = ri.m_CurPlayerInRoom;

			m_Place           = ri.m_Place;
			m_RoomType        = ri.m_RoomType;
			m_NeedPassword    = ri.m_NeedPassword;
		}
	};

	struct RoomInfoList
	{
		enum { XY_ID = MoGui_ROOMINIFOLIST };
		enum { MAX_ROOMLIST = 20 };

		unsigned char      m_nCount;
		RoomInfo           m_RoomList[MAX_ROOMLIST];

		RoomInfoList(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RoomInfoList& ril)
		{
			bos << ril.m_nCount;			
			for(int i=0;i<ril.m_nCount && i<MAX_ROOMLIST;i++)
			{
				bos << ril.m_RoomList[i];
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RoomInfoList& ril)
		{
			ril.ReSet();

			bis >> ril.m_nCount;
			for(int i=0;i<ril.m_nCount && i<MAX_ROOMLIST;i++)
			{
				bis >> ril.m_RoomList[i];
			}	

			return bis;
		}
	};

	struct ReqRoomTableInfo
	{
		enum { XY_ID = MoGui_REQROOMTABLEINFO };

		unsigned short  m_RoomID;

		ReqRoomTableInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const ReqRoomTableInfo& rrti )
		{
			bos << rrti.m_RoomID;
			return bos;
		}

		friend bistream& operator>>( bistream& bis, ReqRoomTableInfo& rrti )
		{
			rrti.ReSet();

			bis >> rrti.m_RoomID;

			return bis;
		}
	};

	struct RespRoomTableInfo
	{
		enum { XY_ID = MoGui_RESPROOMTABLEINFO };

		enum
		{
			SUCCESS = 0,
			NOROOM,
			NORIGHT,
			REQAGAIN                  //重复请求
		};

		unsigned char   m_Flag;
		unsigned short  m_RoomID;

		RespRoomTableInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const RespRoomTableInfo& rrti )
		{
			bos << rrti.m_Flag;
			bos << rrti.m_RoomID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, RespRoomTableInfo& rrti )
		{
			rrti.ReSet();

			bis >> rrti.m_Flag;
			bis >> rrti.m_RoomID;

			return bis;
		}
	};

	struct TableInfo
	{
		enum { XY_ID = MoGui_TABLEINFO };
		
		unsigned short     		m_RoomID;
		unsigned short     		m_TableID;
		unsigned char           m_TableType;
		std::string       		m_TableName;
		std::string             m_TableRule;

		unsigned char      		m_MaxSitPlayerNumber;                            //最大能一起玩的玩家
		unsigned char      		m_CurSitPlayerNumber;                            //当前坐下的玩家
		unsigned short    		m_CurStandPlayerNumber;                          //当前旁观的玩家

		TableInfo(){ ReSet(); }
		void ReSet()
		{
			m_RoomID = 0;
			m_TableID = 0;
			m_TableType = 0;

			m_TableName = "";
			m_TableRule = "";

			m_MaxSitPlayerNumber = 0;
			m_CurSitPlayerNumber = 0;
			m_CurStandPlayerNumber = 0;
		}

		friend bostream& operator<<(bostream& bos,const TableInfo& ti)
		{
			bos << ti.m_RoomID;
			bos << ti.m_TableID;
			bos << ti.m_TableType;
			InString(bos,ti.m_TableName,MAX_TABLENAME_SIZE);
			InString(bos,ti.m_TableRule,MAX_RULE_SIZE);

			bos << ti.m_MaxSitPlayerNumber;
			bos << ti.m_CurSitPlayerNumber;
			bos << ti.m_CurStandPlayerNumber;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,TableInfo& ti)
		{
			ti.ReSet();

			bis >> ti.m_RoomID;
			bis >> ti.m_TableID;
			bis >> ti.m_TableType;
			OutString(bis,ti.m_TableName,MAX_TABLENAME_SIZE);
			OutString(bis,ti.m_TableRule,MAX_RULE_SIZE);
		
			bis >> ti.m_MaxSitPlayerNumber;
			bis >> ti.m_CurSitPlayerNumber;
			bis >> ti.m_CurStandPlayerNumber;

			return bis;
		}
	};

	struct TableInfoList
	{
		enum { XY_ID = MoGui_TABLEINFOLIST };
		enum { MAX_TABLElIST = 20 };

		unsigned short     m_RoomID;
		vector<TableInfo>  m_TableList;

		TableInfoList(){ ReSet();}
		void ReSet()
		{
			m_RoomID = 0;
			m_TableList.clear();
		}

		friend bostream& operator<<(bostream& bos,const TableInfoList& til)
		{
			bos << til.m_RoomID;			
			InVector(bos,til.m_TableList,til.MAX_TABLElIST);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,TableInfoList& til)
		{
			til.ReSet();

			bis >> til.m_RoomID;			
			OutVector(bis,til.m_TableList,til.MAX_TABLElIST);

			return bis;
		}
	};

	struct TableShow
	{
		enum { XY_ID = MoGuiXYID_ShowTable };
		enum { TABLE_HIDE = 0, TABLE_SHOW = 1 };
		
		unsigned short     		m_RoomID;
		unsigned short     		m_TableID;
		unsigned char           m_ShowFlag;

		TableShow(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const TableShow& ts)
		{
			bos << ts.m_RoomID;
			bos << ts.m_TableID;
			bos << ts.m_ShowFlag;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,TableShow& ts)
		{
			ts.ReSet();

			bis >> ts.m_RoomID;
			bis >> ts.m_TableID;
			bis >> ts.m_ShowFlag;

			return bis;
		}
	};

	struct TableShowList
	{
		enum { XY_ID = MoGuiXYID_ShowTableList };
		enum { TABLE_HIDE = 0, TABLE_SHOW = 1 };
		enum { MAX_TABLElIST = 250 };

		unsigned short          m_RoomID;
		unsigned char           m_ShowFlag;
		vector<unsigned short>  m_vecTableID;

		TableShowList(){ ReSet(); }
		void ReSet()
		{
			m_RoomID = 0;
			m_ShowFlag = TABLE_HIDE;			
			m_vecTableID.clear();
		}

		friend bostream& operator<<(bostream& bos,const TableShowList& tsl)
		{
			bos << tsl.m_RoomID;
			bos << tsl.m_ShowFlag;
			InVector(bos,tsl.m_vecTableID,tsl.MAX_TABLElIST);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,TableShowList& tsl)
		{
			tsl.ReSet();

			bis >> tsl.m_RoomID;
			bis >> tsl.m_ShowFlag;
			OutVector(bis,tsl.m_vecTableID,tsl.MAX_TABLElIST);

			return bis;
		}
	};

	struct ReqPlayerAct
	{
		enum { XY_ID = MoGui_REQPLAYERACT };

		enum ACTION{ SITDOWN = 1,STANDUP,CHANGESIT };

		unsigned char	        m_Action;
		unsigned char	        m_SitID;
		unsigned short	        m_TableID;

		SBigNumber              m_nTakeMoney;   

		ReqPlayerAct(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqPlayerAct& pa)
		{
			bos << pa.m_Action;
			bos << pa.m_SitID;
			bos << pa.m_TableID;

			if( pa.m_Action == SITDOWN )
			{
				bos << pa.m_nTakeMoney;
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqPlayerAct& pa)
		{
			pa.ReSet();

			bis >> pa.m_Action;
			bis >> pa.m_SitID;
			bis >> pa.m_TableID;

			if( pa.m_Action == SITDOWN )
			{
				bis >> pa.m_nTakeMoney;
			}

			return bis;
		}
	};
	struct RespPlayerAct
	{
		enum { XY_ID = MoGui_RESPPLAYERACT };

		enum
		{
			SUCCESS,
			ACTFAILED,
			NOTINROOM,                //不在房间里面
			NOTINTABLE,               //不在桌子里面
			SITISNOTEMPTY,            //已经有人了
			OUTOFSITID,               //坐位号超出
			NOTINSIT,                 //不在该位置上
			LESSMONEY,                //所带的钱不够最低坐下条件
			OVERFULLMONEY,            //所带的钱超出最大条件
			OUTOFOWN,                 //超出自己拥有的钱
			VisitorCantPlay,          //游客不能在本桌游戏
			Match,                    //比赛桌不能换位置
			Gameing,                  //游戏中不能换位置
		};

		unsigned char m_Flag;
		unsigned char m_ShowFlag;
		string        m_strMsg;

		RespPlayerAct(){ ReSet();}
		void ReSet() 
		{
			m_Flag = 0;
			m_ShowFlag = 0;
			m_strMsg = "";
		}

		friend bostream& operator<<(bostream& bos,const RespPlayerAct& pa)
		{
			bos << pa.m_Flag;
			bos << pa.m_ShowFlag;
			InString(bos,pa.m_strMsg,250);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespPlayerAct& pa)
		{
			pa.ReSet();

			bis >> pa.m_Flag;
			bis >> pa.m_ShowFlag;
			OutString(bis,pa.m_strMsg,250);

			return bis;
		}
	};

	struct PlayerAct
	{
		enum { XY_ID = MoGui_PLAYERACT };
		enum ACTION{ SITDOWN = 1,STANDUP };

		unsigned short	    m_TableID;
		unsigned char	    m_SitID;
		unsigned char       m_Action;
		unsigned int        m_PID;

		PlayerAct(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerAct& pa)
		{
			bos << pa.m_TableID;
			bos << pa.m_SitID;
			bos << pa.m_Action;
			bos << pa.m_PID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerAct& pa)
		{
			pa.ReSet();

			bis >> pa.m_TableID;
			bis >> pa.m_SitID;
			bis >> pa.m_Action;
			bis >> pa.m_PID;

			return bis;
		}
	};

	struct ReqTracePlayer
	{
		enum { XY_ID = MoGuiXYID_ReqTracePlayer };
		enum { Friend=1,SameCity,SameRoom,SameTable,ForMatch, };

		unsigned char       m_Flag;
		unsigned int        m_PID;

		ReqTracePlayer(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqTracePlayer& src)
		{
			bos << src.m_Flag;
			bos << src.m_PID;
			
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqTracePlayer& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			bis >> src.m_PID;

			return bis;
		}
	};

	struct RespTracePlayer
	{
		enum { XY_ID = MoGuiXYID_RespTracePlayer };
		enum 
		{
			Success = 0,
			Failed,
			NotSitOnTable,
			PlayerInRoomMatch,
			InSameTable,
			NotInRoom,
			NotInTable,
		};

		unsigned char       m_Flag;
		unsigned int        m_PID;
		unsigned char       m_ShowFlag;
		string              m_strMsg;

		RespTracePlayer(){ ReSet(); }
		void ReSet()
		{
			m_Flag = 0;
			m_PID = 0;
			m_ShowFlag = 0;
			m_strMsg = "";
		}

		friend bostream& operator<<(bostream& bos,const RespTracePlayer& src)
		{
			bos << src.m_Flag;
			bos << src.m_PID;
			bos << src.m_ShowFlag;
			InString(bos,src.m_strMsg,250);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespTracePlayer& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			bis >> src.m_PID;
			bis >> src.m_ShowFlag;
			OutString(bis,src.m_strMsg,250);

			return bis;
		}
	};

	struct ChangePlace
	{
		enum { XY_ID = MoGuiXYID_ChangePlace };

		unsigned char        m_Place;

		ChangePlace(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ChangePlace& src)
		{
			bos << src.m_Place;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ChangePlace& src)
		{
			src.ReSet();
			bis >> src.m_Place;
			return bis;
		}
	};

	struct ReqRoomPlayerCount
	{
		enum {XY_ID = MoGui_REQROOMPLAYERCOUNT };

		unsigned short    m_RoomID;

		ReqRoomPlayerCount(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqRoomPlayerCount& rpc)
		{
			bos << rpc.m_RoomID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqRoomPlayerCount& rpc)
		{
			rpc.ReSet();

			bis >> rpc.m_RoomID;
			return bis;
		}
	};

	struct RoomPlayerCount
	{
		enum {XY_ID = MoGui_ROOMPLAYERCOUNT };

		unsigned short	m_RoomID;
		unsigned short	m_nCount;

		RoomPlayerCount(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RoomPlayerCount& rpc)
		{
			bos << rpc.m_RoomID;
			bos << rpc.m_nCount;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,RoomPlayerCount& rpc)
		{
			rpc.ReSet();

			bis >> rpc.m_RoomID;
			bis >> rpc.m_nCount;
			return bis;
		}

		RoomPlayerCount( const RoomPlayerCount& rpc)
		{
			Copy(rpc);
		}

		RoomPlayerCount& operator=( const RoomPlayerCount& rpc)
		{
			if(this == &rpc)
			{
				return *this;
			}

			Copy(rpc);

			return *this;
		}

		void Copy( const RoomPlayerCount& rpc )
		{
			m_RoomID                = rpc.m_RoomID;
			m_nCount                = rpc.m_nCount;
		}
	};

	struct RoomPlayerCountList
	{
		enum {XY_ID = MoGui_ROOMPLAYERCOUNTLIST };

		enum { MAX_ROOMPALYERCOUNTLIST = 200 };

		unsigned char      m_nCount;
		RoomPlayerCount    m_RoomPlayerCountList[MAX_ROOMPALYERCOUNTLIST];

		RoomPlayerCountList(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RoomPlayerCountList& rpc)
		{
			bos << rpc.m_nCount;			
			for(int i=0;i<rpc.m_nCount && i<MAX_ROOMPALYERCOUNTLIST;i++)
			{
				bos << rpc.m_RoomPlayerCountList[i];
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RoomPlayerCountList& rpc)
		{
			rpc.ReSet();

			bis >> rpc.m_nCount;
			for(int i=0;i<rpc.m_nCount && i<MAX_ROOMPALYERCOUNTLIST;i++)
			{
				bis >> rpc.m_RoomPlayerCountList[i];
			}	

			return bis;
		}
	};

	struct ReqGamePlayerCount
	{
		enum {XY_ID = MoGui_REQGAMEPLAYERCOUNT };

		short    m_GameID;

		ReqGamePlayerCount(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqGamePlayerCount& rgpc)
		{
			bos << rgpc.m_GameID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqGamePlayerCount& rgpc)
		{
			rgpc.ReSet();

			bis >> rgpc.m_GameID;
			return bis;
		}
	};

	struct GamePlayerCount
	{
		enum {XY_ID = MoGui_GAMEPLAYERCOUNT };

		short				m_GameID;
		int         	    m_nCount;

		GamePlayerCount(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const GamePlayerCount& gpc)
		{
			bos << gpc.m_GameID;
			bos << gpc.m_nCount;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GamePlayerCount& gpc)
		{
			gpc.ReSet();

			bis >> gpc.m_GameID;
			bis >> gpc.m_nCount;
			return bis;
		}
	};

	struct MatchRoomPlayerCount
	{
		enum {XY_ID = MoGui_MatchRoomPlayerCount };

		short				m_RoomID;
		short         	    m_nSignPlayer;
		short               m_nLeftPlayer;

		MatchRoomPlayerCount(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const MatchRoomPlayerCount& src)
		{
			bos << src.m_RoomID;
			bos << src.m_nSignPlayer;
			bos << src.m_nLeftPlayer;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,MatchRoomPlayerCount& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;
			bis >> src.m_nSignPlayer;
			bis >> src.m_nLeftPlayer;
			return bis;
		}
	};

	struct ReqTablePlayerCount
	{
		enum {XY_ID = MoGui_REQTABLEPLAYERCOUNT };

		unsigned short    m_RoomID;
		unsigned short    m_TableID;

		ReqTablePlayerCount(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqTablePlayerCount& rtpc)
		{
			bos << rtpc.m_RoomID;
			bos << rtpc.m_TableID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqTablePlayerCount& rtpc)
		{
			rtpc.ReSet();

			bis >> rtpc.m_RoomID;
			bis >> rtpc.m_TableID;

			return bis;
		}
	};

	struct TablePlayerCount
	{
		enum {XY_ID = MoGui_TABLEPLAYERCOUNT };

		unsigned short	m_RoomID;
		unsigned short  m_TableID;
		unsigned char	m_nPlayingPlayerCount;
		unsigned short  m_nSeeingPlayerCount;

		TablePlayerCount(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const TablePlayerCount& tpc)
		{
			bos << tpc.m_RoomID;
			bos << tpc.m_TableID;
			bos << tpc.m_nPlayingPlayerCount;
			bos << tpc.m_nSeeingPlayerCount;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,TablePlayerCount& tpc)
		{
			tpc.ReSet();

			bis >> tpc.m_RoomID;
			bis >> tpc.m_TableID;
			bis >> tpc.m_nPlayingPlayerCount;
			bis >> tpc.m_nSeeingPlayerCount;
			return bis;
		}


		TablePlayerCount( const TablePlayerCount& tpc)
		{
			Copy(tpc);
		}

		TablePlayerCount& operator=( const TablePlayerCount& tpc)
		{
			if(this == &tpc)
			{
				return *this;
			}

			Copy(tpc);

			return *this;
		}

		void Copy( const TablePlayerCount& tpc )
		{
			m_RoomID                 = tpc.m_RoomID;
			m_TableID                = tpc.m_TableID;
			m_nPlayingPlayerCount    = tpc.m_nPlayingPlayerCount;
			m_nSeeingPlayerCount     = tpc.m_nSeeingPlayerCount;
		}
	};

	struct msgTablePC
	{
		unsigned short  m_TableID;
		unsigned char	m_nPlayingPlayerCount;
		unsigned short  m_nSeeingPlayerCount;

		msgTablePC(){ ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const msgTablePC& tpc)
		{
			bos << tpc.m_TableID;
			bos << tpc.m_nPlayingPlayerCount;
			bos << tpc.m_nSeeingPlayerCount;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,msgTablePC& tpc)
		{
			tpc.ReSet();

			bis >> tpc.m_TableID;
			bis >> tpc.m_nPlayingPlayerCount;
			bis >> tpc.m_nSeeingPlayerCount;
			return bis;
		}

		msgTablePC( const msgTablePC& tpc)
		{
			Copy(tpc);
		}

		msgTablePC& operator=( const msgTablePC& tpc)
		{
			if(this == &tpc)
			{
				return *this;
			}

			Copy(tpc);

			return *this;
		}

		void Copy( const msgTablePC& tpc )
		{
			m_TableID                = tpc.m_TableID;
			m_nPlayingPlayerCount    = tpc.m_nPlayingPlayerCount;
			m_nSeeingPlayerCount     = tpc.m_nSeeingPlayerCount;
		}
	};

	struct TablePlayerCountList
	{
		enum {XY_ID = MoGui_TABLEPLAYERCOUNTLIST };
		enum { MAX_TABLEPALYERCOUNTLIST = 400 };

		unsigned short             m_RoomID;
		mutable unsigned short     m_nCount;
		vector<msgTablePC>         m_listTablePC;

		TablePlayerCountList(){	ReSet(); }
		void ReSet() 
		{
			m_RoomID = 0;
			m_nCount = 0;
			m_listTablePC.clear();
		}

		friend bostream& operator<<(bostream& bos,const TablePlayerCountList& src)
		{
			src.m_nCount = min(short(src.m_listTablePC.size()),src.MAX_TABLEPALYERCOUNTLIST);

			bos << src.m_RoomID;
			bos << src.m_nCount;
		
			for (int i=0;i<src.m_nCount;++i)
			{
				bos << src.m_listTablePC[i];
			}			

			return bos;
		}
		friend bistream& operator>>(bistream& bis,TablePlayerCountList& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;
			bis >> src.m_nCount;

			if ( src.m_nCount > src.MAX_TABLEPALYERCOUNTLIST )
			{
				throw agproexception(agproexception::rangeerror);
			}

			msgTablePC msgPC;			
			for(int i=0;i<src.m_nCount;i++)
			{
				bis >> msgPC;
				src.m_listTablePC.push_back(msgPC);
			}	

			return bis;
		}
	};
	struct ReqTablePlayerBase
	{
		enum {XY_ID = GameLobbyXYID_ReqTablePlayerBase };

		unsigned short    m_RoomID;
		unsigned short    m_TableID;

		ReqTablePlayerBase(){ ReSet(); }
		void ReSet() { m_RoomID = 0;m_TableID=0; }

		friend bostream& operator<<(bostream& bos,const ReqTablePlayerBase& src)
		{
			bos << src.m_RoomID;
			bos << src.m_TableID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqTablePlayerBase& src)
		{
			src.ReSet();
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			return bis;
		}
	};

	struct ReqRoomPlayerBase
	{
		enum {XY_ID = GameLobbyXYID_ReqRoomPlayerBase };

		unsigned short    m_RoomID;

		ReqRoomPlayerBase(){ ReSet(); }
		void ReSet() { m_RoomID = 0; }

		friend bostream& operator<<(bostream& bos,const ReqRoomPlayerBase& src)
		{
			bos << src.m_RoomID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqRoomPlayerBase& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;
			return bis;
		}
	};
	struct ReqRoomPlayerState
	{
		enum {XY_ID = GameLobbyXYID_ReqRoomPlayerState };

		unsigned short    m_RoomID;

		ReqRoomPlayerState(){ ReSet(); }
		void ReSet() { m_RoomID = 0; }

		friend bostream& operator<<(bostream& bos,const ReqRoomPlayerState& src)
		{
			bos << src.m_RoomID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqRoomPlayerState& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;
			return bis;
		}
	};
	struct ReqCityPlayerBase
	{
		enum {XY_ID = GameLobbyXYID_ReqCityPlayerBase };

		unsigned int    m_PID;

		ReqCityPlayerBase(){ ReSet(); }
		void ReSet() { m_PID = 0; }

		friend bostream& operator<<(bostream& bos,const ReqCityPlayerBase& src)
		{
			bos << src.m_PID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqCityPlayerBase& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			return bis;
		}
	};
	struct ReqCityPlayerState
	{
		enum {XY_ID = GameLobbyXYID_ReqCityPlayerState };

		unsigned int    m_PID;

		ReqCityPlayerState(){ ReSet();}
		void ReSet() { m_PID = 0; }

		friend bostream& operator<<(bostream& bos,const ReqCityPlayerState& src)
		{
			bos << src.m_PID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqCityPlayerState& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			return bis;
		}
	};

	struct ChangeUserInfo
	{
		enum { XY_ID = GameLobbyXYID_ChangeUserInfo };

		unsigned int    m_PID;
		short           m_Year;
		BYTE            m_Sex;

		string          m_NickName;
		string          m_HeadPicUrl;
		string          m_City;

		ChangeUserInfo(){ ReSet(); }
		void ReSet()
		{
			m_PID = 0;
			m_Year = 0;
			m_Sex = 0;

			m_NickName = "";
			m_HeadPicUrl = "";
			m_City = "";
		}

		friend bostream& operator<<(bostream& bos,const ChangeUserInfo& src)
		{
			bos << src.m_PID;
			bos << src.m_Year;
			bos << src.m_Sex;

			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicUrl,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ChangeUserInfo& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_Year;
			bis >> src.m_Sex;

			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicUrl,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			return bis;
		}
	};

	struct RespChangeUserInfo
	{
		enum { XY_ID = GameLobbyXYID_RespChangeUserInfo };
		enum 
		{
			SUCCESS = 0,

			Error_Sex,
			Error_City,
			Error_NickName,
			Error_HeadPicUrl,
		};

		BYTE            m_Flag;
		unsigned int    m_PID;

		BYTE            m_Sex;
		short           m_Year;
		string          m_NickName;
		string          m_HeadPicUrl;
		string          m_City;

		RespChangeUserInfo(){ ReSet(); }
		void ReSet()
		{
			m_Flag = 0;
			m_PID = 0;

			m_Sex = 0;
			m_Year = 0;
			m_NickName = "";
			m_HeadPicUrl = "";
			m_City = "";
		}

		friend bostream& operator<<(bostream& bos,const RespChangeUserInfo& src)
		{
			bos << src.m_Flag;
			bos << src.m_PID;

			if ( src.m_Flag == src.SUCCESS )
			{
				bos << src.m_Sex;
				bos << src.m_Year;
				InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
				InString(bos,src.m_HeadPicUrl,MAX_URL_SIZE);
				InString(bos,src.m_City,MAX_CITYNAME_SIZE);
			}			

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespChangeUserInfo& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			bis >> src.m_PID;

			if ( src.m_Flag == src.SUCCESS )
			{
				bis >> src.m_Sex;
				bis >> src.m_Year;
				OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
				OutString(bis,src.m_HeadPicUrl,MAX_URL_SIZE);
				OutString(bis,src.m_City,MAX_CITYNAME_SIZE);
			}

			return bis;
		}
	};

	struct ReqPlayerAward
	{
		enum {XY_ID = GameLobbyXYID_ReqPlayerAward };

		short           m_AID;
		unsigned int    m_PID;

		ReqPlayerAward(){ ReSet();}
		void ReSet(){ m_PID=0;m_AID=0; }

		friend bostream& operator<<(bostream& bos,const ReqPlayerAward& src)
		{
			bos << src.m_AID;
			bos << src.m_PID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqPlayerAward& src)
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			return bis;
		}
	};

	struct ReqFollowPlayer
	{
		enum { XY_ID = GameLobbyXYID_ReqFollowPlayer };
		enum { Follow,UnFollow,};

		unsigned char   m_Flag;
		unsigned int    m_MyPID;
		unsigned int    m_FollowPID;

		ReqFollowPlayer(){ ReSet();}
		void ReSet(){ memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqFollowPlayer& src)
		{
			bos << src.m_Flag;
			bos << src.m_MyPID;
			bos << src.m_FollowPID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqFollowPlayer& src)
		{
			src.ReSet();
			bis >> src.m_Flag;
			bis >> src.m_MyPID;
			bis >> src.m_FollowPID;
			return bis;
		}
	};
	struct RespFollowPlayer
	{
		enum {XY_ID = GameLobbyXYID_RespFollowPlayer };
		enum { Follow,UnFollow,};

		unsigned char   m_Flag;
		unsigned int    m_MyPID;
		unsigned int    m_FollowPID;

		RespFollowPlayer(){ ReSet();}
		void ReSet(){ memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespFollowPlayer& src)
		{
			bos << src.m_Flag;
			bos << src.m_MyPID;
			bos << src.m_FollowPID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespFollowPlayer& src)
		{
			src.ReSet();
			bis >> src.m_Flag;
			bis >> src.m_MyPID;
			bis >> src.m_FollowPID;
			return bis;
		}
	};

	struct ReqPlayerWinLoseRank
	{
		enum { XY_ID = MoGui_ReqPlayerWinLoseRank };

		unsigned short  m_RoomID;

		ReqPlayerWinLoseRank(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const ReqPlayerWinLoseRank& rpr )
		{
			bos << rpr.m_RoomID;
			return bos;
		}

		friend bistream& operator>>( bistream& bis, ReqPlayerWinLoseRank& rpr )
		{
			rpr.ReSet();

			bis >> rpr.m_RoomID;

			return bis;
		}
	};

	struct RespPlayerWinLoseRank
	{
		enum { XY_ID = MoGui_RespPlayerWinLoseRank };

		unsigned char   m_Flag;
		unsigned short  m_RoomID;

		RespPlayerWinLoseRank(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const RespPlayerWinLoseRank& rspr )
		{
			bos << rspr.m_Flag;
			bos << rspr.m_RoomID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, RespPlayerWinLoseRank& rspr )
		{
			rspr.ReSet();

			bis >> rspr.m_Flag;
			bis >> rspr.m_RoomID;

			return bis;
		}
	};

	struct msgPlayerWinLoseRank
	{
		unsigned int			    m_PID;
		SBigNumber                  m_nWinMoney;                               //有正负之分	

		msgPlayerWinLoseRank(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const msgPlayerWinLoseRank& pr)
		{
			bos << pr.m_PID;
			bos << pr.m_nWinMoney;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,msgPlayerWinLoseRank& pr)
		{
			pr.ReSet();

			bis >> pr.m_PID;
			bis >> pr.m_nWinMoney;

			return bis;
		}

		msgPlayerWinLoseRank( const msgPlayerWinLoseRank& pr)
		{
			Copy(pr);
		}
		msgPlayerWinLoseRank& operator=( const msgPlayerWinLoseRank& pr)
		{
			if(this == &pr)
			{
				return *this;
			}

			Copy(pr);

			return *this;
		}
		void Copy( const msgPlayerWinLoseRank& pr )
		{
			m_PID       = pr.m_PID;
			m_nWinMoney = pr.m_nWinMoney;
		}
	};

	struct PlayerWinLoseRankList
	{
		enum { XY_ID = MoGui_PlayerWinLoseRankList };
		enum { MAX_PLAYERRANKLIST = 30 };

		unsigned short     			m_RoomID;	                              //哪个房间的排名，当为0的时候表示全游戏
		unsigned char      			m_nCount;                                 //消息的总数，不超过30条，以后会有调整
		msgPlayerWinLoseRank        m_PlayerRankList[MAX_PLAYERRANKLIST];     //具体的数据结构

		PlayerWinLoseRankList(){ ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerWinLoseRankList& prl)
		{			
			bos << prl.m_RoomID;
			bos << prl.m_nCount;
			for(int i=0;i<prl.m_nCount && i<MAX_PLAYERRANKLIST;i++)
			{
				bos << prl.m_PlayerRankList[i];
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerWinLoseRankList& prl)
		{
			prl.ReSet();

			bis >> prl.m_RoomID;
			bis >> prl.m_nCount;
			for(int i=0;i<prl.m_nCount && i<MAX_PLAYERRANKLIST;i++)
			{
				bis >> prl.m_PlayerRankList[i];	
			}

			return bis;
		}
	};

	struct PlayerBaseInfo
	{
		enum { XY_ID = MoGui_PLAYERBASEINFO };
		enum { TablePlayer,Friend,SameCity,SameRoom,SameTable,CanFollow,AddFriend };

		unsigned char            m_Flag;
		short                    m_AID;
		unsigned int             m_PID;
		SBigNumber               m_nGameMoney;
		unsigned char            m_PlayerState;
		unsigned char            m_GameLevel;

		std::string              m_NickName;
		std::string              m_HeadPicURL;

		PlayerBaseInfo(){ReSet();}
		void ReSet()
		{
			m_Flag = 0;
			m_AID = 0;
			m_PID = 0;
			m_nGameMoney = 0;
			m_PlayerState = 0;
			m_GameLevel = 0;

			m_NickName = "";
			m_HeadPicURL = "";
		}

		friend bostream& operator<<(bostream& bos,const PlayerBaseInfo& pbi)
		{
			bos << pbi.m_Flag;
			bos << pbi.m_AID;
			bos << pbi.m_PID;
			bos << pbi.m_nGameMoney;
			bos << pbi.m_PlayerState;
			bos << pbi.m_GameLevel;
			InString(bos,pbi.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,pbi.m_HeadPicURL,MAX_URL_SIZE);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerBaseInfo& pbi)
		{
			pbi.ReSet();

			bis >> pbi.m_Flag;
			bis >> pbi.m_AID;
			bis >> pbi.m_PID;
			bis >> pbi.m_nGameMoney;
			bis >> pbi.m_PlayerState;
			bis >> pbi.m_GameLevel;
			OutString(bis,pbi.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,pbi.m_HeadPicURL,MAX_URL_SIZE);

			return bis;
		}

		PlayerBaseInfo( const PlayerBaseInfo& pbi)
		{
			Copy(pbi);
		}

		PlayerBaseInfo& operator=( const PlayerBaseInfo& pbi)
		{
			if( this == &pbi )
			{
				return *this;
			}

			Copy(pbi);

			return *this;
		}

		void Copy( const PlayerBaseInfo& pbi )
		{
			m_Flag            = pbi.m_Flag;
			m_AID             = pbi.m_AID;
			m_PID             = pbi.m_PID;
			m_nGameMoney      = pbi.m_nGameMoney;
			m_PlayerState     = pbi.m_PlayerState;
			m_GameLevel       = pbi.m_GameLevel;
			m_NickName        = pbi.m_NickName;
			m_HeadPicURL      = pbi.m_HeadPicURL;
		}
	};

	struct PlayerGameInfo
	{
		enum { XY_ID = MoGui_PLAYERGAMEINFO };

		unsigned int             m_PID;

		SBigNumber               m_nGameMoney;
		SBigNumber               m_nJF;
		SBigNumber               m_nExperience;

		SBigNumber               m_nWinTimes;
		SBigNumber               m_nLossTimes;

		PlayerGameInfo(){ReSet();}
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerGameInfo& pgi)
		{
			bos << pgi.m_PID;
			bos << pgi.m_nGameMoney;
			bos << pgi.m_nJF;
			bos << pgi.m_nExperience;
			bos << pgi.m_nWinTimes;
			bos << pgi.m_nLossTimes;		

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerGameInfo& pgi)
		{
			pgi.ReSet();

			bis >> pgi.m_PID;
			bis >> pgi.m_nGameMoney;
			bis >> pgi.m_nJF;
			bis >> pgi.m_nExperience;
			bis >> pgi.m_nWinTimes;
			bis >> pgi.m_nLossTimes;

			return bis;
		}

		PlayerGameInfo( const PlayerGameInfo& pgi)
		{
			Copy(pgi);
		}

		PlayerGameInfo& operator=( const PlayerGameInfo& pgi)
		{
			if( this == &pgi )
			{
				return *this;
			}

			Copy(pgi);

			return *this;
		}

		void Copy( const PlayerGameInfo& pgi )
		{
			m_PID              = pgi.m_PID;
			m_nGameMoney       = pgi.m_nGameMoney;
			m_nJF              = pgi.m_nJF;
			m_nExperience      = pgi.m_nExperience;
			m_nWinTimes        = pgi.m_nWinTimes;
			m_nLossTimes       = pgi.m_nLossTimes;
		}
	};

	struct PlayerStateInfo
	{
		enum { XY_ID = MoGui_PLAYERSTATEINFO };
		enum { TablePlayer,Friend,SameCity,SameRoom,SameTable };

		unsigned char            m_Flag;
		unsigned int             m_PID;

		unsigned short           m_RoomID;
		unsigned short           m_TableID;
		unsigned char            m_SitID;
		unsigned char            m_PlayerState;
		string                   m_strState;

		PlayerStateInfo(){ReSet();}
		void ReSet() 
		{
			m_Flag = 0;
			m_PID = 0;

			m_RoomID = 0;
			m_TableID = 0;
			m_SitID = 0;
			m_PlayerState = 0;
			m_strState = "";
		}

		friend bostream& operator<<(bostream& bos,const PlayerStateInfo& psi)
		{
			bos << psi.m_Flag;
			bos << psi.m_PID;
			bos << psi.m_RoomID;
			bos << psi.m_TableID;
			bos << psi.m_SitID;
			bos << psi.m_PlayerState;
			InString(bos,psi.m_strState,50);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerStateInfo& psi)
		{
			psi.ReSet();

			bis >> psi.m_Flag;
			bis >> psi.m_PID;
			bis >> psi.m_RoomID;
			bis >> psi.m_TableID;
			bis >> psi.m_SitID;
			bis >> psi.m_PlayerState;
			OutString(bis,psi.m_strState,50);

			return bis;
		}

		PlayerStateInfo( const PlayerStateInfo& psi)
		{
			Copy(psi);
		}

		PlayerStateInfo& operator=( const PlayerStateInfo& psi)
		{
			if(this == &psi)
			{
				return *this;
			}

			Copy(psi);

			return *this;
		}

		void Copy( const PlayerStateInfo& psi )
		{
			m_Flag             = psi.m_Flag;
			m_PID              = psi.m_PID;
			m_RoomID           = psi.m_RoomID;
			m_TableID          = psi.m_TableID;
			m_SitID            = psi.m_SitID;
			m_PlayerState      = psi.m_PlayerState;
			m_strState         = psi.m_strState;
		}
	};

	struct PlayerStateInfoList
	{
		enum { XY_ID = GameLobbyXYID_PlayerStateInfoList };
		enum { MAX_COUNT = 100 };

		mutable unsigned short        m_nCount;
		vector<PlayerStateInfo>       m_listPSI;

		PlayerStateInfoList(){ ReSet(); }
		void ReSet()
		{
			m_nCount = 0;
			m_listPSI.clear();
		}

		friend bostream& operator<<(bostream& bos,const PlayerStateInfoList& src)
		{
			src.m_nCount = short(min(src.m_listPSI.size(),MAX_COUNT));
		
			bos << src.m_nCount;
			for (int i=0;i<src.m_nCount;++i)			
			{
				bos << src.m_listPSI[i];
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerStateInfoList& src)
		{
			src.ReSet();

			bis >> src.m_nCount;

			if ( src.m_nCount > MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}

			PlayerStateInfo msgPSI;
			for(int i=0;i<src.m_nCount;i++)
			{
				msgPSI.ReSet();
				bis >> msgPSI;
				src.m_listPSI.push_back(msgPSI);
			}

			return bis;
		}
	};

	struct PlayerInfo
	{
		enum { XY_ID = MoGui_PLAYERINFO };

		PlayerBaseInfo           m_BaseInfo;
		PlayerGameInfo           m_GameInfo;
		PlayerStateInfo          m_StateInfo;

		PlayerInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerInfo& pi)
		{
			bos << pi.m_BaseInfo;
			bos << pi.m_GameInfo;
			bos << pi.m_StateInfo;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerInfo& pi)
		{
			pi.ReSet();

			bis >> pi.m_BaseInfo;
			bis >> pi.m_GameInfo;
			bis >> pi.m_StateInfo;

			return bis;
		}
		PlayerInfo( const PlayerInfo& pi)
		{
			Copy(pi);
		}
		PlayerInfo& operator=( const PlayerInfo& pi)
		{
			if(this == &pi)
			{
				return *this;
			}

			Copy(pi);

			return *this;
		}
		void Copy( const PlayerInfo& pi )
		{
			m_BaseInfo.Copy(pi.m_BaseInfo);
			m_GameInfo.Copy(pi.m_GameInfo);
			m_StateInfo.Copy(pi.m_StateInfo);
		}
	};

	struct PlayerGameMoney
	{
		enum { XY_ID = GameLobbyXYID_PlayerGameMoney };

		short                    m_AID;
		unsigned int             m_PID;
		SBigNumber               m_nGameMoney;

		PlayerGameMoney() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerGameMoney& pgm)
		{
			bos << pgm.m_AID;
			bos << pgm.m_PID;
			bos << pgm.m_nGameMoney;			

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerGameMoney& pgm)
		{
			pgm.ReSet();

			bis >> pgm.m_AID;
			bis >> pgm.m_PID;
			bis >> pgm.m_nGameMoney;

			return bis;
		}
	};

	struct PlayerMoGuiMoney
	{
		enum { XY_ID = GameLobbyXYID_PlayerMoGuiMoney };

		short                    m_AID;
		unsigned int             m_PID;
		SBigNumber               m_nMoGuiMoney;

		PlayerMoGuiMoney() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerMoGuiMoney& src)
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nMoGuiMoney;			

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerMoGuiMoney& src)
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nMoGuiMoney;

			return bis;
		}
	};

	struct PlayerHonorAward
	{
		enum { XY_ID = GameLobbyXYID_PlayerHonorAward };

		unsigned int             m_PID;
		INT16                    m_HonorID;
		SBigNumber               m_nAwardMoney;

		PlayerHonorAward() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerHonorAward& src)
		{
			bos << src.m_PID;
			bos << src.m_HonorID;
			bos << src.m_nAwardMoney;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerHonorAward& src)
		{
			src.ReSet();
			bis >> src.m_PID;
			bis >> src.m_HonorID;
			bis >> src.m_nAwardMoney;

			return bis;
		}
	};

	struct JuBaoPengMoney
	{
		enum { XY_ID = GameLobbyXYID_JuBaoPengMoney };

		INT64            m_JuBaoPengMoney;

		JuBaoPengMoney() { ReSet(); }
		void ReSet() { m_JuBaoPengMoney = 0;}

		friend bostream& operator<<(bostream& bos,const JuBaoPengMoney& src)
		{
			bos << src.m_JuBaoPengMoney;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,JuBaoPengMoney& src)
		{
			src.ReSet();
			bis >> src.m_JuBaoPengMoney;
			return bis;
		}
	};

	struct GameLevelAward
	{
		enum { XY_ID = GameLobbyXYID_GameLevelAward };

		BYTE             m_curLevel;
		INT32            m_curAward;
		SBigNumber       m_GameMoney;
		INT32            m_nextAdd;
		INT32            m_nextAward;

		GameLevelAward() { ReSet(); }
		void ReSet()
		{
			m_curLevel = 0;
			m_curAward = 0;
			m_GameMoney = 0;
			m_nextAdd = 0;
			m_nextAward = 0;
		}
		friend bostream& operator<<(bostream& bos,const GameLevelAward& src)
		{
			bos << src.m_curLevel;
			bos << src.m_curAward;
			bos << src.m_GameMoney;
			bos << src.m_nextAdd;
			bos << src.m_nextAward;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,GameLevelAward& src)
		{
			src.ReSet();
			bis >> src.m_curLevel;
			bis >> src.m_curAward;
			bis >> src.m_GameMoney;
			bis >> src.m_nextAdd;
			bis >> src.m_nextAward;
			return bis;
		}
	};

	struct ReqTailInfo
	{
		enum { XY_ID = GameLobbyXYID_ReqTailInfo };
		enum { TailInfo=1,GiftInfo=2,HonorInfo=4,};

		BYTE            m_Flag;
		UINT32          m_nValue;

		ReqTailInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqTailInfo& PTI)
		{		
			bos << PTI.m_Flag;
			bos << PTI.m_nValue;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqTailInfo& PTI)
		{
			PTI.ReSet();

			bis >> PTI.m_Flag;
			bis >> PTI.m_nValue;

			return bis;
		}
	};

	struct PlayerTailInfo
	{
		enum { XY_ID = GameLobbyXYID_PlayerTailInfo };

		UINT32          m_PID;

		UINT32          m_JoinTime;
		UINT32          m_LandTime;

		SBigNumber      m_maxOwnMoney;
		SBigNumber      m_maxWinMoney;

		short           m_FriendCount;
		int             m_nWinTimes;
		int             m_nLossTimes;
		INT64           m_MaxPai;

		BYTE            m_HonorCount;

		PlayerTailInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerTailInfo& src)
		{		
			bos << src.m_PID;

			bos << src.m_JoinTime;
			bos << src.m_LandTime;
			bos << src.m_maxOwnMoney;
			bos << src.m_maxWinMoney;
			bos << src.m_FriendCount;
			bos << src.m_nWinTimes;
			bos << src.m_nLossTimes;
			bos << src.m_MaxPai;
			bos << src.m_HonorCount;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerTailInfo& src)
		{
			src.ReSet();

			bis >> src.m_PID;

			bis >> src.m_JoinTime;
			bis >> src.m_LandTime;
			bis >> src.m_maxOwnMoney;
			bis >> src.m_maxWinMoney;
			bis >> src.m_FriendCount;
			bis >> src.m_nWinTimes;
			bis >> src.m_nLossTimes;
			bis >> src.m_MaxPai;
			bis >> src.m_HonorCount;

			return bis;
		}
	};

	struct ReqPlayerGiftInfo
	{
		enum { XY_ID = GameLobbyXYID_ReqPlayerGiftInfo };

		unsigned int	m_PID;

		ReqPlayerGiftInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqPlayerGiftInfo& PGI)
		{		
			bos << PGI.m_PID;			

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqPlayerGiftInfo& PGI)
		{
			PGI.ReSet();

			bis >> PGI.m_PID;

			return bis;
		}
	};

	struct msgPlayerGiftInfo
	{
		INT32        m_GiftIdx;
		INT16        m_GiftID;
		INT32        m_Price;
		UINT32       m_ActionTime;
		string       m_NickName;

		msgPlayerGiftInfo(){ ReSet(); }
		void ReSet()
		{
			m_GiftIdx = 0;
			m_GiftID = 0;
			m_Price = 0;
			m_ActionTime = 0;
			m_NickName = "";
		}

		friend bostream& operator<<(bostream& bos,const msgPlayerGiftInfo& pgi)
		{
			bos << pgi.m_GiftIdx;
			bos << pgi.m_GiftID;
			bos << pgi.m_Price;
			bos << pgi.m_ActionTime;
			InString(bos,pgi.m_NickName,50);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,msgPlayerGiftInfo& pgi)
		{
			pgi.ReSet();

			bis >> pgi.m_GiftIdx;
			bis >> pgi.m_GiftID;
			bis >> pgi.m_Price;
			bis >> pgi.m_ActionTime;
			OutString(bis,pgi.m_NickName,50);

			return bis;
		}
		msgPlayerGiftInfo( const msgPlayerGiftInfo& pi)
		{
			Copy(pi);
		}
		msgPlayerGiftInfo& operator=( const msgPlayerGiftInfo& pi)
		{
			if(this == &pi)
			{
				return *this;
			}

			Copy(pi);

			return *this;
		}
		void Copy( const msgPlayerGiftInfo& pi )
		{
			m_GiftIdx     = pi.m_GiftIdx;
			m_GiftID      = pi.m_GiftID;
			m_Price       = pi.m_Price;
			m_ActionTime  = pi.m_ActionTime;
			m_NickName    = pi.m_NickName;
		}
	};

	struct PlayerGiftInfo
	{
		enum { XY_ID = GameLobbyXYID_PlayerGiftInfo };

		unsigned int	   m_PID;
		msgPlayerGiftInfo  m_GiftInfo;

		PlayerGiftInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerGiftInfo& PGI)
		{		
			bos << PGI.m_PID;
			bos << PGI.m_GiftInfo;			

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerGiftInfo& PGI)
		{
			PGI.ReSet();

			bis >> PGI.m_PID;
			bis >> PGI.m_GiftInfo;

			return bis;
		}
	};	

	struct PlayerGiftInfoList
	{
		enum { XY_ID = GameLobbyXYID_PlayerGiftInfoList };
		enum { MAX_COUNT = 50 };
		enum { CUR=1,LAST };

		unsigned int                  m_PID;
		unsigned char                 m_Flag;
		mutable unsigned short        m_nCount;
		vector<msgPlayerGiftInfo>     m_listPGI;

		PlayerGiftInfoList(){ReSet();}
		void ReSet() 
		{ 
			m_PID = 0;
			m_Flag = 0;
			m_nCount = 0;
			m_listPGI.clear();
		}

		friend bostream& operator<<(bostream& bos,const PlayerGiftInfoList& pgil)
		{
			pgil.m_nCount = short(min(pgil.m_listPGI.size(),MAX_COUNT));

			bos << pgil.m_PID;
			bos << pgil.m_Flag;
			bos << pgil.m_nCount;

			for (int i=0;i<pgil.m_nCount;++i)
			{
				bos << pgil.m_listPGI[i];
			}
			
			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerGiftInfoList& pgil)
		{
			pgil.ReSet();

			bis >> pgil.m_PID;
			bis >> pgil.m_Flag;
			bis >> pgil.m_nCount;

			if ( pgil.m_nCount > MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}

			msgPlayerGiftInfo msgPGI;
			for(int i=0;i<pgil.m_nCount;i++)
			{
				msgPGI.ReSet();
				bis >> msgPGI;
				pgil.m_listPGI.push_back(msgPGI);
			}

			return bis;
		}
	};

	struct PlayerHonorInfoList
	{
		enum { XY_ID = GameLobbyXYID_PlayerHonorInfoList };
		enum { MAX_COUNT = 200 };

		unsigned int                  m_PID;
		vector<BYTE>                  m_listHonorID;

		PlayerHonorInfoList(){ ReSet();}
		void ReSet()
		{
			m_PID = 0;
			m_listHonorID.clear();
		}

		friend bostream& operator<<(bostream& bos,const PlayerHonorInfoList& src)
		{
			bos << src.m_PID;
			InVector(bos,src.m_listHonorID,src.MAX_COUNT);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerHonorInfoList& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			OutVector(bis,src.m_listHonorID,src.MAX_COUNT);

			return bis;
		}
	};

	struct PlayerReqSendGift
	{
		enum { XY_ID = GameLobbyXYID_ReqSendGift };
		enum { MAX_COUNT = 250 };

		unsigned short          m_GiftID;
		unsigned int            m_SendPID;
		vector<unsigned int>    m_vecRecvPID;

		PlayerReqSendGift(){ ReSet(); }
		void ReSet()
		{
			m_GiftID = 0;
			m_SendPID = 0;
			m_vecRecvPID.clear();
		}

		friend bostream& operator<<(bostream& bos,const PlayerReqSendGift& PSG)
		{
			bos << PSG.m_GiftID;
			bos << PSG.m_SendPID;
			InVector(bos,PSG.m_vecRecvPID,PSG.MAX_COUNT);
			
			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerReqSendGift& PSG)
		{
			PSG.ReSet();

			bis >> PSG.m_GiftID;
			bis >> PSG.m_SendPID;
			OutVector(bis,PSG.m_vecRecvPID,PSG.MAX_COUNT);

			return bis;
		}
	};

	struct PlayerRespSendGift
	{
		enum { XY_ID = GameLobbyXYID_RespSendGift };
		enum
		{
			SUCCESS = 0,
			NOMONEY = 1,
			NOPLAYER = 2,
			NOPRICE = 3,
			Failed,
		};

		unsigned int            m_SendPID;
		unsigned char           m_Flag;	

		PlayerRespSendGift(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); m_Flag=Failed;}

		friend bostream& operator<<(bostream& bos,const PlayerRespSendGift& PSG)
		{
			bos << PSG.m_SendPID;
			bos << PSG.m_Flag;
			
			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerRespSendGift& PSG)
		{
			PSG.ReSet();

			bis >> PSG.m_SendPID;
			bis >> PSG.m_Flag;

			return bis;
		}
	};

	struct PlayerRecvGift
	{
		enum { XY_ID = GameLobbyXYID_RecvGift };

		unsigned int	   m_PID;
		msgPlayerGiftInfo  m_GiftInfo;

		PlayerRecvGift(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const PlayerRecvGift& PGI)
		{		
			bos << PGI.m_PID;
			bos << PGI.m_GiftInfo;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerRecvGift& PGI)
		{
			PGI.ReSet();

			bis >> PGI.m_PID;
			bis >> PGI.m_GiftInfo;			

			return bis;
		}
	};	

	struct PlayerReqSoldGift
	{
		enum { XY_ID = GameLobbyXYID_ReqSoldGift };
		enum { MAX_COUNT = 250 };

		unsigned int       m_PID;
		vector<int>        m_vecGiftIdx;		

		PlayerReqSoldGift(){ ReSet(); }
		void ReSet()
		{ 
			m_PID = 0;
			m_vecGiftIdx.clear();
		}
		friend bostream& operator<<(bostream& bos,const PlayerReqSoldGift& PSG)
		{
			bos << PSG.m_PID;
			InVector(bos,PSG.m_vecGiftIdx,PSG.MAX_COUNT);
			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerReqSoldGift& PSG)
		{
			PSG.ReSet();
			bis >> PSG.m_PID;
			OutVector(bis,PSG.m_vecGiftIdx,PSG.MAX_COUNT);
			return bis;
		}
	};	

	struct PlayerRespSoldGift
	{
		enum { XY_ID = GameLobbyXYID_RespSoldGift };
		enum { MAX_COUNT = 250 };

		unsigned int       m_PID;
		SBigNumber         m_nMoney;
		vector<int>        m_vecGiftIdx;

		PlayerRespSoldGift(){ ReSet(); }
		void ReSet()
		{ 
			m_PID = 0;
			m_nMoney = 0;
			m_vecGiftIdx.clear();
		}

		friend bostream& operator<<(bostream& bos,const PlayerRespSoldGift& PSG)
		{
			bos << PSG.m_PID;
			bos << PSG.m_nMoney;
			InVector(bos,PSG.m_vecGiftIdx,PSG.MAX_COUNT);
			return bos;
		}
		friend bistream& operator>>(bistream& bis,PlayerRespSoldGift& PSG)
		{
			PSG.ReSet();
			bis >> PSG.m_PID;
			bis >> PSG.m_nMoney;
			OutVector(bis,PSG.m_vecGiftIdx,PSG.MAX_COUNT);
			return bis;
		}
	};

	struct ReqChangeGift
	{
		enum { XY_ID = GameLobbyXYID_ReqChangGift };

		unsigned int	   m_PID;
		int                m_GiftIdx;

		ReqChangeGift(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqChangeGift& src)
		{
			bos << src.m_PID;
			bos << src.m_GiftIdx;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqChangeGift& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_GiftIdx;	

			return bis;
		}
	};
	struct RespChangeGift
	{
		enum { XY_ID = GameLobbyXYID_RespChangGift };
		enum
		{
			SUCCESS,
			UNSUCCESS,
			NOGIFT,
			OUTTIME
		};

		unsigned char      m_Flag;
		unsigned int	   m_PID;
		short              m_GiftID;

		RespChangeGift(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespChangeGift& src)
		{
			bos << src.m_Flag;
			if ( src.m_Flag == SUCCESS )
			{
				bos << src.m_PID;
				bos << src.m_GiftID;
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespChangeGift& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			if (src.m_Flag == SUCCESS)
			{
				bis >> src.m_PID;
				bis >> src.m_GiftID;
			}			

			return bis;
		}
	};

	struct ReqChangeBank
	{
		enum { XY_ID = GameLobbyXYID_ReqChangeBank };
		enum { BankToGame=0,GameToBank };

		unsigned int	   m_PID;
		unsigned char      m_nFlag;
		SBigNumber         m_nMoney;

		ReqChangeBank(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqChangeBank& src)
		{
			bos << src.m_PID;
			bos << src.m_nFlag;
			bos << src.m_nMoney;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqChangeBank& src)
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_nFlag;
			bis >> src.m_nMoney;

			return bis;
		}
	};
	struct RespChangeBank
	{
		enum { XY_ID = GameLobbyXYID_RespChangeBank };
		enum
		{
			SUCCESS,
			UNSUCCESS,
			OutOfMoney,
			NotOpenBank,
			CantOperater,
		};

		unsigned char      m_Flag;
		SBigNumber         m_nChangeMoney;
		SBigNumber         m_nBankMoney;
		SBigNumber         m_nGameMoney;

		RespChangeBank(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespChangeBank& src)
		{
			bos << src.m_Flag;
			if ( src.m_Flag == SUCCESS )
			{
				bos << src.m_nChangeMoney;
				bos << src.m_nBankMoney;
				bos << src.m_nGameMoney;
			}
			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespChangeBank& src)
		{
			src.ReSet();
			bis >> src.m_Flag;
			if (src.m_Flag == SUCCESS)
			{
				bis >> src.m_nChangeMoney;
				bis >> src.m_nBankMoney;
				bis >> src.m_nGameMoney;
			}
			return bis;
		}
	};

	struct msgUserAward
	{
		short           m_MoneyFlag;
		SBigNumber      m_nMoney;

		msgUserAward() { ReSet(); }
		void ReSet()
		{
			m_MoneyFlag = 0;
			m_nMoney = 0;
		}

		friend bostream& operator<<( bostream& bos, const msgUserAward& src )
		{
			bos << src.m_MoneyFlag;
			bos << src.m_nMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, msgUserAward& src )
		{
			bis >> src.m_MoneyFlag;
			bis >> src.m_nMoney;

			return bis;
		}
	};

	struct UserAwardInfoList
	{
		enum { XY_ID = GameLobbyXYID_UserAwardList };
		enum { MAX_COUNT = 100 };

		unsigned int                  m_PID;
		SBigNumber                    m_nMoney;
		mutable unsigned short        m_nCount;
		vector<msgUserAward>          m_listAwardInfo;

		UserAwardInfoList() { ReSet(); }
		void ReSet()
		{
			m_PID = 0;
			m_nCount = 0;
			m_listAwardInfo.clear();
		}

		friend bostream& operator<<( bostream& bos, const UserAwardInfoList& src )
		{
			src.m_nCount = short(min(src.m_listAwardInfo.size(),MAX_COUNT));

			bos << src.m_PID;
			bos << src.m_nMoney;
			bos << src.m_nCount;
			for (int i=0;i<src.m_nCount;++i )
			{
				bos << src.m_listAwardInfo[i];
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, UserAwardInfoList& src )
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_nMoney;
			bis >> src.m_nCount;

			if ( src.m_nCount > MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}

			msgUserAward msgUA;
			for (int i=0;i<src.m_nCount;i++)
			{
				bis >> msgUA;
				src.m_listAwardInfo.push_back(msgUA);
			}

			return bis;
		}
	};

	struct UserHuiYuan
	{
		enum { XY_ID = GameLobbyXYID_UserHuiYuan };

		unsigned int                  m_PID;

		SBigNumber                    m_nAddMoney;
		SBigNumber                    m_nGameMoney;
		SBigNumber                    m_nDayGivenMoney;
		BYTE                          m_OpenBank;
		UINT32                        m_FreeFaceTime;
		BYTE                          m_VipLevel;
		UINT32                        m_VipEndTime;
		UINT32                        m_ActionTime;

		UserHuiYuan() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const UserHuiYuan& src )
		{
			bos << src.m_PID;
			bos << src.m_nAddMoney;
			bos << src.m_nGameMoney;
			bos << src.m_nDayGivenMoney;
			bos << src.m_OpenBank;
			bos << src.m_FreeFaceTime;
			bos << src.m_VipLevel;
			bos << src.m_VipEndTime;
			bos << src.m_ActionTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, UserHuiYuan& src )
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_nAddMoney;
			bis >> src.m_nGameMoney;
			bis >> src.m_nDayGivenMoney;
			bis >> src.m_OpenBank;
			bis >> src.m_FreeFaceTime;
			bis >> src.m_VipLevel;
			bis >> src.m_VipEndTime;
			bis >> src.m_ActionTime;

			return bis;
		}
	};

	struct UserMoguiExchange
	{
		enum { XY_ID = GameLobbyXYID_UserMoguiExchange };

		unsigned int                  m_PID;
		SBigNumber                    m_nAddMoguiMoney;
		SBigNumber                    m_nMoguiMoney;

		UserMoguiExchange() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const UserMoguiExchange& src )
		{
			bos << src.m_PID;
			bos << src.m_nAddMoguiMoney;
			bos << src.m_nMoguiMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, UserMoguiExchange& src )
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_nAddMoguiMoney;
			bis >> src.m_nMoguiMoney;
			return bis;
		}
	};

	struct PlayerMaxPai
	{
		enum { XY_ID = GameLobbyXYID_MaxPai };

		long long             m_MaxPai;
		unsigned int          m_MaxPaiTime;

		PlayerMaxPai() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const PlayerMaxPai& src )
		{
			bos << src.m_MaxPai;
			bos << src.m_MaxPaiTime;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, PlayerMaxPai& src )
		{
			src.ReSet();
			bis >> src.m_MaxPai;
			bis >> src.m_MaxPaiTime;
			return bis;
		}
	};

	struct PlayerMaxWin
	{
		enum { XY_ID = GameLobbyXYID_MaxWin };

		SBigNumber            m_MaxWin;
		unsigned int          m_MaxWinTime;

		PlayerMaxWin() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const PlayerMaxWin& src )
		{
			bos << src.m_MaxWin;
			bos << src.m_MaxWinTime;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, PlayerMaxWin& src )
		{
			src.ReSet();
			bis >> src.m_MaxWin;
			bis >> src.m_MaxWinTime;
			return bis;
		}
	};

	struct PlayerMaxMoney
	{
		enum { XY_ID = GameLobbyXYID_MaxMoney };

		SBigNumber            m_MaxMoney;
		unsigned int          m_MaxMoneyTime;

		PlayerMaxMoney() { ReSet(); }
		void ReSet(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const PlayerMaxMoney& src )
		{
			bos << src.m_MaxMoney;
			bos << src.m_MaxMoneyTime;
			return bos;
		}

		friend bistream& operator>>( bistream& bis, PlayerMaxMoney& src )
		{
			src.ReSet();
			bis >> src.m_MaxMoney;
			bis >> src.m_MaxMoneyTime;
			return bis;
		}
	};

	struct ReqCreateTable
	{
		enum { XY_ID = GameLobbyXYID_ReqCreateTable };
		enum { GameMoney,MoGuiMoney,Ticket };
		enum { MaxPlayerOnTable = 9};

		unsigned int	   m_FoundPID;
		unsigned char      m_PayFlag;

		unsigned char      m_MaxPlayer;            //最大玩家数量
		int                m_nBigBlind;            //实际盲注
		short              m_nMinTake;             //盲注的倍数
		short              m_nMaxTake;             //盲注的倍数
		short              m_Pot;                  //盲注的倍数/100
		unsigned char      m_Limite;               //桌子的限注规则

		unsigned char      m_ChipTime;
		string             m_Password;

		int                m_nLastTime;

		ReqCreateTable(){ ReSet(); }
		void ReSet()
		{
			m_FoundPID = 0;
			m_PayFlag = 0;

			m_MaxPlayer = MaxPlayerOnTable;
			m_nBigBlind = 0;
			m_nMaxTake = 0;
			m_nMinTake = 0;
			m_Pot = 0;

			m_Limite = 0;
			
			m_ChipTime = 20;			
			m_Password = "";			

			m_nLastTime = 0;
		}

		friend bostream& operator<<(bostream& bos,const ReqCreateTable& src)
		{
			bos << src.m_FoundPID;
			bos << src.m_PayFlag;

			bos << src.m_MaxPlayer;
			bos << src.m_nBigBlind;
			bos << src.m_nMinTake;
			bos << src.m_nMaxTake;
			bos << src.m_Pot;

			bos <<src.m_Limite;
		
			bos << src.m_ChipTime;
			InString(bos,src.m_Password,TABLE_PASSWORD_SIZE);

			bos << src.m_nLastTime;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqCreateTable& src)
		{
			src.ReSet();

			bis >> src.m_FoundPID;
			bis >> src.m_PayFlag;

			bis >> src.m_MaxPlayer;
			bis >> src.m_nBigBlind;
			bis >> src.m_nMinTake;
			bis >> src.m_nMaxTake;
			bis >> src.m_Pot;

			bis >> src.m_Limite;
			
			bis >> src.m_ChipTime;
			OutString(bis,src.m_Password,TABLE_PASSWORD_SIZE);

			bis >> src.m_nLastTime;

			return bis;
		}
	};

	struct RespCreateTable
	{
		enum { XY_ID = GameLobbyXYID_RespCreateTable };
		enum
		{
			SUCCESS,
			UNSUCCESS,
			NOMONEY,                  //余额不足
			OVERCREATE,               //重复创建房间
			NOTICKS,                  //没有房间票
			TIMEERROR,                //
			NOROOM,
			ROOMFULL,
			LIMITEERROR,              //规则 有限出错
			BLINDERROR,               //盲注出错
			PWOUTLENGTH,              //太长
			PWNODIGITE,               //非数字.	
			SERVERBUSY,
			VISITOR,                  //游戏客不能创建房间
		};

		unsigned char      m_Flag;
		unsigned int	   m_FoundPID;
		SBigNumber         m_PayMoney;
		SBigNumber         m_GameMoney;

		RespCreateTable(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespCreateTable& src)
		{
			bos << src.m_Flag;
			if ( src.m_Flag == SUCCESS )
			{
				bos << src.m_FoundPID;
				bos << src.m_PayMoney;
				bos << src.m_GameMoney;
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespCreateTable& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			if (src.m_Flag == SUCCESS)
			{
				bis >> src.m_FoundPID;
				bis >> src.m_PayMoney;
				bis >> src.m_GameMoney;
			}

			return bis;
		}
	};

	struct CreateTablePID
	{
		enum { XY_ID = GameLobbyXYID_CreateTablePID };
		
		unsigned short     m_RoomID;
		unsigned short     m_TableID;
		unsigned int       m_PID;

		CreateTablePID(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const CreateTablePID& src)
		{		
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_PID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,CreateTablePID& src)
		{
			src.ReSet();
			
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_PID;

			return bis;
		}
	};

	struct ReqSignMatch
	{
		enum { XY_ID = GameLobbyXYID_ReqSignMatch };
		enum { SignUp,SignDown };

		unsigned char      m_Action;
		unsigned short     m_RoomID;

		ReqSignMatch(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqSignMatch& src)
		{
			bos << src.m_Action;
			bos << src.m_RoomID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqSignMatch& src)
		{
			src.ReSet();

			bis >> src.m_Action;
			bis >> src.m_RoomID;

			return bis;
		}
	};

	struct RespSignMatch
	{
		enum { XY_ID = GameLobbyXYID_RespSignMatch };
		enum
		{
			SUCCESS,
			UNSUCCESS,
			Visitor,             //游客不能参加
			NoMoney,             //游戏币不够
		};

		unsigned char      m_Flag;
		unsigned char      m_Action;
		unsigned short     m_RoomID;

		RespSignMatch(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespSignMatch& src)
		{
			bos << src.m_Flag;
			bos << src.m_Action;
			bos << src.m_RoomID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespSignMatch& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			bis >> src.m_Action;
			bis >> src.m_RoomID;

			return bis;
		}
	};

	struct ReqEnterMatch
	{
		enum { XY_ID = GameLobbyXYID_ReqEnterMatch };

		unsigned short     m_RoomID;

		ReqEnterMatch(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqEnterMatch& src)
		{
			bos << src.m_RoomID;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqEnterMatch& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;

			return bis;
		}
	};

	struct MatchTableState
	{
		enum { XY_ID = GameLobbyXYID_MatchTableState };

		unsigned short     m_RoomID;
		unsigned short     m_TableID;
		unsigned char      m_TableState;

		MatchTableState(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const MatchTableState& src)
		{
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_TableState;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,MatchTableState& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_TableState;

			return bis;
		}
	};

	struct MatchRoomState
	{
		enum { XY_ID = GameLobbyXYID_MatchRoomState };

		unsigned short     m_RoomID;		
		unsigned char      m_RoomState;

		MatchRoomState(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const MatchRoomState& src)
		{
			bos << src.m_RoomID;
			bos << src.m_RoomState;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,MatchRoomState& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;
			bis >> src.m_RoomState;

			return bis;
		}
	};

	struct AwokeMatch
	{
		enum { XY_ID = GameLobbyXYID_AwokeMatch };

		int                m_MatchID;
		unsigned short     m_RoomID;
		unsigned int       m_nTime;		

		AwokeMatch(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const AwokeMatch& src)
		{
			bos << src.m_MatchID;
			bos << src.m_RoomID;
			bos << src.m_nTime;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,AwokeMatch& src)
		{
			src.ReSet();

			bis >> src.m_MatchID;
			bis >> src.m_RoomID;
			bis >> src.m_nTime;

			return bis;
		}
	};

	struct ReqMatchRank
	{
		enum { XY_ID = GameLobbyXYID_ReqMatchRank };
		enum { MaxReq = 100 };

		unsigned short     m_RoomID;
		short              m_StartRank;
		short              m_EndRank;

		ReqMatchRank(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const ReqMatchRank& src)
		{
			bos << src.m_RoomID;
			bos << src.m_StartRank;
			bos << src.m_EndRank;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,ReqMatchRank& src)
		{
			src.ReSet();

			bis >> src.m_RoomID;
			bis >> src.m_StartRank;
			bis >> src.m_EndRank;

			return bis;
		}
	};

	struct MatchRankInfo
	{
		enum { XY_ID = GameLobbyXYID_MatchRankInfo };

		unsigned short      m_AID;
		UINT32              m_PID;
		int                 m_TableMoney;
		short               m_Position;

		MatchRankInfo(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const MatchRankInfo& src)
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_TableMoney;
			bos << src.m_Position;

			return bos;
		}
		friend bistream& operator>>(bistream& bis,MatchRankInfo& src)
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_TableMoney;
			bis >> src.m_Position;

			return bis;
		}
	};

	struct RespMatchRank
	{
		enum { XY_ID = GameLobbyXYID_RespMatchRank };
		enum { MaxCount = 100, };
		enum { LobbyRoom,TableGame,};

		unsigned char              m_Flag;
		unsigned short             m_RoomID;
		vector<MatchRankInfo>      m_vectorMatchRank;

		RespMatchRank(){ ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<(bostream& bos,const RespMatchRank& src)
		{
			bos << src.m_Flag;
			bos << src.m_RoomID;
			InVector(bos,src.m_vectorMatchRank,src.MaxCount);

			return bos;
		}
		friend bistream& operator>>(bistream& bis,RespMatchRank& src)
		{
			src.ReSet();

			bis >> src.m_Flag;
			bis >> src.m_RoomID;
			OutVector(bis,src.m_vectorMatchRank,src.MaxCount);

			return bis;
		}
	};

	struct Game_Test
	{
		enum { XY_ID = GameLobbyXYID_Test };

		SBigNumber                    m_Flag;

		Game_Test(){ ReSet(); }
		void ReSet()
		{
			m_Flag = 0;
		}
		friend bostream& operator<<(bostream& bos,const Game_Test& src)
		{
			bos << src.m_Flag;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,Game_Test& src)
		{
			src.ReSet();
			bis >> src.m_Flag;
			return bis;
		}
	};

	struct ServerToClientMessage
	{
		enum { XY_ID = MoGui_GAME_SERVERTOCLIENT };

		unsigned short         m_MsgID;
		short                  m_MsgLen;
		char                   m_Message[MAX_MSGDTDA_LEN];

		ServerToClientMessage() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
		friend bostream& operator<<(bostream& bos, const ServerToClientMessage& STC )
		{
			bos << STC.m_MsgID;
			bos << STC.m_MsgLen;

			if (STC.m_MsgLen >0 && STC.m_MsgLen<=MAX_MSGDTDA_LEN)
			{
				bos.write(STC.m_Message,STC.m_MsgLen);
			}
			return bos;
		}

		friend bistream& operator>>(bistream& bis, ServerToClientMessage& STC )
		{
			STC.ReSet();

			bis >> STC.m_MsgID;
			bis >> STC.m_MsgLen;

			if( STC.m_MsgLen>MAX_MSGDTDA_LEN || STC.m_MsgLen<0 )
				throw agproexception(agproexception::rangeerror);

			bis.read(STC.m_Message, STC.m_MsgLen);

			return bis;
		}
	};
	struct ClientToServerMessage
	{
		enum { XY_ID = MoGui_GAME_CLIENTTOSERVER };

		unsigned short         m_MsgID;
		short                  m_MsgLen;
		char                   m_Message[MAX_MSGDTDA_LEN];

		ClientToServerMessage() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
		friend bostream& operator<<(bostream& bos, const ClientToServerMessage& CTS )
		{
			bos << CTS.m_MsgID;
			bos << CTS.m_MsgLen;

			if (CTS.m_MsgLen >0 && CTS.m_MsgLen<=MAX_MSGDTDA_LEN)
			{
				bos.write(CTS.m_Message,CTS.m_MsgLen);
			}
			return bos;
		}

		friend bistream& operator>>(bistream& bis, ClientToServerMessage& CTS)
		{
			CTS.ReSet();

			bis >> CTS.m_MsgID;
			bis >> CTS.m_MsgLen;

			if( CTS.m_MsgLen>MAX_MSGDTDA_LEN || CTS.m_MsgLen<0 )
				throw agproexception(agproexception::rangeerror);

			bis.read(CTS.m_Message, CTS.m_MsgLen);

			return bis;
		}
	};
}
}
}