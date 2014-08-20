#include ".\moguiclientsocket.h"

MapXieYiIO CMoGuiClientSocket::s_InXieYi;
MapXieYiIO CMoGuiClientSocket::s_OutXieYi;

INT64 CMoGuiClientSocket::s_TotalInPacket = 0;
INT64 CMoGuiClientSocket::s_TotalOutPacket = 0;
INT64 CMoGuiClientSocket::s_TotalInByte = 0;
INT64 CMoGuiClientSocket::s_TotalOutByte = 0;

INT64 CMoGuiClientSocket::s_LastTotalInByte = 0;
INT64 CMoGuiClientSocket::s_LastTotalOutByte = 0;
INT64 CMoGuiClientSocket::s_LastTotalInPacket = 0;
INT64 CMoGuiClientSocket::s_LastTotalOutPacket = 0;

CMoGuiClientSocket::CMoGuiClientSocket( IConnectPool* pPool )
{
	assert(pPool);
	m_pPool = pPool;
	m_pConnect = NULL;

	m_ClientSocketStep = 0;
	m_ClientSocketState = SOCKET_ST_NONE;

	m_nRecvPackets = 0;
	m_nSendPackets = 0;
	m_nRecvSize = 0;
	m_nSendSize = 0;

	m_bCrypt = false;
	m_Crypt.reset();
}

CMoGuiClientSocket::~CMoGuiClientSocket(void)
{
	m_pPool = NULL;
	m_pConnect = NULL;
}

unsigned int CMoGuiClientSocket::GetPeerLongIp( void )
{
	if (m_pConnect)
	{
		return m_pConnect->GetPeerLongIp();
	}
	return 0;
}
string CMoGuiClientSocket::GetPeerStringIp( void )
{
	if (m_pConnect)
	{
		return m_pConnect->GetPeerStringIp();
	}
	return "";
}

void CMoGuiClientSocket::SetCrypt(bool bCrypt)
{
	m_bCrypt = bCrypt;
}
void CMoGuiClientSocket::SetKey(unsigned char* pKey,int keySize)
{
	m_Crypt.setAesKey(pKey,short(keySize));
}

int CMoGuiClientSocket::FinishMsg(UINT16 XYID,int TotalXYLen)
{
	m_nRecvPackets++;
	m_nRecvSize += TotalXYLen;

	s_TotalInPacket++;
	MapXieYiIO::iterator itorXieYi = s_InXieYi.find(XYID);
	if ( itorXieYi == s_InXieYi.end() )
	{
		s_InXieYi.insert(make_pair(XYID,INT64(0)));
		itorXieYi = s_InXieYi.find(XYID);
	}
	itorXieYi->second += TotalXYLen;
	s_TotalInByte += TotalXYLen;

	return 0;
}

bool CMoGuiClientSocket::Connect( const char* ip, int port )
{
	CSelfLock	l( m_lk );

	m_ClientSocketState = SOCKET_ST_NONE;
	IConnect* connect = m_pPool->Connect( ip, port );

	if ( !connect )	return false;

	m_pConnect = connect;
	m_ClientSocketStep  = 1;
	m_pConnect->SetCallback( this );

	m_cd.Wait( m_lk );

	return ( m_pConnect != NULL );
}
void CMoGuiClientSocket::Close( void )
{
	CSelfLock	l( m_lk );
	if( m_pConnect ) m_pConnect->Close( );
}

bool CMoGuiClientSocket::Send( const char* buf, int len )
{
	CSelfLock	l( m_lk );

	if( m_pConnect )
	{
		return m_pConnect->Send( buf, len );
	}

	return false;
}

void CMoGuiClientSocket::OnConnect( void )
{
	CSelfLock	l( m_lk );

	m_ClientSocketState = SOCKET_ST_CONNECTED;
	if ( m_ClientSocketStep==1 )
	{
		m_ClientSocketStep = 2;
		m_cd.Signal( );
	}
}
void CMoGuiClientSocket::OnClose( bool bactive )
{
	CSelfLock	l( m_lk );

	m_pConnect = NULL;
	m_ClientSocketState = SOCKET_ST_NONE;	
	if ( m_ClientSocketStep==1 )
	{
		m_cd.Signal( );
	}
}
int CMoGuiClientSocket::OnMsg( const char* buf, int len )
{
	return len;
}