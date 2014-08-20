#pragma once

#include <vector>
#include <string>

#include "MoGuiGame.h"
#include "gamedezhou.h"

struct stCfServer
{
	int                m_GameID;
	int                m_ServerID;

	int                m_gsPort;
	int                m_gsConnectCount;
	int                m_gsOutCount;

	int                m_dbPort;
	string             m_dbIp;

	int                m_MemCount;
	vector<string>     m_MemcachIpPort;

	int                m_IsCrypt;           //连接协议是否加密
	int                m_CeshiMode;

	int                m_GiftBase;
	int                m_FaceBase;
	int                m_AddMoney;          //增加钱的倍数

	int                m_BotAutoPlayer;     //没有玩家的时候机器人是否要自动打

	int                m_StartPID;
	int                m_EndPID;
	
	size_t             m_MaxShowRoomPlayer;  //房间显示的玩家数	
	size_t             m_MaxShowCityPlayer;  //显示同城玩家数

	stCfServer(){ ReSet();}
	void ReSet()
	{
		m_GameID = 0;
		m_ServerID = 0;

		m_gsPort = 0;
		m_gsConnectCount = 0;
		m_gsOutCount = 0;

		m_dbPort = 0;
		m_dbIp = "";

		m_MemCount = 0;
		m_MemcachIpPort.clear();

		m_IsCrypt = 1;
		m_CeshiMode = 0;

		m_GiftBase = 0;
		m_FaceBase = 0;
		m_AddMoney = 0;

		m_BotAutoPlayer = 0;

		m_StartPID = 0;
		m_EndPID = 0;

		m_MaxShowRoomPlayer = 30;
		m_MaxShowCityPlayer = 30;
	}
};

struct stCfBotPlayer
{
	int               m_MinPlayTime;
	int               m_MaxPlayTime;

	int               m_NoPlayerChipTime;  //指没有玩家时机器人出牌时间

	stCfBotPlayer(){ ReSet();}
	void ReSet(){ memset(this,0,sizeof(*this));}
};

struct stCfGift
{
	int                m_SendGiftOn;

	stCfGift(){ ReSet();}
	void ReSet(){ m_SendGiftOn = 1; }
};

struct stCfSend     //用于配置玩家
{
	int                m_SendON;                //送钱的开关
	int                m_minSend;
	int                m_maxSend;
	int                m_OneTimeOneGame;
	int                m_MaxSendOneGame;
	int                m_MaxRecvOneGame;
	int                m_MaxSendOneDay;
	int                m_MaxRecvOneDay;

	stCfSend(){ ReSet();}
	void ReSet(){ memset(this,0,sizeof(*this));}
};

struct stCfPrivateTable   //自建房间的配置
{	
	int                m_OneHourMul;
	int                m_minPay;
	int                m_maxPay;
	int                m_minLastTime;
	int                m_maxLastTime;
	int                m_minAddTime;
	int                m_maxAddTime;
	int                m_VisitorCreateTable;

	stCfPrivateTable(){ ReSet();}
	void ReSet(){ memset(this,0,sizeof(*this));}
};

struct stCfMatch        //比赛的设置  //单桌比赛
{	
	int                m_nOriginTable;
	int                m_nChampionTable;

	int                m_nMaxBotInRoom;
	int                m_minTimeAddBotPlayer;
	int                m_maxTimeAddBotPlayer;

	stCfMatch(){ ReSet();}
	void ReSet()
	{
		m_nOriginTable = 100;
		m_nChampionTable = 200;

		m_nMaxBotInRoom = 50;
		m_minTimeAddBotPlayer = 0;
		m_maxTimeAddBotPlayer = 0;
	}
};

struct stCfTimePromotion    //时间活动的配置
{	
	int                m_TimePromoteON;
	int                m_TotalStepTimes;
	vector<int>        m_StepTime;
	vector<int>        m_StepMoney;

	stCfTimePromotion(){ ReSet();}
	void ReSet()
	{
		m_TimePromoteON = 0;
		m_TotalStepTimes = 0;
		m_StepTime.clear();
		m_StepMoney.clear();
	}
};

struct stCfRoom         //房间的配置
{
	int                m_RoomInterval;
	vector<int>        m_vecPN;
	vector<int>        m_vecSendTime;

	stCfRoom(){ ReSet();}
	void ReSet()
	{
		m_RoomInterval = 0;
		m_vecPN.clear();
		m_vecSendTime.clear();
	}
};

