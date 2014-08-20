#include "dbserversocket.h"

#include "server.h"

CDBServerSocket::CDBServerSocket( CServer* server, IConnect* connect ):CMoGuiServerSocket(connect)
{
	assert( connect );

	m_pServer  = server;

	m_nStartTime = 0;
	m_nHeartID = 0;

	m_ServerID = 0;
	m_ServerClass = CLIENT_CLASS_NONE;

	m_Login = 0;
	m_nActiveTime = m_pServer->GetCurTime();

	m_TimeOutTimes = 0;
}

CDBServerSocket::~CDBServerSocket(void)
{
	m_pServer  = NULL;
}

void CDBServerSocket::OnConnect( void )
{
	m_SocketState = SOCKET_ST_CONNECTED;
	m_nStartTime = m_pServer->GetCurTime();
	m_nActiveTime = m_pServer->GetCurTime();
}

void CDBServerSocket::OnClose( bool bactive )
{
	m_SocketState = SOCKET_ST_NONE;
	m_pServer->DealCloseSocket( m_pConnect );
}

void CDBServerSocket::OnCheckHeart()
{
	if( m_SocketState == SOCKET_ST_CONNECTED )
	{
		m_TimeOutTimes++;

		m_nHeartID = (m_pServer->GetCurTime())*(::GetTickCount());		

		Resp_Heart rsph;		
		rsph.m_HeatID = m_nHeartID;
		SendMsg(rsph);
	}
}

int CDBServerSocket::OnMsg( const char* buf, int len )
{
	int FinishSize = 0;
	if( m_nRecvPackets == 0 && (FinishSize = CheckFlash(buf,len)) )
	{
		return FinishSize;
	}

	//置为STOP状态的客户端是被顶替的玩家，因为要发送通知玩家的的消息，因此没有立即关掉SOCKET
	//而是等待消息结束，然后超时关闭SOCKET，当然，玩家可以提前自己关掉，
	if( m_SocketState == SOCKET_ST_STOP )
	{
		m_nRecvPackets++;
		m_nRecvSize += len;

		return len;
	}

	int   nLeftLen = len;
	int   CurFinish = 0;
	char* pRecvBuf = const_cast<char*>(buf);

	CRecvMsgPacket recv_packet;
	while( nLeftLen >= MIN_XY_LEN && (CurFinish = recv_packet.GetDataFromBuf(pRecvBuf+FinishSize,nLeftLen)) > 0 )
	{
		FinishSize += CurFinish;
		nLeftLen -= CurFinish;
		assert(nLeftLen>=0);
		assert(FinishSize<=len);

		if ( m_bCrypt )
		{
			m_Crypt.decrypt((BYTE*)recv_packet.m_DataBuf,(BYTE*)recv_packet.m_DataBuf,recv_packet.m_nLen);			
		}
		TransMsg(recv_packet);
	}

	if ( FinishSize ){ m_nActiveTime = m_pServer->GetCurTime(); }

	return FinishSize;
}

int CDBServerSocket::TransMsg( CRecvMsgPacket& recv_packet )
{
	FinishMsg(recv_packet.m_XYID,recv_packet.GetMsgTransLen());

	if ( (!IsLogin()) && (recv_packet.m_XYID != DBServerXY::DBS_ReqServerConnect::XY_ID) )
	{
		this->Close();
		return 0;
	}

	int nRet = 0;
	switch( recv_packet.m_XYID )
	{
	case Req_Heart::XY_ID:
		{
			nRet = OnRecvReqHeat(recv_packet);
		}	
		break;
	case DBServerXY::DBS_ReqServerConnect::XY_ID:
	case DBServerXY::DBS_ReqServerPlayerLogin::XY_ID:
	case DBServerXY::DBS_ReqPlayerInfo::XY_ID:
	case DBServerXY::DBS_ReqAddBotMoney::XY_ID:
	case DBServerXY::DBS_ReqPlayerGameMoney::XY_ID:
	case DBServerXY::DBS_ReqRoomTableInfo::XY_ID:
	case DBServerXY::DBS_ReqBotPlayer::XY_ID:
	case DBServerXY::DBS_PlayerQuite::XY_ID:
	case DBServerXY::WDB_UserFriend::XY_ID:
	case DBServerXY::WDB_WinLoss::XY_ID:
	case DBServerXY::WDB_WinLossList::XY_ID:
	case DBServerXY::WDB_MatchResult::XY_ID:
	case DBServerXY::WDB_SendMoney::XY_ID:
	case DBServerXY::WDB_ShowFace::XY_ID:
	case DBServerXY::WDB_SendGift::XY_ID:
	case DBServerXY::WDB_SoldGift::XY_ID:
	case DBServerXY::WDB_IncomeAndPay::XY_ID:
	case DBServerXY::WDB_ChatMsg::XY_ID:
	case DBServerXY::WDB_CreateTable::XY_ID:
	case DBServerXY::DBS_PlayerDataNotExist::XY_ID:
	case DBServerXY::DBS_RespPlayerOnLine::XY_ID:
	case DBServerXY::WDB_ChangeUserInfo::XY_ID:
	case DBServerXY::DBS_ReqPlayerAward::XY_ID:
	case DBServerXY::WDB_ReportPlayerOnline::XY_ID:
	case DBServerXY::WDB_MaxPai::XY_ID:
	case DBServerXY::WDB_MaxWin::XY_ID:
	case DBServerXY::WDB_MaxMoney::XY_ID:
	case DBServerXY::WDB_MatchWinLoss::XY_ID:
	case DBServerXY::WDB_PlayerActionLog::XY_ID:
	case DBServerXY::WDB_PlayerAward::XY_ID:
	case DBServerXY::WDB_ReqCheckGameInfo::XY_ID:
	case DBServerXY::WDB_PlayerClientError::XY_ID:
	case DBServerXY::WDB_ReqFinishHonor::XY_ID:
	case DBServerXY::GS_To_DBS_Flag::XY_ID:
	case MOGUI_XieYiList:
		{
			if (m_pServer)
			{
				nRet = m_pServer->OnGameServerMsg(this,recv_packet);
			}
		}
		break;	
	default:
		{				
			cout << "CDBServerSocket:未处理协议 id=" << recv_packet.m_XYID << " len=" << recv_packet.m_nLen << endl;					
		}
	}
	if ( nRet )
	{
		this->Close();
	}
	return nRet;
}

int CDBServerSocket::OnRecvReqHeat( CRecvMsgPacket& msgPack )
{
	Req_Heart rqh;
	TransplainMsg(msgPack,rqh);

	if ( rqh.m_HeatID == this->m_nHeartID )
	{
		m_TimeOutTimes = 0;
		m_nActiveTime = m_pServer->GetCurTime();
	}

	return 0;
}