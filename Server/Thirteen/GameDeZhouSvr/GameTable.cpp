#include ".\gametable.h"

#include "GameRoom.h"
#include "dezhougamelogic.h"
#include "CGameServerConfig.h"

CMapFunctionTime CGameTable::s_TableFuncTime;

void CGameTable::DebugError(const char* logstr,...)
{
	m_bTableGameError = true;

	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		string strTemp = "TableID=" + N2S(m_RoomID*10000+m_TableID,8) + " " + string(logbuf);
		Log_Text( LOGLEVEL_ERROR,strTemp.c_str() );
		printf_s("%s Error %s \n",Tool::GetTimeString(m_TableCurTime).c_str(),strTemp.c_str() );
	}
}

void CGameTable::DebugInfo(const char* logstr,...)
{
#ifdef LOG
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);	
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		string strTemp = "TableID=" + N2S(m_RoomID*10000+m_TableID,8) + " " + string(logbuf);
		Log_Text( LOGLEVEL_INFO,strTemp.c_str() );
	}
#endif
}

void  CGameTable::ReleaseInfo(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);	
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		string strTemp = "TableID=" + N2S(m_RoomID*10000+m_TableID,8) + " " + string(logbuf);
		Log_Text( LOGLEVEL_INFO,strTemp.c_str() );
	}
}

void CGameTable::TestPlayerNumber()
{
	if ( m_pGSConfig->CeshiMode() )
	{
		int SitNumber=0,SeeNumber=0;

		string strPlayer="";
		PlayerPtr pPlayer;
		for( MapPlayer::const_iterator itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();++itorPlayer )
		{
			pPlayer = itorPlayer->second;
			if ( pPlayer->IsSeeing() )
			{
				SeeNumber++;
			}
			else if ( pPlayer->IsPlayerSitOnTable() )
			{
				SitNumber++;
			}
			else
			{
				DebugError("TestPlayerNumber PID=%d TableID=%d SitID=%d State=%d",
					pPlayer->GetPID(),pPlayer->GetTableID(),pPlayer->GetSitID(),pPlayer->GetPlayerState() );
			}

			strPlayer += " PID=" + N2S(pPlayer->GetPID()) + " State=" + N2S(pPlayer->GetPlayerState());
		}

		bool bError = false;
		if ( SitNumber > m_MaxSitPlayerNumber )
		{
			bError = true;
		}
		if ( SeeNumber != (m_TablePlayers.size()-m_SitPlayers.size()) )
		{
			bError = true;		
		}
		if ( SitNumber+SeeNumber != int(m_TablePlayers.size()) )
		{
			bError = true;
		}
		if ( SitNumber != m_SitPlayers.size() )
		{
			bError = true;
		}

		if( bError )
		{
			DebugError("SitNumber=%d,m_MaxSitNumber=%d",SitNumber,m_MaxSitPlayerNumber);
			DebugError("SitNumber=%d,m_CurSitNumber=%d",SitNumber,m_SitPlayers.size());
			DebugError("SeeNumber=%d,m_CurSeeNumber=%d",SeeNumber,(m_TablePlayers.size()-m_SitPlayers.size()));
			DebugError("TotalNumber=%d TotalSize=%d",SitNumber+SeeNumber,int(m_TablePlayers.size()) );
			DebugError(strPlayer.c_str());
		}
	}
}

CGameTable::CGameTable(CGameRoom* pRoom,CreateTableInfo& cti)
{
	TraceStackPath logTP("CGameTable::CGameTable");
	CLogFuncTime lft(s_TableFuncTime,"CGameTable");

	if( pRoom )
	{
		m_pRoom                           = pRoom;
		m_pServer                         = pRoom->GetServer();

		m_pGSConfig                       = m_pServer->GetGSConfig();

		this->m_GameID                    = m_pRoom->GetGameID();
		this->m_ServerID                  = m_pRoom->GetServerID();
		this->m_RoomID                    = m_pRoom->GetRoomID();

		m_pGameLogic = NULL;

		InitTable(cti);
	}
	else
	{
		DebugError("CGameTable::CGameTable pRoom=NULL tableid=%d",cti.m_TableID);
	}
}

CGameTable::~CGameTable(void)
{
	m_TablePlayers.clear();
	safe_delete(m_pGameLogic);
}

int CGameTable::InitTable(CreateTableInfo& cti)
{
	TraceStackPath logTP("CGameTable::InitTable");
	CLogFuncTime lft(s_TableFuncTime,"InitTable");

	this->m_TableID                   = cti.m_TableID;
	this->m_TableName                 = cti.m_TableName;

	this->m_Password                  = cti.m_Password;
	this->m_TableRule                 = cti.m_TableRule;
	this->m_TableRuleEX               = cti.m_TableRuleEX;
	this->m_TableMatchRule            = cti.m_MatchRule;
	this->m_FoundByWho                = cti.m_FoundByWho;
	this->m_TableEndTime              = cti.m_EndTime;

	InitTableRule();
	
	m_TableState                = TABLE_ST_NONE;
	m_IsHaveCommonPlayer        = false;
	m_bPrivateFound             = false;
	m_bInUsed                   = true;
	m_bCountChange              = false;
	m_bTableGameError           = false;

	m_MaxInPlayerNumber         = MAX_PLAYER_IN_TABLE;
	m_MaxSitPlayerNumber        = MAX_PALYER_ON_TABLE;

	m_TimeToClearTable          = 0;

	m_TablePlayers.clear();
	m_SitPlayers.clear();

	m_TableTenMinute = UINT32(time(NULL)) + Tool::CRandom::Random_Int(200,600);

	if ( m_pGameLogic )
	{
		m_pGameLogic->InitGameLogic();
	}
	else
	{
		try
		{
			m_pGameLogic = new CDeZhouGameLogic(this);		
		}
		catch (...)
		{
			DebugError("创建游戏逻辑失败 tableid=%d bywho=%d",cti.m_TableID,cti.m_FoundByWho);
		}
	}

	return 0;
}

