#include ".\gameroom.h"

#include "server.h"
#include "gametable.h"

#include "CGameServerConfig.h"
#include "MatchRule.h"

CMapFunctionTime CGameRoom::s_RoomFuncTime;

CGameRoom::CGameRoom( CServer* pServer,CreateRoomInfo& cri )
{
	TraceStackPath logTP("CGameRoom::CGameRoom");
	CLogFuncTime lft(s_RoomFuncTime,"CGameRoom");
	DebugInfo("CGameRoom::CGameRoom Start");

	assert( pServer );

	if( pServer )
	{
		m_pServer = pServer;
		m_pGSConfig                   = pServer->GetGSConfig();
		m_CurRoomTime                 = pServer->GetCurTime();

		this->m_RoomID                = cri.m_RoomID;
		this->m_RoomName              = cri.m_RoomName;
		this->m_Password              = cri.m_Password;
		this->m_RoomRule              = cri.m_RoomRule;
		this->m_RoomRuleEX            = cri.m_RoomRuleEX;
		this->m_RoomMatchRule         = cri.m_MatchRule;
		this->m_AreaRule              = cri.m_AreaRule;

		m_vecForBidAreaSee.clear();
		m_vecAllowAreaSee.clear();
		m_vecForBidAreaPlay.clear();
		m_vecAllowAreaPlay.clear();

		m_ShowPlayers.clear();
		m_RoomPlayers.clear();

		m_setSignMatchPID.clear();
		m_vecMatchPlayerInfo.clear();

		for(MapListTable::iterator itorListTable=m_maplistTables.begin();itorListTable!=m_maplistTables.end();++itorListTable)
		{
			itorListTable->second.clear();
		}
		m_maplistTables.clear();

		map<string,string> mapAreaRule = Tool::GetRuleMapInString(m_AreaRule,string(";"),string("="));
		map<string,string>::iterator itorAreaRule;	

		itorAreaRule = mapAreaRule.find("forbidsee");
		if ( itorAreaRule != mapAreaRule.end() )
		{
			vector<string> vecStrB =  Tool::SplitString(itorAreaRule->second,",");		
			for ( int k=0;k<int(vecStrB.size());k++)
			{
				INT16 AreaID = atoi(vecStrB.at(k).c_str());
				m_vecForBidAreaSee.push_back(AreaID);
			}
		}

		itorAreaRule = mapAreaRule.find("allowsee");
		if ( itorAreaRule != mapAreaRule.end() )
		{
			vector<string> vecStrB =  Tool::SplitString(itorAreaRule->second,",");
			for ( int k=0;k<int(vecStrB.size());k++)
			{
				INT16 AreaID = atoi(vecStrB.at(k).c_str());
				m_vecAllowAreaSee.push_back(AreaID);
			}
		}

		itorAreaRule = mapAreaRule.find("forbidplay");
		if ( itorAreaRule != mapAreaRule.end() )
		{
			vector<string> vecStrB =  Tool::SplitString(itorAreaRule->second,",");
			for ( int k=0;k<int(vecStrB.size());k++)
			{
				INT16 AreaID = atoi(vecStrB.at(k).c_str());
				m_vecForBidAreaPlay.push_back(AreaID);
			}
		}
		itorAreaRule = mapAreaRule.find("allowplay");
		if ( itorAreaRule != mapAreaRule.end() )
		{
			vector<string> vecStrB =  Tool::SplitString(itorAreaRule->second,",");
			for ( int k=0;k<int(vecStrB.size());k++)
			{
				INT16 AreaID = atoi(vecStrB.at(k).c_str());
				m_vecAllowAreaPlay.push_back(AreaID);
			}
		}

		if ( m_vecForBidAreaSee.size()>0 && m_vecAllowAreaSee.size()>0 )
		{			
			DebugError("CreateRoom AllowSeeSize=%d ForbidSeeSize=%d",int(m_vecAllowAreaSee.size()),int(m_vecForBidAreaSee.size()) );
		}
		if ( m_vecForBidAreaPlay.size()>0 && m_vecAllowAreaPlay.size()>0 )
		{			
			DebugError("CreateRoom AllowPlaySize=%d ForbidPlaySize=%d",int(m_vecAllowAreaPlay.size()),int(m_vecForBidAreaPlay.size()) );
		}

		//m_RoomRule = "roomtype=1;place=101;maxtable=250;maxplayer=10000";
		map<string,INT64> mapRule = Tool::GetRuleMapInData(m_RoomRule,string(";"),string("="));
		map<string,INT64>::iterator itorRule;
		
		itorRule = mapRule.find("roomtype");
		if ( itorRule != mapRule.end())
		{
			m_RoomType = BYTE(itorRule->second);
		}
		else
		{
			DebugError("CreateTable m_RoomID=%d m_RoomType=%d",m_RoomID,m_RoomType);
		}

		itorRule = mapRule.find("place");
		if ( itorRule != mapRule.end())
		{
			m_RoomPlace = BYTE(itorRule->second);
		}
		else
		{
			DebugError("CreateTable m_RoomID=%d m_RoomPlace=%d",m_RoomID,m_RoomPlace);
		}

		m_MaxTableInRoom = MAX_TABLE_IN_ROOM;
		itorRule = mapRule.find("maxtable");
		if ( itorRule != mapRule.end())
		{
			m_MaxTableInRoom = UINT32(itorRule->second);
		}
		assert(m_MaxTableInRoom>=50 && m_MaxTableInRoom<=MAX_TABLE_IN_ROOM);

		m_MaxPlayerInRoom = MAX_PLAYER_IN_ROOM;
		itorRule = mapRule.find("maxplayer");
		if ( itorRule != mapRule.end())
		{
			m_MaxPlayerInRoom = UINT32(itorRule->second);
		}
		assert( m_MaxPlayerInRoom>=500 && m_MaxPlayerInRoom<=MAX_PLAYER_IN_ROOM );		

		m_WinLossRoom = 0;
		itorRule = mapRule.find("winloss");
		if ( itorRule != mapRule.end())
		{
			m_WinLossRoom = BYTE(itorRule->second);			
		}
		assert(m_WinLossRoom>=0);

		m_PrivateRoom = 0;
		itorRule = mapRule.find("private");
		if ( itorRule != mapRule.end())
		{
			m_PrivateRoom = BYTE(itorRule->second);
		}
		assert(m_PrivateRoom>=0);

		m_nMinPlayMoney = 0;
		itorRule = mapRule.find("minplaymoney");
		if ( itorRule != mapRule.end())
		{
			m_nMinPlayMoney = itorRule->second;
		}

		m_nMaxPlayMoney = 0;
		itorRule = mapRule.find("maxplaymoney");
		if ( itorRule != mapRule.end())
		{
			m_nMaxPlayMoney = itorRule->second;
		}

		m_MatchRoomState = MatchRoom_ST_None;
		m_pRoomMatchRule = NULL;
		if ( IsRoomMatch() )
		{
			m_MatchRoomState = MatchRoom_ST_WaitSign;
			m_pRoomMatchRule = new CMatchRule();

			mapRule = Tool::GetRuleMapInData(m_RoomMatchRule,string(";"),string("="));
			itorRule = mapRule.find("matchid");
			if ( itorRule != mapRule.end())
			{
				m_pRoomMatchRule->m_MatchID = UINT32(itorRule->second);
			}

			stMatchInfo stMI;
			m_pServer->GetMatchInfo(m_pRoomMatchRule->m_MatchID,stMI);

			m_pRoomMatchRule->m_MatchType     = stMI.m_MatchType;
			m_pRoomMatchRule->m_MatchTicket   = stMI.m_Ticket;
			m_pRoomMatchRule->m_TakeMoney     = stMI.m_TakeMoney;
			m_pRoomMatchRule->m_StartMoney    = stMI.m_StartMoney;

			m_pRoomMatchRule->m_vecBlind      = stMI.m_vecBlind;
			m_pRoomMatchRule->m_vecTime       = stMI.m_vecTime;
			m_pRoomMatchRule->m_vecAwardMoney = stMI.m_vecAwardMoney;
			m_pRoomMatchRule->m_vecAwardJF    = stMI.m_vecAwardJF;

			m_pRoomMatchRule->m_FirstInterval = stMI.m_FirstAwoke;
			m_pRoomMatchRule->m_SecondInterval= stMI.m_SecondAwoke;

			m_pRoomMatchRule->m_RegisterTime  = stMI.m_StartTime;
			m_pRoomMatchRule->m_IntervalTime  = stMI.m_StartInterval;

			m_pRoomMatchRule->m_StartSit      = stMI.m_StartSit;

			if ( !(stMI.m_StartTime>0 && stMI.m_StartInterval>0) )
			{
				DebugError(" RoomID=%d MatchID=%d StartTime=%d Interval=%d",
					m_RoomID,m_pRoomMatchRule->m_MatchID,stMI.m_StartTime,stMI.m_StartInterval );
			}

			m_pRoomMatchRule->InitRegisterTime(m_CurRoomTime);
			m_pRoomMatchRule->InitBlind(m_CurRoomTime);
		}

		m_CurPlayerInRoom       = 0;
		m_RealPlayerInRoom      = 0;
		m_BotPlayerInRoom       = 0;

		m_timeSendTablePC       = m_CurRoomTime;
		m_IntervalSendTablePC   = 60;

		m_timeCheckShow     = m_CurRoomTime;
		m_IntervalCheckShow = 10;

		m_timeSendRoomPS    = m_CurRoomTime;
		m_IntervalRoomPS    = 300;

		m_RankPlayerTime    = m_CurRoomTime;

		m_InervalRoomPC     = 120;
		m_timeSendRoomPC    = m_CurRoomTime;
	}
	else
	{
		DebugError("CGameRoom::CGameRoom 创建房间失败");
	}
	DebugInfo("CGameRoom::CGameRoom End");
}

