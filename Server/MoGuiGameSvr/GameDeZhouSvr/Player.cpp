#include ".\player.h"

#include "gameroom.h"
#include "gametable.h"

using namespace std;
using namespace AGBase;

PLAYERLIST CPlayer::s_pPlayerList;
int CPlayer::s_State[PLAYER_ST_END] = {0};
int CPlayer::s_FinishValue[4]={0};

long CPlayer::s_nMoneyLogCount = 0;

DBS::VectorPTLevelInfo CPlayer::s_vectorGameLevelInfo;
DBS::MapPTLevelInfo CPlayer::s_mapGameLevelInfo;

void CPlayer::ClearAllPlayers()
{
	while (!s_pPlayerList.empty())
	{
		CPlayer* pPlayer = s_pPlayerList.front();
		s_pPlayerList.pop_front();
		safe_delete(pPlayer);
	}
}
void CPlayer::RecyclePlayer(CPlayer* pPlayer)
{
	if( pPlayer == NULL ) return;
	s_pPlayerList.push_back( pPlayer );
}

CPlayer* CPlayer::GetOnePlayer()
{
	CPlayer* retPlayer;
	if ( s_pPlayerList.empty() )
	{
		try
		{
			retPlayer = new CPlayer();
		}
		catch (...)
		{
			printf_s("GetOnePlayer Memory Failed \n");
		}
	}
	else
	{
		retPlayer = s_pPlayerList.front();
		s_pPlayerList.pop_front();
	}

	if ( retPlayer == NULL ) return NULL;
	
	retPlayer->Init();

	return retPlayer;
}
int CPlayer::GetTotalPlayerCount()
{
	return s_pPlayerList.size();
}

CPlayer::CPlayer()
{
}
void CPlayer::SetServer(CServer* pserver)
{
	assert(pserver);
	m_pServer = pserver;
}
CPlayer::~CPlayer(void)
{
}
void CPlayer::OnDestroy(void)
{
	s_State[m_PlayerState]--;
	if ( s_State[m_PlayerState] < 0 )
	{
 		DebugError("OnDestroy State=%d stCount=%d",m_PlayerState,s_State[m_PlayerState]);
	}
	m_PlayerState = PLAYER_ST_NONE;

	RecyclePlayer(this);
}
void CPlayer::Init()
{
	m_pServer = NULL;
	m_pClientSocket = NULL;
	m_pRoom = NULL;
	m_pTable = NULL;

	m_PlayerState = PLAYER_ST_NONE;
	m_PlayerType = PLAYER_TYPE_NONE;
	m_ClientType = CLIENT_TYPE_WEB;

	m_StateInfo = 0;

	m_SocketCloseTime = 0;

	m_KillTime        = 0;
	m_LockTime        = 0;

	m_GSSessionKey = "";

	m_RoomID = 0;
	m_TableID = 0;
	m_SitID = 0;
	m_PlayerPlace = 0;

	m_AID = 0;
	m_PID = 0;
	m_Sex = Sex_None;

	m_ChangeName = 0;
	m_NickName = "";
	m_HeadPicURL = "";
	m_HomePageURL = "";
	m_City = "";

	m_nBankMoney = 0;
	m_OpenBank = 0;

	m_nMoGuiMoney = 0;
	m_nGameMoney = 0;
	
	m_nJF = 0;
	m_nEP = 0;
	m_nWinTimes = 0;
	m_nLossTimes = 0;

	m_nGameTime = 0;

	m_MaxPai = 0;
	m_MaxPaiTime = 0;
	m_MaxMoney = 0;
	m_MaxMoneyTime = 0;
	m_MaxWin = 0;
	m_MaxWinTime = 0;

	memset(m_WinRecord,0,sizeof(m_WinRecord));
	memset(m_Achieve,0,sizeof(m_Achieve));

	if ( m_listHonor.size() )
	{
		m_listHonor.clear();
	}

	m_InvitePID = 0;
	m_JoinTime = 0;
	m_LandTime = 0;

	m_BotLevel = 0;
	m_VipLevel = 0;
	m_GameLevel = 0;

	m_nUpperLimite = 0;
	m_nLowerLimite = 0;

	m_RightLevel = PLAYER_RIGHT_COMMON;
	m_RightTimes = 999;

	m_RoomStateChange = false;
	m_IsRoomShowPlayer = false;
	m_RegisterRoomPlayerTime = 0;
	m_ReqCityPlayerTime = 0;

	m_mapRoomTime.clear();
	m_listFollow.clear();
	m_listBeFollow.clear();
	m_FriendCount = 0;

	m_listReqFriend.clear();

	m_curMapUserGiftInfo.clear();
	m_outMapUserGiftInfo.clear();

	m_FreeFaceTime = 0;
	m_CurGiftIdx = 0;
	m_GiftID = 0;

	m_EndBotPlayerTime = 0;

	m_PromoteTimeStart = 0;
	m_TimeStep = 0;
	m_TimePass = 0;

	m_ContinuPlay = 0;
	m_ContinuLogin = 0;

	m_WinLossMoneyInMatchRoom = 0;

	m_ReqMatchRankTime = 0;

	m_nMatchJF = 0;

	m_PlayerCreateTime = 0;
	m_PlayerSitTime = 0;
	m_OneLifeGameTime = 0;
	m_OneLifeMatchTime = 0;
	m_EnterRoomTimes = 0;
	m_EnterTableTimes = 0;
	m_SitDownTimes = 0;
	m_OneLiftPlayTimes = 0;

	m_TodayGameSecond = 0;
	m_TodayGameCount = 0;
	m_HaveAwardInvite = 0;

	m_JiuMingTimes = 0;

	m_SendGiftTimes = 0;
	m_RecvGiftTimes = 0;
	m_TotalInvite = 0;
	m_ShowFaceTimes = 0;

	if ( m_PlayerState == PLAYER_ST_NONE )
	{
		s_State[m_PlayerState]++;
	}
	else
	{
		DebugError("Player Init State=%d StateCount=%d",m_PlayerState,s_State[m_PlayerState]);
	}
}

void CPlayer::OnKill(UINT32 curTime)
{
	m_KillTime         = curTime;
	SetPlayerState(PLAYER_ST_NONE);
	m_mapRoomTime.clear();
}

void CPlayer::InitEveryDay()
{

}

void CPlayer::DebugError(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_ERROR,logbuf);
		printf_s("Error: Player AID=%d PID=%d %s \n",m_AID,m_PID,logbuf);
	}
}

void CPlayer::DebugInfo(const char* logstr,...)
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

bool CPlayer::IsPlayerSitOnTable()const
{
	if ( m_PlayerState==PLAYER_ST_PALYING || m_PlayerState == PLAYER_ST_WAIT )
	{
		assert(m_RoomID>0&&m_TableID>0&&m_SitID>0);
	}
	if ( m_RoomID>0&&m_TableID>0&&m_SitID>0 )
	{
		assert(m_PlayerState==PLAYER_ST_PALYING || m_PlayerState == PLAYER_ST_WAIT);
	}

	return m_RoomID>0&&m_TableID>0&&m_SitID>0; 
}

bool CPlayer::IsNeedSendMsg()const
{
	bool ret = true;
	if ( m_PlayerType == PLAYER_TYPE_BOT 
		|| !m_pClientSocket 
		|| m_PlayerState == PLAYER_ST_NONE )
	{
		ret = false;
	}
	return ret;
}

bool CPlayer::IsInMatchTable()const
{
	if ( m_pTable && m_SitID>0 )
	{
		assert(IsPlayerSitOnTable());
		if ( m_pTable->IsMatchTable() )
		{
			return true;
		}
	}
	return false;
}

bool CPlayer::IsInTable()const
{
	if ( m_TableID>0 )
	{
		assert( IsPlaying() || IsSeeing() || IsWaiting() );
	}

	return m_RoomID>0 && m_TableID>0 ;
}

UINT32 CPlayer::GetCurTime()
{
	if ( m_pServer )
	{
		return m_pServer->GetCurTime();
	}
	else
	{
		return static_cast<UINT32>(time(NULL));
	}	
}
UINT16 CPlayer::GetServerID()const
{
	return m_pServer->GetServerID();
}
bool CPlayer::CloseSocket()
{
	if( this->m_pClientSocket )
	{
		this->m_pClientSocket->Close();
		this->m_pClientSocket = NULL;
	}
	m_SocketCloseTime = GetCurTime();

	return true;
}

UINT32 CPlayer::GetKillTime()
{
	if ( m_KillTime )
	{
		return GetCurTime() - m_KillTime;
	}
	return 0;
}
UINT32 CPlayer::GetCloseSocketTime()
{
	if ( m_SocketCloseTime )
	{
		return GetCurTime() - m_SocketCloseTime;
	}
	return 0;
}

