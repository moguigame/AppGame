#include ".\dezhougamelogic.h"

#include "Server.h"
#include "GameRoom.h"
#include "GameTable.h"

#include "CGameServerConfig.h"

CMapFunctionTime CDeZhouGameLogic::s_LogicFuncTime;

CDeZhouGameLogic::CDeZhouGameLogic(CGameTable* pTable)
{
	TraceStackPath logTP("CDeZhouGameLogic::CDeZhouGameLogic");
	CLogFuncTime lft(s_LogicFuncTime,"CDeZhouGameLogic");

	m_pTable = NULL;
	m_pRoom = NULL;
	m_pServer = NULL;

	if ( !pTable )
	{
		printf_s("CDeZhouGameLogic pTable Error! \n");
		return ;
	}

	m_pTable = pTable;
	m_pGameLog = new CGameLog(pTable);

	m_pRoom = pTable->GetRoom();
	assert(m_pRoom);

	m_pServer = m_pTable->GetServer();
	assert(m_pServer);

	m_pGSConfig = m_pServer->GetGSConfig();

	InitGameLogic();
}

CDeZhouGameLogic::~CDeZhouGameLogic(void)
{
	safe_delete(m_pGameLog);
}

int CDeZhouGameLogic::InitGameLogic()
{
	TraceStackPath logTP("CDeZhouGameLogic::InitGameLogic");
	CLogFuncTime lft(s_LogicFuncTime,"InitGameLogic");

	m_CurTime = m_pServer->GetCurTime();
	m_StartGameTime = 0;
	m_GameState = GAME_ST_NONE;

	m_ClientTableRule = "";

	m_CurPlayingPlayerNumber = 0;

	m_ActionTime = 0;
	m_DelayTime = 20;

	for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
	{
		m_GP[Sit].Init();
		m_Tallys[Sit].ReSet();
	}

	m_TableType = TABLE_TYPE_COMMON;
	
	m_MaxSitPlayerNumber = MAX_PALYER_ON_TABLE;
	m_MinPlayerForGame = MIN_PLAYER_FOR_GAME;
	m_WaitStartTime = 2;

	m_ManageBotPlayerTime = m_pServer->GetCurTime();
	m_timeAddBotPlayer    = m_pServer->GetCurTime();

	m_TableBotFlag = Table_Bot_None;
	m_TableBotPlayerNumber = 0;
	m_MaxBotChipTime = 5;

	m_BotLossEveryTime = 0;
	m_BotTargetWinLoss = 0;
	m_BotRealWinLoss = 0;

	InitGameRule();
	m_ClientTableRule = GetGameRule();

	return 0;
}

int CDeZhouGameLogic::InitGameRule()
{
	TraceStackPath logTP("CDeZhouGameLogic::InitGameRule");
	CLogFuncTime lft(s_LogicFuncTime,"InitGameRule");

	std::string strRule = "";
	std::map<string,INT64> mapRule;
	std::map<string,INT64>::iterator itorKey;

	strRule = m_pTable->GetRoom()->GetRoomRuleEx();
	mapRule = Tool::GetRuleMapInData(strRule,string(";"),string("="));

	m_BotLossEveryTime = 0;
	itorKey = mapRule.find("botloss");
	if ( itorKey != mapRule.end())
	{
		m_BotLossEveryTime = INT32(itorKey->second);
	}
	if ( !(m_BotLossEveryTime>=-100 && m_BotLossEveryTime<=100) )
	{
		m_pTable->DebugError("BotLoss=%d",m_BotLossEveryTime);
	}

	m_WaitChipTime = T_ChipTime_Long;
	itorKey = mapRule.find("chiptime");
	if ( itorKey != mapRule.end() )
	{
		m_WaitChipTime = INT32(itorKey->second);
	}
	if ( !(m_WaitChipTime>=10 && m_WaitChipTime<=20) )
	{
		m_pTable->DebugError("waitchiptime=%d",m_WaitChipTime);
	}

	m_MaxBotChipTime = 5;
	itorKey = mapRule.find("botchiptime");
	if ( itorKey != mapRule.end() )
	{
		m_MaxBotChipTime = BYTE(itorKey->second);
	}
	if ( m_MaxBotChipTime >= m_WaitChipTime )
	{
		m_MaxBotChipTime = m_WaitChipTime/2;
	}
	if ( !(m_MaxBotChipTime>1 && m_MaxBotChipTime<m_WaitChipTime) )
	{
		m_pTable->DebugError("m_MaxBotChipTime=%d m_WaitChipTime=%d Frist=%s Second=%d",
			int(m_MaxBotChipTime),int(m_WaitChipTime),itorKey->first.c_str(),int(itorKey->second) );		
	}	

	m_WaitStartTime = T_WaitStartTime;
	itorKey = mapRule.find("waittime");
	if (itorKey != mapRule.end())
	{
		m_WaitStartTime = INT32(itorKey->second);
	}
	if ( !(m_WaitStartTime>=2 && m_WaitStartTime<=5) )
	{
		m_pTable->DebugError("waitstarttime=%d",m_WaitStartTime);
	}

	{
		strRule = m_pTable->GetTableRule();
		mapRule = Tool::GetRuleMapInData(strRule,string(";"),string("="));

		m_TableType = TABLE_TYPE_COMMON;
		itorKey = mapRule.find("tabletype");
		if ( itorKey != mapRule.end())
		{
			m_TableType = INT32(itorKey->second);
		}
		if ( !(m_TableType>=TABLE_TYPE_COMMON && m_TableType<TABLE_TYPE_END) )
		{
			m_pTable->DebugError("tabletype=%d",m_TableType);
		}

		itorKey = mapRule.find("servicepay");
		if ( itorKey != mapRule.end() )
		{
			m_nServiceMoney = INT32(itorKey->second);
		}
		else
		{
			m_nServiceMoney = 0;
			m_pTable->DebugError("Init Logic Error m_TableID=%d m_nServiceMoney=%d",m_pTable->GetTableID(),m_nServiceMoney);
		}
		if ( !(m_nServiceMoney >= 0) )
		{
			m_pTable->DebugError("m_nServiceMoney=%d",m_nServiceMoney);
		}

		m_MaxSitPlayerNumber = MAX_PALYER_ON_TABLE;
		m_MinPlayerForGame   = MIN_PLAYER_FOR_GAME;

		itorKey = mapRule.find("chiptime");
		if ( itorKey != mapRule.end() )
		{
			m_WaitChipTime = INT32(itorKey->second);
		}
		if ( !(m_WaitChipTime>=10 && m_WaitChipTime<=20) )
		{
			m_pTable->DebugError("WaitChipTime=%d",m_WaitChipTime);
		}

		itorKey = mapRule.find("waittime");
		if ( itorKey != mapRule.end() )
		{
			m_WaitStartTime = INT32(itorKey->second);
		}
		if ( !(m_WaitStartTime>=2 && m_WaitStartTime<=5) )
		{
			m_pTable->DebugError("WaitStartTime=%d",m_WaitStartTime);
		}
	}

	strRule = m_pTable->GetTableRuleEX();
	//strRule = "tabletype=0;servicepay=1;smallblind=5;bigblind=10;mintake=20;maxtake=200;minforgame=20;chiptime=15;waittime=3;maxin=250;maxsit=9;minplayer=2;pot=0;chip1=0;chip2=0;chip3=0;chip4=0";
	mapRule = Tool::GetRuleMapInData(strRule,string(";"),string("="));

	m_TableBotPlayerNumber = 0;
	m_TableBotFlag = Table_Bot_None;
	itorKey = mapRule.find("botnum");
	if ( itorKey != mapRule.end() )
	{
		m_TableBotPlayerNumber = BYTE(itorKey->second);
	}
	if ( m_TableBotPlayerNumber >= 3 )
	{
		m_TableBotFlag = Table_Bot_Keep;
	}
	else if( m_TableBotPlayerNumber>0 && m_TableBotPlayerNumber<3 )
	{
		m_TableBotFlag = Table_Bot_Flow;
	}

	itorKey = mapRule.find("botchiptime");
	if ( itorKey != mapRule.end() )
	{
		m_MaxBotChipTime = BYTE(itorKey->second);
	}
	if ( m_MaxBotChipTime >= m_WaitChipTime )
	{
		m_MaxBotChipTime = m_WaitChipTime/2;
	}
	if ( !(m_MaxBotChipTime>=1 && m_MaxBotChipTime<=m_WaitChipTime) )
	{
		m_pTable->DebugError("maxbotchiptime=%d",m_MaxBotChipTime);
	}

	itorKey = mapRule.find("botloss");
	if ( itorKey != mapRule.end() )
	{
		m_BotLossEveryTime = INT32(itorKey->second);
	}
	if ( !(m_BotLossEveryTime>=-100 && m_BotLossEveryTime<=100) )
	{
		m_pTable->DebugError("botloss=%d",m_BotLossEveryTime);
	}

	return 0;
}

