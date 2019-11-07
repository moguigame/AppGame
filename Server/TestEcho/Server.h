#pragma once

#include <map>
#include "base.h"
#include "SocketInterFace.h"

#include "boost/utility.hpp"

class CServerSocket;
class CServer : public AGBase::IConnectPoolCallback, public boost::noncopyable
{
public:
	CServer(void);
	virtual ~CServer(void);

	typedef std::map<AGBase::IConnect*, CServerSocket*>  	MapClientSocket;

private:
	bool OnPriorityEvent( void );
	void OnTimer( void );
	void OnAccept(AGBase::IConnect* connect);
	void OnClose(AGBase::IConnect* nocallbackconnect, bool bactive);

public:
	inline void        DebugError(const char* logstr,...);
	inline void        DebugInfo(const char* logstr,...);

private:
	AGBase::IConnectPool*			     m_pPool;
	MapClientSocket          	         m_Clients;

	bool                                 m_bIsInitOK;
	time_t 		                         m_CurTime;
	time_t                               m_CheckActiveTime;
};

