#include ".\dezhougamelogic.h"

#include "Server.h"
#include "GameRoom.h"
#include "GameTable.h"

#include "CGameServerConfig.h"
#include "MatchRule.h"

int CDeZhouGameLogic::s_nWinTimes[MAX_PALYER_ON_TABLE] = {0};
MapBotFace CDeZhouGameLogic::s_mapBotFace;
MapBotChat CDeZhouGameLogic::s_mapBotChat;
CMapFunctionTime CDeZhouGameLogic::s_LogicFuncTime;

VectorFaceID CDeZhouGameLogic::s_KaiXinFaceID;
VectorFaceID CDeZhouGameLogic::s_ShangXinFaceID;
VectorFaceID CDeZhouGameLogic::s_WuLiaoFaceID;
VectorFaceID CDeZhouGameLogic::s_WinFaceID;
VectorFaceID CDeZhouGameLogic::s_LossFaceID;
VectorFaceID CDeZhouGameLogic::s_ThankFaceID;

VectorGiftID CDeZhouGameLogic::s_ThankGiftID;

int CDeZhouGameLogic::InitStaticData()
{
	s_KaiXinFaceID.push_back(1);
	s_KaiXinFaceID.push_back(2);
	s_KaiXinFaceID.push_back(6);
	s_KaiXinFaceID.push_back(11);
	s_KaiXinFaceID.push_back(14);

	s_ShangXinFaceID.push_back(3);
	s_ShangXinFaceID.push_back(8);
	s_ShangXinFaceID.push_back(9);
	s_ShangXinFaceID.push_back(13);
	s_ShangXinFaceID.push_back(18);

	s_WuLiaoFaceID.push_back(2);
	s_WuLiaoFaceID.push_back(6);
	s_WuLiaoFaceID.push_back(7);
	s_WuLiaoFaceID.push_back(10);
	s_WuLiaoFaceID.push_back(12);
	s_WuLiaoFaceID.push_back(15);
	s_WuLiaoFaceID.push_back(17);
	s_WuLiaoFaceID.push_back(19);
	s_WuLiaoFaceID.push_back(20);

	s_ThankFaceID.push_back(1);
	s_ThankFaceID.push_back(2);
	s_ThankFaceID.push_back(4);
	s_ThankFaceID.push_back(11);

	s_WinFaceID.push_back(1);
	s_WinFaceID.push_back(4);
	s_WinFaceID.push_back(11);
	s_WinFaceID.push_back(14);

	s_LossFaceID.push_back(3);
	s_LossFaceID.push_back(8);
	s_LossFaceID.push_back(9);
	s_LossFaceID.push_back(13);
	s_LossFaceID.push_back(18);

	s_ThankGiftID.push_back(1);
	s_ThankGiftID.push_back(4);
	s_ThankGiftID.push_back(6);
	s_ThankGiftID.push_back(7);
	s_ThankGiftID.push_back(8);
	s_ThankGiftID.push_back(103);

	return 0;
}
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

	if ( m_pRoom->IsTableMatch() )
	{
		m_pMatchRule = new CMatchRule();
	}
	else if ( m_pRoom->IsRoomMatch() )
	{
		m_pMatchRule = m_pRoom->GetRoomMatchRule();
	}

	m_pGSConfig = m_pServer->GetGSConfig();

	InitGameLogic();
}

CDeZhouGameLogic::~CDeZhouGameLogic(void)
{
	safe_delete(m_pGameLog);
	if( IsTableMatch() )
	{
		safe_delete(m_pMatchRule);
	}	
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

	m_BankerSitID = 0;
	m_BigBlindSitID = 0;
	m_SmallBlindSitID = 0;
	m_CurPlayerSitID = 0;

	m_CanShowPaiSitID = 0;
	m_FirstPlayerSitID = 0;
	m_EndPlayerSitID = 0;

	m_MaxChipOnTable = 0;
	m_MinAddChip = 0;

	m_ActionTime = 0;
	m_DelayTime = 20;

	m_ForbidenPlayerList.clear();

	memset(m_TablePoolMoneys,0, sizeof(m_TablePoolMoneys));

	for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
	{
		m_GP[Sit].Init();
		m_Tallys[Sit].ReSet();
	}

	m_TableType = TABLE_TYPE_COMMON;
	m_nPotMoney = 0;
	m_nMinLeftForGame = 1;
	m_MaxSitPlayerNumber = MAX_PALYER_ON_TABLE;
	m_MinPlayerForGame = MIN_PLAYER_FOR_GAME;
	m_WaitStartTime = 2;
	m_Limite = TABLE_LIMITE_NO;

	m_ManageBotPlayerTime = m_pServer->GetCurTime();
	m_timeAddBotPlayer    = m_pServer->GetCurTime();

	m_TableBotFlag = Table_Bot_None;
	m_TableBotPlayerNumber = 0;
	m_MaxBotChipTime = 5;
	m_GiftBase = m_nBigBlind;
	m_FaceBase = m_nBigBlind;

	m_BotLossEveryTime = 0;
	m_BotTargetWinLoss = 0;
	m_BotRealWinLoss = 0;

	m_bEndTable = false;
	m_bAllKanPai = false;
	m_bNeedBotWin = false;

	InitGameRule();
	m_ClientTableRule = GetGameRule();

	m_mapTableWinLoss.clear();
	m_vectorWinLossPID.clear();
	m_vecKikOutPID.clear();

	m_BankerSitID = Tool::CRandom::Random_Int(1,m_MaxSitPlayerNumber);
	m_BigBlindSitID = ((m_BankerSitID+1)%m_MaxSitPlayerNumber);
	m_BigBlindSitID = (m_BigBlindSitID==0) ? m_MaxSitPlayerNumber:m_BigBlindSitID;
	m_SmallBlindSitID = ((m_BankerSitID+2)%m_MaxSitPlayerNumber);
	m_SmallBlindSitID = (m_SmallBlindSitID==0) ? m_MaxSitPlayerNumber:m_SmallBlindSitID;

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

	if ( m_pRoom->IsMatchRoom() )
	{
		m_Limite = TABLE_LIMITE_NO;
		m_nServiceMoney = 0;
		m_nPotMoney = 0;
		m_nMinLeftForGame = 1;

		m_MaxSitPlayerNumber = MAX_PALYER_ON_TABLE;
		m_MinPlayerForGame = MIN_PLAYER_FOR_GAME;

		if ( m_pRoom->IsRoomMatch() )
		{
			m_TableType = TABLE_TYPE_RoomMatch;

			m_nBigBlind = m_pMatchRule->m_BigBlind;
			m_nSmallBlind = m_pMatchRule->m_SmallBlind;

			m_nMinTakeIn = m_pMatchRule->m_TakeMoney;
			m_nMaxTakeIn = m_pMatchRule->m_TakeMoney*2;
		}
		else if ( m_pRoom->IsTableMatch() )
		{
			m_TableType = TABLE_TYPE_TableMatch;
			m_pTable->SetTableMatchState(MatchTable_ST_MatchOver);

			strRule = m_pTable->GetTableMatchRule();
			mapRule = Tool::GetRuleMapInData(strRule,string(";"),string("="));
			itorKey = mapRule.find("matchid");
			if ( itorKey != mapRule.end())
			{
				m_pMatchRule->m_MatchID = INT32(itorKey->second);
				assert(m_pMatchRule->m_MatchID>0);
			}

			strRule = m_pTable->GetTableRule();
			mapRule = Tool::GetRuleMapInData(strRule,string(";"),string("="));

			m_pMatchRule->m_MinPlayerForMatch = MAX_PALYER_ON_TABLE;
			itorKey = mapRule.find("minplayer");
			if ( itorKey != mapRule.end())
			{
				m_pMatchRule->m_MinPlayerForMatch = INT32(itorKey->second);
				assert(m_pMatchRule->m_MinPlayerForMatch>=MIN_PLAYER_FOR_GAME);
			}
			assert( m_pMatchRule->m_MinPlayerForMatch>=2 && m_pMatchRule->m_MinPlayerForMatch<=MAX_PALYER_ON_TABLE);

			itorKey = mapRule.find("chiptime");
			if ( itorKey != mapRule.end() )
			{
				m_WaitChipTime = INT32(itorKey->second);
			}
			if ( !(m_WaitChipTime>=T_ChipTime_Quick && m_WaitChipTime<=T_ChipTime_Long) )
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

			MoGui::Game::DeZhou::GS::stMatchInfo msgMI;
			m_pServer->GetMatchInfo(m_pMatchRule->m_MatchID,msgMI);
			m_pMatchRule->m_MatchType   = msgMI.m_MatchType;
			m_pMatchRule->m_MatchTicket = msgMI.m_Ticket;
			m_pMatchRule->m_TakeMoney   = msgMI.m_TakeMoney;
			m_pMatchRule->m_StartMoney  = msgMI.m_StartMoney;

			m_pMatchRule->m_vecBlind = msgMI.m_vecBlind;
			m_pMatchRule->m_vecTime  = msgMI.m_vecTime;
			m_pMatchRule->m_vecAwardMoney = msgMI.m_vecAwardMoney;
			m_pMatchRule->m_vecAwardJF = msgMI.m_vecAwardJF;

			m_pMatchRule->InitBlind(m_CurTime);

			m_nBigBlind = m_pMatchRule->m_BigBlind;
			m_nSmallBlind = m_pMatchRule->m_SmallBlind;

			m_nMinTakeIn = m_pMatchRule->m_TakeMoney;
			m_nMaxTakeIn = m_pMatchRule->m_TakeMoney*2;
		}
	}
	else
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

		itorKey = mapRule.find("smallblind");
		if ( itorKey != mapRule.end())
		{
			m_nSmallBlind = INT32(itorKey->second);
		}
		else
		{
			m_pTable->DebugError("Init Logic Error m_TableID=%d smallblind=%d",m_pTable->GetTableID(),m_nSmallBlind);
		}

		itorKey = mapRule.find("bigblind");
		if ( itorKey != mapRule.end())
		{
			m_nBigBlind = INT32(itorKey->second);
		}
		else
		{
			m_pTable->DebugError("Init Logic Error m_TableID=%d m_nBigBlind=%d",m_pTable->GetTableID(),m_nBigBlind);
		}
		if ( !(m_nSmallBlind>0 && m_nBigBlind>m_nSmallBlind) )
		{
			m_pTable->DebugError("SmallBlind=%d,BigBlind=%d",m_nSmallBlind,m_nBigBlind);
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

		itorKey = mapRule.find("mintake");
		if ( itorKey != mapRule.end() && m_nBigBlind > 0 )
		{
			m_nMinTakeIn = (itorKey->second)*m_nBigBlind;
		}
		else
		{
			m_pTable->DebugError("Init Logic Error m_TableID=%d m_nMinTakeIn=%s",m_pTable->GetTableID(),Tool::N2S(m_nMinTakeIn).c_str() );		
		}

		itorKey = mapRule.find("maxtake");
		if ( itorKey != mapRule.end() && m_nBigBlind > 0 )
		{
			m_nMaxTakeIn = (itorKey->second)*m_nBigBlind;
		}
		else
		{
			m_pTable->DebugError("Init Logic Error m_TableID=%d m_nMaxTakeIn=%s",m_pTable->GetTableID(),Tool::N2S(m_nMaxTakeIn).c_str() );
		}
		if ( !(m_nMinTakeIn>0 && m_nMaxTakeIn>=m_nMinTakeIn) )
		{
			m_pTable->DebugError("Roomid=%d tableid=%d mintake=%s maxtake=%s",
				m_pTable->GetRoomID(),m_pTable->GetTableID(),N2S(m_nMinTakeIn).c_str(),N2S(m_nMaxTakeIn).c_str() );
		}

		m_nPotMoney = 0;
		itorKey = mapRule.find("pot");
		if ( itorKey != mapRule.end() )
		{
			m_nPotMoney = INT32(itorKey->second);
		}
		if ( m_nPotMoney < 0 )
		{
			m_pTable->DebugError("Pot=%d",m_nPotMoney);
		}

		m_nMinLeftForGame = 1;
		itorKey = mapRule.find("minforgame");
		if ( itorKey != mapRule.end() )
		{
			m_nMinLeftForGame = INT32(itorKey->second)*m_nBigBlind;
		}
		m_nMinLeftForGame = max(1,m_nMinLeftForGame);

		m_MaxSitPlayerNumber = MAX_PALYER_ON_TABLE;
		itorKey = mapRule.find("maxsit");
		if ( itorKey != mapRule.end() )
		{
			m_MaxSitPlayerNumber = INT32(itorKey->second);
		}
		if ( !(m_MaxSitPlayerNumber>=2 && m_MaxSitPlayerNumber<=MAX_PALYER_ON_TABLE))
		{
			m_pTable->DebugError("MaxPlayer=%d",m_MaxSitPlayerNumber);
		}

		m_MinPlayerForGame = MIN_PLAYER_FOR_GAME;
		itorKey = mapRule.find("minplayer");
		if ( itorKey != mapRule.end() )
		{
			m_MinPlayerForGame = INT32(itorKey->second);
		}
		if ( !(m_MinPlayerForGame>=2 && m_MinPlayerForGame<=MAX_PALYER_ON_TABLE))
		{
			m_pTable->DebugError("MinPlayerForGame=%d",m_MinPlayerForGame);
		}

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

		m_Limite = TABLE_LIMITE_NO;
		itorKey = mapRule.find("limite");
		if ( itorKey != mapRule.end() )
		{
			m_Limite = BYTE(itorKey->second);
		}
		if ( !(m_Limite>=TABLE_LIMITE_NO && m_Limite<=TABLE_LIMITE_POT) )
		{
			m_pTable->DebugError("limite=%d",m_Limite);
		}

		if ( !(m_nBigBlind>0&&m_nSmallBlind>0&&m_nMinTakeIn>0&&m_nMaxTakeIn>0&&m_nMinLeftForGame>=0&&m_nPotMoney>=0&&m_nServiceMoney>=0))
		{
			m_pTable->DebugError("规则创建出错 tableid=%d",m_pTable->GetTableID());
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

	m_GiftBase = m_nBigBlind;
	itorKey = mapRule.find("giftbase");
	if ( itorKey != mapRule.end() )
	{
		m_GiftBase = INT32(itorKey->second);
	}

	m_FaceBase = m_nBigBlind;
	itorKey = mapRule.find("facebase");
	if ( itorKey != mapRule.end() )
	{
		m_FaceBase = INT32(itorKey->second);
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
	//m_pTable->CeShiInfo("CDeZhouGameLogic::OnTime Start RoomId=%d TableID=%d",m_pTable->GetRoomID(),m_pTable->GetTableID());

	m_CurTime = curTime;
	if ( IsGameIdle() && m_CurTime >= m_StartGameTime )
	{
		if ( CanStartGame() )
		{
			if ( IsMatchTable() )
			{
				StartMatchGame();
			}
			else
			{
				StartCommonGame();
			}
		}
	}

	if ( IsTableMatch() && IsMatching() )
	{
		if ( m_pMatchRule->AddBlind(m_CurTime) )
		{
			SendChangeBlind(m_pMatchRule->m_BigBlind,m_pMatchRule->m_SmallBlind);
		}
	}

	OnTimeManageBotPlayer(m_CurTime);

	if ( m_CurTime >= m_timeAddBotPlayer )
	{
		m_timeAddBotPlayer = m_CurTime + CRandom::Random_Int(m_pGSConfig->m_cfMatch.m_minTimeAddBotPlayer,m_pGSConfig->m_cfMatch.m_maxTimeAddBotPlayer);
		OnTimeAddTableMatchBotPlayer();
	}

	if ( N_CeShiLog::c_BotPlayerLeave )  //这里也是测试用的代码 测试玩家断线离开
	{
		int TempSit = Tool::CRandom::Random_Int(0,100);
		if ( TempSit<m_MaxSitPlayerNumber && m_GP[TempSit].IsBotPlayer() )
		{
			if ( CRandom::GetChangce(100,1) )
			{
				m_pTable->RemoveBotPlayer(m_GP[TempSit].GetPID());
			}
		}
	}

	OnTimeAutoChip();
	DoShowFaceMsg();
	DoSendGiftMsg();

	if ( m_GameState == GAME_ST_DIVIDEMONEY )
	{
		DoWashOutPlayer();
		if ( m_CurTime - m_ActionTime >= m_DelayTime )
		{
			OnDivideMoneyEnd();
		}
	}

	//m_pTable->CeShiInfo("CDeZhouGameLogic::OnTime End... ");
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
					m_pTable->AddBotPlayer(m_nBigBlind);
				}
			}
			else if ( CRandom::GetChangce(10,1) && CommPlayerNumber<=3 /* && m_pRoom->GetRealPlayerCount()<100 */ )
			{
				m_pTable->AddBotPlayer(m_nBigBlind);
			}
		}
	}
	if ( m_TableBotFlag == Table_Bot_Flow )
	{
		if ( CommPlayerNumber>0 && CurSitNumber<=3 && CRandom::GetChangce(3,1) )  //玩家较少的情况下，给玩家补充机器人
		{
			m_pTable->AddBotPlayer(m_nBigBlind);
		}
	}
}

void CDeZhouGameLogic::OnTimeAddTableMatchBotPlayer()
{
	if ( IsTableMatch() && (!IsMatching()) && m_pTable->GetTableMatchState()==MatchTable_ST_WaitSign 
		&& m_pTable->GetSitPlayerNumber() < m_pMatchRule->m_MinPlayerForMatch 
		&& m_pRoom->GetBotPlayerCount() < m_pGSConfig->m_cfMatch.m_nMaxBotInRoom )
	{
		CLogFuncTime lft(s_LogicFuncTime,"OnTimeAddTableMatchBotPlayer");

		int nSitPlayer = GetSitPlayerNumber();
		int nBotPlayer = GetSitBotPlayerNumber();
		if ( (nSitPlayer-nBotPlayer)>0 || (CRandom::GetChangce(nSitPlayer/3+1,1)) )
		{
			m_pTable->AddBotPlayer(m_pMatchRule->GetStartMoney()/20);
		}
	}	
}

void CDeZhouGameLogic::OnTimeAutoChip()
{	
	if ( IsGamePlaying() )
	{
		CLogFuncTime lft(s_LogicFuncTime,"OnTimeAutoChip");

		if ( m_CurTime-m_ActionTime > m_DelayTime )
		{
			if ( IsAvailSitID(m_CurPlayerSitID) )
			{
				int Sit = m_CurPlayerSitID - 1;
				if( m_GP[Sit].IsBotPlayer() )
				{
					if ( m_pGSConfig->m_cfServer.m_BotAutoPlayer )
					{
						BotPlayerChip();
					}
					else
					{
						if ( m_pTable->m_IsHaveCommonPlayer )
						{
							BotPlayerChip();
						}
						else
						{
							m_ActionTime = m_CurTime;
						}
					}
				}
				else
				{
					AutoChip();
				}
			}
			else
			{
				m_pTable->DebugError("OnTime TableID=%d CurSitID=%d",m_pTable->GetTableID(),m_CurPlayerSitID);
			}
		}
	}
}

string CDeZhouGameLogic::GetGameRule()
{
	string strGameRule = "";
	if ( IsMatchTable() )
	{
		stringstream ssIn;
		ssIn<<"smallblind="      <<m_pMatchRule->m_SmallBlind   <<";"
			<<"bigblind="        <<m_pMatchRule->m_BigBlind     <<";"			
			<<"ticket="          <<m_pMatchRule->m_MatchTicket  <<";"
			<<"takemoney="       <<m_pMatchRule->m_TakeMoney    <<";"
			<<"tabletype="       <<int(m_TableType)             <<";"
			<<"limite="          <<int(TABLE_LIMITE_NO)         <<";"
			<<"pot="             <<int(0)                       <<";"
			<<"facebase="        <<m_FaceBase                   <<";"
			<<"giftbase="        <<m_GiftBase                   <<";"
			<<"chiptime="        <<int(m_WaitChipTime)          <<";"
			<<"password="        <<int(0)                       <<";"
			<<endl;
		strGameRule = ssIn.str();
	}
	else
	{
		int Password = (m_pTable->IsNeedPassword()?1:0);
		int MinTake = 0,MaxTake = 0;
		if ( m_nBigBlind > 0 )
		{
			MinTake = int(m_nMinTakeIn/m_nBigBlind);
			MaxTake = int(m_nMaxTakeIn/m_nBigBlind);
		}
		else
		{
			m_pTable->DebugError("GetGameRule RoomID=%d m_nBigBlind=%d",m_pTable->GetRoomID(),m_nBigBlind);
		}
		string strMark;
		if ( m_Limite == TABLE_LIMITE_NO)
		{
			strMark += "无限";
		}
		else if ( m_Limite == TABLE_LIMITE_TEXAS )
		{
			strMark += "限注";
		}
		else if ( m_Limite == TABLE_LIMITE_POT )
		{
			strMark += "限池";
		}
		if ( m_nPotMoney > 0 )
		{
			strMark += "有底";
		}
		if ( !strMark.empty() )
		{
			strMark += "桌";
		}		

		stringstream ssIn;
		ssIn<<"smallblind="        <<m_nSmallBlind                <<";"
			<<"bigblind="          <<m_nBigBlind                  <<";"
			<<"service="           <<m_nServiceMoney              <<";"
			<<"mintake="           <<MinTake                      <<";"
			<<"maxtake="           <<MaxTake                      <<";"
			<<"tabletype="         <<int(m_TableType)             <<";"
			<<"pot="               <<m_nPotMoney                  <<";"
			<<"limite="            <<int(m_Limite)                <<";"
			<<"facebase="          <<m_FaceBase                   <<";"
			<<"giftbase="          <<m_GiftBase                   <<";"
			<<"chiptime="          <<int(m_WaitChipTime)          <<";"
			<<"password="          <<Password                     <<";"
			<<"mark="              <<strMark                      <<";"
			<<endl;
		strGameRule = ssIn.str();
	}
	return strGameRule;
}

void CDeZhouGameLogic::DebugLogicState()
{
	m_pTable->ReleaseInfo("DebugAllPlayer:State=%d",m_GameState);

	m_pTable->ReleaseInfo("BigSit=%d SmallSit=%d CurSit=%d EndSit=%d FirstSit=%d",
		m_BigBlindSitID,m_SmallBlindSitID,m_CurPlayerSitID,m_EndPlayerSitID,m_FirstPlayerSitID );

	for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit)
	{
		DebugSitPlayer(Sit+1);
	}
}

void CDeZhouGameLogic::DebugAllPlayer()
{
	m_pTable->ReleaseInfo("DebugAllPlayer:State=%d",m_GameState);
	m_pTable->ReleaseInfo("BigSit=%d SmallSit=%d CurSit=%d EndSit=%d FirstSit=%d",
		m_BigBlindSitID,m_SmallBlindSitID,m_CurPlayerSitID,m_EndPlayerSitID,m_FirstPlayerSitID );
	m_pGameLog->Print();
}

