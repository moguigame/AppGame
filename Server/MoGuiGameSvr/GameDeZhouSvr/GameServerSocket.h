#pragma once

#include "MoGuiServerSocket.h"
#include "xieyi.h"

#include "boost/shared_ptr.hpp"

class CServer;
class CPlayer;

class GameServerSocket : public CMoGuiServerSocket, public AGBase::CMemoryPool_Public<GameServerSocket, 100>, public boost::noncopyable
{
public:
	GameServerSocket(CServer* server, AGBase::IConnect* connect);
	virtual ~GameServerSocket(void);

	typedef AGBase::RefPtr<CPlayer>	                   PlayerPtr;

public:
	UINT32      m_LoginTime;
private:
	CServer*	m_pServer;
	PlayerPtr   m_pPlayer;

	UINT32      m_LoginPID;           //��֤ʱ�����������NID;
	std::string m_strSessionKey;      //��ǰ������֤������SESSION

	int         m_Step;               //��¼��ҷ���XY�Ĳ���
	UINT32      m_nStartTime;         //����������ʱ��
	UINT32      m_nActiveTime;        //�ϴη���������ʱ��
	INT64       m_nHeartID;           //�ϴη�����������ֵ
	int         m_TimeOutTimes;       //��ʱ����

	UINT32      m_nIP;                //��¼SOCKET��IP��ַ

	short       m_CloseFlag;          //���ڼ�¼���رյ�ԭ��
	UINT32      m_Msg10Time;          //������Ҫ�����ڿ�������Ϸ�����Ϣ
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
	std::string GetSessionKey(){ return m_strSessionKey; }
	void        SetSessionKey(const std::string& strKey){ m_strSessionKey = strKey; }
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