int CGameTable::InitTableRule()
{
	TraceStackPath logTP("CGameTable::InitTableRule");
	CLogFuncTime lft(s_TableFuncTime,"InitTableRule");

	//cti.m_TableRule = "tabletype=0;servicepay=1;minforgame=20;chiptime=15;waittime=3;maxin=250;maxsit=9;minplayer=2;";
	map<string,INT64> mapRule = Tool::GetRuleMapInData(m_TableRule,string(";"),string("="));
	map<string,INT64>::iterator itorRule;



	return 0;
}

bool CGameTable::IsTablePlayerFull()
{
	return int(m_TablePlayers.size()) >= m_MaxInPlayerNumber; 
}
bool CGameTable::IsNeedPassword() 
{
	return m_Password.length() > 0;
}
BYTE CGameTable::GetSitPlayerNumber() 
{
	return m_SitPlayers.size();
}
INT16 CGameTable::GetSeePlayerNumber()
{
	INT16 nSeePlayer = m_TablePlayers.size() - m_SitPlayers.size();
	if ( nSeePlayer < 0 )
	{
		nSeePlayer = 0;
		DebugError("GetSeePlayerNumber TablePlayer=%d SitPlayer=%d",m_TablePlayers.size(),m_SitPlayers.size());
	}
	return nSeePlayer;
}
INT16 CGameTable::GetTotalPlayer()
{
	return m_TablePlayers.size();
}
int CGameTable::GetEmptySitNumber()
{
	int nEmptySit = m_MaxSitPlayerNumber - m_SitPlayers.size();
	if ( nEmptySit < 0 )
	{
		nEmptySit = 0;
		DebugError("GetEmptySitNumber MaxSit=%d SitPlayer=%d",m_MaxSitPlayerNumber,m_SitPlayers.size() );
	}
	return m_MaxSitPlayerNumber-m_SitPlayers.size();
}
bool CGameTable::IsTableEmpty()
{
	return int(m_TablePlayers.size())==0;
}

void CGameTable::reportStartGame()
{
	m_TableState = TABLE_ST_PLAYING;
}
void CGameTable::reportEndGame()
{
	m_TableState = TABLE_ST_WAIT;
}

void CGameTable::reportEndTable()
{
	TraceStackPath logTP("CGameTable::reportEndTable");
	CLogFuncTime lft(s_TableFuncTime,"reportEndTable");

	DebugInfo("CGameTable::reportEndTable Start LeftTime=%d",GetTableLeftTime());

	m_TableState = TABLE_ST_END;
	m_bInUsed = false;

	m_FoundByWho = 0;

	vector<unsigned int> delBotPID;
	vector<unsigned int> leavePID;

	for ( MapPlayer::iterator itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();++itorPlayer)
	{
		if ( itorPlayer->second->IsBotPlayer() )
		{			
			delBotPID.push_back(itorPlayer->second->GetPID());
		}
		else
		{
			leavePID.push_back(itorPlayer->second->GetPID());
		}
	}
	if ( delBotPID.size() )
	{
		for (size_t i=0;i<delBotPID.size();++i)
		{
			RemoveBotPlayer(delBotPID[i]);
		}
	}
	if ( leavePID.size() )
	{
		for (size_t i=0;i<leavePID.size();++i)
		{
			PlayerLeaveTable(GetTablePlayerByPID(leavePID[i]),Action_Flag_SysLogic);
		}
	}

	assert(m_TablePlayers.size()==0);
	assert(m_SitPlayers.size()==0);
	if ( m_TablePlayers.size() )
	{
		m_TablePlayers.clear();
	}
	if ( m_SitPlayers.size() )
	{
		m_SitPlayers.clear();
	}

	DebugInfo("CGameTable::reportEndTable End TablePlayers=%d SitPlayers=%d",m_TablePlayers.size(),m_SitPlayers.size());
}

void CGameTable::AddTableTime(int nAddTime)
{
	if( nAddTime>0 && m_TableEndTime>0 )
	{
		m_TableEndTime += nAddTime;
	}
}

UINT32  CGameTable::GetTableLeftTime()
{
	UINT32 LeftTime = N_Time::Month_1;
	if ( m_TableEndTime > 0 )
	{
		LeftTime = max(0,m_TableEndTime-m_TableCurTime);
	}
	return LeftTime;
}