void CDeZhouGameLogic::DebugSitPlayer(int SitID)
{
	if ( IsAvailSitID(SitID) )
	{
		int Sit = SitID - 1;
		m_pTable->DebugInfo("DebugSitPlayer SitID=%d PID=%d GameState=%d TableMoney=%s curMoney=%s ",
			SitID,m_GP[Sit].GetPID(),m_GP[Sit].GetGameState(),
			Tool::N2S(m_GP[Sit].GetTableMoney()).c_str(),Tool::N2S(m_GP[Sit].GetCurChip()).c_str() );
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

bool CDeZhouGameLogic::GetGiftInfo(INT16 GiftID,DBServerXY::DBS_msgGiftInfo& GiftInfo)
{
	return m_pServer->GetGiftInfo(GiftID,GiftInfo);	
}
bool CDeZhouGameLogic::GetFaceInfo(INT16 FaceID,DBServerXY::DBS_msgFaceInfo& FaceInfo)
{	
	return m_pServer->GetFaceInfo(FaceID,FaceInfo);	
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
		if ( m_GP[Sit].IsPlaying() || (m_GP[Sit].IsAllIn()&&m_GP[Sit].GetCurChip()>0) )
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

BYTE CDeZhouGameLogic::GetPrePlayingPlayerSitID( BYTE CurSitID)
{
	int LogSitID = CurSitID;
	if( CurSitID>0 && CurSitID<=m_MaxSitPlayerNumber )
	{
		int TempSitID = CurSitID;
		for (;;)
		{
			if( --CurSitID <= 0 )
			{
				CurSitID += m_MaxSitPlayerNumber;
			}
			if ( CurSitID == TempSitID )
			{
				break;
			}

			if ( m_GP[CurSitID-1].IsPlaying() )
			{
				return CurSitID;
			}
		}
	}

	m_pTable->DebugError("GetPrePlayingPlayerSitID PreCurSitID=%d SitID=%d",LogSitID,CurSitID);

	return 0;
}

BYTE CDeZhouGameLogic::GetNextPlayingPlayerSitID( BYTE CurSitID)
{
	int TempSitID = CurSitID;
	if( CurSitID>=0 && CurSitID<=m_MaxSitPlayerNumber )
	{
		for (;;)
		{
			if ( ++CurSitID > m_MaxSitPlayerNumber )
			{
				CurSitID -= m_MaxSitPlayerNumber;
			}
			if ( CurSitID == TempSitID )
			{
				break;
			}
			if ( m_GP[CurSitID-1].IsPlaying() )
			{
				return CurSitID;
			}
		}
	}

	//m_pTable->DebugError("CGameTable::GetNextPlayingPlayerSitID CurSitID=%d",TempSitID);

	return 0;
}

bool CDeZhouGameLogic::IsMatching()const 
{
	if ( IsMatchTable())
	{
		return m_pMatchRule->m_bMatching;
	}
	return false;
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
INT64 CDeZhouGameLogic::GetCurMaxChipOnTable()
{
	assert(m_GameState>=GAME_ST_HAND && m_GameState<=GAME_ST_RIVER);

	INT64 nMaxChip = 0;
	if ( m_GameState>=GAME_ST_HAND && m_GameState<=GAME_ST_RIVER )
	{
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
		{
			if ( m_GP[Sit].GetCurChip() > nMaxChip )
			{
				nMaxChip = m_GP[Sit].GetCurChip();
			}
		}
	}
	return nMaxChip;
}

INT64 CDeZhouGameLogic::GetPlayerRealTableMoneyBySitID(int SitID)
{
	INT64 ret = 0;
	if ( IsAvailSitID(SitID) && m_GP[SitID-1].IsHavePlayer() )
	{
		ret = m_GP[SitID-1].GetRealTableMoney();
	}
	return ret;
}

void CDeZhouGameLogic::RecordWinLoss(UINT32 PID,INT64 nWinLoss)
{
	MapTablePlayerWinLoss::iterator itorPI = m_mapTableWinLoss.find(PID);
	if ( itorPI == m_mapTableWinLoss.end() )
	{
		m_mapTableWinLoss.insert(make_pair(PID,0));
		itorPI = m_mapTableWinLoss.find(PID);
	}

	itorPI->second += nWinLoss;
	m_vectorWinLossPID.push_back(PID);
}

void CDeZhouGameLogic::InitGameData()
{
	for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
	{
		m_TablePoolMoneys[Sit].Init();
	}

	m_GamePaiLogic.InitPai();

	m_nGameWinLossMoney = 0;
	m_nGameLossMoney = 0;
}

bool CDeZhouGameLogic::IsSitWin(int Sit)
{
	if ( IsAvailSitID(Sit+1) && m_GP[Sit].IsInGame() )
	{
		INT64 MaxPai = 0;
		for ( int i=0;i<m_MaxSitPlayerNumber;i++)
		{
			if ( m_GP[i].IsInGame() && m_GP[i].m_PaiType.GetValue() > MaxPai )
			{
				MaxPai = m_GP[i].m_PaiType.GetValue();
			}
		}

		if ( m_GP[Sit].m_PaiType.GetValue() == MaxPai )
		{
			return true;
		}
	}
	return false;
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
		if ( IsCommondTable() || IsRoomMatch() )
		{
			if ( SitPlayerCount >= m_MinPlayerForGame )
			{
				bRet = true;
			}
		}
		else if ( IsTableMatch() )
		{
			if ( IsMatching() )
			{
				if ( SitPlayerCount >= m_MinPlayerForGame )
				{
					bRet = true;
				}
			}
			else
			{
				if ( SitPlayerCount >= m_pMatchRule->m_MinPlayerForMatch )
				{
					bRet = true;
				}
			}
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

	//m_pTable->CeShiInfo("StartCommonGame() Start ...");

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
				rGPlayer.m_OtherLoss = 0;
				rGPlayer.m_HandPai.Init();
				rGPlayer.m_PaiType.Init();
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
	
	m_CurPlayingPlayerNumber = 0;
	for(int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		CGamePlayer& rGPlayer = m_GP[Sit];
		if ( rGPlayer.IsHavePlayer() )
		{
			if ( rGPlayer.GetTableMoney() < m_nMinLeftForGame )
			{
				rGPlayer.SetGameState( GAME_PLAYER_ST_WAITING );
				SendReqMoney(Sit+1,rGPlayer.GetPID());
			}
			else
			{
				m_CurPlayingPlayerNumber++;
				rGPlayer.SetGameState( GAME_PLAYER_ST_PLAYING );
			}
		}
		else
		{
			rGPlayer.SetGameState( GAME_PLAYER_ST_NULL );
		}
	}

	if ( m_CurPlayingPlayerNumber < m_MinPlayerForGame )
	{
		return ;
	}

	if( m_pTable->IsTablePrivate() )      //自建桌发送核对时间
	{
		m_bEndTable = false;
		UINT32 TempTableLeftTime = m_pTable->GetTableLeftTime();
		if ( TempTableLeftTime>0 && TempTableLeftTime<1800 )
		{
			GameDeZhou_TableLeft msgTL;
			msgTL.m_LeftTime = TempTableLeftTime;
			SendLogicMsgToAllPlayer(msgTL);			
		}
		else if ( TempTableLeftTime <= 0 )
		{		
			m_bEndTable = true;
			GameDeZhou_TableEnd msgTE;
			SendLogicMsgToAllPlayer(msgTE);			
		}
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

	OnHand();

	//m_pTable->CeShiInfo("StartCommonGame() End... ");
}

void CDeZhouGameLogic::StartMatchGame()
{
	TraceStackPath logTP("CDeZhouGameLogic::StartMatchGame");
	CLogFuncTime lft(s_LogicFuncTime,"StartMatchGame");
	m_pTable->CeShiInfo("StartMatchGame() Start");

	if ( !IsMatching() )
	{
		int nMatchPlayer = 0;
		for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
		{
			PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
			if( pPlayer )
			{
				if ( Sit >= m_MaxSitPlayerNumber )
				{
					m_pTable->DebugError("错误座位 TableID=%d,SitID=%d PlayerID=%d",m_pTable->GetTableID(),Sit+1,pPlayer->GetPID());					
					LogicStandUpPlayer(pPlayer,Sit+1,Action_Flag_SysLogic);
					continue;
				}

				if ( pPlayer->GetGameMoney() < m_pMatchRule->GetMatchCost() )
				{
					GameDeZhou_Flag msgFlag;
					msgFlag.m_Flag = msgFlag.NoMoneyForMatch;
					pPlayer->SendGameLogicMsg(msgFlag);

					m_pTable->DebugError("余额不足 TableID=%d,SitID=%d PlayerID=%d",m_pTable->GetTableID(),Sit+1,pPlayer->GetPID());
					LogicStandUpPlayer(pPlayer,Sit+1,Action_Flag_SysLogic);
					continue;
				}
				++nMatchPlayer;
			}
		}
		if ( nMatchPlayer >= m_pMatchRule->m_MinPlayerForMatch )
		{
			if ( IsTableMatch() )
			{
				m_pTable->DebugInfo("单桌比赛开始 PlayerNum=%d  ************************ ",m_CurPlayingPlayerNumber);

				m_pTable->SetTableMatchState(MatchTable_ST_Matching);
				m_pServer->ChangeMatchTableState(m_pTable);

				m_pMatchRule->m_bMatching = true;
				m_pMatchRule->m_MatchStartTime = m_CurTime;
				m_pMatchRule->m_TotalLossMoney = 0;
				m_pMatchRule->m_TotalMatchMoney = INT64(nMatchPlayer)*m_pMatchRule->GetStartMoney();

				m_pMatchRule->m_TotalPlayer = nMatchPlayer;
				m_pMatchRule->m_LeftPlayer  = nMatchPlayer;

				m_pMatchRule->m_bChampTable = true;

				m_pMatchRule->InitBlind(m_CurTime);

				m_pMatchRule->m_MatchGUID = INT64(m_CurTime-N_Time::Second_MoGuiBegin)*INT64(10000000000) 
					+ INT64(m_pTable->GetRoomID())*100000 + INT64(m_pTable->GetTableID())*10;
			}

			for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
				if ( pPlayer )
				{
					pPlayer->AddGameMoney(-m_pMatchRule->GetMatchCost());					
					pPlayer->AddMoneyLog(-m_pMatchRule->GetMatchCost(),"JoinMatch");
					pPlayer->SetPlayerState(PLAYER_ST_PALYING);
				}
			}
		}
		else
		{
			m_pTable->DebugError("StartMatchGame TableID=%d nPlayer=%d MinPlayer=%d",m_pTable->GetTableID(),nMatchPlayer,m_MinPlayerForGame );
		}
	}

	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
		if ( pPlayer )
		{
			if ( m_GP[Sit].IsExit() || m_GP[Sit].IsWashOut() )
			{
				m_pTable->DebugError("StartMatchGame InitPlayerData TableID=%d,SitID=%d PlayerID=%d",m_pTable->GetTableID(),Sit+1,pPlayer->GetPID());
			}
			else
			{
				m_GP[Sit].SetGameState(GAME_PLAYER_ST_WAITING);
				m_GP[Sit].m_OtherLoss = 0;
				m_GP[Sit].m_HandPai.Init();
				m_GP[Sit].m_PaiType.Init();
			}
		}
		else
		{
			if ( m_GP[Sit].IsHavePlayer() )
			{
				m_pTable->DebugError("StartMatchGame TableID=%d,SitID=%d PlayerID=%d",m_pTable->GetTableID(),Sit+1,m_GP[Sit].GetPID());
			}
			else if ( m_GP[Sit].IsExit() )
			{
				m_GP[Sit].Init();
			}
		}
	}

	m_CurPlayingPlayerNumber = 0;
	for(int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() )
		{
			if ( m_GP[Sit].GetTableMoney() < m_nMinLeftForGame ) //游戏币不符合的玩家应该在上局结束的时候清掉
			{
				m_pTable->DebugError("StartMatchGame NoMoney TableID=%d,SitID=%d PlayerID=%d",m_pTable->GetTableID(),Sit+1,m_GP[Sit].GetPID());
			}
			else
			{
				m_CurPlayingPlayerNumber++;		
				m_GP[Sit].SetGameState( GAME_PLAYER_ST_PLAYING );
			}
		}
	}

	if ( m_CurPlayingPlayerNumber < m_MinPlayerForGame )
	{
		m_pTable->DebugError("StartMatchGame  Error 人数不足");
		if ( IsRoomMatch() )
		{
			m_StartGameTime = N_Time::Max_Time;
		}
		return ;
	}

	if ( IsRoomMatch() )
	{
		if ( m_CurPlayingPlayerNumber == m_pMatchRule->m_LeftPlayer )
		{
			m_pMatchRule->m_bChampTable = true;
			if ( !m_pRoom->CheckChampTable() )
			{
				m_pTable->DebugError("CheckChampTable CurPlayer=%d LeftPlayer=%d",m_CurPlayingPlayerNumber,m_pMatchRule->m_LeftPlayer);
			}
		}
	}

	m_nBigBlind = m_pMatchRule->m_BigBlind;
	m_nSmallBlind = m_pMatchRule->m_SmallBlind;

	for(int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		m_Tallys[Sit].ReSet();
	}

	OnHand();

	//m_pTable->CeShiInfo("StartMatchGame() End");
}

void CDeZhouGameLogic::OnHand()
{
	TraceStackPath logTP("CDeZhouGameLogic::OnHand");
	CLogFuncTime lft(s_LogicFuncTime,"OnHand");
	m_pTable->CeShiInfo("OnHand() Start");

	CheckLogicMsg();

	m_pGameLog->Init();

	m_StartGameTime = m_CurTime;
	m_GameState = GAME_ST_HAND;
	SendGameState();

	InitGameData();

	m_GameLogicGUID = INT64(m_CurTime-N_Time::Second_MoGuiBegin)*INT64(10000000000) 
		+ INT64(m_pTable->GetRoomID())*100000 + INT64(m_pTable->GetTableID())*10;

	m_pGameLog->AddStepString("开始游戏......");
	m_pGameLog->AddStepString("m_nServiceMoney=" + N2S(m_nServiceMoney)+" m_MaxSitPlayerNumber=" + N2S(m_MaxSitPlayerNumber) 
		+ " m_BigBlind=" + N2S(m_nBigBlind) );

	if ( m_BankerSitID == 0 )
	{
		m_BankerSitID = CRandom::Random_Int(1,m_MaxSitPlayerNumber);
	}
	m_BankerSitID = GetNextPlayingPlayerSitID(m_BankerSitID);
	m_SmallBlindSitID = GetNextPlayingPlayerSitID(m_BankerSitID);
	m_BigBlindSitID = GetNextPlayingPlayerSitID(m_SmallBlindSitID);

	assert( m_BankerSitID && m_SmallBlindSitID && m_BigBlindSitID && "OnHand" );

	m_pGameLog->AddStepString("m_BankerSitID=" + N2S(m_BankerSitID)+" m_SmallBlindSitID=" + N2S(m_SmallBlindSitID)
		+ " m_BigBlindSitID=" + N2S(m_BigBlindSitID) );

	assert(m_BankerSitID&&m_SmallBlindSitID&&m_BigBlindSitID);
	if ( !(m_BankerSitID&&m_SmallBlindSitID&&m_BigBlindSitID) )
	{
		m_pTable->DebugError("CGameTable::TableStartGame()");
	}

	GameDeZhou_Start st;
	SendLogicMsgToAllPlayer(st);

	SendBankerInfo();//必须在发牌之前发送

	GameDeZhou_StartInfo startinfo;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		m_GP[Sit].SetAllIn(false);
		m_GP[Sit].SetHaveSendMoney(false);
		m_GP[Sit].SetShowFace(false);
		if ( m_GP[Sit].IsPlaying() )
		{
			m_GamePaiLogic.SetPlayerRightBySit(Sit,PLAYER_RIGHT_COMMON);

			bool bJoinJuBao = false;
			PlayerPtr pTempPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
			if ( m_GP[Sit].m_JuBaoCount>0 && m_GP[Sit].m_JuBaoChip>0 )
			{				
				while( m_GP[Sit].m_JuBaoChip > N_JuBaoPeng::cs_JuBaoPengFlagMoney )
				{
					m_GP[Sit].m_JuBaoChip -= N_JuBaoPeng::cs_JuBaoPengFlagMoney;
				}
				int nJuBaoChip = m_GP[Sit].m_JuBaoChip;
				
				if ( m_GP[Sit].GetTableMoney() > (nJuBaoChip+m_nServiceMoney) )
				{
					bJoinJuBao = true;
					m_GP[Sit].DecTableMoney(nJuBaoChip);
					m_GP[Sit].m_OtherLoss += nJuBaoChip;
				}
				else
				{					
					if ( pTempPlayer->GetGameMoney() > (nJuBaoChip+m_GP[Sit].GetTableMoney()) )
					{
						bJoinJuBao = true;
					}
				}
				if ( bJoinJuBao )
				{
					if ( pTempPlayer )
					{
						pTempPlayer->AddGameMoney(-nJuBaoChip);						
						pTempPlayer->AddMoneyLog(-nJuBaoChip,"JoinJuBaoPeng");
					}
					m_GP[Sit].m_JuBaoCount--;
					m_GP[Sit].m_JuBaoChip += N_JuBaoPeng::cs_JuBaoPengFlagMoney;

					DBServerXY::WDB_IncomeAndPay msgIP;
					msgIP.m_AID            = m_GP[Sit].GetAID();
					msgIP.m_PID            = m_GP[Sit].GetPID();
					msgIP.m_Flag           = N_IncomeAndPay::JuBaoPengChip;
					msgIP.m_nMoney         = nJuBaoChip;
					LogicSendMsgToDBServer(msgIP);

					m_pServer->AddJuBaoPeng(nJuBaoChip);
				}
			}
			if ( bJoinJuBao == false )
			{
				if ( m_GP[Sit].m_JuBaoCount>0 || m_GP[Sit].m_JuBaoChip>0 )
				{
					GameDeZhou_RespJoinJuBaoPeng msgRespJJBP;
					msgRespJJBP.m_Flag   = msgRespJJBP.NOMONEY;
					pTempPlayer->SendGameLogicMsg(msgRespJJBP);

					m_GP[Sit].m_JuBaoCount = 0;
					m_GP[Sit].m_JuBaoChip = 0;

					GameDeZhou_JoinJuBaoPengInfo msgInfo;
					msgInfo.m_Flag       = msgInfo.ExitJuBaoPeng;
					msgInfo.m_SitID      = Sit+1;
					SendLogicMsgToAllPlayer(msgInfo);
				}
			}

			if ( m_nServiceMoney>0 && m_GP[Sit].GetTableMoney() > m_nServiceMoney )
			{
				m_GP[Sit].DecTableMoney(m_nServiceMoney);
				m_Tallys[Sit].m_nServiceMoney = m_nServiceMoney;
				startinfo.m_PlayerStartInfo[Sit].m_nServiceMoney = m_nServiceMoney;
			}
			startinfo.m_PlayerStartInfo[Sit].m_SitID = Sit+1;

			m_GP[Sit].SetOriginMoney(m_GP[Sit].GetTableMoney());
			m_GP[Sit].m_OtherLoss = 0;
		}
	}

	if ( m_nPotMoney == 0 )
	{
		int Sit = m_BigBlindSitID-1;
		m_GP[Sit].SetCurChip(min(m_GP[Sit].GetTableMoney(),m_nBigBlind));
		startinfo.m_PlayerStartInfo[Sit].m_nChip =  m_GP[Sit].GetCurChip();
		startinfo.m_PlayerStartInfo[Sit].m_Action = ACTION_BIGBLIND;
		m_GP[Sit].SetLastAction(ACTION_BIGBLIND);

		Sit = m_SmallBlindSitID-1;
		m_GP[Sit].SetCurChip(min(m_GP[Sit].GetTableMoney(),m_nSmallBlind));
		startinfo.m_PlayerStartInfo[Sit].m_nChip = m_GP[Sit].GetCurChip();
		startinfo.m_PlayerStartInfo[Sit].m_Action = ACTION_SMALLBLIND;
		m_GP[Sit].SetLastAction(ACTION_SMALLBLIND);

		m_MaxChipOnTable = max(m_GP[m_BigBlindSitID-1].GetCurChip(),m_GP[m_SmallBlindSitID-1].GetCurChip());
	}
	else
	{
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			if ( m_GP[Sit].IsPlaying() )
			{
				if ( Sit+1 == m_BigBlindSitID )
				{
					m_GP[Sit].SetCurChip(min(m_GP[Sit].GetTableMoney(),(m_nPotMoney+m_nBigBlind)));
					startinfo.m_PlayerStartInfo[Sit].m_nChip = m_GP[Sit].GetCurChip();
					startinfo.m_PlayerStartInfo[Sit].m_Action = ACTION_BIGBLIND;
					m_GP[Sit].SetLastAction(ACTION_BIGBLIND);
				}
				else if ( Sit+1 == m_SmallBlindSitID )
				{
					m_GP[Sit].SetCurChip(min(m_GP[Sit].GetTableMoney(),(m_nPotMoney+m_nSmallBlind)));
					startinfo.m_PlayerStartInfo[Sit].m_nChip = m_GP[Sit].GetCurChip();
					startinfo.m_PlayerStartInfo[Sit].m_Action = ACTION_SMALLBLIND;
					m_GP[Sit].SetLastAction(ACTION_SMALLBLIND);
				}
				else
				{
					m_GP[Sit].SetCurChip(min(m_GP[Sit].GetTableMoney(),m_nPotMoney));
					startinfo.m_PlayerStartInfo[Sit].m_nChip = m_GP[Sit].GetCurChip();
					startinfo.m_PlayerStartInfo[Sit].m_Action = ACTION_POT;
					m_GP[Sit].SetLastAction(ACTION_POT);
				}
				m_MaxChipOnTable = max(m_MaxChipOnTable,m_GP[Sit].GetCurChip());
			}
		}
	}

	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsPlaying() )
		{
			startinfo.m_PlayerStartInfo[Sit].m_nTableMoney = m_GP[Sit].GetTableMoney() - m_GP[Sit].GetCurChip();
			//m_pTable->DebugInfo("SitID=%d CurChip=%-10s Origin=%-10s",Sit+1,Tool::N2S(m_GP[Sit].GetCurChip()).c_str(),
			//	Tool::N2S(m_GP[Sit].GetOriginMoney()).c_str() );

			if ( m_GP[Sit].GetTableMoney() == m_GP[Sit].GetCurChip() )
			{
				m_GP[Sit].SetGameState(GAME_PLAYER_ST_ALLIN);
			}
		}
	}

	for (int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
	{
		if ( m_GP[Sit].IsAttendGame() )
		{
			PlayerPtr pTempPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
			assert(pTempPlayer && pTempPlayer->GetPID()==m_GP[Sit].GetPID());
			if ( pTempPlayer->m_RightTimes > 0 )
			{
				pTempPlayer->m_RightTimes--;
				m_GamePaiLogic.SetPlayerRightBySit(Sit,pTempPlayer->m_RightLevel);
			}
			else
			{
				m_GamePaiLogic.SetPlayerRightBySit(Sit,PLAYER_RIGHT_COMMON);
			}
			pTempPlayer->m_TodayGameCount++;
			pTempPlayer->UpdateGameLevel(true);

			pTempPlayer->m_OneLiftPlayTimes++;
		}
	}
	
	m_GamePaiLogic.MakePai(true);

	if ( m_pGSConfig->CeshiMode() )
	{
		DezhouLib::CPaiType MaxPai;
		for(int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
		{
			if ( m_GP[Sit].IsAttendGame() && m_GP[Sit].m_PaiType>MaxPai )
			{
				MaxPai = m_GP[Sit].m_PaiType;
			}
		}
		for(int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
		{
			if ( m_GP[Sit].IsAttendGame() && m_GP[Sit].m_PaiType == MaxPai )
			{
				s_nWinTimes[Sit]++;
			}
		}
	}

	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsAttendGame() )
		{
			DezhouLib::stHandPai stHP = m_GamePaiLogic.GetHandPai(Sit);
			m_GP[Sit].m_HandPai.SetPai(stHP.m_Pai[0],stHP.m_Pai[1]);
			m_GP[Sit].m_PaiType = m_GamePaiLogic.GetPlayerPaiType(Sit);

			m_pGameLog->AddStepString("SitID="+N2S(Sit+1)+" OrigenMoney="+N2S(m_GP[Sit].GetOriginMoney(),10)
				+ " Pai1="+N2S(m_GP[Sit].m_HandPai.m_Pai[0],2)+ " Pai2="+N2S(m_GP[Sit].m_HandPai.m_Pai[1],2)
				+ " PaiType="+N2S(m_GP[Sit].m_PaiType.GetType()) + " PID="+N2S(m_GP[Sit].GetPID()) );
		}
	}

	PlayerPtr pPlayer,TempPlayer = NULL;
	while( pPlayer = m_pTable->GetNextPlayer( TempPlayer ) )
	{
		TempPlayer = pPlayer;

		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			startinfo.m_PlayerStartInfo[Sit].m_HandPai.Init();
			if ( m_GP[Sit].IsInGame() )
			{
				if( pPlayer->GetPID() == m_GP[Sit].GetPID() )
				{
					startinfo.m_PlayerStartInfo[Sit].m_HandPai = m_GP[Sit].m_HandPai;
				}
				else
				{
					startinfo.m_PlayerStartInfo[Sit].m_HandPai = stHandPai(PAI_VALUE_BACK,PAI_VALUE_BACK);
				}
			}
		}

		SendLogicMsgToOnePlayer(startinfo,pPlayer);
	}

	PlayerChipStep  csBig,csSmall;
	csSmall.m_SitID = m_SmallBlindSitID;
	csSmall.m_nMoney = m_GP[m_SmallBlindSitID-1].GetCurChip();
	csSmall.m_LeftMoney = m_GP[m_SmallBlindSitID-1].GetRealTableMoney();
	csSmall.m_Step = m_GameState;
	m_pGameLog->AddStepString(csSmall.GetString());

	csBig.m_SitID = m_BigBlindSitID;
	csBig.m_nMoney = m_GP[m_BigBlindSitID-1].GetCurChip();
	csBig.m_LeftMoney = m_GP[m_BigBlindSitID-1].GetRealTableMoney();
	csBig.m_Step = m_GameState;
	m_pGameLog->AddStepString(csBig.GetString());

	m_MinAddChip             = m_nBigBlind;
	m_FirstPlayerSitID       = m_BigBlindSitID;
	m_EndPlayerSitID         = m_BigBlindSitID;

	m_CurPlayerSitID = GetNextPlayingPlayerSitID(m_BigBlindSitID);
	if ( GetPlayingPlayerNumber()==0 || m_CurPlayerSitID==0 ) //因为有存在一开始就结束的情况 比如玩家下小盲注后就没有钱，只有两个玩家
	{
		EndGame();
	}
	else
	{
		if ( m_GP[m_EndPlayerSitID-1].IsAllIn() )
		{
			m_EndPlayerSitID = GetPrePlayingPlayerSitID(m_EndPlayerSitID);
		}

		SendPlayerPower();
		m_ActionTime = m_CurTime;		
		
		SetBotPlayerBlind();
		ChangeBotBlind();
	}

	//m_pTable->CeShiInfo("OnHand() End");
}

void CDeZhouGameLogic::ChangeBotBlind()
{
	TraceStackPath logTP("CDeZhouGameLogic::ChangeBotBlind");
	CLogFuncTime lft(s_LogicFuncTime,"ChangeBotBlind");
	//m_pTable->CeShiInfo("ChangeBotBlind Start...");

	if ( IsMatchTable() && CRandom::GetChangce(2,1) )
	{
		return ;
	}

	bool bAllPlayerAreBot = true;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
	{
		if ( m_GP[Sit].IsPlaying() && m_GP[Sit].IsNotBotPlayer() )
		{
			bAllPlayerAreBot = false;
			break;
		}
	}

	if ( bAllPlayerAreBot )
	{
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
		{
			if ( m_GP[Sit].IsPlaying() && m_GP[Sit].IsBotPlayer() && CRandom::GetChangce(2,1) )
			{
				int nBeiShu = m_GamePaiLogic.GetBlindBeiShu(Sit);
				m_GP[Sit].m_MinBlind *= nBeiShu;
				m_GP[Sit].m_MaxBlind *= nBeiShu;
			}
		}
	}
	//m_pTable->CeShiInfo("ChangeBotBlind End...");
}
void CDeZhouGameLogic::SetBotPlayerBlind()
{
	TraceStackPath logTP("CDeZhouGameLogic::SetBotPlayerBlind");
	CLogFuncTime lft(s_LogicFuncTime,"SetBotPlayerBlind");

	int nPlayerCount = 0;
	int nBotCount = 0;
	for(int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsAttendGame() )
		{
			if ( m_GP[Sit].IsNotBotPlayer() )nPlayerCount++;
			else nBotCount++;
		}
	}
	assert(nBotCount+nPlayerCount==m_CurPlayingPlayerNumber);
	if ( nPlayerCount )
	{
		m_BotTargetWinLoss += m_BotLossEveryTime*m_nBigBlind;
	}

	m_pTable->CeShiInfo("SetBotPlayerBlind() Start BotRealLoss=%s,BotTargetLoss=%s",Tool::N2S(m_BotRealWinLoss).c_str(),Tool::N2S(m_BotTargetWinLoss).c_str() );

	INT64 nNeedWinMoney = m_BotTargetWinLoss - m_BotRealWinLoss;
	int NeedWinBlind    = int( nNeedWinMoney/m_nBigBlind );
	bool bNeedWin = false;
	if ( NeedWinBlind > 100 )bNeedWin = true;
	else if ( NeedWinBlind < -100 )bNeedWin = false;
	else bNeedWin = CRandom::GetChangce(2,1);

	if ( IsMatchTable() && NeedWinBlind > 0 )bNeedWin = true;
	if ( nNeedWinMoney > 1000000 )bNeedWin = true;

	m_bNeedBotWin = bNeedWin;

	for ( int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsInGame() && m_GP[Sit].IsBotPlayer() )
		{
			bool bWin = IsSitWin(Sit);
			int  nPaiValueType = m_GP[Sit].m_PaiType.m_Type;
			int  HandPaiType = m_GamePaiLogic.GetHandPaiLevel(Sit);
			int  nMoneyBlind = int(m_GP[Sit].GetTableMoney()/m_nBigBlind);

			if ( HandPaiType == HandPai_Level_1 )
			{
				if ( bWin && bNeedWin )
				{
					m_GP[Sit].m_MinBlind = CRandom::Random_Int(5,10)*nPaiValueType;
					if ( CRandom::GetChangce(PAI_TYPE_KING,min(nPaiValueType*3,PAI_TYPE_KING)) )
					{
						m_GP[Sit].m_MinBlind = CRandom::Random_Int(20,100)*nPaiValueType;
					}
					m_GP[Sit].m_MaxBlind = MAX_Blind;
				}
				else
				{
					m_GP[Sit].m_MinBlind = CRandom::Random_Int(5,20);
					m_GP[Sit].m_MaxBlind = CRandom::Random_Int(10,20)*int(pow(2.0,nPaiValueType));

					if ( CRandom::GetChangce(PAI_TYPE_KING,min(PAI_TYPE_KING,2*nPaiValueType)) )
					{
						m_GP[Sit].m_MaxBlind *= CRandom::Random_Int(nPaiValueType,10);
					}
				}

				if ( nMoneyBlind <= 500 && CRandom::GetChangce(max(nMoneyBlind,200),50) )
				{
					m_GP[Sit].m_MinBlind = max(m_GP[Sit].m_MinBlind,200);
					m_GP[Sit].m_MaxBlind = max(m_GP[Sit].m_MaxBlind,500);
				}
			}
			else if ( HandPaiType == HandPai_Level_2 )
			{
				if ( bWin && bNeedWin )
				{
					m_GP[Sit].m_MinBlind = CRandom::Random_Int(5,10)*nPaiValueType;
					if ( CRandom::GetChangce(PAI_TYPE_KING,min(nPaiValueType*2,PAI_TYPE_KING)) )
					{
						m_GP[Sit].m_MinBlind = CRandom::Random_Int(10,40)*nPaiValueType;
					}					
					m_GP[Sit].m_MaxBlind = MAX_Blind;
				}
				else
				{
					m_GP[Sit].m_MinBlind = CRandom::Random_Int(5,10);
					m_GP[Sit].m_MaxBlind = CRandom::Random_Int(5,10)*int(pow(2.0,nPaiValueType-1));

					if ( CRandom::GetChangce(PAI_TYPE_KING,nPaiValueType) )
					{
						m_GP[Sit].m_MaxBlind *= CRandom::Random_Int(nPaiValueType,10);
					}
				}

				if ( nMoneyBlind <= 500 && CRandom::GetChangce(max(nMoneyBlind,100),10) )
				{
					m_GP[Sit].m_MinBlind = max(m_GP[Sit].m_MinBlind,100);
					m_GP[Sit].m_MaxBlind = max(m_GP[Sit].m_MaxBlind,500);
				}
			}			
			else if ( HandPaiType == HandPai_Level_3 )
			{
				if ( CRandom::GetChangce(2,1) && nPaiValueType < 5 )
				{
					m_GP[Sit].m_MaxBlind = 0;
					m_GP[Sit].m_MinBlind = 0;
				}
				else
				{
					if ( bWin && bNeedWin )
					{
						if ( CRandom::GetChangce(PAI_TYPE_KING,min(nPaiValueType,PAI_TYPE_KING)) )
						{
							m_GP[Sit].m_MinBlind = CRandom::Random_Int(nPaiValueType,10);
							m_GP[Sit].m_MaxBlind = MAX_Blind;
						}
						else
						{
							m_GP[Sit].m_MinBlind = CRandom::Random_Int(1,nPaiValueType);
							m_GP[Sit].m_MaxBlind = 100*nPaiValueType;
						}
					}
					else
					{
						m_GP[Sit].m_MinBlind = nPaiValueType/3;
						m_GP[Sit].m_MaxBlind = CRandom::Random_Int(5,10)*nPaiValueType;

						if ( CRandom::GetChangce(PAI_TYPE_KING*2,nPaiValueType) )
						{
							m_GP[Sit].m_MaxBlind *= CRandom::Random_Int(nPaiValueType,10);
						}
					}
				}
			}
			else  //直接弃牌
			{
				if ( nPaiValueType >= 5 )
				{
					m_GP[Sit].m_MinBlind = 1;
					m_GP[Sit].m_MaxBlind = 30*(nPaiValueType-4);
				}
				else
				{
					m_GP[Sit].m_MinBlind = 0;
					m_GP[Sit].m_MaxBlind = 0;
				}
			}
		}
	}
	//m_pTable->CeShiInfo("SetBotPlayerBlind() End");
}