bool CPlayer::CanKillNow()
{
	DebugInfo("CanKillNow Start Socket=%d PID=%d",reinterpret_cast<int>(m_pClientSocket),m_PID);

	if ( m_pRoom )
	{
		if ( m_RoomID == m_pRoom->GetRoomID() )
		{
			if ( m_pTable )
			{
				if ( m_TableID == m_pTable->GetTableID() )
				{
					if ( m_pTable->IsTableMatching() && this->IsPlayerSitOnTable() )
					{
						return false;
					}

					if ( m_pTable->IsTablePlaying() && this->IsPlaying() )
					{
						return false;
					}
				}
				else
				{
					DebugError("CPlayer::CanKillNow PID=%d,TableID=%d",m_PID,m_TableID);
				}
			}
		}
		else
		{
			DebugError("CPlayer::CanKillNow PID=%d,RoomID=%d",m_PID,m_RoomID);
		}
	}

	DebugInfo("CanKillNow End Socket=%d PID=%d",reinterpret_cast<int>(m_pClientSocket),m_PID);

	return true;
}
void CPlayer::DisAttachSocket()
{
	DebugInfo("DisAttachSocket Start Socket=%d PID=%d",reinterpret_cast<int>(m_pClientSocket),m_PID);

	if( this->m_pClientSocket )
	{
		this->m_pClientSocket->Close();
		this->m_pClientSocket = NULL;
	}
	m_SocketCloseTime = GetCurTime();

	DebugInfo("DisAttachSocket End");
}
void CPlayer::AttachSocket( GameServerSocket* pcs )
{
	DebugInfo("AttachSocket Start old_Socket=%d new_Socket=%d",reinterpret_cast<int>(m_pClientSocket),reinterpret_cast<int>(pcs) );

	if ( pcs == m_pClientSocket && pcs )
	{
		DebugError("AttachSocket 相同的SOCKET pcs=%d",reinterpret_cast<int>(pcs) );
	}

	m_KillTime = 0;
	m_SocketCloseTime = 0;	
	m_mapRoomTime.clear();

	if( this->m_pClientSocket )
	{
		Game_MsgFlag msgFlag;
		msgFlag.m_Flag = msgFlag.LAND_OTHER_PLACE;
		msgFlag.m_nValue = pcs->GetPeerLongIp();
		this->m_pClientSocket->SendMsg(msgFlag);

		this->m_pClientSocket->SetPlayer(NULL);
		this->m_pClientSocket->Stop(GetCurTime());
		//this->m_pClientSocket->Close();
		//this->m_pClientSocket = NULL;
	}

	if( pcs && pcs->GetConnect() )
	{
		m_pClientSocket = pcs;
		pcs->SetPlayer(this);		
	}
	else
	{
		DebugError("CPlayer::Attach");
		assert(0);
	}

	DebugInfo("AttachSocket End");
}

void CPlayer::LeaveTable()
{
	m_SitID = 0;
	m_TableID = 0;
	m_pTable = nullptr;	
}
void CPlayer::LeaveRoom()
{
	LeaveTable();
	m_RoomID = 0;
	m_pRoom = nullptr;
}
void CPlayer::JoinTable(CGameTable* pTable)
{
	assert(pTable);
	m_pTable = pTable;
	m_TableID = pTable->GetTableID();
}
void CPlayer::JoinRoom(CGameRoom* pRoom)
{
	assert(pRoom);
	m_pRoom = pRoom;
	m_RoomID = pRoom->GetRoomID();
}

bool CPlayer::IsFollow(UINT32 PID)
{
	if ( PID > 0 )
	{
		ListPID::iterator itorPID = find(m_listFollow.begin(),m_listFollow.end(),PID);
		if ( itorPID != m_listFollow.end() )
		{
			return true;
		}
	}
	return false;
}
bool CPlayer::IsBeFollow(UINT32 PID)
{
	if ( PID > 0 )
	{
		ListPID::iterator itorPID = find(m_listBeFollow.begin(),m_listBeFollow.end(),PID);
		if ( itorPID != m_listBeFollow.end() )
		{
			return true;
		}
	}
	return false;
}
void CPlayer::AddFollowFriend(UINT32 PID)
{
	if ( !(IsFollow(PID)) )
	{
		m_listFollow.push_back(PID);
	}
}
void CPlayer::DeleteFollowFriend(UINT32 PID)
{
	ListPID::iterator itorPID = find(m_listFollow.begin(),m_listFollow.end(),PID);
	if ( itorPID != m_listFollow.end() )
	{
		m_listFollow.erase(itorPID);
	}
}
void CPlayer::AddBeFollowFriend(UINT32 PID)
{
	if (!(IsBeFollow(PID)))
	{
		m_listBeFollow.push_back(PID);
	}
}
void CPlayer::DeleteBeFollowFriend(UINT32 PID)
{
	ListPID::iterator itorPID = find(m_listBeFollow.begin(),m_listBeFollow.end(),PID);
	if ( itorPID != m_listBeFollow.end() )
	{
		m_listBeFollow.erase(itorPID);
	}
}
bool CPlayer::IsPIDInReqList(UINT32 PID)
{
	if ( m_listReqFriend.size() )
	{
		for (ListReqFriend::iterator itorReqAF=m_listReqFriend.begin();itorReqAF!=m_listReqFriend.end();++itorReqAF)
		{
			if ( itorReqAF->m_ReqPID == PID )
			{
				return true;
			}
		}
	}
	return false;
}
void CPlayer::DeletePIDByTime(UINT32 LatestTime)
{
	for (ListReqFriend::iterator itorReqAF=m_listReqFriend.begin();itorReqAF!=m_listReqFriend.end();)
	{
		if ( itorReqAF->m_ReqTime < LatestTime )
		{
			itorReqAF = m_listReqFriend.erase(itorReqAF);
		}
		else
		{
			++itorReqAF;
		}
	}
}
void CPlayer::DeleteReqFriendPID(UINT32 PID)
{
	for (ListReqFriend::iterator itorReqAF=m_listReqFriend.begin();itorReqAF!=m_listReqFriend.end();)
	{
		if ( itorReqAF->m_ReqPID == PID )
		{
			itorReqAF = m_listReqFriend.erase(itorReqAF);
			//break;
		}
		else
		{
			++itorReqAF;
		}
	}
}
void CPlayer::AddReqFriend(UINT32 PID,UINT32 CurTime)
{
	if ( PID>0 && CurTime>0 )
	{
		if ( !(IsPIDInReqList(PID)) )
		{
			stAddFriendInfo stAFI;
			stAFI.m_ReqPID = PID;
			stAFI.m_ReqTime = CurTime;
			m_listReqFriend.push_back(stAFI);
		}
	}
}

