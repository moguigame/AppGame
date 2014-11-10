#pragma once

#include "MoGuiGame.h"
#include "gamedef.h"

namespace MoGui
{
namespace MoGuiXY
{
namespace DBServerXY
{
	using namespace MoGui::MoGuiXY::PublicXYData;
	using namespace MoGui::Game::DeZhou;

	const short  MoGui_SERVER_CONNECT                              = MOGUI_FIRST_SERVER+1;            //连接DB服务器的认证
	const short  MoGui_SERVER_CONNECTDATA                          = MOGUI_FIRST_SERVER+2;            //认证返回

	const short  MoGui_SERVER_PLAYERLOGIN                          = MOGUI_FIRST_SERVER+11;
	const short  MoGui_SERVER_PLAYERLOGINDATA                      = MOGUI_FIRST_SERVER+12;
	const short  MoGui_SERVER_PLAYERQUITE                          = MOGUI_FIRST_SERVER+13;           //玩家下线
	const short  MoGui_SERVER_PlayerDataNotExist                   = MOGUI_FIRST_SERVER+14;
	const short  MoGui_SERVER_PlayerDataClear                      = MOGUI_FIRST_SERVER+15;

	const short  MoGui_Server_ReqPlayerOnLine                      = MOGUI_FIRST_SERVER+16;
	const short  MoGui_Server_RespPlayerOnLine                     = MOGUI_FIRST_SERVER+17;

	const short  DBServerXYID_ReqPlayerInfo                        = MOGUI_FIRST_SERVER+18;
	const short  DBServerXYID_RespPlayerInfo                       = MOGUI_FIRST_SERVER+19;
	const short  DBServerXYID_PlayerData                           = MOGUI_FIRST_SERVER+20;
	const short  DBServerXYID_PlayerDataEx                         = MOGUI_FIRST_SERVER+21;
	const short  DBServerXYID_PlayerMatchData                      = MOGUI_FIRST_SERVER+22;
	const short  DBServerXYID_PlayerLimite                         = MOGUI_FIRST_SERVER+23;
	const short  DBServerXYID_PlayerRight                          = MOGUI_FIRST_SERVER+24;
	const short  DBServerXYID_ReqPlayerAward                       = MOGUI_FIRST_SERVER+25;
	const short  DBServerXYID_ReqChangeBankMoney                   = MOGUI_FIRST_SERVER+26;
	const short  DBServerXYID_RespChangeBankMoney                  = MOGUI_FIRST_SERVER+27;

	const short  MoGui_SERVER_REQROOMTABLEINFO                     = MOGUI_FIRST_SERVER+31;
	const short  MoGui_SERVER_RESPROOMTABLEINFO                    = MOGUI_FIRST_SERVER+32;
	const short  MoGui_SERVER_ROOMINFO                             = MOGUI_FIRST_SERVER+35;
	const short  MoGui_SERVER_TABLEINFO                            = MOGUI_FIRST_SERVER+36;

	const short  MoGui_SERVER_REQBOTPLAYER                         = MOGUI_FIRST_SERVER+51;
	const short  MoGui_SERVER_RESPBOTPLAYER                        = MOGUI_FIRST_SERVER+52;
	const short  MoGui_SERVER_BOTPLAYERDATA                        = MOGUI_FIRST_SERVER+53;

	const short  DBServerXYID_ReqAddBotMoney                       = MOGUI_FIRST_SERVER+56;
	const short  DBServerXYID_RespAddBotMoney                      = MOGUI_FIRST_SERVER+57;
	
	const short  MoGui_Server_FaceInfoList                         = MOGUI_FIRST_SERVER+62;
	const short  MoGui_Server_GiftInfoList                         = MOGUI_FIRST_SERVER+67;
	const short  DBServerXYID_MatchInfoList                        = MOGUI_FIRST_SERVER+68;
	const short  DBServerXYID_MatchInfo                            = MOGUI_FIRST_SERVER+69;
	

	const short  DBServerXYID_ReqUserGiftInfo                      = MOGUI_FIRST_SERVER+71;
	const short  DBServerXYID_UserGiftInfo                         = MOGUI_FIRST_SERVER+72;
	const short  DBServerXYID_UserGiftInfoList                     = MOGUI_FIRST_SERVER+73;

	const short  DBServerXYID_UserAwardInfoList                    = MOGUI_FIRST_SERVER+81;

	const short  DBServerXYID_ChangeUserInfo                       = MOGUI_FIRST_SERVER+91;
	const short  DBServerXYID_ReqPlayerGameMoney                   = MOGUI_FIRST_SERVER+92;
	const short  DBServerXYID_RespPlayerGameMoney                  = MOGUI_FIRST_SERVER+93;

	const short  DBServerXYID_DBS_GS_Flag                          = MOGUI_FIRST_SERVER+98;
	const short  DBServerXYID_GS_DBS_Flag                          = MOGUI_FIRST_SERVER+99;
	
	const short  MoGui_SERVER_USERFRIEND                           = MOGUI_FIRST_SERVER+101;

	const short  DBServerXYID_UserHuiYuan                          = MOGUI_FIRST_SERVER+111;
	const short  DBServerXYID_MoguiExchange                        = MOGUI_FIRST_SERVER+112;

	const short  DBServerXYID_HonorInfo                            = MOGUI_FIRST_SERVER+121;
	const short  DBServerXYID_HonorInfoList                        = MOGUI_FIRST_SERVER+122;
	const short  DBServerXYID_ReqFinishHonor                       = MOGUI_FIRST_SERVER+123;
	const short  DBServerXYID_RespFinishHonor                      = MOGUI_FIRST_SERVER+124;

	const short  DBServerXYID_GameLevelInfo                        = MOGUI_FIRST_SERVER+125;

	const short  DBServerXYID_PlayerMoneyCheck                     = MOGUI_FIRST_SERVER+131;

	const short  MoGui_SERVER_DB_FRIST                             = MOGUI_FIRST_SERVER+500;
	const short  MoGui_SERVER_WDB_USERFRIEND                       = MoGui_SERVER_DB_FRIST+1;
	const short  MoGui_SERVER_WDB_WINLOSS                          = MoGui_SERVER_DB_FRIST+2;
	const short  MoGui_SERVER_WDB_WINLOSSLIST                      = MoGui_SERVER_DB_FRIST+3;

	const short  DBServerXYID_RespWinLoss                          = MoGui_SERVER_DB_FRIST+4;
	const short  DBServerXYID_RespWinLossList                      = MoGui_SERVER_DB_FRIST+5;

	const short  DBServerXYID_WDB_MatchResult                      = MoGui_SERVER_DB_FRIST+6;
	const short  DBServerXYID_WDB_MatchWinLoss                     = MoGui_SERVER_DB_FRIST+7;

	const short  DBServerXYID_WDB_ReqCheckGameInfo                 = MoGui_SERVER_DB_FRIST+8;

	const short  MoGui_SERVER_WDB_IncomeAndPay                     = MoGui_SERVER_DB_FRIST+10;
	const short  MoGui_SERVER_WDB_SendMoney                        = MoGui_SERVER_DB_FRIST+11;
	const short  MoGui_SERVER_WDB_ShowFace                         = MoGui_SERVER_DB_FRIST+16;
	const short  DBServerXYID_WDB_SendGift                         = MoGui_SERVER_DB_FRIST+21;
	const short  DBServerXYID_WDB_SoldGift                         = MoGui_SERVER_DB_FRIST+22;
	const short  DBServerXYID_WDB_CreateTable                      = MoGui_SERVER_DB_FRIST+23;
	const short  DBServerXYID_WDB_ChatMsg                          = MoGui_SERVER_DB_FRIST+24;

	const short  DBServerXYID_WDB_MaxPai                           = MoGui_SERVER_DB_FRIST+40;
	const short  DBServerXYID_WDB_MaxWin                           = MoGui_SERVER_DB_FRIST+41;
	const short  DBServerXYID_WDB_MaxMoney                         = MoGui_SERVER_DB_FRIST+42;

	const short  DBServerXYID_WDB_PlayerAction                     = MoGui_SERVER_DB_FRIST+46;
	const short  DBServerXYID_WDB_PlayerClientError                = MoGui_SERVER_DB_FRIST+47;

	const short  DBServerXYID_WDB_PlayerAward                      = MoGui_SERVER_DB_FRIST+51;

	const short  DBServerXYID_ReportPlayerOnline                   = MoGui_SERVER_DB_FRIST+101;

	struct DBS_ReqServerConnect
	{
		enum { XY_ID = MoGui_SERVER_CONNECT };

		short				m_ServerID;
		unsigned char       m_ClientClass;
		Session             m_Session;

		DBS_ReqServerConnect() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const DBS_ReqServerConnect& rsc )
		{
			bos << rsc.m_ServerID;
			bos << rsc.m_ClientClass;
			bos << rsc.m_Session;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_ReqServerConnect& rsc )
		{
			rsc.ReSet();

			bis >> rsc.m_ServerID;
			bis >> rsc.m_ClientClass;
			bis >> rsc.m_Session;

			return bis;
		}
	};

	struct DBS_RespServerConnectData
	{
		enum { XY_ID = MoGui_SERVER_CONNECTDATA };

		enum
		{
			SUCCESS,
			KEYERROR,
			SESSIONERROR,
			SERVEREXIST,
		};

		unsigned char       m_Flag;
	
		DBS_RespServerConnectData() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_RespServerConnectData& rscd )
		{
			bos << rscd.m_Flag;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_RespServerConnectData& rscd )
		{
			rscd.ReSet();

			bis >> rscd.m_Flag;

			return bis;
		}
	};

	struct DBS_ReqServerPlayerLogin
	{
		enum { XY_ID = MoGui_SERVER_PLAYERLOGIN };
		enum { NOTEXIST,EXIST };

		unsigned int           m_PID;
		short                  m_AID;

		unsigned char          m_PlayerType;
		unsigned char          m_ClientType;
		unsigned char          m_LoginType;
		unsigned char          m_HaveData;
		std::string            m_IP;
		Session                m_Session;

		int                    m_Socket;
		std::string            m_strLoginKey;	

		DBS_ReqServerPlayerLogin() { ReSet(); }
		void ReSet() 
		{
			m_PID = 0;
			m_AID = 0;

			m_PlayerType = 0;
			m_ClientType = 0;
			m_LoginType = 0;
			m_HaveData = 0;
			m_IP = "";
			m_Session.ReSet();

			m_Socket = 0;
			m_strLoginKey = "";
		}

		friend bostream& operator<<( bostream& bos, const DBS_ReqServerPlayerLogin& srl )
		{
			bos << srl.m_PID;
			bos << srl.m_AID;
			bos << srl.m_PlayerType;
			bos << srl.m_ClientType;
			bos << srl.m_LoginType;
			bos << srl.m_HaveData;
			InString(bos,srl.m_IP,20);
			bos << srl.m_Session;

			bos << srl.m_Socket;
			InString(bos,srl.m_strLoginKey,50);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_ReqServerPlayerLogin& srl )
		{
			srl.ReSet();

			bis >> srl.m_PID;
			bis >> srl.m_AID;
			bis >> srl.m_PlayerType;
			bis >> srl.m_ClientType;
			bis >> srl.m_LoginType;
			bis >> srl.m_HaveData;
			OutString(bis,srl.m_IP,20);
			bis >> srl.m_Session;

			bis >> srl.m_Socket;
			OutString(bis,srl.m_strLoginKey,50);

			return bis;
		}
	};
	