void CDeZhouGameLogic::OnTimer( UINT32 curTime )
{
	TraceStackPath logTP("CDeZhouGameLogic::OnTimer");
	CLogFuncTime lft(s_LogicFuncTime,"CDeZhouGameLogic::OnTimer");
	//m_pTable->DebugInfo("CDeZhouGameLogic::OnTime Start RoomId=%d TableID=%d",m_pTable->GetRoomID(),m_pTable->GetTableID());

	m_CurTime = curTime;
	if ( IsGameIdle() && m_CurTime >= m_StartGameTime )
	{
	}

	OnTimeManageBotPlayer(m_CurTime);

	//m_pTable->DebugInfo("CDeZhouGameLogic::OnTime End... ");
}

void CDeZhouGameLogic::OnTimeManageBotPlayer(UINT32 curTime)
{
	if ( curTime - m_ManageBotPlayerTime < 10 )
	{
		return ;
	}

	CLogFuncTime lft(s_LogicFuncTime,"OnTimeManageBotPlayer");

	m_ManageBotPlayerTime = curTime;

	int BotPlayerNumber=0,CommPlayerNumber=0;
	int CurSitNumber = m_pTable->GetSitPlayerNumber();
	for ( int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() )
		{
			if ( m_GP[Sit].IsBotPlayer() )
			{
				BotPlayerNumber++;
			}
			else
			{
				CommPlayerNumber++;
			}
		}
	}
	if ( CurSitNumber != BotPlayerNumber+CommPlayerNumber )
	{
		m_pTable->DebugError("OnTimeManageBotPlayer CurSit=%d Bot=%d Comm=%d",CurSitNumber,BotPlayerNumber,CommPlayerNumber);
	}

	TestSitPlayer();

	if ( m_TableBotFlag == Table_Bot_Keep )
	{
		if ( CurSitNumber < m_TableBotPlayerNumber )  //加机器人的情况
		{
			if ( CurSitNumber == 0 )
			{
				for ( int i=0;i<m_TableBotPlayerNumber;i++)
				{
					m_pTable->AddBotPlayer(0);
				}
			}
			else if ( CRandom::GetChangce(10,1) && CommPlayerNumber<=3 /* && m_pRoom->GetRealPlayerCount()<100 */ )
			{
				m_pTable->AddBotPlayer(0);
			}
		}
	}
	if ( m_TableBotFlag == Table_Bot_Flow )
	{
		if ( CommPlayerNumber>0 && CurSitNumber<=3 && CRandom::GetChangce(3,1) )  //玩家较少的情况下，给玩家补充机器人
		{
			m_pTable->AddBotPlayer(0);
		}
	}
}

string CDeZhouGameLogic::GetGameRule()
{
	string strGameRule = "";
	return strGameRule;
}

void CDeZhouGameLogic::DebugLogicState()
{
	m_pTable->ReleaseInfo("DebugAllPlayer:State=%d",m_GameState);

	for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit)
	{
		DebugSitPlayer(Sit+1);
	}
}

void CDeZhouGameLogic::DebugAllPlayer()
{
	m_pTable->ReleaseInfo("DebugAllPlayer:State=%d",m_GameState);
	m_pGameLog->Print();
}