void CPlayer::DeleteGift(int GiftIdx)
{
	MapUserGiftInfo::iterator itorGI = m_curMapUserGiftInfo.find(GiftIdx);
	if ( itorGI != m_curMapUserGiftInfo.end() )
	{
		m_curMapUserGiftInfo.erase(itorGI);
	}

	itorGI = m_outMapUserGiftInfo.find(GiftIdx);
	if ( itorGI != m_outMapUserGiftInfo.end() )
	{
		m_outMapUserGiftInfo.erase(itorGI);
	}
}
void CPlayer::AddGift(const DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int nFlag)
{
	if ( nFlag == N_Gift::GiftTime_Cur )
	{
		MapUserGiftInfo::iterator itorGI = m_curMapUserGiftInfo.find(stDBUgi.m_GiftIdx);
		if ( itorGI == m_curMapUserGiftInfo.end() )
		{
			m_curMapUserGiftInfo.insert(make_pair(stDBUgi.m_GiftIdx,stDBUgi));
		}
	}
	else if ( nFlag == N_Gift::GiftTime_Out )
	{
		MapUserGiftInfo::iterator itorGI = m_outMapUserGiftInfo.find(stDBUgi.m_GiftIdx);
		if ( itorGI == m_outMapUserGiftInfo.end() )
		{
			m_outMapUserGiftInfo.insert(make_pair(stDBUgi.m_GiftIdx,stDBUgi));
		}
	}
}
bool CPlayer::GetCurUserGift(DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int GiftIdx)
{
	MapUserGiftInfo::iterator itorGI = m_curMapUserGiftInfo.find(GiftIdx);
	if ( itorGI != m_curMapUserGiftInfo.end() )
	{
		stDBUgi = itorGI->second;
		return true;
	}
	return false;
}
bool CPlayer::GetUserGift(DBServerXY::DBS_msgUserGiftInfo& stDBUgi, int GiftIdx)
{
	bool bRet = false;
	MapUserGiftInfo::iterator itorGI = m_outMapUserGiftInfo.find(GiftIdx);
	if ( itorGI != m_outMapUserGiftInfo.end() ){
		stDBUgi = itorGI->second;
		bRet = true;
	}
	if ( bRet == false ){
		itorGI = m_curMapUserGiftInfo.find(GiftIdx);
		if (itorGI != m_curMapUserGiftInfo.end()){
			stDBUgi = itorGI->second;
			bRet = true;
		}
	}
	return bRet;
}
void CPlayer::CheckMaxGameMoney()
{
	if ( m_nGameMoney > m_MaxMoney )
	{
		m_MaxMoney      = m_nGameMoney;
		m_MaxMoneyTime  = GetCurTime();

		SendMaxMoney();

		DBServerXY::WDB_MaxMoney wdbMM;
		wdbMM.m_AID              = m_AID;
		wdbMM.m_PID              = m_PID;
		wdbMM.m_MaxMoney         = m_nGameMoney;
		wdbMM.m_MaxMoneyTime     = m_MaxMoneyTime;

		m_pServer->SendMsgToDBServer(wdbMM);
	}
}
void CPlayer::CheckMaxWinMoney(INT64 nWinMoney)
{
	if ( nWinMoney > m_MaxWin )
	{
		m_MaxWin     = nWinMoney;
		m_MaxWinTime = GetCurTime();

		SendMaxWin();

		DBServerXY::WDB_MaxWin msgMW;
		msgMW.m_AID              = m_AID;
		msgMW.m_PID              = m_PID;
		msgMW.m_MaxWin           = nWinMoney;
		msgMW.m_MaxWinTime       = m_MaxWinTime;
		m_pServer->SendMsgToDBServer(msgMW);
	}
}
void CPlayer::CheckMaxPai(DezhouLib::CPaiType& stPaiType)
{
	DezhouLib::CPaiType TempPaiType(m_MaxPai);
	if ( stPaiType > TempPaiType )
	{
		m_MaxPai      = stPaiType.GetPaiVectorValue();
		m_MaxPaiTime  = GetCurTime();

		SendMaxPai();

		DBServerXY::WDB_MaxPai msgMaxPai;
		msgMaxPai.m_AID        = m_AID;
		msgMaxPai.m_PID        = m_PID;
		msgMaxPai.m_MaxPai     = m_MaxPai;
		msgMaxPai.m_MaxPaiTime = m_MaxPaiTime;
		m_pServer->SendMsgToDBServer(msgMaxPai);
	}
}
int CPlayer::IsHonorFinish(INT16 HonorID)
{
	int nRet = N_Honor::HonorResult_Error;
	if ( DezhouLib::IsValidHonorID(HonorID) )
	{
		BYTE nType = DezhouLib::GetType(HonorID);
		BYTE nIdx  = DezhouLib::GetIdx(HonorID);
		if ( DezhouLib::IsValidType(nType) && DezhouLib::IsValidIdx(nIdx) )
		{
			if ( (m_Achieve[nType-1]&S_Uint32_Number[nIdx-1]) == S_Uint32_Number[nIdx-1] )
			{
				nRet = N_Honor::HonorResult_True;
			}
			else
			{
				nRet = N_Honor::HonorResult_False;
			}
		}
	}
	return nRet;
}
void CPlayer::FinishHonor(INT16 HonorID)
{
	if ( DezhouLib::IsValidHonorID(HonorID) )
	{
		BYTE nType = DezhouLib::GetType(HonorID);
		BYTE nIdx  = DezhouLib::GetIdx(HonorID);
		if ( DezhouLib::IsValidType(nType) && DezhouLib::IsValidIdx(nIdx) 
			&& (m_Achieve[nType-1]&S_Uint32_Number[nIdx-1])==0 )
		{
			DBServerXY::WDB_ReqFinishHonor msgFH;
			msgFH.m_AID         = m_AID;
			msgFH.m_PID         = m_PID;
			msgFH.m_HonorID     = HonorID;
			m_pServer->SendMsgToDBServer(msgFH);

			if ( m_Achieve[nType-1]==CPlayer::s_FinishValue[nType-1] && (m_Achieve[nType-1]&N_Honor::Honor_Full)==0 )
			{
				msgFH.ReSet();
				msgFH.m_AID         = m_AID;
				msgFH.m_PID         = m_PID;
				msgFH.m_HonorID     = nType*100+31;
				m_pServer->SendMsgToDBServer(msgFH);
			}
		}
	}
}
void CPlayer::DoFinishHonor(INT16 HonorID)
{
	if ( DezhouLib::IsValidHonorID(HonorID) )
	{
		if ( find(m_listHonor.begin(),m_listHonor.end(),HonorID) == m_listHonor.end() )
		{
			m_listHonor.push_back(BYTE(HonorID));
		}

		BYTE nType = DezhouLib::GetType(HonorID);
		BYTE nIdx  = DezhouLib::GetIdx(HonorID);
		if ( DezhouLib::IsValidType(nType) && DezhouLib::IsValidIdx(nIdx) )
		{
			m_Achieve[nType-1] |= S_Uint32_Number[nIdx-1];
		}
	}
}
bool CPlayer::IsFinishAchieve(int nSort)
{
	if ( nSort>=N_Honor::HonorSort_Tong && nSort<=N_Honor::HonorSort_Zuan )
	{
		return (m_Achieve[nSort]&=S_Uint32_Number[30]) == S_Uint32_Number[30];
	}
	return false;
}
void CPlayer::SendLastGiftInfo(PlayerPtr pPlayer)
{
	if ( pPlayer )
	{
		GameXY::msgPlayerGiftInfo stPGI;
		GameXY::PlayerGiftInfoList msgPGIL;

		int nSendCount = 0;

		msgPGIL.m_PID  = m_PID;
		msgPGIL.m_Flag = msgPGIL.LAST;
		for ( MapUserGiftInfo::iterator itorUGI=m_outMapUserGiftInfo.begin();itorUGI!=m_outMapUserGiftInfo.end();++itorUGI)
		{
			stPGI.ReSet();
			stPGI.m_GiftIdx    = itorUGI->second.m_GiftIdx;
			stPGI.m_GiftID     = itorUGI->second.m_GiftID;
			stPGI.m_Price      = itorUGI->second.m_Price;
			stPGI.m_ActionTime = itorUGI->second.m_ActionTime;
			stPGI.m_NickName   = itorUGI->second.m_NickName;
			msgPGIL.m_listPGI.push_back(stPGI);

			nSendCount++;
			if ( pPlayer->GetPID() != m_PID && nSendCount >= 10 )
			{
				break;
			}

			if ( msgPGIL.m_listPGI.size() >= msgPGIL.MAX_COUNT )
			{
				SendMsg(msgPGIL);
				msgPGIL.m_listPGI.clear();
				msgPGIL.m_nCount = 0;
			}
		}
		if ( msgPGIL.m_listPGI.size())
		{
			SendMsg(msgPGIL);
		}
	}
}

void CPlayer::SendCurGiftInfo(PlayerPtr pPlayer)
{
	if ( pPlayer )
	{
		GameXY::msgPlayerGiftInfo stPGI;
		GameXY::PlayerGiftInfoList msgPGIL;

		int nSendCount = 0;

		msgPGIL.m_PID  = m_PID;
		msgPGIL.m_Flag = msgPGIL.CUR;
		for ( MapUserGiftInfo::iterator itorUGI=m_curMapUserGiftInfo.begin();itorUGI!=m_curMapUserGiftInfo.end();++itorUGI)
		{
			stPGI.ReSet();
			stPGI.m_GiftIdx    = itorUGI->second.m_GiftIdx;
			stPGI.m_GiftID     = itorUGI->second.m_GiftID;
			stPGI.m_Price      = itorUGI->second.m_Price;
			stPGI.m_ActionTime = itorUGI->second.m_ActionTime;
			stPGI.m_NickName   = itorUGI->second.m_NickName;
			msgPGIL.m_listPGI.push_back(stPGI);

			nSendCount++;
			if ( pPlayer->GetPID() != m_PID && nSendCount >= 20 )
			{
				break;
			}

			if ( msgPGIL.m_listPGI.size() >= msgPGIL.MAX_COUNT )
			{
				pPlayer->SendMsg(msgPGIL);
				msgPGIL.m_listPGI.clear();
				msgPGIL.m_nCount = 0;
			}
		}
		if ( msgPGIL.m_listPGI.size()  )
		{
			pPlayer->SendMsg(msgPGIL);
			msgPGIL.m_listPGI.clear();
			msgPGIL.m_nCount = 0;
		}
	}
}

void CPlayer::UpdateGameMoney()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::PlayerGameMoney msgPGM;
		msgPGM.m_AID          = m_AID;
		msgPGM.m_PID          = m_PID;
		msgPGM.m_nGameMoney   = m_nGameMoney;
		SendMsg(msgPGM);
	}	
}

