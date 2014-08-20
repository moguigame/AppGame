#include ".\gameroom.h"

#include "server.h"
#include "gametable.h"

#include "CGameServerConfig.h"

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

		m_RoomPlayers.clear();

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
			m_RoomType = int(itorRule->second);
		}
		else
		{
			DebugError("CreateTable m_RoomID=%d m_RoomType=%d",m_RoomID,m_RoomType);
		}

		itorRule = mapRule.find("place");
		if ( itorRule != mapRule.end())
		{
			m_RoomPlace = int(itorRule->second);
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

		m_RoomMinTake = 1000;
		m_RoomMaxTake = 1000000;
		itorRule = mapRule.find("mintake");
		if ( itorRule != mapRule.end())
		{
			m_RoomMinTake = itorRule->second;
		}
		itorRule = mapRule.find("maxtake");
		if ( itorRule != mapRule.end())
		{
			m_RoomMaxTake = itorRule->second;
			if ( m_RoomMaxTake == 0 )
			{
				m_RoomMaxTake = INT64_MAX;
			}
		}		

		m_CurPlayerInRoom       = 0;
		m_RealPlayerInRoom      = 0;
		m_BotPlayerInRoom       = 0;
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

	m_Tables.clear();
	m_RoomPlayers.clear();

	m_vecForBidAreaSee.clear();
	m_vecAllowAreaSee.clear();
	m_vecForBidAreaPlay.clear();
	m_vecAllowAreaPlay.clear();
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
				if ( int(m_Tables.size()) < m_MaxTableInRoom )
				{
					m_Tables.insert( make_pair(cti.m_TableID,pTable) );
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
	return strRule;
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

bool CGameRoom::OnTimer(UINT32 curTime)
{
	TraceStackPath logTP("CGameRoom::OnTimer");
	CLogFuncTime lft(s_RoomFuncTime,"CGameRoom::OnTimer");
	DebugInfo("CGameRoom::OnTimer Start RoomID=%d",m_RoomID);

	m_CurRoomTime = curTime;

	for(MapTable::iterator itorTables=m_Tables.begin();itorTables!=m_Tables.end();itorTables++)
	{
		itorTables->second->OnTimer(curTime);
	}

	DebugInfo("CGameRoom::OnTimer End RoomID=%d",m_RoomID);
	return true;
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

int  CGameRoom::OnRoomMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CGameRoom::OnRoomMsg");
	CLogFuncTime lft(s_RoomFuncTime,"OnRoomMsg");

	int ret = 0;
	switch ( msgPack.m_XYID )
	{
	case 1:
		break;
	default:
		{
			DebugError("OnRoomMsg 未处理协议 xyid=%d xylen=%d",msgPack.m_XYID,msgPack.m_nLen);
		}
	}
	return ret;
}

int  CGameRoom::OnReqJoinTable(PlayerPtr pPlayer,GameXY::ReqJoinTable& rjt)
{
	TraceStackPath logTP("CGameRoom::OnReqJoinTable");
	CLogFuncTime lft(s_RoomFuncTime,"OnReqJoinTable");
	DebugInfo("CGameRoom::onReqJoinTable start ReqPID=%d",pPlayer->GetPID());

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

	DebugInfo("CGameRoom::onReqJoinTable end");

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


int CGameRoom::PlayerCanJoinRoom(PlayerPtr pPlayer)
{
	int nRet = 0;
	if ( pPlayer->GetGameMoney() < m_RoomMinTake )
	{
		nRet = RespJoinRoom::MoneyLess;
	}
	else if ( pPlayer->GetGameMoney() > m_RoomMaxTake )
	{
		nRet = RespJoinRoom::MoneyMore;
	}
	return nRet;
}
bool CGameRoom::PlayerCanLeaveRoom(PlayerPtr pPlayer)
{
	return true;
}

bool CGameRoom::AttachPlayerRoom(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::AttachPlayerRoom");
	CLogFuncTime lft(s_RoomFuncTime,"AttachPlayerRoom");

	DebugInfo("CGameRoom::AttachPlayer start AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID());

	MapPlayer::iterator itorPlayer = m_RoomPlayers.find(pPlayer->GetPID());
	if( itorPlayer == m_RoomPlayers.end())
	{
		pPlayer->LeaveRoom();
		return false;
	}
	else
	{
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

	DebugInfo("CGameRoom::AttachPlayer End");
	return true;
}

bool CGameRoom::PlayerJoinRoom(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::PlayerJoinRoom");
	CLogFuncTime lft(s_RoomFuncTime,"PlayerJoinRoom");

	DebugInfo("CGameRoom::PlayerJoinRoom start PID=%d",pPlayer->GetPID());

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
	pPlayer->SetFriendStateChange(true);
	pPlayer->m_EnterRoomTimes++;

	DebugInfo("CGameRoom::PlayerJoinRoom end");
	return true;
}

bool CGameRoom::PlayerLeaveRoom(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CGameRoom::PlayerLeaveRoom");
	CLogFuncTime lft(s_RoomFuncTime,"PlayerLeaveRoom");

	DebugInfo("CGameRoom::PlayerLeaveRoom Start PID=%d",pPlayer->GetPID());

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
	pPlayer->SetFriendStateChange(true);

	DebugInfo("CGameRoom::PlayerLeaveRoom end");
	return true;
}

bool CGameRoom::PlayerChangeTable(PlayerPtr pPlayer)
{
	bool bRet = false;
	if ( pPlayer->GetRoomID()==GetRoomID() )
	{
		int nCurTableID = pPlayer->GetTableID();
		CGameTable* pCurTable = pPlayer->GetTable();
		if ( nCurTableID && pCurTable )
		{
			assert(GetTableByID(nCurTableID)==pCurTable);
			pCurTable->PlayerLeaveTable(pPlayer);
		}

		for(MapTable::iterator itorTable=m_Tables.begin();itorTable!=m_Tables.end();itorTable++)
		{
			CGameTable* pTempTable = itorTable->second;
			if ( pTempTable->GetTableID()!=nCurTableID && pTempTable->GetEmptySitID() )
			{
				pTempTable->PutPlayerSitDown(pPlayer,0);
				break;
			}
		}
	}
	return true;
}