int CGameTable::GetEmptySitID(int FromSitID) const
{
	assert( FromSitID >=0 && FromSitID <= m_MaxSitPlayerNumber );

	if ( FromSitID>=0 && FromSitID<=this->m_MaxSitPlayerNumber )
	{
		BYTE retSitID,nCount;
		for(retSitID=FromSitID,nCount=0;nCount<m_MaxSitPlayerNumber;)
		{
			retSitID++;
			if ( retSitID > m_MaxSitPlayerNumber)
			{
				retSitID -= m_MaxSitPlayerNumber;
			}
			if ( IsSitIDEmpty(retSitID) )
			{
				return retSitID;
			}
			nCount++;
		}
	}

	return 0;
}
PlayerPtr CGameTable::GetSitPlayerBySitID(BYTE SitID) const
{
	if ( SitID <= m_MaxSitPlayerNumber )
	{
		MapSitPlayer::const_iterator itorSitPlayer = m_SitPlayers.find(SitID);
		if ( itorSitPlayer != m_SitPlayers.end() )
		{
			return itorSitPlayer->second;
		}
	}
	return NULL;
}
bool CGameTable::IsSitIDEmpty(BYTE SitID) const
{
	if ( SitID > 0 && SitID <= m_MaxSitPlayerNumber )
	{
		MapSitPlayer::const_iterator itorSitPlayer = m_SitPlayers.find(SitID);
		if ( itorSitPlayer == m_SitPlayers.end() )
		{
			return true;
		}
	}
	return false;
}
bool CGameTable::IsTableLogicStop()const
{
	return m_pGameLogic->IsGameLogicStop();
}
void CGameTable::StartTableLogic()
{
	m_pGameLogic->StartGameLogic();
}

void CGameTable::AddPlayerToTable(PlayerPtr pPlayer)
{
	if ( pPlayer )
	{
		MapPlayer::iterator itorPlayer = m_TablePlayers.find(pPlayer->GetPID());
		if ( itorPlayer == m_TablePlayers.end() )
		{
			m_TablePlayers.insert(make_pair(pPlayer->GetPID(),pPlayer));
		}
	}
}
void CGameTable::DeletePlayerFromTable(PlayerPtr pPlayer)
{
	if ( pPlayer )
	{
		MapPlayer::iterator itorPlayer = m_TablePlayers.find(pPlayer->GetPID());
		if ( itorPlayer != m_TablePlayers.end() )
		{
			m_TablePlayers.erase(itorPlayer);
		}
	}
}
bool CGameTable::AddSitPlayerBySitID( PlayerPtr pPlayer,BYTE SitID )
{
	if ( pPlayer && IsAvailSitID(SitID) )
	{
		for ( MapSitPlayer::iterator itorPlayer=m_SitPlayers.begin();itorPlayer!=m_SitPlayers.end();++itorPlayer )
		{
			if ( itorPlayer->second->GetPID() == pPlayer->GetPID() )
			{
				DebugError("AddSitPlayer 玩家已经坐在其它位置上了 TableID=%d pPlayer=%d SitID=%d OldSitID=%d",
					m_TableID,pPlayer->GetPID(),SitID,pPlayer->GetSitID() );
			}
		}

		MapSitPlayer::iterator itorSitPlayer = m_SitPlayers.find(SitID);
		if ( itorSitPlayer == m_SitPlayers.end() )
		{
			m_SitPlayers.insert(make_pair(SitID,pPlayer));
			return true;
		}
		else
		{
			DebugError("AddSitPlayer 位置上有其它玩家 TableID=%d pPlayer=%d SitID=%d OldPID=%d",
				m_TableID,pPlayer->GetPID(),SitID,itorSitPlayer->second->GetPID() );
		}
	}
	else
	{
		DebugError("AddSitPlayer pPlayerPID=%d SitID=%d",pPlayer->GetPID(),SitID);
	}
	return false;
}
bool CGameTable::DeleteSitPlayerBySitID(PlayerPtr pPlayer,BYTE SitID)
{
	if ( pPlayer && IsAvailSitID(SitID) )
	{
		MapSitPlayer::iterator itorSitPlayer = m_SitPlayers.find(SitID);
		if ( itorSitPlayer != m_SitPlayers.end() )
		{
			if ( itorSitPlayer->second->GetPID() == pPlayer->GetPID() )
			{
				m_SitPlayers.erase(itorSitPlayer);
				return true;
			}
			else
			{
				DebugError("DeleteSitPlayerBySitID PID=%d SitID=%d",pPlayer->GetPID(),SitID);
			}
		}
	}
	return false;
}

PlayerPtr CGameTable::GetTablePlayerByPID(UINT32 PID)const
{
	PlayerPtr retPlayer = NULL;
	MapPlayer::const_iterator itorPlayer = m_TablePlayers.find(PID);
	if ( itorPlayer != m_TablePlayers.end() )
	{
		retPlayer = itorPlayer->second;
	}
	return retPlayer;
}

PlayerPtr CGameTable::GetNextPlayer(PlayerPtr pPlayer)
{
	if ( pPlayer )
	{
		MapPlayer::iterator itorPlayer;
		for( itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();++itorPlayer )
		{
			if ( itorPlayer->second == pPlayer )
			{
				++itorPlayer;
				if ( itorPlayer != m_TablePlayers.end() )
				{
					return itorPlayer->second;		
				}
				break;
			}
		}
	}
	else
	{
		if ( m_TablePlayers.size() )
		{
			MapPlayer::iterator itorPlayer = m_TablePlayers.begin();
			if ( itorPlayer->second )
			{
				return itorPlayer->second;
			}
		}
	}

	return NULL;
}