void CPlayer::UpdateGameInfo()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::PlayerGameInfo msgPGI;
		GetPlayerGameInfo(msgPGI);
		SendMsg(msgPGI);
	}
}

void CPlayer::UpdateMoGuiMoney()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::PlayerMoGuiMoney msgPGM;
		msgPGM.m_AID          = m_AID;
		msgPGM.m_PID          = m_PID;
		msgPGM.m_nMoGuiMoney  = m_nMoGuiMoney;
		SendMsg(msgPGM);
	}
}

void CPlayer::SetPlayerData(const DBServerXY::DBS_PlayerData& msgPD)
{
	m_AID            = msgPD.m_AID;
	m_PID            = msgPD.m_PID;
	m_Sex            = msgPD.m_Sex;

	m_ChangeName     = msgPD.m_ChangeName;
	m_PlayerType     = msgPD.m_PlayerType;
	m_NickName       = msgPD.m_NickName;
	m_HeadPicURL     = msgPD.m_HeadPicURL;
	m_HomePageURL    = msgPD.m_HomePageURL;
	m_City           = msgPD.m_City;

	m_nBankMoney     = msgPD.m_nBankMoney;
	m_OpenBank       = msgPD.m_OpenBank;
	m_nMoGuiMoney    = int(msgPD.m_nMoGuiMoney);
	m_nGameMoney     = msgPD.m_nGameMoney;
	m_nMatchJF       = msgPD.m_nMatchJF;

	m_nJF            = msgPD.m_nJF;
	m_nEP            = msgPD.m_nEP;
	m_nWinTimes      = int(msgPD.m_nWinTimes);
	m_nLossTimes     = int(msgPD.m_nLossTimes);
	m_nGameTime      = int(msgPD.m_nGameTime);

	m_nUpperLimite   = msgPD.m_nUpperLimite;
	m_nLowerLimite   = msgPD.m_nLowerLimite;

	m_RightLevel     = msgPD.m_RightLevel;
	m_RightTimes     = msgPD.m_RightTimes;

	m_JoinTime       = msgPD.m_JoinTime;
	m_LandTime       = UINT32(time(NULL));
	m_InvitePID      = msgPD.m_InvitePID;

	m_ContinuLogin   = msgPD.m_ContinuLogin;
	m_ContinuPlay    = msgPD.m_ContinuPlay;

	UpdateGameLevel(false);	

	ClearMoneyLog();
	AddMoneyLog(msgPD.m_nGameMoney,"SetPlayerData");
}

void CPlayer::GetPlayerData(GameXY::Game_PlayerData& game_msgPD)
{
	game_msgPD.m_AID            = m_AID;
	game_msgPD.m_PID            = m_PID;

	game_msgPD.m_Sex            = m_Sex;
	game_msgPD.m_PlayerType     = m_PlayerType;
	game_msgPD.m_ChangeName     = m_ChangeName;

	game_msgPD.m_NickName       = m_NickName;
	game_msgPD.m_HeadPicURL     = m_HeadPicURL;
	game_msgPD.m_HomePageURL    = m_HomePageURL;
	game_msgPD.m_City           = m_City;

	game_msgPD.m_nBankMoney     = m_nBankMoney;
	game_msgPD.m_OpenBank       = m_OpenBank;
	game_msgPD.m_nMoGuiMoney    = m_nMoGuiMoney;
	game_msgPD.m_nGameMoney     = m_nGameMoney;
	game_msgPD.m_nMatchJF       = m_nMatchJF;

	game_msgPD.m_nJF            = m_nJF;
	game_msgPD.m_nEP            = m_nEP;
	game_msgPD.m_nWinTimes      = m_nWinTimes;
	game_msgPD.m_nLossTimes     = m_nLossTimes;
	game_msgPD.m_nGameTime      = m_nGameTime;

	game_msgPD.m_JoinTime       = m_JoinTime;
	game_msgPD.m_LandTime       = m_LandTime;

	game_msgPD.m_RoomID         = m_RoomID;
	game_msgPD.m_TableID        = m_TableID;
	game_msgPD.m_SitID          = m_SitID;
	game_msgPD.m_PlayerState    = m_PlayerState;

	game_msgPD.m_GameLevel      = m_GameLevel;
}

void CPlayer::SetPlayerDataEx(const MoGui::MoGuiXY::DBServerXY::DBS_PlayerDataEx& msgPDEx)
{
	m_MaxMoney                  = msgPDEx.m_MaxMoney;
	m_MaxMoneyTime              = msgPDEx.m_MaxMoneyTime;
	m_MaxPai                    = msgPDEx.m_MaxPai;
	m_MaxPaiTime                = msgPDEx.m_MaxPaiTime;
	m_MaxWin                    = msgPDEx.m_MaxWin;
	m_MaxWinTime                = msgPDEx.m_MaxWinTime;

	memcpy(m_WinRecord,msgPDEx.m_WinRecord,sizeof(m_WinRecord));
	memcpy(m_Achieve,msgPDEx.m_Achieve,sizeof(m_Achieve));

	m_FreeFaceTime              = msgPDEx.m_FreeFaceTime;
	m_VipLevel                  = BYTE(msgPDEx.m_VipLevel);

	m_SendGiftTimes             = msgPDEx.m_SendGift;
	m_RecvGiftTimes             = msgPDEx.m_RecvGift;
	m_TotalInvite               = msgPDEx.m_TotalInvite;
	m_ShowFaceTimes             = msgPDEx.m_ShowFace;

	m_FriendCount               = msgPDEx.m_FriendCount;

	CheckMaxGameMoney();
}
void CPlayer::GetPlayerDataEx(GameXY::Game_PlayerDataEx& game_msgPDEx)
{
	game_msgPDEx.m_AID          = m_AID;
	game_msgPDEx.m_PID          = m_PID;

	game_msgPDEx.m_MaxMoney     = m_MaxMoney;
	game_msgPDEx.m_MaxMoneyTime = m_MaxMoneyTime;
	game_msgPDEx.m_MaxPai       = m_MaxPai;
	game_msgPDEx.m_MaxPaiTime   = m_MaxPaiTime;
	game_msgPDEx.m_MaxWin       = m_MaxWin;
	game_msgPDEx.m_MaxWinTime   = m_MaxWinTime;

	//memcpy(game_msgPDEx.m_WinRecord,m_WinRecord,sizeof(m_WinRecord));
	//memcpy(game_msgPDEx.m_Achieve,m_Achieve,sizeof(m_Achieve));

	game_msgPDEx.m_VipLevel     = m_VipLevel;
	game_msgPDEx.m_FriendCount  = m_FriendCount;
}
void CPlayer::GetPlayerTailInfo(GameXY::PlayerTailInfo& msgPTI)
{
	msgPTI.m_PID                = m_PID;

	msgPTI.m_JoinTime           = m_JoinTime;
	msgPTI.m_LandTime           = m_LandTime;
	msgPTI.m_MaxMoneyTime       = m_MaxMoneyTime;
	msgPTI.m_maxOwnMoney        = m_MaxMoney;
	msgPTI.m_MaxWinTime         = m_MaxWinTime;
	msgPTI.m_maxWinMoney        = m_MaxWin;

	msgPTI.m_FriendCount        = m_FriendCount;
	msgPTI.m_nWinTimes          = m_nWinTimes;
	msgPTI.m_nLossTimes         = m_nLossTimes;
	msgPTI.m_MaxPaiTime         = m_MaxPaiTime;
	msgPTI.m_MaxPai             = m_MaxPai;

	msgPTI.m_HonorCount         = BYTE(m_listHonor.size());
}

void CPlayer::SetBotPlayerData(const MoGui::MoGuiXY::DBServerXY::DBS_BotPlayerData& bpd)
{
	m_AID            = bpd.m_AID;
	m_PID            = bpd.m_PID;
	m_Sex            = bpd.m_Sex;

	m_NickName       = bpd.m_NickName;
	m_HeadPicURL     = bpd.m_HeadPicURL;
	m_HomePageURL    = bpd.m_HomePageURL;
	m_City           = bpd.m_City;

	m_nMoGuiMoney    = INT32(bpd.m_nMoGuiMoney);
	m_nGameMoney     = bpd.m_nGameMoney;
	m_nMatchJF       = bpd.m_nMatchJF;

	m_nJF            = bpd.m_nJF;
	m_nEP            = bpd.m_nEP;
	m_nWinTimes      = INT32(bpd.m_nWinTimes);
	m_nLossTimes     = INT32(bpd.m_nLossTimes);
	m_nGameTime      = INT32(bpd.m_nGameTime);

	m_nUpperLimite   = bpd.m_nUpperLimite;
	m_nLowerLimite   = bpd.m_nLowerLimite;

	m_RightLevel     = bpd.m_RightLevel;
	m_RightTimes     = bpd.m_RightTimes;

	UpdateGameLevel(false);

	m_MaxPai         = bpd.m_MaxPai;
	m_MaxPaiTime     = bpd.m_MaxPaiTime;
	m_MaxMoney       = bpd.m_MaxMoney;
	m_MaxMoneyTime   = bpd.m_MaxMoneyTime;
	m_MaxWin         = bpd.m_MaxWin;
	m_MaxWinTime     = bpd.m_MaxWinTime;

	memcpy(m_WinRecord,bpd.m_WinRecord,sizeof(m_WinRecord));
	memcpy(m_Achieve,bpd.m_Achieve,sizeof(m_Achieve));

	m_VipLevel       = bpd.m_VipLevel;

	m_JoinTime       = bpd.m_JoinTime;
	m_LandTime       = UINT32(time(NULL));
	m_InvitePID      = bpd.m_InvitePID;

	ClearMoneyLog();
	AddMoneyLog(bpd.m_nGameMoney,"SetBotPlayerData");
}