void CDeZhouGameLogic::DebugSitPlayer(int SitID)
{
	if ( IsAvailSitID(SitID) )
	{
		int Sit = SitID - 1;
		m_pTable->DebugInfo("DebugSitPlayer SitID=%d PID=%d GameState=%d TableMoney=%s ",
			SitID,m_GP[Sit].GetPID(),m_GP[Sit].GetGameState(),Tool::N2S(m_GP[Sit].GetTableMoney()).c_str() );
	}
	else
	{
		m_pTable->DebugError("DebugSitPlayer SitID=%d",SitID);
	}
}

void CDeZhouGameLogic::TestSitPlayer()
{
	if ( m_pGSConfig->CeshiMode() )
	{
		int BotPlayerNumber = 0,CommPlayerNumber = 0;
		int CurSitNumber = m_pTable->GetSitPlayerNumber();

		for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit )
		{
			if ( m_GP[Sit].IsHavePlayer() )
			{
				if ( m_GP[Sit].IsBotPlayer() )
				{
					BotPlayerNumber++;
				}
				else
				{
					CommPlayerNumber++;
				}

				if( Sit>=m_MaxSitPlayerNumber )
				{
					m_pTable->DebugError("TestSitPlayer SitID=%d MaxSitPlayer=%d",Sit+1,m_MaxSitPlayerNumber );
				}
			}
		}

		if ( BotPlayerNumber+CommPlayerNumber != CurSitNumber )
		{
			m_pTable->DebugError("BotPlayerNumber=%d CommPlayerNumber=%d CurSitNumber=%d",BotPlayerNumber,CommPlayerNumber,CurSitNumber );
		}
	}
}

void CDeZhouGameLogic::TestGPPlayer(PlayerPtr pPlayer,int SitID)
{
	if ( m_pGSConfig->CeshiMode() )
	{
		int Sit = SitID - 1;
		if ( !(IsAvailSitID(SitID) && m_GP[Sit].GetAID()==pPlayer->GetAID() && m_GP[Sit].GetPID()==pPlayer->GetPID() && pPlayer->GetSitID()==SitID) )
		{
			m_pTable->DebugError("TestGPPlayer SitID=%d PPID=%d",SitID,pPlayer->GetPID() );
		}
	}
}

bool CDeZhouGameLogic::IsSameTable(PlayerPtr LeftPlayer,PlayerPtr RightPlayer)
{
	if ( LeftPlayer && RightPlayer && LeftPlayer->GetRoomID()>0 && LeftPlayer->GetTableID()>0 
		&& LeftPlayer->GetRoomID()==RightPlayer->GetRoomID() && LeftPlayer->GetTableID()==RightPlayer->GetTableID() )
	{
		return true;
	}
	return false;
}

bool CDeZhouGameLogic::IsFriend(PlayerPtr LeftPlayer,PlayerPtr RightPlayer)
{
	if ( LeftPlayer && RightPlayer )
	{
		if ( LeftPlayer->IsFollow(RightPlayer->GetPID()) && RightPlayer->IsFollow(LeftPlayer->GetPID()) )
		{
			return true;
		}
	}
	return false;
}

bool CDeZhouGameLogic::IsFriend(UINT32 LPID,UINT32 RPID)
{
	PlayerPtr lPlayer = m_pServer->GetPlayerByPID(LPID);
	PlayerPtr rPlayer = m_pServer->GetPlayerByPID(RPID);

	if ( !lPlayer || !rPlayer )
	{
		return false;
	}
	
	return IsFriend(lPlayer,rPlayer);
}

bool CDeZhouGameLogic::IsPlayerInTable(UINT32 PID)
{
	PlayerPtr pPlayer = m_pTable->GetTablePlayerByPID(PID);
	if (pPlayer)
	{
		return true;
	}
	return false;
}

int CDeZhouGameLogic::GetPlayingPlayerNumber()
{
	int ret = 0;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsPlaying() )
		{
			ret++;
		}
	}
	return ret;
}
int CDeZhouGameLogic::GetAvailPlayerNumber()
{
	int ret = 0;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsPlaying() )
		{
			ret++;
		}
	}
	return ret;
}
int CDeZhouGameLogic::GetSitPlayerNumber()
{
	int ret = 0;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() )
		{
			ret++;
		}
	}
	return ret;
}
int CDeZhouGameLogic::GetSitBotPlayerNumber()
{
	int ret = 0;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() && m_GP[Sit].IsBotPlayer() )
		{
			ret++;
		}
	}
	return ret;
}

bool CDeZhouGameLogic::IsGameLogicStop()const
{
	return m_StartGameTime == N_Time::Max_Time;
}
int CDeZhouGameLogic::GetSitPlayerState(int SitID,UINT32 PID)
{
	int Sit = SitID - 1;
	if ( IsAvailSitID(SitID) && m_GP[Sit].GetPID()==PID )
	{
		return m_GP[Sit].GetGameState();
	}
	return 0;
}

void CDeZhouGameLogic::InitGameData()
{
	m_GamePaiLogic.InitPai();

	m_nGameWinLossMoney = 0;
	m_nGameLossMoney = 0;
}

void CDeZhouGameLogic::StartGameLogic()
{
	m_pTable->DebugInfo("StartGameLogic RoomID=%d TableID=%d ",m_pRoom->GetRoomID(),m_pTable->GetTableID() );
	m_StartGameTime = m_CurTime;
}
void CDeZhouGameLogic::EndGameLogic()
{
	m_pTable->DebugInfo("EndGameLogic RoomID=%d TableID=%d ",m_pRoom->GetRoomID(),m_pTable->GetTableID() );
	m_StartGameTime = N_Time::Max_Time;
}

bool CDeZhouGameLogic::CanStartGame()
{
	TraceStackPath logTP("CDeZhouGameLogic::CanStartGame");
	CLogFuncTime lft(s_LogicFuncTime,"CanStartGame");

	bool bRet = false;
	int SitPlayerCount = m_pTable->GetSitPlayerNumber();
	if ( SitPlayerCount > 0 )
	{
		m_GameState = GAME_ST_WAIT;		
		if ( SitPlayerCount >= m_MinPlayerForGame )
		{
			bRet = true;
		}
	}
	else
	{
		m_GameState = GAME_ST_NONE;
	}

	return bRet;
}