int CGameTable::AddBotPlayer(INT64 nBigBlind)
{
	TraceStackPath logTP("CGameTable::AddBotPlayer");
	CLogFuncTime lft(s_TableFuncTime,"AddBotPlayer");
	
	//DebugInfo("AddBotPlayer Start...");

	PlayerPtr BotPlayer = m_pServer->GetBotPlayer(nBigBlind*500,nBigBlind*4000);
	if ( BotPlayer ) m_pServer->m_GetBotStep1++;
	if ( !BotPlayer )
	{
		BotPlayer = m_pServer->GetBotPlayer(nBigBlind*200,nBigBlind*20000,true);
		if ( BotPlayer )m_pServer->m_GetBotStep2++;
		else m_pServer->m_GetBotFailed++;
	}
	
	int SitID = GetEmptySitID(Tool::CRandom::Random_Int(1,m_MaxSitPlayerNumber));
	if ( SitID && BotPlayer )
	{
		bool bSuccess = false;
		if ( m_pRoom->PlayerJoinRoom(BotPlayer) && PlayerJoinTable(BotPlayer) )
		{
			BotPlayer->m_LandTime = m_TableCurTime - Tool::CRandom::Random_Int(600,3600);

			if ( PlayerSitDown(BotPlayer,SitID,0,Action_Flag_SysLogic) )
			{
				bSuccess = true;

				BotPlayer->m_EndBotPlayerTime = m_TableCurTime + CRandom::Random_Int(m_pGSConfig->m_cfBP.m_MinPlayTime,m_pGSConfig->m_cfBP.m_MaxPlayTime);
			}
		}

		if ( !bSuccess )
		{
			m_pRoom->PlayerLeaveRoom(BotPlayer);
			m_pServer->ResetBotPlayer(BotPlayer);

			DebugError("添加玩家失败 TableID=%d,SitID=%d",m_TableID,SitID);
		}
	}
	else
	{
		DebugError("No Bot RoomID=%d nBigBlind=%s SitID=%d",m_RoomID,Tool::N2S(nBigBlind).c_str(),SitID);
	}

	//DebugInfo("AddBotPlayer End...");

	return 0;
}
int CGameTable::RemoveBotPlayer(UINT32 PID)
{
	TraceStackPath logTP("CGameTable::RemoveBotPlayer");
	CLogFuncTime lft(s_TableFuncTime,"RemoveBotPlayer");
	
	DebugInfo("CGameTable::RemoveBotPlayer PID=%d",PID);

	MapPlayer::iterator itorPlayer = m_TablePlayers.find(PID);
	if ( itorPlayer != m_TablePlayers.end() )
	{
		m_pServer->ResetBotPlayer(itorPlayer->second);
	}
	return 0;
}

bool CGameTable::GetTableInfo(TableInfo& ti)
{
	ti.m_RoomID               = this->m_RoomID;
	ti.m_TableID              = this->m_TableID;
	ti.m_TableType            = this->m_pGameLogic->m_TableType;
	ti.m_TableName            = this->m_TableName;
	ti.m_TableRule            = m_pGameLogic->m_ClientTableRule;

	ti.m_MaxSitPlayerNumber   = this->m_MaxSitPlayerNumber;
	ti.m_CurSitPlayerNumber   = this->GetSitPlayerNumber();
	ti.m_CurStandPlayerNumber = this->GetSeePlayerNumber();

	return true;
}

bool CGameTable::OnTimer(UINT32 curTime)
{
	m_TableCurTime = curTime;
	m_pGameLogic->OnTimer(curTime);

	if ( m_TableCurTime >= m_TableTenMinute )
	{
		m_TableTenMinute = m_TableCurTime + Tool::CRandom::Random_Int(400,800);
		for (MapPlayer::iterator itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();++itorPlayer)
		{
			PlayerPtr pPlayer = itorPlayer->second;
			if ( pPlayer->GetTable()!=this || pPlayer->GetTableID()!=m_TableID )
			{
				DebugError("Find Error Player PID=%d SitID=%d State=%d RoomID=%d TableID=%d",pPlayer->GetPID(),pPlayer->GetSitID(),
					pPlayer->GetPlayerState(),pPlayer->GetRoomID(),pPlayer->GetTableID() );
			}
		}
	}

	return true;
}

void CGameTable::ClearAllPlayer()
{
	TraceStackPath logTP("CGameTable::ClearAllPlayer");
	CLogFuncTime lft(s_TableFuncTime,"ClearAllPlayer");
	
	DebugInfo("ClearAllPlayer RoomID=%d TableID=%d",m_pRoom->GetRoomID(),m_TableID );
	m_TimeToClearTable = 0;

	VectorPID vecBotPlayer,vecCommPlayer;
	for (MapPlayer::iterator itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();++itorPlayer )
	{
		PlayerPtr pTempPlayer = itorPlayer->second;
		if ( pTempPlayer->IsBotPlayer())
		{
			vecBotPlayer.push_back(pTempPlayer->GetPID());
		}
		else
		{
			vecCommPlayer.push_back(pTempPlayer->GetPID());
		}
	}

	if ( vecBotPlayer.size() )
	{
		for ( size_t nPos=0;nPos<vecBotPlayer.size();++nPos)
		{
			PlayerPtr pTempPlayer = GetTablePlayerByPID(vecBotPlayer[nPos]);
			if ( pTempPlayer )
			{
				m_pServer->ResetBotPlayer(pTempPlayer);
			}
		}		
	}
	if ( vecCommPlayer.size() )
	{
		for ( size_t nPos=0;nPos<vecCommPlayer.size();++nPos)
		{
			PlayerPtr pTempPlayer = GetTablePlayerByPID(vecCommPlayer[nPos]);
			if ( pTempPlayer )
			{
				PlayerLeaveTable(pTempPlayer);
			}
		}
	}

	if ( m_TablePlayers.size() || m_SitPlayers.size() )
	{
		DebugError("ClearAllPlayer() TablePlayerSize=%d SitPlayerSize=%d ",m_TablePlayers.size(),m_SitPlayers.size() );
	}

	if ( m_TablePlayers.size() )m_TablePlayers.clear();
	if ( m_SitPlayers.size() )m_SitPlayers.clear();
}