CGameRoom::~CGameRoom(void)
{
	MapTable::iterator itorTable;
	for(itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++)
	{
		if( itorTable->second )
		{
			delete itorTable->second;
			itorTable->second = NULL;
		}
	}

	for(MapListTable::iterator itorListTable=m_maplistTables.begin();itorListTable!=m_maplistTables.end();++itorListTable)
	{
		itorListTable->second.clear();
	}
	m_maplistTables.clear();

	m_Tables.clear();
	m_RoomPlayers.clear();
	m_ShowPlayers.clear();

	m_vecForBidAreaSee.clear();
	m_vecAllowAreaSee.clear();
	m_vecForBidAreaPlay.clear();
	m_vecAllowAreaPlay.clear();

	m_setSignMatchPID.clear();
	m_vecMatchPlayerInfo.clear();
	safe_delete(m_pRoomMatchRule);
}

void CGameRoom::DebugError(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		string strTemp = "RoomID=" + N2S(m_RoomID) + " " + string(logbuf);
		Log_Text(LOGLEVEL_ERROR,strTemp.c_str() );
		printf_s("%s Error: Room %s \n",Tool::GetTimeString(m_CurRoomTime).c_str(),logbuf );
	}
}

void CGameRoom::DebugInfo(const char* logstr,...)
{
#ifdef LOG
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		string strTemp = "RoomID=" + N2S(m_RoomID) + " " + string(logbuf);
		Log_Text(LOGLEVEL_INFO,strTemp.c_str() );
	}
#endif
}

void CGameRoom::CeShiInfo(const char* logstr,...)
{
	if( m_pGSConfig->CeshiMode() )
	{
		static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
		va_list args;
		va_start(args, logstr);
		int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
		va_end(args);
		if (len>0 && len<=MAX_LOG_BUF_SIZE )
		{
			string strTemp = "RoomID=" + N2S(m_RoomID) + " " + string(logbuf);
			Log_Text( LOGLEVEL_INFO,strTemp.c_str() );
		}
	}
}

UINT16 CGameRoom::GetGameID() const
{
	return m_pServer->GetGameID();
}
UINT16 CGameRoom::GetServerID() const
{ 
	return m_pServer->GetServerID();
}
UINT16 CGameRoom::GetRoomID() const
{
	return m_RoomID;
}

CGameTable*  CGameRoom::CreateTable(CreateTableInfo& cti)
{
	TraceStackPath logTP("CGameRoom::CreateTable");
	CLogFuncTime lft(s_RoomFuncTime,"CreateTable");
	//DebugInfo("CGameRoom::CreateTable Start RoomID=%d TableID=%d",m_RoomID,cti.m_TableID);

	CGameTable* pTable = GetTableByID(cti.m_TableID);
	if( pTable == NULL )
	{
		try
		{
			//DebugError("CreateTable TableID=%d Rule=%s RuleEx=%s Match=%s",cti.m_TableID,cti.m_TableRule.c_str(),cti.m_TableRuleEX.c_str(),cti.m_MatchRule.c_str());
			pTable = new CGameTable(this,cti);
			if( pTable )
			{
				if ( m_Tables.size() < m_MaxTableInRoom )
				{
					m_Tables.insert( make_pair(cti.m_TableID,pTable) );
					if ( IsCommonRoom() || IsTableMatch() )        //普通房间桌子的分配方式
					{
						int TableIdx = pTable->GetTableID()/100+1;
						MapListTable::iterator itorTableList = m_maplistTables.find(TableIdx);
						if ( itorTableList == m_maplistTables.end() )
						{
							ListTable TempListTable;
							m_maplistTables.insert(make_pair(TableIdx,TempListTable));
							itorTableList = m_maplistTables.find(TableIdx);
						}
						itorTableList->second.push_back(pTable);
					}
				}
				else
				{
					safe_delete(pTable);
					DebugError("CGameRoom::CreateTable Room Is Full TableSize=%d RID=%d TID=%d",m_Tables.size(),GetRoomID(),cti.m_TableID);
				}				
			}
		}
		catch (...)
		{
			safe_delete(pTable);
			DebugError("CGameRoom::CreateTable Mem Out RID=%d TID=%d",GetRoomID(),cti.m_TableID);
		}
	}
	else
	{
		DebugError("CGameRoom::CreateTable 桌子已经存在  RID=%d TID=%d",GetRoomID(),cti.m_TableID );
	}

	//DebugInfo("CGameRoom::CreateTable End");
	return pTable;
}

string CGameRoom::GetClientRoomRule()
{
	string strRule = "";

	if ( IsRoomMatch() )
	{
		stringstream ssIn;
		ssIn<<"matchid="    <<m_pRoomMatchRule->m_MatchID  <<";"	
			<<"starttime="  <<m_pRoomMatchRule->m_RegisterTime <<";"
			<<"ticket="     <<m_pRoomMatchRule->m_MatchTicket <<";"
			<<endl;
		strRule = ssIn.str();
	}
	else
	{
		stringstream ssIn;
		ssIn<<"private="       <<int(this->m_PrivateRoom)  <<";"
			<<"minplaymoney="  <<m_nMinPlayMoney           <<";"
			<<"maxplaymoney="  <<m_nMaxPlayMoney           <<";"
			<<endl;
		strRule = ssIn.str();
	}

	return strRule;
}

bool CGameRoom::IsMatching()
{
	if ( m_pRoomMatchRule && IsRoomMatch() && m_pRoomMatchRule->IsMatching() )
	{
		return true;
	}
	return false;
}

void CGameRoom::InitMatchTable()
{
	TraceStackPath logTP("CGameRoom::InitMatchTable");

	assert(IsRoomMatch());

	int nTableCount = 0;
	if ( m_pRoomMatchRule->GetMatchType() == Match_Type_JingBiao )
	{
		nTableCount = m_pGSConfig->m_cfMatch.m_nOriginTable;
	}
	else
	{
		nTableCount = m_pGSConfig->m_cfMatch.m_nChampionTable;
	}

	for ( int nCount=0;nCount<nTableCount;++nCount)
	{
		CreateRoomMatchTable();
	}
}

bool CGameRoom::GetRoomInfo(RoomInfo& ri)
{
	ri.m_RoomID                = this->m_RoomID;
	ri.m_RoomName              = this->m_RoomName;
	ri.m_RoomRule              = GetClientRoomRule();

	ri.m_MaxPlayerInRoom       = this->m_MaxPlayerInRoom;
	ri.m_CurPlayerInRoom       = this->m_CurPlayerInRoom;

	ri.m_Place                 = this->m_RoomPlace;
	ri.m_RoomType              = this->m_RoomType;
	ri.m_NeedPassword          = this->IsNeedPassword();

	return true;
}

bool CGameRoom::GetRoomInfo(PlayerPtr pPlayer,RoomInfo& ri)
{
	if( PlayerCanSee(pPlayer) )
	{
		GetRoomInfo(ri);
		return true;
	}
	else
	{
		return false;
	}
}

int CGameRoom::GetCreateTableCount(UINT32 PID)
{
	TraceStackPath logTP("CGameRoom::GetCreateTableCount");

	int nCount = 0;
	if ( IsPrivateRoom() && PID>0 )
	{
		for( MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++)
		{
			CGameTable* pTable = itorTable->second;
			if ( pTable->IsTablePrivate() && pTable->IsTableInUsed() && pTable->GetFoundPID()==PID )
			{
				nCount++;
			}
		}
	}
	return nCount;
}

void CGameRoom::UpdateMatchRoomState()
{
	TraceStackPath logTP("CGameRoom::UpdateMatchRoomState");

	GameXY::MatchRoomState msgMRS;
	msgMRS.m_RoomID = m_RoomID;
	msgMRS.m_RoomState = m_MatchRoomState;
	m_pServer->SendMsgToAllPlayerInServer(msgMRS);
}

