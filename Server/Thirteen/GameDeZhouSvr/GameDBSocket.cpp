#include "gamedbsocket.h"

#include "server.h"

CGameDBSocket::CGameDBSocket(  CServer* server ):CMoGuiClientSocket( server->GetConnectPool() )
{
	assert(server);

	m_pServer = server;	
	server->SetBDSocket(this);

	m_bCanUse = false;
	m_bAutoConnect = true;

	m_nPort = 0;
	m_IP = "";
}
CGameDBSocket::~CGameDBSocket(void)
{
	m_pServer = NULL;
}

void CGameDBSocket::DebugError(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_ERROR,logbuf);
		printf_s("Error CGameDBSocket %s \n",logbuf);
	}
}

void CGameDBSocket::DebugInfo(const char* logstr,...)
{
#ifdef LOG
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_INFO,logbuf);
	}
#endif
}


void CGameDBSocket::Close()
{
	CMoGuiClientSocket::Close();
	SetAutoConnect(false);
}

bool CGameDBSocket::StartConnect()
{
	if ( m_nPort==0 || m_IP.empty() )
	{
		m_nPort = m_pServer->GetDBPort();
		m_IP = m_pServer->GetDBIP();
	}

	return CMoGuiClientSocket::Connect(m_IP.c_str(),m_nPort);
}

void CGameDBSocket::OnTimer( void )
{

}
void CGameDBSocket::OnConnect( void )
{
	CMoGuiClientSocket::OnConnect();

	DBServerXY::DBS_ReqServerConnect rsc;
	rsc.m_ServerID = m_pServer->GetServerID();
	rsc.m_strSession = std::string("");
	rsc.m_ServerClass = CLIENT_CLASS_GAMESERVER;
	this->SendMsg(rsc);
}

void CGameDBSocket::OnClose( bool bactive )
{
	CMoGuiClientSocket::OnClose(bactive);

	m_bCanUse = false;
}

int CGameDBSocket::OnMsg( const char* buf, int len )
{
	int   FinishSize = 0;
	int   nLeftLen = len;
	int   CurFinish = 0;

	//char pRecvBuf[MAX_RECV_BUF_SIZE];
	//memcpy(pRecvBuf,buf,len);
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

	return FinishSize;
}

int  CGameDBSocket::TransMsg(CRecvMsgPacket& recv_packet)
{
	FinishMsg(recv_packet.m_XYID,recv_packet.GetMsgTransLen());

	int nRet = 0;
	switch( recv_packet.m_XYID )
	{
	case PublicXY::Resp_Heart::XY_ID:
		{
			OnRespHeart(recv_packet);
		}
		break;
	case DBServerXY::DBS_RespServerConnectData::XY_ID:
	case DBServerXY::DBS_RespRoomTableInfo::XY_ID:
	case DBServerXY::DBS_BotPlayerData::XY_ID:
	case DBServerXY::DBS_RespBotPlayer::XY_ID:
	case DBServerXY::DBS_UserFriendInfo::XY_ID:
	case DBServerXY::DBS_UserAwardInfoList::XY_ID:
	case DBServerXY::DBS_UserHuiYuan::XY_ID:
	case DBServerXY::DBS_UserMoguiExchange::XY_ID:
	case DBServerXY::DBS_UserGiftInfoList::XY_ID:		
	case DBServerXY::DBS_UserGiftInfo::XY_ID:
	case DBServerXY::DBS_FaceInfoList::XY_ID:			
	case DBServerXY::DBS_GiftInfoList::XY_ID:
	case DBServerXY::DBS_MatchInfo::XY_ID:
	case DBServerXY::DBS_RoomInfo::XY_ID:
	case DBServerXY::DBS_TableInfo::XY_ID:
	case DBServerXY::DBS_RespServerPlayerLoginData::XY_ID:
	case DBServerXY::DBS_PlayerData::XY_ID:
	case DBServerXY::DBS_PlayerLimite::XY_ID:
	case DBServerXY::DBS_PlayerRight::XY_ID:
	case DBServerXY::DBS_ReqPlayerOnLine::XY_ID:
	case DBServerXY::DBS_RespPlayerInfo::XY_ID:
	case DBServerXY::DBS_PlayerMoneyCheck::XY_ID:
	case DBServerXY::DBS_RespAddBotMoney::XY_ID:
	case DBServerXY::WDB_RespWinLoss::XY_ID:
	case DBServerXY::WDB_RespWinLossList::XY_ID:
	case DBServerXY::DBS_RespPlayerGameMoney::XY_ID:
	case DBServerXY::DBS_To_GS_Flag::XY_ID:
	case DBServerXY::WDB_RespFinishHonor::XY_ID:
	case PublicXY::BatchProtocol::XY_ID:
	case MOGUI_XieYiList:
		{
			nRet = m_pServer->OnDBServerMsg(this,recv_packet);
		}
		break;
	default:
		{
			printf_s("CGameDBSocket:未处理协议，XYID=%d XYLen=%d \n",recv_packet.m_XYID,recv_packet.m_nLen );
		}
	}

	if ( nRet )
	{
		printf_s("CGameDBSocket:协议处理错误，XYID=%d XYLen=%d \n",recv_packet.m_XYID,recv_packet.m_nLen );
		this->Close();
	}
	return nRet;
}

int CGameDBSocket::OnRespHeart(CRecvMsgPacket& msgPack)
{
	PublicXY::Resp_Heart resh;
	TransplainMsg(msgPack,resh);

	PublicXY::Req_Heart reh;
	reh.m_HeatID = resh.m_HeatID;
	SendMsg(reh);

	return 0;
}