void CPlayer::UpdateGameLevel(bool bCheck)
{
	int nTotalPlayTime = m_nWinTimes + m_nLossTimes;	

	int nLevel = 0;
	for (size_t nPos=0;nPos<s_vectorGameLevelInfo.size();++nPos)
	{
		if( nTotalPlayTime < s_vectorGameLevelInfo[nPos].m_TotalPT )
		{
			nLevel = nPos + 1;
			break;
		}
	}

	BYTE nOldLevel = m_GameLevel;
	m_GameLevel = nLevel;

	if ( bCheck && nLevel>1 && nLevel>nOldLevel )
	{
		INT32 AwardMoney = s_vectorGameLevelInfo[nLevel-1-1].m_AwardMoney;

		m_nGameMoney += AwardMoney;

		AddMoneyLog(AwardMoney,"UpdateGameLevel");

		GameLevelAward msgGLA;
		msgGLA.m_curLevel     = m_GameLevel;
		msgGLA.m_curAward     = AwardMoney;
		msgGLA.m_GameMoney    = m_nGameMoney;
		msgGLA.m_nextAdd      = s_vectorGameLevelInfo[nLevel-1].m_AddPT;
		msgGLA.m_nextAward    = s_vectorGameLevelInfo[nLevel-1].m_AwardMoney;
		SendMsg(msgGLA);

		DBServerXY::WDB_IncomeAndPay msgIP;
		msgIP.m_AID         = m_AID;
		msgIP.m_PID         = m_PID;
		msgIP.m_nMoney      = AwardMoney;
		msgIP.m_Flag        = N_IncomeAndPay::GameLevel;
		msgIP.m_strReMark   = "Level="+N2S(m_GameLevel);
		PlayerSendMsgToDBServer(msgIP);
	}
}
void CPlayer::GetPlayerBaseInfo(PlayerBaseInfo& pbi)
{
	pbi.m_AID                = this->m_AID;
	pbi.m_PID                = this->m_PID;
	pbi.m_nGameMoney         = this->m_nGameMoney;
	pbi.m_PlayerState        = this->m_PlayerState;
	pbi.m_GameLevel          = this->m_GameLevel;
	pbi.m_NickName           = this->m_NickName;
	pbi.m_HeadPicURL         = this->m_HeadPicURL;
}
void CPlayer::GetPlayerGameInfo(PlayerGameInfo& pgi)
{
	pgi.m_PID                = this->m_PID;
	pgi.m_nGameMoney         = this->m_nGameMoney;
	pgi.m_nJF                = this->m_nJF;
	pgi.m_nExperience        = this->m_nEP;
	
	pgi.m_nWinTimes          = this->m_nWinTimes;
	pgi.m_nLossTimes         = this->m_nLossTimes;
}
void CPlayer::GetPlayerStateInfo(PlayerStateInfo& psi)
{
	psi.m_PID                = this->m_PID;
	psi.m_RoomID             = this->m_RoomID;
	psi.m_TableID            = this->m_TableID;
	psi.m_SitID              = this->m_SitID;
	psi.m_PlayerState        = this->m_PlayerState;

	if ( (psi.m_Flag==psi.SameCity)
		|| (psi.m_Flag==psi.SameTable)
		|| (psi.m_Flag==psi.Friend) )
	{
		if ( m_TableID>0 && m_pTable )
		{
			psi.m_strState = m_pTable->GetTableName();
		}
		else if ( m_PlayerState>=PLAYER_ST_LOBBY )
		{
			psi.m_strState = "大厅";
		}
	}	
}
void CPlayer::SendMaxPai()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::PlayerMaxPai msgMP;
		msgMP.m_MaxPai        = m_MaxPai;
		msgMP.m_MaxPaiTime    = m_MaxPaiTime;
		SendMsg(msgMP);
	}	
}
void CPlayer::SendMaxWin()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::PlayerMaxWin msgMW;
		msgMW.m_MaxWin        = m_MaxWin;
		msgMW.m_MaxWinTime    = m_MaxWinTime;
		SendMsg(msgMW);
	}	
}
void CPlayer::SendMaxMoney()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::PlayerMaxMoney msgMM;
		msgMM.m_MaxMoney      = m_MaxMoney;
		msgMM.m_MaxMoneyTime  = m_MaxMoneyTime;
		SendMsg(msgMM);
	}	
}
void CPlayer::SendPlayerData()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::Game_PlayerData game_msgPD;
		GetPlayerData(game_msgPD);
		SendMsg(game_msgPD);
	}	
}
void CPlayer::SendPlayerDataEx()
{
	if ( IsNeedSendMsg() )
	{
		GameXY::Game_PlayerDataEx game_msgPDEx;
		GetPlayerDataEx(game_msgPDEx);
		SendMsg(game_msgPDEx);
	}
}
void CPlayer::GetHonorList(VectorHonorID& listHI)
{
	int TempInt;
	for ( int nType=0;nType<4;++nType)
	{
		TempInt = 1;
		for ( int nPos=0;nPos<30;++nPos)
		{
			if ( (m_Achieve[nType]&TempInt) == TempInt )
			{
				listHI.push_back(DezhouLib::GetHonorID(nType+1,nPos+1));
			}
			TempInt = (TempInt<<1);
		}
	}
}
void CPlayer::SendPlayerHonor()
{
	/*
	if ( IsNeedSendMsg() )
	{
		if ( m_listHonor.size() == 0 )
		{
			int TempInt;
			for ( int nType=0;nType<4;++nType)
			{
				TempInt = 1;
				for ( int nPos=0;nPos<30;++nPos)
				{
					if ( (m_Achieve[nType]&TempInt) == TempInt )
					{
						BYTE HonorID = BYTE(DezhouLib::GetHonorID(nType+1,nPos+1));
						m_listHonor.push_back(HonorID);
					}
					TempInt = (TempInt<<1);
				}
			}
		}

		if ( m_listHonor.size() )
		{
			GameXY::PlayerHonorInfoList msgPHIL;
			msgPHIL.m_PID = m_PID;
			msgPHIL.m_listHonorID = m_listHonor;
			SendMsg(msgPHIL);
		}
	}
	*/
}
void CPlayer::SendHonorToPlayer(PlayerPtr pPlayer)
{
	if ( pPlayer && pPlayer->IsNeedSendMsg() && m_listHonor.size() )
	{
		GameXY::PlayerHonorInfoList msgPHIL;
		msgPHIL.m_PID = m_PID;
		msgPHIL.m_listHonorID = m_listHonor;
		pPlayer->SendMsg(msgPHIL);
	}
}
void CPlayer::SetPlayerState( BYTE st )
{
	if( st != m_PlayerState )
	{
		s_State[m_PlayerState]--;
		s_State[st]++;

		if ( s_State[m_PlayerState]<0 || s_State[st]<=0 )
		{
			DebugError("PlayerState=%d st=%d pscount=%d stcount=%d",m_PlayerState,st,s_State[m_PlayerState],s_State[st]);
		}

		m_PlayerState = st;
	}
}
void CPlayer::SetPlayerType(BYTE type)
{
	m_PlayerType = type;
}

