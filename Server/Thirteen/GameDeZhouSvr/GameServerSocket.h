#pragma once

#include "MoGuiServerSocket.h"
#include "xieyi.h"

#include "boost/shared_ptr.hpp"

using namespace AGBase;
using namespace std;

class CServer;
class CPlayer;

class GameServerSocket : public CMoGuiServerSocket, public CMemoryPool_Public<GameServerSocket, 100>, public boost::noncopyable
{
public:
	GameServerSocket( CServer* server, IConnect* connect );
	virtual ~GameServerSocket(void);

	typedef RefPtr<CPlayer>	                   PlayerPtr;

public:
	UINT32      m_LoginTime;
private:
	CServer*	m_pServer;
	PlayerPtr   m_pPlayer;

	UINT32      m_LoginPID;           //验证时玩家送上来的NID;
	string      m_strSessionKey;      //当前当次验证产生的SESSION

	int         m_Step;               //记录玩家发送XY的步聚
	UINT32      m_nStartTime;         //连接上来的时间
	UINT32      m_nActiveTime;        //上次发心跳包的时间
	INT64       m_nHeartID;           //上次发送心跳包的值
	int         m_TimeOutTimes;       //超时次数

	UINT32      m_nIP;                //记录SOCKET的IP地址

	short       m_CloseFlag;          //用于记录被关闭的原因
	UINT32      m_Msg10Time;          //这里主要是用于控制玩家上发的信息
	UINT32      m_Msg10Count;
	UINT32      m_Msg60Time;
	UINT32      m_Msg60Count;
public:
	PlayerPtr   GetPlayer() const { return m_pPlayer; }
	void        SetPlayer(PlayerPtr player );
	int         GetActiveTime() const { return m_nActiveTime; }
	void        SetActiveTime(UINT32 nTime){m_nActiveTime=nTime;}
	UINT32      GetSocketPID() const { return m_LoginPID; }
	void        SetSocketPID(UINT32 PID) { m_LoginPID = PID; }
	string      GetSessionKey(){ return m_strSessionKey;}
	void        SetSessionKey(const string& strKey){ m_strSessionKey = strKey; }
	std::string GetIPString();
	short       GetCloseFlag(){ return m_CloseFlag; };

	void		OnConnect( void );
	void		OnClose( bool bactive );
	int			OnMsg( const char* buf, int len );
	int         TransMsg( CRecvMsgPacket& msgPack );

	void        OnHeart(long long );
	int         OnRecvReqHeat( CRecvMsgPacket& msgPack );
	int         OnReqKey(CRecvMsgPacket& msgPack);	
	
	void        SendActive();
	int         GetOutTimes()const{return m_TimeOutTimes; }

	int         CheckMsgSpeed();

	void        DebugError(const char* logstr,...);
	void        DebugInfo(const char* logstr,...);
};
typedef boost::shared_ptr<GameServerSocket> PGameServerSocket;