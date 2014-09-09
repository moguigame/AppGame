#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "MoGuiClientSocket.h"

class CServer;

class CGameDBSocket	: public CMoGuiClientSocket, public boost::noncopyable
{
public:
	CGameDBSocket( CServer* server);
	virtual ~CGameDBSocket(void);

	void OnConnect( void );
	void OnClose( bool bactive );
	int  OnMsg( const char* buf, int len );

	void Close();
	bool StartConnect();

	void OnTimer( void );
	void SetCanUse(bool bUse) { m_bCanUse = bUse; }
	bool CanUse()const { return IsConnected() && m_bCanUse; }

	void SetAutoConnect(bool bAuto){ m_bAutoConnect = bAuto; }
	bool GetAutoConnect() const { return m_bAutoConnect; }

	int  TransMsg(CRecvMsgPacket& msgPack);

	int  OnRespHeart(CRecvMsgPacket& msgPack);
	
	void        DebugError(const char* logstr,...);
	void        DebugInfo(const char* logstr,...);

private:
	CServer*        m_pServer;
	bool            m_bCanUse;
	bool            m_bAutoConnect;

	int             m_nPort;
	std::string     m_IP;
};