void CPlayer::AddMatchJF(INT64 nJF)
{
	if ( nJF>=0 )
	{
		m_nMatchJF += nJF;
	}
	else
	{
		DebugError("AddMatchJF PID=%d m_nMatchJF=%s nJF=%s",
			m_PID,Tool::N2S(m_nMatchJF).c_str(),Tool::N2S(nJF).c_str() );
	}
}
void CPlayer::DecGameMoney(INT64 nMoney)
{
	if ( nMoney>=0 && m_nGameMoney-nMoney>=0)
	{
		m_nGameMoney -= nMoney;
	}
	else
	{
		DebugError("DecGameMoney PID=%-10d m_nGameMoney=%s nMoney=%s",
			m_PID,Tool::N2S(m_nGameMoney).c_str(),Tool::N2S(nMoney).c_str() );
	}	
}
void CPlayer::AddGameMoney(INT64 nMoney)
{
	DebugInfo("AddGameMoney PID=%d GameMoney=%s AddMoney=%s",m_PID,N2S(m_nGameMoney).c_str(),N2S(nMoney).c_str());

	m_nGameMoney += nMoney;
	if ( m_nGameMoney < 0 )
	{
		DebugError("AddGameMoney PID=%-10d m_nGameMoney=%s nMoney=%s",m_PID,Tool::N2S(m_nGameMoney).c_str(),Tool::N2S(nMoney).c_str() );

		m_nGameMoney = 0;		
	}
}
void CPlayer::DecMoGuiMoney(INT32 nMoney)
{
	if ( nMoney>=0 && m_nMoGuiMoney-nMoney >= 0 )
	{
		m_nMoGuiMoney -= nMoney;
	}
	else
	{
		DebugError("DecMoGuiMoney PID=%-10d m_nMoGuiMoney=%d nMoney=%d",m_PID,m_nMoGuiMoney,nMoney );
	}
}
void CPlayer::AddMoGuiMoney(INT32 nMoney)
{
	if ( nMoney>=0 && m_nMoGuiMoney+nMoney>=0 )
	{
		m_nMoGuiMoney += nMoney;
	}
	else
	{
		DebugError("AddMoGuiMoney PID=%-10d m_nMoGuiMoney=%d nMoney=%d",m_PID,m_nMoGuiMoney,nMoney );
	}
}
bool CPlayer::HaveSendTableInfoList(UINT16 roomid)
{
	if( m_mapRoomTime.size() > 0 )
	{
		MapSendRoomTableInfo::iterator itorSend = m_mapRoomTime.find(roomid);
		if (itorSend != m_mapRoomTime.end())
		{
			return true;
		}
	}
	return false;
}
void CPlayer::AddRoomHaveSendTableInfo( UINT16 roomid)
{
	MapSendRoomTableInfo::iterator itorSend = m_mapRoomTime.find(roomid);
	if (itorSend == m_mapRoomTime.end())
	{
		m_mapRoomTime.insert(make_pair(roomid,0));
	}
}

UINT32 CPlayer::GetSendTime(UINT16 roomid)
{
	MapSendRoomTableInfo::iterator itorSend = m_mapRoomTime.find(roomid);
	if (itorSend != m_mapRoomTime.end())
	{
		return itorSend->second;
	}
	else
	{
		//DebugError("GetSendTime roomid=%d",roomid);
	}
	return 0;
}
void CPlayer::SetSendTime(UINT16 roomid,UINT32 SendTime)
{
	MapSendRoomTableInfo::iterator itorSend = m_mapRoomTime.find(roomid);
	if (itorSend != m_mapRoomTime.end())
	{
		itorSend->second = SendTime;
	}
	else
	{
		//DebugError("SetSendTime roomid=%d",roomid);
	}
}

bool CPlayer::CanReqMatchRank(UINT32 curTime)
{
	if ( curTime - m_ReqMatchRankTime >= 25 )
	{
		m_ReqMatchRankTime = curTime;
		return true;
	}
	return false;
}

void  CPlayer::SetCityStateChange(bool bChange)
{
	if ( bChange )
		m_StateInfo |= StateInfo_City;
	else
		m_StateInfo &= (~StateInfo_City);
}
bool  CPlayer::IsCityStateChange()
{
	return (m_StateInfo&StateInfo_City) == StateInfo_City;
}
void CPlayer::SetFriendStateChange(bool bChange)
{
	if ( bChange )
		m_StateInfo |= StateInfo_Friend;
	else
		m_StateInfo &= (~StateInfo_Friend);
}
bool CPlayer::IsFriendStateChange()
{
	return (m_StateInfo&StateInfo_Friend) == StateInfo_Friend;
}

bool CPlayer::IsSendSameTablePlayer()
{
	return (m_StateInfo&StateInfo_SendSameTable) == StateInfo_SendSameTable;
}
void CPlayer::SetSendSameTablePlayer(bool bSend)
{
	if ( bSend )
		m_StateInfo |= StateInfo_SendSameTable;
	else
		m_StateInfo &= (~StateInfo_SendSameTable);
}
bool CPlayer::IsShowLobby()
{
	return (m_StateInfo&StateInfo_ShowLobby) == StateInfo_ShowLobby;
}
void CPlayer::SetShowLobby(bool bShow)
{
	if ( bShow )
		m_StateInfo |= StateInfo_ShowLobby;
	else
		m_StateInfo &= (~StateInfo_ShowLobby);
}
int  CPlayer::OnPacket(CRecvMsgPacket& msgPack)
{
	//DebugInfo("CPlayer::OnPacket Start PID XYID=%d Len=%d",m_PID,msgPack.m_XYID,msgPack.m_nLen );

	int nRet = 0;

	switch ( msgPack.m_XYID )
	{
	case GameXY::ReqRoomInfo::XY_ID:
	case GameXY::ReqRoomTableInfo::XY_ID:
	case GameXY::ReqJoinRoom::XY_ID:
	case GameXY::ReqJoinTable::XY_ID:
	case GameXY::ReqSignMatch::XY_ID:
	case GameXY::ReqEnterMatch::XY_ID:
	case GameXY::ReqMatchRank::XY_ID:
	case GameXY::ReqTailInfo::XY_ID:
	case GameXY::ReqPlayerGiftInfo::XY_ID:
	case GameXY::ReqGamePlayerCount::XY_ID:
	case GameXY::ReqRoomPlayerCount::XY_ID:
	case GameXY::ReqTablePlayerCount::XY_ID:
	case GameXY::ReqCityPlayerBase::XY_ID:
	case GameXY::ReqCityPlayerState::XY_ID:
	case GameXY::PlayerReqSendGift::XY_ID:
	case GameXY::PlayerReqSoldGift::XY_ID:
	case GameXY::ReqChangeGift::XY_ID:
	case GameXY::ReqChangeBank::XY_ID:
	case GameXY::ReqCreateTable::XY_ID:
	case GameXY::ChangeUserInfo::XY_ID:
	case GameXY::ReqPlayerAward::XY_ID:
	case GameXY::ReqTracePlayer::XY_ID:
	case GameXY::ChangePlace::XY_ID:
	case GameXY::Game_MsgFlag::XY_ID:
	case GameXY::Game_Test::XY_ID:
		{
			if( m_pServer )
			{
				nRet = m_pServer->OnServerMsg(this,msgPack);
			}
			else
			{
				DebugError("CPlayer::OnPacket Server=NULL PID=%d AID=%d id=%d len=%d",m_PID,m_AID,msgPack.m_XYID,msgPack.m_nLen);
			}
		}
		break;
	case GameXY::ReqTablePlayerBase::XY_ID:
	case GameXY::ReqRoomPlayerBase::XY_ID:
	case GameXY::ReqRoomPlayerState::XY_ID:
		{
			if ( m_pRoom && m_RoomID )
			{
				nRet = m_pRoom->OnRoomMsg(this,msgPack);
			}
			else
			{
				DebugError("CPlayer::OnPacket pRoom=%d RoomID=%d PID=%d AID=%d xyid=%d xylen=%d",
					m_pRoom,m_RoomID,m_PID,m_AID,msgPack.m_XYID,msgPack.m_nLen );
			}
		}
		break;
	case GameXY::ClientToServerMessage::XY_ID:
	case GameXY::ReqPlayerAct::XY_ID:
	case GameXY::ReqLeaveTable::XY_ID:
		{
			if( m_pTable && m_TableID )
			{
				nRet = m_pTable->OnTableMsg(this,msgPack);
			}
			else
			{
				DebugError("CPlayer::OnPacket PID=%d m_pRoom=%d m_RoomID=%d m_pTable=NULL TableID=%d msgid=%d msglen=%d",
					m_PID,m_pRoom,m_RoomID,m_TableID,msgPack.m_XYID,msgPack.m_nLen );

				if ( msgPack.m_XYID == GameXY::ClientToServerMessage::XY_ID )
				{
					GameXY::ClientToServerMessage CTSMsg;
					ExplainMsg(msgPack,CTSMsg);
					DebugError("CPlayer::OnPacket GameMsg XYID=%d XYLEN=%d",CTSMsg.m_MsgID,CTSMsg.m_MsgLen);
				}
				else if ( msgPack.m_XYID == GameXY::ReqPlayerAct::XY_ID )
				{
					GameXY::ReqPlayerAct rpa;
					TransplainMsg(msgPack,rpa);

					DebugError("PlayerAct Action=%d SitID=%d TableID=%d TakeMoney=%s",
						rpa.m_Action,rpa.m_SitID,rpa.m_TableID,N2S(rpa.m_nTakeMoney.get()).c_str());

					if ( m_pRoom )
					{
						CGameTable* pTable = m_pRoom->GetTableByID(rpa.m_TableID);
						if ( pTable )
						{
							PlayerPtr pPlayer = pTable->GetTablePlayerByPID(m_PID);
							if ( pPlayer )
							{
								DebugError("Player In Table PID=%d TableID=%d",pPlayer->GetPID(),pTable->GetTableID() );
							}
						}
					}

					GameXY::PlayerLeaveTableInfo lt;
					lt.m_RoomID  = m_RoomID;
					lt.m_TableID = rpa.m_TableID;
					lt.m_PID     = m_PID;
					SendMsg(lt);
				}
				else if ( msgPack.m_XYID == GameXY::ReqLeaveTable::XY_ID )
				{
					GameXY::ReqLeaveTable rlv;
					TransplainMsg(msgPack,rlv);

					DebugError("LeaveTable RoomID=%d TableID=%d pRoom=%d pTable=%d",rlv.m_RoomID,rlv.m_TableID,m_pRoom,m_pTable);

					if ( m_pRoom )
					{
						CGameTable* pTable = m_pRoom->GetTableByID(rlv.m_TableID);
						if ( pTable )
						{
							PlayerPtr pPlayer = pTable->GetTablePlayerByPID(m_PID);
							if ( pPlayer )
							{
								DebugError("Player In Table PID=%d TableID=%d",pPlayer->GetPID(),pTable->GetTableID() );
							}
						}
					}

					GameXY::PlayerLeaveTableInfo lt;
					lt.m_RoomID  = rlv.m_RoomID;
					lt.m_TableID = rlv.m_TableID;
					lt.m_PID     = m_PID;
					SendMsg(lt);
				}
			}
		}
		break;
	default:
		{
			DebugError("CPlayer::OnPacket 无法处理协议 id=%d len=%d",msgPack.m_XYID,msgPack.m_nLen);
		}
	}

	//DebugInfo("CPlayer::OnPacket End");

	return nRet;
}

