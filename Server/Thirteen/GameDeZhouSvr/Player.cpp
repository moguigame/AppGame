#include ".\player.h"

#include "gameroom.h"
#include "gametable.h"

PLAYERLIST CPlayer::s_pPlayerList;
int CPlayer::s_State[PLAYER_ST_END] = {0};

long CPlayer::s_nMoneyLogCount = 0;

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

	m_AID = 0;
	m_PID = 0;
	m_Sex = Sex_None;

	m_ChangeName = 0;
	m_NickName = "";
	m_HeadPicURL = "";
	m_HomePageURL = "";
	m_City = "";
	m_HeadID = 0;

	m_nBankMoney = 0;

	m_nMoGuiMoney = 0;
	m_nGameMoney = 0;

	m_nMedal = 0;
	m_Diamond = 0;
	m_GoldenPig = 0;
	m_PigFood = 0;
	
	m_nJF = 0;
	m_nEP = 0;
	m_nWinTimes = 0;
	m_nLossTimes = 0;

	m_nGameTime = 0;

	m_MaxPai = "";
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

	m_mapRoomTime.clear();
	m_listFollow.clear();
	m_listBeFollow.clear();
	m_FriendCount = 0;

	m_listReqFriend.clear();

	m_curMapUserGiftInfo.clear();
	m_outMapUserGiftInfo.clear();

	m_FreeFaceTime = 0;


	m_EndBotPlayerTime = 0;

	m_PromoteTimeStart = 0;
	m_TimeStep = 0;
	m_TimePass = 0;

	m_ContinuPlay = 0;
	m_ContinuLogin = 0;

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
					if ( m_pTable->GetTableState()==TABLE_ST_PLAYING && this->IsPlaying() )
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
bool CPlayer::GetOutUserGift(DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int GiftIdx)
{
	MapUserGiftInfo::iterator itorGI = m_outMapUserGiftInfo.find(GiftIdx);
	if ( itorGI != m_outMapUserGiftInfo.end() )
	{
		stDBUgi = itorGI->second;
		return true;
	}
	return false;
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
void CPlayer::CheckMaxPai()
{
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
	m_HeadID         = msgPD.m_HeadID;
	m_PlayerType     = msgPD.m_PlayerType;
	m_InvitePID      = msgPD.m_InvitePID;

	m_NickName       = msgPD.m_NickName;
	m_HeadPicURL     = msgPD.m_HeadPicURL;
	m_HomePageURL    = msgPD.m_HomePageURL;
	m_City           = msgPD.m_City;

	m_nGameMoney     = msgPD.m_nGameMoney;
	m_nMedal         = int(msgPD.m_nMedal);
	m_Diamond        = int(msgPD.m_Diamond);
	m_GoldenPig      = int(msgPD.m_GoldenPig);
	m_PigFood        = int(msgPD.m_PigFood);

	m_nJF            = msgPD.m_nJF;
	m_nEP            = msgPD.m_nEP;
	m_nWinTimes      = int(msgPD.m_nWinTimes);
	m_nLossTimes     = int(msgPD.m_nLossTimes);
	m_DrawnTimes     = int(msgPD.m_DrawnTimes);
	m_EscapeTimes    = int(msgPD.m_EscapeTimes);
	m_nGameTime      = int(msgPD.m_nGameTime);

	m_MaxPaiTime     = msgPD.m_MaxPaiTime;
	m_MaxMoney       = msgPD.m_MaxMoney;
	m_MaxMoneyTime   = msgPD.m_MaxMoneyTime;
	m_MaxWin         = msgPD.m_MaxWin;
	m_MaxWinTime     = msgPD.m_MaxWinTime;

	m_JoinTime       = msgPD.m_JoinTime;
	m_LandTime       = UINT32(time(NULL));

	m_ContinuLogin   = msgPD.m_ContinuLogin;
	m_ContinuPlay    = msgPD.m_ContinuPlay;

	ClearMoneyLog();
	AddMoneyLog(msgPD.m_nGameMoney,"SetPlayerData");
}

void CPlayer::GetPlayerData(GameXY::Game_PlayerData& game_msgPD)
{
	game_msgPD.m_AID            = m_AID;
	game_msgPD.m_PID            = m_PID;

	game_msgPD.m_Sex            = m_Sex;
	game_msgPD.m_HeadID         = m_HeadID;
	game_msgPD.m_PlayerType     = m_PlayerType;
	game_msgPD.m_InvitePID      = m_InvitePID;

	game_msgPD.m_NickName       = m_NickName;
	game_msgPD.m_HeadPicURL     = m_HeadPicURL;
	game_msgPD.m_HomePageURL    = m_HomePageURL;
	game_msgPD.m_City           = m_City;

	game_msgPD.m_nGameMoney     = m_nGameMoney;
	game_msgPD.m_nMedal         = m_nMedal;
	game_msgPD.m_Diamond        = m_Diamond;
	game_msgPD.m_GoldenPig      = m_GoldenPig;
	game_msgPD.m_PigFood        = m_PigFood;

	game_msgPD.m_nJF            = m_nJF;
	game_msgPD.m_nEP            = m_nEP;
	game_msgPD.m_nWinTimes      = m_nWinTimes;
	game_msgPD.m_nLossTimes     = m_nLossTimes;
	game_msgPD.m_DrawnTimes     = m_DrawnTimes;
	game_msgPD.m_EscapeTimes    = m_EscapeTimes;
	game_msgPD.m_nGameTime      = m_nGameTime;

	game_msgPD.m_MaxPaiTime     = m_MaxPaiTime;
	game_msgPD.m_MaxMoney       = m_MaxMoney;
	game_msgPD.m_MaxMoneyTime   = m_MaxMoneyTime;
	game_msgPD.m_MaxWin         = m_MaxWin;
	game_msgPD.m_MaxWinTime     = m_MaxWinTime;

	game_msgPD.m_JoinTime       = m_JoinTime;
	game_msgPD.m_LandTime       = m_LandTime;

	game_msgPD.m_RoomID         = m_RoomID;
	game_msgPD.m_TableID        = m_TableID;
	game_msgPD.m_SitID          = m_SitID;
	game_msgPD.m_PlayerState    = m_PlayerState;
}

void CPlayer::GetPlayerDataEx(GameXY::Game_PlayerDataEx& game_msgPDEx)
{
	//game_msgPDEx.m_AID          = m_AID;
	//game_msgPDEx.m_PID          = m_PID;

	//game_msgPDEx.m_MaxMoney     = m_MaxMoney;
	//game_msgPDEx.m_MaxMoneyTime = m_MaxMoneyTime;
	//game_msgPDEx.m_MaxPai       = m_MaxPai;
	//game_msgPDEx.m_MaxPaiTime   = m_MaxPaiTime;
	//game_msgPDEx.m_MaxWin       = m_MaxWin;
	//game_msgPDEx.m_MaxWinTime   = m_MaxWinTime;

	////memcpy(game_msgPDEx.m_WinRecord,m_WinRecord,sizeof(m_WinRecord));
	////memcpy(game_msgPDEx.m_Achieve,m_Achieve,sizeof(m_Achieve));

	//game_msgPDEx.m_VipLevel     = m_VipLevel;
	//game_msgPDEx.m_FriendCount  = m_FriendCount;
}

void CPlayer::GetPlayerTailInfo(GameXY::PlayerTailInfo& msgPTI)
{
	msgPTI.m_PID                = m_PID;

	msgPTI.m_JoinTime           = m_JoinTime;
	msgPTI.m_LandTime           = m_LandTime;
	msgPTI.m_maxOwnMoney        = m_MaxMoney;
	msgPTI.m_maxWinMoney        = m_MaxWin;

	msgPTI.m_FriendCount        = m_FriendCount;
	msgPTI.m_nWinTimes          = m_nWinTimes;
	msgPTI.m_nLossTimes         = m_nLossTimes;
	msgPTI.m_MaxPai             = m_MaxPai;
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

	m_nGameMoney     = bpd.m_nGameMoney;

	m_nJF            = bpd.m_nJF;
	m_nEP            = bpd.m_nEP;
	m_nWinTimes      = INT32(bpd.m_nWinTimes);
	m_nLossTimes     = INT32(bpd.m_nLossTimes);
	m_nGameTime      = INT32(bpd.m_nGameTime);

	m_MaxPai         = bpd.m_MaxPai;
	m_MaxPaiTime     = bpd.m_MaxPaiTime;
	m_MaxMoney       = bpd.m_MaxMoney;
	m_MaxMoneyTime   = bpd.m_MaxMoneyTime;
	m_MaxWin         = bpd.m_MaxWin;
	m_MaxWinTime     = bpd.m_MaxWinTime;

	m_JoinTime       = bpd.m_JoinTime;
	m_LandTime       = UINT32(time(NULL));
	m_InvitePID      = bpd.m_InvitePID;

	ClearMoneyLog();
	AddMoneyLog(bpd.m_nGameMoney,"SetBotPlayerData");
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
		//GameXY::Game_PlayerDataEx game_msgPDEx;
		//GetPlayerDataEx(game_msgPDEx);
		//SendMsg(game_msgPDEx);
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
	case GameXY::ReqLeaveRoom::XY_ID:
	case GameXY::ReqJoinTable::XY_ID:
	case GameXY::ReqChangeTable::XY_ID:
	case GameXY::ReqTailInfo::XY_ID:
	case GameXY::ChangeUserInfo::XY_ID:
	case GameXY::ReqPlayerAward::XY_ID:
	case GameXY::ReqTracePlayer::XY_ID:
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

void CPlayer::AddMoneyLog(INT64 nAddMoney,string strLog)
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

