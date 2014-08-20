#pragma once

#include "MoGui.h"
#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "MoGuiClientSocket.h"
#include "socketcallback.h"
#include "ClientPool.h"

class CClientPool;

class CClientSocket	: public CMoGuiClientSocket
{
public:
	CClientSocket(void);
	virtual ~CClientSocket(void);

	void OnConnect( void );
	void OnClose( bool bactive );
	int  OnMsg( const char* buf, int len );

	void Close();
	bool Connect(string IP,int nPort);

	void OnTimer( UINT32 curTime );

	void  SetClientPool(CClientPool* pPool);
	void  SetCallBack(CSocketCallBack* pCallBack);

private:
	CClientPool*        m_pClientPool;
	CSocketCallBack*    m_pCallBack;

	int                 m_nPort;
	string              m_IP;

public:
	template<class Txieyi>
		int SendGameLogicMsg(Txieyi& xieyi)
	{
		assert( xieyi.XY_ID < GameXYID_End );

		GameXY::ClientToServerMessage CTS;
		CTS.m_MsgID = xieyi.XY_ID;

		bostream	bos;
		bos.attach(CTS.m_Message,MAXGAMEMSGLEN);
		bos<<xieyi;
		CTS.m_MsgLen = short(bos.length());		

		SendMsg(CTS);

		return 0;
	}

private:
	CClientSocket(const CClientSocket &l);
	CClientSocket &operator=(const CClientSocket &l);
};
