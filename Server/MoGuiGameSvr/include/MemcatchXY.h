#pragma once

#include "MoGuiGame.h"

namespace MoGui
{
namespace MemcatchXY
{
namespace DeZhou
{
	//struct memGSUserData                 //游戏送游戏币活动

	//struct memUserLandRecord             //记录玩家登陆的情况

	//struct memUserBaseInfo               //用于客户端取玩家的头像和NICKNAME信息
	//struct memUserInfo                   //用于存放玩家的信息

	//struct memUserDataInfo               //记录玩家相关信息
	//struct memUserGameInfo
	//struct memUserGameInfoEx             //用于存放玩家游戏相关信息，但是不经常更新的内容
	//struct memUserGameWinRecord          //用于存放玩家以何种方式赢的数据，经常更新，但在cach也备一份

	//struct memGiftInfo                   //用于存放礼物的信息，同时供网页接口调用

	//游戏送游戏币活动
	struct memGSUserData
	{
		INT16     m_AID;
		UINT32    m_PID;

		INT32     m_LastActTime;         //上次记录的时间
		INT8      m_CurStep;             //当前第几步
		INT16     m_PassTime;            //当前步过去的时间

		UINT32    m_FirstTimeToday;
		UINT16    m_TodayGameSecond;
		UINT16    m_TodayGameCount;
		INT8      m_HaveAwardInvite;
		INT8      m_JiuMingTimes;

		memGSUserData(){ Reset();}
		void Reset(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<(bostream& bos, const memGSUserData& src)
		{
			bos << src.m_LastActTime;
			bos << src.m_CurStep;
			bos << src.m_PassTime;

			bos << src.m_FirstTimeToday;
			bos << src.m_TodayGameSecond;
			bos << src.m_TodayGameCount;
			bos << src.m_HaveAwardInvite;

			bos << src.m_JiuMingTimes;

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memGSUserData& src)
		{
			bis >> src.m_LastActTime;
			bis >> src.m_CurStep;
			bis >> src.m_PassTime;

			bis >> src.m_FirstTimeToday;
			bis >> src.m_TodayGameSecond;
			bis >> src.m_TodayGameCount;
			bis >> src.m_HaveAwardInvite;

			bis >> src.m_JiuMingTimes;

			return bis;
		}
	};

	//记录玩家登陆的情况
	struct memUserLandRecord
	{
		INT16            m_AID;
		UINT32           m_PID;

		UINT32           m_FirstTodayTime;
		BYTE             m_ContinueLogin;
		BYTE             m_ContinuePlay;

		memUserLandRecord(){ Reset();}
		void Reset(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<(bostream& bos, const memUserLandRecord& src)
		{
			bos << src.m_FirstTodayTime;
			bos << src.m_ContinueLogin;
			bos << src.m_ContinuePlay;
			
			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserLandRecord& src)
		{
			bis >> src.m_FirstTodayTime;
			bis >> src.m_ContinueLogin;
			bis >> src.m_ContinuePlay;

			return bis;
		}
	};

	//记录玩家所有相关信息的，最长时间保存的
	struct memUserDataInfo
	{
		INT16                                 m_AID;
		UINT32                                m_PID;

		int                                   m_HaveGameInfo;
		int                                   m_Sex;
		UINT32                                m_InvitePID;
		UINT32                                m_JoinTime;
		int                                   m_ChangeName;
		int                                   m_PlayerLevel;
		int                                   m_GiftIdx;

		std::string                           m_NickName;
		std::string                           m_HeadPicURL;
		std::string                           m_HomePageURL;
		std::string                           m_City;

		memUserDataInfo(){ Reset(); }
		void Reset()
		{
			m_PID = 0;
			m_AID = 0;

			m_HaveGameInfo = 0;
			m_Sex = 0;
			m_InvitePID = 0;
			m_JoinTime = 0;
			m_ChangeName = 0;
			m_PlayerLevel = 1;
			m_GiftIdx = 0;

			m_NickName = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";
		}
		friend bostream& operator<<(bostream& bos, const memUserDataInfo& src)
		{
			bos << src.m_HaveGameInfo;
			bos << src.m_Sex;
			bos << src.m_InvitePID;
			bos << src.m_JoinTime;
			bos << src.m_ChangeName;
			bos << src.m_PlayerLevel;
			bos << src.m_GiftIdx;

			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicURL,MAX_URL_SIZE);
			InString(bos,src.m_HomePageURL,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserDataInfo& src)
		{
			bis >> src.m_HaveGameInfo;
			bis >> src.m_Sex;
			bis >> src.m_InvitePID;
			bis >> src.m_JoinTime;
			bis >> src.m_ChangeName;
			bis >> src.m_PlayerLevel;
			bis >> src.m_GiftIdx;

			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicURL,MAX_URL_SIZE);
			OutString(bis,src.m_HomePageURL,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			return bis;
		}
	};

