#pragma once

#include "MoGui.h"
#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "ClientSocket.h"

class CClientSocket;

class CClientPool : public IConnectPoolCallback
{
public:
	CClientPool(void);
	~CClientPool(void);

	typedef list<CClientSocket*>   ListSocket;

	bool OnPriorityEvent( void );
	void OnTimer( void );
	void OnAccept( IConnect* connect );
	void OnClose( IConnect* nocallbackconnect, bool bactive );
	void DealCloseSocket( IConnect* connect );

	UINT32                     GetCurTime()const{return m_CurTime;}
	IConnect*                  Connect( const char* ip, int port );
	IConnectPool*              GetConnectPool(){ return m_pPool; }

	void                       AddSocket(CClientSocket* pSocket);
	void                       DeleteSocket(CClientSocket* pSocket);

private:
	IConnectPool*			   m_pPool;
	UINT32                     m_CurTime;

	ListSocket                 m_listSocket;

private:
	CClientPool(const CClientPool &);
	CClientPool &operator=(const CClientPool &);
};