void CGameRoom::StartRoomMatch()
{
	TraceStackPath logTP("CGameRoom::StartRoomMatch");
	CLogFuncTime lft(s_RoomFuncTime,"StartRoomMatch");
	DebugInfo("StartRoomMatch Start...");
	m_MatchRoomState = MatchRoom_ST_MatchStart;
	UpdateMatchRoomState();

	m_setSignMatchPID.clear();
	m_vecMatchPlayerInfo.clear();

	m_pRoomMatchRule->InitBlind(m_CurRoomTime);
	m_pRoomMatchRule->m_MatchGUID = INT64(m_CurRoomTime-N_Time::Second_MoGuiBegin)*INT64(10000000000) + INT64(m_RoomID)*100000;

	m_pRoomMatchRule->m_bMatching = true;
	m_pRoomMatchRule->m_MatchStartTime = m_CurRoomTime;

	PlayerPtr pPlayer;
	int nCount = 0;
	for (MapPlayer::iterator itorPlayer=m_RoomPlayers.begin();itorPlayer!=m_RoomPlayers.end();++itorPlayer)
	{
		pPlayer = itorPlayer->second;
		assert( pPlayer->IsPlaying() );
		assert( pPlayer->GetRoom() && pPlayer->GetRoomID()==m_RoomID);
		assert( pPlayer->GetTableID() && pPlayer->GetTable() );
		assert( pPlayer->GetSitID() );

		++nCount;
		stMatchPlayerRank stRMPI;
		stRMPI.m_AID = pPlayer->GetAID();
		stRMPI.m_PID = pPlayer->GetPID();
		stRMPI.m_TableMoney = m_pRoomMatchRule->GetStartMoney();
		stRMPI.m_Position = nCount;
		m_vecMatchPlayerInfo.push_back(stRMPI);

		assert(m_vecMatchPlayerInfo.size() == m_RoomPlayers.size());
	}

	for( MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++ )
	{
		CGameTable* pTable = itorTable->second;
		pTable->StartTableLogic();
	}
	DebugInfo("StartRoomMatch End...");
}
void CGameRoom::EndRoomMatch()
{
	TraceStackPath logTP("CGameRoom::EndRoomMatch");

	m_MatchRoomState = MatchRoom_ST_MatchOver;
	UpdateMatchRoomState();
}
bool CGameRoom::CheckChampTable()
{
	if ( IsRoomMatch() && m_pRoomMatchRule->IsMatching() )
	{
		int TableCount = 0;
		for( MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++)
		{
			if ( itorTable->second->GetSitPlayerNumber() )
			{
				++TableCount;
			}
		}
		if ( TableCount>1 )
		{
			return false;
		}
		else if ( TableCount == 1 )
		{
			return true;
		}
		else
		{
			DebugError("RoomID=%d MatchID=%d",m_RoomID,m_pRoomMatchRule->GetMatchID() );
		}
	}
	return false;
}

int CGameRoom::PlayerJoinMatch(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::PlayerJoinMatch");
	CLogFuncTime lft(s_RoomFuncTime,"PlayerJoinMatch");
	
	DebugInfo("PlayerJoinMatch Start...PID=%d",pPlayer->GetPID());
	if ( !IsPlayerInRoom(pPlayer) )
	{
		PlayerJoinRoom(pPlayer);
	}
	
	for( MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++ )
	{
		CGameTable* pTable = itorTable->second;
		int SitPlayerNumber = pTable->GetSitPlayerNumber();
		if ( SitPlayerNumber < m_pRoomMatchRule->m_StartSit )
		{
			pTable->PutPlayerSitDown(pPlayer,m_pRoomMatchRule->GetStartMoney());
			break;
		}
	}
	DebugInfo("PlayerJoinMatch End");
	return 0;
}

void CGameRoom::AssignMatchPlayer()
{
	TraceStackPath logTP("CGameRoom::AssignMatchPlayer");
	CLogFuncTime lft(s_RoomFuncTime,"AssignMatchPlayer");
	
	for( MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++ )
	{
		CGameTable* pTable = itorTable->second;
		assert(pTable);
		
		if ( pTable->IsTableLogicStop() && pTable->GetSitPlayerNumber()>0 )
		{
			int nTablePlayerNumber = pTable->GetSitPlayerNumber();
			vector<PlayerPtr> vecPlayer;
			vector<INT64> vecMoney;
			pTable->GetPlayerTableMoney(vecPlayer,vecMoney);
			assert( nTablePlayerNumber==vecMoney.size() && nTablePlayerNumber==vecPlayer.size() );

			vector<UINT16> vecTableID;		
			for ( int CurSit=MAX_PALYER_ON_TABLE-1;CurSit>0;--CurSit )   //空桌每桌一人
			{
				for ( MapTable::iterator itorTempTable=m_Tables.begin();itorTempTable!=m_Tables.end();itorTempTable++ )
				{
					CGameTable* pTempTable = itorTempTable->second;
					int TempCount = pTempTable->GetSitPlayerNumber();
					if ( pTempTable == pTable || TempCount==0 )
					{
						continue;
					}

					if ( pTempTable->GetSitPlayerNumber() == CurSit )
					{
						vecTableID.push_back(pTempTable->GetTableID());
						if ( int(vecTableID.size()) >= nTablePlayerNumber )
						{
							CurSit = 0;
							break;
						}
					}
				}
			}

			if ( int(vecTableID.size())>0 && int(vecTableID.size())<nTablePlayerNumber )
			{
				//不足部分由空桌里的多余位置补齐
				vector<UINT16> TempVecTableID = vecTableID;
				for( int nCanAdd=1;nCanAdd<=MAX_PALYER_ON_TABLE-1;++nCanAdd )
				{
					for (UINT32 Pos=0;Pos<TempVecTableID.size();++Pos)
					{
						CGameTable* pTempTable = GetTableByID( TempVecTableID[Pos] );
						assert(pTempTable);
						if ( pTempTable )
						{
							if ( MAX_PALYER_ON_TABLE - pTempTable->GetSitPlayerNumber() - nCanAdd > 0 )
							{
								vecTableID.push_back(pTempTable->GetTableID());
								if ( int(vecTableID.size()) >= nTablePlayerNumber )
								{
									nCanAdd = MAX_PALYER_ON_TABLE+1;
									break;
								}
							}
						}
					}
				}
			}

			if ( int(vecTableID.size()) >= nTablePlayerNumber )
			{
				for (int Pos=0;Pos<nTablePlayerNumber;++Pos)
				{
					PlayerPtr pPlayer = vecPlayer[Pos];
					CGameTable* pTempTable = GetTableByID( vecTableID[Pos] );
					CGameTable* pPlayerTable = pPlayer->GetTable();

					pPlayerTable->PlayerLeaveTable(pPlayer,Action_Flag_Match);
					pTempTable->PutPlayerSitDown(pPlayer,vecMoney[Pos]);

					if ( pTempTable->IsTableLogicStop() )    //避免两次转移分配玩家
					{
						pTempTable->StartTableLogic();
					}
				}
			}
			else
			{
				if ( nTablePlayerNumber >= 2 )
				{
					pTable->StartTableLogic();
				}
				else
				{
					DebugError("AssignMatchPlayer TableID=%d nCount=%d",pTable->GetTableID(),nTablePlayerNumber);
				}
			}
		}
	}
}

bool CGameRoom::SignUpPlayer(UINT32 PID)
{
	SetPID::iterator itorPID = m_setSignMatchPID.find(PID);
	if ( itorPID == m_setSignMatchPID.end() )
	{
		m_setSignMatchPID.insert(PID);
		return true;
	}
	return false;
}
bool CGameRoom::SignDownPlayer(UINT32 PID)
{
	SetPID::iterator itorPID = m_setSignMatchPID.find(PID);
	if ( itorPID != m_setSignMatchPID.end() )
	{
		m_setSignMatchPID.erase(PID);
		return true;
	}
	return false;
}
bool CGameRoom::IsPlayerSignedMatch(UINT32 PID)
{
	return ( m_setSignMatchPID.find(PID) != m_setSignMatchPID.end() );
}
bool CGameRoom::IsPlayerInRoom(PlayerPtr pPlayer)
{
	assert(pPlayer);
	if ( pPlayer )
	{
		return (m_RoomPlayers.find(pPlayer->GetPID()) != m_RoomPlayers.end());
	}
	return false;
}

void CGameRoom::FirstAwoke()
{
	TraceStackPath logTP("CGameRoom::FirstAwoke");

	assert( IsRoomMatch() && m_pRoomMatchRule );
	assert(m_MatchRoomState == MatchRoom_ST_WaitEnter);

	GameXY::AwokeMatch msgAM;
	msgAM.m_RoomID = m_RoomID;
	msgAM.m_MatchID = m_pRoomMatchRule->GetMatchID();
	msgAM.m_nTime = m_pRoomMatchRule->m_RegisterTime-m_CurRoomTime;

	for ( SetPID::iterator itorPID=m_setSignMatchPID.begin();itorPID!=m_setSignMatchPID.end();++itorPID )
	{
		PlayerPtr pPlayer = m_pServer->GetPlayerByPID(*itorPID);
		if ( pPlayer && (!IsPlayerInRoom(pPlayer)) )
		{
			pPlayer->SendMsg(msgAM);
		}
	}
}
void CGameRoom::SecondAwoke()
{
	TraceStackPath logTP("CGameRoom::SecondAwoke");

	assert( IsRoomMatch() && m_pRoomMatchRule );
	assert(m_MatchRoomState == MatchRoom_ST_WaitEnter);

	GameXY::AwokeMatch msgAM;
	msgAM.m_RoomID = m_RoomID;
	msgAM.m_MatchID = m_pRoomMatchRule->GetMatchID();
	msgAM.m_nTime = m_pRoomMatchRule->m_RegisterTime - m_CurRoomTime;

	for ( SetPID::iterator itorPID=m_setSignMatchPID.begin();itorPID!=m_setSignMatchPID.end();++itorPID )
	{
		PlayerPtr pPlayer = m_pServer->GetPlayerByPID(*itorPID);
		if ( pPlayer && (!IsPlayerInRoom(pPlayer)) )
		{			
			pPlayer->SendMsg(msgAM);			
		}
	}
}

CGameTable* CGameRoom::CreateRoomMatchTable()
{
	TraceStackPath logTP("CGameRoom::CreateRoomMatchTable");
	CLogFuncTime lft(s_RoomFuncTime,"CreateRoomMatchTable");
	assert( IsRoomMatch() && m_pRoomMatchRule );

	CGameTable* pTable = NULL;
	UINT16 TableID = GetNoUseID();

	if ( TableID != 0 )
	{
		CreateTableInfo  stCTI;
		stCTI.m_TableID = TableID;
		stCTI.m_EndTime = 0;
		stCTI.m_FoundByWho = 0;
		stCTI.m_Password = "";
		stCTI.m_TableName = "比赛桌"+Tool::N2S(TableID);
		stCTI.m_TableRule = "";
		stCTI.m_TableRuleEX = "";
		stCTI.m_MatchRule = "";

		pTable = CreateTable(stCTI);
	}
	return pTable;
}