	struct memUserBaseInfo
	{
		//用于客户端取的时候客户端没有玩家AID
		UINT32                                m_PID;
		INT16                                 m_AID;

		std::string                           m_NickName;
		std::string                           m_HeadPicURL;
		std::string                           m_HomePageURL;

		memUserBaseInfo(){ Reset();}
		void Reset()
		{
			m_PID = 0;
			m_AID = 0;
			m_NickName = "";			
			m_HeadPicURL = "";
			m_HomePageURL = "";
		}

		friend bostream& operator<<(bostream& bos, const memUserBaseInfo& src)
		{
			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicURL,MAX_URL_SIZE);
			InString(bos,src.m_HomePageURL,MAX_URL_SIZE);

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserBaseInfo& src)
		{
			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicURL,MAX_URL_SIZE);
			OutString(bis,src.m_HomePageURL,MAX_URL_SIZE);

			return bis;
		}
	};

	struct memUserInfo
	{
		INT16                                 m_AID;
		UINT32                                m_PID;
		
		UINT32                                m_JoinTime;

		std::string                           m_NickName;
		std::string                           m_HeadPicURL;
		std::string                           m_HomePageURL;
		std::string                           m_City;

		INT64                                 m_nGameMoney;
		INT64                                 m_nMatchJF;
		INT64                                 m_nJF;
		INT64                                 m_nEP;
		INT32                                 m_nWinTimes;
		INT32                                 m_nLossTimes;
		INT32                                 m_nGameTime;

		memUserInfo(){ Reset();}
		void Reset()
		{
			m_AID = 0;
			m_PID = 0;
			
			m_JoinTime = 0;
			m_NickName = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";

			m_nGameMoney = 0;
			m_nMatchJF = 0;
			m_nJF = 0;
			m_nEP = 0;
			m_nWinTimes = 0;
			m_nLossTimes = 0;
			m_nGameTime = 0;
		}

		friend bostream& operator<<(bostream& bos, const memUserInfo& src)
		{			
			bos << src.m_JoinTime;
			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicURL,MAX_URL_SIZE);
			InString(bos,src.m_HomePageURL,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			bos << src.m_nGameMoney;
			bos << src.m_nMatchJF;
			bos << src.m_nJF;
			bos << src.m_nEP;
			bos << src.m_nWinTimes;
			bos << src.m_nLossTimes;
			bos << src.m_nGameTime;

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserInfo& src)
		{			
			bis >> src.m_JoinTime;
			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicURL,MAX_URL_SIZE);
			OutString(bis,src.m_HomePageURL,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			bis >> src.m_nGameMoney;
			bis >> src.m_nMatchJF;
			bis >> src.m_nJF;
			bis >> src.m_nEP;
			bis >> src.m_nWinTimes;
			bis >> src.m_nLossTimes;
			bis >> src.m_nGameTime;

			return bis;
		}
	};

	struct memUserGameInfo
	{
		UINT32                                m_PID;
		INT16                                 m_AID;

		INT64                                 m_nGameMoney;
		INT32                                 m_nMoguiMoney;
		INT64                                 m_nMatchJF;

		INT64                                 m_JF;
		INT64                                 m_EP;
		INT32                                 m_nWinTimes;
		INT32                                 m_nLossTimes;
		INT32                                 m_nGameTime;

		INT64                                 m_nUpperLimite;
		INT64                                 m_nLowerLimite;

		int                                   m_Right;

		memUserGameInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<(bostream& bos, const memUserGameInfo& src)
		{
			bos << src.m_nGameMoney;
			bos << src.m_nMoguiMoney;
			bos << src.m_nMatchJF;
			bos << src.m_JF;
			bos << src.m_EP;
			bos << src.m_nWinTimes;
			bos << src.m_nLossTimes;
			bos << src.m_nGameTime;
			bos << src.m_nUpperLimite;
			bos << src.m_nLowerLimite;
			bos << src.m_Right;

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserGameInfo& src)
		{		
			bis >> src.m_nGameMoney;
			bis >> src.m_nMoguiMoney;
			bis >> src.m_nMatchJF;
			bis >> src.m_JF;
			bis >> src.m_EP;
			bis >> src.m_nWinTimes;
			bis >> src.m_nLossTimes;
			bis >> src.m_nGameTime;
			bis >> src.m_nUpperLimite;
			bis >> src.m_nLowerLimite;
			bis >> src.m_Right;

			return bis;
		}
	};

	struct memUserGameInfoEx
	{
		short                m_AID;
		unsigned int         m_PID;

		INT64                m_MaxPai;
		UINT32               m_MaxPaiTime;
		INT64                m_MaxMoney;
		UINT32               m_MaxMoneyTime;
		INT64                m_MaxWin;
		UINT32               m_MaxWinTime;

		int                  m_Achieve[4];

		memUserGameInfoEx(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<(bostream& bos, const memUserGameInfoEx& src)
		{
			bos << src.m_MaxPai;
			bos << src.m_MaxPaiTime;
			bos << src.m_MaxMoney;
			bos << src.m_MaxMoneyTime;
			bos << src.m_MaxWin;
			bos << src.m_MaxWinTime;

			for ( int i=0;i<4;i++)
			{
				bos << src.m_Achieve[i];
			}

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserGameInfoEx& src)
		{		
			bis >> src.m_MaxPai;
			bis >> src.m_MaxPaiTime;
			bis >> src.m_MaxMoney;
			bis >> src.m_MaxMoneyTime;
			bis >> src.m_MaxWin;
			bis >> src.m_MaxWinTime;

			for ( int i=0;i<4;i++)
			{
				bis >> src.m_Achieve[i];
			}
			
			return bis;
		}
	};

	struct memUserGameWinRecord
	{
		enum { WinTypeNum = 10 };

		short                m_AID;
		unsigned int         m_PID;

		int                  m_WinRecord[WinTypeNum];

		memUserGameWinRecord(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<(bostream& bos, const memUserGameWinRecord& src)
		{
			for (int i=0;i<WinTypeNum;i++)
			{
				bos << src.m_WinRecord[i];
			}
			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserGameWinRecord& src)
		{		
			for (int i=0;i<WinTypeNum;i++)
			{
				bis >> src.m_WinRecord[i];
			}
			return bis;
		}
	};

	struct memGiftInfo
	{
		int                  m_GiftIdx;
		UINT32               m_RecvPID;
		INT16                m_GiftID;
		INT32                m_Price;
		UINT32               m_SendPID;
		std::string          m_NickName;

		UINT32               m_ActionTime;
		UINT32               m_CurEndTime;
		UINT32               m_LastEndTime;

		memGiftInfo(){Init();}
		void Init()
		{
			m_GiftIdx    = 0;
			m_RecvPID    = 0;
			m_GiftID     = 0;
			m_Price      = 0;
			m_SendPID    = 0;
			m_NickName   = "";

			m_ActionTime = 0;
			m_CurEndTime = 0;
			m_LastEndTime= 0;
		}

		friend bostream& operator<<(bostream& bos, const memGiftInfo& src)
		{
			bos << src.m_GiftIdx;
			bos << src.m_RecvPID;
			bos << src.m_GiftID;
			bos << src.m_Price;
			bos << src.m_SendPID;
			InString(bos,src.m_NickName,50);
			bos << src.m_ActionTime;
			bos << src.m_CurEndTime;
			bos << src.m_LastEndTime;

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memGiftInfo& src)
		{
			bis >> src.m_GiftIdx;
			bis >> src.m_RecvPID;
			bis >> src.m_GiftID;
			bis >> src.m_Price;
			bis >> src.m_SendPID;
			OutString(bis,src.m_NickName,50);
			bis >> src.m_ActionTime;
			bis >> src.m_CurEndTime;
			bis >> src.m_LastEndTime;

			return bis;
		}
	};	
}
}
}