#include ".\clientpool.h"

#include <stdio.h>
#include <stdarg.h>

CClientPool::CClientPool(void)
{
	m_pPool = CreateConnectPool();
	m_pPool->SetCallback( this );
	m_pPool->Start(0,0,1);

	m_listSocket.clear();
}

CClientPool::~CClientPool(void)
{
	m_pPool->Stop( );
	DestoryConnectPool( m_pPool );
}

bool CClientPool::OnPriorityEvent( void )
{
	return false;
}

IConnect* CClientPool::Connect( const char* ip, int port )
{
	if ( m_pPool )
	{
		return m_pPool->Connect(ip,port);
	}
	return 0;
}

void CClientPool::OnTimer( void )
{
	m_CurTime = static_cast<UINT32>(time( NULL ));

	for (ListSocket::iterator itorSocket=m_listSocket.begin();itorSocket!=m_listSocket.end();++itorSocket)
	{
		(*itorSocket)->OnTimer(m_CurTime);
	}
}

void CClientPool::OnAccept( IConnect* connect )
{
	std::cout<<"OnAccept......"<<endl;
}

void CClientPool::OnClose( IConnect* nocallbackconnect, bool bactive )
{
	std::cout<<"OnClose......"<<endl;
}

void CClientPool::DealCloseSocket( IConnect* connect )
{
	std::cout<<"DealCloseSocket......"<<endl;
}

void CClientPool::AddSocket(CClientSocket* pSocket)
{
	if ( pSocket )
	{
		ListSocket::iterator itorSocket = find(m_listSocket.begin(),m_listSocket.end(),pSocket);
		if (itorSocket == m_listSocket.end())
		{
			m_listSocket.push_back(pSocket);
		}
	}	
}
void CClientPool::DeleteSocket(CClientSocket* pSocket)
{
	for(;;)
	{
		ListSocket::iterator itorSocket = find(m_listSocket.begin(),m_listSocket.end(),pSocket);
		if (itorSocket!=m_listSocket.end())
		{
			m_listSocket.erase(itorSocket);
		}
		else
		{
			break;
		}
	}
}