void CPlayer::CheckGameInfo()
{
	DBServerXY::WDB_ReqCheckGameInfo msgCGI;
	msgCGI.m_AID              = m_AID;
	msgCGI.m_PID              = m_PID;
	msgCGI.m_nGameMoney       = m_nGameMoney;
	msgCGI.m_nMoGuiMoney      = m_nMoGuiMoney;
	msgCGI.m_nMatchJF         = m_nMatchJF;
	msgCGI.m_nJF              = m_nJF;
	msgCGI.m_nEP              = m_nEP;
	msgCGI.m_nUpperLimite     = m_nUpperLimite;
	msgCGI.m_nLowerLimite     = m_nLowerLimite;
	msgCGI.m_nWinTimes        = m_nWinTimes;
	msgCGI.m_nLossTimes       = m_nLossTimes;
	msgCGI.m_nGameTime        = m_nGameTime;
	msgCGI.m_RightLevel       = m_RightLevel;
	msgCGI.m_RightTimes       = m_RightTimes;

	PlayerSendMsgToDBServer(msgCGI);
}

void CPlayer::CheckWinTimes()
{
	int nWinTimes = m_nWinTimes;
	if ( nWinTimes >= 10 )
	{	
		FinishHonor(N_Honor::First_Win10);	
		if ( nWinTimes >= 300 )
		{			
			FinishHonor(N_Honor::Second_Win300);			
			if ( nWinTimes >= 1000 )
			{				
				FinishHonor(N_Honor::Third_Win1000);				
				if( nWinTimes >= 10000 )
				{					
					FinishHonor(N_Honor::Fouth_Win10000);					
				}							
			}
		}					
	}
}
void CPlayer::CheckPlayTimes()
{
	int nTotalPlayTimes = m_nWinTimes + m_nLossTimes;
	if ( nTotalPlayTimes >= 100 )
	{		
		FinishHonor(N_Honor::First_Play100);		
		if ( nTotalPlayTimes >= 1000 )
		{			
			FinishHonor(N_Honor::Second_Play1000);
			if ( nTotalPlayTimes >= 5000 )
			{				
				FinishHonor(N_Honor::Third_Play5000);
				if ( nTotalPlayTimes >= 20000 )
				{					
					FinishHonor(N_Honor::Fouth_Play20000);					
				}
			}
		}
	}
}
void CPlayer::CheckWinRecord(int nType)
{
	switch(nType)
	{
	case PAI_TYPE_THREE:
		{
			int nWinTypeTimes = m_WinRecord[PAI_TYPE_THREE-1];
			if ( nWinTypeTimes >= 1 )
			{				
				FinishHonor(N_Honor::First_ThreeWin1);				
				if ( nWinTypeTimes >= 20 )
				{					
					FinishHonor(N_Honor::Second_ThreeWin20);					
					if ( nWinTypeTimes >= 50 )
					{						
						FinishHonor(N_Honor::Third_ThreeWin50);						
						if ( nWinTypeTimes >= 100 )
						{							
							FinishHonor(N_Honor::Fouth_ThreeWin100);							
						}
					}
				}
			}
		}
		break;
	case PAI_TYPE_STR:
		{
			int nWinTypeTimes = m_WinRecord[PAI_TYPE_STR-1]; //顺子
			if ( nWinTypeTimes >= 1 )
			{				
				FinishHonor(N_Honor::First_HuaWin1);				
				if ( nWinTypeTimes >= 20 )
				{					
					FinishHonor(N_Honor::Second_StrWin20);					
					if ( nWinTypeTimes >= 50 )
					{						
						FinishHonor(N_Honor::Third_StrWin50);						
						if ( nWinTypeTimes >= 100 )
						{							
							FinishHonor(N_Honor::Fouth_StrWin100);							
						}
					}
				}
			}
		}
		break;
	case PAI_TYPE_HUA:
		{
			int nWinTypeTimes = m_WinRecord[PAI_TYPE_HUA-1]; //同花
			if ( nWinTypeTimes >= 1 )
			{				
				FinishHonor(N_Honor::First_HuaWin1);
				if ( nWinTypeTimes >= 20 )
				{					
					FinishHonor(N_Honor::Second_HuaWin20);					
					if ( nWinTypeTimes >= 50 )
					{						
						FinishHonor(N_Honor::Third_HuaWin50);						
						if ( nWinTypeTimes >= 100 )
						{							
							FinishHonor(N_Honor::Fouth_HuaWin100);							
						}
					}
				}
			}
		}
		break;
	case PAI_TYPE_THREETWO:
		{
			int nWinTypeTimes = m_WinRecord[PAI_TYPE_THREETWO-1]; //同花
			if ( nWinTypeTimes >= 1 )
			{				
				FinishHonor(N_Honor::First_ThreeTwoWin1);				
				if ( nWinTypeTimes >= 20 )
				{					
					FinishHonor(N_Honor::Second_ThreeTwoWin20);					
					if ( nWinTypeTimes >= 50 )
					{
						FinishHonor(N_Honor::Third_ThreeTwoWin50);						
						if ( nWinTypeTimes >= 100 )
						{							
							FinishHonor(N_Honor::Fouth_ThreeTwoWin100);							
						}
					}
				}
			}
		}
		break;
	case PAI_TYPE_FOUR:
		{
			int nWinTypeTimes = m_WinRecord[PAI_TYPE_FOUR-1];
			if ( nWinTypeTimes >= 1 )
			{				
				FinishHonor(N_Honor::First_FourWin1);				
				if ( nWinTypeTimes >= 5 )
				{					
					FinishHonor(N_Honor::Second_FourWin5);					
					if ( nWinTypeTimes >= 10 )
					{						
						FinishHonor(N_Honor::Third_FourWin10);						
						if ( nWinTypeTimes >= 30 )
						{							
							FinishHonor(N_Honor::Fouth_FourWin30);							
						}
					}
				}
			}
		}
		break;
	case PAI_TYPE_HUASTR:
		{
			int nWinTypeTimes = m_WinRecord[PAI_TYPE_HUASTR-1];
			if ( nWinTypeTimes >= 1 )
			{				
				FinishHonor(N_Honor::First_HuaStrWin1);				
				if ( nWinTypeTimes >= 5 )
				{					
					FinishHonor(N_Honor::Second_HuaStrWin5);					
					if ( nWinTypeTimes >= 10 )
					{						
						FinishHonor(N_Honor::Third_HuaStrWin10);						
						if ( nWinTypeTimes >= 30 )
						{							
							FinishHonor(N_Honor::Fouth_HuaStrWin30);							
						}
					}
				}
			}
		}
		break;
	case PAI_TYPE_KING:
		{
			int nWinTypeTimes = m_WinRecord[PAI_TYPE_KING-1];
			if ( nWinTypeTimes >= 1 )
			{				
				FinishHonor(N_Honor::Second_King1);				
				if ( nWinTypeTimes >= 5 )
				{					
					FinishHonor(N_Honor::Third_King5);					
					if ( nWinTypeTimes >= 20 )
					{						
						FinishHonor(N_Honor::Fouth_King20);						
					}
				}
			}
		}
		break;
	default:
		{
			//DebugError("CheckWinRecord PID=%d nType=%d",m_PID,nType);
		}
		break;
	}
}