bool CGameTable::PutPlayerSitDown(PlayerPtr pPlayer,INT64 nTableMoney)
{
	TraceStackPath logTP("CGameTable::PutPlayerSitDown");
	CLogFuncTime lft(s_TableFuncTime,"PutPlayerSitDown");
	
	DebugInfo("PutPlayerSitDown PID=%d",pPlayer->GetPID());
	int SitID = GetEmptySitID();
	if ( IsAvailSitID(SitID) )
	{
		assert(pPlayer->GetTable()!=this);
		assert(pPlayer->GetTableID()!=m_TableID);
		if ( DoPlayerJoinTable(pPlayer) && PlayerSitDown(pPlayer,SitID,nTableMoney,Action_Flag_SysLogic) )
		{
			return true;
		}
		else
		{
			DebugError("PutPlayerSitDown Join Sit Failed TableID=%d Sit=%d PID=%d",m_TableID,SitID,pPlayer->GetPID());
		}
	}
	else
	{
		DebugError("PutPlayerSitDown TableID=%d Sit=%d PID=%d",m_TableID,SitID,pPlayer->GetPID());
	}

	return false;
}

bool CGameTable::PlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag)
{
	TraceStackPath logTP("CGameTable::PlayerSitDown");
	CLogFuncTime lft(s_TableFuncTime,"PlayerSitDown");
	
	DebugInfo("CGameTable::PlayerSitDown PID=%d SitID=%d Money=%s",pPlayer->GetPID(),SitID,Tool::N2S(nTakeMoney).c_str() );

	bool nRet = false;
	if ( pPlayer && IsAvailSitID(SitID) && pPlayer->GetTableID()==m_TableID )
	{
		m_bCountChange = true;

		GameXY::PlayerAct pa;
		pa.m_PID = pPlayer->GetPID();
		pa.m_Action = pa.SITDOWN;
		pa.m_SitID = SitID;
		pa.m_TableID = m_TableID;
		SendMsgToAllPlayerInTable(pa);

		AddSitPlayerBySitID(pPlayer,SitID);
		pPlayer->SetSitID(SitID);
		pPlayer->SetPlayerState(PLAYER_ST_WAIT);

		pPlayer->m_RoomStateChange = true;
		pPlayer->SetFriendStateChange(true);

		m_pGameLogic->SitDownPlayerAction(pPlayer);		
		m_pGameLogic->GamePlayerSitDown(pPlayer,SitID,nTakeMoney,Flag);
		
		nRet = true;
	}
	else
	{
		nRet = false;
		if ( pPlayer )
		{
			DebugError("CGameTable::PlayerSitDown PPID=%d PTableID=%d PSitID=%d TableID=%d SitID=%d",
				pPlayer->GetPID(),pPlayer->GetTableID(),pPlayer->GetSitID(),m_TableID,SitID );
		}
		else
		{
			DebugError("CGameTable::PlayerSitDown TableID=%d SitID=%d",m_TableID,SitID );
		}		
	}

	TestPlayerNumber();

	DebugInfo("CGameTable::PlayerSitDown End........");
	return nRet;
}

bool CGameTable::PlayerStandUp(PlayerPtr pPlayer,int SitID,int Flag)
{
	TraceStackPath logTP("CGameTable::PlayerStandUp");
	CLogFuncTime lft(s_TableFuncTime,"PlayerStandUp");
	
	DebugInfo("CGameTable::PlayerStandUp PID=%d SitID=%d PSitID=%d Flag=%d",pPlayer->GetPID(),SitID,pPlayer->GetSitID(),Flag);

	bool ret = false;
	PlayerPtr TempPlayer = GetSitPlayerBySitID(SitID);
	if ( pPlayer && TempPlayer && pPlayer==TempPlayer 
		&& IsAvailSitID(SitID) && pPlayer->IsPlayerSitOnTable()
		&& pPlayer->GetTableID() == m_TableID && pPlayer->GetSitID() == SitID )
	{
		m_bCountChange = true;

		GameXY::PlayerAct pa;
		pa.m_PID = pPlayer->GetPID();
		pa.m_TableID = pPlayer->GetTableID();
		pa.m_SitID = pPlayer->GetSitID();
		pa.m_Action = pa.STANDUP;
		SendMsgToAllPlayerInTable(pa);
		
		m_pGameLogic->GamePlayerStandUp(pPlayer,SitID,Flag);	
		m_pGameLogic->StandUpPlayerAction(pPlayer);

		DeleteSitPlayerBySitID(pPlayer,SitID);
		pPlayer->SetSitID(0);
		pPlayer->SetPlayerState( PLAYER_ST_SEEING );

		pPlayer->m_RoomStateChange = true;
		pPlayer->SetFriendStateChange(true);

		ret = true;
	}
	else
	{
		DebugError("CGameTable::PlayerStandUp tableid=%d sitid=%d psitid=%d PST=%d PID=%d",
			m_TableID,SitID,pPlayer->GetSitID(),pPlayer->GetPlayerState(),pPlayer->GetPID() );
		ret = false;
	}

	TestPlayerNumber();

	DebugInfo("CGameTable::PlayerStandUp End........");
	return ret;
}