int CDeZhouGameLogic::OnGameMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnGameMsg");
	CLogFuncTime lft(s_LogicFuncTime,"OnGameMsg");

	m_pTable->CeShiInfo("OnGameMsg Start XYID=%d XYLen=%d",msgPack.m_XYID,msgPack.m_nLen);

	GameXY::ClientToServerMessage CTSMsg;
	TransplainMsg(msgPack,CTSMsg);

	m_pTable->CeShiInfo("OnGameMsg GameXYID=%d XYLen=%d",CTSMsg.m_MsgID,CTSMsg.m_MsgLen);

	int ret = 0;
	switch ( CTSMsg.m_MsgID )
	{
	case GameDeZhou_ReqAddChip::XY_ID:
		{
			ret = OnPlayerAddChip(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_ReqShowPai::XY_ID:
		{
			ret = OnReqShowPai(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_RespAddMoney::XY_ID:
		{
			ret = OnAddMoney(pPlayer,CTSMsg);
		}
		break;
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
	case GameDeZhou_ReqShowFace::XY_ID:
		{
			ret = OnShowFace(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_ReqSendMoney::XY_ID:
		{
			ret = OnSendMoney(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_SendGift::XY_ID:
		{
			ret = OnSendGift(pPlayer,CTSMsg);
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
	case GameDeZhou_ReqChangGift::XY_ID:
		{
			ret = OnReqChangeGift(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_ReqAddTableTime::XY_ID:
		{
			ret = OnReqAddTableTime(pPlayer,CTSMsg);
		}
		break;
	case GameDeZhou_ReqJoinJuBaoPeng::XY_ID:
		{
			ret = OnJoinJuBaoPeng(pPlayer,CTSMsg);	
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

	m_pTable->CeShiInfo("OnGameMsg End");

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

int CDeZhouGameLogic::OnShowFace(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnShowFace");
	CLogFuncTime lft(s_LogicFuncTime,"OnShowFace");

	GameDeZhou_ReqShowFace msgSF;
	TransplainMsgCTS(CTSMsg,msgSF);

	m_pTable->DebugInfo("OnShowFace Start SitID=%d FaceID=%d PID=%d",msgSF.m_SitID,msgSF.m_FaceID,pPlayer->GetPID());

	GameDeZhou_RespShowFace msgRespSF;
	msgRespSF.m_FaceID = msgSF.m_FaceID;
	msgRespSF.m_SitID = msgSF.m_SitID;

	if ( pPlayer && pPlayer->GetTableID() == m_pTable->GetTableID() 
		&& pPlayer->GetSitID()==msgSF.m_SitID && IsAvailSitID(msgSF.m_SitID) )
	{
		DBServerXY::DBS_msgFaceInfo stFI;
		if ( GetFaceInfo(msgSF.m_FaceID,stFI) )
		{
			CGamePlayer& rCurGPlayer = m_GP[msgSF.m_SitID-1];
			bool bShowSuccess = false;
			INT64 PayMoney = 0;
			if ( pPlayer->m_FreeFaceTime < m_CurTime )
			{
				PayMoney = INT64(stFI.m_Price)*m_FaceBase/100;
				PayMoney = max(PayMoney,stFI.m_MinPrice);
				PayMoney = min(PayMoney,stFI.m_MaxPrice);
			
				INT64 MaxCanUseMoney = rCurGPlayer.GetTableMoney()-(m_nBigBlind+m_nServiceMoney);
				if ( rCurGPlayer.IsInGame() )
				{
					MaxCanUseMoney = rCurGPlayer.GetTableMoney()-(m_MaxChipOnTable+m_nBigBlind);
				}
				if ( IsTableMatch() )
				{
					MaxCanUseMoney = 0;
				}
				if ( MaxCanUseMoney >= PayMoney )
				{
					bShowSuccess = true;

					rCurGPlayer.DecTableMoney(PayMoney);
					pPlayer->AddGameMoney(-PayMoney);
					pPlayer->AddMoneyLog(-PayMoney,"OnShowFace");
					pPlayer->UpdateGameMoney();

					rCurGPlayer.m_OtherLoss += PayMoney;

					m_pGameLog->AddStepString("ShowFace SitID="+N2S(msgSF.m_SitID)+" FaceID="+N2S(msgSF.m_FaceID)+" nMoney="+N2S(PayMoney));

					msgRespSF.m_PayMoney   = PayMoney;
					msgRespSF.m_TableMoney = rCurGPlayer.GetRealTableMoney();
				}
				else
				{
					MaxCanUseMoney = pPlayer->GetGameMoney() - rCurGPlayer.GetTableMoney();
					if ( MaxCanUseMoney >= PayMoney )
					{
						bShowSuccess = true;

						pPlayer->AddGameMoney(-PayMoney);
						pPlayer->AddMoneyLog(-PayMoney,"OnShowFace Pay By GameMoney");
						pPlayer->UpdateGameMoney();

						//m_pGameLog->AddStepString("ShowFace SitID="+N2S(msgSF.m_SitID)+" FaceID="+N2S(msgSF.m_FaceID)+" nMoney="+N2S(PayMoney));

						msgRespSF.m_PayMoney   = PayMoney;
						msgRespSF.m_TableMoney = rCurGPlayer.GetRealTableMoney();
					}
				}
			}
			else
			{
				bShowSuccess = true;
			}

			if ( bShowSuccess )
			{
				rCurGPlayer.SetShowFace(true);
				pPlayer->m_ShowFaceTimes++;
				pPlayer->CheckShowFace();

				DBServerXY::WDB_ShowFace msgDBSF;
				msgDBSF.m_PID           = pPlayer->GetPID();
				msgDBSF.m_AID           = pPlayer->GetAID();
				msgDBSF.m_FaceID        = msgSF.m_FaceID;
				msgDBSF.m_PayMoney      = PayMoney;
				LogicSendMsgToDBServer(msgDBSF);

				SendLogicMsgToAllPlayer(msgRespSF);
			}
		}
	}
	else
	{
		m_pTable->DebugError("OnShowFace PID=%d PlayerTableID=%d PlayerSitID=%d MsgSitID=%d FaceID=%d",
			pPlayer->GetPID(),pPlayer->GetTableID(),pPlayer->GetSitID(),msgSF.m_SitID,msgSF.m_FaceID );
	}

	m_pTable->DebugInfo("OnShowFace End PayMoney=%s TableMoney=%s",N2S(msgRespSF.m_PayMoney.get()).c_str(),N2S(msgRespSF.m_TableMoney.get()).c_str());
	return 0;
}
int CDeZhouGameLogic::OnSendMoney(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnSendMoney");
	CLogFuncTime lft(s_LogicFuncTime,"OnSendMoney");

	GameDeZhou_ReqSendMoney msgSM;
	TransplainMsgCTS(CTSMsg,msgSM);

	m_pTable->CeShiInfo("CDeZhouGameLogic::OnSendMoney SendPID=%d RecvPID=%d Money=%s",
		msgSM.m_SendPID,msgSM.m_RecvPID,N2S(msgSM.m_nSendMoney.get()).c_str() );

	GameDeZhou_RespSendMoney msgRespSM;
	msgRespSM.m_Flag = msgRespSM.UNSUCCESS;

	msgSM.m_nSendMoney = max(msgSM.m_nSendMoney,m_pGSConfig->m_cfSend.m_minSend);
	msgSM.m_nSendMoney = min(msgSM.m_nSendMoney,m_pGSConfig->m_cfSend.m_maxSend);

	PlayerPtr pSendPlayer = m_pTable->GetTablePlayerByPID(msgSM.m_SendPID);
	PlayerPtr pRecvPlayer = m_pTable->GetTablePlayerByPID(msgSM.m_RecvPID);
	if ( !(pSendPlayer && pRecvPlayer && IsAvailSitID(pSendPlayer->GetSitID()) && IsAvailSitID(pRecvPlayer->GetSitID())) )
	{
		return 0;
	}
	int SendSit = pSendPlayer->GetSitID()-1;
	int RecvSit = pRecvPlayer->GetSitID()-1;
	if ( m_pGSConfig->m_cfSend.m_SendON == 0 )
	{
		msgRespSM.m_Flag = msgRespSM.CantSend;
	}
	else if ( m_GP[SendSit].GetHaveSendMoney() && m_pGSConfig->m_cfSend.m_OneTimeOneGame )
	{
		msgRespSM.m_Flag = msgRespSM.ReSend;
	}
	else
	{
		INT64 nSendMoney = msgSM.m_nSendMoney;
		nSendMoney = min(nSendMoney,m_GP[SendSit].GetTableMoney()-(m_nBigBlind+m_nServiceMoney));
		if ( m_GP[SendSit].IsInGame() )
		{
			nSendMoney = min(nSendMoney,m_GP[SendSit].GetTableMoney()-(m_MaxChipOnTable+m_nBigBlind));
		}

		if ( nSendMoney > 0 )
		{
			msgSM.m_nSendMoney = nSendMoney;
			msgRespSM.m_Flag = msgRespSM.SUCCESS;

			m_GP[SendSit].SetHaveSendMoney(true);
			m_GP[SendSit].DecTableMoney(msgSM.m_nSendMoney);
			m_GP[SendSit].m_OtherLoss += msgSM.m_nSendMoney;
			pSendPlayer->AddGameMoney(-msgSM.m_nSendMoney);
			pSendPlayer->UpdateGameMoney();
			pSendPlayer->AddMoneyLog(-msgSM.m_nSendMoney,"SendMoney");

			m_GP[RecvSit].AddTableMoney(msgSM.m_nSendMoney);
			m_GP[RecvSit].m_OtherLoss -= msgSM.m_nSendMoney;					
			pRecvPlayer->AddGameMoney(msgSM.m_nSendMoney);
			pRecvPlayer->UpdateGameMoney();
			pRecvPlayer->AddMoneyLog(msgSM.m_nSendMoney,"RecvMoney");

			m_pGameLog->AddStepString("SendMoney SendSitID="+N2S(SendSit+1)+" RecvSitID="+N2S(RecvSit+1)+" nMoney="+N2S(msgSM.m_nSendMoney.get()));

			msgRespSM.m_nSendMoney              = msgSM.m_nSendMoney;
			msgRespSM.m_SendPID                 = msgSM.m_SendPID;
			msgRespSM.m_SendPlayerTableMoney    = m_GP[SendSit].GetRealTableMoney();
			msgRespSM.m_RecvPID                 = msgSM.m_RecvPID;
			msgRespSM.m_RecvPlayerTableMoney    = m_GP[RecvSit].GetRealTableMoney();	

			DBServerXY::WDB_SendMoney msgDBSendMoney;
			msgDBSendMoney.m_SendAID            = pSendPlayer->GetAID();
			msgDBSendMoney.m_SendPID            = msgSM.m_SendPID;
			msgDBSendMoney.m_RecvAID            = pRecvPlayer->GetAID();
			msgDBSendMoney.m_RecvPID            = msgSM.m_RecvPID;
			msgDBSendMoney.m_nSendMoney = int(msgSM.m_nSendMoney);
			LogicSendMsgToDBServer(msgDBSendMoney);

			if ( m_GP[RecvSit].IsBotPlayer() && s_ThankFaceID.size()>0 )
			{
				bool bShowFace = false;
				if ( msgSM.m_nSendMoney >= 500 )
				{
					bShowFace = Tool::CRandom::GetChangce(3,1);
				}
				else if ( msgSM.m_nSendMoney >= 50 )
				{
					bShowFace = Tool::CRandom::GetChangce(10,1);
				}
				if ( bShowFace )
				{
					int Idx = Tool::CRandom::Random_Int(0,s_ThankFaceID.size()-1);
					int FaceID = s_ThankFaceID[Idx];

					DeZhouLogic::DeZhouLogic_ShowFace msgSF;
					msgSF.m_SitID       = RecvSit + 1;
					msgSF.m_PID         = pRecvPlayer->GetPID();
					msgSF.m_FaceID      = FaceID;
					msgSF.m_ActionTime  = m_CurTime + Tool::CRandom::Random_Int(5,10);
					PushMsgInList(msgSF);
				}
			}
		}
		else
		{
			msgRespSM.m_Flag = msgRespSM.OUTMONEY;
			m_pTable->DebugInfo("OnSendMoney SendPID=%d RecvPID=%d SendMoney=%s",
				msgSM.m_SendPID,msgSM.m_RecvPID,N2S(msgSM.m_nSendMoney.get()).c_str() );
		}
	}

	if ( msgRespSM.m_Flag == msgRespSM.SUCCESS )
	{
		SendLogicMsgToAllPlayer(msgRespSM);
	}
	else
	{
		SendLogicMsgToOnePlayer(msgRespSM,pPlayer);
	}	

	return 0;
}

int CDeZhouGameLogic::OnSendGift(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnSendGift");
	CLogFuncTime lft(s_LogicFuncTime,"OnSendGift");

	GameDeZhou_SendGift msgSG;
	TransplainMsgCTS(CTSMsg,msgSG);

	m_pTable->DebugInfo("OnSendGift SendPID=%d GiftID=%d Count=%d",msgSG.m_SendPID,msgSG.m_GiftID,msgSG.m_vecToPID.size());

	GameDeZhou_RespSendGift msgRespSG;

	if ( pPlayer && m_pServer && pPlayer->GetPID()==msgSG.m_SendPID && IsPlayerInTable(pPlayer->GetPID()) && msgSG.m_vecToPID.size() )
	{
		DBServerXY::DBS_msgGiftInfo stGI;
		if ( m_pServer->GetGiftInfo(msgSG.m_GiftID,stGI) )
		{
			int   nSendCount = int(msgSG.m_vecToPID.size());
			int   GiftPrice = 0;
			INT64 MaxCanUseMoney = 0;
			INT64 PayMoney = 0;
			bool  bUseTableMoney = false;

			if ( stGI.m_PriceFlag == N_Gift::PriceFlag_MoGui )
			{
				GiftPrice = stGI.m_Price;
				PayMoney = GiftPrice*nSendCount;
				MaxCanUseMoney = pPlayer->GetMoGuiMoney();

				if ( nSendCount <= 0 )
				{
					msgRespSG.m_Flag = msgRespSG.NOPLAYER;
				}
				else if ( GiftPrice <= 0 )
				{
					msgRespSG.m_Flag = msgRespSG.NOPRICE;
				}
				else
				{
					if ( MaxCanUseMoney>=PayMoney && PayMoney>0 )
					{
						pPlayer->DecMoGuiMoney(int(PayMoney));
						pPlayer->UpdateMoGuiMoney();

						msgRespSG.m_Flag      = msgRespSG.SUCCESS;
						msgRespSG.m_GiftID    = msgSG.m_GiftID;
						msgRespSG.m_SendPID   = msgSG.m_SendPID;
						msgRespSG.m_vecToPID  = msgSG.m_vecToPID;

						SendLogicMsgToAllPlayer(msgRespSG);
					}
					else
					{
						msgRespSG.m_Flag = msgRespSG.NOMOGUI;
					}
				}
			}
			else if ( stGI.m_PriceFlag==N_Gift::PriceFlag_Chang || stGI.m_PriceFlag == N_Gift::PriceFlag_Fixed )
			{
				if ( stGI.m_PriceFlag==N_Gift::PriceFlag_Chang )
				{
					GiftPrice = int((INT64(stGI.m_Price)*m_GiftBase)/100);
					GiftPrice = max(GiftPrice,stGI.m_MinPrice);
					GiftPrice = min(GiftPrice,stGI.m_MaxPrice);
				}
				else
				{
					GiftPrice = stGI.m_Price;
				}

				PayMoney = INT64(GiftPrice)*nSendCount;

				if ( nSendCount <= 0 )
				{
					msgRespSG.m_Flag = msgRespSG.NOPLAYER;
				}
				else if ( GiftPrice <= 0 )
				{
					msgRespSG.m_Flag = msgRespSG.NOPRICE;
				}
				else
				{
					if ( pPlayer->IsPlayerSitOnTable() )
					{
						int   SitID = pPlayer->GetSitID();
						if ( IsAvailSitID(SitID) )
						{
							int Sit = SitID - 1;
							if ( m_GP[Sit].IsInGame() )
							{
								MaxCanUseMoney = m_GP[Sit].GetTableMoney() - ( m_MaxChipOnTable + m_nBigBlind );
							}
							else
							{
								MaxCanUseMoney = m_GP[Sit].GetTableMoney() - ( m_nBigBlind+m_nServiceMoney );
							}

							if ( MaxCanUseMoney >= PayMoney )
							{
								bUseTableMoney = true;
								msgRespSG.m_Flag = msgRespSG.SUCCESS;
							}
							else
							{
								bUseTableMoney = false;

								if ( m_GP[Sit].IsInGame() )
								{
									MaxCanUseMoney = pPlayer->GetGameMoney() - (m_GP[Sit].GetOriginMoney()+m_nServiceMoney);
								}
								else
								{
									MaxCanUseMoney = pPlayer->GetGameMoney() - m_GP[Sit].GetTableMoney();
								}
								

								if ( MaxCanUseMoney >= PayMoney )
								{									
									msgRespSG.m_Flag = msgRespSG.SUCCESS;
								}
								else
								{
									msgRespSG.m_Flag = msgRespSG.NOMONEY;
								}
							}
						}
						else
						{
							m_pTable->DebugError("OnSendGift SitID=%d",SitID);
						}
					}
					else
					{
						bUseTableMoney = false;
						MaxCanUseMoney = pPlayer->GetGameMoney();
						if ( MaxCanUseMoney >= PayMoney )
						{							
							msgRespSG.m_Flag = msgRespSG.SUCCESS;
						}
						else
						{
							msgRespSG.m_Flag = msgRespSG.NOMONEY;
						}
					}

					if ( msgRespSG.m_Flag == msgRespSG.SUCCESS )
					{
						pPlayer->AddGameMoney(-PayMoney);
						pPlayer->AddMoneyLog(-PayMoney,"LogicOnSendGift");
						pPlayer->UpdateGameMoney();

						if ( bUseTableMoney )
						{
							int SitID = pPlayer->GetSitID();
							if ( IsAvailSitID(SitID) )
							{
								int Sit = SitID - 1;
								m_GP[Sit].DecTableMoney(PayMoney);
								SendTableMoneyBySitID(SitID);

								m_GP[Sit].m_OtherLoss += PayMoney;

								m_pGameLog->AddStepString("SendGift PayTable SitID="+N2S(pPlayer->GetSitID())+" PayMoney="+N2S(PayMoney));
							}
							else
							{
								m_pTable->DebugError("OnSendGift Can't Find Player SitID=%d",SitID);
							}
						}
						else
						{
							SendGameMoneyBySitID(pPlayer->GetSitID());

							m_pGameLog->AddStepString("SendGift PayGameMoney SitID="+N2S(pPlayer->GetSitID())+" PayMoney="+N2S(PayMoney));
						}

						msgRespSG.m_SendPID    = msgSG.m_SendPID;
						msgRespSG.m_GiftID     = msgSG.m_GiftID;
						msgRespSG.m_vecToPID   = msgSG.m_vecToPID;

						SendLogicMsgToAllPlayer(msgRespSG);
					}
				}
			}
			else
			{
				m_pTable->DebugError("OnSendGift PID=%d GiftID=%d PriceFlag=%d",pPlayer->GetPID(),stGI.m_GiftID,stGI.m_PriceFlag);
			}

			if ( msgRespSG.m_Flag == msgRespSG.SUCCESS )
			{
				DBServerXY::WDB_SendGift msgDBSendGift;
				msgDBSendGift.m_AID         = pPlayer->GetAID();
				msgDBSendGift.m_PID         = msgSG.m_SendPID;
				msgDBSendGift.m_GiftID      = msgSG.m_GiftID;
				msgDBSendGift.m_Price       = GiftPrice;
				msgDBSendGift.m_PriceFlag   = stGI.m_PriceFlag;
				msgDBSendGift.m_PayMoney    = PayMoney;
				msgDBSendGift.m_vecToPID    = msgSG.m_vecToPID;
				m_pTable->SendMsgToDBServer(msgDBSendGift);

				PlayerPtr pSendPlayer = m_pTable->GetTablePlayerByPID(msgSG.m_SendPID);
				if ( pSendPlayer )
				{
					pSendPlayer->m_SendGiftTimes += msgSG.m_vecToPID.size();
					pSendPlayer->CheckSendGift();
				}

				int nCount = msgSG.m_vecToPID.size();
				if ( nCount > 0 )
				{
					int Idx = Tool::CRandom::Random_Int(0,nCount-1);
					UINT32 RecvPID = msgSG.m_vecToPID[Idx];
					PlayerPtr pRecvPlayer = m_pTable->GetTablePlayerByPID(RecvPID);
					if ( pRecvPlayer && RecvPID!=msgSG.m_SendPID )
					{
						int SitID = pRecvPlayer->GetSitID();
						int Sit   = SitID - 1;
						if ( IsAvailSitID(SitID) && m_GP[Sit].IsBotPlayer() && m_GP[Sit].GetPID()==RecvPID )
						{
							if ( Tool::CRandom::GetChangce(2,1) )
							{
								int Idx = Tool::CRandom::Random_Int(0,s_ThankFaceID.size()-1);
								int FaceID = s_ThankFaceID[Idx];

								DeZhouLogic::DeZhouLogic_ShowFace msgSF;
								msgSF.m_SitID       = SitID;
								msgSF.m_PID         = RecvPID;
								msgSF.m_FaceID      = FaceID;
								msgSF.m_ActionTime  = m_CurTime + Tool::CRandom::Random_Int(5,10);
								PushMsgInList(msgSF);
							}
							else if ( Tool::CRandom::GetChangce(3,1) )
							{
								int GiftID = 0;
								if ( Tool::CRandom::GetChangce(4,1) )
								{
									GiftID = msgSG.m_GiftID;
								}
								else
								{
									int Idx = Tool::CRandom::Random_Int(0,s_ThankGiftID.size()-1);
									int GiftID = s_ThankGiftID[Idx];
								}

								if ( GiftID > 0 )
								{
									DeZhouLogic::DeZhouLogic_SendGift msgLogicSG;
									msgLogicSG.m_GiftID       = GiftID;
									msgLogicSG.m_SendPID      = RecvPID;
									msgLogicSG.m_SendSitID    = SitID;
									msgLogicSG.m_RecvPID      = msgSG.m_SendPID;
									msgLogicSG.m_ActionTime   = m_CurTime + Tool::CRandom::Random_Int(5,10);
									PushMsgInList(msgLogicSG);
								}
							}
						}
					}					
				}
			}
		}
	}

	if ( msgRespSG.m_Flag != msgRespSG.SUCCESS )
	{
		SendLogicMsgToOnePlayer(msgRespSG,pPlayer);
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
			pPlayer->CheckFriend();

			if ( ToPlayer )
			{
				ToPlayer->m_FriendCount++;
				ToPlayer->AddFollowFriend(msgAF.m_FromPID);
				ToPlayer->AddBeFollowFriend(msgAF.m_FromPID);
				ToPlayer->CheckFriend();
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

			if( IsCommondTable() && IsAvailSitID(pPlayer->GetSitID()) )
			{
				int nCurSitID = pPlayer->GetSitID();
				CGamePlayer& rTempGPlayer = m_GP[nCurSitID-1];

				rTempGPlayer.AddTableMoney(nMoney);
				rTempGPlayer.m_OtherLoss -= nMoney;
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
int CDeZhouGameLogic::OnReqChangeGift(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnReqChangeGift");
	CLogFuncTime lft(s_LogicFuncTime,"OnReqChangeGift");

	GameDeZhou_ReqChangGift msgCG;
	TransplainMsgCTS(CTSMsg,msgCG);

	GameDeZhou_RespChangGift msgRespCG;
	msgRespCG.m_PID = msgCG.m_PID;
	msgRespCG.m_Flag = msgRespCG.UNSUCCESS;

	if ( pPlayer && pPlayer->GetPID()==msgCG.m_PID )
	{
		DBServerXY::DBS_msgUserGiftInfo msgUGI;
		if ( pPlayer->GetCurUserGift(msgUGI,msgCG.m_GiftIdx) )
		{
			
			DBServerXY::DBS_msgGiftInfo msgGI;
			if ( GetGiftInfo(msgUGI.m_GiftID,msgGI) )
			{
				bool bCanUse = false;
				if ( msgGI.m_PriceFlag==N_Gift::PriceFlag_MoGui || msgGI.m_PriceFlag==N_Gift::PriceFlag_Fixed )
				{
					bCanUse = true;
				}
				else if ( msgGI.m_PriceFlag == N_Gift::PriceFlag_Chang )
				{
					int GiftPrice = int((INT64(msgGI.m_Price)*m_GiftBase)/100);
					GiftPrice = max(GiftPrice,msgGI.m_MinPrice);
					GiftPrice = min(GiftPrice,msgGI.m_MaxPrice);

					if ( msgUGI.m_Price >= GiftPrice )
					{
						bCanUse = true;
					}
				}
				if ( bCanUse )
				{
					pPlayer->m_CurGiftIdx = msgCG.m_GiftIdx;
					msgRespCG.m_Flag = msgRespCG.SUCCESS;
					msgRespCG.m_GiftID = msgUGI.m_GiftID;
				}
				else
				{
					msgRespCG.m_Flag = msgRespCG.CHEAP;
				}
			}
			else
			{
				msgRespCG.m_Flag = msgRespCG.NOGIFTID;
			}
		}
		else
		{
			if ( msgCG.m_GiftIdx == 0 )
			{
				pPlayer->m_CurGiftIdx = msgCG.m_GiftIdx;
				msgRespCG.m_Flag = msgRespCG.SUCCESS;
				msgRespCG.m_PID = msgUGI.m_GiftID;
				
			}
			else
			{
				msgRespCG.m_Flag = msgRespCG.NOGIFT;
			}			
		}
	}

	if ( msgRespCG.m_Flag == msgRespCG.SUCCESS )
	{
		SendLogicMsgToAllPlayer(msgRespCG);
	}
	else
	{
		SendLogicMsgToOnePlayer(msgRespCG,pPlayer);
	}

	return 0;
}
void CDeZhouGameLogic::DoJuBaoPengMsg()
{
	if ( m_listLogicMsg.size() )
	{
		TraceStackPath logTP("CDeZhouGameLogic::DoJuBaoPengMsg");
		CLogFuncTime lft(s_LogicFuncTime,"DoJuBaoPengMsg");

		vector<ListBlockMsg::iterator> vectorDel;
		for (ListBlockMsg::iterator itorMsg=m_listLogicMsg.begin();itorMsg!=m_listLogicMsg.end();++itorMsg)
		{
			PMsgBlock pMsg = *itorMsg;
			if ( pMsg.get() && pMsg->m_MsgID == DeZhouLogic::DeZhouLogic_XYID_JuBaoPeng )
			{
				vectorDel.push_back(itorMsg);
				DeZhouLogic::DeZhouLogic_JuBaoPeng msgJBP;
				ExplainBlockToMsg(*pMsg,msgJBP);

				int SitID = msgJBP.m_SitID;
				int Sit   = msgJBP.m_SitID - 1;
				if ( IsAvailSitID(SitID) && m_GP[Sit].GetPID()==msgJBP.m_PID && m_GP[Sit].m_JuBaoChip>0 && m_GP[Sit].IsAttendGame() )
				{
					m_GP[Sit].m_JuBaoChip  = msgJBP.m_nZhuChip;
					m_GP[Sit].m_JuBaoCount = msgJBP.m_nCount;
				}
			}
		}
		for (size_t nPos=0;nPos<vectorDel.size();++nPos)
		{
			m_listLogicMsg.erase(vectorDel[nPos]);
		}
	}
}
void CDeZhouGameLogic::DoAddMoney()
{
	if ( m_listLogicMsg.size() )
	{
		TraceStackPath logTP("CDeZhouGameLogic::DoAddMoney");
		CLogFuncTime lft(s_LogicFuncTime,"DoAddMoney");

		vector<ListBlockMsg::iterator> vectorDel;
		for (ListBlockMsg::iterator itorMsg=m_listLogicMsg.begin();itorMsg!=m_listLogicMsg.end();++itorMsg)
		{
			PMsgBlock pMsg = *itorMsg;
			if ( pMsg.get() && pMsg->m_MsgID == DeZhouLogic::DeZhouLogic_XYID_AddMoney )
			{
				vectorDel.push_back(itorMsg);
				DeZhouLogic::DeZhouLogic_AddMoney msgAM;
				ExplainBlockToMsg(*pMsg,msgAM);

				int SitID = msgAM.m_SitID;
				int Sit   = msgAM.m_SitID - 1;
				if ( IsAvailSitID(SitID) && m_GP[Sit].GetPID()==msgAM.m_PID && m_GP[Sit].IsAttendGame() )
				{
					PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(SitID);
					if ( pPlayer && pPlayer->GetPID()==msgAM.m_PID )
					{
						msgAM.m_nAddMoney = min(msgAM.m_nAddMoney,pPlayer->GetGameMoney() - m_GP[Sit].GetTableMoney());
						msgAM.m_nAddMoney = min(msgAM.m_nAddMoney,m_nMaxTakeIn - m_GP[Sit].GetTableMoney());
						if ( msgAM.m_nAddMoney > 0 )
						{
							m_GP[Sit].AddTableMoney(msgAM.m_nAddMoney);
							m_GP[Sit].m_OtherLoss -= msgAM.m_nAddMoney;

							m_pGameLog->AddStepString("DoAddMoney SitID="+N2S(msgAM.m_SitID)+" nAddMoney="+N2S(msgAM.m_nAddMoney));

							GameDeZhou_AddMoneyInfo msgAMInfo;
							msgAMInfo.m_Flag = msgAMInfo.SUCCESS;
							msgAMInfo.m_AddMoney = msgAM.m_nAddMoney;
							msgAMInfo.m_TableMoney = m_GP[Sit].GetTableMoney();
							msgAMInfo.m_GameMoney = pPlayer->GetGameMoney() - m_GP[Sit].GetTableMoney();
							SendLogicMsgToOnePlayer(msgAMInfo,pPlayer);

							SendTableMoneyBySitID(msgAM.m_SitID);
						}
					}
				}
			}
		}
		for (size_t nPos=0;nPos<vectorDel.size();++nPos)
		{
			m_listLogicMsg.erase(vectorDel[nPos]);
		}
	}
}
void CDeZhouGameLogic::DoWashOutPlayer()
{
	if ( m_listLogicMsg.size() )
	{
		TraceStackPath logTP("CDeZhouGameLogic::DoWashOutPlayer");
		CLogFuncTime lft(s_LogicFuncTime,"DoWashOutPlayer");

		m_pTable->DebugInfo("DoWashOutPlayer Start...");
		vector<ListBlockMsg::iterator> vectorDel;
		for (ListBlockMsg::iterator itorMsg=m_listLogicMsg.begin();itorMsg!=m_listLogicMsg.end();++itorMsg)
		{
			PMsgBlock pMsg = *itorMsg;
			if ( pMsg.get() && pMsg->m_MsgID == DeZhouLogic::DeZhouLogic_XYID_WashOutPlayer )
			{
				vectorDel.push_back(itorMsg);
				DeZhouLogic::DeZhouLogic_WashOutPlayer msgWOP;
				ExplainBlockToMsg(*pMsg,msgWOP);
		
				m_pTable->DebugInfo("WashOut Player Count=%d",msgWOP.m_listSit.size() );
				for (int Pos=0;Pos<int(msgWOP.m_listSit.size());++Pos)
				{
					int TempSitID = msgWOP.m_listSit[Pos]+1;
					PlayerPtr pTempPlayer = m_pTable->GetSitPlayerBySitID(TempSitID);
					if ( pTempPlayer && pTempPlayer->GetSitID() == TempSitID )
					{
						m_pTable->DebugInfo("DoWashOutPlayer PID=%d SitID=%d",pTempPlayer->GetPID(),pTempPlayer->GetSitID());
						LogicStandUpPlayer(pTempPlayer,pTempPlayer->GetSitID(),Action_Flag_SysLogic);
					}
					else
					{
						m_pTable->DebugError("WashOut SitID=%d",TempSitID);
						m_pTable->DebugError("WashOut Player=%d",pTempPlayer);
					}
				}
				TestSitPlayer();
			}
		}
		for (size_t nPos=0;nPos<vectorDel.size();++nPos)
		{
			m_listLogicMsg.erase(vectorDel[nPos]);
		}
		m_pTable->DebugInfo("DoWashOutPlayer End");
	}
}
void CDeZhouGameLogic::CheckLogicMsg()
{
	if ( m_listLogicMsg.size() )
	{
		TraceStackPath logTP("CDeZhouGameLogic::CheckLogicMsg");
		CLogFuncTime lft(s_LogicFuncTime,"CheckLogicMsg");

		INT64 nEndTime = Tool::GetMilliSecond();
		vector<ListBlockMsg::iterator> vectorDel;
		for (ListBlockMsg::iterator itorMsg=m_listLogicMsg.begin();itorMsg!=m_listLogicMsg.end();++itorMsg)
		{
			PMsgBlock pMsg = *itorMsg;
			if ( pMsg.get() && (nEndTime-pMsg->m_nTimeCount)>=1000*1800 )
			{
				vectorDel.push_back(itorMsg);
				m_pTable->DebugError("CheckLogicMsg MsgID=%d OutTime=%s",pMsg->m_MsgID,N2S(nEndTime-pMsg->m_nTimeCount).c_str());
			}
		}
		for (size_t nPos=0;nPos<vectorDel.size();++nPos)
		{
			m_listLogicMsg.erase(vectorDel[nPos]);
		}
	}
}
void CDeZhouGameLogic::DoSendGiftMsg()
{
	if ( m_listLogicMsg.size() )
	{
		TraceStackPath logTP("CDeZhouGameLogic::DoSendGiftMsg");
		CLogFuncTime lft(s_LogicFuncTime,"DoSendGiftMsg");

		vector<ListBlockMsg::iterator> vectorDel;
		for (ListBlockMsg::iterator itorMsg=m_listLogicMsg.begin();itorMsg!=m_listLogicMsg.end();++itorMsg)
		{
			PMsgBlock pMsg = *itorMsg;
			if ( pMsg.get() && pMsg->m_MsgID == DeZhouLogic::DeZhouLogic_XYID_SendGift )
			{				
				DeZhouLogic::DeZhouLogic_SendGift msgSG;
				ExplainBlockToMsg(*pMsg,msgSG);

				if ( m_CurTime >= msgSG.m_ActionTime )
				{
					vectorDel.push_back(itorMsg);

					m_pTable->DebugInfo("DoSendGiftMsg SendSitID=%d SendPID=%d RecvPID=%d GiftID=%d",
						msgSG.m_SendSitID,msgSG.m_SendPID,msgSG.m_RecvPID,msgSG.m_GiftID );

					int Sit = msgSG.m_SendSitID - 1;
					PlayerPtr pSendPlayer = m_pTable->GetSitPlayerBySitID(msgSG.m_SendSitID);
					if ( pSendPlayer && IsAvailSitID(msgSG.m_SendSitID) && m_GP[Sit].GetPID()==msgSG.m_SendPID )
					{
						GameXY::ClientToServerMessage CTS;
						GameDeZhou_SendGift msgReqSG;

						msgReqSG.m_GiftID    = msgSG.m_GiftID;
						msgReqSG.m_SendPID   = msgSG.m_SendPID;
						msgReqSG.m_vecToPID.push_back(msgSG.m_RecvPID);

						m_pTable->DebugInfo("SendPID=%d RecvPID=%d GiftID=%d",msgReqSG.m_SendPID,msgReqSG.m_vecToPID.at(0),msgReqSG.m_GiftID );

						MakeLogicCTS(msgReqSG,CTS);
						OnSendGift(pSendPlayer,CTS);
					}
				}
			}
		}
		for (size_t nPos=0;nPos<vectorDel.size();++nPos)
		{
			m_listLogicMsg.erase(vectorDel[nPos]);
		}
	}
}
void CDeZhouGameLogic::DoShowFaceMsg()
{
	if ( m_listLogicMsg.size() )
	{
		TraceStackPath logTP("CDeZhouGameLogic::DoShowFaceMsg");
		CLogFuncTime lft(s_LogicFuncTime,"DoShowFaceMsg");

		vector<ListBlockMsg::iterator> vectorDel;
		for (ListBlockMsg::iterator itorMsg=m_listLogicMsg.begin();itorMsg!=m_listLogicMsg.end();++itorMsg)
		{
			PMsgBlock pMsg = *itorMsg;
			if ( pMsg.get() && pMsg->m_MsgID == DeZhouLogic::DeZhouLogic_XYID_ShowFace )
			{				
				DeZhouLogic::DeZhouLogic_ShowFace msgSF;
				ExplainBlockToMsg(*pMsg,msgSF);

				if ( m_CurTime >= msgSF.m_ActionTime )
				{
					vectorDel.push_back(itorMsg);
					int Sit = msgSF.m_SitID - 1;
					if ( IsAvailSitID(msgSF.m_SitID) && m_GP[Sit].GetPID()==msgSF.m_PID )
					{
						BotShowFace(msgSF.m_SitID,msgSF.m_FaceID);
					}
				}				
			}
		}
		for (size_t nPos=0;nPos<vectorDel.size();++nPos)
		{
			m_listLogicMsg.erase(vectorDel[nPos]);
		}
	}
}
int CDeZhouGameLogic::OnJoinJuBaoPeng(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnJoinJuBaoPeng");
	CLogFuncTime lft(s_LogicFuncTime,"OnJoinJuBaoPeng");

	GameDeZhou_ReqJoinJuBaoPeng msgJJBP;
	TransplainMsgCTS(CTSMsg,msgJJBP);

	if ( IsCommondTable() && IsAvailSitID(msgJJBP.m_SitID) && msgJJBP.m_SitID==pPlayer->GetSitID() )
	{
		int Sit = msgJJBP.m_SitID - 1;
		if ( m_GP[Sit].GetPID() == pPlayer->GetPID() )
		{
			msgJJBP.m_nZhuChip     = max(msgJJBP.m_nZhuChip,m_nBigBlind/10);
			msgJJBP.m_nZhuChip     = max(msgJJBP.m_nZhuChip,N_JuBaoPeng::cs_LowZhuChip);
			msgJJBP.m_nZhuChip     = min(msgJJBP.m_nZhuChip,N_JuBaoPeng::cs_TopZhuChip);
			msgJJBP.m_nZhuChip     = min(msgJJBP.m_nZhuChip,10*m_nBigBlind);
			msgJJBP.m_nCount       = max(msgJJBP.m_nCount,0);
			if (msgJJBP.m_nCount>1) msgJJBP.m_nCount = 10000;			
			if ( msgJJBP.m_nCount * msgJJBP.m_nZhuChip == 0 )
			{
				msgJJBP.m_nCount = msgJJBP.m_nZhuChip = 0;
			}

			GameDeZhou_RespJoinJuBaoPeng msgRespJJBP;
			msgRespJJBP.m_Flag     = msgRespJJBP.SUCCESS;
			msgRespJJBP.m_nCount   = msgJJBP.m_nCount;
			msgRespJJBP.m_nZhuChip = msgJJBP.m_nZhuChip;
			pPlayer->SendMsg(msgRespJJBP);

			if ( m_GP[Sit].m_JuBaoChip > N_JuBaoPeng::cs_JuBaoPengFlagMoney )
			{
				GameDeZhou_JoinJuBaoPengInfo msgInfo;
				msgInfo.m_SitID        = msgJJBP.m_SitID;
				msgInfo.m_Flag         = msgJJBP.m_nCount>0?msgInfo.JoinJuBaoPeng:msgInfo.ExitJuBaoPeng;
				SendLogicMsgToAllPlayer(msgInfo);

				DeZhouLogic::DeZhouLogic_JuBaoPeng logicmsg_JBP;
				logicmsg_JBP.m_SitID       = msgJJBP.m_SitID;
				logicmsg_JBP.m_PID         = pPlayer->GetPID();
				logicmsg_JBP.m_nZhuChip    = msgJJBP.m_nZhuChip;
				logicmsg_JBP.m_nCount      = msgJJBP.m_nCount;
				PushMsgInList(logicmsg_JBP);
			}
			else
			{
				if ( msgJJBP.m_nCount>0 && msgJJBP.m_nZhuChip>0 && m_GP[Sit].m_JuBaoCount*m_GP[Sit].m_JuBaoChip == 0 )
				{
					m_GP[Sit].m_JuBaoCount = msgJJBP.m_nCount;
					m_GP[Sit].m_JuBaoChip  = msgJJBP.m_nZhuChip;

					GameDeZhou_JoinJuBaoPengInfo msgInfo;
					msgInfo.m_SitID        = msgJJBP.m_SitID;
					msgInfo.m_Flag         = msgInfo.JoinJuBaoPeng;
					SendLogicMsgToAllPlayer(msgInfo);
				}
				else if( msgJJBP.m_nCount*msgJJBP.m_nZhuChip==0 && m_GP[Sit].m_JuBaoCount>0 && m_GP[Sit].m_JuBaoChip>0 )
				{
					m_GP[Sit].m_JuBaoCount = msgJJBP.m_nCount;
					m_GP[Sit].m_JuBaoChip  = msgJJBP.m_nZhuChip;

					GameDeZhou_JoinJuBaoPengInfo msgInfo;
					msgInfo.m_SitID        = msgJJBP.m_SitID;
					msgInfo.m_Flag         = msgInfo.ExitJuBaoPeng;
					SendLogicMsgToAllPlayer(msgInfo);
				}
			}
		}
	}

	return 0;
}

int CDeZhouGameLogic::OnReqAddTableTime(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnReqAddTableTime");
	CLogFuncTime lft(s_LogicFuncTime,"OnReqAddTableTime");

	GameDeZhou_ReqAddTableTime msgATT;
	TransplainMsgCTS(CTSMsg,msgATT);

	DBServerXY::WDB_IncomeAndPay msgAIP;
	msgAIP.m_PID = pPlayer->GetPID();
	msgAIP.m_AID = pPlayer->GetAID();
	msgAIP.m_Flag = N_IncomeAndPay::AddTableTime;

	GameDeZhou_RespAddTableTime msgRespATT;
	msgRespATT.m_Flag = msgRespATT.UNSUCCESS;
	msgRespATT.m_PID = msgATT.m_PID;

	if ( pPlayer && pPlayer->GetPID()==msgATT.m_PID && msgATT.m_AddTime>=m_pGSConfig->m_cfPT.m_minAddTime 
		&& msgATT.m_AddTime<=m_pGSConfig->m_cfPT.m_maxAddTime && msgATT.m_AddTime )
	{
		CGameRoom* pRoom = m_pServer->GetRoomByID(msgATT.m_RoomID);
		if ( pRoom )
		{
			CGameTable* pTable = pRoom->GetTableByID(msgATT.m_TableID);
			if ( pTable && pTable->IsTablePrivate() )
			{				
				int   nHour = msgATT.m_AddTime;
				int   nPayMoney = nHour*m_nBigBlind*m_pGSConfig->m_cfPT.m_OneHourMul;
				INT64 MaxCanUseMoney = 0;
				bool  bUseTableMoney = false;

				if ( nPayMoney>0 && IsPlayerInTable(pPlayer->GetPID()) )
				{
					if ( pPlayer->IsPlayerSitOnTable() )
					{
						int SitID = pPlayer->GetSitID();
						if ( IsAvailSitID(SitID) )
						{
							int Sit = SitID - 1;
							if ( m_GP[Sit].IsInGame() )
							{
								MaxCanUseMoney = m_GP[Sit].GetTableMoney() - max( m_MaxChipOnTable,m_nBigBlind );
							}
							else
							{
								MaxCanUseMoney = m_GP[Sit].GetTableMoney() - ( m_nBigBlind+m_nServiceMoney );
							}

							if ( MaxCanUseMoney >= nPayMoney )
							{
								bUseTableMoney = true;
								msgRespATT.m_Flag = msgRespATT.SUCCESS;
							}
							else
							{
								bUseTableMoney = false;

								if ( m_GP[Sit].IsInGame() )
								{
									MaxCanUseMoney = pPlayer->GetGameMoney() - (m_GP[Sit].GetOriginMoney()+m_nServiceMoney);
								}
								else
								{
									MaxCanUseMoney = pPlayer->GetGameMoney() - m_GP[Sit].GetTableMoney();
								}

								if ( MaxCanUseMoney >= nPayMoney )
								{									
									msgRespATT.m_Flag = msgRespATT.SUCCESS;
								}
								else
								{
									msgRespATT.m_Flag = msgRespATT.OUTMONEY;
								}
							}
						}
						else
						{
							m_pTable->DebugError("OnReqAddTableTime SitID=%d",SitID);
						}
					}
					else
					{
						bUseTableMoney = false;
						MaxCanUseMoney = pPlayer->GetGameMoney();
						if ( MaxCanUseMoney >= nPayMoney )
						{							
							msgRespATT.m_Flag = msgRespATT.SUCCESS;
						}
						else
						{
							msgRespATT.m_Flag = msgRespATT.OUTMONEY;
						}
					}
				}

				if ( msgRespATT.m_Flag == msgRespATT.SUCCESS )
				{
					msgAIP.m_nMoney = nPayMoney;
					msgAIP.m_strReMark = Tool::N2S(pTable->GetRoomID()*10000+pTable->GetTableID());

					pTable->AddTableTime(nHour*3600);
					pPlayer->AddGameMoney(-nPayMoney);
					pPlayer->AddMoneyLog(-nPayMoney,"OnReqAddTableTime");
					pPlayer->UpdateGameMoney();

					if ( bUseTableMoney )
					{
						int SitID = pPlayer->GetSitID();
						if ( IsAvailSitID(SitID) )
						{
							int Sit = SitID - 1;
							m_GP[Sit].DecTableMoney(nPayMoney);
							SendTableMoneyBySitID(SitID);

							m_GP[Sit].m_OtherLoss += nPayMoney;					

							m_pGameLog->AddStepString("AddTableTime PayTable SitID="+N2S(pPlayer->GetSitID())+" PayMoney="+N2S(nPayMoney));
						}
						else
						{
							m_pTable->DebugError("OnSendGift Can't Find Player SitID=%d",SitID);
						}
					}
					else
					{
						SendGameMoneyBySitID(pPlayer->GetSitID());

						m_pGameLog->AddStepString("AddTableTime PayGameMoney SitID="+N2S(pPlayer->GetSitID())+" PayMoney="+N2S(nPayMoney));
					}

					msgRespATT.m_AddTime = nHour*3600;
					msgRespATT.m_nLeftTime = pTable->GetTableLeftTime();
				}
			}
			else
			{
				msgRespATT.m_Flag = msgRespATT.NOTABLE;
			}
		}
		else
		{
			msgRespATT.m_Flag = msgRespATT.NOROOM;
		}
	}

	if ( msgRespATT.m_Flag == msgRespATT.SUCCESS )
	{
		SendLogicMsgToAllPlayer(msgRespATT);
		LogicSendMsgToDBServer(msgAIP);		
	}
	else
	{
		SendLogicMsgToOnePlayer(msgRespATT,pPlayer);
	}

	return 0;
}

int CDeZhouGameLogic::OnAddMoney(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnAddMoney");
	CLogFuncTime lft(s_LogicFuncTime,"OnAddMoney");

	GameDeZhou_RespAddMoney msgAM;
	TransplainMsgCTS(CTSMsg,msgAM);

	GameDeZhou_AddMoneyInfo msgRespAMI;
	msgRespAMI.m_Flag = msgRespAMI.UNSUCCESS;

	int Sit = msgAM.m_SitID - 1;

	if ( pPlayer && IsAvailSitID(msgAM.m_SitID) )
	{
		CGamePlayer& rGPlayer = m_GP[msgAM.m_SitID-1];
		if ( pPlayer->GetTableID()==m_pTable->GetTableID() && pPlayer->GetSitID()==msgAM.m_SitID 
			&& msgAM.m_AddMoney>0 && rGPlayer.GetPID()==pPlayer->GetPID() )
		{
			msgAM.m_AddMoney = max(msgAM.m_AddMoney,INT64(m_pGSConfig->m_cfServer.m_AddMoney)*m_nBigBlind);

			if ( IsMatchTable() )
			{
				msgRespAMI.m_Flag = msgRespAMI.MATCH;
			}
			else
			{
				if ( rGPlayer.IsHavePlayer() )
				{
					if ( rGPlayer.IsInGame() )
					{
						msgRespAMI.m_Flag = msgRespAMI.PLAYING;

						DeZhouLogic::DeZhouLogic_AddMoney logicmsgAM;
						logicmsgAM.m_SitID      = msgAM.m_SitID;
						logicmsgAM.m_PID        = pPlayer->GetPID();
						logicmsgAM.m_nAddMoney  = msgAM.m_AddMoney;
						PushMsgInList(logicmsgAM);
					}
					else
					{
						msgAM.m_AddMoney = min(msgAM.m_AddMoney,pPlayer->GetGameMoney() - rGPlayer.GetTableMoney());
						msgAM.m_AddMoney = min(msgAM.m_AddMoney,m_nMaxTakeIn - rGPlayer.GetTableMoney());

						if ( rGPlayer.GetTableMoney() >= m_nMaxTakeIn )
						{
							msgRespAMI.m_Flag = msgRespAMI.FULLMONEY;
						}
						else if ( msgAM.m_AddMoney <= 0 )
						{
							msgRespAMI.m_Flag = msgRespAMI.OUTMONEY;
						}
						else
						{
							rGPlayer.AddTableMoney(msgAM.m_AddMoney);
							rGPlayer.m_OtherLoss -= msgAM.m_AddMoney;

							m_pGameLog->AddStepString("AddMoney SitID="+N2S(msgAM.m_SitID)+" nAddMoney="+N2S(msgAM.m_AddMoney.get()));

							msgRespAMI.m_Flag       = msgRespAMI.SUCCESS;
							msgRespAMI.m_AddMoney   = msgAM.m_AddMoney;
							msgRespAMI.m_TableMoney = rGPlayer.GetTableMoney();
							msgRespAMI.m_GameMoney  = pPlayer->GetGameMoney() - rGPlayer.GetTableMoney();
						}
					}
				}
			}
		}
	}

	SendLogicMsgToOnePlayer(msgRespAMI,pPlayer);

	if ( msgRespAMI.m_Flag == msgRespAMI.SUCCESS )
	{
		SendTableMoneyBySitID(msgAM.m_SitID);
	}

	return 0;
}

int CDeZhouGameLogic::OnReqShowPai(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnReqShowPai");
	CLogFuncTime lft(s_LogicFuncTime,"OnReqShowPai");

	GameDeZhou_ReqShowPai msgSP;
	TransplainMsgCTS(CTSMsg,msgSP);

	if ( pPlayer && IsAvailSitID(msgSP.m_SitID) && pPlayer->GetTableID() == m_pTable->GetTableID() 
		&& pPlayer->GetSitID()==msgSP.m_SitID && msgSP.m_SitID == m_CanShowPaiSitID )
	{
		GameDeZhou_ShowPai msgRespSP;
		msgRespSP.m_SitID = msgSP.m_SitID;

		stHandPai stHP = m_GP[msgSP.m_SitID-1].m_HandPai;
		msgRespSP.m_HandPai[0] = stHP.m_Pai[0];
		msgRespSP.m_HandPai[1] = stHP.m_Pai[1];

		SendLogicMsgToAllPlayer(msgRespSP);

		if ( !(stHP.IsValidPai()) )
		{
			m_pTable->DebugError("OnReqShowPai pai1=%d pai2=%d",stHP.m_Pai[0],stHP.m_Pai[1]);
			DebugAllPlayer();
		}
	}
	else
	{
		m_pTable->DebugError("OnReqShowPai PID=%d sitid=%d",pPlayer->GetPID(),msgSP.m_SitID);
	}

	return 0;
}

int CDeZhouGameLogic::OnPlayerAddChip(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTSMsg)
{
	TraceStackPath logTP("CDeZhouGameLogic::OnPlayerAddChip");
	CLogFuncTime lft(s_LogicFuncTime,"OnPlayerAddChip");

 	GameDeZhou_ReqAddChip ad;
	TransplainMsgCTS(CTSMsg,ad);

	m_pTable->CeShiInfo("OnPlayerAddChip Start...ChipSitID=%d ChipMoney=%s MaxChip=%s MinAdd=%s",
		ad.m_CurPlayerSitID,N2S(ad.m_nCurChipMoney.get()).c_str(),N2S(m_MaxChipOnTable).c_str(),N2S(m_MinAddChip).c_str() );

	if( IsGamePlaying() && pPlayer && ad.m_CurPlayerSitID == m_CurPlayerSitID && m_GP[m_CurPlayerSitID-1].IsPlaying() && 
		pPlayer->GetTableID() == m_pTable->GetTableID() && pPlayer->GetSitID() == m_CurPlayerSitID )
	{
		int playersit = m_CurPlayerSitID - 1;

		bool bEndGame = false;
		bool bEndCall = false;
		bool bSuccess = false;
		bool bChangMaxPlayer = false;   //记录是否需要重新计算第一个玩家和最后一个玩家

		INT64 nCurMaxChipOnTable = m_MaxChipOnTable;
		INT64 nCurMinAddChip = m_MinAddChip;

		if ( m_pGSConfig->CeshiMode() )
		{
			INT64 CurMaxChip = GetCurMaxChipOnTable();
			if ( nCurMaxChipOnTable != CurMaxChip )
			{
				m_pTable->DebugError("nCurMaxChipOnTable=%s MaxChip=%s",N2S(nCurMaxChipOnTable).c_str(),N2S(CurMaxChip).c_str());
				DebugAllPlayer();
			}
		}

		GameDeZhou_AddChipInfo acInfo;
		acInfo.m_CurPlayerSitID = ad.m_CurPlayerSitID;
		if ( ad.m_nCurChipMoney <= 0 )
		{
			if ( (ad.m_nCurChipMoney < 0)
				|| (ad.m_ChipAction == ACTION_GiveUp) 
				|| ( m_GP[playersit].GetCurChip() < nCurMaxChipOnTable ) )
			{
				acInfo.m_ChipAction = ACTION_GiveUp;
				m_GP[playersit].SetGameState(GAME_PLAYER_ST_GIVEUP);

				if ( this->GetAvailPlayerNumber()<=1 || GetPlayingPlayerNumber()==0 )
				{
					bEndCall = true;
					bEndGame = true;
				}
			}
			else
			{
				acInfo.m_ChipAction = ACTION_KanPai;
			}

			acInfo.m_nCurChipMoney = m_GP[playersit].GetCurChip();
			acInfo.m_nTableMoney = m_GP[playersit].GetTableMoney() - m_GP[playersit].GetCurChip();

			if ( m_CurPlayerSitID == this->m_EndPlayerSitID )
			{
				bEndCall = true;
				if( m_GameState == GAME_ST_RIVER || this->GetPlayingPlayerNumber() <= 1 )
				{
					bEndGame = true;
				}
			}			

			if ( (!bEndCall) && (!bEndGame) )
			{
				m_CurPlayerSitID = GetNextPlayingPlayerSitID(m_CurPlayerSitID);
				if ( m_CurPlayerSitID == 0 )
				{
					m_pTable->DebugError("OnPlayerAddChip m_CurPlayerSitID==0 Chip=%s",N2S(ad.m_nCurChipMoney.get()).c_str() );
				}
			}
		}
		else
		{
			INT64 MaxCanChip = nCurMaxChipOnTable;
			for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				if ( Sit== playersit )
				{
					continue;
				}
				if( m_GP[Sit].IsPlaying() ) //用nCurMaxChipOnTable将所有ALLIN的玩家淘汰掉
				{
					if ( m_GP[Sit].GetTableMoney() > MaxCanChip )
					{
						MaxCanChip = m_GP[Sit].GetTableMoney();
					}
				}
			}
			if ( MaxCanChip == 0 )
			{
				m_pTable->DebugError("OnPlayerAddChip MaxCanChip=0 TableID=%d SitID=%d",m_pTable->GetTableID(),m_CurPlayerSitID);
				this->DebugAllPlayer();
			}

			INT64 MinMustChip = max(nCurMaxChipOnTable,m_nBigBlind);
			ad.m_nCurChipMoney = max(ad.m_nCurChipMoney,m_MinAddChip);
			ad.m_nCurChipMoney = max(ad.m_nCurChipMoney,m_nBigBlind);
			ad.m_nCurChipMoney = max(ad.m_nCurChipMoney,nCurMaxChipOnTable);
			if ( ad.m_nCurChipMoney > nCurMaxChipOnTable )//加注额必须为最小加注量的倍数，先增上去再根据实际情况用后面的条件降下来
			{
				if ( (ad.m_nCurChipMoney-nCurMaxChipOnTable) % m_MinAddChip )
				{
					ad.m_nCurChipMoney = nCurMaxChipOnTable+((ad.m_nCurChipMoney-nCurMaxChipOnTable)/m_MinAddChip+1)*m_MinAddChip;
				}
			}

			if ( m_Limite ==  TABLE_LIMITE_TEXAS )
			{
				ad.m_nCurChipMoney = min(ad.m_nCurChipMoney,MinMustChip+m_MinAddChip);
				ad.m_nCurChipMoney = min(ad.m_nCurChipMoney,m_MinAddChip*4);
			}
			else if ( m_Limite == TABLE_LIMITE_POT )
			{
				INT64 PotMoney = 0;
				for ( int Idx=0;Idx<MAX_PALYER_ON_TABLE;Idx++)
				{
					PotMoney += m_TablePoolMoneys[Idx].m_nPoolMoney;
				}
				for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
				{
					if ( m_GP[Sit].IsAttendGame() )
					{
						PotMoney += m_GP[Sit].GetCurChip();
					}
				}
				PotMoney += (nCurMaxChipOnTable-m_GP[playersit].GetCurChip());
				
				ad.m_nCurChipMoney = min(ad.m_nCurChipMoney,PotMoney*2);
			}
			
			ad.m_nCurChipMoney = min(ad.m_nCurChipMoney,MaxCanChip);
			ad.m_nCurChipMoney = min(ad.m_nCurChipMoney,m_GP[playersit].GetTableMoney());

			if ( nCurMaxChipOnTable == 0 )
			{
				bChangMaxPlayer = true;
				if ( ad.m_nCurChipMoney > m_MinAddChip )
				{
					acInfo.m_ChipAction = ACTION_AddChip;
				}
				else
				{
					acInfo.m_ChipAction = ACTION_Chip;
				}
				m_MaxChipOnTable = ad.m_nCurChipMoney;
				m_MinAddChip     = max(ad.m_nCurChipMoney,m_MinAddChip);
			}
			else
			{
				if ( ad.m_nCurChipMoney > nCurMaxChipOnTable )
				{
					bChangMaxPlayer = true;
					acInfo.m_ChipAction = ACTION_AddChip;
				}
				else
				{
					if ( ad.m_nCurChipMoney > m_GP[playersit].GetCurChip() )
					{
						acInfo.m_ChipAction = ACTION_Follow;
					}
					else
					{
						acInfo.m_ChipAction = ACTION_KanPai;
					}
				}
				m_MinAddChip     = max(ad.m_nCurChipMoney-nCurMaxChipOnTable,m_MinAddChip);
				m_MaxChipOnTable = max(ad.m_nCurChipMoney,m_MaxChipOnTable);
			}

			if ( m_Limite == TABLE_LIMITE_TEXAS )
			{
				if ( m_GameState==GAME_ST_HAND || m_GameState==GAME_ST_FLOP )
				{
					m_MinAddChip = m_nBigBlind;
				}
				else if ( m_GameState==GAME_ST_TURN || m_GameState==GAME_ST_RIVER )
				{
					m_MinAddChip = m_nBigBlind*2;
				}
			}
		
			if ( ad.m_nCurChipMoney > 0 && ad.m_nCurChipMoney == m_GP[playersit].GetTableMoney() )
			{
				//acInfo.m_ChipAction |= ACTION_AllIn;
				acInfo.m_ChipAction = ACTION_AllIn;
				m_GP[playersit].SetGameState(GAME_PLAYER_ST_ALLIN);
				if ( m_GameState == GAME_ST_HAND )
				{
					m_GP[playersit].SetAllIn(true);
				}
			}

			if ( ad.m_nCurChipMoney > m_GP[playersit].GetTableMoney() )
			{
				m_pTable->DebugError("OnPlayerAddChip CurChip=%s TableMoney=%s",
					Tool::N2S(ad.m_nCurChipMoney.get()).c_str(),Tool::N2S(m_GP[playersit].GetTableMoney()).c_str() );
			}

			acInfo.m_nCurChipMoney = ad.m_nCurChipMoney;
			acInfo.m_nTableMoney   = m_GP[playersit].GetTableMoney() - ad.m_nCurChipMoney;

			m_GP[playersit].SetCurChip(ad.m_nCurChipMoney);

			if ( bChangMaxPlayer )
			{
				m_FirstPlayerSitID = m_CurPlayerSitID;                                  //加注后当前玩家变为第一个玩家
				m_EndPlayerSitID = this->GetPrePlayingPlayerSitID(m_FirstPlayerSitID);
				m_CurPlayerSitID = this->GetNextPlayingPlayerSitID(m_CurPlayerSitID);
				assert( m_EndPlayerSitID && m_CurPlayerSitID && "OnPlayerAddChip" );
				if ( m_CurPlayerSitID == 0 )
				{
					m_pTable->DebugError("ChangMaxPlayer OnPlayerAddChip m_CurPlayerSitID==0 非正常结束游戏");
					bEndCall = true;
					bEndGame = true;
				}
			}
			else
			{
				if ( m_CurPlayerSitID == this->m_EndPlayerSitID )
				{
					bEndCall = true;
					if ( GetPlayingPlayerNumber() <= 1 || m_GameState == GAME_ST_RIVER )
					{
						bEndGame = true;
					}
				}
				else if ( GetPlayingPlayerNumber() <= 1 && IsAvailSitID(m_EndPlayerSitID) && m_GP[m_EndPlayerSitID-1].IsAllIn() )
				{
					bEndCall = true;
					bEndGame = true;
				}
				else
				{
					m_CurPlayerSitID = this->GetNextPlayingPlayerSitID(m_CurPlayerSitID);
					if ( m_CurPlayerSitID == 0 )
					{
						m_pTable->DebugError("OnPlayerAddChip m_CurPlayerSitID == 0");
					}
				}
			}
		}

		if ( (!bEndCall) && (!bEndGame) )
		{
			if ( IsAvailSitID(m_CurPlayerSitID) && GetPlayingPlayerNumber()==1 && m_GP[m_CurPlayerSitID-1].GetCurChip()==m_MaxChipOnTable )
			{
				bEndCall = true;
				bEndGame = true;
			}
		}

		if ( m_CurPlayerSitID == 0 )
		{
			m_pTable->DebugError("OnPlayerAddChip EndCall=%d EndGame=%d",int(bEndCall),int(bEndGame) );

			bEndCall = true;
			bEndGame = true;
		}

		if ( m_GP[playersit].m_AutoOutTimes < 100)
		{
			m_GP[playersit].m_AutoOutTimes = 0;
		}
		m_GP[playersit].SetLastAction(acInfo.m_ChipAction);
		SendLogicMsgToAllPlayer(acInfo);

		assert( acInfo.m_nCurChipMoney>=0 && acInfo.m_nTableMoney>=0 );

		PlayerChipStep csChip;
		csChip.m_SitID = ad.m_CurPlayerSitID;
		csChip.m_nMoney = ad.m_nCurChipMoney;
		csChip.m_LeftMoney = m_GP[playersit].GetRealTableMoney();
		csChip.m_Step = m_GameState;
		m_pGameLog->AddStepString(csChip.GetString());

		m_pTable->CeShiInfo("GS=%d AID=%d PID=%d SitID=%d Action=%d Max=%s Chip=%s Left=%s",
			m_GameState,pPlayer->GetAID(),pPlayer->GetPID(),acInfo.m_CurPlayerSitID,acInfo.m_ChipAction,
			Tool::N2S(m_MaxChipOnTable).c_str(),Tool::N2S(acInfo.m_nCurChipMoney.get()).c_str(),Tool::N2S(acInfo.m_nTableMoney.get()).c_str() );

		m_ActionTime = m_CurTime;

		if( bEndCall )
		{
			if ( bEndGame )
			{
				EndGame();
			}
			else if ( m_GameState == GAME_ST_HAND )
			{
				OnFlop();
			}
			else if ( m_GameState == GAME_ST_FLOP )
			{
				OnTurn();
			}
			else if ( m_GameState == GAME_ST_TURN )
			{
				OnRiver();
			}
			else
			{				
				m_pTable->DebugError("OnPlayerAddChip tid=%d state=%d",m_pTable->GetTableID(),m_GameState);
			}
		}
		else
		{
			SendPlayerPower();
		}
	}
	else
	{
		if ( pPlayer )
		{
			m_pTable->DebugError("OnPlayerAddChip AID=%d PID=%d tid=%-3d ptid=%-3d sid=%d psid=%d PST=%d GS=%d",
				pPlayer->GetAID(),pPlayer->GetPID(),
				m_pTable->GetTableID(),pPlayer->GetTableID(),m_CurPlayerSitID,pPlayer->GetSitID(),pPlayer->GetPlayerState(),m_GameState );
		}
		else
		{
			m_pTable->DebugError("OnPlayerAddChip TID=%d SitID=%d GS=%d",m_pTable->GetTableID(),m_CurPlayerSitID,m_GameState);
		}
	}

	//m_pTable->CeShiInfo("OnPlayerAddChip End...");
	return 0;
}

void CDeZhouGameLogic::SendGameMoneyBySitID( const int& SitID,PlayerPtr pPlayer )
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

void CDeZhouGameLogic::SendTableMoneyBySitID(const int& SitID,PlayerPtr pPlayer)
{
	if ( IsAvailSitID(SitID) )
	{
		GameDeZhou_TableMoney msgTM;
		msgTM.m_SitID = SitID;

		if ( m_GP[SitID-1].IsInGame() )
		{
			msgTM.m_nTableMoney = m_GP[SitID-1].GetTableMoney() - m_GP[SitID-1].GetCurChip();
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
void CDeZhouGameLogic::SendTablePai(BYTE State,PlayerPtr pPlayer)
{
	GameDeZhou_TablePai tp;
	if ( State == GAME_ST_FLOP )
	{
		tp.m_PaiCount = 3;
	}
	else if (  State == GAME_ST_TURN )
	{
		tp.m_PaiCount = 4;
	}
	else if ( State == GAME_ST_RIVER || State == GAME_ST_DIVIDEMONEY )
	{
		tp.m_PaiCount = 5;
	}
	else
	{		
		return ;
	}
	
	for (int i=0;i<tp.m_PaiCount;i++)
	{
		tp.m_TablePai[i] = m_GamePaiLogic.GetTablePai(i);
	}
	if ( pPlayer )
	{
		SendLogicMsgToOnePlayer(tp,pPlayer);
	}
	else
	{
		SendLogicMsgToAllPlayer(tp);
	}
}
void CDeZhouGameLogic::SendBankerInfo(PlayerPtr pPlayer)
{
	GameDeZhou_BankerInfo bi;
	bi.m_BankerSitID = m_BankerSitID;
	bi.m_BigBlindSitID = m_BigBlindSitID;
	bi.m_SmallBlindSitID = m_SmallBlindSitID;

	if ( pPlayer )
	{
		SendLogicMsgToOnePlayer(bi,pPlayer);
	}
	else
	{
		SendLogicMsgToAllPlayer(bi);
	}
}
void CDeZhouGameLogic::SendMoneyPool(PlayerPtr pPlayer)
{
	GameDeZhou_MoneyPool mp;
	for (int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
	{
		if ( m_TablePoolMoneys[PoolIdx].m_nPoolMoney > 0 )
		{				
			mp.m_MoneyPools[mp.m_MoneyPoolCount++] = m_TablePoolMoneys[PoolIdx].m_nPoolMoney;
		}
	}
	if ( mp.m_MoneyPoolCount > 0)
	{
		if ( pPlayer )
		{
			SendLogicMsgToOnePlayer(mp,pPlayer);
		}
		else
		{
			SendLogicMsgToAllPlayer(mp);
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

	if ( IsMatchTable() ) pPlayer->m_OneLifeMatchTime += nSitTime;
	else  pPlayer->m_OneLifeGameTime += nSitTime;

	pPlayer->m_TodayGameSecond += nSitTime;
	pPlayer->CheckPlayAndInviteAward();

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
			m_pTable->DebugError("MatchPlayerStandUp m_PromoteTimeStart=%d m_TimePass=%d",pPlayer->m_PromoteTimeStart,pPlayer->m_TimePass );
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

void CDeZhouGameLogic::SendReqMoney(int SitID,UINT32 PID)
{
	TraceStackPath logTP("CDeZhouGameLogic::SendReqMoney");
	CLogFuncTime lft(s_LogicFuncTime,"SendReqMoney");

	if ( !IsAvailSitID(SitID) )
	{
		m_pTable->DebugError("SendReqMoney SitID=%d PID=%d",SitID,PID);
		return ;
	}

	CGamePlayer& rGPlayer = m_GP[SitID-1];
	if ( rGPlayer.GetReqMoneyTime() == 0 )
	{
		if ( rGPlayer.IsNotBotPlayer() )
		{
			PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(SitID);
			if ( pPlayer )
			{
				GameDeZhou_ReqAddMoney msgAM;
				msgAM.m_SitID   = SitID;
				msgAM.m_MinAdd  = min(pPlayer->GetGameMoney(),m_nMinTakeIn);
				msgAM.m_MaxAdd  = min(pPlayer->GetGameMoney(),m_nMaxTakeIn);
				SendLogicMsgToOnePlayer(msgAM,pPlayer);
			}
		}
		rGPlayer.SetReqMoneyTime(m_CurTime);
	}
	else
	{
		if ( m_CurTime - rGPlayer.GetReqMoneyTime() >= T_ReqMoney )
		{			
			GameDeZhou_Flag msgFlag;
			msgFlag.m_Flag = msgFlag.TakeMoneyOutTime;
			SendLogicMsgToOnePlayer(msgFlag,rGPlayer.GetPID());

			PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(SitID);
			if ( pPlayer )
			{
				m_pTable->DebugInfo("SendReqMoney PID=%d SitID=%d PPID=%d",PID,SitID,pPlayer->GetPID());
				LogicStandUpPlayer( m_pTable->GetSitPlayerBySitID(SitID),SitID,Action_Flag_SysLogic );
			}			
		}
	}
}

void CDeZhouGameLogic::SendChangeBlind(int BigBlind,int SmallBlind,PlayerPtr pPlayer)
{
	m_pTable->DebugInfo("SendChangeBlind RoomID=%d TableID=%d",m_pRoom->GetRoomID(),m_pTable->GetTableID() );

	assert( IsMatchTable() && IsMatching() );
	if ( IsMatchTable() && IsMatching() )
	{
		GameDeZhou_MatchBlind msgMB;
		msgMB.m_BigBlind = BigBlind;
		msgMB.m_SmallBlind = SmallBlind;

		if( pPlayer )
		{
			SendLogicMsgToOnePlayer(msgMB,pPlayer);
		}
		else
		{
			SendLogicMsgToAllPlayer(msgMB);
		}		
	}	
}

void CDeZhouGameLogic::InitMatchData()
{
	for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit)
	{
		m_GP[Sit].Init();
	}
	StartGameLogic();
}

void CDeZhouGameLogic::SendPlayerPower(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CDeZhouGameLogic::SendPlayerPower");
	CLogFuncTime lft(s_LogicFuncTime,"SendPlayerPower");

	if ( IsAvailSitID(m_CurPlayerSitID) )
	{
		CGamePlayer& rCurGPlayer = m_GP[m_CurPlayerSitID-1];
		if ( rCurGPlayer.IsInGame() && !(rCurGPlayer.IsAllIn()) && rCurGPlayer.GetRealTableMoney()==0 )
		{
			m_pTable->DebugError("玩家游戏币为0 自动放弃 PSitID=%d SmallID=%d,BidID=%d GameState=%d",
				m_CurPlayerSitID,m_SmallBlindSitID,m_BigBlindSitID,m_GameState);

			GameXY::ClientToServerMessage CTS;
			GameDeZhou_ReqAddChip rac;

			rac.m_CurPlayerSitID = m_CurPlayerSitID;
			rac.m_nCurChipMoney = 0;
			rac.m_ChipAction = ACTION_GiveUp;
		
			MakeLogicCTS(rac,CTS);
			OnPlayerAddChip(m_pTable->GetTablePlayerByPID(rCurGPlayer.GetPID()),CTS);
		}
		else
		{
			INT64 nCurMaxChipOnTable = m_MaxChipOnTable;
			if ( m_pGSConfig->CeshiMode() )
			{
				INT64 CurMaxChip = GetCurMaxChipOnTable();
				if ( nCurMaxChipOnTable != CurMaxChip )
				{
					m_pTable->DebugError("nCurMaxChipOnTable=%s MaxChip=%s",N2S(nCurMaxChipOnTable).c_str(),N2S(CurMaxChip).c_str());
					DebugAllPlayer();
				}
			}

			if ( rCurGPlayer.IsBotPlayer() )
			{
				int MaxChipBlind = int(m_MaxChipOnTable/m_nBigBlind);
				int nBeiShu = 1;
				if ( MaxChipBlind>20 && MaxChipBlind<100 ) nBeiShu = 2;
				else if ( MaxChipBlind>=100) nBeiShu = 3;
				
				if ( CRandom::GetChangce(6,4-nBeiShu) )
				{
					m_DelayTime = Tool::CRandom::Random_Int(1,max(1,m_MaxBotChipTime/2));
				}
				else
				{
					m_DelayTime = Tool::CRandom::Random_Int(nBeiShu,max(nBeiShu,m_MaxBotChipTime));
				}

				if ( m_pTable->m_IsHaveCommonPlayer == false )
				{
					m_DelayTime = max(m_MaxBotChipTime,m_pGSConfig->m_cfBP.m_NoPlayerChipTime);
				}
			}
			else
			{
				m_DelayTime = m_WaitChipTime + 1;
				if ( rCurGPlayer.m_AutoOutTimes )
				{
					if ( rCurGPlayer.m_AutoOutTimes>=2)
					{
						m_DelayTime = m_WaitChipTime/4;
					}
					else
					{
						m_DelayTime = m_WaitChipTime/2;
					}
					m_DelayTime = max(m_DelayTime,10);
				}
			}

			GameDeZhou_PlayerTurn pt;
			pt.m_SitID = m_CurPlayerSitID;
			if ( rCurGPlayer.IsBotPlayer() ) pt.m_LeftTime = m_ActionTime+m_WaitChipTime-m_CurTime;
			else pt.m_LeftTime = m_ActionTime+m_DelayTime-m_CurTime;

			if ( pPlayer )
			{
				SendLogicMsgToOnePlayer(pt,pPlayer);
			}
			else
			{
				SendLogicMsgToAllPlayer(pt);
			}

			INT64 MaxLeft = 0;
			for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				if ( m_GP[Sit].IsPlaying() && (Sit+1)!=m_CurPlayerSitID )
				{
					if ( m_GP[Sit].GetTableMoney() > MaxLeft )
					{
						MaxLeft = m_GP[Sit].GetTableMoney();
					}
				}
			}
			MaxLeft = max(MaxLeft,nCurMaxChipOnTable);
			if ( MaxLeft == 0 )
			{
				m_pTable->DebugError("SendPlayerPower tablelid=%d cursitid=%d maxnum=%d MaxLeft=%s",
					m_pTable->GetTableID(),m_CurPlayerSitID,m_MaxSitPlayerNumber,Tool::N2S(MaxLeft).c_str() );
			}

			INT64 MaxCanChip = min(rCurGPlayer.GetTableMoney(),MaxLeft);
			if ( m_Limite == TABLE_LIMITE_TEXAS )
			{
				MaxCanChip = min(MaxCanChip,nCurMaxChipOnTable+m_MinAddChip);
				MaxCanChip = min(MaxCanChip,m_MinAddChip*4);
			}
			else if ( m_Limite == TABLE_LIMITE_POT )
			{
				INT64 PotMoney = 0;
				for ( int Idx=0;Idx<MAX_PALYER_ON_TABLE;Idx++)
				{
					PotMoney += m_TablePoolMoneys[Idx].m_nPoolMoney;
				}
				for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
				{
					if ( m_GP[Sit].IsAttendGame() )
					{
						PotMoney += m_GP[Sit].GetCurChip();
					}
				}
				PotMoney += (nCurMaxChipOnTable - rCurGPlayer.GetCurChip());
				MaxCanChip = min(MaxCanChip,PotMoney*2);
			}

			GameDeZhou_PlayerPower msgPP;
			msgPP.m_nMaxChip    = nCurMaxChipOnTable;
			//msgPP.m_nMaxChip    = max(nCurMaxChipOnTable,min(MaxCanChip,m_nBigBlind));
			msgPP.m_nMinAddChip = m_MinAddChip;
			msgPP.m_nTopChip    = MaxCanChip;
			SendLogicMsgToOnePlayer(msgPP,rCurGPlayer.GetPID());

			m_pTable->DebugInfo("SendPlayerPower PID=%d TablelID=%d CurSitID=%d m_nMaxChip=%s m_nMinAddChip=%s m_nTopChip=%s Delay=%d",
				rCurGPlayer.GetPID(),m_pTable->GetTableID(),m_CurPlayerSitID,N2S(nCurMaxChipOnTable).c_str(),
				N2S(m_MinAddChip).c_str(),N2S(msgPP.m_nTopChip.get()).c_str(),m_DelayTime );
		}
	}
	else
	{
		m_pTable->DebugError("SendPlayerPower tablelid=%d cursitid=%d maxnum=%d",m_pTable->GetTableID(),m_CurPlayerSitID,m_MaxSitPlayerNumber);
		this->DebugAllPlayer();
	}
}

void CDeZhouGameLogic::PackMoney()
{
	TraceStackPath logTP("CDeZhouGameLogic::PackMoney");
	CLogFuncTime lft(s_LogicFuncTime,"PackMoney");

	m_pTable->CeShiInfo("PackMoney Start m_GameState=%d",m_GameState);

	if ( m_GameState>=GAME_ST_HAND && m_GameState<GAME_ST_RIVER )
	{
		ChangeBotBlind();
	}

	INT64 minMoney,sumMoney;
	INT64 CurTurnMaxChip = 0;
	TablePoolMoney TempPoolMoney[MAX_PALYER_ON_TABLE];

	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		//m_GP[Sit].SetLastAction(ACTION_NONE);
		if ( m_GP[Sit].GetCurChip() > CurTurnMaxChip )
		{
			CurTurnMaxChip = m_GP[Sit].GetCurChip();
		}
	}
	m_bAllKanPai = (CurTurnMaxChip==0) ? true:false;

	int nChipPlayer = 0;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].GetCurChip() > 0 )
		{
			nChipPlayer++;			
		}

		//此处已经将玩家下注的游戏币从玩家游戏对象里面减掉,离开玩家不算
		if ( m_GP[Sit].GetCurChip() > 0 && m_GP[Sit].IsAttendGame() )
		{						
			m_GP[Sit].DecTableMoney(m_GP[Sit].GetCurChip());
		}
	}

	if ( nChipPlayer == 0 )
	{
		m_pGameLog->AddStepString("PackMoney Back");
		return ;
	}

	//当只有一个玩家的时候计算机玩家最大收益的下注
	//这里主要是考虑下大注玩家跑掉的情况，余下的玩家又赢不起，所以在计算的时候肯定要做筹码补偿下注，
	//但这个时候肯定是在游戏结束的时候，否则就是不正常	
	INT64 nLeftAddMoney = 0;
	int   nLeftSit = -1;
	if ( GetPlayingPlayerNumber() == 1 )
	{
		nLeftSit = -1;
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			if ( m_GP[Sit].IsPlaying() )
			{
				nLeftSit = Sit;
				break;
			}
		}
		if ( nLeftSit>=0 && nLeftSit<m_MaxSitPlayerNumber )
		{
			if ( m_GP[nLeftSit].GetCurChip() < CurTurnMaxChip && m_GP[nLeftSit].GetTableMoney() > 0 )
			{
				nLeftAddMoney = min(CurTurnMaxChip-m_GP[nLeftSit].GetCurChip(),m_GP[nLeftSit].GetTableMoney() );
				m_GP[nLeftSit].SetCurChip(m_GP[nLeftSit].GetCurChip() + nLeftAddMoney);
			}			
		}

		if ( nLeftAddMoney > 0 )
		{
			m_pTable->DebugInfo("自增: nLeftAddMoney = %s",Tool::N2S(nLeftAddMoney).c_str());
			m_pGameLog->AddStepString("PackMoney AddMoney SitID="+N2S(nLeftSit+1)+" nAddMoney="+N2S(nLeftAddMoney));
		}
	}

	if ( IsGamePlaying() )
	{
		int k = 0;
		do
		{
			sumMoney = 0;
			minMoney = 0;
			for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				if ( minMoney == 0 && m_GP[Sit].GetCurChip() > 0 )
				{
					minMoney = m_GP[Sit].GetCurChip();
				}
				if ( m_GP[Sit].GetCurChip() < minMoney && m_GP[Sit].GetCurChip() > 0 )
				{
					minMoney = m_GP[Sit].GetCurChip();
				}
			}
			if ( minMoney == 0 )
			{
				break ;
			}
			for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				if ( m_GP[Sit].GetCurChip() > 0 )
				{
					assert( m_GP[Sit].GetCurChip() >= minMoney );

					m_GP[Sit].SetCurChip(m_GP[Sit].GetCurChip()-minMoney);
					sumMoney += minMoney;
					TempPoolMoney[k].m_ChipSit[Sit] = 1;
				}
			}
			TempPoolMoney[k].m_nPoolMoney = sumMoney;
			k++;
		} while ( sumMoney > 0 );

		stMoneyPool StepMP;
		StepMP.m_Flag = 0;
		for(int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			StepMP.m_vecMoney.push_back(TempPoolMoney[PoolIdx].m_nPoolMoney);
		}
		m_pGameLog->AddStepString(StepMP.GetString());

		for ( int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++ )
		{
			if ( TempPoolMoney[PoolIdx].m_nPoolMoney > 0 )
			{
				int j=0;
				for (j=0;j<MAX_PALYER_ON_TABLE;j++)
				{
					if( DoTogether(this->m_TablePoolMoneys[j],TempPoolMoney[PoolIdx]) )
					{
						TempPoolMoney[PoolIdx].Init();
						break;
					}
				}
				if ( j >= MAX_PALYER_ON_TABLE )
				{
					m_pTable->DebugError("TableID=%d PackMoney 初步合并筹码",m_pTable->GetTableID());
				}
			}
		}

		StepMP.Init();
		StepMP.m_Flag = m_GameState;
		for(int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			StepMP.m_vecMoney.push_back(m_TablePoolMoneys[PoolIdx].m_nPoolMoney);
		}
		//m_pGameLog->AddStepString(StepMP.GetString());
		
		for (int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			if (m_TablePoolMoneys[PoolIdx].m_nPoolMoney > 0)
			{
				//在效堆中将弃牌和逃跑的玩家清理掉
				bool bHaveValiChip = false;
				for(int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
				{
					if ( m_TablePoolMoneys[PoolIdx].m_ChipSit[Sit] == 1 && m_GP[Sit].IsInGame() )
					{
						bHaveValiChip = true;
						break;
					}
				}
				if ( bHaveValiChip )
				{
					for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
					{
						if ( m_TablePoolMoneys[PoolIdx].m_ChipSit[Sit] == 1 )
						{
							if ( !m_GP[Sit].IsInGame() )
							{
								m_TablePoolMoneys[PoolIdx].m_ChipSit[Sit] = 0;
							}
						}
					}
				}
				else
				{
					//将逃走玩家的筹码系统回收，弃牌的玩家将回给玩家本身
					int nChipPlayer = m_TablePoolMoneys[PoolIdx].GetChipCount();
					INT64 nChipMoney = 0;
					if ( nChipPlayer > 0 )
					{
						nChipMoney = m_TablePoolMoneys[PoolIdx].m_nPoolMoney/nChipPlayer;
					}
					m_pTable->DebugInfo("Clear Pool PoolIdx=%d nChipPlayer=%d nChipMoney=%s",PoolIdx,nChipPlayer,Tool::N2S(nChipMoney).c_str() );

					if ( nChipPlayer > 0 && nChipMoney > 0 )
					{
						for(int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
						{
							if ( m_TablePoolMoneys[PoolIdx].m_ChipSit[Sit]==1 && (!m_GP[Sit].IsInGame()) )//逃跑弃牌的玩家回归系统
							{
								SysWinMoney(nChipMoney);
								m_TablePoolMoneys[PoolIdx].m_ChipSit[Sit] = 0;
								m_TablePoolMoneys[PoolIdx].m_nPoolMoney -= nChipMoney;

								m_pTable->DebugInfo("回收 PoolIdx=%d SitID=%d nMoney=%s",PoolIdx,Sit+1,Tool::N2S(nChipMoney).c_str() );
								m_pGameLog->AddStepString("SysBackMoney PoolIdx=" + N2S(PoolIdx)  + " SitID="+Tool::N2S(Sit+1)
									+" nMoney="+Tool::N2S(nChipMoney) );

								if ( m_TablePoolMoneys[PoolIdx].m_nPoolMoney < 0 )
								{
									m_TablePoolMoneys[PoolIdx].m_nPoolMoney = 0;

									m_pTable->DebugError("回收 TableID=%d m_nPoolMoney=%s",
										m_pTable->GetTableID(),Tool::N2S(m_TablePoolMoneys[PoolIdx].m_nPoolMoney).c_str() );
								}															
							}
						}
					}
					else
					{
						m_pTable->DebugError("系统回收 TableID=%d nPlayer=%d nChipMoney=%s",
							m_pTable->GetTableID(),nChipPlayer,Tool::N2S(nChipMoney).c_str() );
					}
				}
			}
		}

		StepMP.Init();
		StepMP.m_Flag = m_GameState;
		for(int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			StepMP.m_vecMoney.push_back(m_TablePoolMoneys[PoolIdx].m_nPoolMoney);
		}
		//m_pGameLog->AddStepString(StepMP.GetString());

		for ( int PoolIdx =0;PoolIdx<MAX_PALYER_ON_TABLE-1;PoolIdx++)
		{
			for ( int k=PoolIdx+1;k<MAX_PALYER_ON_TABLE;k++)
			{
				if( m_TablePoolMoneys[k].m_nPoolMoney>0 && DoTogether(m_TablePoolMoneys[PoolIdx],m_TablePoolMoneys[k]) )
				{
					m_TablePoolMoneys[k].Init();
					//m_pTable->DebugInfo("TableID=%d PackMoney 合并筹码 left=%d right=%d",m_pTable->GetTableID(),PoolIdx,k);
				}
			}
		}

		//如果有虚拟下注将虚拟下注清掉,虚注肯定是最后一堆中的
		if ( nLeftAddMoney > 0 )
		{
			bool bHaveReduce = false;
			for (int PoolIndex=MAX_PALYER_ON_TABLE-1;PoolIndex>=0;PoolIndex--)
			{
				if ( m_TablePoolMoneys[PoolIndex].m_nPoolMoney > 0 )
				{
					if ( m_TablePoolMoneys[PoolIndex].m_ChipSit[nLeftSit] == 1 )
					{
						m_pTable->DebugInfo("自减 leftaddmoney=%s,poolmoney=%s",Tool::N2S(nLeftAddMoney).c_str(),
							Tool::N2S(m_TablePoolMoneys[PoolIndex].m_nPoolMoney).c_str() );						

						bHaveReduce = true;
						m_TablePoolMoneys[PoolIndex].m_nPoolMoney -= nLeftAddMoney;

						m_pGameLog->AddStepString("PackMoney Reduce AddMoney="+Tool::N2S(nLeftAddMoney)
							+" PoolMoney="+Tool::N2S(m_TablePoolMoneys[PoolIndex].m_nPoolMoney) );

						if ( m_TablePoolMoneys[PoolIndex].m_nPoolMoney <= 0 )
						{
							m_pTable->DebugError("PackMoney TableID=%d m_GameState=%d leftaddmoney=%s,poolmoney=%s",
								m_pTable->GetTableID(),m_GameState,Tool::N2S(nLeftAddMoney).c_str(),
								Tool::N2S(m_TablePoolMoneys[PoolIndex].m_nPoolMoney).c_str() );
						}
						break;
					}
				}
			}

			if ( !bHaveReduce )
			{
				m_pTable->DebugError("自减 TableID=%d Failed",m_pTable->GetTableID());
			}			
		}
        
		for (int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			if ( m_TablePoolMoneys[PoolIdx].m_nPoolMoney > 0 && m_TablePoolMoneys[PoolIdx].GetChipCount()==0 )
			{
				m_pTable->DebugError("PackMoney() TableID=%d 第%d池出错",m_pTable->GetTableID(),PoolIdx+1);
			}			
		}

		StepMP.Init();
		StepMP.m_Flag = m_GameState;
		for(int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			StepMP.m_vecMoney.push_back(m_TablePoolMoneys[PoolIdx].m_nPoolMoney);
		}
		m_pGameLog->AddStepString(StepMP.GetString());

		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			m_GP[Sit].SetCurChip(0);
		}

		SendMoneyPool();
	}
	else
	{
		m_pTable->DebugError("PackMoney() state=%d",m_GameState);
	}

	m_pTable->CeShiInfo("PackMoney End PlayingPlayer=%d",GetPlayingPlayerNumber());
}

void CDeZhouGameLogic::OnFlop()
{
	TraceStackPath logTP("CDeZhouGameLogic::OnFlop");
	CLogFuncTime lft(s_LogicFuncTime,"OnFlop");
	m_pTable->CeShiInfo("OnFlop() Start TableId=%d CurPlayer=%d,EndPlayer=%d",m_pTable->GetTableID(),m_CurPlayerSitID,m_EndPlayerSitID );

	PackMoney();
	m_GameState = GAME_ST_FLOP;
	SendGameState();

	m_CurPlayerSitID = this->GetNextPlayingPlayerSitID(m_BankerSitID);
	m_EndPlayerSitID = this->GetPrePlayingPlayerSitID(m_CurPlayerSitID);
	assert( m_CurPlayerSitID && m_EndPlayerSitID && "OnFlop" );

	m_MaxChipOnTable = 0;
	m_MinAddChip = m_nBigBlind;

	SendTablePai(GAME_ST_FLOP);
	SendPlayerPower();

	//m_pTable->CeShiInfo("OnFlop() End");
}

void CDeZhouGameLogic::OnTurn()
{
	TraceStackPath logTP("CDeZhouGameLogic::OnTurn");
	CLogFuncTime lft(s_LogicFuncTime,"OnTurn");
	m_pTable->CeShiInfo("OnTurn() Start TableId=%d CurPlayer=%d,EndPlayer=%d",m_pTable->GetTableID(),m_CurPlayerSitID,m_EndPlayerSitID );

	PackMoney();
	m_GameState = GAME_ST_TURN;
	SendGameState();

	m_CurPlayerSitID = this->GetNextPlayingPlayerSitID(m_BankerSitID);
	m_EndPlayerSitID = this->GetPrePlayingPlayerSitID(m_CurPlayerSitID);
	assert( m_CurPlayerSitID && m_EndPlayerSitID && "OnTurn" );

	m_MaxChipOnTable = 0;
	m_MinAddChip = m_nBigBlind;

	if ( m_Limite == TABLE_LIMITE_TEXAS )
	{
		this->m_MinAddChip = m_nBigBlind*2;
	}

	SendTablePai(GAME_ST_TURN);
	SendPlayerPower();

	//m_pTable->CeShiInfo("OnTurn() End");
}

void CDeZhouGameLogic::OnRiver()
{
	TraceStackPath logTP("CDeZhouGameLogic::OnRiver");
	CLogFuncTime lft(s_LogicFuncTime,"OnRiver");
	m_pTable->CeShiInfo("OnRiver() Start TableId=%d CurPlayer=%d,EndPlayer=%d",m_pTable->GetTableID(),m_CurPlayerSitID,m_EndPlayerSitID );

	PackMoney();
	m_GameState = GAME_ST_RIVER;
	SendGameState();

	m_CurPlayerSitID = this->GetNextPlayingPlayerSitID(m_BankerSitID);
	m_EndPlayerSitID = this->GetPrePlayingPlayerSitID(m_CurPlayerSitID);
	assert( m_CurPlayerSitID && m_EndPlayerSitID && "OnRiver" );

	m_MaxChipOnTable = 0;
	m_MinAddChip = m_nBigBlind;

	if ( m_Limite == TABLE_LIMITE_TEXAS )
	{
		this->m_MinAddChip = m_nBigBlind*2;
	}

	SendTablePai(GAME_ST_RIVER);
	SendPlayerPower();

	//m_pTable->CeShiInfo("OnRiver() End");
}

void CDeZhouGameLogic::CheckPlayerJuBaoPeng(int SitID,int nPaiType,msgWinJuBaoPeng& msgWJBP)
{
	if ( IsAvailSitID(SitID) == false )
	{
		m_pTable->DebugError("CheckPlayerJuBaoPeng SitID=%d PaiType=%d",SitID,nPaiType);
		return ;
	}

	TraceStackPath logTP("CDeZhouGameLogic::CheckPlayerJuBaoPeng");
	CLogFuncTime lft(s_LogicFuncTime,"CheckPlayerJuBaoPeng");

	msgWJBP.ReSet();

	CGamePlayer& rGPlayer = m_GP[SitID-1];
	while( rGPlayer.m_JuBaoChip > N_JuBaoPeng::cs_JuBaoPengFlagMoney )
	{
		rGPlayer.m_JuBaoChip -= N_JuBaoPeng::cs_JuBaoPengFlagMoney;
	}

	INT64 nJuBaoChip = rGPlayer.m_JuBaoChip;
	int   nBeiShu    = DezhouLib::GetJuBaoPengBeiShu(nPaiType);
	INT64 nAddMoney  = nJuBaoChip * nBeiShu;

	PlayerPtr pTempPlayer = m_pTable->GetSitPlayerBySitID(SitID);
	if ( pTempPlayer && DezhouLib::IsValidJuBaoPaiType(nPaiType) && nJuBaoChip > 0 )
	{
		pTempPlayer->CheckJuBaoPeng(nPaiType);

		msgWJBP.m_PID             = rGPlayer.GetPID();
		msgWJBP.m_PaiType         = nPaiType;
		msgWJBP.m_nBeiShu         = nBeiShu;
		msgWJBP.m_nZhuChip        = nJuBaoChip;
		msgWJBP.m_WinMoney        = nAddMoney;

		rGPlayer.m_OtherLoss -= nAddMoney;
		rGPlayer.AddTableMoney(nAddMoney);
		pTempPlayer->AddGameMoney(nAddMoney);
		pTempPlayer->AddMoneyLog(nAddMoney,"CheckPlayerJuBaoPeng");

		DBServerXY::WDB_IncomeAndPay msgIP;
		msgIP.m_AID               = rGPlayer.GetAID();
		msgIP.m_PID               = rGPlayer.GetPID();
		msgIP.m_Flag              = N_IncomeAndPay::JuBaoPengAward;
		msgIP.m_nMoney            = nAddMoney;
		msgIP.m_strReMark         = N2S(nPaiType);
		LogicSendMsgToDBServer(msgIP);

		m_pServer->AddJuBaoPeng(-nAddMoney);
	}
}
void CDeZhouGameLogic::EndGame()
{
	TraceStackPath logTP("CDeZhouGameLogic::EndGame");
	CLogFuncTime lft(s_LogicFuncTime,"EndGame");

	m_pTable->CeShiInfo("CDeZhouGameLogic::EndGame() Start");
	PackMoney();

	int EndPlayer = 0;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		m_GP[Sit].SetLastAction(ACTION_NONE);
		if ( m_GP[Sit].IsInGame() )
		{
			EndPlayer++;
			m_CanShowPaiSitID = Sit+1;
		}
	}

	BYTE TempGameState = m_GameState;
	if ( m_GameState != GAME_ST_RIVER && EndPlayer > 1 )
	{
		SendTablePai(GAME_ST_RIVER);
	}

	m_MaxChipOnTable = 0;
	m_GameState = GAME_ST_DIVIDEMONEY;
	SendGameState();

	GameDeZhou_Result msgResult;
	GameDeZhou_WinJuBaoPeng msgWJBPList;
	msgWinJuBaoPeng msgWJBP;
	if ( EndPlayer >= 1 )
	{
		DezhouLib::CPaiType AllPlayerPaiType[MAX_PALYER_ON_TABLE];
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			CGamePlayer& rCurGPlayer = m_GP[Sit];
			if ( rCurGPlayer.IsInGame() )
			{
				AllPlayerPaiType[Sit] = m_GamePaiLogic.GetPlayerPaiType(Sit);
				assert( AllPlayerPaiType[Sit] == rCurGPlayer.m_PaiType );
				assert( AllPlayerPaiType[Sit].m_Type>0 && AllPlayerPaiType[Sit].m_PaiTypeValue>0 && AllPlayerPaiType[Sit].m_PaiVectorValue>0 );

				if ( EndPlayer > 1 )
				{
					rCurGPlayer.m_PaiType = AllPlayerPaiType[Sit];
				}
				else if ( EndPlayer == 1 )
				{
					if ( TempGameState>=GAME_ST_FLOP && TempGameState<=GAME_ST_RIVER )
					{
						BYTE PaiArray[SELECT_CARDS];
						memset(PaiArray,0,SELECT_CARDS);
						PaiArray[0] = rCurGPlayer.m_HandPai.m_Pai[0];
						PaiArray[1] = rCurGPlayer.m_HandPai.m_Pai[1];

						int nPai = 0;
						if ( TempGameState == GAME_ST_FLOP )
							nPai = 3;
						else if (  TempGameState == GAME_ST_TURN )
							nPai = 4;
						else if ( TempGameState == GAME_ST_RIVER )
							nPai = 5;
						assert(nPai>=3 && nPai<=5);
						for ( int nPos=0;nPos<nPai;nPos++)
						{
							PaiArray[nPos+2] = m_GamePaiLogic.GetTablePai(nPos);
						}
						rCurGPlayer.m_PaiType = DezhouLib::GetMaxPaiType(PaiArray);
					}
					else
					{
						rCurGPlayer.m_PaiType = DezhouLib::CPaiType();
					}
				}

				DezhouLib::CPaiType TempPaiType = rCurGPlayer.m_PaiType;
				int nPaiType = TempPaiType.m_Type;
				if ( DezhouLib::IsValidPaiType(nPaiType) )
				{
					PlayerPtr TempPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
					if ( TempPlayer )
					{
						TempPlayer->CheckMaxPai(TempPaiType);
					}
					if ( rCurGPlayer.m_JuBaoChip > N_JuBaoPeng::cs_JuBaoPengFlagMoney )
					{
						CheckPlayerJuBaoPeng(Sit+1,nPaiType,msgWJBP);
						if ( msgWJBP.m_PID>0 && msgWJBP.m_WinMoney>0 )
						{
							msgWJBPList.m_listWin.push_back(msgWJBP);
						}
					}
				}
			}
		}
		
		for (int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			if ( m_TablePoolMoneys[PoolIdx].m_nPoolMoney > 0 )
			{
				vector<BYTE> vMaxSeat;
				DezhouLib::CPaiType MaxPaiType;			

				for(int j=0;j<m_MaxSitPlayerNumber;j++)
				{
					if( m_TablePoolMoneys[PoolIdx].m_ChipSit[j] == 1 && (!AllPlayerPaiType[j].IsEmpty()) )
					{
						if ( AllPlayerPaiType[j] == MaxPaiType )
						{
							vMaxSeat.push_back(j);
						}
						else if ( AllPlayerPaiType[j] > MaxPaiType )
						{
							vMaxSeat.clear();
							MaxPaiType = AllPlayerPaiType[j];
							vMaxSeat.push_back(j);
						}
					}
				}

				if ( vMaxSeat.size() == 0 )//如果没有玩家赢钱就找出下注的玩家均放进去
				{
					m_pTable->DebugInfo("有下好注逃跑的最大的玩家 poolidx=%d",PoolIdx);
					for (int j=0;j<m_MaxSitPlayerNumber;j++)
					{
						if ( m_TablePoolMoneys[PoolIdx].m_ChipSit[j] == 1 )
						{
							vMaxSeat.push_back(j);
						}
					}
				}
				for (int j=0;j<int(vMaxSeat.size());j++)
				{
					m_TablePoolMoneys[PoolIdx].m_WinSitID[j] = vMaxSeat[j]+1;
				}
			}
		}

		if ( EndPlayer > 1 )
		{
			BYTE ShowPaiSit[MAX_PALYER_ON_TABLE];
			memset(ShowPaiSit,0,sizeof(BYTE)*MAX_PALYER_ON_TABLE);

			//哪些玩家赢的标志，凡是在奖池里面拿过钱的玩家均算赢，
			for (int PoolIdx = 0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
			{
				if ( m_TablePoolMoneys[PoolIdx].m_nPoolMoney > 0 )
				{
					for (int j=0;j<m_MaxSitPlayerNumber;j++)
					{
						int nWinSitID = m_TablePoolMoneys[PoolIdx].m_WinSitID[j];
						if ( nWinSitID>0  )
						{						
							if ( (!AllPlayerPaiType[nWinSitID-1].IsEmpty())	&& m_TablePoolMoneys[PoolIdx].GetChipCount()>=1 )
							{
								msgResult.m_PlayerHandPai[nWinSitID-1].m_bWin = 1;
								ShowPaiSit[nWinSitID-1] = 1;
							}
							else
							{
								m_pTable->DebugError("MoneyPool=%d WinSitID=%d Count=%d",PoolIdx,nWinSitID,m_TablePoolMoneys[PoolIdx].GetChipCount() );
							}						
						}
					}
				}				
			}
			//所有ALLIN玩家以及以AllKanPai结束的均可看见
			for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				if ( m_GP[Sit].IsInGame() )
				{
					if ( m_bAllKanPai || m_GP[Sit].IsAllIn() ) 
					{
						ShowPaiSit[Sit] = 1;
					}						
				}
			}
			for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				if ( ShowPaiSit[Sit] == 1 )
				{
					msgResultPai& rRP = msgResult.m_PlayerHandPai[Sit];
					rRP.m_SitID       = Sit + 1;
					rRP.m_HandPai[0]  = m_GP[Sit].m_HandPai.m_Pai[0];
					rRP.m_HandPai[1]  = m_GP[Sit].m_HandPai.m_Pai[1];
					rRP.m_PaiType[0]  = AllPlayerPaiType[Sit].m_Type;
					for (int i=0;i<5;i++)
					{
						rRP.m_PaiType[i+1] = AllPlayerPaiType[Sit].m_Pai[i];
					}					
				}
			}
		}

		for (int PoolIdx=0;PoolIdx<MAX_PALYER_ON_TABLE;PoolIdx++)
		{
			int     nWinCount = 0;
			INT64   nLeft     = 0;
			INT64   nWinMoney = 0;

			if ( m_TablePoolMoneys[PoolIdx].m_nPoolMoney > 0 )
			{
				msgResult.m_MoneyPools[PoolIdx].m_nMoney = m_TablePoolMoneys[PoolIdx].m_nPoolMoney;				
				nWinCount = m_TablePoolMoneys[PoolIdx].GetWinCount();
				if (nWinCount > 0)
				{
					if (nWinCount > 1)
					{
						nLeft = m_TablePoolMoneys[PoolIdx].m_nPoolMoney%nWinCount;
					}
					nWinMoney = m_TablePoolMoneys[PoolIdx].m_nPoolMoney/nWinCount;
				}
				else
				{					
					m_pTable->DebugError("算分错误 tid=%d poolidx=%d",m_pTable->GetTableID(),PoolIdx);
				}

				for (int j=0;j<nWinCount;j++)
				{
					int nSeat = m_TablePoolMoneys[PoolIdx].m_WinSitID[j]-1;

					msgPlayerWinMoney pwm;
					if (nSeat>=0 && nSeat<m_MaxSitPlayerNumber)
					{
						m_GP[nSeat].AddTableMoney(nWinMoney);
						pwm.SetPlayerWinMoney(m_TablePoolMoneys[PoolIdx].m_WinSitID[j],nWinMoney);
						if ( j == 0 )
						{
							pwm.m_nWinMoney += nLeft;
							m_GP[nSeat].AddTableMoney(nLeft);							
						}
						msgResult.m_MoneyPools[PoolIdx].m_PlayersInPool[j] = pwm;

						if ( !IsAvailSitID(pwm.m_SitID) )
						{
							m_pTable->DebugError("WinSitID=%d",pwm.m_SitID);
						}
					}
					else
					{
						m_pTable->DebugError("算分错误 tid=%d nSeat=%d",m_pTable->GetTableID(),nSeat);
					}				
				}
			}
		}
	}
	else
	{
		//assert(EndPlayer);
		m_pTable->DebugError("结束人数不对 endplayer=%d",EndPlayer);
	}

	if ( IsCommondTable() )
	{
		for ( int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			if ( m_GP[Sit].m_JuBaoCount*m_GP[Sit].m_JuBaoChip==0 && m_GP[Sit].m_JuBaoCount+m_GP[Sit].m_JuBaoChip>0 )
			{
				m_GP[Sit].m_JuBaoCount = m_GP[Sit].m_JuBaoChip = 0;

				GameDeZhou_JoinJuBaoPengInfo msgInfo;
				msgInfo.m_SitID         = Sit+1;
				msgInfo.m_Flag          = msgInfo.ExitJuBaoPeng;
				SendLogicMsgToAllPlayer(msgInfo);
			}
		}
	}

	if ( msgWJBPList.m_listWin.size() )
	{
		SendLogicMsgToAllPlayer(msgWJBPList);
	}

	msgResult.TidyUp();
	SendLogicMsgToAllPlayer(msgResult);

	if ( EndPlayer == 1 )
	{
		if ( IsAvailSitID(m_CanShowPaiSitID) )
		{
			m_pTable->DebugInfo("ShowPaiSitID=%d",m_CanShowPaiSitID);

			GameDeZhou_Flag msgFlag;
			msgFlag.m_Flag = msgFlag.ShowPai;
			SendLogicMsgToOnePlayer(msgFlag,m_GP[m_CanShowPaiSitID-1].GetPID());
		}
	}

	int nIsPlayerWin[MAX_PALYER_ON_TABLE];
	memset(nIsPlayerWin,0,MAX_PALYER_ON_TABLE*sizeof(int));

	int nPlayerWinType[MAX_PALYER_ON_TABLE];
	memset(nPlayerWinType,0,MAX_PALYER_ON_TABLE*sizeof(int));

	if ( IsMatchTable() )
	{
		m_pTable->DebugInfo("MatchTable......");
		XieYiList<DBServerXY::WDB_MatchWinLoss,9> wdbmsgMWL;
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			if ( m_GP[Sit].IsAttendGame() )
			{
				BYTE nPaiType = m_GP[Sit].m_PaiType.GetType();

				DBServerXY::WDB_MatchWinLoss msgMWL;
				msgMWL.m_GameGUID             = m_GameLogicGUID;
				msgMWL.m_MatchGUID            = m_pMatchRule->m_MatchGUID;
				msgMWL.m_AID                  = m_GP[Sit].GetAID();
				msgMWL.m_PID                  = m_GP[Sit].GetPID();

				msgMWL.m_RoomID               = m_pTable->GetRoomID();
				msgMWL.m_TableID              = m_pTable->GetTableID();
				msgMWL.m_SitID                = Sit+1;
				msgMWL.m_nWinLossMoney        = m_GP[Sit].GetWinLossMoney();
				msgMWL.m_PaiType              = nPaiType;
				if ( m_GP[Sit].IsInGame() )
				{
					if ( m_GP[Sit].GetWinLossMoney() > 0 )
						msgMWL.m_EndGameFlag = TALLY_WIN;
					else
						msgMWL.m_EndGameFlag = TALLY_LOSS;
				}
				else
				{
					msgMWL.m_EndGameFlag = TALLY_GIVEUP;
				}
				wdbmsgMWL.m_XieYiList.push_back(msgMWL);

				PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
				if ( pPlayer )
				{
					if ( m_GP[Sit].GetWinLossMoney()> 0 )
					{
						nIsPlayerWin[Sit] = 1;
						nPlayerWinType[Sit] = nPaiType;
						pPlayer->m_nWinTimes++;
						if ( DezhouLib::IsValidPaiType(nPaiType) )
						{
							pPlayer->m_WinRecord[nPaiType-1]++;
						}
					}
					else
					{
						pPlayer->m_nLossTimes++;
					}
					pPlayer->UpdateGameInfo();
				}
			}
		}
		LogicSendMsgToDBServer(wdbmsgMWL);

		//淘汰玩家，同一局被淘汰的玩家输钱多的名次靠前
		vector<int> vecWashOut;
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			if ( m_GP[Sit].IsAttendGame() )
			{
				if ( m_GP[Sit].GetTableMoney() < m_nMinLeftForGame )
				{
					vecWashOut.push_back(Sit);
				}
				if ( IsRoomMatch() )
				{
					m_pRoom->AddPlayerMatchTableMoney(m_GP[Sit].GetPID(),int(m_GP[Sit].GetWinLossMoney()));
				}
			}
		}
		if ( vecWashOut.size() > 1 )
		{
			for ( int i=0;i<int(vecWashOut.size())-1;++i)
			{
				for(int j=i+1;j<int(vecWashOut.size());++j)
				{
					if ( m_GP[vecWashOut[i]].GetWinLossMoney() < m_GP[vecWashOut[j]].GetWinLossMoney() )
					{
						swap(vecWashOut[i],vecWashOut[j]);
					}
				}
			}

			DeZhouLogic::DeZhouLogic_WashOutPlayer msgWOP;
			msgWOP.m_listSit = vecWashOut;
			PushMsgInList(msgWOP);
		}
	}
	else
	{
		m_pTable->DebugInfo("CommonTable......");
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			if ( m_GP[Sit].IsAttendGame() )
			{
				if ( m_pGSConfig->CeshiMode() )
				{
					m_pTable->DebugInfo("EndGame SitID=%d WinLossMoney=%s TableMoney=%s CurChip=%s OtherLoss=%s OriginMoney=%s",
						Sit+1,N2S(m_GP[Sit].GetWinLossMoney()).c_str(),N2S(m_GP[Sit].GetTableMoney()).c_str(),N2S(m_GP[Sit].GetCurChip()).c_str(),
						N2S(m_GP[Sit].m_OtherLoss).c_str(),N2S(m_GP[Sit].GetOriginMoney()).c_str() );
				}				

				BYTE nPaiType = m_GP[Sit].m_PaiType.GetType();
				m_Tallys[Sit].m_AID           = m_GP[Sit].GetAID();
				m_Tallys[Sit].m_PID           = m_GP[Sit].GetPID();			
				m_Tallys[Sit].m_nWinLossMoney = m_GP[Sit].GetWinLossMoney();
				m_Tallys[Sit].m_LeftPai       = m_GP[Sit].m_HandPai.m_Pai[0];
				m_Tallys[Sit].m_RightPai      = m_GP[Sit].m_HandPai.m_Pai[1];
				m_Tallys[Sit].m_PaiType       = nPaiType;
				if ( m_GP[Sit].IsInGame() )
				{
					if ( m_Tallys[Sit].m_nWinLossMoney > 0 )
						m_Tallys[Sit].m_EndGameFlag = TALLY_WIN;			
					else
						m_Tallys[Sit].m_EndGameFlag = TALLY_LOSS;
				}
				else
				{
					m_Tallys[Sit].m_EndGameFlag = TALLY_GIVEUP;
				}

				if ( m_Tallys[Sit].m_EndGameFlag == TALLY_WIN )
				{
					assert( m_Tallys[Sit].m_nWinLossMoney > 0 );
					//玩家最大赢钱数
					INT64 nWinMoney = m_Tallys[Sit].m_nWinLossMoney;
					PlayerPtr TempPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
					if ( TempPlayer )
					{
						TempPlayer->CheckMaxWinMoney(nWinMoney);
						TempPlayer->CheckWinMoney(nWinMoney);
						TempPlayer->CheckOwnMoneyHonor();

						if ( m_GP[Sit].IsBotPlayer() )
						{
							bool bShowFace = false;
							if ( nWinMoney > INT64(m_nBigBlind)*100 )
							{
								bShowFace = Tool::CRandom::GetChangce(5,3);
							}
							else if ( nWinMoney > INT64(m_nBigBlind)*50 )
							{
								bShowFace = Tool::CRandom::GetChangce(3,1);
							}
							else if ( nWinMoney > INT64(m_nBigBlind)*20 )
							{
								bShowFace = Tool::CRandom::GetChangce(10,1);
							}
							if ( bShowFace )
							{
								int Idx = Tool::CRandom::Random_Int(0,s_WinFaceID.size()-1);
								int FaceID = s_WinFaceID[Idx];

								DeZhouLogic::DeZhouLogic_ShowFace msgSF;
								msgSF.m_SitID       = Sit + 1;
								msgSF.m_PID         = TempPlayer->GetPID();
								msgSF.m_FaceID      = FaceID;
								msgSF.m_ActionTime  = m_CurTime + Tool::CRandom::Random_Int(5,10);
								PushMsgInList(msgSF);
							}
						}						
					}					
				}
				else
				{
					if ( m_GP[Sit].IsBotPlayer() )
					{
						INT64 nWinMoney = m_Tallys[Sit].m_nWinLossMoney;
						nWinMoney = max(nWinMoney,-nWinMoney);

						PlayerPtr TempPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
						if ( TempPlayer )
						{
							bool bShowFace = false;
							if ( nWinMoney > INT64(m_nBigBlind)*100 )
							{
								bShowFace = Tool::CRandom::GetChangce(5,3);
							}
							else if ( nWinMoney > INT64(m_nBigBlind)*50 )
							{
								bShowFace = Tool::CRandom::GetChangce(3,1);
							}
							else if ( nWinMoney > INT64(m_nBigBlind)*20 )
							{
								bShowFace = Tool::CRandom::GetChangce(10,1);
							}
							if ( bShowFace )
							{
								int Idx = Tool::CRandom::Random_Int(0,s_LossFaceID.size()-1);
								int FaceID = s_LossFaceID[Idx];

								DeZhouLogic::DeZhouLogic_ShowFace msgSF;
								msgSF.m_SitID       = Sit + 1;
								msgSF.m_PID         = TempPlayer->GetPID();
								msgSF.m_FaceID      = FaceID;
								msgSF.m_ActionTime  = m_CurTime + Tool::CRandom::Random_Int(5,10);
								PushMsgInList(msgSF);
							}
						}
					}					
				}

				m_Tallys[Sit].m_Money    = m_Tallys[Sit].m_nWinLossMoney - m_Tallys[Sit].m_nServiceMoney;
				m_Tallys[Sit].m_JF       = 0;
				m_Tallys[Sit].m_EP       = 0;

				if ( m_GP[Sit].IsBotPlayer() )
				{
					m_BotRealWinLoss += m_GP[Sit].GetWinLossMoney();
				}

				PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
				if ( pPlayer )
				{
					pPlayer->AddGameMoney(m_Tallys[Sit].m_Money);
					pPlayer->AddMoneyLog(m_Tallys[Sit].m_Money,"WinMoney");
					pPlayer->m_nEP       += m_Tallys[Sit].m_EP;
					pPlayer->m_nJF       += m_Tallys[Sit].m_JF;
					if ( m_Tallys[Sit].m_nWinLossMoney > 0 )
					{
						pPlayer->CheckMaxGameMoney();

						nIsPlayerWin[Sit] = 1;
						nPlayerWinType[Sit] = nPaiType;
						pPlayer->m_nWinTimes++;
						if ( DezhouLib::IsValidPaiType(nPaiType) )
						{
							pPlayer->m_WinRecord[nPaiType-1]++;
						}
					}
					else
					{
						pPlayer->m_nLossTimes++;
					}

					pPlayer->m_nUpperLimite += m_Tallys[Sit].m_nServiceMoney*10;
					pPlayer->m_nLowerLimite += max(1,m_Tallys[Sit].m_nServiceMoney/10);

					//领取低保补助
					if ( pPlayer->IsNotBotPlayer() && pPlayer->GetGameMoney()<1000 && pPlayer->m_JiuMingTimes<5 )
					{
						pPlayer->m_JiuMingTimes++;

						pPlayer->AddGameMoney(2000);
						m_GP[Sit].AddTableMoney(2000);

						SendGameMoneyBySitID(Sit+1);
						SendTableMoneyBySitID(Sit+1);

						DBServerXY::WDB_IncomeAndPay msgIP;
						msgIP.m_AID        = pPlayer->GetAID();
						msgIP.m_PID        = pPlayer->GetPID();
						msgIP.m_nMoney     = 2000;
						msgIP.m_Flag       = N_IncomeAndPay::JiuMingQian;
						LogicSendMsgToDBServer(msgIP);
					}
				}
				else
				{
					m_pTable->DebugError("EndGame PID=%d SitID=%d",m_GP[Sit].GetPID(),Sit+1);
				}

				m_pGameLog->AddStepString( "SitID="+N2S(Sit+1) + " OrigenMoney=" + N2S(m_GP[Sit].GetOriginMoney(),10)
					+ " LeftMoney=" + N2S(m_GP[Sit].GetTableMoney(),10)
					+ " LossMoney=" + N2S(m_GP[Sit].GetWinLossMoney(),10)
					+ " OtherLoss=" + N2S(m_GP[Sit].m_OtherLoss,10) );

				if ( m_GP[Sit].GetOriginMoney() != m_GP[Sit].GetTableMoney()+m_GP[Sit].m_OtherLoss-m_GP[Sit].GetWinLossMoney() )
				{
					m_pTable->DebugError("玩家平衡出错 SitID",Sit+1);
				}

				if ( m_pGSConfig->CeshiMode() )
				{
					m_pTable->DebugInfo("SitID=%d PID=%-10d Service=%d WinLoss=%-10s Origin=%-10s TableM=%-10s Chip=%-10s",
						Sit+1,m_GP[Sit].GetPID(),m_Tallys[Sit].m_nServiceMoney,
						Tool::N2S(m_Tallys[Sit].m_nWinLossMoney).c_str(),Tool::N2S(m_GP[Sit].GetOriginMoney()).c_str(),
						Tool::N2S(m_GP[Sit].GetTableMoney()).c_str(),Tool::N2S(m_GP[Sit].GetCurChip()).c_str() );
				}				
			}
		}

		WriteToDB(m_Tallys,m_MaxSitPlayerNumber);
		m_pTable->reportEndGame();

		m_pTable->CeShiInfo("EndGame WinLossMoney=%s LossMoney=%s",Tool::N2S(m_nGameWinLossMoney).c_str(),Tool::N2S(m_nGameLossMoney).c_str());

		if ( m_nGameWinLossMoney != 0 )
		{
			m_pTable->DebugError("EndGame 记分错误");
			DebugAllPlayer();
		}
	}

	//对玩家的成就进行判断，因为即要判断比赛也要判断普通场，以此在前面的循环外面，
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsAttendGame() )
		{
			PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
			if ( pPlayer )
			{
				pPlayer->CheckPlayTimes();
				pPlayer->CheckPlayAndInviteAward();
				if ( nIsPlayerWin[Sit] == 1 )
				{
					m_GP[Sit].m_nContinuWin++;
					pPlayer->CheckContinuWin(m_GP[Sit].m_nContinuWin);
					
					if ( m_GP[Sit].GetAllIn() )
					{
						pPlayer->FinishHonor(N_Honor::First_AllInWin1);
					}
					pPlayer->CheckWinTimes();
					if ( DezhouLib::IsValidPaiType(nPlayerWinType[Sit]) )
					{
						pPlayer->CheckWinRecord(nPlayerWinType[Sit]);
					}
					if ( EndPlayer > 1 )
					{
						stHandPai& rTempHP = m_GP[Sit].m_HandPai;
						if ( DezhouLib::GetPaiValue(rTempHP.m_Pai[0])==14 && DezhouLib::GetPaiValue(rTempHP.m_Pai[1])==14 )
						{
							pPlayer->FinishHonor(N_Honor::First_AAWin);
						}
						else if ( (DezhouLib::GetPaiValue(rTempHP.m_Pai[0])==7 && DezhouLib::GetPaiValue(rTempHP.m_Pai[1])==2)
							|| (DezhouLib::GetPaiValue(rTempHP.m_Pai[0])==2 && DezhouLib::GetPaiValue(rTempHP.m_Pai[1])==7) )
						{
							pPlayer->FinishHonor(N_Honor::First_27Win);
						}
					}
				}
				else
				{
					m_GP[Sit].m_nContinuWin = 0;
				}
			}
		}
	}

	m_ActionTime = m_CurTime;
	m_DelayTime = msgResult.m_MoneyPoolCount*4 + 1;

	m_pGameLog->AddStepString("游戏结束......");

	if ( m_pTable->m_bTableGameError )
	{
		DebugAllPlayer();
	}

	//m_pTable->CeShiInfo("CDeZhouGameLogic::EndGame() End");
}
void CDeZhouGameLogic::AddBotJuBaoPeng()
{
	if ( !IsMatchTable() )
	{
		CLogFuncTime lft(s_LogicFuncTime,"AddBotJuBaoPeng");
		int nCount = 0;
		for ( int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
		{
			if ( m_GP[Sit].GetJuBaoPeng() )
			{
				nCount++;
			}
		}
		if ( nCount == 0 )
		{
			for ( int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
			{
				if ( m_GP[Sit].IsBotPlayer() && Tool::CRandom::GetChangce(3,1) )
				{
					PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
					if ( pPlayer )
					{
						GameXY::ClientToServerMessage CTS;
						GameDeZhou_ReqJoinJuBaoPeng msgJJBP;

						msgJJBP.m_SitID      = Sit+1;
						msgJJBP.m_nCount     = 10000;
						msgJJBP.m_nZhuChip   = max(m_nBigBlind/5,1);

						MakeLogicCTS(msgJJBP,CTS);
						OnJoinJuBaoPeng(pPlayer,CTS);
					}
				}
				
			}
		}
	}
}
void CDeZhouGameLogic::OnDivideMoneyEnd()
{
	TraceStackPath logTP("CDeZhouGameLogic::OnDivideMoneyEnd");
	CLogFuncTime lft(s_LogicFuncTime,"OnDivideMoneyEnd");
	m_pTable->CeShiInfo("CDeZhouGameLogic::OnDivideMoneyEnd() Start");
	m_pGameLog->AddStepString("OnDivideMoneyEnd()");

	m_pTable->m_bTableGameError = false;

	m_GameState = GAME_ST_END;
	SendGameState();

	DoJuBaoPengMsg();
	DoAddMoney();

	m_ActionTime = m_CurTime;
	m_StartGameTime = m_CurTime + m_WaitStartTime;

	//玩家管理
	int BotPlayerNumber=0,CommPlayerNumber=0;
	int CurSitNumber = m_pTable->GetSitPlayerNumber();
	for ( int Sit = 0;Sit<m_MaxSitPlayerNumber;Sit++)
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

	if ( m_TableBotFlag == Table_Bot_Flow )
	{
		if ( BotPlayerNumber==CurSitNumber && BotPlayerNumber>0 && CRandom::GetChangce(5,1) )
		{
			for ( int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
			{
				if (m_GP[Sit].IsHavePlayer() && m_GP[Sit].IsBotPlayer() )
				{
					m_pTable->CeShiInfo("流动桌只有机器玩家踢走机器人 SitID=%d PID=%d",Sit+1,m_GP[Sit].GetPID() );
					m_pTable->RemoveBotPlayer( m_GP[Sit].GetPID() );
					break;
				}
			}
		}
	}

	TestSitPlayer();
	
	for ( int Sit = 0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() )
		{
			PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);

			assert( pPlayer );
			assert( pPlayer->GetPID() == m_GP[Sit].GetPID() );

			if ( m_GP[Sit].IsBotPlayer() )
			{
				if ( m_GP[Sit].GetTableMoney() < m_nMinLeftForGame )
				{
					m_pTable->RemoveBotPlayer(m_GP[Sit].GetPID());
				}
			}
		}
	}

	if ( !IsMatchTable() )
	{
		for ( int Sit = 0;Sit<m_MaxSitPlayerNumber;Sit++)
		{
			if ( m_GP[Sit].IsHavePlayer() && m_GP[Sit].IsBotPlayer() )
			{				
				PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);

				assert( pPlayer );
				assert( pPlayer->GetPID() == m_GP[Sit].GetPID() );
				if( (pPlayer->m_EndBotPlayerTime>0 && m_CurTime >= pPlayer->m_EndBotPlayerTime) )
				{
					m_pTable->DebugInfo("机器人玩家游戏时间已到，SitID=%d PID=%d",Sit+1,pPlayer->GetPID() );
					m_pTable->RemoveBotPlayer(m_GP[Sit].GetPID());
					break;
				}
			}
		}
	}

	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if (m_GP[Sit].IsHavePlayer())
		{
			PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(Sit+1);
			if ( pPlayer )
			{
				pPlayer->UpdateGameInfo();
			}
			if ( m_GP[Sit].IsAttendGame() )
			{
				SendTableMoneyBySitID(Sit+1);
			}
		}
	}

	//更新输赢玩家的值
	if ( m_vectorWinLossPID.size() )
	{
		stTablePlayerWinLoss stPWL;
		GameDeZhou_TableWinLoss msgTWL;
		for (size_t nPos=0;nPos<m_vectorWinLossPID.size();++nPos)
		{
			MapTablePlayerWinLoss::iterator itorPI = m_mapTableWinLoss.find(m_vectorWinLossPID[nPos]);
			if ( itorPI != m_mapTableWinLoss.end() )
			{
				stPWL.m_PID           = itorPI->first;
				stPWL.m_WinLossMoney  = itorPI->second;
				msgTWL.m_vectorWinLoss.push_back(stPWL);
			}
		}
		if ( msgTWL.m_vectorWinLoss.size() )
		{
			SendLogicMsgToAllPlayer(msgTWL);
		}
		m_vectorWinLossPID.clear();
	}

	//更新聚宝盆的值
	if ( IsCommondTable() )
	{
		JuBaoPengMoney msgJBPM;
		msgJBPM.m_JuBaoPengMoney = m_pServer->GetJuBaoPeng();
		m_pTable->SendMsgToAllPlayerInTable(msgJBPM);

		AddBotJuBaoPeng();
	}	

	if ( m_bEndTable )
	{
		m_pTable->reportEndTable();
	}

	//m_pTable->CeShiInfo("CDeZhouGameLogic::OnDivideMoneyEnd() End");
}

void CDeZhouGameLogic::EndTableMatch(PlayerPtr pWinPlayer,int SitID)
{
	TraceStackPath logTP("CDeZhouGameLogic::EndTableMatch");
	CLogFuncTime lft(s_LogicFuncTime,"EndTableMatch");
	m_pTable->DebugInfo("EndTableMatch Start......WinSitID=%d AID=%d PID=%d",SitID,pWinPlayer->GetAID(),pWinPlayer->GetPID());

	TestGPPlayer(pWinPlayer,SitID);

	assert(pWinPlayer);
	assert(SitID>0);
	assert(IsAvailSitID(SitID));

	EndMatchPlayer(pWinPlayer,SitID);
	m_pTable->SetTableMatchState(MatchTable_ST_MatchOver);
	m_pServer->ChangeMatchTableState(m_pTable);

	m_pTable->SetClearTableTime(m_CurTime+N_Time::Minute_1);

	if ( m_pMatchRule->m_TotalMatchMoney != m_pMatchRule->m_TotalLossMoney+m_GP[SitID-1].GetTableMoney() )
	{
		m_pTable->DebugError("EndTableMatch GUID=%s MatchMoney=%s LossMoney=%s TableMoney=%s",
			N2S(m_pMatchRule->m_MatchGUID).c_str(),N2S(m_pMatchRule->m_TotalMatchMoney).c_str(),
			N2S(m_pMatchRule->m_TotalLossMoney).c_str(),N2S(m_GP[SitID-1].GetTableMoney()).c_str() );
	}

	m_pMatchRule->ResetMatchData();

	m_pTable->DebugInfo("EndTableMatch AID=%d PID=%d",pWinPlayer->GetAID(),pWinPlayer->GetPID() );
}

void CDeZhouGameLogic::EndRoomMatch(PlayerPtr pPlayer,int SitID)
{
	TraceStackPath logTP("CDeZhouGameLogic::EndRoomMatch");
	CLogFuncTime lft(s_LogicFuncTime,"EndRoomMatch");
	m_pTable->ReleaseInfo("EndTableMatch Start......");

	TestGPPlayer(pPlayer,SitID);
	assert(pPlayer);
	assert(SitID>0);
	assert(IsAvailSitID(SitID));

	EndMatchPlayer(pPlayer,SitID);
	EndGameLogic();

	m_pTable->DebugInfo("EndTableMatch AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID() );
}

void CDeZhouGameLogic::EndMatchPlayer(PlayerPtr pPlayer,int SitID)
{
	TraceStackPath logTP("CDeZhouGameLogic::EndMatchPlayer");
	CLogFuncTime lft(s_LogicFuncTime,"EndMatchPlayer");
	m_pTable->CeShiInfo("EndMatchPlayer() Start PID=%d LeftPlayer=%d",pPlayer->GetPID(),m_pMatchRule->m_LeftPlayer);

	TestGPPlayer(pPlayer,SitID);
	
	pPlayer->Lock(m_CurTime);

	int nPosition = m_pMatchRule->m_LeftPlayer--;
	INT64 AwardMoney = m_pMatchRule->GetAwardGameMoney(nPosition);
	INT32 AwardJF = m_pMatchRule->GetAwardJF(nPosition);

	assert(nPosition>=0);
	m_GP[SitID-1].m_nPosition = nPosition;

	SendWashOutPlayer(SitID);

	if ( AwardMoney > 0 )
	{
		pPlayer->AddGameMoney(AwardMoney);
		pPlayer->AddMoneyLog(AwardMoney,"MatchAward");
	}
	if ( AwardJF > 0 )
	{
		pPlayer->AddMatchJF(AwardJF);
	}

	INT64 nTicket = m_pMatchRule->GetTicket();
	pPlayer->m_nUpperLimite += nTicket*10;
	pPlayer->m_nLowerLimite += max(1,nTicket/10);

	GameXY::DeZhou::GameDeZhou_MatchResult PlayerMR;
	PlayerMR.m_MatchID = m_pMatchRule->m_MatchID;
	PlayerMR.m_Position = nPosition;
	PlayerMR.m_nAwardMoney = AwardMoney;
	PlayerMR.m_nGameMoney = pPlayer->GetGameMoney();
	PlayerMR.m_nAwardJF = AwardJF;
	PlayerMR.m_nMatchJF = pPlayer->GetMatchJF();
	pPlayer->SendGameLogicMsg(PlayerMR);

	DBServerXY::WDB_MatchResult msgMR;
	msgMR.m_MatchGUID         = m_pMatchRule->m_MatchGUID;
	msgMR.m_AID               = pPlayer->GetAID();
	msgMR.m_PID               = pPlayer->GetPID();
	msgMR.m_RoomID            = m_pTable->GetRoomID();
	msgMR.m_TableID           = m_pTable->GetTableID();
	msgMR.m_MatchID           = m_pMatchRule->m_MatchID;
	msgMR.m_MatchType         = m_pMatchRule->m_MatchType;
	msgMR.m_Ticket            = m_pMatchRule->GetTicket();
	msgMR.m_TakeMoney         = m_pMatchRule->GetTakeMoney();
	msgMR.m_AwardMoney        = AwardMoney;
	msgMR.m_AwardJF           = AwardJF;
	msgMR.m_Position          = nPosition;
	msgMR.m_Money             = AwardMoney - m_pMatchRule->GetMatchCost();
	msgMR.m_JF                = 0;
	msgMR.m_EP                = 0;
	m_pServer->SendMsgToDBServer(msgMR);

	if ( m_GP[SitID-1].GetRealTableMoney() > 0 )
	{
		DBServerXY::WDB_MatchWinLoss msgMWL;
		msgMWL.m_MatchGUID        = m_pMatchRule->m_MatchGUID;
		msgMWL.m_GameGUID         = m_GameLogicGUID;
		msgMWL.m_AID              = pPlayer->GetAID();
		msgMWL.m_PID              = pPlayer->GetPID();
		msgMWL.m_nWinLossMoney    = -m_GP[SitID-1].GetRealTableMoney();
		msgMWL.m_RoomID           = m_pTable->GetRoomID();
		msgMWL.m_TableID          = m_pTable->GetTableID();
		msgMWL.m_SitID            = SitID;
		m_pServer->SendMsgToDBServer(msgMWL);

		msgMWL.ReSet();
		msgMWL.m_MatchGUID        = m_pMatchRule->m_MatchGUID;
		msgMWL.m_GameGUID         = m_GameLogicGUID;
		msgMWL.m_AID              = 0;
		msgMWL.m_PID              = 0;
		msgMWL.m_nWinLossMoney    = m_GP[SitID-1].GetRealTableMoney();
		msgMWL.m_RoomID           = m_pTable->GetRoomID();
		msgMWL.m_TableID          = m_pTable->GetTableID();
		msgMWL.m_SitID            = 0;
		m_pServer->SendMsgToDBServer(msgMWL);
	}

	m_pTable->DebugInfo("EndMatchPlayer End PID=%d Position=%d",pPlayer->GetPID(),nPosition);
}
void CDeZhouGameLogic::SysWinMoney(INT64 nMoney)
{
	if( IsCommondTable() )
	{
		m_nGameLossMoney += nMoney;

		DBServerXY::WDB_WinLoss wl;
		wl.m_GameGUID          = m_GameLogicGUID;
		wl.m_AID               = 0;
		wl.m_PID               = 0;
		wl.m_RoomID            = m_pTable->GetRoomID();
		wl.m_TableID           = m_pTable->GetTableID();
		wl.m_SitID             = 0;
		wl.m_nServiceMoney     = 0;
		wl.m_nWinLossMoney     = nMoney;
		wl.m_EndGameFlag       = TALLY_SYS;
		wl.m_LeftPai           = 0;
		wl.m_RightPai          = 0;
		wl.m_PaiType           = 0;

		wl.m_Money             = nMoney;
		wl.m_JF                = 0;
		wl.m_EP                = 0;

		LogicSendMsgToDBServer(wl);
	}
	else
	{
		m_pMatchRule->m_TotalLossMoney += nMoney;

		DBServerXY::WDB_MatchWinLoss wdbmsg_MWL;
		wdbmsg_MWL.m_AID       = 0;
		wdbmsg_MWL.m_PID       = 0;
		wdbmsg_MWL.m_GameGUID  = m_GameLogicGUID;
		wdbmsg_MWL.m_MatchGUID = m_pMatchRule->m_MatchGUID;
		wdbmsg_MWL.m_RoomID    = m_pTable->GetRoomID();
		wdbmsg_MWL.m_TableID   = m_pTable->GetTableID();
		wdbmsg_MWL.m_SitID     = 0;
		wdbmsg_MWL.m_nWinLossMoney = nMoney;

		LogicSendMsgToDBServer(wdbmsg_MWL);
	}	
}

void CDeZhouGameLogic::WriteToDB(stWinLossTally* pWLT,int nLen)
{
	TraceStackPath logTP("CDeZhouGameLogic::WriteToDB");
	CLogFuncTime lft(s_LogicFuncTime,"WriteToDB");
	
	m_pTable->CeShiInfo("WriteToDB Start m_nGameLossMoney=%s m_nGameWinLossMoney=%s",
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

	//m_pTable->CeShiInfo("WriteToDB End");
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

				DBServerXY::DBS_msgUserGiftInfo msgUGI;
				if ( pPlayer->GetCurUserGift(msgUGI,pPlayer->m_CurGiftIdx) )
				{
					DBServerXY::DBS_msgGiftInfo msgGI;
					if ( GetGiftInfo(msgUGI.m_GiftID,msgGI) )
					{
						bool bCanUse = false;
						if ( msgGI.m_PriceFlag==N_Gift::PriceFlag_MoGui || msgGI.m_PriceFlag==N_Gift::PriceFlag_Fixed )
						{
							bCanUse = true;
						}
						else if ( msgGI.m_PriceFlag == N_Gift::PriceFlag_Chang )
						{
							int GiftPrice = int((INT64(msgGI.m_Price)*m_GiftBase)/100);
							GiftPrice = max(GiftPrice,msgGI.m_MinPrice);
							GiftPrice = min(GiftPrice,msgGI.m_MaxPrice);

							if ( msgUGI.m_Price >= GiftPrice )
							{
								bCanUse = true;
							}
						}
						if ( bCanUse )
						{
							spi.m_GiftID = msgUGI.m_GiftID;
						}
					}
				}
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
	m_pTable->CeShiInfo("SendGameToPlayer Start PID=%d",pPlayer->GetPID());

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
	SendBankerInfo(pPlayer);

	GameDeZhou_GamePlayerInfo gpi;
	for (int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() )
		{
			gpi.m_GamePlayerInfo[Sit].m_SitID             = Sit+1;
			gpi.m_GamePlayerInfo[Sit].m_GameState         = m_GP[Sit].GetGameState();
			gpi.m_GamePlayerInfo[Sit].m_Action            = m_GP[Sit].GetLastAction();
			gpi.m_GamePlayerInfo[Sit].m_nTableMoney       = m_GP[Sit].GetTableMoney() - m_GP[Sit].GetCurChip();
			gpi.m_GamePlayerInfo[Sit].m_nChip             = m_GP[Sit].GetCurChip();
			if ( IsGamePlaying() && m_GP[Sit].IsInGame() )
			{
				if ( m_GP[Sit].GetPID() == pPlayer->GetPID() )
				{
					gpi.m_GamePlayerInfo[Sit].m_HandPai       = m_GP[Sit].m_HandPai;
				}
				else
				{
					gpi.m_GamePlayerInfo[Sit].m_HandPai = stHandPai(PAI_VALUE_BACK,PAI_VALUE_BACK);
				}
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

	SendMatchInfoToPlayer(pPlayer);

	SendTablePai(m_GameState,pPlayer);
	if ( IsTableMatch() && IsMatching() )
	{	
		SendChangeBlind(m_pMatchRule->m_BigBlind,m_pMatchRule->m_SmallBlind,pPlayer);
	}

	if( IsGamePlaying() )
	{
		SendMoneyPool(pPlayer);
	}

	SendAllJuBaoPeng(pPlayer);
	
	GameDeZhou_ReLink rlEnd;
	rlEnd.m_Flag = rlEnd.End;
	SendLogicMsgToOnePlayer(rlEnd,pPlayer);

	if( IsGamePlaying() )
	{
		if ( m_pTable->m_IsHaveCommonPlayer && m_DelayTime>m_WaitChipTime )
		{
			m_DelayTime = m_WaitChipTime;
		}

		GameDeZhou_PlayerTurn pt;
		pt.m_SitID    = m_CurPlayerSitID;
		if ( m_GP[m_CurPlayerSitID-1].IsBotPlayer() )
			pt.m_LeftTime = m_ActionTime+m_WaitChipTime-m_CurTime;
		else 
			pt.m_LeftTime = m_ActionTime+m_DelayTime-m_CurTime;
		SendLogicMsgToOnePlayer(pt,pPlayer);

		if ( pPlayer->IsPlaying() && pPlayer->GetSitID()==m_CurPlayerSitID )
		{
			SendPlayerPower(pPlayer);
		}		
	}

	//m_pTable->CeShiInfo("SendGameToPlayer End");
}

void CDeZhouGameLogic::SendAllJuBaoPeng(PlayerPtr pPlayer)
{
	for(int Sit=0;Sit<m_MaxSitPlayerNumber;Sit++)
	{
		if ( m_GP[Sit].IsHavePlayer() && m_GP[Sit].m_JuBaoChip>0 )
		{
			GameDeZhou_JoinJuBaoPengInfo msgJBPInfo;
			msgJBPInfo.m_Flag = msgJBPInfo.JoinJuBaoPeng;
			msgJBPInfo.m_SitID = Sit+1;
			pPlayer->SendGameLogicMsg(msgJBPInfo);
		}
	}
}

void CDeZhouGameLogic::SendWashOutPlayer(int SitID,PlayerPtr pPlayer)
{
	assert(IsAvailSitID(SitID));
	assert(IsTableMatch());

	int Sit = SitID - 1;
	GameDeZhou_WashOutPlayerInfo msgWOPI;
	msgWOPI.m_SitID        = SitID;
	msgWOPI.m_PID          = m_GP[Sit].GetPID();
	msgWOPI.m_Position     = m_GP[Sit].m_nPosition;
	msgWOPI.m_NickName     = m_GP[Sit].m_NickName;
	msgWOPI.m_HeadPicURL   = m_GP[Sit].m_HeadPicURL;

	if ( pPlayer )
	{
		pPlayer->SendGameLogicMsg(msgWOPI);
	}
	else
	{
		SendLogicMsgToAllPlayer(msgWOPI);
	}
}

void CDeZhouGameLogic::SendTableWinLoss(PlayerPtr pPlayer)
{
	assert(m_pTable->IsTablePrivate());
	if ( m_mapTableWinLoss.size() > GameDeZhou_TableWinLoss::MaxWinLossCount )
	{

	}
}

void CDeZhouGameLogic::SendMatchInfoToPlayer(PlayerPtr pPlayer)
{
	if ( IsTableMatch() && pPlayer ) //发送淘汰的玩家
	{
		for (int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
		{
			if ( m_GP[Sit].IsWashOut() )
			{
				SendWashOutPlayer(Sit+1,pPlayer);
			}
		}
	}
}

void CDeZhouGameLogic::BotPlayerChip()
{
	TraceStackPath logTP("CDeZhouGameLogic::BotPlayerChip");
	CLogFuncTime lft(s_LogicFuncTime,"BotPlayerChip");
	m_pTable->CeShiInfo("BotPlayerChip Start m_CurPlayerSitID=%d ",m_CurPlayerSitID);
	if ( IsAvailSitID(m_CurPlayerSitID) && IsGamePlaying() && m_GP[m_CurPlayerSitID-1].IsBotPlayer() )
	{
		BYTE Sit = m_CurPlayerSitID - 1;

		GameXY::ClientToServerMessage CTS;
		GameDeZhou_ReqAddChip rac;

		rac.m_CurPlayerSitID = m_CurPlayerSitID;
		
		INT64 nCurMaxChipOnTable = m_MaxChipOnTable;
		if ( m_pGSConfig->CeshiMode() )
		{
			INT64 CurMaxChip = GetCurMaxChipOnTable();
			if ( nCurMaxChipOnTable != CurMaxChip )
			{
				m_pTable->DebugError("nCurMaxChipOnTable=%s MaxChip=%s",N2S(nCurMaxChipOnTable).c_str(),N2S(CurMaxChip).c_str());
				DebugAllPlayer();
			}
		}

		if ( m_GP[Sit].m_MaxBlind>0 && m_GP[Sit].m_MinBlind>0 )
		{
			if ( nCurMaxChipOnTable == 0 )
			{
				if ( CRandom::GetChangce(2,1) && m_GP[Sit].m_MaxBlind <= 50 )
				{
					rac.m_nCurChipMoney = 0;
					rac.m_ChipAction = ACTION_KanPai;
				}
				else
				{
					if ( m_GP[Sit].m_MinBlind >= 5 )
					{
						rac.m_nCurChipMoney = INT64(m_nBigBlind) * (m_GP[Sit].m_MinBlind/5)*5;
					}
					else
					{
						rac.m_nCurChipMoney = INT64(m_nBigBlind) * m_GP[Sit].m_MinBlind;
					}
					rac.m_ChipAction = ACTION_Chip;
				}
			}
			else
			{
				nCurMaxChipOnTable = max(nCurMaxChipOnTable,m_nBigBlind);
				if ( nCurMaxChipOnTable > INT64(m_GP[Sit].m_MaxBlind)*m_nBigBlind )
				{
                    rac.m_nCurChipMoney = 0;
					rac.m_ChipAction = ACTION_KanPai;
					
					INT64 nTableMoney = m_GP[Sit].GetOriginMoney() - m_GP[Sit].m_OtherLoss;
					INT64 nHaveChipMoney = -m_GP[Sit].GetWinLossMoney();
					assert( nTableMoney>0 && nHaveChipMoney>=0 );
					if ( nHaveChipMoney*100/nTableMoney>=60 )
					{
						rac.m_nCurChipMoney = min(nCurMaxChipOnTable,m_GP[Sit].GetTableMoney());
						rac.m_ChipAction = ACTION_Chip;
					}
				}
				else
				{
					if ( nCurMaxChipOnTable < INT64(m_GP[Sit].m_MinBlind)*m_nBigBlind )
					{
						if ( CRandom::GetChangce(2,1))
						{
							rac.m_nCurChipMoney = max(nCurMaxChipOnTable,m_nBigBlind);
						}
						else
						{
							int CurChipBlind = max(int(nCurMaxChipOnTable/m_nBigBlind),1);
							rac.m_nCurChipMoney = INT64(CRandom::Random_Int(CurChipBlind,m_GP[Sit].m_MinBlind))*m_nBigBlind;
							rac.m_nCurChipMoney = min(INT64(m_GP[Sit].m_MinBlind)*m_nBigBlind,rac.m_nCurChipMoney);
						}
					}
					else
					{
						if ( CRandom::GetChangce(PAI_TYPE_MAX-1,m_GP[Sit].m_PaiType.m_Type) && m_GP[Sit].m_MaxBlind>=50 )
						{
							rac.m_nCurChipMoney = min(nCurMaxChipOnTable+m_MinAddChip*CRandom::Random_Int(1,5),INT64(m_GP[Sit].m_MaxBlind)*m_nBigBlind);
						}
						else
						{
							rac.m_nCurChipMoney = nCurMaxChipOnTable;
						}
					}
					rac.m_ChipAction = ACTION_AddChip;
				}
			}
		}
		else
		{
			rac.m_nCurChipMoney = 0;
			rac.m_ChipAction = ACTION_KanPai;
		}

		INT64 nChipMoney = rac.m_nCurChipMoney;
		INT64 nTableMoney = m_GP[Sit].GetTableMoney();
		if ( nChipMoney>0 && nChipMoney<nTableMoney )
		{
			if ( (nChipMoney*100/nTableMoney)>=80 )
			{
				rac.m_nCurChipMoney = nTableMoney;
			}
		}

		if ( m_GameState == GAME_ST_HAND ) //开局ALLIN
		{
			if ( !IsMatchTable() ) 
			{
				int nChangce = m_GamePaiLogic.GetHandPaiChangce(Sit);

				INT64 nMaxAllIn = 0;
				for ( int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
				{
					if ( m_GP[Sit].IsAllIn() && m_GP[Sit].GetCurChip()>nMaxAllIn )
					{
						nMaxAllIn = m_GP[Sit].GetCurChip();
					}
				}

				int nAllInBlind = max(int(nMaxAllIn/m_nBigBlind),1);
				int nTableBlind = max(int(nTableMoney/m_nBigBlind),1);
				if ( nMaxAllIn>0 )
				{
					if ( nTableBlind<500 && CRandom::GetChangce(nAllInBlind,min(nAllInBlind,nChangce*2)) )
					{
						rac.m_nCurChipMoney = max(rac.m_nCurChipMoney,min(nMaxAllIn,nTableMoney));
					}					
				}
				else
				{
					if ( nTableBlind<=200 && CRandom::GetChangce(nTableBlind*2,min(nTableBlind,nChangce)) )
					{
						rac.m_nCurChipMoney = nTableMoney;
					}
				}
			}
		}

		m_pTable->CeShiInfo("BotPlayerChip SitID=%d ChipMoney=%s Blind=%d MaxChip=%s MaxBlind=%d MinBlind=%d",
			rac.m_CurPlayerSitID,Tool::N2S(rac.m_nCurChipMoney.get()).c_str(),m_nBigBlind,
			Tool::N2S(nCurMaxChipOnTable).c_str(),m_GP[Sit].m_MaxBlind,m_GP[Sit].m_MinBlind );

		if ( !m_GP[Sit].GetShowFace() )
		{
			DeZhouLogic::DeZhouLogic_ShowFace msgSF;
			msgSF.m_SitID      = rac.m_CurPlayerSitID;
			msgSF.m_PID        = m_GP[Sit].GetPID();
			if ( rac.m_nCurChipMoney > 0 )
			{
				if ( rac.m_nCurChipMoney == m_GP[Sit].GetTableMoney() )
				{
					if ( Tool::CRandom::GetChangce(10,4) )  //祈祷表情
					{
						msgSF.m_FaceID     = N_Face::FaceID_QiDao;
						msgSF.m_ActionTime = m_CurTime + Tool::CRandom::Random_Int(5,10);
						PushMsgInList(msgSF);
					}
				}
				else if ( rac.m_nCurChipMoney > nCurMaxChipOnTable )
				{
					if ( Tool::CRandom::GetChangce(10,2) )
					{
						int FaceCount = s_KaiXinFaceID.size();
						if ( FaceCount > 0 )
						{
							int Idx        = CRandom::Random_Int(0,FaceCount-1);
							int FaceID     = s_KaiXinFaceID[Idx];

							msgSF.m_FaceID     = FaceID;
							msgSF.m_ActionTime = m_CurTime + Tool::CRandom::Random_Int(1,3);
							PushMsgInList(msgSF);
						}
					}
				}
				else
				{
					if ( Tool::CRandom::GetChangce(10,1) )
					{
						int FaceCount = s_WuLiaoFaceID.size();
						if ( FaceCount > 0 )
						{
							int Idx        = CRandom::Random_Int(0,FaceCount-1);
							int FaceID     = s_WuLiaoFaceID[Idx];

							msgSF.m_FaceID     = FaceID;
							msgSF.m_ActionTime = m_CurTime + Tool::CRandom::Random_Int(1,3);
							PushMsgInList(msgSF);
						}
					}
				}
			}
			else
			{
				if ( m_GP[Sit].GetCurChip()>0 && Tool::CRandom::GetChangce(10,1) )
				{
					int FaceCount = s_ShangXinFaceID.size();
					if ( FaceCount > 0 )
					{
						int Idx = CRandom::Random_Int(0,FaceCount-1);
						int FaceID = s_ShangXinFaceID[Idx];

						msgSF.m_FaceID     = FaceID;
						msgSF.m_ActionTime = m_CurTime + Tool::CRandom::Random_Int(1,3);
						PushMsgInList(msgSF);
					}
				}
			}
		}

		MakeLogicCTS(rac,CTS);
		OnPlayerAddChip(m_pTable->GetTablePlayerByPID(m_GP[Sit].GetPID()),CTS);
	}
	else
	{
		m_pTable->DebugError("BotPlayerChip() GameState=%d m_CurPlayerSitID=%d",m_GameState,m_CurPlayerSitID);
		if ( IsAvailSitID(m_CurPlayerSitID))
		{
			m_pTable->DebugError("BotPlayerChip() PID=%d",m_GP[m_CurPlayerSitID-1].GetPID());
		}
	}

	//m_pTable->CeShiInfo("BotPlayerChip End");
}
void CDeZhouGameLogic::BotShowFace(int SitID,int FaceID)
{
	TraceStackPath logTP("CDeZhouGameLogic::BotShowFace");
	CLogFuncTime lft(s_LogicFuncTime,"BotShowFace");
	int Sit = SitID - 1;
	if ( IsAvailSitID(SitID) && m_GP[Sit].IsBotPlayer() )
	{
		PlayerPtr pPlayer = m_pTable->GetSitPlayerBySitID(SitID);
		if ( pPlayer )
		{
			GameXY::ClientToServerMessage CTS;
			GameDeZhou_ReqShowFace msgSF;

			msgSF.m_SitID    = SitID;
			msgSF.m_FaceID   = FaceID;

			MakeLogicCTS(msgSF,CTS);
			OnShowFace(pPlayer,CTS);
		}
	}
	else
	{
		if ( !IsAvailSitID(SitID) )
		{
			m_pTable->DebugError("BotShowFace SitID=%d",SitID);
		}
		else if ( !m_GP[Sit].IsBotPlayer() )
		{
			m_pTable->DebugError("BotShowFace AID=%d PID=%d",m_GP[Sit].GetAID(),m_GP[Sit].GetPID());
		}
	}
}
void CDeZhouGameLogic::AutoChip()
{
	TraceStackPath logTP("CDeZhouGameLogic::AutoChip");
	CLogFuncTime lft(s_LogicFuncTime,"AutoChip");
	if ( IsAvailSitID(m_CurPlayerSitID) && IsGamePlaying() )
	{
		GameXY::ClientToServerMessage CTS;
		GameDeZhou_ReqAddChip rac;

		rac.m_CurPlayerSitID = m_CurPlayerSitID;

		BYTE Sit = m_CurPlayerSitID - 1;
		if ( m_GP[Sit].GetCurChip() >= m_MaxChipOnTable )
		{
			rac.m_nCurChipMoney = m_GP[Sit].GetCurChip();
			rac.m_ChipAction = ACTION_KanPai;
		}
		else
		{
			rac.m_nCurChipMoney = 0;
			rac.m_ChipAction = ACTION_GiveUp;
		}

		m_GP[Sit].m_AutoOutTimes++;
		if ( m_GP[Sit].m_AutoOutTimes > 3 && (!IsMatchTable()) )
		{
			m_pTable->DebugInfo("OutTime 超时 CurSitID=%d",rac.m_CurPlayerSitID);
			LogicStandUpPlayer(m_pTable->GetSitPlayerBySitID(m_CurPlayerSitID),m_CurPlayerSitID,Action_Flag_SysLogic);
		}
		else
		{
			m_GP[Sit].m_AutoOutTimes += 100;

			m_pTable->DebugInfo("AutoOut PID=%d SitID=%d Chip=%s Action=%d",
				m_GP[Sit].GetPID(),rac.m_CurPlayerSitID,N2S(rac.m_nCurChipMoney.get()).c_str(),rac.m_ChipAction );

			MakeLogicCTS(rac,CTS);
			OnPlayerAddChip(m_pTable->GetTablePlayerByPID(m_GP[Sit].GetPID()),CTS);

			m_GP[Sit].m_AutoOutTimes = m_GP[Sit].m_AutoOutTimes % 100;
		}		
	}
	else
	{
		m_pTable->DebugError("AutoChip() GameState=%d m_CurPlayerSitID=%d m_MaxSitPlayerNumber=%d",m_GameState,m_CurPlayerSitID,m_MaxSitPlayerNumber);
	}
}

BYTE CDeZhouGameLogic::CanSitDown( PlayerPtr pPlayer,BYTE SitID,INT64& nTakeMoney)
{
	BYTE ret = GameXY::RespPlayerAct::SUCCESS;

	nTakeMoney = max(nTakeMoney,m_nMinTakeIn);
	nTakeMoney = min(nTakeMoney,m_nMaxTakeIn);	

	if ( IsAvailSitID(SitID) )
	{
		if ( m_GP[SitID-1].IsHavePlayer() )
		{
			ret = GameXY::RespPlayerAct::SITISNOTEMPTY;
		}

		/*
		if ( nTakeMoney < m_nMinTakeIn )
		{
			ret = GameXY::RespPlayerAct::LESSMONEY;
		}
		else if ( nTakeMoney > m_nMaxTakeIn )
		{
			ret = GameXY::RespPlayerAct::OVERFULLMONEY;
		}*/

		if ( nTakeMoney > pPlayer->GetGameMoney() )
		{
			if ( pPlayer->GetGameMoney() >= m_nMinTakeIn )
			{
				m_nMinTakeIn = pPlayer->GetGameMoney();
			}
			else
			{
				ret = GameXY::RespPlayerAct::OUTOFOWN;
			}			
		}

		if ( IsMatchTable() )
		{
			ret = GameXY::RespPlayerAct::ACTFAILED;
			if ( IsTableMatch() 
				&& ( !IsMatching() )
				&& pPlayer->GetGameMoney() >= m_pMatchRule->GetMatchCost()
				&& m_pTable->GetTableMatchState() == MatchTable_ST_WaitSign )
			{
				ret = ret = GameXY::RespPlayerAct::SUCCESS;
			}
		}
	}
	else
	{
		ret = GameXY::RespPlayerAct::OUTOFSITID;
	}

	return ret;
}

void CDeZhouGameLogic::PlayerJoinGame(PlayerPtr pPlayer)
{
}

void CDeZhouGameLogic::LogicStandUpPlayer(PlayerPtr pPlayer,int SitID,int Flag)
{
	TraceStackPath logTP("CDeZhouGameLogic::LogicStandUpPlayer");
	CLogFuncTime lft(s_LogicFuncTime,"LogicStandUpPlayer");
	m_pTable->CeShiInfo("CDeZhouGameLogic::LogicStandUpPlayer SitID=%d PID=%d PSitID=%d",SitID,pPlayer->GetPID(),pPlayer->GetSitID());

	if ( pPlayer->IsBotPlayer() )
	{
		m_pTable->RemoveBotPlayer(pPlayer->GetPID());
	}
	else
	{
		m_pTable->PlayerStandUp(pPlayer,SitID,Flag);
	}

	m_pTable->CeShiInfo("CDeZhouGameLogic::LogicStandUpPlayer End........");
}

void CDeZhouGameLogic::GamePlayerStandUp( PlayerPtr pPlayer,int SitID,int Flag )
{
	TraceStackPath logTP("CDeZhouGameLogic::GamePlayerStandUp");
	CLogFuncTime lft(s_LogicFuncTime,"GamePlayerStandUp");

	m_pTable->CeShiInfo("CDeZhouGameLogic::GamePlayerStandUp SitID=%d PID=%d PSitID=%d",SitID,pPlayer->GetPID(),pPlayer->GetSitID());

	if ( IsAvailSitID(SitID) && m_GP[SitID-1].IsHavePlayer() && m_GP[SitID-1].GetPID()==pPlayer->GetPID() )
	{
		int Sit = SitID - 1;
		
		if ( IsGamePlaying() )
		{
			if ( m_GP[Sit].IsAttendGame() )
			{
				assert( m_GP[Sit].GetAID()==pPlayer->GetAID());
				assert( m_GP[Sit].GetPID()==pPlayer->GetPID());
				
				pPlayer->Lock(m_CurTime);

				DBServerXY::WDB_WinLoss wl;
				wl.m_GameGUID          = m_GameLogicGUID;
				wl.m_AID               = m_GP[Sit].GetAID();
				wl.m_PID               = m_GP[Sit].GetPID();
				wl.m_SitID             = SitID;
				wl.m_RoomID            = m_pTable->GetRoomID();
				wl.m_TableID           = m_pTable->GetTableID();
				wl.m_nServiceMoney     = m_Tallys[Sit].m_nServiceMoney;
				wl.m_nWinLossMoney     = m_GP[Sit].GetWinLossMoney();
				if (m_GP[Sit].IsInGame())				
					wl.m_EndGameFlag = TALLY_ESCAPE;				
				else				
					wl.m_EndGameFlag = TALLY_GIVEUP_ESCAPE;							
				wl.m_PaiType           = 0;
				wl.m_LeftPai           = m_GP[Sit].m_HandPai.m_Pai[0];
				wl.m_RightPai          = m_GP[Sit].m_HandPai.m_Pai[1];
				
				wl.m_Money             = wl.m_nWinLossMoney - wl.m_nServiceMoney;
				wl.m_JF                = 0;
				wl.m_EP                = 0;
				m_pServer->SendMsgToDBServer(wl);

				m_nGameWinLossMoney += wl.m_nWinLossMoney;

				m_Tallys[Sit].ReSet();

				m_GP[Sit].SetGameState(GAME_PLAYER_ST_NULL);
				m_GP[Sit].SetLastAction(ACTION_NONE);
				m_GP[Sit].DecTableMoney( m_GP[Sit].GetCurChip() );

				pPlayer->AddGameMoney(wl.m_Money);
				pPlayer->AddMoneyLog(wl.m_Money,"StandUp");
				pPlayer->UpdateGameMoney();

				pPlayer->m_nJF       += wl.m_JF;
				pPlayer->m_nEP       += wl.m_EP;
				pPlayer->m_nLossTimes++;

				pPlayer->m_nUpperLimite += wl.m_nServiceMoney*10;
				pPlayer->m_nLowerLimite += max(1,wl.m_nServiceMoney/10);

				if ( m_GP[Sit].IsBotPlayer() )
				{
					m_BotRealWinLoss += m_GP[Sit].GetWinLossMoney();
				}

				PlayerChipStep ChipCS;
				ChipCS.m_SitID          = SitID;
				ChipCS.m_nMoney         = wl.m_nWinLossMoney;
				ChipCS.m_LeftMoney      = m_GP[Sit].GetTableMoney();
				ChipCS.m_Step           = m_GameState;
				m_pGameLog->AddStepString("玩家逃走 " + ChipCS.GetString());

				bool bEndTurn = false;
				bool bEndGame = false;

				if ( GetAvailPlayerNumber() <= 1 || GetPlayingPlayerNumber() == 0 )
				{
					bEndTurn = true;
					bEndGame = true;
				}
				else
				{
					if ( SitID == m_CurPlayerSitID )
					{
						if ( SitID == m_EndPlayerSitID )
						{
							bEndTurn = true;
							if ( m_GameState == GAME_ST_RIVER || GetPlayingPlayerNumber() <= 1 )
							{
								bEndGame = true;
							}
						}
						else
						{
							m_ActionTime = m_CurTime;

							m_CurPlayerSitID = GetNextPlayingPlayerSitID(m_CurPlayerSitID);
							assert( m_CurPlayerSitID && "GamePlayerStandUp" );
							SendPlayerPower();
						}
					}
					else
					{
						if ( SitID == m_EndPlayerSitID )
						{
							m_EndPlayerSitID = GetPrePlayingPlayerSitID(m_FirstPlayerSitID);
						}
					}
				}

				if ( (!bEndTurn) && (!bEndGame) )
				{
					if ( IsAvailSitID(m_CurPlayerSitID) && GetPlayingPlayerNumber()==1 && m_GP[m_CurPlayerSitID-1].GetCurChip()==m_MaxChipOnTable )
					{
						bEndTurn = true;
						bEndGame = true;
					}
				}

				if( bEndTurn )
				{
					if ( bEndGame )
					{
						EndGame();
					}
					else if ( m_GameState == GAME_ST_HAND )
					{
						OnFlop();
					}
					else if ( m_GameState == GAME_ST_FLOP )
					{
						OnTurn();
					}
					else if ( m_GameState == GAME_ST_TURN )
					{
						OnRiver();
					}
					else
					{
						m_pTable->DebugError("GamePlayerStandUp tid=%d state=%d",m_pTable->GetTableID(),m_GameState);
					}
				}
			}
		}

		if ( m_GP[Sit].m_JuBaoChip>0 || m_GP[Sit].m_JuBaoCount>0 )
		{
			m_GP[Sit].m_JuBaoChip = 0;
			m_GP[Sit].m_JuBaoCount = 0;

			GameDeZhou_JoinJuBaoPengInfo msgInfo;
			msgInfo.m_SitID    = SitID;
			msgInfo.m_Flag     = msgInfo.ExitJuBaoPeng;
			SendLogicMsgToAllPlayer(msgInfo);
		}

		INT64 CurChip = m_GP[Sit].GetCurChip();
		m_GP[Sit].Init();
		m_GP[Sit].SetCurChip(CurChip);

		pPlayer->UpdateGameInfo();
		m_GamePaiLogic.SetPlayerRightBySit(Sit,PLAYER_RIGHT_NONE);
	}
	else
	{
		m_pTable->DebugError("GamePlayerStandUp TableID=%d SitID=%d",m_pTable->GetTableID(),SitID);
		if ( IsAvailSitID(SitID) )
		{
			m_pTable->DebugError("State=%d GPPID=%d PID=%d",m_GP[SitID-1].GetGameState(),m_GP[SitID-1].GetPID(),pPlayer->GetPID());
		}
	}

	m_pTable->CeShiInfo("CDeZhouGameLogic::GamePlayerStandUp End........");
}

void CDeZhouGameLogic::MatchPlayerStandUp(PlayerPtr pPlayer,int SitID,int Flag )
{
	TraceStackPath logTP("CDeZhouGameLogic::MatchPlayerStandUp");
	CLogFuncTime lft(s_LogicFuncTime,"MatchPlayerStandUp");

	int Sit = SitID - 1;

	m_pTable->CeShiInfo("CDeZhouGameLogic::MatchPlayerStandUp SitID=%d PID=%d PSitID=%d Flag=%d",
		SitID,pPlayer->GetPID(),pPlayer->GetSitID(),Flag);
	DebugSitPlayer(SitID);

	TestGPPlayer(pPlayer,SitID);
	assert(IsAvailSitID(SitID));
	assert(m_GP[Sit].IsHavePlayer());
	assert(m_GP[Sit].GetPID()==pPlayer->GetPID());

	if ( Flag == Action_Flag_Match )       //表示在比赛过程中由程序的换位置
	{
		assert( !IsGamePlaying() );
		assert( IsRoomMatch() );

		if ( IsGamePlaying() )
		{
			m_pTable->DebugError("MatchPlayerStandUp game is playing");
		}
		if ( !IsRoomMatch() )
		{
			m_pTable->DebugError("MatchPlayerStandUp TableType=%d",m_TableType);
		}

		if ( IsAvailSitID(SitID) && m_GP[Sit].IsHavePlayer() && m_GP[Sit].GetPID()==pPlayer->GetPID() )
		{
			m_GP[Sit].Init();
		}
		else
		{
			m_pTable->DebugError("MatchPlayerStandUp TableID=%d SitID=%d PID=%d",m_pTable->GetTableID(),SitID,pPlayer->GetPID() );
		}

		if ( !IsGamePlaying() && IsRoomMatch() && GetSitPlayerNumber() <= 0 )
		{
			EndGameLogic();
		}

		return ;
	}

	if ( IsAvailSitID(SitID) && m_GP[Sit].IsHavePlayer() && m_GP[Sit].GetPID()==pPlayer->GetPID() )
	{
		if ( IsMatching() )
		{
			EndMatchPlayer(pPlayer,SitID);
			if ( IsGamePlaying() )
			{
				int Sit = SitID-1;
				if ( m_GP[Sit].GetWinLossMoney() != 0 )
				{
					DBServerXY::WDB_MatchWinLoss msgMWL;
					msgMWL.m_MatchGUID        = m_pMatchRule->m_MatchGUID;
					msgMWL.m_GameGUID         = m_GameLogicGUID;
					msgMWL.m_AID              = pPlayer->GetAID();
					msgMWL.m_PID              = pPlayer->GetPID();
					msgMWL.m_nWinLossMoney    = m_GP[Sit].GetWinLossMoney();
					msgMWL.m_RoomID           = m_pTable->GetRoomID();
					msgMWL.m_TableID          = m_pTable->GetTableID();
					msgMWL.m_SitID            = SitID;
					msgMWL.m_PaiType          = m_GP[Sit].m_PaiType.GetType();
					assert( m_GP[Sit].GetWinLossMoney() <= 0 );
					if ( m_GP[Sit].GetWinLossMoney() > 0 )
					{
						msgMWL.m_EndGameFlag  = TALLY_WIN;
						PlayerPtr pTempPlayer = m_pTable->GetSitPlayerBySitID(SitID);
						if ( pTempPlayer && DezhouLib::IsValidPaiType(msgMWL.m_PaiType) )
						{
							pTempPlayer->m_WinRecord[msgMWL.m_PaiType-1]++;
						}
					}
					else
					{
						msgMWL.m_EndGameFlag  = TALLY_LOSS;
					}
					m_pServer->SendMsgToDBServer(msgMWL);
				}
			}

			m_pMatchRule->m_TotalLossMoney += m_GP[Sit].GetRealTableMoney();
			m_GP[Sit].SetGameState(GAME_PLAYER_ST_NULL);
			if( IsTableMatch() )
			{
				m_GP[Sit].SetGameState(GAME_PLAYER_ST_WASHOUT);
			}
			m_GP[Sit].SetLastAction(ACTION_NONE);
			m_GP[Sit].DecTableMoney( m_GP[Sit].GetCurChip() );

			if ( IsGamePlaying() )
			{
				bool bEndTurn = false;
				bool bEndGame = false;
				if ( GetAvailPlayerNumber() <= 1 || GetPlayingPlayerNumber() == 0 )
				{
					bEndTurn = true;
					bEndGame = true;
				}
				else
				{
					if ( SitID == m_CurPlayerSitID )
					{
						if ( SitID == m_EndPlayerSitID )
						{
							bEndTurn = true;
							if ( m_GameState == GAME_ST_RIVER || GetPlayingPlayerNumber() <= 1 )
							{
								bEndGame = true;
							}
						}
						else
						{
							m_ActionTime = m_CurTime;

							m_CurPlayerSitID = GetNextPlayingPlayerSitID(m_CurPlayerSitID);
							assert( m_CurPlayerSitID && "MatchPlayerStandUp" );
							SendPlayerPower();
						}
					}
					else
					{
						if ( SitID == m_EndPlayerSitID )
						{
							m_EndPlayerSitID = GetPrePlayingPlayerSitID(m_FirstPlayerSitID);
						}
					}
				}

				if ( (!bEndTurn) && (!bEndGame) )
				{
					if ( IsAvailSitID(m_CurPlayerSitID) && GetPlayingPlayerNumber()==1 && m_GP[m_CurPlayerSitID-1].GetCurChip()==m_MaxChipOnTable )
					{
						bEndTurn = true;
						bEndGame = true;
					}
				}

				if( bEndTurn )
				{
					if ( bEndGame )
					{
						EndGame();
					}
					else if ( m_GameState == GAME_ST_HAND )
					{
						OnFlop();
					}
					else if ( m_GameState == GAME_ST_FLOP )
					{
						OnTurn();
					}
					else if ( m_GameState == GAME_ST_TURN )
					{
						OnRiver();
					}
					else
					{
						m_pTable->DebugError("MatchPlayerStandUp tid=%d state=%d",m_pTable->GetTableID(),m_GameState);
					}
				}
			}
			
			//当玩家只余下一个的时候游戏结束
			int nLeftPlayer = GetSitPlayerNumber();
			if ( IsTableMatch() ) 
			{
				assert( nLeftPlayer == m_pMatchRule->m_LeftPlayer );
				if ( nLeftPlayer == 1 )       //中间走到只有一个玩家了，比赛结束
				{
					PlayerPtr pWinPlayer = nullptr;
					int nWinSitID = 0;
					for ( int nSit=0;nSit<m_MaxSitPlayerNumber;++nSit)
					{
						if ( m_GP[nSit].IsHavePlayer() )
						{
							pWinPlayer = m_pTable->GetSitPlayerBySitID(nSit+1);
							nWinSitID = nSit+1;
							break;
						}
					}
					EndTableMatch(pWinPlayer,nWinSitID);
				}
			}
			else if ( IsRoomMatch() )
			{
				if ( nLeftPlayer == 1 && m_pMatchRule->IsChampTable() )       //锦标赛只余下一人结束
				{
					PlayerPtr pWinPlayer = nullptr;
					int nWinSitID = 0;
					for ( int nSit=0;nSit<m_MaxSitPlayerNumber;++nSit)
					{
						if ( m_GP[nSit].IsHavePlayer() )
						{
							pWinPlayer = m_pTable->GetSitPlayerBySitID(nSit+1);
							nWinSitID = nSit+1;
							break;
						}
					}
					EndRoomMatch(pWinPlayer,nWinSitID);
				}
				else if ( nLeftPlayer <= 3 && (!m_pMatchRule->IsChampTable()) )
				{
					EndGameLogic();
				}
			}			
		}

		if ( IsMatching() )
		{			
			if ( IsTableMatch() )
			{
				m_GP[Sit].SetGameState(GAME_PLAYER_ST_WASHOUT);
			}
			else
			{
				INT64 CurChip = m_GP[Sit].GetCurChip();
				m_GP[Sit].Init();
				m_GP[Sit].SetCurChip(CurChip);
			}
			pPlayer->UpdateGameInfo();
		}		
		else
		{
			m_GP[Sit].Init();
		}

		m_GamePaiLogic.SetPlayerRightBySit(Sit,PLAYER_RIGHT_NONE);
	}
	else
	{
		m_pTable->DebugError("MatchPlayerStandUp TableID=%d SitID=%d PID=%d",m_pTable->GetTableID(),SitID,pPlayer->GetPID());
	}

	DebugSitPlayer(SitID);
	m_pTable->CeShiInfo("CDeZhouGameLogic::MatchPlayerStandUp End........");
}

void CDeZhouGameLogic::GamePlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag)
{
	TraceStackPath logTP("CDeZhouGameLogic::GamePlayerSitDown");
	CLogFuncTime lft(s_LogicFuncTime,"GamePlayerSitDown");
	m_pTable->CeShiInfo("CDeZhouGameLogic::GamePlayerSitDown Start SitID=%d",SitID);
	
	if ( pPlayer && IsAvailSitID(SitID) && m_GP[SitID-1].IsExit() )
	{
		int Sit = SitID - 1;

		INT64 CurChip = m_GP[Sit].GetCurChip();
		m_GP[Sit].Init();
		m_GP[Sit].SetCurChip(CurChip);

		m_GP[Sit].SetPID(pPlayer->GetPID());
		m_GP[Sit].SetAID(pPlayer->GetAID());
		m_GP[Sit].SetTableMoney(nTakeMoney);
		m_GP[Sit].SetGameState(GAME_PLAYER_ST_WAITING);
		m_GP[Sit].SetPlayerType(pPlayer->GetPlayerType());
		m_GP[Sit].m_NickName = pPlayer->GetNickName();
		m_GP[Sit].m_HeadPicURL = pPlayer->GetHeadPicURL();
		m_GP[Sit].m_nPosition = 0;

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

	m_pTable->CeShiInfo("CDeZhouGameLogic::GamePlayerSitDown End........");
}

void CDeZhouGameLogic::MatchPlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag)
{
	TraceStackPath logTP("CDeZhouGameLogic::MatchPlayerSitDown");
	CLogFuncTime lft(s_LogicFuncTime,"MatchPlayerSitDown");
	m_pTable->CeShiInfo("CDeZhouGameLogic::MatchPlayerSitDown Start SitID=%d",SitID);

	if ( pPlayer && IsAvailSitID(SitID) && m_GP[SitID-1].IsExit() )
	{
		int Sit = SitID - 1;

		INT64 CurChip = m_GP[Sit].GetCurChip();
		m_GP[Sit].Init();
		m_GP[Sit].SetCurChip(CurChip);

		m_GP[Sit].SetPID(pPlayer->GetPID());
		m_GP[Sit].SetAID(pPlayer->GetAID());
		m_GP[Sit].SetTableMoney(m_pMatchRule->GetStartMoney());
		m_GP[Sit].SetGameState(GAME_PLAYER_ST_WAITING);
		m_GP[Sit].SetPlayerType(pPlayer->GetPlayerType());
		m_GP[Sit].m_NickName = pPlayer->GetNickName();
		m_GP[Sit].m_HeadPicURL = pPlayer->GetHeadPicURL();
		m_GP[Sit].m_nPosition = 0;

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
		m_pTable->DebugError("MatchPlayerSitDown TableID=%d PID=%d SitID=%d TakeMoney=%s IsExit=%d",
			m_pTable->GetTableID(),pPlayer->GetPID(),SitID,Tool::N2S(nTakeMoney).c_str(),m_GP[SitID-1].IsExit() );

		DebugLogicState();
		m_pTable->DebugError("MatchID=%d Matching=%d",m_pMatchRule->GetMatchID(),m_pMatchRule->IsMatching() );
	}

	m_pTable->CeShiInfo("CDeZhouGameLogic::MatchPlayerSitDown End........");
}