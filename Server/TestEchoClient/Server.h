#pragma once

#include <map>
#include <vector>
#include "base.h"
#include "SocketInterFace.h"

#include "boost/utility.hpp"

class CClientSocket;
class CServer : public AGBase::IConnectPoolCallback, public boost::noncopyable
{
public:
	CServer(void);
	virtual ~CServer(void);	

private:
	bool OnPriorityEvent( void );
	void OnTimer( void );
	void OnAccept(AGBase::IConnect* connect);
	void OnClose(AGBase::IConnect* nocallbackconnect, bool bactive);

public:
	AGBase::IConnect* Connect(const char* strIP,int nPort,AGBase::IConnectCallback* callback);

	inline void        DebugError(const char* logstr,...);
	inline void        DebugInfo(const char* logstr,...);

private:
	AGBase::IConnectPool*			     m_pPool;
	std::vector<CClientSocket*>          m_vecConnects;;

	bool                                 m_bIsInitOK;
	time_t 		                         m_CurTime;
	time_t                               m_CheckActiveTime;
};