void CGameRoom::AddPlayerMatchTableMoney(UINT32 PID,int TableMoney)
{
	assert( IsRoomMatch() && m_pRoomMatchRule );

	bool bSuccess = false;
	for (VectorMatchPlayerRank::iterator itorPI=m_vecMatchPlayerInfo.begin();itorPI!=m_vecMatchPlayerInfo.end();++itorPI)
	{
		if ( itorPI->m_PID == PID )
		{
			bSuccess = true;
			itorPI->m_TableMoney += TableMoney;
			break;
		}
	}
	if ( !bSuccess )
	{
		DebugError("AddPlayerMatchTableMoney PID=%d AddTableMoney=%d",PID,TableMoney);
	}
}

int CGameRoom::GetMatchID() const
{
	if ( IsRoomMatch() )
	{
		if ( m_pRoomMatchRule )
		{
			return m_pRoomMatchRule->GetMatchID();
		}
		assert(0);
	}
	return 0;
}

bool CGameRoom::GetMatchRoolState(UINT32 PID,MatchRoomState& msgMRS)
{
	if ( IsRoomMatch() )
	{
		assert(m_pRoomMatchRule);
		msgMRS.m_RoomID = m_RoomID;
		if ( m_MatchRoomState == MatchRoom_ST_WaitSign )
		{
			msgMRS.m_RoomState = MatchRoom_ST_WaitSign;
			if ( IsPlayerSignedMatch(PID) )
			{
				msgMRS.m_RoomState = MatchRoom_ST_HaveSign;
			}
		}
		else if ( m_MatchRoomState == MatchRoom_ST_WaitEnter )
		{
			msgMRS.m_RoomState = MatchRoom_ST_WaitSign;
			if ( IsPlayerSignedMatch(PID) )
			{
				msgMRS.m_RoomState = MatchRoom_ST_WaitEnter;
			}
		}
		else
		{
			msgMRS.m_RoomState = m_MatchRoomState;
		}
		assert( msgMRS.m_RoomState != MatchRoom_ST_None );

		return true;
	}
	return false;
}

bool CGameRoom::OnTimer(UINT32 curTime)
{
	TraceStackPath logTP("CGameRoom::OnTimer");
	CLogFuncTime lft(s_RoomFuncTime,"CGameRoom::OnTimer");
	//CeShiInfo("CGameRoom::OnTimer Start RoomID=%d",m_RoomID);

	m_CurRoomTime = curTime;
	if ( m_CurRoomTime >= m_timeSendTablePC + m_IntervalSendTablePC )
	{
		m_timeSendTablePC = m_CurRoomTime;
		OnTimeSendTablePlayerCount();
	}

	if ( m_CurRoomTime >= m_timeSendRoomPS + m_IntervalRoomPS )
	{
		m_timeSendRoomPS = m_CurRoomTime;
		OnTimeSendRoomPlayerState();
	}

	if ( m_CurRoomTime >= m_timeSendRoomPC + m_InervalRoomPC )
	{
		m_timeSendRoomPC = m_CurRoomTime;
		if ( IsRoomMatch() )
		{
			if ( IsMatching() )
				m_InervalRoomPC = 10;
			else
			{
				if ( m_CurRoomTime >= m_pRoomMatchRule->m_FirstAwoke )
					m_InervalRoomPC = 10;
				else
					m_InervalRoomPC = 120;
			}
		}
		OnTimeSendRoomPlayerCount();
	}

	if ( m_CurRoomTime >= m_timeCheckShow + m_IntervalCheckShow )
	{
		m_timeCheckShow = m_CurRoomTime;

		if ( IsCommonRoom() )
		{
			SetShowHideTable();
		}
		else if ( IsTableMatch() )
		{
			SetTableMatchTableState();
		}		
	}

	if ( IsRoomMatch() )
	{
		if ( m_pRoomMatchRule->IsMatching() )
		{
			if ( m_CurRoomTime - m_RankPlayerTime >= 30 )
			{
				m_RankPlayerTime = m_CurRoomTime;

				CompareMatchPlayer stCMP;
				sort(m_vecMatchPlayerInfo.begin(),m_vecMatchPlayerInfo.end(),stCMP);

				if ( m_pGSConfig->CeshiMode() )
				{
					INT64 MatchMoney = 0;
					for ( VectorMatchPlayerRank::iterator itorPI=m_vecMatchPlayerInfo.begin();itorPI!=m_vecMatchPlayerInfo.end();++itorPI )
					{
						MatchMoney += itorPI->m_TableMoney;
					}
					if ( MatchMoney != m_pRoomMatchRule->m_TotalMatchMoney - m_pRoomMatchRule->m_TotalLossMoney )
					{
						DebugError("matchmoney=%s,Totalmoney=%s lossMoney=%s",Tool::N2S(MatchMoney).c_str(),
							Tool::N2S(m_pRoomMatchRule->m_TotalMatchMoney).c_str(),Tool::N2S(m_pRoomMatchRule->m_TotalLossMoney).c_str() );
					}
				}
			}

			AssignMatchPlayer();

			if ( m_pRoomMatchRule->AddBlind(m_CurRoomTime) )
			{
				for(MapTable::iterator itorTables=m_Tables.begin();itorTables!=m_Tables.end();itorTables++)
				{
					itorTables->second->OnChangeBlind(m_pRoomMatchRule->m_BigBlind,m_pRoomMatchRule->m_SmallBlind);
				}
			}			
		}
		else
		{
			if ( m_CurRoomTime >= m_pRoomMatchRule->m_RegisterTime )
			{
				assert(m_MatchRoomState == MatchRoom_ST_WaitEnter);
				StartRoomMatch();
			}
			else if ( m_CurRoomTime >= m_pRoomMatchRule->m_FirstAwoke )
			{
				m_MatchRoomState = MatchRoom_ST_WaitEnter;
				m_pRoomMatchRule->m_FirstAwoke = N_Time::Max_Time;
				FirstAwoke();
			}
			else if ( m_CurRoomTime >= m_pRoomMatchRule->m_SecondAwoke )
			{
				m_MatchRoomState = MatchRoom_ST_WaitEnter;
				m_pRoomMatchRule->m_SecondAwoke = N_Time::Max_Time;
				SecondAwoke();
			}
		}
	}

	for(MapTable::iterator itorTables=m_Tables.begin();itorTables!=m_Tables.end();itorTables++)
	{
		itorTables->second->OnTimer(curTime);
	}

	//CeShiInfo("CGameRoom::OnTimer End RoomID=%d",m_RoomID);
	return true;
}

UINT32 CGameRoom::GetTime() const
{
	return m_CurRoomTime;
}

CGameTable* CGameRoom::GetTableByID(UINT16 tableid)
{
	MapTable::iterator itorTable = m_Tables.find(tableid);
	if(itorTable!=m_Tables.end())
	{
		return itorTable->second;
	}
	return NULL;
}
CGameTable* CGameRoom::GetNoUseTable()
{	
	for (MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();++itorTable)
	{
		if ( !(itorTable->second->IsTableInUsed()) )
		{
			return itorTable->second;
		}
	}
	return NULL;
}

UINT16 CGameRoom::GetNoUseID()
{
	for (;;)
	{
		UINT16 TableID = Tool::CRandom::Random_Int(1,MAX_TALBE_ID);
		if ( GetTableByID(TableID) == NULL )
		{
			return TableID;
		}
	}
	return 0;
}

void CGameRoom::Clear()
{
	m_RoomPlayers.clear();
}