void CGameTable::SendPlayerBaseInfoToPlayer(PlayerPtr pPlayer)
{
	if ( pPlayer && pPlayer->IsNotBotPlayer() )
	{
		PlayerBaseInfo msgPBI;
		PlayerStateInfo msgPSI;
		for(MapPlayer::iterator itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();itorPlayer++)
		{
			msgPBI.m_Flag = msgPBI.SameTable;
			itorPlayer->second->GetPlayerBaseInfo(msgPBI);

			msgPSI.m_Flag = msgPSI.SameTable;
			itorPlayer->second->GetPlayerStateInfo(msgPSI);
			
			pPlayer->SendMsg(msgPBI);
			pPlayer->SendMsg(msgPSI);
		}
	}
}

bool CGameTable::AttachPlayerTable(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameTable::AttachPlayerTable");
	CLogFuncTime lft(s_TableFuncTime,"AttachPlayerTable");
	
	DebugInfo("CGameTable::AttachPlayerTable Start PID=%d",pPlayer->GetPID());

	bool ret = false; 
	MapPlayer::iterator itorPlayer = m_TablePlayers.find(pPlayer->GetPID());
	if( itorPlayer == m_TablePlayers.end() )
	{
		pPlayer->LeaveTable();
		ret = false;

		DebugError("AttachPlayerTable PID=%d TableID=%d sit=%d",pPlayer->GetPID(),pPlayer->GetTableID(),pPlayer->GetSitID());		
	}
	else
	{
		if( pPlayer->GetRoomID()!=m_RoomID || pPlayer->GetTableID()!=m_TableID )
		{
			this->DebugError("AttachPlayerTable PlayerRoomID=%d PlayerTableID=%d RoomID=%d TableID=%d",
				pPlayer->GetRoomID(),pPlayer->GetTableID(),m_RoomID,m_TableID);
		}

		GameXY::RespJoinTable rsjt;
		rsjt.m_RoomID = pPlayer->GetRoomID();
		rsjt.m_TableID = pPlayer->GetTableID();
		rsjt.m_Flag = rsjt.SUCCESS;
		rsjt.m_Type = rsjt.ATTACH;
		pPlayer->SendMsg(rsjt);

		PlayerJoinTableInfo msgJR;
		msgJR.m_RoomID = m_RoomID;
		msgJR.m_TableID = m_TableID;
		msgJR.m_PID = pPlayer->GetPID();
		pPlayer->SendMsg(msgJR);

		PlayerBaseInfo MyInfo,OtherInfo;
		pPlayer->GetPlayerBaseInfo(MyInfo);
		MyInfo.m_Flag = MyInfo.TablePlayer;
		for(itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();itorPlayer++)
		{
			itorPlayer->second->SendMsg(MyInfo);

			if ( itorPlayer->second != pPlayer )
			{
				OtherInfo.ReSet();
				itorPlayer->second->GetPlayerBaseInfo(OtherInfo);
				OtherInfo.m_Flag = OtherInfo.TablePlayer;
				pPlayer->SendMsg(OtherInfo);
			}
		}

		m_pGameLogic->SendGameToPlayer(pPlayer);

		ret = true;
	}

	this->TestPlayerNumber();

	DebugInfo("CGameTable::AttachPlayerTable End");
	return ret;
}

bool CGameTable::DoPlayerJoinTable(PlayerPtr pPlayer)
{
	DebugInfo("CGameTable::DoPlayerJoinTable Start PID=%d",pPlayer->GetPID());
	if ( pPlayer->IsNotBotPlayer() )
	{
		GameXY::RespJoinTable rsjt;
		rsjt.m_RoomID    = m_RoomID;
		rsjt.m_TableID   = m_TableID;
		rsjt.m_Flag      = rsjt.SUCCESS;
		rsjt.m_Type      = rsjt.JOIN;
		pPlayer->SendMsg(rsjt);
	}
	return PlayerJoinTable(pPlayer);
}

