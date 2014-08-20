#pragma once

#include "MoGui.h"
#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "clientsocket.h"
#include "socketcallback.h"

class CClientSocket;

//#define TestInfo

class CTestPlayer : public CSocketCallBack
{
public:
	CTestPlayer(int aid,int pid);
	~CTestPlayer(void);

	int                  m_AID;
	int                  m_PID;

	int                  m_OriRoomID;
	int                  m_OriTableID;
	int                  m_OriSitID;

	int                  m_RoomID;
	int                  m_TableID;
	int                  m_SitID;
	int                  m_PlayerState;

	int                  m_GameState;
	int                  m_CurPlayerSitID;

	int                  m_nBigBlind;

	INT64                m_nGameMoney;
	INT64                m_nTableMoney;
	string               m_NickName;

	unsigned int         m_curTime;
	UINT32               m_LastTime;

	CClientPool          m_clientpool;
	CClientSocket        m_clientsocket;

	void                           DebugError(const char* str,...);
	void                           DebugInfo(const char* str,...);

	int  StartMsg();
	int  OnTimer(UINT32);
	int  OnMsg(CMoGuiClientSocket* pSocket,CRecvMsgPacket& msgPack);
	int  OnRespKey(CRecvMsgPacket& msgPack);
	int  OnRespHeart(CRecvMsgPacket& msgPack);
	int  OnGameRule(CRecvMsgPacket& msgPack);
	int  OnBatchProtocl(CRecvMsgPacket& msgPack);
	int  OnRespGameConnect(CRecvMsgPacket& msgPack);
	int  OnPlayerData(CRecvMsgPacket& msgPack);
	int  OnPlayerDataEX(CRecvMsgPacket& msgPack);
	int  OnUserAwardInfoList(CRecvMsgPacket& msgPack);
	int  OnRoomInfoList(CRecvMsgPacket& msgPack);
	int  OnRespRoomTableInfo(CRecvMsgPacket& msgPack);
	int  OnTableInfo(CRecvMsgPacket& msgPack);
	int  OnTableInfoList(CRecvMsgPacket& msgPack);
	int  OnRespJoinRoom(CRecvMsgPacket& msgPack);
	int  OnJoinTable(CRecvMsgPacket& msgPack);
	int  OnPlayerAct(CRecvMsgPacket& msgPack);
	int  OnLeaveTable(CRecvMsgPacket& msgPack);
	int  OnLeaveRoom(CRecvMsgPacket& msgPack);
	int  OnPlayerBaseInfo(CRecvMsgPacket& msgPack);
	int  OnPlayerGameInfo(CRecvMsgPacket& msgPack);
	int  OnTableShowList(CRecvMsgPacket& msgPack);
	int  OnTableShow(CRecvMsgPacket& msgPack);
	int  OnPlayerStateInfo(CRecvMsgPacket& msgPack);
	int  OnRespPlayerAct(CRecvMsgPacket& msgPack);
	int  OnTablePlayerCountList(CRecvMsgPacket& msgPack);
	int  OnPlayerGameMoney(CRecvMsgPacket& msgPack);

	int  OnGameMsg(CRecvMsgPacket& msgPack);
	int  OnGameRelink(GameXY::ClientToServerMessage& CTS);
	int  OnGameSitPlayerInfo(GameXY::ClientToServerMessage& CTS);
	int  OnGameUpGameMoney(GameXY::ClientToServerMessage& CTS);
	int  OnGameTableMoney(GameXY::ClientToServerMessage& CTS);
	int  OnAddChipInfo(GameXY::ClientToServerMessage& CTS);
	int  OnPlayerTurn(GameXY::ClientToServerMessage& CTS);
	int  OnPlayerPower(GameXY::ClientToServerMessage& CTS);
	int  OnGameStart(GameXY::ClientToServerMessage& CTS);
	int  OnStartInfo(GameXY::ClientToServerMessage& CTS);
	int  OnGameState(GameXY::ClientToServerMessage& CTS);
	int  OnGameFlag(GameXY::ClientToServerMessage& CTS);
	int  OnReqAddMoney(GameXY::ClientToServerMessage& CTS);	

	template<class Txieyi>
		int ExplainGameCTSMsg(GameXY::ClientToServerMessage& CTS,Txieyi& xieyi)//用于解释有外层包装的游戏协议
	{
		int ret = 0;

		bistream bis;
		bis.attach(CTS.m_Message,CTS.m_MsgLen);

		try
		{
			bis >> xieyi;
		}
		catch ( agproexception* e )
		{
			ret = 10000+e->m_cause;
		}
		catch ( biosexception* e )
		{
			ret = 20000+e->m_cause;
		}
		catch(...)
		{
			ret = 30000;
		}

		if ( ret )
		{
			cout<<"ExplainGameCTSMsg XYID="<<CTS.m_MsgID<<"XYLen="<<CTS.m_MsgLen<<endl;
		}

		return ret;
	}

#define TransplainMsgCTS(CTS,xieyi)        \
	if ( ExplainGameCTSMsg(CTS,xieyi) )    \
	{	                                   \
	return SOCKET_MSG_ERROR_STREAM;        \
	}
};