	struct DBS_ReqPlayerInfo
	{
		enum { XY_ID = DBServerXYID_ReqPlayerInfo };
		enum
		{
			Award            = 0x0001,
			PlayerData       = 0x0002,
			PlayerDataEx     = 0x0004,
			PlayerMatchData  = 0x0008,
			Friend           = 0x0010,
			Gift             = 0x0020,
		};

		short                  m_AID;
		unsigned int           m_PID;		
		unsigned short         m_DataFlag;

		DBS_ReqPlayerInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_ReqPlayerInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;		
			bos << src.m_DataFlag;
		
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_ReqPlayerInfo& src )
		{
			src.ReSet();
			
			bis >> src.m_AID;
			bis >> src.m_PID;			
			bis >> src.m_DataFlag;

			return bis;
		}
	};

	struct DBS_RespPlayerInfo
	{
		enum { XY_ID = DBServerXYID_RespPlayerInfo };
		enum
		{
			Award            = 0x0001,
			PlayerData       = 0x0002,
			PlayerDataEx     = 0x0004,
			PlayerMatchData  = 0x0008,
			Friend           = 0x0010,
			Gift             = 0x0020,
		};

		short                  m_AID;
		unsigned int           m_PID;
		unsigned short         m_DataFlag;

		DBS_RespPlayerInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_RespPlayerInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_DataFlag;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_RespPlayerInfo& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_DataFlag;

			return bis;
		}
	};

	struct DBS_RespServerPlayerLoginData
	{
		enum { XY_ID = MoGui_SERVER_PLAYERLOGINDATA };
		enum { NOTEXIST,EXIST };
		enum
		{
			SUCCESS,
			Error_SessionError,
			Error_OutMemory,
			Error_NoSession,
			Error_Forbid,
			Error_NoPlayer,
			Error_DBError,
			Error_PlayerData,
			Error_GameInfo,
			Error_UserInfo,
			Error_InviteJoin,
			Error_ServerID,
			Error_CreateGamePlayer,
		};

		unsigned char                 m_Flag;
		unsigned short                m_AID;
		unsigned int                  m_PID;

		int                           m_Socket;
		std::string                   m_SessionKey;

		DBS_RespServerPlayerLoginData() { ReSet(); }
		void ReSet()
		{
			m_Flag = 0;
			m_AID = 0;
			m_PID = 0;
			m_Socket = 0;
			m_SessionKey = "";
		}

		friend bostream& operator<<( bostream& bos, const DBS_RespServerPlayerLoginData& srld )
		{
			bos << srld.m_Flag;
			bos << srld.m_AID;
			bos << srld.m_PID;
			bos << srld.m_Socket;
			InString(bos,srld.m_SessionKey,50);

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_RespServerPlayerLoginData& srld )
		{
			srld.ReSet();

			bis >> srld.m_Flag;
			bis >> srld.m_AID;
			bis >> srld.m_PID;
			bis >> srld.m_Socket;
			OutString(bis,srld.m_SessionKey,50);

			return bis;
		}
	};

	struct DBS_PlayerData
	{
		enum { XY_ID = DBServerXYID_PlayerData };

		short                         m_AID;
		unsigned int                  m_PID;

		unsigned char                 m_Sex;

		unsigned char                 m_PlayerType;
		int                           m_ChangeName;

		std::string                   m_NickName;
		std::string					  m_HeadPicURL;
		std::string                   m_HomePageURL;
		std::string                   m_City;

		SBigNumber                    m_nBankMoney;
		unsigned char                 m_OpenBank;
		SBigNumber                    m_nMoGuiMoney;
		SBigNumber 					  m_nGameMoney;
		SBigNumber                    m_nMatchJF;

		SBigNumber                    m_nJF;
		SBigNumber                    m_nEP;
		SBigNumber                    m_nWinTimes;
		SBigNumber                    m_nLossTimes;
		SBigNumber                    m_nGameTime;

		SBigNumber           	      m_nUpperLimite;
		SBigNumber           		  m_nLowerLimite;

		UINT8                         m_RightLevel;
		UINT8                         m_RightTimes;

		int                           m_GiftIdx;

		UINT32                        m_JoinTime;                 //加入游戏的时间
		UINT32                        m_InvitePID;                //被哪个玩家邀请进来，0为系统

		UINT8                         m_ContinuPlay;
		UINT8                         m_ContinuLogin;

		DBS_PlayerData() { ReSet(); }
		void ReSet() 
		{ 
			m_AID = 0;
			m_PID = 0;
			m_Sex = 0;

			m_PlayerType = 0;
			m_ChangeName = 0;

			m_NickName = "";
			m_HeadPicURL = "";			
			m_HomePageURL = "";
			m_City = "";

			m_nBankMoney = 0;
			m_OpenBank = 0;
			m_nMoGuiMoney = 0;
			m_nGameMoney = 0;
			m_nMatchJF = 0;			

			m_nJF = 0;
			m_nEP = 0;
			m_nWinTimes = 0;
			m_nLossTimes = 0;
			m_nGameTime = 0;

			m_nUpperLimite = 0;
			m_nLowerLimite = 0;

			m_RightLevel = 0;
			m_RightTimes = 0;

			m_GiftIdx = 0;

			m_JoinTime = 0;
			m_InvitePID = 0;

			m_ContinuPlay = 0;
			m_ContinuLogin = 0;
		}

		friend bostream& operator<<( bostream& bos, const DBS_PlayerData& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_Sex;
			bos << src.m_PlayerType;
			bos << src.m_ChangeName;

			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicURL,MAX_URL_SIZE);				
			InString(bos,src.m_HomePageURL,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			bos << src.m_nBankMoney;
			bos << src.m_OpenBank;
			bos << src.m_nMoGuiMoney;
			bos << src.m_nGameMoney;
			bos << src.m_nMatchJF;

			bos << src.m_nJF;
			bos << src.m_nEP;
			bos << src.m_nWinTimes;
			bos << src.m_nLossTimes;
			bos << src.m_nGameTime;

			bos << src.m_nUpperLimite;
			bos << src.m_nLowerLimite;

			bos << src.m_RightLevel;
			bos << src.m_RightTimes;

			bos << src.m_GiftIdx;

			bos << src.m_JoinTime;
			bos << src.m_InvitePID;

			bos << src.m_ContinuPlay;
			bos << src.m_ContinuLogin;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_PlayerData& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_Sex;
			bis >> src.m_PlayerType;
			bis >> src.m_ChangeName;

			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicURL,MAX_URL_SIZE);	
			OutString(bis,src.m_HomePageURL,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			bis >> src.m_nBankMoney;
			bis >> src.m_OpenBank;
			bis >> src.m_nMoGuiMoney;
			bis >> src.m_nGameMoney;
			bis >> src.m_nMatchJF;

			bis >> src.m_nJF;
			bis >> src.m_nEP;
			bis >> src.m_nWinTimes;
			bis >> src.m_nLossTimes;
			bis >> src.m_nGameTime;

			bis >> src.m_nUpperLimite;
			bis >> src.m_nLowerLimite;

			bis >> src.m_RightLevel;
			bis >> src.m_RightTimes;

			bis >> src.m_GiftIdx;

			bis >> src.m_JoinTime;
			bis >> src.m_InvitePID;

			bis >> src.m_ContinuPlay;
			bis >> src.m_ContinuLogin;

			return bis;
		}
	};

	struct DBS_PlayerDataEx
	{
		enum { XY_ID = DBServerXYID_PlayerDataEx };
		enum { WinTypeNum = 10 };

		short                         m_AID;
		unsigned int                  m_PID;

		INT64                         m_MaxPai;
		UINT32                        m_MaxPaiTime;
		INT64                         m_MaxMoney;
		UINT32                        m_MaxMoneyTime;
		INT64                         m_MaxWin;
		UINT32                        m_MaxWinTime;

		int                           m_WinRecord[WinTypeNum];
		int                           m_Achieve[4];

		int                           m_SendGift;
		int                           m_RecvGift;
		int                           m_TotalInvite;
		int                           m_ShowFace;

		INT16                         m_FriendCount;
		UINT32                        m_FreeFaceTime;
		unsigned char                 m_VipLevel;

		DBS_PlayerDataEx() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_PlayerDataEx& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_MaxPai;
			bos << src.m_MaxPaiTime;
			bos << src.m_MaxMoney;
			bos << src.m_MaxMoneyTime;
			bos << src.m_MaxWin;
			bos << src.m_MaxWinTime;

			for ( int i=0;i<WinTypeNum;i++)
			{
				bos << src.m_WinRecord[i];
			}
			for ( int i=0;i<4;i++)
			{
				bos << src.m_Achieve[i];
			}

			bos << src.m_SendGift;
			bos << src.m_RecvGift;
			bos << src.m_TotalInvite;
			bos << src.m_ShowFace;

			bos << src.m_FriendCount;
			bos << src.m_FreeFaceTime;
			bos << src.m_VipLevel;			

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_PlayerDataEx& src )
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

			for ( int i=0;i<WinTypeNum;i++)
			{
				bis >> src.m_WinRecord[i];
			}
			for ( int i=0;i<4;i++)
			{
				bis >> src.m_Achieve[i];
			}

			bis >> src.m_SendGift;
			bis >> src.m_RecvGift;
			bis >> src.m_TotalInvite;
			bis >> src.m_ShowFace;

			bis >> src.m_FriendCount;
			bis >> src.m_FreeFaceTime;
			bis >> src.m_VipLevel;

			return bis;
		}
	};

	struct DBS_PlayerLimite
	{
		enum { XY_ID = DBServerXYID_PlayerLimite };

		short                  m_AID;
		unsigned int           m_PID;

		SBigNumber             m_nUpperLimite;
		SBigNumber             m_nLowerLimite;

		DBS_PlayerLimite() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_PlayerLimite& pq )
		{
			bos << pq.m_AID;
			bos << pq.m_PID;
			bos << pq.m_nUpperLimite;
			bos << pq.m_nLowerLimite;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_PlayerLimite& pq )
		{
			pq.ReSet();

			bis >> pq.m_AID;
			bis >> pq.m_PID;
			bis >> pq.m_nUpperLimite;
			bis >> pq.m_nLowerLimite;

			return bis;
		}
	};

	struct DBS_ReqPlayerAward
	{
		enum {XY_ID = DBServerXYID_ReqPlayerAward };

		short           m_AID;
		unsigned int    m_PID;

		DBS_ReqPlayerAward(){ ReSet();}
		void ReSet(){ m_PID=0;m_AID=0; }

		friend bostream& operator<<(bostream& bos,const DBS_ReqPlayerAward& src)
		{
			bos << src.m_AID;
			bos << src.m_PID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis,DBS_ReqPlayerAward& src)
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			return bis;
		}
	};

	struct DBS_PlayerRight
	{
		enum { XY_ID = DBServerXYID_PlayerRight };

		short                  m_AID;
		unsigned int           m_PID;

		unsigned char          m_Level;
		unsigned char          m_Times;

		DBS_PlayerRight() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_PlayerRight& pq )
		{
			bos << pq.m_AID;
			bos << pq.m_PID;
			bos << pq.m_Level;
			bos << pq.m_Times;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_PlayerRight& pq )
		{
			pq.ReSet();

			bis >> pq.m_AID;
			bis >> pq.m_PID;
			bis >> pq.m_Level;
			bis >> pq.m_Times;

			return bis;
		}
	};

	struct DBS_PlayerQuite
	{
		enum { XY_ID = MoGui_SERVER_PLAYERQUITE };

		short                  m_AID;
		unsigned int           m_PID;
		short				   m_ServerID;

		int                    m_GiftIdx;

		DBS_PlayerQuite() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_PlayerQuite& pq )
		{
			bos << pq.m_AID;
			bos << pq.m_PID;
			bos << pq.m_ServerID;

			bos << pq.m_GiftIdx;
			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_PlayerQuite& pq )
		{
			pq.ReSet();

			bis >> pq.m_AID;
			bis >> pq.m_PID;
			bis >> pq.m_ServerID;

			bis >> pq.m_GiftIdx;
			return bis;
		}
	};

	struct DBS_PlayerDataNotExist
	{
		enum { XY_ID = MoGui_SERVER_PlayerDataNotExist };

		unsigned int           m_PID;
		short				   m_ServerID;

		DBS_PlayerDataNotExist() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_PlayerDataNotExist& pne )
		{
			bos << pne.m_PID;
			bos << pne.m_ServerID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_PlayerDataNotExist& pne )
		{
			pne.ReSet();

			bis >> pne.m_PID;
			bis >> pne.m_ServerID;

			return bis;
		}
	};

	struct DBS_PlayerDataClear
	{
		enum { XY_ID = MoGui_SERVER_PlayerDataClear };

		unsigned int           m_PID;
		short				   m_ServerID;

		DBS_PlayerDataClear() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_PlayerDataClear& pdl )
		{
			bos << pdl.m_PID;
			bos << pdl.m_ServerID;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_PlayerDataClear& pdl )
		{
			pdl.ReSet();

			bis >> pdl.m_PID;
			bis >> pdl.m_ServerID;

			return bis;
		}
	};

	struct DBS_ReqPlayerOnLine
	{
		enum { XY_ID = MoGui_Server_ReqPlayerOnLine };
		enum { MAX_COUNT = 250 , };

		short				        m_ServerID;
		std::vector<unsigned int>   m_vecPID;

		DBS_ReqPlayerOnLine() { ReSet(); }
		void ReSet()
		{
			m_ServerID = 0;
			m_vecPID.clear();
		}

		friend bostream& operator<<( bostream& bos, const DBS_ReqPlayerOnLine& src )
		{
			bos << src.m_ServerID;
			InVector(bos,src.m_vecPID,src.MAX_COUNT);

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_ReqPlayerOnLine& src )
		{
			src.ReSet();

			bis >> src.m_ServerID;
			OutVector(bis,src.m_vecPID,src.MAX_COUNT);

			return bis;
		}
	};

	struct DBS_RespPlayerOnLine
	{
		enum { XY_ID = MoGui_Server_RespPlayerOnLine };
		enum { MAX_COUNT = 250 , };

		short				           m_ServerID;
		std::vector<unsigned int>      m_vecOutPID;
		std::vector<unsigned int>      m_vecOnlinePID;

		DBS_RespPlayerOnLine() { ReSet(); }
		void ReSet() 
		{
			m_ServerID = 0;
			m_vecOutPID.clear();
			m_vecOnlinePID.clear();
		}

		friend bostream& operator<<( bostream& bos, const DBS_RespPlayerOnLine& src )
		{
			bos << src.m_ServerID;
			InVector(bos,src.m_vecOutPID,src.MAX_COUNT);
			InVector(bos,src.m_vecOnlinePID,src.MAX_COUNT);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_RespPlayerOnLine& src )
		{
			src.ReSet();

			bis >> src.m_ServerID;
			OutVector(bis,src.m_vecOutPID,src.MAX_COUNT);
			OutVector(bis,src.m_vecOnlinePID,src.MAX_COUNT);

			return bis;
		}
	};

	struct DBS_ReqAddBotMoney
	{
		enum { XY_ID = DBServerXYID_ReqAddBotMoney };

		unsigned short         m_AID;
		unsigned int           m_PID;
		BYTE                   m_nLevel;
		SBigNumber             m_nGameMoney;

		DBS_ReqAddBotMoney() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_ReqAddBotMoney& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nLevel;
			bos << src.m_nGameMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_ReqAddBotMoney& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nLevel;
			bis >> src.m_nGameMoney;

			return bis;
		}
	};

	struct DBS_RespAddBotMoney
	{
		enum { XY_ID = DBServerXYID_RespAddBotMoney };
		enum
		{
			SUCCESS,
			UNSUCCESS,
			Money_error,
		};

		unsigned char          m_Flag;
		unsigned short         m_AID;
		unsigned int           m_PID;
		SBigNumber             m_nGameMoney;

		DBS_RespAddBotMoney() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_RespAddBotMoney& src )
		{
			bos << src.m_Flag;
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nGameMoney;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_RespAddBotMoney& src )
		{
			src.ReSet();

			bis >> src.m_Flag;
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nGameMoney;

			return bis;
		}
	};

	struct DBS_ReqRoomTableInfo
	{
		enum { XY_ID = MoGui_SERVER_REQROOMTABLEINFO };

		short                m_ServerID;

		DBS_ReqRoomTableInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_ReqRoomTableInfo& ri )
		{			
			bos << ri.m_ServerID;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_ReqRoomTableInfo& ri )
		{
			ri.ReSet();

			bis >> ri.m_ServerID;

			return bis;
		}
	};

	struct DBS_RespRoomTableInfo
	{
		enum { XY_ID = MoGui_SERVER_RESPROOMTABLEINFO };
		enum 
		{
			SUCCESS,            
			SERVERIDERROR,
			NOROOMTABLEINFO,
		};

		short                m_Flag;

		DBS_RespRoomTableInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_RespRoomTableInfo& ri )
		{			
			bos << ri.m_Flag;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_RespRoomTableInfo& ri )
		{
			ri.ReSet();

			bis >> ri.m_Flag;

			return bis;
		}
	};

	struct DBS_RoomInfo
	{
		enum { XY_ID = MoGui_SERVER_ROOMINFO };

		unsigned short              m_RoomID;
		std::string                 m_RoomName;
		std::string                 m_Password;		
		std::string                 m_AreaRule;
		std::string                 m_RoomRule;
		std::string                 m_RoomRuleEX;
		std::string                 m_MatchRule;
		unsigned int                m_EndTime;

		DBS_RoomInfo() { ReSet(); }
		void ReSet()
		{
			m_RoomID = 0; 

			m_RoomName = "";
			m_Password = "";		
			m_AreaRule = "";
			m_RoomRule = "";
			m_RoomRuleEX = "";
			m_MatchRule = "";

			m_EndTime = 0; 
		}

		friend bostream& operator<<( bostream& bos, const DBS_RoomInfo& ri )
		{
			bos << ri.m_RoomID;
			InString(bos,ri.m_RoomName,MAX_ROOMNAME_SIZE);
			InString(bos,ri.m_Password,ROOM_PASSWORD_SIZE);			
			InString(bos,ri.m_AreaRule,MAX_RULE_SIZE);
			InString(bos,ri.m_RoomRule,MAX_RULE_SIZE);
			InString(bos,ri.m_RoomRuleEX,MAX_RULE_SIZE);
			InString(bos,ri.m_MatchRule,MAX_RULE_SIZE);
			bos << ri.m_EndTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_RoomInfo& ri )
		{
			ri.ReSet();
			
			bis >> ri.m_RoomID;
			OutString(bis,ri.m_RoomName,MAX_ROOMNAME_SIZE);
			OutString(bis,ri.m_Password,ROOM_PASSWORD_SIZE);			
			OutString(bis,ri.m_AreaRule,MAX_RULE_SIZE);
			OutString(bis,ri.m_RoomRule,MAX_RULE_SIZE);
			OutString(bis,ri.m_RoomRuleEX,MAX_RULE_SIZE);
			OutString(bis,ri.m_MatchRule,MAX_RULE_SIZE);
			bis >> ri.m_EndTime;

			return bis;
		}
	};

	struct DBS_TableInfo
	{
		enum { XY_ID = MoGui_SERVER_TABLEINFO };

		unsigned short         m_RoomID;
		unsigned short         m_TableID;
		std::string            m_TableName;
		std::string            m_Password;
		std::string            m_TableRule;
		std::string            m_TableRuleEX;
		std::string            m_MatchRule;
		UINT32                 m_FoundByWho;
		UINT32                 m_TableEndTime;

		DBS_TableInfo() { ReSet(); }
		void ReSet()
		{
			m_RoomID = 0;
			m_TableID = 0;
			m_TableName = "";
			m_Password = "";
			m_TableRule = "";
			m_TableRuleEX = "";
			m_MatchRule = "";

			m_FoundByWho = 0;
			m_TableEndTime = 0;
		}

		friend bostream& operator<<( bostream& bos, const DBS_TableInfo& ti )
		{			
			bos << ti.m_RoomID;
			bos << ti.m_TableID;
			InString(bos,ti.m_TableName,MAX_TABLENAME_SIZE);
			InString(bos,ti.m_Password,TABLE_PASSWORD_SIZE);
			InString(bos,ti.m_TableRule,MAX_RULE_SIZE);
			InString(bos,ti.m_TableRuleEX,MAX_RULE_SIZE);
			InString(bos,ti.m_MatchRule,MAX_RULE_SIZE);
			bos << ti.m_FoundByWho;
			bos << ti.m_TableEndTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_TableInfo& ti )
		{
			ti.ReSet();

			bis >> ti.m_RoomID;
			bis >> ti.m_TableID;
			OutString(bis,ti.m_TableName,MAX_TABLENAME_SIZE);
			OutString(bis,ti.m_Password,TABLE_PASSWORD_SIZE);
			OutString(bis,ti.m_TableRule,MAX_RULE_SIZE);
			OutString(bis,ti.m_TableRuleEX,MAX_RULE_SIZE);
			OutString(bis,ti.m_MatchRule,MAX_RULE_SIZE);
			bis >> ti.m_FoundByWho;
			bis >> ti.m_TableEndTime;

			return bis;
		}
	};

	struct DBS_ReqBotPlayer
	{
		enum { XY_ID = MoGui_SERVER_REQBOTPLAYER };

		short                       m_ServerID;
		UINT32                      m_StartPlayerNumber;
		UINT32                      m_EndPlayerNumber;

		DBS_ReqBotPlayer() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_ReqBotPlayer& rbp )
		{			
			bos << rbp.m_ServerID;
			bos << rbp.m_StartPlayerNumber;
			bos << rbp.m_EndPlayerNumber;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_ReqBotPlayer& rbp )
		{
			rbp.ReSet();

			bis >> rbp.m_ServerID;
			bis >> rbp.m_StartPlayerNumber;
			bis >> rbp.m_EndPlayerNumber;

			return bis;
		}
	};
	struct DBS_RespBotPlayer
	{
		enum { XY_ID = MoGui_SERVER_RESPBOTPLAYER };
		enum 
		{
			SUCCESS,
			SERVERIDERROR,
			NOBOTPLAYER,
			RANGERROR,

			UNSUCCESS = 100
		};

		short                m_Flag;

		DBS_RespBotPlayer() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_RespBotPlayer& rbp )
		{			
			bos << rbp.m_Flag;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_RespBotPlayer& rbp )
		{
			rbp.ReSet();

			bis >> rbp.m_Flag;

			return bis;
		}
	};
	struct DBS_BotPlayerData
	{
		enum { XY_ID = MoGui_SERVER_BOTPLAYERDATA };
		enum { WinTypeNum = 10 };
		
		unsigned short                m_AID;
		unsigned int                  m_PID;

		unsigned char                 m_Sex;
		
		std::string                   m_NickName;
		std::string					  m_HeadPicURL;
		std::string                   m_HomePageURL;
		std::string                   m_City;

		SBigNumber                    m_nMoGuiMoney;
		SBigNumber    				  m_nGameMoney;
		SBigNumber                    m_nMatchJF;

		UINT32                        m_JoinTime;
		UINT32                        m_InvitePID;
		UINT8                         m_BotLevel;

		SBigNumber           	      m_nUpperLimite;
		SBigNumber           		  m_nLowerLimite;

		UINT8                         m_RightLevel;
		UINT8                         m_RightTimes;

		SBigNumber                    m_nJF;
		SBigNumber                    m_nEP;
		SBigNumber                    m_nWinTimes;
		SBigNumber                    m_nLossTimes;
		SBigNumber                    m_nGameTime;

		INT64                         m_MaxPai;
		UINT32                        m_MaxPaiTime;
		INT64                         m_MaxMoney;
		UINT32                        m_MaxMoneyTime;
		INT64                         m_MaxWin;
		UINT32                        m_MaxWinTime;

		int                           m_WinRecord[WinTypeNum];
		int                           m_Achieve[4];

		unsigned char                 m_VipLevel;
		INT16                         m_FriendCount;

		DBS_BotPlayerData() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;

			m_Sex = 0;

			m_NickName = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";

			m_nMoGuiMoney = 0;
			m_nGameMoney = 0;
			m_nMatchJF = 0;

			m_JoinTime = 0;
			m_InvitePID = 0;
			m_BotLevel = 0;

			m_nUpperLimite = 0;
			m_nLowerLimite = 0;

			m_RightLevel = 0;
			m_RightTimes = 0;

			m_nJF = 0;
			m_nEP = 0;
			m_nWinTimes = 0;
			m_nLossTimes = 0;
			m_nGameTime = 0;

			m_MaxPai = 0;
			m_MaxPaiTime = 0;
			m_MaxMoney = 0;
			m_MaxMoneyTime = 0;
			m_MaxWin = 0;
			m_MaxWinTime = 0;

			memset(m_WinRecord,0,sizeof(m_WinRecord));
			memset(m_Achieve,0,sizeof(m_Achieve));

			m_VipLevel = 0;
			m_FriendCount = 0;
		}

		friend bostream& operator<<( bostream& bos, const DBS_BotPlayerData& bpd )
		{
			bos << bpd.m_AID;
			bos << bpd.m_PID;
			bos << bpd.m_Sex;

			InString(bos,bpd.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,bpd.m_HeadPicURL,MAX_URL_SIZE);			
			InString(bos,bpd.m_HomePageURL,MAX_URL_SIZE);
			InString(bos,bpd.m_City,MAX_CITYNAME_SIZE);

			bos << bpd.m_nMoGuiMoney;
			bos << bpd.m_nGameMoney;
			bos << bpd.m_nMatchJF;

			bos << bpd.m_JoinTime;
			bos << bpd.m_InvitePID;
			bos << bpd.m_BotLevel;

			bos << bpd.m_nUpperLimite;
			bos << bpd.m_nLowerLimite;
			bos << bpd.m_RightLevel;
			bos << bpd.m_RightTimes;
			
			bos << bpd.m_nJF;
			bos << bpd.m_nEP;			
			bos << bpd.m_nWinTimes;
			bos << bpd.m_nLossTimes;
			bos << bpd.m_nGameTime;

			bos << bpd.m_MaxPai;
			bos << bpd.m_MaxPaiTime;
			bos << bpd.m_MaxMoney;
			bos << bpd.m_MaxMoneyTime;
			bos << bpd.m_MaxWin;
			bos << bpd.m_MaxWinTime;

			for ( int i=0;i<WinTypeNum;i++)
			{
				bos << bpd.m_WinRecord[i];
			}
			for ( int i=0;i<4;i++)
			{
				bos << bpd.m_Achieve[i];
			}

			bos << bpd.m_VipLevel;
			bos << bpd.m_FriendCount;

			return bos;
		}

		friend bistream& operator>>( bistream& bis, DBS_BotPlayerData& bpd )
		{
			bpd.ReSet();
			
			bis >> bpd.m_AID;
			bis >> bpd.m_PID;
			bis >> bpd.m_Sex;
			
			OutString(bis,bpd.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,bpd.m_HeadPicURL,MAX_URL_SIZE);
			OutString(bis,bpd.m_HomePageURL,MAX_URL_SIZE);
			OutString(bis,bpd.m_City,MAX_CITYNAME_SIZE);

			bis >> bpd.m_nMoGuiMoney;
			bis >> bpd.m_nGameMoney;
			bis >> bpd.m_nMatchJF;

			bis >> bpd.m_JoinTime;
			bis >> bpd.m_InvitePID;
			bis >> bpd.m_BotLevel;

			bis >> bpd.m_nUpperLimite;
			bis >> bpd.m_nLowerLimite;
			bis >> bpd.m_RightLevel;
			bis >> bpd.m_RightTimes;

			bis >> bpd.m_nJF;
			bis >> bpd.m_nEP;
			bis >> bpd.m_nWinTimes;
			bis >> bpd.m_nLossTimes;
			bis >> bpd.m_nGameTime;

			bis >> bpd.m_MaxPai;
			bis >> bpd.m_MaxPaiTime;
			bis >> bpd.m_MaxMoney;
			bis >> bpd.m_MaxMoneyTime;
			bis >> bpd.m_MaxWin;
			bis >> bpd.m_MaxWinTime;

			for ( int i=0;i<WinTypeNum;i++)
			{
				bis >> bpd.m_WinRecord[i];
			}
			for ( int i=0;i<4;i++)
			{
				bis >> bpd.m_Achieve[i];
			}
			bis >> bpd.m_VipLevel;
			bis >> bpd.m_FriendCount;

			return bis;
		}
	};

	struct DBS_HonorInfo
	{
		enum { XY_ID = DBServerXYID_HonorInfo };

		unsigned short         m_HonorID;
		BYTE                   m_Type;
		BYTE                   m_Idx;
		SBigNumber             m_HonorMoney;

		DBS_HonorInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_HonorInfo& src )
		{			
			bos << src.m_HonorID;
			bos << src.m_Type;
			bos << src.m_Idx;
			bos << src.m_HonorMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_HonorInfo& src )
		{
			src.ReSet();
			bis >> src.m_HonorID;
			bis >> src.m_Type;
			bis >> src.m_Idx;
			bis >> src.m_HonorMoney;
			return bis;
		}
	};

	struct DBS_HonorInfoList
	{
		enum { XY_ID = DBServerXYID_HonorInfoList };
		enum { MAX_COUNT = 200 };

		std::vector<DBS_HonorInfo> m_HIList;

		DBS_HonorInfoList() { ReSet(); }
		void ReSet() { m_HIList.clear(); }

		friend bostream& operator<<( bostream& bos, const DBS_HonorInfoList& src )
		{
			InVector(bos,src.m_HIList,MAX_COUNT);
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_HonorInfoList& src )
		{
			src.ReSet();
			OutVector(bis,src.m_HIList,MAX_COUNT);
			return bis;
		}
	};

	struct WDB_ReqFinishHonor
	{
		enum { XY_ID = DBServerXYID_ReqFinishHonor };

		short                  m_AID;
		unsigned int           m_PID;

		short                  m_HonorID;		

		WDB_ReqFinishHonor() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const WDB_ReqFinishHonor& src )
		{			
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_HonorID;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_ReqFinishHonor& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_HonorID;
			return bis;
		}
	};

	struct WDB_RespFinishHonor
	{
		enum { XY_ID = DBServerXYID_RespFinishHonor };

		short                  m_AID;
		unsigned int           m_PID;

		short                  m_HonorID;
		SBigNumber             m_HonorMoney;

		WDB_RespFinishHonor() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const WDB_RespFinishHonor& src )
		{			
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_HonorID;
			bos << src.m_HonorMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_RespFinishHonor& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_HonorID;
			bis >> src.m_HonorMoney;
			return bis;
		}
	};

	struct DBS_GameLevelInfo
	{
		enum { XY_ID = DBServerXYID_GameLevelInfo };

		INT16              m_PTLevel;
		INT32              m_TotalPT;
		INT32              m_AddPT;
		INT32              m_AwardMoney;

		DBS_GameLevelInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_GameLevelInfo& src )
		{			
			bos << src.m_PTLevel;
			bos << src.m_TotalPT;
			bos << src.m_AddPT;
			bos << src.m_AwardMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_GameLevelInfo& src )
		{
			src.ReSet();
			bis >> src.m_PTLevel;
			bis >> src.m_TotalPT;
			bis >> src.m_AddPT;
			bis >> src.m_AwardMoney;
			return bis;
		}
	};

	struct DBS_PlayerMoneyCheck
	{
		enum { XY_ID = DBServerXYID_PlayerMoneyCheck };

		UINT32             m_PID;
		SBigNumber         m_GameMoney;

		DBS_PlayerMoneyCheck() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_PlayerMoneyCheck& src )
		{			
			bos << src.m_PID;
			bos << src.m_GameMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_PlayerMoneyCheck& src )
		{
			src.ReSet();
			bis >> src.m_PID;
			bis >> src.m_GameMoney;
			return bis;
		}
	};

	struct DBS_msgFaceInfo
	{
		unsigned short         m_FaceID;
		unsigned short         m_Type;
		unsigned short         m_PriceFlag;
		unsigned short         m_Price;
		int                    m_MinPrice;
		int                    m_MaxPrice;

		DBS_msgFaceInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_msgFaceInfo& msgFI )
		{			
			bos << msgFI.m_FaceID;
			bos << msgFI.m_Type;
			bos << msgFI.m_PriceFlag;
			bos << msgFI.m_Price;
			bos << msgFI.m_MinPrice;
			bos << msgFI.m_MaxPrice;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_msgFaceInfo& msgFI )
		{
			msgFI.ReSet();

			bis >> msgFI.m_FaceID;
			bis >> msgFI.m_Type;
			bis >> msgFI.m_PriceFlag;
			bis >> msgFI.m_Price;
			bis >> msgFI.m_MinPrice;
			bis >> msgFI.m_MaxPrice;

			return bis;
		}

		DBS_msgFaceInfo(const DBS_msgFaceInfo& msgfi)
		{
			Copy(msgfi);
		}
		DBS_msgFaceInfo& operator=(const DBS_msgFaceInfo& msgfi)
		{
			if( this != &msgfi )
			{
				Copy(msgfi);
			}
			return *this;
		}
		void Copy(const DBS_msgFaceInfo& msgfi)
		{
			m_FaceID         = msgfi.m_FaceID;
			m_Type           = msgfi.m_Type;
			m_PriceFlag      = msgfi.m_PriceFlag;
			m_Price          = msgfi.m_Price;
			m_MinPrice       = msgfi.m_MinPrice;
			m_MaxPrice       = msgfi.m_MaxPrice;
		}
	};

	struct DBS_FaceInfoList
	{
		enum { XY_ID = MoGui_Server_FaceInfoList };
		enum { MAX_COUNT = 100 };

		mutable short          m_nCount;
		DBS_msgFaceInfo        m_FIList[MAX_COUNT];

		DBS_FaceInfoList() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_FaceInfoList& FI )
		{
			FI.m_nCount = 0;
			for ( int nCount=0;nCount<FI.MAX_COUNT;++nCount)
			{
				if ( FI.m_FIList[nCount].m_FaceID > 0)
				{
					FI.m_nCount++;
				}
			}

			bos << FI.m_nCount;
			for ( int nCount=0;nCount<FI.MAX_COUNT;++nCount)
			{
				if ( FI.m_FIList[nCount].m_FaceID > 0)
				{
					bos << FI.m_FIList[nCount];
				}
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_FaceInfoList& FI )
		{
			FI.ReSet();

			bis >> FI.m_nCount;
			if ( FI.m_nCount > FI.MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}
			for ( int nCount=0;nCount<FI.m_nCount;nCount++)
			{
				bis >> FI.m_FIList[nCount];
			}

			return bis;
		}
	};

	struct DBS_msgGiftInfo
	{
		unsigned short         m_GiftID;
		unsigned short         m_Type;
		unsigned short         m_PriceFlag;
		int                    m_Price;
		int                    m_MinPrice;
		int                    m_MaxPrice;
		unsigned short         m_Rebate;
		unsigned int           m_CurLastTime;
		unsigned int           m_LastTime;

		DBS_msgGiftInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_msgGiftInfo& msgGI )
		{			
			bos << msgGI.m_GiftID;
			bos << msgGI.m_Type;
			bos << msgGI.m_PriceFlag;
			bos << msgGI.m_Price;
			bos << msgGI.m_MinPrice;
			bos << msgGI.m_MaxPrice;
			bos << msgGI.m_Rebate;
			bos << msgGI.m_CurLastTime;
			bos << msgGI.m_LastTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_msgGiftInfo& msgGI )
		{
			msgGI.ReSet();

			bis >> msgGI.m_GiftID;
			bis >> msgGI.m_Type;
			bis >> msgGI.m_PriceFlag;
			bis >> msgGI.m_Price;
			bis >> msgGI.m_MinPrice;
			bis >> msgGI.m_MaxPrice;
			bis >> msgGI.m_Rebate;
			bis >> msgGI.m_CurLastTime;
			bis >> msgGI.m_LastTime;

			return bis;
		}

		DBS_msgGiftInfo(const DBS_msgGiftInfo& msggi)
		{
			Copy(msggi);
		}
		DBS_msgGiftInfo& operator=(const DBS_msgGiftInfo& msggi)
		{
			if( this != &msggi )
			{
				Copy(msggi);
			}
			return *this;
		}
		void Copy(const DBS_msgGiftInfo& msggi)
		{
			m_GiftID         = msggi.m_GiftID;
			m_Type           = msggi.m_Type;
			m_PriceFlag      = msggi.m_PriceFlag;
			m_Price          = msggi.m_Price;
			m_MinPrice       = msggi.m_MinPrice;
			m_MaxPrice       = msggi.m_MaxPrice;
			m_Rebate         = msggi.m_Rebate;
			m_CurLastTime    = msggi.m_CurLastTime;
			m_LastTime       = msggi.m_LastTime;			
		}
	};

	struct DBS_GiftInfoList
	{
		enum { XY_ID = MoGui_Server_GiftInfoList };
		enum { MAX_COUNT = 100 };

		mutable short          m_nCount;
		DBS_msgGiftInfo        m_GIList[MAX_COUNT];

		DBS_GiftInfoList() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_GiftInfoList& GI )
		{
			GI.m_nCount = 0;
			for ( int nCount=0;nCount<MAX_COUNT;++nCount)
			{
				if ( GI.m_GIList[nCount].m_GiftID > 0)
				{
					GI.m_nCount++;
				}
			}

			bos << GI.m_nCount;
			for ( int nCount=0;nCount<MAX_COUNT;++nCount)
			{
				if ( GI.m_GIList[nCount].m_GiftID > 0)
				{
					bos << GI.m_GIList[nCount];
				}
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_GiftInfoList& GI )
		{
			GI.ReSet();

			bis >> GI.m_nCount;
			if ( GI.m_nCount > MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}
			for ( int nCount=0;nCount<GI.m_nCount;nCount++)
			{
				bis >> GI.m_GIList[nCount];
			}

			return bis;
		}
	};


	struct DBS_MatchInfo
	{
		enum { XY_ID = DBServerXYID_MatchInfo };

		int                    m_MatchID;
		int                    m_MatchType;
		int                    m_Ticket;
		int                    m_TakeMoney;
		int                    m_StartMoney;
		std::string            m_strBlind;
		std::string            m_strAward;
		std::string            m_StrRule;
		UINT32                 m_StartTime;
		int                    m_StartInterval;

		DBS_MatchInfo() { ReSet(); }
		void ReSet()
		{
			m_MatchID = 0;
			m_MatchType = 0;
			m_Ticket = 0;
			m_TakeMoney = 0;
			m_StartMoney = 0;
			m_strBlind = "";
			m_strAward = "";
			m_StrRule = "";
			m_StartTime = 0;
			m_StartInterval = 0;
		}

		friend bostream& operator<<( bostream& bos, const DBS_MatchInfo& src )
		{			
			bos << src.m_MatchID;
			bos << src.m_MatchType;
			bos << src.m_Ticket;
			bos << src.m_TakeMoney;
			bos << src.m_StartMoney;
			InString(bos,src.m_strBlind,MAX_RULE_SIZE);
			InString(bos,src.m_strAward,MAX_RULE_SIZE);			
			InString(bos,src.m_StrRule,MAX_RULE_SIZE);
			bos << src.m_StartTime;
			bos << src.m_StartInterval;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_MatchInfo& src )
		{
			src.ReSet();

			bis >> src.m_MatchID;
			bis >> src.m_MatchType;
			bis >> src.m_Ticket;
			bis >> src.m_TakeMoney;
			bis >> src.m_StartMoney;
			OutString(bis,src.m_strBlind,MAX_RULE_SIZE);
			OutString(bis,src.m_strAward,MAX_RULE_SIZE);
			OutString(bis,src.m_StrRule,MAX_RULE_SIZE);
			bis >> src.m_StartTime;
			bis >> src.m_StartInterval;

			return bis;
		}

		DBS_MatchInfo(const DBS_MatchInfo& src)
		{
			Copy(src);
		}
		DBS_MatchInfo& operator=(const DBS_MatchInfo& src)
		{
			if( this != &src )
			{
				Copy(src);
			}
			return *this;
		}
		void Copy(const DBS_MatchInfo& src)
		{
			m_MatchID         = src.m_MatchID;
			m_MatchType       = src.m_MatchType;
			m_Ticket          = src.m_Ticket;
			m_TakeMoney       = src.m_TakeMoney;
			m_StartMoney      = src.m_StartMoney;
			m_strBlind        = src.m_strBlind;
			m_strAward        = src.m_strAward;
			m_StrRule         = src.m_StrRule;
			m_StartTime       = src.m_StartTime;
			m_StartInterval   = src.m_StartInterval;
		}
	};

	struct DBS_ReqUserGiftInfo
	{
		enum { XY_ID = DBServerXYID_ReqUserGiftInfo };

		unsigned int              m_PID;

		DBS_ReqUserGiftInfo() { ReSet(); }
		void ReSet(){m_PID = 0;}

		friend bostream& operator<<( bostream& bos, const DBS_ReqUserGiftInfo& ugi )
		{
			bos << ugi.m_PID;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_ReqUserGiftInfo& ugi )
		{
			ugi.ReSet();
			bis >> ugi.m_PID;
			return bis;
		}
	};

	struct DBS_msgUserGiftInfo
	{
		unsigned int           m_GiftIdx;
		unsigned short         m_GiftID;
		int                    m_Price;
		unsigned int           m_ActionTime;
		std::string            m_NickName;

		DBS_msgUserGiftInfo() { ReSet(); }
		void ReSet()
		{ 
			m_GiftIdx = 0;
			m_GiftID = 0;
			m_Price = 0;
			m_ActionTime = 0;
			m_NickName = "";
		}

		friend bostream& operator<<( bostream& bos, const DBS_msgUserGiftInfo& msgGI )
		{			
			bos << msgGI.m_GiftIdx;
			bos << msgGI.m_GiftID;
			bos << msgGI.m_Price;
			bos << msgGI.m_ActionTime;
			InString(bos,msgGI.m_NickName,50);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_msgUserGiftInfo& msgGI )
		{
			msgGI.ReSet();
			
			bis >> msgGI.m_GiftIdx;
			bis >> msgGI.m_GiftID;
			bis >> msgGI.m_Price;
			bis >> msgGI.m_ActionTime;
			OutString(bis,msgGI.m_NickName,50);

			return bis;
		}

		DBS_msgUserGiftInfo(const DBS_msgUserGiftInfo& msggi)
		{
			Copy(msggi);
		}
		DBS_msgUserGiftInfo& operator=(const DBS_msgUserGiftInfo& msggi)
		{
			if( this != &msggi )
			{
				Copy(msggi);
			}
			return *this;
		}
		void Copy(const DBS_msgUserGiftInfo& msggi)
		{			
			m_GiftIdx      = msggi.m_GiftIdx;
			m_GiftID       = msggi.m_GiftID;
			m_Price        = msggi.m_Price;
			m_ActionTime   = msggi.m_ActionTime;
			m_NickName     = msggi.m_NickName;
		}
	};

	struct DBS_UserGiftInfo
	{
		enum { XY_ID = DBServerXYID_UserGiftInfo };

		short                         m_AID;
		unsigned int                  m_PID;
		DBS_msgUserGiftInfo           m_GiftInfo;

		DBS_UserGiftInfo() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_UserGiftInfo& ugi )
		{
			bos << ugi.m_AID;
			bos << ugi.m_PID;
			bos << ugi.m_GiftInfo;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_UserGiftInfo& ugi )
		{
			ugi.ReSet();

			bis >> ugi.m_AID;
			bis >> ugi.m_PID;
			bis >> ugi.m_GiftInfo;

			return bis;
		}
	};

	struct DBS_UserGiftInfoList
	{
		enum { XY_ID = DBServerXYID_UserGiftInfoList };
		enum { MAX_COUNT = 60 };

		short                              m_AID;
		unsigned int                       m_PID;
		BYTE                               m_Flag;
		mutable unsigned short             m_nCount;
		std::vector<DBS_msgUserGiftInfo>   m_listGiftInfo;

		DBS_UserGiftInfoList() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_Flag = 0;
			m_nCount = 0;
			m_listGiftInfo.clear();
		}

		friend bostream& operator<<( bostream& bos, const DBS_UserGiftInfoList& ugi )
		{
			ugi.m_nCount = short(min(ugi.m_listGiftInfo.size(),MAX_COUNT));

			bos << ugi.m_AID;
			bos << ugi.m_PID;
			bos << ugi.m_Flag;
			bos << ugi.m_nCount;

			for (int i=0;i<ugi.m_nCount;++i)
			{
				bos << ugi.m_listGiftInfo[i];
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_UserGiftInfoList& ugi )
		{
			ugi.ReSet();

			bis >> ugi.m_AID;
			bis >> ugi.m_PID;
			bis >> ugi.m_Flag;
			bis >> ugi.m_nCount;

			if ( ugi.m_nCount > MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}

			DBS_msgUserGiftInfo msgUGI;
			for (int i=0;i<ugi.m_nCount;i++)
			{
				bis >> msgUGI;
				ugi.m_listGiftInfo.push_back(msgUGI);
			}

			return bis;
		}
	};

	struct msgUserAward
	{		
		SBigNumber      m_nMoney;
		short           m_MoneyFlag;		

		msgUserAward() { ReSet(); }
		void ReSet()
		{
			m_MoneyFlag = 0;
			m_nMoney = 0;
		}

		friend bostream& operator<<( bostream& bos, const msgUserAward& src )
		{
			bos << src.m_nMoney;
			bos << src.m_MoneyFlag;
		
			return bos;
		}
		friend bistream& operator>>( bistream& bis, msgUserAward& src )
		{
			bis >> src.m_nMoney;
			bis >> src.m_MoneyFlag;

			return bis;
		}
	};

	struct DBS_UserAwardInfoList
	{
		enum { XY_ID = DBServerXYID_UserAwardInfoList };
		enum { MAX_COUNT = 100 };

		short                         m_AID;
		unsigned int                  m_PID;
		SBigNumber                    m_Money;
		mutable unsigned short        m_nCount;
		std::vector<msgUserAward>     m_listAwardInfo;

		DBS_UserAwardInfoList() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_Money = 0;
			m_nCount = 0;
			m_listAwardInfo.clear();
		}

		friend bostream& operator<<( bostream& bos, const DBS_UserAwardInfoList& src )
		{
			src.m_nCount = short(min(src.m_listAwardInfo.size(),MAX_COUNT));

			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Money;
			bos << src.m_nCount;

			for (int i=0;i<src.m_nCount;++i)
			{
				bos << src.m_listAwardInfo[i];
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_UserAwardInfoList& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Money;
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

	struct DBS_UserHuiYuan
	{
		enum { XY_ID = DBServerXYID_UserHuiYuan };

		short                         m_AID;
		unsigned int                  m_PID;

		SBigNumber                    m_AddMoney;
		SBigNumber                    m_DayMoney;
		unsigned char                 m_OpenBank;
		UINT32                        m_FreeFaceTime;
		unsigned char                 m_VipLevel;
		UINT32                        m_VipEndTime;
		UINT32                        m_ActionTime;

		DBS_UserHuiYuan() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_UserHuiYuan& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_AddMoney;
			bos << src.m_DayMoney;
			bos << src.m_OpenBank;
			bos << src.m_FreeFaceTime;
			bos << src.m_VipLevel;
			bos << src.m_VipEndTime;
			bos << src.m_ActionTime;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_UserHuiYuan& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_AddMoney;
			bis >> src.m_DayMoney;
			bis >> src.m_OpenBank;
			bis >> src.m_FreeFaceTime;
			bis >> src.m_VipLevel;
			bis >> src.m_VipEndTime;
			bis >> src.m_ActionTime;
			return bis;
		}
	};

	struct DBS_UserMoguiExchange
	{
		enum { XY_ID = DBServerXYID_MoguiExchange };

		short                         m_AID;
		unsigned int                  m_PID;
		INT32                         m_AddMoguiMoney;
		UINT32                        m_ActionTime;

		DBS_UserMoguiExchange() { ReSet(); }
		void ReSet() { memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const DBS_UserMoguiExchange& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_AddMoguiMoney;
			bos << src.m_ActionTime;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_UserMoguiExchange& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_AddMoguiMoney;
			bis >> src.m_ActionTime;
			return bis;
		}
	};

	struct WDB_ChangeUserInfo
	{
		enum { XY_ID = DBServerXYID_ChangeUserInfo };

		short                     m_AID;
		unsigned int              m_PID;
		
		unsigned char             m_Sex;
		std::string               m_NickName;
		std::string               m_HeadPicUrl;
		std::string               m_City;

		WDB_ChangeUserInfo() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;

			m_Sex = 0;		

			m_NickName = "";
			m_HeadPicUrl = "";
			m_City = "";
		}

		friend bostream& operator<<( bostream& bos, const WDB_ChangeUserInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_Sex;

			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicUrl,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_ChangeUserInfo& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_Sex;

			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicUrl,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			return bis;
		}
	};

	struct DBS_ReqPlayerGameMoney
	{
		enum { XY_ID = DBServerXYID_ReqPlayerGameMoney };

		short                     m_AID;
		unsigned int              m_PID;
		SBigNumber                m_nGameMoney;

		DBS_ReqPlayerGameMoney() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_nGameMoney = 0;
		}
		friend bostream& operator<<( bostream& bos, const DBS_ReqPlayerGameMoney& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nGameMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_ReqPlayerGameMoney& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nGameMoney;
			return bis;
		}
	};
	struct DBS_RespPlayerGameMoney
	{
		enum { XY_ID = DBServerXYID_RespPlayerGameMoney };

		short                     m_AID;
		unsigned int              m_PID;
		SBigNumber                m_nDiffMoney;

		DBS_RespPlayerGameMoney() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_nDiffMoney = 0;
		}
		friend bostream& operator<<( bostream& bos, const DBS_RespPlayerGameMoney& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nDiffMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_RespPlayerGameMoney& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nDiffMoney;
			return bis;
		}
	};

	struct DBS_To_GS_Flag
	{
		enum { XY_ID = DBServerXYID_DBS_GS_Flag };
		enum
		{
			SendFriendOver=2,
			CheckClientExist=3,
		};

		short                     m_AID;
		unsigned int              m_PID;

		short                     m_Flag;
		SBigNumber                m_nValue;

		DBS_To_GS_Flag() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_Flag = 0;
			m_nValue = 0;
		}
		friend bostream& operator<<( bostream& bos, const DBS_To_GS_Flag& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Flag;
			bos << src.m_nValue;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_To_GS_Flag& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Flag;
			bis >> src.m_nValue;
			return bis;
		}
	};

	struct GS_To_DBS_Flag
	{
		enum { XY_ID = DBServerXYID_GS_DBS_Flag };
		enum
		{
			AddContinuPlay       = 1,
			CheckClientExist     = 3,
			AwardPlayerInvite    = 4,
		};

		short                     m_AID;
		unsigned int              m_PID;

		short                     m_Flag;
		SBigNumber                m_nValue;

		GS_To_DBS_Flag() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_Flag = 0;
			m_nValue = 0;
		}
		friend bostream& operator<<( bostream& bos, const GS_To_DBS_Flag& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Flag;
			bos << src.m_nValue;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, GS_To_DBS_Flag& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Flag;
			bis >> src.m_nValue;
			return bis;
		}
	};

	struct DBS_UserFriendInfo
	{
		enum { XY_ID = MoGui_SERVER_USERFRIEND };
		enum { MAX_FRIENDNUM = 250 };

		short                         m_AID;
		unsigned int                  m_PID;
		BYTE                          m_Flag;
		std::vector<unsigned int>     m_vecFriendID;

		DBS_UserFriendInfo() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_Flag = 0;
			m_vecFriendID.clear();
		}
		friend bostream& operator<<( bostream& bos, const DBS_UserFriendInfo& uf )
		{
			bos << uf.m_AID;
			bos << uf.m_PID;
			bos << uf.m_Flag;
			InVector(bos,uf.m_vecFriendID,uf.MAX_FRIENDNUM);
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DBS_UserFriendInfo& uf )
		{
			uf.ReSet();
			bis >> uf.m_AID;
			bis >> uf.m_PID;
			bis >> uf.m_Flag;
			OutVector(bis,uf.m_vecFriendID,uf.MAX_FRIENDNUM);
			return bis;
		}
	};

	struct WDB_UserFriend
	{
		enum { XY_ID = MoGui_SERVER_WDB_USERFRIEND };

		unsigned char             m_Flag;
		unsigned int              m_LeftPID;
		unsigned int              m_RightPID;

		WDB_UserFriend() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_UserFriend& wuf )
		{
			bos << wuf.m_Flag;
			bos << wuf.m_LeftPID;
			bos << wuf.m_RightPID;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_UserFriend& wuf )
		{
			wuf.ReSet();

			bis >> wuf.m_Flag;
			bis >> wuf.m_LeftPID;
			bis >> wuf.m_RightPID;

			return bis;
		}
	};

	struct WDB_WinLoss
	{
		enum { XY_ID = MoGui_SERVER_WDB_WINLOSS };

		long long                 m_GameGUID;
		short                     m_AID;
		unsigned int              m_PID;
		short                     m_RoomID;
		short                     m_TableID;
		unsigned char             m_SitID;
		SBigNumber                m_nServiceMoney;
		SBigNumber                m_nWinLossMoney;
		unsigned char             m_EndGameFlag;
		unsigned char             m_LeftPai;
		unsigned char             m_RightPai;
		unsigned char             m_PaiType;

		SBigNumber                m_Money;
		SBigNumber                m_JF;
		SBigNumber                m_EP;

		WDB_WinLoss() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_WinLoss& wl )
		{
			bos << wl.m_GameGUID;
			bos << wl.m_AID;
			bos << wl.m_PID;
			bos << wl.m_RoomID;
			bos << wl.m_TableID;
			bos << wl.m_SitID;
			bos << wl.m_nServiceMoney;
			bos << wl.m_nWinLossMoney;
			bos << wl.m_EndGameFlag;
			bos << wl.m_LeftPai;
			bos << wl.m_RightPai;
			bos << wl.m_PaiType;

			bos << wl.m_Money;
			bos << wl.m_JF;
			bos << wl.m_EP;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_WinLoss& wl )
		{
			wl.ReSet();

			bis >> wl.m_GameGUID;
			bis >> wl.m_AID;
			bis >> wl.m_PID;
			bis >> wl.m_RoomID;
			bis >> wl.m_TableID;
			bis >> wl.m_SitID;
			bis >> wl.m_nServiceMoney;
			bis >> wl.m_nWinLossMoney;
			bis >> wl.m_EndGameFlag;
			bis >> wl.m_LeftPai;
			bis >> wl.m_RightPai;
			bis >> wl.m_PaiType;

			bis >> wl.m_Money;
			bis >> wl.m_JF;
			bis >> wl.m_EP;

			return bis;
		}
	};

	struct WDB_WinLossList
	{
		enum { XY_ID = MoGui_SERVER_WDB_WINLOSSLIST };
		
		mutable BYTE              m_nCount;
		WDB_WinLoss               m_WinLoss[MAX_PALYER_ON_TABLE];

		WDB_WinLossList() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_WinLossList& wll )
		{
			wll.m_nCount = 0;
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if ( wll.m_WinLoss[i].m_PID > 0 )
				{
					wll.m_nCount++;
				}
			}
			bos << wll.m_nCount;		
			for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if ( wll.m_WinLoss[i].m_PID > 0 )
				{
					bos << wll.m_WinLoss[i];
				}
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_WinLossList& wll )
		{
			wll.ReSet();

			bis >> wll.m_nCount;
			if ( wll.m_nCount > MAX_PALYER_ON_TABLE )
			{
				throw agproexception(agproexception::rangeerror);
			}
			for (int i=0;i<wll.m_nCount;i++)
			{
				bis >> wll.m_WinLoss[i];
			}

			return bis;
		}
	};

	struct WDB_RespWinLoss
	{
		enum { XY_ID = DBServerXYID_RespWinLoss };

		unsigned int             m_PID;

		WDB_RespWinLoss() { ReSet(); }
		void ReSet()
		{
			m_PID = 0;
		}

		friend bostream& operator<<( bostream& bos, const WDB_RespWinLoss& src )
		{
			bos << src.m_PID;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_RespWinLoss& src )
		{
			src.ReSet();

			bis >> src.m_PID;

			return bis;
		}
	};

	struct WDB_RespWinLossList
	{
		enum { XY_ID = DBServerXYID_RespWinLossList };
		enum { Max_Count = 100 };

		std::vector<unsigned int>      m_vecPID;

		WDB_RespWinLossList() { ReSet(); }
		void ReSet()
		{
			m_vecPID.clear();
		}

		friend bostream& operator<<( bostream& bos, const WDB_RespWinLossList& src )
		{
			InVector(bos,src.m_vecPID,Max_Count);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_RespWinLossList& src )
		{
			src.ReSet();

			OutVector(bis,src.m_vecPID,Max_Count);

			return bis;
		}
	};

	struct WDB_MatchResult
	{
		enum { XY_ID = DBServerXYID_WDB_MatchResult };

		long long                 m_MatchGUID;
		short                     m_AID;
		unsigned int              m_PID;
		short                     m_RoomID;
		short                     m_TableID;
		int                       m_MatchID;
		int                       m_MatchType;
		SBigNumber                m_Ticket;
		SBigNumber                m_TakeMoney;
		SBigNumber                m_AwardMoney;
		SBigNumber                m_AwardJF;
		int                       m_Position;

		SBigNumber                m_Money;
		SBigNumber                m_JF;
		SBigNumber                m_EP;

		WDB_MatchResult() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_MatchResult& src )
		{
			bos << src.m_MatchGUID;
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_MatchID;
			bos << src.m_MatchType;
			bos << src.m_Ticket;
			bos << src.m_TakeMoney;
			bos << src.m_AwardMoney;
			bos << src.m_AwardJF;
			bos << src.m_Position;

			bos << src.m_Money;
			bos << src.m_JF;
			bos << src.m_EP;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_MatchResult& src )
		{
			src.ReSet();

			bis >> src.m_MatchGUID;
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_MatchID;
			bis >> src.m_MatchType;
			bis >> src.m_Ticket;
			bis >> src.m_TakeMoney;
			bis >> src.m_AwardMoney;
			bis >> src.m_AwardJF;
			bis >> src.m_Position;

			bis >> src.m_Money;
			bis >> src.m_JF;
			bis >> src.m_EP;

			return bis;
		}
	};

	struct WDB_MatchWinLoss
	{
		enum { XY_ID = DBServerXYID_WDB_MatchWinLoss };

		long long                 m_MatchGUID;
		long long                 m_GameGUID;
		short                     m_AID;
		unsigned int              m_PID;
		short                     m_RoomID;
		short                     m_TableID;
		unsigned char             m_SitID;
		SBigNumber                m_nWinLossMoney;

		unsigned char             m_EndGameFlag;
		unsigned char             m_PaiType;

		WDB_MatchWinLoss() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_MatchWinLoss& wl )
		{
			bos << wl.m_MatchGUID;
			bos << wl.m_GameGUID;
			bos << wl.m_AID;
			bos << wl.m_PID;
			bos << wl.m_RoomID;
			bos << wl.m_TableID;
			bos << wl.m_SitID;
			bos << wl.m_nWinLossMoney;

			bos << wl.m_EndGameFlag;
			bos << wl.m_PaiType;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_MatchWinLoss& wl )
		{
			wl.ReSet();

			bis >> wl.m_MatchGUID;
			bis >> wl.m_GameGUID;
			bis >> wl.m_AID;
			bis >> wl.m_PID;
			bis >> wl.m_RoomID;
			bis >> wl.m_TableID;
			bis >> wl.m_SitID;
			bis >> wl.m_nWinLossMoney;

			bis >> wl.m_EndGameFlag;
			bis >> wl.m_PaiType;
		
			return bis;
		}
	};

	struct WDB_ReqCheckGameInfo
	{
		enum { XY_ID = DBServerXYID_WDB_ReqCheckGameInfo };

		short                     m_AID;
		unsigned int              m_PID;

		SBigNumber                m_nMoGuiMoney;
		SBigNumber                m_nGameMoney;
		SBigNumber                m_nMatchJF;

		SBigNumber                m_nJF;
		SBigNumber                m_nEP;

		SBigNumber                m_nUpperLimite;
		SBigNumber                m_nLowerLimite;

		SBigNumber                m_nWinTimes;
		SBigNumber                m_nLossTimes;
		SBigNumber                m_nGameTime;

		UINT8                     m_RightLevel;
		UINT8                     m_RightTimes;

		WDB_ReqCheckGameInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_ReqCheckGameInfo& src )
		{			
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_nMoGuiMoney;
			bos << src.m_nGameMoney;
			bos << src.m_nMatchJF;

			bos << src.m_nJF;
			bos << src.m_nEP;

			bos << src.m_nUpperLimite;
			bos << src.m_nLowerLimite;

			bos << src.m_nWinTimes;
			bos << src.m_nLossTimes;
			bos << src.m_nGameTime;

			bos << src.m_RightLevel;
			bos << src.m_RightTimes;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_ReqCheckGameInfo& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_nMoGuiMoney;
			bis >> src.m_nGameMoney;
			bis >> src.m_nMatchJF;

			bis >> src.m_nJF;
			bis >> src.m_nEP;

			bis >> src.m_nUpperLimite;
			bis >> src.m_nLowerLimite;

			bis >> src.m_nWinTimes;
			bis >> src.m_nLossTimes;
			bis >> src.m_nGameTime;

			bis >> src.m_RightLevel;
			bis >> src.m_RightTimes;

			return bis;
		}
	};

	struct WDB_IncomeAndPay
	{
		enum { XY_ID = MoGui_SERVER_WDB_IncomeAndPay };

		short              m_AID;
		unsigned int       m_PID;
		SBigNumber         m_nMoney;
		short              m_Flag;
		std::string        m_strReMark;
	
		WDB_IncomeAndPay() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_nMoney = 0;
			m_Flag = 0;
			m_strReMark = "";
		}

		friend bostream& operator<<( bostream& bos, const WDB_IncomeAndPay& Src )
		{			
			bos << Src.m_AID;
			bos << Src.m_PID;
			bos << Src.m_nMoney;
			bos << Src.m_Flag;
			InString(bos,Src.m_strReMark,MAX_MSG_LEN);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_IncomeAndPay& Src )
		{
			Src.ReSet();

			bis >> Src.m_AID;
			bis >> Src.m_PID;
			bis >> Src.m_nMoney;
			bis >> Src.m_Flag;
			OutString(bis,Src.m_strReMark,MAX_MSG_LEN);

			return bis;
		}
	};

	struct WDB_SendMoney
	{
		enum { XY_ID = MoGui_SERVER_WDB_SendMoney };
		
		short                     m_SendAID;
		unsigned int              m_SendPID;
		short                     m_RecvAID;
		unsigned int              m_RecvPID;
		SBigNumber                m_nSendMoney;
		SBigNumber                m_Service;

		WDB_SendMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_SendMoney& sm )
		{
			bos << sm.m_SendAID;
			bos << sm.m_SendPID;
			bos << sm.m_RecvAID;
			bos << sm.m_RecvPID;
			bos << sm.m_nSendMoney;
			bos << sm.m_Service;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_SendMoney& sm )
		{
			sm.ReSet();
			
			bis >> sm.m_SendAID;
			bis >> sm.m_SendPID;
			bis >> sm.m_RecvAID;
			bis >> sm.m_RecvPID;
			bis >> sm.m_nSendMoney;
			bis >> sm.m_Service;

			return bis;
		}
	};

	struct WDB_ShowFace
	{
		enum { XY_ID = MoGui_SERVER_WDB_ShowFace };

		short                     m_AID;
		unsigned int              m_PID;
		short                     m_FaceID;
		SBigNumber                m_PayMoney;

		WDB_ShowFace() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_ShowFace& SF )
		{
			bos << SF.m_AID;
			bos << SF.m_PID;
			bos << SF.m_FaceID;
			bos << SF.m_PayMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_ShowFace& SF )
		{
			SF.ReSet();

			bis >> SF.m_AID;
			bis >> SF.m_PID;
			bis >> SF.m_FaceID;
			bis >> SF.m_PayMoney;

			return bis;
		}
	};

	struct WDB_SendGift
	{
		enum { XY_ID = DBServerXYID_WDB_SendGift };
		enum { MAX_COUNT = 500 };
		
		short                     m_AID;
		unsigned int              m_PID;
		SBigNumber                m_PayMoney;
		short                     m_GiftID;
		short                     m_PriceFlag;
		int                       m_Price;

		mutable short                  m_nCount;
		std::vector<unsigned int>      m_vecToPID;

		WDB_SendGift() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_PayMoney = 0;
			m_GiftID = 0;
			m_PriceFlag = 0;
			m_Price = 0;

			m_nCount = 0;
			m_vecToPID.clear();
		}

		friend bostream& operator<<( bostream& bos, const WDB_SendGift& msgSG )
		{
			msgSG.m_nCount = short(min(msgSG.m_vecToPID.size(),MAX_COUNT));

			bos << msgSG.m_AID;
			bos << msgSG.m_PID;
			bos << msgSG.m_PayMoney;
			bos << msgSG.m_GiftID;
			bos << msgSG.m_PriceFlag;
			bos << msgSG.m_Price;

			bos << msgSG.m_nCount;
			for (int nCount=0;nCount<msgSG.m_nCount;nCount++)
			{
				bos << msgSG.m_vecToPID[nCount];
			}

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_SendGift& msgSG )
		{
			msgSG.ReSet();

			bis >> msgSG.m_AID;
			bis >> msgSG.m_PID;
			bis >> msgSG.m_PayMoney;
			bis >> msgSG.m_GiftID;
			bis >> msgSG.m_PriceFlag;
			bis >> msgSG.m_Price;
			
			bis >> msgSG.m_nCount;
			if ( msgSG.m_nCount > MAX_COUNT )
			{
				throw agproexception(agproexception::rangeerror);
			}

			unsigned int TempPID;
			for ( int i=0;i<msgSG.m_nCount;i++)
			{
				bis >> TempPID;
				msgSG.m_vecToPID.push_back(TempPID);
			}

			return bis;
		}
	};

	struct WDB_SoldGift
	{
		enum { XY_ID = DBServerXYID_WDB_SoldGift };
		enum { MAX_COUNT = 250 };

		short                     m_AID;
		unsigned int              m_PID;
		SBigNumber                m_nMoney;
		std::vector<int>          m_vecGiftIdx;

		WDB_SoldGift() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_nMoney = 0;
			m_vecGiftIdx.clear();
		}
		friend bostream& operator<<( bostream& bos, const WDB_SoldGift& msgSG )
		{
			bos << msgSG.m_AID;
			bos << msgSG.m_PID;
			bos << msgSG.m_nMoney;
			InVector(bos,msgSG.m_vecGiftIdx,msgSG.MAX_COUNT);
			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_SoldGift& msgSG )
		{
			msgSG.ReSet();
			bis >> msgSG.m_AID;
			bis >> msgSG.m_PID;
			bis >> msgSG.m_nMoney;
			OutVector(bis,msgSG.m_vecGiftIdx,msgSG.MAX_COUNT);
			return bis;
		}
	};

	struct WDB_CreateTable
	{
		enum { XY_ID = DBServerXYID_WDB_CreateTable };
	
		short                       m_AID;
		UINT32               		m_FoundByWho;
		SBigNumber                  m_nMoney;

		INT16                		m_ServerID;
		INT16                		m_RoomID;
		INT16                		m_TableID;

		std::string                 m_TableName;
		std::string                 m_Password;
		std::string                 m_TableRule;
		std::string                 m_TableRuleEX;
		std::string                 m_TableMatchRule;

		UINT32               		m_EndTime;	

		WDB_CreateTable() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_FoundByWho = 0;
			m_nMoney = 0;

			m_ServerID = 0;
			m_RoomID = 0;
			m_TableID = 0;

			m_TableName = "";
			m_Password = "";
			m_TableRule = "";
			m_TableRuleEX = "";
			m_TableMatchRule = "";

			m_EndTime = 0;
		}

		friend bostream& operator<<( bostream& bos, const WDB_CreateTable& src )
		{
			bos << src.m_AID;
			bos << src.m_FoundByWho;
			bos << src.m_nMoney;

			bos << src.m_ServerID;
			bos << src.m_RoomID;
			bos << src.m_TableID;

			InString(bos,src.m_TableName,MAX_TABLENAME_SIZE);
			InString(bos,src.m_Password,TABLE_PASSWORD_SIZE);
			InString(bos,src.m_TableRule,MAX_RULE_SIZE);
			InString(bos,src.m_TableRuleEX,MAX_RULE_SIZE);
			InString(bos,src.m_TableMatchRule,MAX_RULE_SIZE);

			bos << src.m_EndTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_CreateTable& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_FoundByWho;
			bis >> src.m_nMoney;

			bis >> src.m_ServerID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;

			OutString(bis,src.m_TableName,MAX_TABLENAME_SIZE);
			OutString(bis,src.m_Password,TABLE_PASSWORD_SIZE);
			OutString(bis,src.m_TableRule,MAX_RULE_SIZE);
			OutString(bis,src.m_TableRuleEX,MAX_RULE_SIZE);
			OutString(bis,src.m_TableMatchRule,MAX_RULE_SIZE);
		
			bis >> src.m_EndTime;

			return bis;
		}
	};

	struct  WDB_ChatMsg
	{
		enum { XY_ID = DBServerXYID_WDB_ChatMsg };

		unsigned int             m_PID;
		unsigned int             m_ToPID;

		unsigned short           m_RoomID;
		unsigned short           m_TableID;
		BYTE                     m_SitID;

		std::string              m_ChatMsg;

		WDB_ChatMsg() { ReSet(); }
		void ReSet()
		{
			m_PID = 0;
			m_ToPID = 0;

			m_RoomID = 0;
			m_TableID = 0;
			m_SitID = 0;

			m_ChatMsg = "";
		}
		friend bostream& operator<<( bostream& bos, const WDB_ChatMsg& src )
		{
			bos << src.m_PID;
			bos << src.m_ToPID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_SitID;
			InString(bos,src.m_ChatMsg,MAX_TABLECHAT_SIZE);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_ChatMsg& src )
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_ToPID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_SitID;
			OutString(bis,src.m_ChatMsg,MAX_TABLECHAT_SIZE);

			return bis;
		}
	};

	struct ReqChangeBankMoney
	{
		enum { XY_ID = DBServerXYID_ReqChangeBankMoney };
		enum { BankToGame=0,GameToBank };

		short                    m_AID;
		unsigned int             m_PID;
		unsigned char            m_Flag;
		SBigNumber               m_nMoney;

		ReqChangeBankMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this)); }

		friend bostream& operator<<( bostream& bos, const ReqChangeBankMoney& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Flag;
			bos << src.m_nMoney;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, ReqChangeBankMoney& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Flag;
			bis >> src.m_nMoney;
			return bis;
		}
	};

	struct WDB_MaxPai
	{
		enum { XY_ID = DBServerXYID_WDB_MaxPai };

		short                    m_AID;
		unsigned int             m_PID;
		unsigned int             m_MaxPaiTime;
		long long                m_MaxPai;

		WDB_MaxPai() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_MaxPai& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_MaxPaiTime;
			bos << src.m_MaxPai;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_MaxPai& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_MaxPaiTime;
			bis >> src.m_MaxPai;

			return bis;
		}
	};

	struct WDB_MaxWin
	{
		enum { XY_ID = DBServerXYID_WDB_MaxWin };

		short                    m_AID;
		unsigned int             m_PID;
		unsigned int             m_MaxWinTime;
		long long                m_MaxWin;

		WDB_MaxWin() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_MaxWin& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_MaxWinTime;
			bos << src.m_MaxWin;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_MaxWin& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_MaxWinTime;
			bis >> src.m_MaxWin;

			return bis;
		}
	};

	struct WDB_MaxMoney
	{
		enum { XY_ID = DBServerXYID_WDB_MaxMoney };

		short                    m_AID;
		unsigned int             m_PID;
		unsigned int             m_MaxMoneyTime;
		long long                m_MaxMoney;

		WDB_MaxMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_MaxMoney& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_MaxMoneyTime;
			bos << src.m_MaxMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_MaxMoney& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_MaxMoneyTime;
			bis >> src.m_MaxMoney;

			return bis;
		}
	};

	struct WDB_PlayerActionLog
	{
		enum { XY_ID = DBServerXYID_WDB_PlayerAction };

		short                    m_AID;
		unsigned int             m_PID;

		int                      m_OneLifeTotalTime;
		int                      m_OneLifeGameTime;
		int                      m_OneLifeMatchTime;
		INT16                    m_EnterRoomTimes;
		INT16                    m_EnterTableTimes;
		INT16                    m_SitDownTimes;
		INT16                    m_PlayTimes;

		WDB_PlayerActionLog() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_PlayerActionLog& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_OneLifeTotalTime;
			bos << src.m_OneLifeGameTime;
			bos << src.m_OneLifeMatchTime;
			bos << src.m_EnterRoomTimes;
			bos << src.m_EnterTableTimes;
			bos << src.m_SitDownTimes;
			bos << src.m_PlayTimes;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_PlayerActionLog& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_OneLifeTotalTime;
			bis >> src.m_OneLifeGameTime;
			bis >> src.m_OneLifeMatchTime;
			bis >> src.m_EnterRoomTimes;
			bis >> src.m_EnterTableTimes;
			bis >> src.m_SitDownTimes;
			bis >> src.m_PlayTimes;

			return bis;
		}
	};

	struct WDB_PlayerClientError
	{
		enum { XY_ID = DBServerXYID_WDB_PlayerClientError };

		short                    m_AID;
		unsigned int             m_PID;

		short                    m_Flag;
		std::string              m_strDes;

		WDB_PlayerClientError() { ReSet(); }
		void ReSet(){
			m_AID = 0;
			m_PID = 0;
			m_Flag = 0;
			m_strDes = "";
		}

		friend bostream& operator<<( bostream& bos, const WDB_PlayerClientError& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;			
			bos << src.m_Flag;
			InString(bos, src.m_strDes, 250);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_PlayerClientError& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;			
			bis >> src.m_Flag;
			OutString(bis, src.m_strDes, 250);

			return bis;
		}
	};

	struct WDB_PlayerAward
	{
		enum { XY_ID = DBServerXYID_WDB_PlayerAward };

		short                    m_AID;
		unsigned int             m_PID;
		short                    m_nFlag;
		short                    m_PayMode;
		unsigned int             m_EndTime;
		SBigNumber               m_nMoney;
		std::string              m_strLog;

		WDB_PlayerAward() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_nFlag = 0;
			m_PayMode = 0;
			m_EndTime = 0;
			m_nMoney = 0;
			m_strLog = "";
		}
		friend bostream& operator<<( bostream& bos, const WDB_PlayerAward& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nFlag;
			bos << src.m_PayMode;
			bos << src.m_EndTime;
			bos << src.m_nMoney;
			InString(bos,src.m_strLog,50);
			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_PlayerAward& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nFlag;
			bis >> src.m_PayMode;
			bis >> src.m_EndTime;
			bis >> src.m_nMoney;
			OutString(bis,src.m_strLog,50);
			return bis;
		}
	};

	struct WDB_ReportPlayerOnline
	{
		enum { XY_ID = DBServerXYID_ReportPlayerOnline };

		short                     m_ServerID;
		short                     m_AID;
		short                     m_Total;
		short                     m_Lobby;
		short                     m_Table; //观旁
		short                     m_Game;
		short                     m_Match;

		WDB_ReportPlayerOnline() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const WDB_ReportPlayerOnline& src )
		{
			bos << src.m_ServerID;
			bos << src.m_AID;
			bos << src.m_Total;
			bos << src.m_Lobby;
			bos << src.m_Table;
			bos << src.m_Game;
			bos << src.m_Match;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, WDB_ReportPlayerOnline& src )
		{
			src.ReSet();

			bis >> src.m_ServerID;
			bis >> src.m_AID;
			bis >> src.m_Total;
			bis >> src.m_Lobby;
			bis >> src.m_Table;
			bis >> src.m_Game;
			bis >> src.m_Match;

			return bis;
		}
	};
}
}
}