void CDeZhouGameLogic::StartCommonGame()
{
	TraceStackPath logTP("CDeZhouGameLogic::StartCommonGame");
	CLogFuncTime lft(s_LogicFuncTime,"StartCommonGame");

	//m_pTable->DebugInfo("StartCommonGame() Start ...");

	for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
	{
		int SitID = Sit+1;
		CGamePlayer& rGPlayer = m_GP[Sit];
		PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(SitID);

		if ( pPlayer && Sit>=m_MaxSitPlayerNumber )
		{
			m_pTable->DebugError("Error SitID TableID=%d,SitID=%d PlayerID=%d",m_pTable->GetTableID(),SitID,pPlayer->GetPID());			
			LogicStandUpPlayer(pPlayer,SitID,Action_Flag_SysLogic);
			continue;
		}

		if ( pPlayer )
		{
			if ( rGPlayer.IsExit() || rGPlayer.IsWashOut() )
			{
				m_pTable->DebugError("StartCommonGame InitPlayerData TableID=%d,SitID=%d PlayerID=%d",
					m_pTable->GetTableID(),SitID,pPlayer->GetPID());
			}
			else
			{
				rGPlayer.SetGameState(GAME_PLAYER_ST_WAITING);
			}
		}
		else
		{
			if ( rGPlayer.IsHavePlayer() || rGPlayer.IsWashOut() )
			{
				m_pTable->DebugError("StartCommonGame InitPlayerData222 TableID=%d,SitID=%d GPlayerID=%d",
					m_pTable->GetTableID(),SitID,rGPlayer.GetPID() );
			}
			if ( rGPlayer.IsExit() || rGPlayer.IsWashOut() )
			{
				rGPlayer.Init();
			}
		}
	}

	if ( m_CurPlayingPlayerNumber < m_MinPlayerForGame )
	{
		return ;
	}

	for(int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		m_Tallys[Sit].ReSet();

		PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);		
		if ( m_GP[Sit].IsPlaying() )
		{
			m_Tallys[Sit].m_PID = m_GP[Sit].GetPID();
			pPlayer->SetPlayerState(PLAYER_ST_PALYING);
		}
		else if( m_GP[Sit].IsWait() )
		{
			pPlayer->SetPlayerState(PLAYER_ST_WAIT);
		}
	}

	m_pTable->DebugInfo("游戏开始......PlayerNum=%d---------------------------",m_CurPlayingPlayerNumber);
	m_pTable->reportStartGame();

	//m_pTable->DebugInfo("StartCommonGame() End... ");
}