bool CGameRoom::PlayerCanSee(PlayerPtr pPlayer)
{
	vector<INT16>::iterator itorAreaID;

	if( m_vecForBidAreaSee.size() > 0 )
	{
		itorAreaID = find(m_vecForBidAreaSee.begin(),m_vecForBidAreaSee.end(),pPlayer->GetAID());
		if( itorAreaID != m_vecForBidAreaSee.end() )
		{
			return false;
		}
		else
		{
			return true;
		}
	}	

	if( m_vecAllowAreaSee.size() > 0 )
	{
		itorAreaID = find(m_vecAllowAreaSee.begin(),m_vecAllowAreaSee.end(),pPlayer->GetAID());
		if ( itorAreaID != m_vecAllowAreaSee.end() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}	

	return true;
}

bool CGameRoom::PlayerCanPlay(PlayerPtr pPlayer)
{
	vector<INT16>::iterator itorAreaID;

	if( m_vecForBidAreaPlay.size() > 0 )
	{
		itorAreaID = find(m_vecForBidAreaPlay.begin(),m_vecForBidAreaPlay.end(),pPlayer->GetAID());
		if( itorAreaID != m_vecForBidAreaPlay.end() )
		{
			return false;
		}
		else
		{
			return true;
		}
	}	

	if( m_vecAllowAreaPlay.size() > 0 )
	{
		itorAreaID = find(m_vecAllowAreaPlay.begin(),m_vecAllowAreaPlay.end(),pPlayer->GetAID());
		if ( itorAreaID != m_vecAllowAreaPlay.end() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}	

	return true;
}

void CGameRoom::SetTableMatchTableState()
{
	TraceStackPath logTP("CGameRoom::SetTableMatchTableState");
	CLogFuncTime lft(s_RoomFuncTime,"SetTableMatchTableState");

	for ( MapListTable::iterator itorListTable=m_maplistTables.begin();itorListTable!=m_maplistTables.end();++itorListTable)
	{
		ListTable TempListTable = itorListTable->second;

		int nShowCount=0,nWaitCount=0,nPlayingCount=0,nMatchOverCount=0;
		CGameTable *pUnshowTable=NULL,*pMatchOverTable=NULL,*pTempTable=NULL;
		for (ListTable::iterator itorTable=TempListTable.begin();itorTable!=TempListTable.end();++itorTable)
		{
			pTempTable = *itorTable;
			if ( pTempTable->IsTableShow() )
			{
				nShowCount++;
				if ( pTempTable->GetTableMatchState() == MatchTable_ST_WaitSign )
				{
					nWaitCount++;
				}
				else if ( pTempTable->GetTableMatchState() == MatchTable_ST_Matching )
				{
					nPlayingCount++;
				}
				else
				{
					nMatchOverCount++;
					pMatchOverTable = pTempTable;
				}
			}
			else if ( !pUnshowTable )
			{
				pUnshowTable = pTempTable;
			}
		}

		assert(nShowCount>0);
		assert(nWaitCount+nPlayingCount+nMatchOverCount>0);

		if ( nWaitCount != 1 ) //保证有一个在等待报名
		{
			if ( nWaitCount > 1 )
			{
				DebugError("SetTableMatchTableState 超过一个桌子在等待中 TableListId=%d",itorListTable->first);
			}
			else
			{
				if ( nMatchOverCount > 0 )
				{
					pMatchOverTable->ClearAllPlayer();
					pMatchOverTable->InitMatchTableData();
					pMatchOverTable->SetTableMatchState(MatchTable_ST_WaitSign);
					m_pServer->ChangeMatchTableState(pMatchOverTable);
				}
				else
				{
					if ( pUnshowTable )
					{
						pUnshowTable->ClearAllPlayer();
						pUnshowTable->InitMatchTableData();
						m_pServer->ShowTable(pUnshowTable);
						pUnshowTable->SetTableMatchState(MatchTable_ST_WaitSign);
						m_pServer->ChangeMatchTableState(pUnshowTable);
					}
					else
					{
						DebugError("SetTableMatchTableState 桌子用完 TableListId=%d nShowCount=%d nWaitCount=%d nPlayingCount=%d nMatchOverCount=%d",
							itorListTable->first,nShowCount,nWaitCount,nPlayingCount,nMatchOverCount);
					}
				}
			}
		}

		if ( nShowCount>5 && nMatchOverCount>=3 )    //去掉多余的桌子
		{
			for ( ListTable::iterator itorTable=TempListTable.begin();itorTable!=TempListTable.end();++itorTable )
			{
				pTempTable = *itorTable;
				if ( pTempTable->IsTableShow() && (!pTempTable->IsTableRuleShow())
					&& (pTempTable->GetTableMatchState()==MatchTable_ST_None || pTempTable->GetTableMatchState()==MatchTable_ST_MatchOver) )
				{
					m_pServer->HideTable(pTempTable);
					break;
				}
			}
		}
	}
}

void CGameRoom::SetShowHideTable()
{
	TraceStackPath logTP("CGameRoom::SetShowHideTable");
	CLogFuncTime lft(s_RoomFuncTime,"SetShowHideTable");

	for ( MapListTable::iterator itorListTable=m_maplistTables.begin();itorListTable!=m_maplistTables.end();++itorListTable)
	{
		ListTable TempListTable = itorListTable->second;
		int EmptySit = 0;
		int EmptyTable = 0;

		CGameTable *pUnshowTable=NULL,*pEmptyTable=NULL,*pTempTable=NULL;
		for (ListTable::iterator itorTable=TempListTable.begin();itorTable!=TempListTable.end();++itorTable)
		{
			pTempTable = *itorTable;
			if ( pTempTable->IsTableShow() )
			{
				EmptySit += pTempTable->GetEmptySitNumber();
				if ( pTempTable->IsTableEmpty() )
				{
					EmptyTable++;

					if ( !pTempTable->IsTableRuleShow() )
					{
						pEmptyTable = pTempTable;
					}						
				}
			}
			else if ( !pUnshowTable )
			{
				pUnshowTable = pTempTable;
			}
		}
		if ( EmptySit < 15 && EmptyTable == 0 && pUnshowTable )
		{
			//pUnshowTable->ClearAllPlayer();
			m_pServer->ShowTable(pUnshowTable);
		}
		if ( EmptySit > 30 && EmptyTable > 1 && pEmptyTable )
		{
			m_pServer->HideTable(pEmptyTable);
		}
	}
}

int  CGameRoom::OnRoomMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CGameRoom::OnRoomMsg");
	CLogFuncTime lft(s_RoomFuncTime,"OnRoomMsg");

	int ret = 0;
	switch ( msgPack.m_XYID )
	{
	case ReqRoomPlayerBase::XY_ID:
		{
			//ret = OnReqRoomPlayerBase(pPlayer,msgPack);
		}
	break;
	case ReqRoomPlayerState::XY_ID:
		{
			//ret = OnReqRoomPlayerState(pPlayer,msgPack);
		}
		break;
	case ReqTablePlayerBase::XY_ID:
		{
			ret = OnReqTablePlayerBase(pPlayer,msgPack);
		}
		break;
	default:
		{
			DebugError("OnRoomMsg 未处理协议 xyid=%d xylen=%d",msgPack.m_XYID,msgPack.m_nLen);
		}
	}
	return ret;
}

int CGameRoom::OnReqTablePlayerBase(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CGameRoom::OnReqTablePlayerBase");
	CLogFuncTime lft(s_RoomFuncTime,"OnReqTablePlayerBase");
	CeShiInfo("CGameRoom::OnReqTablePlayerBase Start");

	if ( IsMatchRoom() ) return 0;

	GameXY::ReqTablePlayerBase msgRTPB;
	TransplainMsg(msgPack,msgRTPB);

	if ( pPlayer && pPlayer->GetRoomID()==msgRTPB.m_RoomID )
	{
		CGameTable* pTable = GetTableByID(msgRTPB.m_TableID);
		if ( pTable )
		{
			pTable->SendPlayerBaseInfoToPlayer(pPlayer);
		}
	}

	CeShiInfo("CGameRoom::OnReqTablePlayerBase End");

	return 0;
}

int CGameRoom::OnReqRoomPlayerBase(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CGameRoom::OnReqRoomPlayerBase");
	CeShiInfo("CGameRoom::OnReqRoomPlayerBase Start");

	GameXY::ReqRoomPlayerBase msgRPB;
	TransplainMsg(msgPack,msgRPB);

	if ( pPlayer && pPlayer->GetRoomID()==msgRPB.m_RoomID )
	{
		if ( pPlayer->m_RegisterRoomPlayerTime == 0 )
		{
			pPlayer->m_RegisterRoomPlayerTime = m_CurRoomTime;
			SendRoomPlayerToPlayer(pPlayer);
		}
	}

	CeShiInfo("CGameRoom::OnReqRoomPlayerBase End");

	return 0;
}

int CGameRoom::OnReqMatchRank(PlayerPtr pPlayer,GameXY::ReqMatchRank& msgMR)
{
	TraceStackPath logTP("CGameRoom::OnReqMatchRank");
	if ( IsRoomMatch() && IsMatching() && pPlayer->CanReqMatchRank(m_CurRoomTime)
		&& msgMR.m_EndRank>msgMR.m_StartRank && msgMR.m_StartRank>0 )
	{
		if ( msgMR.m_EndRank - msgMR.m_StartRank < msgMR.MaxReq )
		{
			GameXY::RespMatchRank msgRespMR;
			MatchRankInfo stMR;

			if ( pPlayer->IsInTable() ) msgRespMR.m_Flag = msgRespMR.TableGame;
			else msgRespMR.m_Flag = msgRespMR.LobbyRoom;

			msgRespMR.m_RoomID = m_RoomID;
			for ( int Pos=msgMR.m_StartRank-1;Pos<msgMR.m_EndRank && Pos<int(m_vecMatchPlayerInfo.size());++Pos)
			{
				stMatchPlayerRank& r_RMPI = m_vecMatchPlayerInfo[Pos];
				if ( r_RMPI.m_TableMoney <= 0 )
				{
					continue;
				}
				stMR.m_AID           = r_RMPI.m_AID;
				stMR.m_PID           = r_RMPI.m_PID;
				stMR.m_TableMoney    = r_RMPI.m_TableMoney;
				stMR.m_Position      = r_RMPI.m_Position;
				msgRespMR.m_vectorMatchRank.push_back(stMR);
			}
			pPlayer->SendMsg(msgRespMR);
		}
	}
	
	return 0;
}

int CGameRoom::OnReqRoomPlayerState(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	return 0;
}

int  CGameRoom::OnReqJoinTable(PlayerPtr pPlayer,GameXY::ReqJoinTable& rjt)
{
	TraceStackPath logTP("CGameRoom::OnReqJoinTable");
	CLogFuncTime lft(s_RoomFuncTime,"OnReqJoinTable");
	CeShiInfo("CGameRoom::onReqJoinTable start ReqPID=%d",pPlayer->GetPID());

	GameXY::RespJoinTable rsjt;
	CGameTable* pTable = GetTableByID( rjt.m_TableID );
	if( pTable )
	{
		if( pTable->IsTablePlayerFull() )
		{
			rsjt.m_Flag      = rsjt.PLAYERFULL;
			rsjt.m_ShowFlag  = ShowMsg_Show;
			rsjt.m_strMsg    = "当前桌子人数已满";
		}
		else if( pTable->IsNeedPassword() && (string(rjt.m_Password) != pTable->GetTablePassword()) )
		{
			rsjt.m_Flag      = rsjt.PASSWORDERROR;
			rsjt.m_ShowFlag  = ShowMsg_Show;
			rsjt.m_strMsg    = "桌子密码错误";
		}
		else if ( pTable->GetTableLeftTime() <= 10 )  //该桌游戏即将结束
		{
			rsjt.m_Flag      = rsjt.TABLEEND;
			rsjt.m_ShowFlag  = ShowMsg_Show;
			rsjt.m_strMsg    = "本桌游戏时间已到期，请选择其它桌子游戏";
		}
		else
		{
			rsjt.m_Flag     = rsjt.SUCCESS;
			rsjt.m_RoomID   = rjt.m_RoomID;
			rsjt.m_TableID  = rjt.m_TableID;

			if( pPlayer->GetTableID() > 0 )
			{
				rsjt.m_Type = rsjt.CHANGE;
				assert(pPlayer->GetTable());
			}
			else
			{
				rsjt.m_Type = rsjt.JOIN;
				assert(!pPlayer->GetTable());
			}
		}
	}
	else
	{
		rsjt.m_Flag      = rsjt.NOTABLE;
		rsjt.m_ShowFlag  = ShowMsg_Show;
		rsjt.m_strMsg    = "加入错误的桌子";
	}

	pPlayer->SendMsg(rsjt);

	CeShiInfo("CGameRoom::onReqJoinTable end");

	if( rsjt.m_Flag == rsjt.SUCCESS )
	{
		CGameTable* pTable;
		if( pPlayer->GetTableID() > 0 )
		{
			pTable = GetTableByID(pPlayer->GetTableID());
			if( pTable )
			{
				pTable->PlayerLeaveTable(pPlayer);
			}
		}

		pTable = GetTableByID(rjt.m_TableID);
		if( pTable )
		{
			pTable->PlayerJoinTable(pPlayer);
		}

		return 0;
	}
	else
	{
		return 10000+rsjt.m_Flag;
	}
}

int CGameRoom::SendTablePlayerCount(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::SendTablePlayerCount");
	CLogFuncTime lft(s_RoomFuncTime,"SendTablePlayerCount");
	CeShiInfo("CGameRoom::SendTablePlayerCount Start");

	if ( pPlayer )
	{
		if ( !pPlayer->IsNeedSendMsg() )
		{
			return 0;
		}

		if ( pPlayer->GetSendTime(m_RoomID)+m_IntervalSendTablePC > m_CurRoomTime )
		{
			return 0;
		}

		if ( IsRoomMatch() )
		{
			return 0;
		}

		msgTablePC tpc;
		TablePlayerCountList msgTPCL;
		msgTPCL.m_RoomID = m_RoomID;

		CGameTable* pTempTable;
		for( MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++)
		{
			pTempTable = itorTable->second;
			if ( pTempTable->IsTableShow() )
			{
				tpc.ReSet();
				tpc.m_TableID             = pTempTable->GetTableID();
				tpc.m_nPlayingPlayerCount = pTempTable->GetSitPlayerNumber();
				tpc.m_nSeeingPlayerCount  = pTempTable->GetSeePlayerNumber();

				msgTPCL.m_listTablePC.push_back(tpc);

				if ( msgTPCL.m_listTablePC.size() >= msgTPCL.MAX_TABLEPALYERCOUNTLIST)
				{
					pPlayer->SendMsg(msgTPCL);
					msgTPCL.m_listTablePC.clear();
				}
			}
		}
		if ( msgTPCL.m_listTablePC.size() )
		{
			pPlayer->SendMsg(msgTPCL);
		}		
		pPlayer->SetSendTime(m_RoomID,m_CurRoomTime);
	}

	CeShiInfo("CGameRoom::SendTablePlayerCount End");
	return 0;
}
int CGameRoom::OnTimeSendTablePlayerCount()
{
	if ( IsRoomMatch() ) return 0;

	TraceStackPath logTP("CGameRoom::OnTimeSendTablePlayerCount");
	CLogFuncTime lft(s_RoomFuncTime,"OnTimeSendTablePlayerCount");
	//CeShiInfo("CGameRoom::OnTimeSendTablePlayerCount Start");

	msgTablePC tpc;
	TablePlayerCountList msgTPCL;
	msgTPCL.m_RoomID = m_RoomID;
	
	CGameTable* pTempTable;
	for( MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++)
	{
		pTempTable = itorTable->second;
		if ( pTempTable->IsTableShow() && pTempTable->m_bCountChange )
		{
			pTempTable->m_bCountChange = false;

			tpc.ReSet();
			tpc.m_TableID             = pTempTable->GetTableID();
			tpc.m_nPlayingPlayerCount = pTempTable->GetSitPlayerNumber();
			tpc.m_nSeeingPlayerCount  = pTempTable->GetSeePlayerNumber();

			msgTPCL.m_listTablePC.push_back(tpc);

			if ( msgTPCL.m_listTablePC.size() >= msgTPCL.MAX_TABLEPALYERCOUNTLIST )
			{
				break;
			}
		}
	}

	if ( msgTPCL.m_listTablePC.size() > 0 )
	{
		CeShiInfo("OnTimeSendTablePlayerCount RoomID=%d TableSize=%d",m_RoomID,msgTPCL.m_listTablePC.size());

		PlayerPtr TempPlayer;
		for (MapPlayer::iterator itorPlayer=m_RoomPlayers.begin();itorPlayer!=m_RoomPlayers.end();++itorPlayer)
		{
			TempPlayer = itorPlayer->second;
			if ( TempPlayer->IsNeedSendMsg() && (!TempPlayer->IsInTable()) )
			{
				TempPlayer->SendMsg(msgTPCL);
				TempPlayer->SetSendTime(m_RoomID,m_CurRoomTime);
			}
		}
	}

	//下次下发的时间由当前房间内玩家的数量来决定
	int PlayerCount = m_RealPlayerInRoom;
	for (int Idx=0;Idx<int(m_pGSConfig->m_cfRoom.m_vecPN.size());Idx++)
	{
		if ( PlayerCount <= m_pGSConfig->m_cfRoom.m_vecPN.at(Idx) )
		{
			m_IntervalSendTablePC = m_pGSConfig->m_cfRoom.m_vecSendTime.at(Idx);
			break;
		}
	}

	//CeShiInfo("CGameRoom::OnTimeSendTablePlayerCount End");
	return 0;
}

int CGameRoom::OnTimeSendRoomPlayerCount()
{
	TraceStackPath logTP("CGameRoom::OnTimeSendRoomPlayerCount");
	CLogFuncTime lft(s_RoomFuncTime,"OnTimeSendRoomPlayerCount");
	//CeShiInfo("CGameRoom::OnTimeSendRoomPlayerCount Start");

	if ( IsMatchRoom() )
	{
		MatchRoomPlayerCount msgMRPC;
		msgMRPC.m_RoomID = m_RoomID;
		if ( IsMatching() )
		{
			msgMRPC.m_nSignPlayer = m_vecMatchPlayerInfo.size();
			msgMRPC.m_nLeftPlayer = m_pRoomMatchRule->m_LeftPlayer;
		}
		else
		{
			msgMRPC.m_nSignPlayer = m_setSignMatchPID.size();
			msgMRPC.m_nLeftPlayer = 0;
		}
		m_pServer->SendMsgToAllPlayerInPlace(msgMRPC,m_RoomPlace);
	}
	else
	{
	}

	//CeShiInfo("CGameRoom::OnTimeSendRoomPlayerCount End");
	return 0;
}

int CGameRoom::SendTableListInfoToPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::SendTableListInfoToPlayer");
	CLogFuncTime lft(s_RoomFuncTime,"SendTableListInfoToPlayer");
	CeShiInfo("CGameRoom::SendTableListInfoToPlayer start");

	if ( (!pPlayer) || (!pPlayer->IsNeedSendMsg()) || IsRoomMatch() )
	{
		return 0;
	}

	BatchProtocol bp;
	bp.m_Flag = bp.START;
	bp.m_XYID = TableInfoList::XY_ID;
	pPlayer->SendMsg(bp);

	TableInfo ti;
	TableInfoList til;
	til.m_RoomID = m_RoomID;
	for( MapTable::iterator itorTable = m_Tables.begin();itorTable != m_Tables.end();itorTable++ )
	{
		if ( itorTable->second->IsTableShow() )
		{
			itorTable->second->GetTableInfo(ti);
			til.m_TableList.push_back(ti);
			if ( til.m_TableList.size() >= til.MAX_TABLElIST )
			{
				pPlayer->SendMsg(til);
				til.ReSet();
				til.m_RoomID = m_RoomID;
			}
		}
	}
	if ( til.m_TableList.size() )
	{
		pPlayer->SendMsg(til);
	}

	bp.m_Flag = bp.END;
	bp.m_nCount = 0;
	pPlayer->SendMsg(bp);

	pPlayer->AddRoomHaveSendTableInfo(m_RoomID);

	//将房间的可见信息发给玩家
	GameXY::TableShowList msgTSL;
	msgTSL.m_RoomID = m_RoomID;
	msgTSL.m_ShowFlag = msgTSL.TABLE_SHOW;
	for( MapTable::iterator itorTable = m_Tables.begin();itorTable != m_Tables.end();itorTable++ )
	{
		if ( itorTable->second->IsTableShow() )
		{
			msgTSL.m_vecTableID.push_back(itorTable->second->GetTableID());
		}

		if( msgTSL.m_vecTableID.size() >= msgTSL.MAX_TABLElIST )
		{
			pPlayer->SendMsg(msgTSL);
			msgTSL.m_vecTableID.clear();
		}
	}
	if( msgTSL.m_vecTableID.size() > 0 )
	{
		pPlayer->SendMsg(msgTSL);
		msgTSL.m_vecTableID.clear();
	}

	if ( IsTableMatch() )
	{
		MatchTableState msgMTS;
		XieYiList<MatchTableState,250> vecotrMT;
		for( MapTable::iterator itorTable = m_Tables.begin();itorTable != m_Tables.end();itorTable++ )
		{
			if ( itorTable->second->IsTableShow() )
			{
				assert(m_RoomID==itorTable->second->GetRoomID());
				msgMTS.m_RoomID = m_RoomID;
				msgMTS.m_TableID = itorTable->second->GetTableID();
				msgMTS.m_TableState = itorTable->second->GetTableMatchState();
				vecotrMT.m_XieYiList.push_back(msgMTS);

				if (vecotrMT.IsMustSendOut() )
				{
					pPlayer->SendMsg(vecotrMT);
					vecotrMT.Reset();
				}
			}
		}
		if ( vecotrMT.IsHaveData() )
		{
			pPlayer->SendMsg(vecotrMT);
		}
	}

	if ( IsPrivateRoom() )
	{
		CreateTablePID msgCTPID;
		XieYiList<CreateTablePID,200> vectorCTPID;
		for( MapTable::iterator itorTable = m_Tables.begin();itorTable != m_Tables.end();itorTable++ )
		{
			if ( itorTable->second->IsTableShow() )
			{
				msgCTPID.m_RoomID = m_RoomID;
				msgCTPID.m_TableID = itorTable->second->GetTableID();
				msgCTPID.m_PID = itorTable->second->GetFoundPID();
				vectorCTPID.m_XieYiList.push_back(msgCTPID);

				if (vectorCTPID.IsMustSendOut())
				{
					pPlayer->SendMsg(vectorCTPID);
					vectorCTPID.Reset();
				}
			}
		}
		if (vectorCTPID.IsHaveData())
		{
			pPlayer->SendMsg(vectorCTPID);
			vectorCTPID.Reset();
		}
	}

	//将房间中可见玩家发送几个给玩家
	if ( IsCommonRoom() && pPlayer->IsSendSameTablePlayer()==false )
	{
		for( MapTable::iterator itorTable = m_Tables.begin();itorTable != m_Tables.end();itorTable++ )
		{
			CGameTable* pTable = itorTable->second;
			if ( pTable && pTable->GetTotalPlayer() >= 5 )
			{
				pPlayer->SetSendSameTablePlayer(true);
				pTable->SendPlayerBaseInfoToPlayer(pPlayer);
				break;
			}
		}
	}

	CeShiInfo("CGameRoom::SendTableListInfoToPlayer end");
	
	return 0;
}

