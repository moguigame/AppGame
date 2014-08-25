#include "IOCPDefine.h"
#include "ConnectPool.h"

#include "Connect.h"
#include "Dispatcher.h"
#include "IOCP.h"

namespace AGBase
{

static CLock connectpool_lock;
static WSADATA wsa_data;
static unsigned long long connectpool_count=0;

IConnectPool* CreateConnectPool( void )
{
	CSelfLock l( connectpool_lock );

	if ( connectpool_count==0 )
	{
		if ( ::WSAStartup( MAKEWORD(2, 2), &wsa_data ) != 0 )
		{
			fprintf(stderr, "Error: CreateConnectPool init wsastartup failed\n");
			return 0;
		}

		if ( LOBYTE(wsa_data.wVersion)!=2 )
		{
			::WSACleanup( );
			return 0;
		}
	}

	++connectpool_count;

	return new CConnectPool();
}

void DestoryConnectPool( IConnectPool* ppool )
{
	CSelfLock l( connectpool_lock );
	if ( ppool )	delete ppool;

	if ( --connectpool_count==0 )
	{
		::WSACleanup( );
	}
}

std::string	GetAGBaseVersion( void )
{
	return "1.0.0.1";
}

CConnectPool::CConnectPool( void )
	: m_callback( 0 ), m_status( 0 )
{
	m_dispatcher= new CDispatcher( );
	m_iocp		= new CIOCP( );

	assert( m_iocp );
	assert( m_dispatcher );
}

CConnectPool::~CConnectPool( void )
{
	if ( m_iocp )
	{
		delete m_iocp;
		m_iocp = 0;
	}
	if ( m_dispatcher )
	{
		delete m_dispatcher;
		m_dispatcher = 0;
	}
	m_status = 0;
}

void CConnectPool::SetCallback( IConnectPoolCallback* callback )
{
	CSelfLock l( connectpool_lock );

	if( !callback || m_status!=0 )	return;

	m_callback	= callback;
	m_status	= 1;
}

bool CConnectPool::Start( int port, int clientcnt, int connectcnt )
{
	CSelfLock l( connectpool_lock );

	if ( m_status!=1 )												
		return false;

	if ( !m_dispatcher->Init( this ) )				
		return false;

	if ( !m_iocp->Init( m_dispatcher, clientcnt, connectcnt ) )	
		return false;

	if ( port>0 && !m_iocp->Listen( port, _DEFAULT_RECV_BUFF, _DEFAULT_SEND_BUFF ) )						
		return false;

	m_status = 2;

	return true;
}

void CConnectPool::AddForbidIP( const char* ip )
{
	m_dispatcher->AddForbidIP( ip );
}

void CConnectPool::DelForbidIP( const char* ip )
{
	m_dispatcher->DelForbidIP( ip );
}

void CConnectPool::Stop( void )
{
	CSelfLock l( connectpool_lock );

	if ( m_status==2 )
	{
		m_iocp->Fini( );
		m_dispatcher->Fini( );
		if( m_callback )	m_status = 1;
		else				m_status = 0;
	}
}

IConnect* CConnectPool::Connect(const char* ip, int port)
{
	CSelfLock l( connectpool_lock );

	if ( m_status!=2 ) return 0;

	return m_iocp->Connect( ip, port, _DEFAULT_RECV_BUFF, _DEFAULT_SEND_BUFF );
}

bool CConnectPool::OnPriorityEvent( void )
{
	return m_callback->OnPriorityEvent( );
}

void CConnectPool::OnTimer( void )
{
	m_callback->OnTimer( );
}

void CConnectPool::OnAccept( CConnect* connect )
{
	//fprintf(stderr, "CConnectPool::OnAccept, %p\n", connect);
	m_callback->OnAccept( connect );
}

void CConnectPool::OnClose( CConnect* connect, bool bactive, bool nocallback )
{
	if ( nocallback )
	{
		m_callback->OnClose(connect, bactive);
	}

	m_iocp->Close( connect, bactive );
}

void CConnectPool::OnIdle( void )
{
	m_callback->OnIdle( );
}

bool IConnectPoolCallback::OnPriorityEvent( void )
{
	return false;
}

void IConnectPoolCallback::OnTimer( void )
{

}

void IConnectPoolCallback::OnIdle( void )
{ 
	SleepMillisecond(1); 
}

}