struct stCfTime
{
	int                 m_KeepPlayerData;

	stCfTime(){ ReSet();}
	void ReSet()
	{
		m_KeepPlayerData = 600;
	}
};


class CGameServerConfig
{
public:
	CGameServerConfig()
	{
		m_cfServer.ReSet();
		m_cfGift.ReSet();
		m_cfSend.ReSet();
		m_cfPT.ReSet();
		m_cfMatch.ReSet();
		m_cfTP.ReSet();
		m_cfRoom.ReSet();
		m_cfBP.ReSet();
		m_cfTime.ReSet();
	}

	~CGameServerConfig(){}

	void DebugError(const string& strLog)
	{
		cout<<strLog<<" Error----------------------------------------"<<endl;
	}

	void Init()
	{
		int TempInt;
		string strPath,strTemp;
		char CurPath[256],TempBuf[256];

		::GetCurrentDirectory(255,CurPath);
		strPath = string(CurPath)+"\\"+"gs_dz.ini";
		
		m_cfServer.m_GameID = ::GetPrivateProfileInt("serverconfig","gameid",0,strPath.c_str());
		if ( m_cfServer.m_GameID <= 0 ) DebugError("m_cfServer.m_GameID");
		cout<<"m_cfServer.GameID="<<m_cfServer.m_GameID<<endl;

		m_cfServer.m_ServerID = ::GetPrivateProfileInt("serverconfig","serverid",0,strPath.c_str());
		if(m_cfServer.m_ServerID <= 0) DebugError("m_cfServer.m_ServerID");
		cout<<"m_cfServer.m_ServerID="<<m_cfServer.m_ServerID<<endl;

		m_cfServer.m_gsPort = ::GetPrivateProfileInt("serverconfig","gsport",0,strPath.c_str());
		if(m_cfServer.m_gsPort <= 4000) DebugError("m_cfServer.m_gsPort");
		cout<<"m_cfServer.m_gsPort="<<m_cfServer.m_gsPort<<endl;

		m_cfServer.m_gsConnectCount = ::GetPrivateProfileInt("serverconfig","gsconnectcount",0,strPath.c_str());
		if(m_cfServer.m_gsConnectCount <= 1000) DebugError("m_cfServer.m_gsConnectCount");
		cout<<"m_cfServer.m_gsConnectCount="<<m_cfServer.m_gsConnectCount<<endl;

		m_cfServer.m_gsOutCount = ::GetPrivateProfileInt("serverconfig","gsoutcount",0,strPath.c_str());
		if(m_cfServer.m_gsOutCount <= 0) DebugError("m_cfServer.m_gsOutCount");
		cout<<"m_cfServer.m_gsOutCount="<<m_cfServer.m_gsOutCount<<endl;

		m_cfServer.m_dbPort = ::GetPrivateProfileInt("serverconfig","dbport",0,strPath.c_str());
		if(m_cfServer.m_dbPort <= 4000) DebugError("m_cfServer.m_dbPort");
		cout<<"m_cfServer.m_dbPort="<<m_cfServer.m_dbPort<<endl;

		::GetPrivateProfileString("serverconfig","dbip","",TempBuf,255,strPath.c_str());
		m_cfServer.m_dbIp = string(TempBuf);
		if (m_cfServer.m_dbIp.empty()) DebugError("m_cfServer.m_dbIp");
		cout<<"m_cfServer.m_dbIp="<<m_cfServer.m_dbIp<<endl;

		m_cfServer.m_MemCount = ::GetPrivateProfileInt("serverconfig","cachcount",0,strPath.c_str());
		if(m_cfServer.m_MemCount <= 0) DebugError("m_cfServer.m_MemCount");
		cout<<"m_cfServer.m_MemCount="<<m_cfServer.m_MemCount<<endl;

		if( m_cfServer.m_MemCount > 0 )
		{
			string strTemp,strKey;
			for (int nLoop=0;nLoop<m_cfServer.m_MemCount;nLoop++)
			{
				strKey = "cach" + Tool::N2S(nLoop+1);
				::GetPrivateProfileString("serverconfig",strKey.c_str(),"",TempBuf,255,strPath.c_str());
				strTemp = string(TempBuf);
				if (strTemp.empty()) DebugError("cach");
				cout<<"strTemp="<<strTemp<<endl;
				m_cfServer.m_MemcachIpPort.push_back(strTemp);
			}
		}
		if ( m_cfServer.m_MemcachIpPort.size() != m_cfServer.m_MemCount )
		{
			DebugError("cach m_MemCount Size");
		}		

		m_cfServer.m_IsCrypt = ::GetPrivateProfileInt("serverconfig","crypt",0,strPath.c_str());
		if(m_cfServer.m_IsCrypt<0||m_cfServer.m_IsCrypt>1) DebugError("m_cfServer.m_IsCrypt");
		cout<<"m_cfServer.m_IsCrypt="<<m_cfServer.m_IsCrypt<<endl;

		m_cfServer.m_CeshiMode = ::GetPrivateProfileInt("serverconfig","ceshimode",0,strPath.c_str());
		cout<<"m_cfServer.m_CeshiMode="<<m_cfServer.m_CeshiMode<<endl;

		m_cfServer.m_StartPID = ::GetPrivateProfileInt("serverconfig","startpid",-1,strPath.c_str());
		if(m_cfServer.m_StartPID <= 1000 ) DebugError("m_cfServer.m_StartPID");
		cout<<"m_cfServer.m_StartPID="<<m_cfServer.m_StartPID<<endl;

		m_cfServer.m_EndPID = ::GetPrivateProfileInt("serverconfig","endpid",-1,strPath.c_str());
		if(m_cfServer.m_EndPID <= m_cfServer.m_StartPID ) DebugError("m_cfServer.m_EndPID");
		cout<<"m_cfServer.m_EndPID="<<m_cfServer.m_EndPID<<endl;

		m_cfServer.m_BotAutoPlayer = ::GetPrivateProfileInt("serverconfig","botauto",0,strPath.c_str());
		if(m_cfServer.m_BotAutoPlayer<0 || m_cfServer.m_BotAutoPlayer>1) DebugError("m_cfServer.m_BotAutoPlayer");
		cout<<"m_cfServer.m_BotAutoPlayer="<<m_cfServer.m_BotAutoPlayer<<endl;

		m_cfServer.m_GiftBase = ::GetPrivateProfileInt("serverconfig","giftbase",0,strPath.c_str());
		if(m_cfServer.m_GiftBase < 100) DebugError("m_cfServer.m_GiftBase");
		cout<<"m_cfServer.m_GiftBase="<<m_cfServer.m_GiftBase<<endl;

		m_cfServer.m_FaceBase = ::GetPrivateProfileInt("serverconfig","facebase",0,strPath.c_str());
		if(m_cfServer.m_FaceBase < 100) DebugError("m_cfServer.m_FaceBase");
		cout<<"m_cfServer.m_FaceBase="<<m_cfServer.m_FaceBase<<endl;

		m_cfServer.m_AddMoney = ::GetPrivateProfileInt("serverconfig","addmoney",0,strPath.c_str());
		if(m_cfServer.m_AddMoney < 20) DebugError("m_cfServer.m_AddMoney");
		cout<<"m_cfServer.m_AddMoney="<<m_cfServer.m_AddMoney<<endl;

		m_cfServer.m_MaxShowRoomPlayer = size_t(::GetPrivateProfileInt("serverconfig","roommax",30,strPath.c_str()));
		if( m_cfServer.m_MaxShowRoomPlayer<10 || m_cfServer.m_MaxShowRoomPlayer>50 ) DebugError("m_cfServer.m_MaxShowRoomPlayer");
		cout<<"m_cfServer.m_MaxShowRoomPlayer="<<m_cfServer.m_MaxShowRoomPlayer<<endl;

		m_cfServer.m_MaxShowCityPlayer = size_t(::GetPrivateProfileInt("serverconfig","citymax",30,strPath.c_str()));
		if( m_cfServer.m_MaxShowCityPlayer<10 || m_cfServer.m_MaxShowCityPlayer>50 ) DebugError("m_cfServer.m_MaxShowCityPlayer");
		cout<<"m_cfServer.m_MaxShowCityPlayer="<<m_cfServer.m_MaxShowCityPlayer<<endl;

		TempInt = ::GetPrivateProfileInt("sendgift","on",-1,strPath.c_str());
		if(TempInt == -1) DebugError("m_cfGift.m_SendGiftON");
		else m_cfGift.m_SendGiftOn = TempInt;
		cout<<"m_cfGift.m_SendGiftOn="<<m_cfGift.m_SendGiftOn<<endl;

		m_cfSend.m_SendON = ::GetPrivateProfileInt("sendrecv","on",-1,strPath.c_str());
		if(m_cfSend.m_SendON == -1) DebugError("m_cfSend.m_SendON");
		cout<<"m_cfSend.m_SendON="<<m_cfSend.m_SendON<<endl;

		m_cfSend.m_minSend = ::GetPrivateProfileInt("sendrecv","min",-1,strPath.c_str());
		if(m_cfSend.m_minSend == -1)  DebugError("m_cfSend.m_minSend");
		cout<<"m_cfSend.m_minSend="<<m_cfSend.m_minSend<<endl;

		m_cfSend.m_maxSend = ::GetPrivateProfileInt("sendrecv","max",-1,strPath.c_str());
		if(m_cfSend.m_maxSend == -1) DebugError("m_cfSend.m_maxSend");
		cout<<"m_cfSend.m_maxSend="<<m_cfSend.m_maxSend<<endl;

		m_cfSend.m_OneTimeOneGame = ::GetPrivateProfileInt("sendrecv","onetime",-1,strPath.c_str());
		if(m_cfSend.m_OneTimeOneGame == -1) DebugError("m_cfSend.m_OneTimeOneGame");
		cout<<"m_cfSend.m_OneTimeOneGame="<<m_cfSend.m_OneTimeOneGame<<endl;

		m_cfSend.m_MaxRecvOneGame = 2000;
		m_cfSend.m_MaxSendOneGame = 2000;

		m_cfSend.m_MaxSendOneDay = ::GetPrivateProfileInt("sendrecv","maxsend",-1,strPath.c_str());
		if(m_cfSend.m_MaxSendOneDay == -1) DebugError("m_cfSend.m_MaxSendOneDay");
		cout<<"m_cfSend.m_MaxSendOneDay="<<m_cfSend.m_MaxSendOneDay<<endl;

		m_cfSend.m_MaxRecvOneDay = ::GetPrivateProfileInt("sendrecv","maxrecv",-1,strPath.c_str());
		if(m_cfSend.m_MaxRecvOneDay == -1) DebugError("m_cfSend.m_MaxRecvOneDay");
		cout<<"m_cfSend.m_MaxRecvOneDay="<<m_cfSend.m_MaxRecvOneDay<<endl;

		m_cfPT.m_OneHourMul = ::GetPrivateProfileInt("private","hour",-1,strPath.c_str());
		if(m_cfPT.m_OneHourMul == -1) DebugError("m_cfPT.m_OneHourMul");
		cout<<"m_cfPT.m_OneHourMul="<<m_cfPT.m_OneHourMul<<endl;

		m_cfPT.m_minPay = ::GetPrivateProfileInt("private","min",-1,strPath.c_str());
		if(m_cfPT.m_minPay == -1) DebugError("m_cfPT.m_minPay");
		cout<<"m_cfPT.m_minPay="<<m_cfPT.m_minPay<<endl;

		m_cfPT.m_maxPay = ::GetPrivateProfileInt("private","max",-1,strPath.c_str());
		if(m_cfPT.m_maxPay == -1) DebugError("m_cfPT.m_maxPay");
		cout<<"m_cfPT.m_maxPay="<<m_cfPT.m_maxPay<<endl;

		m_cfPT.m_minLastTime = ::GetPrivateProfileInt("private","mintime",0,strPath.c_str());
		if(m_cfPT.m_minLastTime == 0) DebugError("m_cfPT.m_minLastTime");
		cout<<"m_cfPT.m_minLastTime="<<m_cfPT.m_minLastTime<<endl;

		m_cfPT.m_maxLastTime = ::GetPrivateProfileInt("private","maxtime",0,strPath.c_str());
		if(m_cfPT.m_maxLastTime == 0) DebugError("m_cfPT.m_maxLastTime");
		cout<<"m_cfPT.m_maxLastTime="<<m_cfPT.m_maxLastTime<<endl;

		m_cfPT.m_minAddTime = ::GetPrivateProfileInt("private","minaddtime",0,strPath.c_str());
		if(m_cfPT.m_minAddTime == 0) DebugError("m_cfPT.m_minAddTime");
		cout<<"m_cfPT.m_minAddTime="<<m_cfPT.m_minAddTime<<endl;

		m_cfPT.m_maxAddTime = ::GetPrivateProfileInt("private","maxaddtime",0,strPath.c_str());
		if(m_cfPT.m_maxAddTime == 0) DebugError("m_cfPT.m_maxAddTime");
		cout<<"m_cfPT.m_maxAddTime="<<m_cfPT.m_maxAddTime<<endl;

		m_cfPT.m_VisitorCreateTable = ::GetPrivateProfileInt("private","visitor",0,strPath.c_str());
		if(m_cfPT.m_VisitorCreateTable < 0) DebugError("m_cfPT.m_VisitorCreateTable");
		cout<<"m_cfPT.m_VisitorCreateTable="<<m_cfPT.m_VisitorCreateTable<<endl;

		m_cfTP.m_TimePromoteON = ::GetPrivateProfileInt("TimePromote","on",-1,strPath.c_str());
		if(m_cfTP.m_TimePromoteON == -1) DebugError("m_cfTP.m_TimePromoteON");
		cout<<"m_cfTP.m_TimePromoteON="<<m_cfTP.m_TimePromoteON<<endl;

		m_cfTP.m_TotalStepTimes = ::GetPrivateProfileInt("TimePromote","step",0,strPath.c_str());
		if(m_cfTP.m_TotalStepTimes == 0) DebugError("m_cfTP.m_TotalStepTimes");
		cout<<"m_cfTP.m_TotalStepTimes="<<m_cfTP.m_TotalStepTimes<<endl;

		::GetPrivateProfileString("TimePromote","money","",TempBuf,255,strPath.c_str());
		strTemp = string(TempBuf);
		if (strTemp.empty()) DebugError("TimePromote money");
		cout<<"money="<<strTemp<<endl;
		std::vector<string> VecStrA = Tool::SplitString(strTemp,",");
		for (int Idx=0;Idx<int(VecStrA.size());Idx++)
		{
			int nMoney = 0;
			Tool::S2N(VecStrA.at(Idx),nMoney);
			m_cfTP.m_StepMoney.push_back(nMoney);
		}

		::GetPrivateProfileString("TimePromote","time","",TempBuf,255,strPath.c_str());
		strTemp = string(TempBuf);
		if (strTemp.empty()) DebugError("TimePromote time");
		cout<<"time="<<strTemp<<endl;
		VecStrA = Tool::SplitString(strTemp,",");
		for (int Idx=0;Idx<int(VecStrA.size());Idx++)
		{
			int nTime = 0;
			Tool::S2N(VecStrA.at(Idx),nTime);
			m_cfTP.m_StepTime.push_back(nTime);
		}

		if ( m_cfTP.m_TimePromoteON )
		{
			if ( m_cfTP.m_TotalStepTimes!=m_cfTP.m_StepMoney.size() || m_cfTP.m_TotalStepTimes!=m_cfTP.m_StepTime.size())
			{
				DebugError("time TimePromote time");
			}
			for ( int nIdx=0;nIdx<int(m_cfTP.m_StepTime.size());nIdx++)
			{
				cout<<m_cfTP.m_StepTime.at(nIdx)<<" ";
			}
			cout<<endl;

			for ( int nIdx=0;nIdx<int(m_cfTP.m_StepMoney.size());nIdx++)
			{
				cout<<m_cfTP.m_StepMoney.at(nIdx)<<" ";
			}
			cout<<endl;
		}

		::GetPrivateProfileString("room","pn","",TempBuf,255,strPath.c_str());
		strTemp = string(TempBuf);
		if (strTemp.empty()) DebugError("room pn");
		cout<<"pn="<<strTemp<<endl;
		VecStrA = Tool::SplitString(strTemp,",");
		for (int Idx=0;Idx<int(VecStrA.size());Idx++)
		{
			int nPN = 0;
			Tool::S2N(VecStrA.at(Idx),nPN);
			m_cfRoom.m_vecPN.push_back(nPN);
		}

		::GetPrivateProfileString("room","time","",TempBuf,255,strPath.c_str());
		strTemp = string(TempBuf);
		if (strTemp.empty()) DebugError("room time");
		cout<<"time="<<strTemp<<endl;
		VecStrA = Tool::SplitString(strTemp,",");
		for (int Idx=0;Idx<int(VecStrA.size());Idx++)
		{
			int nTime = 0;
			Tool::S2N(VecStrA.at(Idx),nTime);
			m_cfRoom.m_vecSendTime.push_back(nTime);
		}
		if ( m_cfRoom.m_vecPN.size() != m_cfRoom.m_vecSendTime.size() )
		{
			DebugError("room time pn");
		}

		m_cfRoom.m_RoomInterval = ::GetPrivateProfileInt("room","interval",0,strPath.c_str());
		if(m_cfRoom.m_RoomInterval < 30) DebugError("m_cfRoom.m_RoomInterval");
		cout<<"m_cfRoom.m_RoomInterval="<<m_cfRoom.m_RoomInterval<<endl;

		m_cfMatch.m_nOriginTable = ::GetPrivateProfileInt("match","origintable",50,strPath.c_str());
		if(m_cfMatch.m_nOriginTable < 50) DebugError("m_cfMatch.m_nOriginTable");
		cout<<"m_cfMatch.m_nOriginTable="<<m_cfMatch.m_nOriginTable<<endl;

		m_cfMatch.m_nChampionTable = ::GetPrivateProfileInt("match","championtable",100,strPath.c_str());
		if(m_cfMatch.m_nChampionTable < 100) DebugError("m_cfMatch.m_nChampionTable");
		cout<<"m_cfMatch.m_nChampionTable="<<m_cfMatch.m_nChampionTable<<endl;		

		m_cfMatch.m_nMaxBotInRoom = ::GetPrivateProfileInt("match","maxbotinroom",50,strPath.c_str());
		if( m_cfMatch.m_nMaxBotInRoom<0 || m_cfMatch.m_nMaxBotInRoom>1000 ) DebugError("m_cfMatch.m_nMaxBotInRoom");
		cout<<"m_cfMatch.m_nMaxBotInRoom="<<m_cfMatch.m_nMaxBotInRoom<<endl;

		m_cfMatch.m_minTimeAddBotPlayer = ::GetPrivateProfileInt("match","mintimeaddbot",20,strPath.c_str());
		if( m_cfMatch.m_minTimeAddBotPlayer<0 || m_cfMatch.m_nMaxBotInRoom>1000 ) DebugError("m_cfMatch.m_minTimeAddBotPlayer");
		cout<<"m_cfMatch.m_minTimeAddBotPlayer="<<m_cfMatch.m_minTimeAddBotPlayer<<endl;

		m_cfMatch.m_maxTimeAddBotPlayer = ::GetPrivateProfileInt("match","maxtimeaddbot",300,strPath.c_str());
		if( m_cfMatch.m_maxTimeAddBotPlayer<m_cfMatch.m_minTimeAddBotPlayer )DebugError("m_cfMatch.m_maxTimeAddBotPlayer");
		cout<<"m_cfMatch.m_maxTimeAddBotPlayer="<<m_cfMatch.m_maxTimeAddBotPlayer<<endl;

		m_cfBP.m_MinPlayTime = ::GetPrivateProfileInt("bplayer","mintime",300,strPath.c_str());
		if( m_cfBP.m_MinPlayTime<=0 ) DebugError("m_cfBP.m_MinPlayTime");
		cout<<"m_cfBP.m_MinPlayTime="<<m_cfBP.m_MinPlayTime<<endl;

		m_cfBP.m_MaxPlayTime = ::GetPrivateProfileInt("bplayer","maxtime",1200,strPath.c_str());
		if( m_cfBP.m_MaxPlayTime<m_cfBP.m_MinPlayTime ) DebugError("m_cfBP.m_MaxPlayTime");
		cout<<"m_cfBP.m_MaxPlayTime="<<m_cfBP.m_MaxPlayTime<<endl;

		m_cfBP.m_NoPlayerChipTime = ::GetPrivateProfileInt("bplayer","noplayerchip",60,strPath.c_str());
		if( m_cfBP.m_NoPlayerChipTime < 2 ) DebugError("m_cfBP.m_NoPlayerChipTime");
		cout<<"m_cfBP.m_NoPlayerChipTime="<<m_cfBP.m_NoPlayerChipTime<<endl;

		cout<<endl;
		cout<<endl;
	}
	bool CeshiMode() const
	{
		return m_cfServer.m_CeshiMode != 0;
	}

public:
	stCfServer          m_cfServer;
	stCfGift            m_cfGift;
	stCfSend            m_cfSend;
	stCfPrivateTable    m_cfPT;
	stCfMatch           m_cfMatch;
	stCfTimePromotion   m_cfTP;
	stCfRoom            m_cfRoom;
	stCfBotPlayer       m_cfBP;
	stCfTime            m_cfTime;
};