int CGameRoom::SendRoomPlayerToPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::SendRoomPlayerToPlayer");
	CLogFuncTime lft(s_RoomFuncTime,"SendRoomPlayerToPlayer");
	//if ( m_pServer->m_bShowRoomPlayer == false ) return 0;
	//CeShiInfo("CGameRoom::SendRoomPlayerToPlayer Start");

	//PlayerPtr pTempPlayer;

	//PlayerBaseInfo msgPBI;
	//PlayerStateInfo msgPSI;

	//for (MapPlayer::iterator itorPlayer=m_ShowPlayers.begin();itorPlayer!=m_ShowPlayers.end();itorPlayer++)
	//{
	//	pTempPlayer = itorPlayer->second;
	//	if ( pPlayer->GetPID() != pTempPlayer->GetPID() )
	//	{
	//		msgPBI.m_Flag = msgPBI.SameRoom;
	//		pTempPlayer->GetPlayerBaseInfo(msgPBI);			
	//		pPlayer->SendMsg(msgPBI);

	//		msgPSI.m_Flag = msgPSI.SameRoom;
	//		pTempPlayer->GetPlayerStateInfo(msgPSI);			
	//		pPlayer->SendMsg(msgPSI);
	//	}
	//}

	//CeShiInfo("CGameRoom::SendRoomPlayerToPlayer End");
	return 0;
}