void CPlayer::CheckSendGift()
{
	int nSendTimes = m_SendGiftTimes;
	if ( nSendTimes >=20 )
	{		
		FinishHonor(N_Honor::First_Send20);		
		if ( nSendTimes >= 200 )
		{			
			FinishHonor(N_Honor::Second_Send200);			
			if ( nSendTimes >= 1000 )
			{				
				FinishHonor(N_Honor::Third_Send1000);				
				if ( nSendTimes >= 10000 )
				{					
					FinishHonor(N_Honor::Fouth_Send10000);					
				}
			}
		}
	}	
}
void CPlayer::CheckRecvGift()
{
	int nRecvTimes = m_RecvGiftTimes;
	if ( nRecvTimes >= 20 )
	{		
		FinishHonor(N_Honor::First_Recv20);		
		if ( nRecvTimes >= 200 )
		{			
			FinishHonor(N_Honor::Second_Recv200);
			if ( nRecvTimes >= 1000 )
			{				
				FinishHonor(N_Honor::Third_Recv1000);
				if ( nRecvTimes >= 10000 )
				{					
					FinishHonor(N_Honor::Fouth_Recv10000);
				}
			}
		}
	}
}

void CPlayer::CheckShowFace()
{
	int nShowFaceTimes = m_ShowFaceTimes;
	if ( nShowFaceTimes >= 100 )
	{		
		FinishHonor(N_Honor::First_ShowFace100);
		if ( nShowFaceTimes >= 500 )
		{			
			FinishHonor(N_Honor::Second_ShowFace500);
			if ( nShowFaceTimes >= 2500 )
			{				
				FinishHonor(N_Honor::Third_ShowFace2500);
				if ( nShowFaceTimes >= 10000 )
				{					
					FinishHonor(N_Honor::Fouth_ShowFace10000);				
				}
			}			
		}
	}
}

void CPlayer::CheckFriend()
{
	int nFriendCount = 0;
	for ( ListPID::iterator itorPID=m_listFollow.begin();itorPID!=m_listFollow.end();++itorPID)
	{
		if ( find(m_listBeFollow.begin(),m_listBeFollow.end(),*itorPID) != m_listBeFollow.end() )
		{
			nFriendCount++;
		}
	}
	if( nFriendCount >= 1 )
	{
		FinishHonor(N_Honor::First_Friend1);
		if ( nFriendCount >= 30 )
		{
			FinishHonor(N_Honor::Second_Friend30);
			if ( nFriendCount >= 100)
			{
				FinishHonor(N_Honor::Third_Friend100);
				if ( nFriendCount >= 200 )
				{
					FinishHonor(N_Honor::Fouth_Friend200);					
				}
			}
		}		
	}
}

 void CPlayer::CheckWinMoney(INT64 nWinMoney)
 {
	 if ( nWinMoney >= 5000)
	 {
		 FinishHonor(N_Honor::First_WinMoney5K);
		 if ( nWinMoney >= 25000 )
		 {
			  FinishHonor(N_Honor::First_WinMoney25K);
			  if ( nWinMoney >= 50000)
			  {
				  FinishHonor(N_Honor::Seoncd_Win5W);
				  if ( nWinMoney >= 250000 )
				  {
					  FinishHonor(N_Honor::Seoncd_Win25W);
					  if ( nWinMoney >= 1000000 )
					  {
						 FinishHonor(N_Honor::Third_Win100W);
						 if ( nWinMoney >= 5000000 )
						 {
							  FinishHonor(N_Honor::Third_Win500W);
							  if ( nWinMoney >= 20000000 )
							  {
								  FinishHonor(N_Honor::Fouth_Win2000W);								  ;
								  if ( nWinMoney >= 200000000 )
								  {
									  FinishHonor(N_Honor::Fouth_Win20000W);
								  }
							  }
						 }
					  }
				  }
			  }
		 }
	 }
 }
 void CPlayer::CheckOwnMoneyHonor()
 {
	 INT64 nOwnMoney = m_nGameMoney;
	 if ( nOwnMoney >= 500000 )
	 {
		 FinishHonor(N_Honor::First_MoneyUp50W);
		 if ( nOwnMoney >= 1000000 )
		 {
			 FinishHonor(N_Honor::First_MoneyUp100W);
			 if ( nOwnMoney >= 10000000 )
			 {
				  FinishHonor(N_Honor::Second_MoneyUp1000W);
				  if ( nOwnMoney >= 100000000 )
				  {
					   FinishHonor(N_Honor::Third_MoneyUp10000W);
					   if ( nOwnMoney >= 1000000000 )
					   {
						   FinishHonor(N_Honor::Third_MoneyUp10E);
					   }
				  }
			 }
		 }
	 }
 }
 void CPlayer::CheckJuBaoPeng(int nType)
 {
	 if ( DezhouLib::IsValidJuBaoPaiType(nType) )
	 {
		 FinishHonor(N_Honor::Frist_WinJuBaoPeng);
		 if ( nType == PAI_TYPE_FOUR )
		 {
			  FinishHonor(N_Honor::Second_WinJuBaoPengFour);
		 }
		 else if ( nType == PAI_TYPE_HUASTR )
		 {
			 FinishHonor(N_Honor::Third_WinJuBaoPengHuaStr);
		 }
		 else if ( nType == PAI_TYPE_KING )
		 {
			 FinishHonor(N_Honor::Fouth_WinJuBaoPengKing);
		 }
	 }
 }

 void CPlayer::CheckContinuWin(int nContinuWin)
 {
	 if ( nContinuWin >= 3 )
	 {
		 FinishHonor(N_Honor::First_GoOnWin3);
		 if ( nContinuWin >= 6 )
		 {
			 FinishHonor(N_Honor::Seoncd_GoOnWin6);
			 if ( nContinuWin >= 10 && IsFinishAchieve(N_Honor::HonorSort_Tong) )
			 {
				 FinishHonor(N_Honor::Third_GoOnWin10);
				 if ( nContinuWin >= 15 && IsFinishAchieve(N_Honor::HonorSort_Ying) )
				 {
					 FinishHonor(N_Honor::Fouth_GoOnWin15);
				 }
			 }
		 }
	 }
 }

 void CPlayer::CheckPlayAndInviteAward()
 {
	 if ( m_HaveAwardInvite==0 && (m_TodayGameSecond>1800 || m_TodayGameCount>=20) )
	 {
		 m_HaveAwardInvite = 1;
		 m_ContinuPlay++;
		 CheckContinuPlay();

		 DBServerXY::GS_To_DBS_Flag msgGSF;
		 msgGSF.m_AID               = m_AID;
		 msgGSF.m_PID               = m_PID;
		 msgGSF.m_Flag              = msgGSF.AddContinuPlay;
		 msgGSF.m_nValue            = 1;
		 PlayerSendMsgToDBServer(msgGSF);

		 msgGSF.m_AID               = m_AID;
		 msgGSF.m_PID               = m_PID;
		 msgGSF.m_Flag              = msgGSF.AwardPlayerInvite;
		 msgGSF.m_nValue            = 100;
		 PlayerSendMsgToDBServer(msgGSF);
	 }
 }
 void CPlayer::CheckContinuPlay()
 {
	 int nContinuPlay = m_ContinuPlay;
	 if ( nContinuPlay >= 5 )
	 {
		 FinishHonor(N_Honor::First_Land5);
		 if ( nContinuPlay >= 10 )
		 {
			  FinishHonor(N_Honor::Second_Land10);
			  if ( nContinuPlay >= 15 )
			  {
				  FinishHonor(N_Honor::Third_Land15);
				  if ( nContinuPlay >= 30 )
				  {
					  FinishHonor(N_Honor::Fouth_Land30);
				  }				  
			  }			 
		 }
	 }
 }

void CPlayer::AddMoneyLog(INT64 nAddMoney,const string& strLog)
{
	if( N_CeShiLog::c_MoneyLog )
	{
		InterlockedIncrement(&s_nMoneyLogCount);
		m_vectorMoneyLog.push_back("GameMoney="+N2S(m_nGameMoney,10)+" nAddMoney="+N2S(nAddMoney,10)+" "+strLog);
	}
}

void CPlayer::ClearMoneyLog()
{
	if ( m_vectorMoneyLog.size() )
	{
		InterlockedExchangeAdd(&s_nMoneyLogCount,0-int(m_vectorMoneyLog.size()) );
		m_vectorMoneyLog.clear();
	}
}