bool CGameTable::PlayerJoinTable(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameTable::PlayerJoinTable");
	CLogFuncTime lft(s_TableFuncTime,"PlayerJoinTable");
	
	DebugInfo("CGameTable::PlayerJoinTable Start PID=%d",pPlayer->GetPID());

	m_bCountChange = true;

	if ( m_TableState == TABLE_ST_NONE )
	{
		m_TableState = TABLE_ST_WAIT;
	}

	if ( !m_IsHaveCommonPlayer && (pPlayer->IsNotBotPlayer()) )
	{
		m_IsHaveCommonPlayer = true;
	}

	AddPlayerToTable(pPlayer);
	pPlayer->JoinTable(this);
	pPlayer->SetPlayerState( PLAYER_ST_SEEING );

	if ( pPlayer->IsNeedSendMsg() )
	{
		PlayerJoinTableInfo msgJR;
		msgJR.m_RoomID = m_RoomID;
		msgJR.m_TableID = m_TableID;
		msgJR.m_PID = pPlayer->GetPID();
		pPlayer->SendMsg(msgJR);
	}

	pPlayer->m_RoomStateChange = true;
	pPlayer->SetFriendStateChange(true);
	pPlayer->m_EnterTableTimes++;

	PlayerBaseInfo MyInfo,OtherInfo;
	pPlayer->GetPlayerBaseInfo(MyInfo);
	MyInfo.m_Flag = MyInfo.TablePlayer;
	for(MapPlayer::iterator itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();itorPlayer++)
	{
		itorPlayer->second->SendMsg(MyInfo);
		if ( itorPlayer->second != pPlayer && pPlayer->IsNotBotPlayer() )
		{
			OtherInfo.ReSet();
			itorPlayer->second->GetPlayerBaseInfo(OtherInfo);
			OtherInfo.m_Flag = OtherInfo.TablePlayer;
			pPlayer->SendMsg(OtherInfo);
		}
	}

	if ( pPlayer->IsNotBotPlayer() )
	{
		m_pGameLogic->SendGameToPlayer(pPlayer);
	}

	this->TestPlayerNumber();

	DebugInfo("CGameTable::PlayerJoinTable End");
	return true;
}

bool CGameTable::PlayerLeaveTable(PlayerPtr pPlayer,int Flag)
{
	TraceStackPath logTP("CGameTable::PlayerLeaveTable");
	CLogFuncTime lft(s_TableFuncTime,"PlayerLeaveTable");
	
	DebugInfo("CGameTable::PlayerLeaveTable Start PID=%d",pPlayer->GetPID());

	m_bCountChange = true;

	bool ret = false;
	if ( pPlayer && pPlayer->GetTableID()==this->m_TableID && pPlayer->GetRoomID() == this->m_RoomID )
	{
		if ( pPlayer->IsPlayerSitOnTable() )
		{
			if( GetSitPlayerBySitID(pPlayer->GetSitID()) == pPlayer )
			{
				GameXY::PlayerAct msgPA;
				msgPA.m_PID     = pPlayer->GetPID();
				msgPA.m_Action  = msgPA.STANDUP;
				msgPA.m_SitID   = pPlayer->GetSitID();
				msgPA.m_TableID = m_TableID;
				SendMsgToAllPlayerInTable(msgPA);				
			
				m_pGameLogic->GamePlayerStandUp(pPlayer,pPlayer->GetSitID(),Flag);				
				m_pGameLogic->StandUpPlayerAction(pPlayer);
			}
			else
			{
				DebugError("PlayerLeaveTable PID=%d SitID=%d",pPlayer->GetPID(),pPlayer->GetSitID());
			}
		}

		DeleteSitPlayerBySitID(pPlayer,pPlayer->GetSitID());

		GameXY::PlayerLeaveTableInfo lt;
		lt.m_RoomID = this->m_RoomID;
		lt.m_TableID = this->m_TableID;
		lt.m_PID = pPlayer->GetPID();
		SendMsgToAllPlayerInTable(lt);

		DeletePlayerFromTable(pPlayer);
		pPlayer->LeaveTable();
		pPlayer->SetPlayerState( PLAYER_ST_ROOM );		

		pPlayer->m_RoomStateChange = true;
		pPlayer->SetFriendStateChange(true);

		if ( m_IsHaveCommonPlayer && (!pPlayer->IsBotPlayer()) )
		{
			m_IsHaveCommonPlayer = false;
			for (MapPlayer::iterator itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();itorPlayer++)
			{
				if (!itorPlayer->second->IsBotPlayer())
				{
					m_IsHaveCommonPlayer = true;
					break;
				}
			}
		}
		
		ret = true;
	}
	else
	{
		if ( pPlayer )
		{
			DebugError("PlayerLeaveTable PID=%d RoomID=%d PlayerRoomID=%d PlayerTableID=%d",
				pPlayer->GetPID(),m_RoomID,pPlayer->GetRoomID(),pPlayer->GetTableID());
		}
		else
		{
			DebugError("PlayerLeaveTable 玩家参数不对");
		}
		ret = false;
	}

	this->TestPlayerNumber();

	DebugInfo("CGameTable::PlayerLeaveTable End");
	return ret;
}

int CGameTable::OnTableMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CGameTable::OnTableMsg");
	CLogFuncTime lft(s_TableFuncTime,"OnTableMsg");
	
	//DebugInfo("OnTableMsg Start");

	int ret = 0;
	switch ( msgPack.m_XYID )
	{
	case GameXY::ReqPlayerAct::XY_ID:
		{
			ret = OnReqPlayerAct(pPlayer,msgPack);
		}
		break;
	case GameXY::ReqLeaveTable::XY_ID:
		{
			ret = OnPlayerLeaveTable(pPlayer,msgPack);
		}
		break;
	case GameXY::ClientToServerMessage::XY_ID:
		{
			ret = m_pGameLogic->OnGameMsg(pPlayer,msgPack);
		}
		break;
	default:
		{
			DebugError("CGameTable::OnTableMsg XYID=%d XYLen=%d",msgPack.m_XYID,msgPack.m_nLen);
		}
	}
	if ( ret )
	{
		DebugError("CGameTable::OnTableMsg Ret=%d XYID=%d XYLen=%d",ret,msgPack.m_XYID,msgPack.m_nLen);
	}

	//DebugInfo("OnTableMsg End");

	return ret;
}