int CGameRoom::OnTimeSendRoomPlayerState()
{
	TraceStackPath logTP("CGameRoom::OnTimeSendRoomPlayerState");
	CLogFuncTime lft(s_RoomFuncTime,"OnTimeSendRoomPlayerState");
	//if ( m_pServer->m_bShowRoomPlayer == false ) return 0;
	//CeShiInfo("CGameRoom::OnTimeSendRoomPlayerState Start");	

	//GameXY::PlayerStateInfo msgPSI;
	//GameXY::PlayerStateInfoList msgPSIList;

	//PlayerPtr pPlayer;
	//for ( MapPlayer::iterator itorPlayer=m_ShowPlayers.begin();itorPlayer!=m_ShowPlayers.end();++itorPlayer )
	//{
	//	pPlayer = itorPlayer->second;
	//	if ( pPlayer->m_RoomStateChange )
	//	{
	//		pPlayer->m_RoomStateChange = false;
	//		msgPSI.m_Flag = msgPSI.SameRoom;
	//		pPlayer->GetPlayerStateInfo(msgPSI);			

	//		msgPSIList.m_listPSI.push_back(msgPSI);
	//	}
	//}
	//for ( MapPlayer::iterator TempitorPlayer=m_RoomPlayers.begin();TempitorPlayer!=m_RoomPlayers.end();++TempitorPlayer)
	//{
	//	pPlayer = TempitorPlayer->second;
	//	if ( pPlayer->m_RegisterRoomPlayerTime && (!pPlayer->IsInTable()) )
	//	{
	//		pPlayer->SendMsg(msgPSIList);
	//		pPlayer->m_RegisterRoomPlayerTime = m_CurRoomTime;
	//	}
	//}

	//CeShiInfo("CGameRoom::OnTimeSendRoomPlayerState End");
	return 0;
}
int CGameRoom::SendRoomPlayerState(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::SendRoomPlayerState");
	CLogFuncTime lft(s_RoomFuncTime,"SendRoomPlayerState");
	//if ( m_pServer->m_bShowRoomPlayer == false ) return 0;
	//CeShiInfo("CGameRoom::SendRoomPlayerState Start");

	//if ( IsCommonRoom() && pPlayer && pPlayer->m_RegisterRoomPlayerTime+m_IntervalRoomPS<m_CurRoomTime )
	//{
	//	GameXY::PlayerStateInfo msgPSI;
	//	GameXY::PlayerStateInfoList msgPSIList;

	//	PlayerPtr pTempPlayer;
	//	for ( MapPlayer::iterator itorPlayer=m_ShowPlayers.begin();itorPlayer!=m_ShowPlayers.end();++itorPlayer )
	//	{
	//		pTempPlayer = itorPlayer->second;

	//		msgPSI.m_Flag = msgPSI.SameRoom;
	//		pTempPlayer->GetPlayerStateInfo(msgPSI);			
	//		msgPSIList.m_listPSI.push_back(msgPSI);
	//	}

	//	pPlayer->SendMsg(msgPSIList);
	//	pPlayer->m_RegisterRoomPlayerTime = m_CurRoomTime;
	//}

	//CeShiInfo("CGameRoom::SendRoomPlayerState End");
	return 0;
}

void CGameRoom::AddPlayerToClient(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::AddPlayerToClient");
	CLogFuncTime lft(s_RoomFuncTime,"AddPlayerToClient");
	
	//if ( m_pServer->m_bShowRoomPlayer == false ) return ;
	////CeShiInfo("CGameRoom::AddPlayerToClient Start AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID());

	//if ( pPlayer )
	//{
	//	pPlayer->m_IsRoomShowPlayer = true;

	//	GameXY::PlayerBaseInfo msgPBI;
	//	msgPBI.m_Flag = msgPBI.SameRoom;
	//	pPlayer->GetPlayerBaseInfo(msgPBI);

	//	GameXY::PlayerStateInfo msgPSI;
	//	msgPSI.m_Flag = msgPSI.SameRoom;
	//	pPlayer->GetPlayerStateInfo(msgPSI);

	//	for ( MapPlayer::iterator itorPlayer=m_RoomPlayers.begin();itorPlayer!=m_RoomPlayers.end();itorPlayer++)
	//	{
	//		if ( itorPlayer->second->m_RegisterRoomPlayerTime )
	//		{
	//			itorPlayer->second->SendMsg(msgPBI);
	//			itorPlayer->second->SendMsg(msgPSI);
	//		}
	//	}
	//}
	////CeShiInfo("CGameRoom::AddPlayerToClient End");
}
void CGameRoom::DeletePlayerToClient(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::DeletePlayerToClient");
	CLogFuncTime lft(s_RoomFuncTime,"DeletePlayerToClient");

	//if ( m_pServer->m_bShowRoomPlayer == false ) return ;
	////CeShiInfo("CGameRoom::DeletePlayerToClient Start AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID());
	//if ( pPlayer )
	//{
	//	pPlayer->m_IsRoomShowPlayer = false;

	//	GameXY::PlayerStateInfo msgPSI;
	//	msgPSI.m_Flag = msgPSI.SameRoom;
	//	pPlayer->GetPlayerStateInfo(msgPSI);		
	//	msgPSI.m_RoomID = 0;
	//	msgPSI.m_PlayerState = PLAYER_ST_NONE;

	//	for ( MapPlayer::iterator itorPlayer=m_RoomPlayers.begin();itorPlayer!=m_RoomPlayers.end();itorPlayer++)
	//	{
	//		if ( itorPlayer->second->m_RegisterRoomPlayerTime )
	//		{
	//			itorPlayer->second->SendMsg(msgPSI);
	//		}
	//	}
	//}
	////CeShiInfo("CGameRoom::DeletePlayerToClient End");
}

