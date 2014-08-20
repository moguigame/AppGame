#include "clientsocket.h"

CClientSocket::CClientSocket(void) : CMoGuiClientSocket(NULL)
{
	m_pClientPool = NULL;
	m_succeed = false;
	m_status  = 0;
	m_pConnect = 0;

	m_nPort = 0;
	m_IP = "";
}

CClientSocket::~CClientSocket(void)
{
	m_pClientPool = NULL;
	m_pPool = 0;
	m_pConnect = 0;
	m_status = 0;
}

void CClientSocket::SetClientPool(CClientPool* pPool)
{
	CMoGuiClientSocket::SetConnectPool(pPool->GetConnectPool());
	m_pClientPool = pPool;
}

void CClientSocket::SetCallBack(CSocketCallBack* pCallBack)
{
	m_pCallBack = pCallBack;
}

void CClientSocket::Close()
{
	CMoGuiClientSocket::Close();
}

bool CClientSocket::Connect(string IP,int nPort)
{
	return CMoGuiClientSocket::Connect(IP.c_str(),nPort);
}

void CClientSocket::OnTimer( UINT32 curTime )
{
	m_pCallBack->OnTimer(curTime);
}
void CClientSocket::OnConnect( void )
{
	CSelfLock	l( m_lk );

	m_State = SOCKET_ST_CONNECTED;
	m_succeed = true;
	if ( m_status==1 )
	{
		m_status = 2;
		m_cd.Signal( );
	}
	m_pCallBack->StartMsg();
	m_pClientPool->AddSocket(this);
}

void CClientSocket::OnClose( bool bactive )
{
	CSelfLock	l( m_lk );

	m_State = SOCKET_ST_NONE;	
	m_succeed = false;
	m_pConnect = 0;
	if ( m_status==1 )
	{
		m_cd.Signal( );
	}
	m_pClientPool->DeleteSocket(this);
}

int CClientSocket::OnMsg( const char* buf, int len )
{
	short XY_Id = 0;
	short XY_Len = 0;
	int FinishSize = 0;

	CRecvMsgPacket recv_packet;
	if( (FinishSize = GetDataFromBuf(buf,len,recv_packet)) > 0 )
	{
		m_nRecvPackets++;
		m_nRecvSize += FinishSize;

		m_pCallBack->OnMsg(this,recv_packet);
	}
	else
	{
		//cout << "CClientSocket id=" << recv_packet.m_XYID << " len=" << recv_packet.m_nLen << " Msglen="<<len << endl;
	}

	return FinishSize;
}