int CGameTable::OnReqPlayerAct(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CGameTable::OnReqPlayerAct");
	CLogFuncTime lft(s_TableFuncTime,"OnReqPlayerAct");

	int nRet = 0;

	GameXY::ReqPlayerAct rpa;
	TransplainMsg(msgPack,rpa);

	DebugInfo("OnReqPlayerAct Start PID=%d SitID=%d Action=%d",pPlayer->GetPID(),pPlayer->GetSitID(),rpa.m_Action);

	if( pPlayer && rpa.m_TableID == m_TableID 
		&& rpa.m_TableID == pPlayer->GetTableID() && m_RoomID == pPlayer->GetRoomID() )
	{
		GameXY::RespPlayerAct rspa;
		rspa.m_Flag = rspa.ACTFAILED;
		rspa.m_ShowFlag = ShowMsg_None;
		rspa.m_strMsg   = "动作失败";

		INT64 nReqTakeMoney = rpa.m_nTakeMoney;
		if( IsAvailSitID(rpa.m_SitID) )
		{
			if( rpa.m_Action == rpa.SITDOWN )
			{
				CGameRoom* pRoom = m_pServer->GetRoomByID(m_RoomID);
				if ( pRoom )
				{
					if ( pRoom->PlayerCanPlay(pPlayer) )
					{
						if( GetSitPlayerBySitID(rpa.m_SitID) )
						{
							rspa.m_Flag     = rspa.SITISNOTEMPTY;
							rspa.m_ShowFlag = ShowMsg_Show;
							rspa.m_strMsg   = "当前座位已经有人了";
						}
						else if ( pPlayer->GetSitID() > 0 )
						{
							rspa.m_Flag = rspa.ACTFAILED;
							rspa.m_ShowFlag = ShowMsg_None;
							rspa.m_strMsg   = "您已经坐在位置上了";
						}
						else
						{
							//rspa.m_Flag = m_pGameLogic->CanSitDown(pPlayer,rpa.m_SitID,nReqTakeMoney);
							if ( rspa.m_Flag == GameXY::RespPlayerAct::SITISNOTEMPTY )
							{
								rspa.m_ShowFlag = ShowMsg_Show;
								rspa.m_strMsg   = "当前座位已经有人了";
							}
							else if ( rspa.m_Flag == GameXY::RespPlayerAct::OUTOFOWN )
							{
								rspa.m_ShowFlag = ShowMsg_Show;
								rspa.m_strMsg   = "您当前的游戏币不足，不能坐下游戏";
							}
						}
					}
					else
					{
						rspa.m_Flag     = rspa.VisitorCantPlay;
						rspa.m_ShowFlag = ShowMsg_Show;
						rspa.m_strMsg   = "游客不能在本桌游戏，请选择游客专场游戏";
					}
				}
			}
			else if( rpa.m_Action == rpa.STANDUP )
			{
				if( GetSitPlayerBySitID(rpa.m_SitID)==pPlayer )
				{
					rspa.m_Flag = rspa.SUCCESS;
				}
				else
				{
					rspa.m_Flag = rspa.NOTINSIT;
					rspa.m_ShowFlag = ShowMsg_Show;
					rspa.m_strMsg   = "你不在当前的位置上";
				}
			}
		}
		else
		{
			rspa.m_Flag = rspa.OUTOFSITID;
		}

		if ( rspa.m_Flag == GameXY::RespPlayerAct::OUTOFSITID )
		{
			rspa.m_ShowFlag = ShowMsg_Show;
			rspa.m_strMsg   = "您选择的座位不能坐下";
		}

		if ( rspa.m_Flag == GameXY::RespPlayerAct::SUCCESS )
		{
			rspa.m_ShowFlag = 0;
			rspa.m_strMsg   = "";
		}

		pPlayer->SendMsg(rspa);

		if( rspa.m_Flag == rspa.SUCCESS )
		{
			if( rpa.m_Action == rpa.SITDOWN )
			{
				PlayerSitDown(pPlayer,rpa.m_SitID,nReqTakeMoney);
			}
			else if(rpa.m_Action == rpa.STANDUP )
			{
				PlayerStandUp(pPlayer,rpa.m_SitID);
			}
		}
	}
	else
	{
		if ( pPlayer )
		{
			DebugError("PID=%d RoomID=%d TableID=%d p_Rid=%d p_Tid=%d",
				pPlayer->GetPID(),m_RoomID,m_TableID,pPlayer->GetRoomID(),pPlayer->GetTableID() );
		}
		nRet = 100;
	}

	return nRet;
}

int CGameTable::OnPlayerLeaveTable(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CGameTable::OnPlayerLeaveTable");
	CLogFuncTime lft(s_TableFuncTime,"OnPlayerLeaveTable");

	GameXY::ReqLeaveTable rlv;
	TransplainMsg(msgPack,rlv);

	DebugInfo("OnPlayerLeaveTable Start PID=%d",pPlayer->GetPID());

	if( pPlayer->GetRoomID() == rlv.m_RoomID && pPlayer->GetTableID() == rlv.m_TableID )
	{
		PlayerLeaveTable(pPlayer);
	}
	else
	{
		DebugError("CGameTable::OnPlayerLeaveTable:PID=%d RoomID=%d TableID=%d MsgRoomID=%d MsgTableID=%d",
			pPlayer->GetPID(),m_RoomID,pPlayer->GetTableID(),rlv.m_RoomID,rlv.m_TableID );
	}

	return 0;
}