void CGameRoom::AddShowPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::AddShowPlayer");
	CLogFuncTime lft(s_RoomFuncTime,"AddShowPlayer");

	//if ( m_pServer->m_bShowRoomPlayer == false ) return ;
	////CeShiInfo("CGameRoom::AddShowPlayer Start AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID());

	//if ( pPlayer && IsCommonRoom() )
	//{
	//	if ( m_ShowPlayers.size() >= m_pGSConfig->m_cfServer.m_MaxShowRoomPlayer )
	//	{
	//		PlayerPtr TempPlayer = NULL;
	//		bool bSuccess = false;

	//		if ( !(pPlayer->IsBotPlayer()) )
	//		{
	//			for ( MapPlayer::iterator itorPlayer=m_ShowPlayers.begin();itorPlayer!=m_ShowPlayers.end();itorPlayer++ )
	//			{
	//				TempPlayer = itorPlayer->second;
	//				if ( TempPlayer->IsBotPlayer() )
	//				{	
	//					bSuccess = true;

	//					DeletePlayerToClient(TempPlayer);
	//					m_ShowPlayers.erase(itorPlayer);
	//					
	//					m_ShowPlayers.insert(make_pair(pPlayer->GetPID(),pPlayer));
	//					AddPlayerToClient(pPlayer);

	//					break;
	//				}
	//			}
	//		}
	//		if ( !bSuccess )
	//		{
	//			for ( MapPlayer::iterator itorPlayer=m_ShowPlayers.begin();itorPlayer!=m_ShowPlayers.end();itorPlayer++ )
	//			{
	//				TempPlayer = itorPlayer->second;
	//				if ( pPlayer->GetGameMoney() > TempPlayer->GetGameMoney() )
	//				{
	//					DeletePlayerToClient(TempPlayer);
	//					m_ShowPlayers.erase(itorPlayer);
	//					
	//					m_ShowPlayers.insert(make_pair(pPlayer->GetPID(),pPlayer));
	//					AddPlayerToClient(pPlayer);

	//					break;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{			
	//		m_ShowPlayers.insert(make_pair(pPlayer->GetPID(),pPlayer));
	//		AddPlayerToClient(pPlayer);
	//	}
	//}
	////CeShiInfo("CGameRoom::AddShowPlayer End");
}
void CGameRoom::RemoveShowPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::RemoveShowPlayer");
	CLogFuncTime lft(s_RoomFuncTime,"RemoveShowPlayer");

	//if ( m_pServer->m_bShowRoomPlayer == false ) return ;
	//
	//MapPlayer::iterator itorPlayer = m_ShowPlayers.find(pPlayer->GetPID());
	//if ( itorPlayer != m_ShowPlayers.end() )
	//{
	//	m_ShowPlayers.erase(itorPlayer);
	//	DeletePlayerToClient(pPlayer);
	//}
}

bool CGameRoom::AttachPlayerRoom(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::AttachPlayerRoom");
	CLogFuncTime lft(s_RoomFuncTime,"AttachPlayerRoom");

	CeShiInfo("CGameRoom::AttachPlayer start AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID());

	MapPlayer::iterator itorPlayer = m_RoomPlayers.find(pPlayer->GetPID());
	if( itorPlayer == m_RoomPlayers.end())
	{
		pPlayer->LeaveRoom();
		return false;
	}
	else
	{
		SendTableListInfoToPlayer(pPlayer);

		GameXY::RespJoinRoom rsjr;
		rsjr.m_RoomID = pPlayer->GetRoomID();
		rsjr.m_Flag = rsjr.SUCCESS;
		rsjr.m_Type = rsjr.ATTACH;
		pPlayer->SendMsg(rsjr);

		if ( pPlayer->IsShowLobby() == false )
		{
			Game_MsgFlag msgMF;
			msgMF.m_Flag = msgMF.LobbyReady;
			pPlayer->SendMsg(msgMF);

			pPlayer->SetShowLobby(true);
		}

		CGameTable* pTable;
		if( pTable = this->GetTableByID( pPlayer->GetTableID()) )
		{
			pTable->AttachPlayerTable( pPlayer );
		}
	}

	CeShiInfo("CGameRoom::AttachPlayer End");
	return true;
}

bool CGameRoom::PlayerJoinRoom(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::PlayerJoinRoom");
	CLogFuncTime lft(s_RoomFuncTime,"PlayerJoinRoom");

	CeShiInfo("CGameRoom::PlayerJoinRoom start PID=%d",pPlayer->GetPID());

	if ( pPlayer->HaveSendTableInfoList(m_RoomID) == false )
	{
 		SendTableListInfoToPlayer(pPlayer);
	}

	if ( pPlayer->IsShowLobby() == false )
	{
		Game_MsgFlag msgMF;
		msgMF.m_Flag = msgMF.LobbyReady;
		pPlayer->SendMsg(msgMF);

		pPlayer->SetShowLobby(true);
	}

	MapPlayer::iterator itorPlayer = m_RoomPlayers.find(pPlayer->GetPID());
	if( itorPlayer == m_RoomPlayers.end())
	{
		m_RoomPlayers.insert( make_pair(pPlayer->GetPID(),pPlayer) );

		m_CurPlayerInRoom++;
		assert( m_CurPlayerInRoom == UINT16(m_RoomPlayers.size()));
		if ( pPlayer->IsNotBotPlayer() )
		{
			m_RealPlayerInRoom++;
		}
		else
		{
			m_BotPlayerInRoom++;
		}
		assert( m_CurPlayerInRoom == m_RealPlayerInRoom+m_BotPlayerInRoom);
	}
	else
	{
		DebugError("CGameRoom::PlayerJoinRoom Player 已经存在了");
	}

	if ( pPlayer->GetSocket() )
	{
		PlayerJoinRoomInfo msgJR;
		msgJR.m_RoomID = this->GetRoomID();
		msgJR.m_PID = pPlayer->GetPID();
		pPlayer->SendMsg(msgJR);
	}

	pPlayer->JoinRoom(this);
	pPlayer->SetPlayerState(PLAYER_ST_ROOM);

	pPlayer->m_RegisterRoomPlayerTime = 0;
	pPlayer->m_RoomStateChange = true;
	pPlayer->SetCityStateChange(true);
	pPlayer->SetFriendStateChange(true);
	pPlayer->m_EnterRoomTimes++;

	if ( pPlayer->IsNotBotPlayer() )
	{
		SendTablePlayerCount(pPlayer);
	}
	AddShowPlayer(pPlayer);

	CeShiInfo("CGameRoom::PlayerJoinRoom end");
	return true;
}

bool CGameRoom::PlayerLeaveRoom(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::PlayerLeaveRoom");
	CLogFuncTime lft(s_RoomFuncTime,"PlayerLeaveRoom");

	CeShiInfo("CGameRoom::PlayerLeaveRoom Start PID=%d",pPlayer->GetPID());

	if ( pPlayer->GetTableID() > 0 )
	{
		assert(pPlayer->GetTable());
		CGameTable* pTable = GetTableByID( pPlayer->GetTableID() );
		if( pTable )
		{
			assert( pTable == pPlayer->GetTable());
			pTable->PlayerLeaveTable(pPlayer,Action_Flag_SysLogic);			
		}
	}
	else
	{
		assert(!pPlayer->GetTable());
	}

	if ( pPlayer->GetSocket() )
	{
		PlayerLeaveRoomInfo lr;
		lr.m_RoomID = this->GetRoomID();
		lr.m_PID = pPlayer->GetPID();
		pPlayer->SendMsg(lr);
	}

	MapPlayer::iterator itorPlayer = m_RoomPlayers.find(pPlayer->GetPID());
	if( itorPlayer != m_RoomPlayers.end())
	{
		m_RoomPlayers.erase(itorPlayer);

		m_CurPlayerInRoom--;
		assert( m_CurPlayerInRoom == UINT16(m_RoomPlayers.size()));
		if ( pPlayer->IsNotBotPlayer() )
		{
			m_RealPlayerInRoom--;
			assert(m_RealPlayerInRoom>=0);
		}
		else
		{
			m_BotPlayerInRoom--;
			assert(m_BotPlayerInRoom>=0);
		}
		assert(m_CurPlayerInRoom == m_RealPlayerInRoom+m_BotPlayerInRoom);
	}

	pPlayer->LeaveRoom();
	pPlayer->SetPlayerState( PLAYER_ST_LOBBY );

	pPlayer->m_RegisterRoomPlayerTime = 0;
	pPlayer->m_IsRoomShowPlayer = false;
	pPlayer->m_RoomStateChange = true;
	pPlayer->SetCityStateChange(true);
	pPlayer->SetFriendStateChange(true);

	RemoveShowPlayer(pPlayer);

	CeShiInfo("CGameRoom::PlayerLeaveRoom end");
	return true;
}