int CDeZhouGameLogic::OnGameMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnGameMsg");
	CLogFuncTime lft(s_LogicFuncTime,"OnGameMsg");

	m_pTable->DebugInfo("OnGameMsg Start XYID=%d XYLen=%d",msgPack.m_XYID,msgPack.m_nLen);

	GameXY::ClientToServerMessage CTSMsg;
	TransplainMsg(msgPack,CTSMsg);

	m_pTable->DebugInfo("OnGameMsg GameXYID=%d XYLen=%d",CTSMsg.m_MsgID,CTSMsg.m_MsgLen);

	int ret = 0;
	switch ( CTSMsg.m_MsgID )
	{
	case GameDeZhou_TableChat::XY_ID:
		{
			ret = OnTableChat(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_PrivateChat::XY_ID:
		{
			ret = OnPrivateChat(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_TableQuickChat::XY_ID:
		{
			ret = OnTableQuickChat(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_ReqAddFriend::XY_ID:
		{
			ret = OnAddFriend(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_ReqTailInfo::XY_ID:
		{
			ret = OnLogicReqTailInfo(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_ReqPromoteTime::XY_ID:
		{
			ret = OnReqPromoteTime(pPlayer,CTSMsg);
		}
		break;
	default:
		{
			ret = 10000;
		}
	}
	if ( ret )
	{
		m_pTable->DebugError("CDeZhouGameLogic::OnGameMsg id=%d len=%d ret=%d pid=%d",CTSMsg.m_MsgID,CTSMsg.m_MsgLen,ret,pPlayer->GetPID());
	}

	m_pTable->DebugInfo("OnGameMsg End");

	return ret;
}

int CDeZhouGameLogic::OnTableChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnTableChat");
	CLogFuncTime lft(s_LogicFuncTime,"OnTableChat");

	GameDeZhou_TableChat msgChat;
	TransplainMsgCTS(CTSMsg,msgChat);

	if ( pPlayer && pPlayer->GetTableID() == m_pTable->GetTableID() )
	{
		SendLogicMsgToAllPlayer(msgChat);

		DBServerXY::WDB_ChatMsg wdb_chatmsg;
		wdb_chatmsg.m_PID       = pPlayer->GetPID();
		wdb_chatmsg.m_RoomID    = pPlayer->GetRoomID();
		wdb_chatmsg.m_TableID   = pPlayer->GetTableID();
		wdb_chatmsg.m_SitID     = pPlayer->GetSitID();
		wdb_chatmsg.m_ChatMsg   = msgChat.m_ChatMsg;
		LogicSendMsgToDBServer(wdb_chatmsg);
	}
	else
	{
		m_pTable->DebugError("OnTableChat PID=%d msgPID=%d msg=%s",pPlayer->GetPID(),msgChat.m_PID,msgChat.m_ChatMsg.c_str());
	}

	return 0;
}
int CDeZhouGameLogic::OnPrivateChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	return 0;
}

int CDeZhouGameLogic::OnTableQuickChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnTableQuickChat");
	CLogFuncTime lft(s_LogicFuncTime,"OnTableQuickChat");

	GameDeZhou_TableQuickChat msgChat;
	TransplainMsgCTS(CTSMsg,msgChat);

	if ( pPlayer && pPlayer->GetTableID() == m_pTable->GetTableID() )
	{
		SendLogicMsgToAllPlayer(msgChat);

		DBServerXY::WDB_ChatMsg wdb_chatmsg;
		wdb_chatmsg.m_PID       = pPlayer->GetPID();
		wdb_chatmsg.m_RoomID    = pPlayer->GetRoomID();
		wdb_chatmsg.m_TableID   = pPlayer->GetTableID();
		wdb_chatmsg.m_SitID     = pPlayer->GetSitID();
		wdb_chatmsg.m_ChatMsg   = N2S(msgChat.m_Idx);
		LogicSendMsgToDBServer(wdb_chatmsg);
	}
	else
	{
		m_pTable->DebugError("OnTableChat PID=%d msgPID=%d Idx=%d",pPlayer->GetPID(),msgChat.m_PID,msgChat.m_Idx);
	}

	return 0;
}

int CDeZhouGameLogic::OnAddFriend(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnAddFriend");
	CLogFuncTime lft(s_LogicFuncTime,"OnAddFriend");

	GameDeZhou_ReqAddFriend msgAF;
	TransplainMsgCTS(CTSMsg,msgAF);

	if( pPlayer && pPlayer->GetPID()==msgAF.m_FromPID && msgAF.m_ToPID>0 )
	{
		bool bAddSuccess = false;
		PlayerPtr ToPlayer = m_pServer->GetPlayerByPID(msgAF.m_ToPID);
		if ( msgAF.m_Flag==msgAF.Req )
		{
			if ( ToPlayer && IsSameTable(pPlayer,ToPlayer) 
				&& IsAvailSitID(ToPlayer->GetSitID()) && IsAvailSitID(pPlayer->GetSitID()) 		
				&& !(ToPlayer->IsPIDInReqList(msgAF.m_FromPID))
				&& !(IsFriend(pPlayer,ToPlayer)) )
			{
				SendLogicMsgToAllPlayer(msgAF);

				if ( pPlayer->IsPIDInReqList(msgAF.m_ToPID) || ToPlayer->IsFollow(msgAF.m_FromPID) )
				{
					bAddSuccess = true;
					pPlayer->DeleteReqFriendPID(msgAF.m_ToPID);
					ToPlayer->DeleteReqFriendPID(msgAF.m_FromPID);
				}
				else
				{
					ToPlayer->AddReqFriend(msgAF.m_FromPID,m_CurTime);
				}
			}
		}
		else if(msgAF.m_Flag == msgAF.Accept)
		{
			if ( pPlayer->IsPIDInReqList(msgAF.m_ToPID) )
			{
				bAddSuccess = true;
				pPlayer->DeleteReqFriendPID(msgAF.m_ToPID);
			}
		}
		else if ( msgAF.m_Flag == msgAF.Refuse )
		{
			bAddSuccess = false;
			pPlayer->DeleteReqFriendPID(msgAF.m_ToPID);
		}

		if ( bAddSuccess )
		{
			if ( IsSameTable(pPlayer,ToPlayer) )
			{
				GameDeZhou_RespAddFriend msgRespAF;
				msgRespAF.m_LeftPID = msgAF.m_FromPID;
				msgRespAF.m_RightPID = msgAF.m_ToPID;
				SendLogicMsgToAllPlayer(msgRespAF);
			}

			DBServerXY::WDB_UserFriend msgUF;
			msgUF.m_Flag = N_Friend::Both;
			msgUF.m_LeftPID = msgAF.m_FromPID;
			msgUF.m_RightPID = msgAF.m_ToPID;
			m_pTable->SendMsgToDBServer(msgUF);

			pPlayer->m_FriendCount++;
			pPlayer->DeleteReqFriendPID(msgAF.m_ToPID);
			pPlayer->AddFollowFriend(msgAF.m_ToPID);
			pPlayer->AddBeFollowFriend(msgAF.m_ToPID);

			if ( ToPlayer )
			{
				ToPlayer->m_FriendCount++;
				ToPlayer->AddFollowFriend(msgAF.m_FromPID);
				ToPlayer->AddBeFollowFriend(msgAF.m_FromPID);
			}

			CServer* pServer = m_pTable->GetServer();
			if ( pServer && ToPlayer )
			{
				pServer->OnFriendLand(pPlayer,ToPlayer);
				pServer->OnFriendLand(ToPlayer,pPlayer);
			}
		}
	}
	else
	{
		m_pTable->DebugError("OnAddFriend FromID=%d ToID=%d",msgAF.m_FromPID,msgAF.m_ToPID);
	}

	return 0;
}

int CDeZhouGameLogic::OnReqPromoteTime(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnReqPromoteTime");
	CLogFuncTime lft(s_LogicFuncTime,"OnReqPromoteTime");

	GameDeZhou_ReqPromoteTime msgPT;
	TransplainMsgCTS(CTSMsg,msgPT);

	if ( pPlayer && IsRightStep(pPlayer) )
	{
		if ( pPlayer->m_PromoteTimeStart>0 )
		{
			//assert( pPlayer->GetSitID() && pPlayer->GetTable() );
			pPlayer->m_TimePass += (m_CurTime-pPlayer->m_PromoteTimeStart);
			pPlayer->m_PromoteTimeStart = m_CurTime;
		}

		if ( pPlayer->m_TimeStep==msgPT.m_Step && pPlayer->m_TimePass>=m_pGSConfig->m_cfTP.m_StepTime[pPlayer->m_TimeStep-1] )
		{
			INT64 nMoney = m_pGSConfig->m_cfTP.m_StepMoney[pPlayer->m_TimeStep-1];

			GameDeZhou_RespPromoteTime msgRespPT;
			msgRespPT.m_Step       = pPlayer->m_TimeStep;
			msgRespPT.m_PrizeMoney = nMoney;

			pPlayer->m_TimeStep++;
			pPlayer->m_TimePass = 0;
			if ( pPlayer->m_TimeStep > m_pGSConfig->m_cfTP.m_TotalStepTimes )
			{
				pPlayer->m_TimeStep = 0;
			}

			if ( pPlayer->m_TimeStep > 0 )
			{
				int nPlayerStep = pPlayer->m_TimeStep;
				msgRespPT.m_NextStep     = nPlayerStep;
				msgRespPT.m_NextTime     = m_pGSConfig->m_cfTP.m_StepTime[nPlayerStep-1];
				msgRespPT.m_NextMoney    = m_pGSConfig->m_cfTP.m_StepMoney[nPlayerStep-1];
			}
			SendLogicMsgToOnePlayer(msgRespPT,pPlayer);
			SendTimePromote(pPlayer);

			pPlayer->AddGameMoney(nMoney);
			pPlayer->AddMoneyLog(nMoney,"OnReqPromoteTime");
			pPlayer->UpdateGameMoney();

			if( IsAvailSitID(pPlayer->GetSitID()) )
			{
				int nCurSitID = pPlayer->GetSitID();
				CGamePlayer& rTempGPlayer = m_GP[nCurSitID-1];

				rTempGPlayer.AddTableMoney(nMoney);
				SendTableMoneyBySitID(nCurSitID);

				m_pTable->DebugInfo("OnReqPromoteTime PID=%d SitID=%d GSST=%d Money=%s",
					pPlayer->GetPID(),pPlayer->GetSitID(),rTempGPlayer.GetGameState(),N2S(nMoney).c_str() );
			}			

			DBServerXY::WDB_IncomeAndPay msgIP;
			msgIP.m_AID        = pPlayer->GetAID();
			msgIP.m_PID        = pPlayer->GetPID();
			msgIP.m_nMoney     = nMoney;
			msgIP.m_Flag       = N_IncomeAndPay::TimePromote;
			LogicSendMsgToDBServer(msgIP);			
		}
	}

	return 0;
}
int CDeZhouGameLogic::OnLogicReqTailInfo(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnLogicReqTailInfo");
	CLogFuncTime lft(s_LogicFuncTime,"OnLogicReqTailInfo");

	GameDeZhou_ReqTailInfo msgTI;
	TransplainMsgCTS(CTSMsg,msgTI);

	return 0;
}

void CDeZhouGameLogic::SendGameMoneyBySitID( const int SitID,PlayerPtr pPlayer )
{
	if ( IsAvailSitID(SitID) )
	{
		GameDeZhou_GameMoney msgGM;
		msgGM.m_SitID = SitID;

		PlayerPtr TempPlayer = m_pTable->GetSitPlayerBySitID(SitID);
		if ( TempPlayer )
		{
			msgGM.m_nGameMoney = TempPlayer->GetGameMoney(); //- m_GP[SitID-1].GetTableMoney();

			if ( pPlayer )
			{
				SendLogicMsgToOnePlayer(msgGM,pPlayer);
			}
			else
			{
				SendLogicMsgToAllPlayer(msgGM);
			}
		}
		else
		{
			m_pTable->DebugError("SendGameMoneyBySitID SitID=%d",SitID);
		}
	}
}

void CDeZhouGameLogic::SendTableMoneyBySitID(const int SitID,PlayerPtr pPlayer)
{
	if ( IsAvailSitID(SitID) )
	{
		GameDeZhou_TableMoney msgTM;
		msgTM.m_SitID = SitID;

		if ( m_GP[SitID-1].IsInGame() )
		{
			msgTM.m_nTableMoney = m_GP[SitID-1].GetTableMoney();
		}
		else
		{
			msgTM.m_nTableMoney = m_GP[SitID-1].GetTableMoney();
		}		

		if ( pPlayer )
		{
			SendLogicMsgToOnePlayer(msgTM,pPlayer);
		}
		else
		{
			SendLogicMsgToAllPlayer(msgTM);
		}
	}
}

void CDeZhouGameLogic::SendGameState(PlayerPtr pPlayer)
{
	GameDeZhou_GameState gs;
	gs.m_GameST = m_GameState;

	if ( pPlayer )
	{
		SendLogicMsgToOnePlayer(gs,pPlayer);
	}
	else
	{
		SendLogicMsgToAllPlayer(gs);
	}
}

bool CDeZhouGameLogic::IsRightStep(PlayerPtr pPlayer)
{
	assert(pPlayer);
	if ( pPlayer )
	{
		return pPlayer->m_TimeStep>0 && pPlayer->m_TimeStep<=m_pGSConfig->m_cfTP.m_TotalStepTimes ;
	}
	return false;
}

void CDeZhouGameLogic::SitDownPlayerAction(PlayerPtr pPlayer)
{
	//assert(pPlayer->m_PlayerSitTime==0);

	if ( pPlayer->m_PlayerSitTime != 0 )
	{
		m_pTable->DebugError("SitDownPlayerAction AID=%d PID=%d TableID=%d SitID=%d LastSitTime=%s",
			pPlayer->GetAID(),pPlayer->GetPID(),pPlayer->GetTableID(),pPlayer->GetSitID(),Tool::GetTimeString(pPlayer->m_PlayerSitTime).c_str() );
	}	
	pPlayer->m_PlayerSitTime = m_CurTime;
	pPlayer->m_PromoteTimeStart = m_CurTime;

	pPlayer->m_SitDownTimes++;
	if ( IsRightStep(pPlayer) )
	{
		SendTimePromote(pPlayer);
	}
}
void CDeZhouGameLogic::StandUpPlayerAction(PlayerPtr pPlayer)
{
	UINT32 curTime = m_CurTime;
	UINT32 nSitTime = curTime-pPlayer->m_PlayerSitTime;

	assert(pPlayer->m_PlayerSitTime>0);
	assert(nSitTime>=0);

	pPlayer->m_OneLifeGameTime += nSitTime;

	pPlayer->m_TodayGameSecond += nSitTime;

	if ( IsRightStep(pPlayer) )
	{
		pPlayer->m_TimePass += (curTime - pPlayer->m_PromoteTimeStart);
	}

	pPlayer->m_nGameTime += nSitTime;

	pPlayer->m_PlayerSitTime = 0;
	pPlayer->m_PromoteTimeStart = 0;
}
void CDeZhouGameLogic::SitDownTimePromote(PlayerPtr pPlayer,UINT32 curTime)
{
	if ( IsRightStep(pPlayer) )
	{
		if ( pPlayer->m_PromoteTimeStart>0 && curTime - pPlayer->m_PromoteTimeStart > 0 )
		{
			pPlayer->m_TimePass += (curTime - pPlayer->m_PromoteTimeStart);
		}
		pPlayer->m_PromoteTimeStart = m_CurTime;
		SendTimePromote(pPlayer);
	}
}
void CDeZhouGameLogic::StandUpTimePromote(PlayerPtr pPlayer,UINT32 curTime)
{
	if ( IsRightStep(pPlayer) )
	{
		if ( pPlayer->m_PromoteTimeStart>0 && curTime - pPlayer->m_PromoteTimeStart > 0 )
		{
			pPlayer->m_TimePass += (curTime - pPlayer->m_PromoteTimeStart);
		}
		else
		{
			
		}
		pPlayer->m_PromoteTimeStart = 0;
	}
}

void CDeZhouGameLogic::SendTimePromote(PlayerPtr pPlayer)
{
	GameDeZhou_PromoteTime msgPT;
	if ( IsRightStep(pPlayer) )
	{
		int PlayerStep = pPlayer->m_TimeStep;

		msgPT.m_Step       = PlayerStep;
		msgPT.m_LeftTime   = max(0,m_pGSConfig->m_cfTP.m_StepTime[PlayerStep-1] - pPlayer->m_TimePass);
		msgPT.m_AwardMoney = m_pGSConfig->m_cfTP.m_StepMoney[PlayerStep-1];
	}
	else
	{
		msgPT.m_Step       = 0;
		msgPT.m_LeftTime   = 0;
		msgPT.m_AwardMoney = 0;
	}
	SendLogicMsgToOnePlayer(msgPT,pPlayer);
}

void CDeZhouGameLogic::WriteToDB(stWinLossTally* pWLT,int nLen)
{
	TraceStackPath logTP("CDeZhouGameLogic::WriteToDB");
	CLogFuncTime lft(s_LogicFuncTime,"WriteToDB");
	
	m_pTable->DebugInfo("WriteToDB Start m_nGameLossMoney=%s m_nGameWinLossMoney=%s",
		Tool::N2S(m_nGameLossMoney).c_str(),Tool::N2S(m_nGameWinLossMoney).c_str() );

	if ( nLen>0 && nLen <= MAX_PALYER_ON_TABLE )
	{
		for (int i=0;i<nLen;i++)
		{
			m_nGameWinLossMoney += pWLT[i].m_nWinLossMoney;
		}
		m_nGameWinLossMoney += m_nGameLossMoney;

		if ( m_nGameWinLossMoney == 0 )
		{
			DBServerXY::WDB_WinLossList wll;
			DBServerXY::WDB_WinLoss wl;

			for (int i=0;i<nLen;i++)
			{
				if ( pWLT[i].m_PID > 0 )
				{
					PlayerPtr TempPlayer = m_pTable->GetTablePlayerByPID(pWLT[i].m_PID);
					if ( TempPlayer )
					{
						TempPlayer->Lock(m_CurTime);						
					}

					wl.ReSet();
					wl.m_GameGUID          = m_GameLogicGUID;
					wl.m_AID               = pWLT[i].m_AID;
					wl.m_PID               = pWLT[i].m_PID;
					wl.m_RoomID            = m_pTable->GetRoomID();
					wl.m_TableID           = m_pTable->GetTableID();
					wl.m_SitID             = i + 1;
					wl.m_nServiceMoney     = pWLT[i].m_nServiceMoney;
					wl.m_nWinLossMoney     = pWLT[i].m_nWinLossMoney;
					wl.m_EndGameFlag       = pWLT[i].m_EndGameFlag;
					wl.m_PaiType           = pWLT[i].m_PaiType;
					wl.m_LeftPai           = pWLT[i].m_LeftPai;
					wl.m_RightPai          = pWLT[i].m_RightPai;

					wl.m_Money             = pWLT[i].m_Money;
					wl.m_JF                = pWLT[i].m_JF;
					wl.m_EP                = pWLT[i].m_EP;

					wll.m_WinLoss[i] = wl;
				}
			}

			m_pServer->SendMsgToDBServer(wll);
		}
		else
		{
			m_pTable->DebugError("CGameTable::reportWriteToDB tableid=%d GameWinLossMoney=%s",m_pTable->GetTableID(),Tool::N2S(m_nGameWinLossMoney).c_str());
		}
	}

	//m_pTable->DebugInfo("WriteToDB End");
}

bool CDeZhouGameLogic::GetSitPlayerInfo(PlayerPtr pPlayer,GameDeZhou_SitPlayerInfo& spi)
{
	TraceStackPath logTP("CDeZhouGameLogic::GetSitPlayerInfo");
	CLogFuncTime lft(s_LogicFuncTime,"GetSitPlayerInfo");

	bool ret = false;
	if ( pPlayer )
	{
		BYTE SitID = pPlayer->GetSitID();
		BYTE Sit = pPlayer->GetSitID() - 1;
		if ( IsAvailSitID(SitID) )
		{
			if (m_GP[Sit].IsHavePlayer() && m_GP[Sit].GetPID() == pPlayer->GetPID())
			{
				ret = true;

				spi.m_SitID              = pPlayer->GetSitID();
				spi.m_PID                = pPlayer->GetPID();
				spi.m_nJF                = pPlayer->GetJF();
				spi.m_nExperience        = pPlayer->GetExperience();
				spi.m_nWinTimes          = pPlayer->m_nWinTimes;
				spi.m_nLossTimes         = pPlayer->m_nLossTimes;
				spi.m_MaxPai             = pPlayer->GetMaxPai();
				spi.m_GameLevel          = pPlayer->m_GameLevel;
				spi.m_VipLevel           = pPlayer->m_VipLevel;
				spi.m_Sex                = pPlayer->m_Sex;
				spi.m_HomePageURL        = pPlayer->GetHomePageURL();
			}
		}
	}
	if ( !ret )
	{
		m_pTable->DebugError("GetSitPlayerInfo PID=%d",pPlayer->GetPID());
	}

	return ret;
}

void CDeZhouGameLogic::SendGameToPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CDeZhouGameLogic::SendGameToPlayer");
	CLogFuncTime lft(s_LogicFuncTime,"SendGameToPlayer");
	m_pTable->DebugInfo("SendGameToPlayer Start PID=%d",pPlayer->GetPID());

	GameDeZhou_ReLink rlStart;
	rlStart.m_Flag = rlStart.Start;
	SendLogicMsgToOnePlayer(rlStart,pPlayer);

	SendTimePromote(pPlayer);

	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		PlayerPtr pTempPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
		if ( pTempPlayer )
		{
			if ( m_GP[Sit].IsHavePlayer() && m_GP[Sit].GetPID() == pTempPlayer->GetPID() )
			{
				GameDeZhou_SitPlayerInfo spi;
				if( GetSitPlayerInfo(pTempPlayer,spi) )
				{
					SendLogicMsgToOnePlayer(spi,pPlayer);
				}
			}
			else
			{
				m_pTable->DebugError("SendGameToPlayer TableID=%d SitID=%d PID=%d GPID=%d",m_pTable->GetTableID(),Sit+1,pTempPlayer->GetPID(),m_GP[Sit].GetPID());
			}
		}
		else
		{
			if ( m_GP[Sit].IsHavePlayer() )
			{
				m_pTable->DebugError("SendGameToPlayer TableID=%d SitID=%d GPID=%d",m_pTable->GetTableID(),Sit+1,m_GP[Sit].GetPID());
			}
		}
	}
	
	SendGameState(pPlayer);

	GameDeZhou_GamePlayerInfo gpi;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() )
		{
			gpi.m_GamePlayerInfo[Sit].m_SitID             = Sit+1;
			gpi.m_GamePlayerInfo[Sit].m_GameState         = m_GP[Sit].GetGameState();
			gpi.m_GamePlayerInfo[Sit].m_nTableMoney       = m_GP[Sit].GetTableMoney();
			if ( IsGamePlaying() && m_GP[Sit].IsInGame() )
			{
			}
		}
	}
	SendLogicMsgToOnePlayer(gpi,pPlayer);

	for(int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() )
		{
			SendTableMoneyBySitID(Sit+1,pPlayer);
			SendGameMoneyBySitID(Sit+1,pPlayer);
		}
	}
	
	GameDeZhou_ReLink rlEnd;
	rlEnd.m_Flag = rlEnd.End;
	SendLogicMsgToOnePlayer(rlEnd,pPlayer);

	if( IsGamePlaying() )
	{
		if ( m_pTable->m_IsHaveCommonPlayer && m_DelayTime>m_WaitChipTime )
		{
			m_DelayTime = m_WaitChipTime;
		}
	}

	//m_pTable->DebugInfo("SendGameToPlayer End");
}

void CDeZhouGameLogic::PlayerJoinGame(PlayerPtr pPlayer)
{
}

void CDeZhouGameLogic::LogicStandUpPlayer(PlayerPtr pPlayer,int SitID,int Flag)
{
	TraceStackPath logTP("CDeZhouGameLogic::LogicStandUpPlayer");
	CLogFuncTime lft(s_LogicFuncTime,"LogicStandUpPlayer");
	m_pTable->DebugInfo("CDeZhouGameLogic::LogicStandUpPlayer SitID=%d PID=%d PSitID=%d",SitID,pPlayer->GetPID(),pPlayer->GetSitID());

	if ( pPlayer->IsBotPlayer() )
	{
		m_pTable->RemoveBotPlayer(pPlayer->GetPID());
	}
	else
	{
		m_pTable->PlayerStandUp(pPlayer,SitID,Flag);
	}

	m_pTable->DebugInfo("CDeZhouGameLogic::LogicStandUpPlayer End........");
}

void CDeZhouGameLogic::GamePlayerStandUp( PlayerPtr pPlayer,int SitID,int Flag )
{
	TraceStackPath logTP("CDeZhouGameLogic::GamePlayerStandUp");
	CLogFuncTime lft(s_LogicFuncTime,"GamePlayerStandUp");

	m_pTable->DebugInfo("CDeZhouGameLogic::GamePlayerStandUp SitID=%d PID=%d PSitID=%d",SitID,pPlayer->GetPID(),pPlayer->GetSitID());

	if ( IsAvailSitID(SitID) && m_GP[SitID-1].IsHavePlayer() && m_GP[SitID-1].GetPID()==pPlayer->GetPID() )
	{
		int Sit = SitID - 1;
	}
	else
	{
		m_pTable->DebugError("GamePlayerStandUp TableID=%d SitID=%d",m_pTable->GetTableID(),SitID);
		if ( IsAvailSitID(SitID) )
		{
			m_pTable->DebugError("State=%d GPPID=%d PID=%d",m_GP[SitID-1].GetGameState(),m_GP[SitID-1].GetPID(),pPlayer->GetPID());
		}
	}

	m_pTable->DebugInfo("CDeZhouGameLogic::GamePlayerStandUp End........");
}

void CDeZhouGameLogic::GamePlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag)
{
	TraceStackPath logTP("CDeZhouGameLogic::GamePlayerSitDown");
	CLogFuncTime lft(s_LogicFuncTime,"GamePlayerSitDown");
	m_pTable->DebugInfo("CDeZhouGameLogic::GamePlayerSitDown Start SitID=%d",SitID);
	
	if ( pPlayer && IsAvailSitID(SitID) && m_GP[SitID-1].IsExit() )
	{
		GameDeZhou_SitPlayerInfo spi;
		if( GetSitPlayerInfo(pPlayer,spi) )
		{
			SendLogicMsgToAllPlayer(spi);
		}

		SendTableMoneyBySitID(SitID);
		SendGameMoneyBySitID(SitID);
	}
	else
	{
		m_pTable->DebugError("GamePlayerSitDown TableID=%d SitID=%d TakeMoney=%s IsExit=%d",
			m_pTable->GetTableID(),SitID,Tool::N2S(nTakeMoney).c_str(),m_GP[SitID-1].IsExit());
	}

	m_pTable->DebugInfo("CDeZhouGameLogic::GamePlayerSitDown End........");
}