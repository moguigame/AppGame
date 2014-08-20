#include "testplayer.h"

CTestPlayer::CTestPlayer(int aid,int pid)
{
	int TempInt = 0;

	m_AID      = aid;
	m_PID      = pid;

	DebugInfo("AID=%d PID=%d",m_PID,m_AID);

	m_OriRoomID = 10201;
	m_OriTableID = (pid-2001)/9+1;
	TempInt = (pid-2000)%9;
	m_OriSitID = TempInt?TempInt:9;

	DebugInfo("初始位置为:RoomID=%d m_OriTableID=%d m_OriSitID=%d",m_OriRoomID,m_OriTableID,m_PID,m_OriSitID);

	m_RoomID = 0;
	m_TableID = 0;
	m_SitID = 0;

	m_PlayerState = 0;

	m_GameState = GAME_ST_NONE;

	m_CurPlayerSitID = 0;
	m_nBigBlind = 10;

	m_nGameMoney = 0;
	m_nTableMoney = 0;

	m_clientsocket.SetClientPool(&m_clientpool);
	m_clientsocket.SetCallBack(this);

	m_clientsocket.SetCrypt(true);
}

CTestPlayer::~CTestPlayer(void)
{
}

void CTestPlayer::DebugError(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		string strTemp = "PID=" + N2S(m_PID) + " " + string(logbuf);
		std::cout<<"Error "<<strTemp<<endl;
	}
}
void CTestPlayer::DebugInfo(const char* logstr,...)
{
#ifdef TestInfo
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		string strTemp = "PID=" + N2S(m_PID) + " " + string(logbuf);
		std::cout<<"Info  "<<strTemp<<endl;
	}
#endif
}

int CTestPlayer::StartMsg()
{
	ReqKey msgRK;
	msgRK.m_Flag = msgRK.KeyDisable;
	m_clientsocket.SendMsg(msgRK);

	return 0;
}

int CTestPlayer::OnTimer(UINT32 curTime)
{
	m_curTime = curTime;

	if ( m_curTime-m_LastTime >= 5 && m_PlayerState )
	{
		m_LastTime = m_curTime;
		
		if ( m_RoomID == 0 )
		{
			ReqJoinRoom msgJR;
			msgJR.m_RoomID = m_OriRoomID;
			m_clientsocket.SendMsg(msgJR);

			DebugInfo("请求加入房间 RoomID=%d",msgJR.m_RoomID);
		}		
		else if ( m_TableID == 0 )
		{
			ReqJoinTable msgJT;
			msgJT.m_RoomID    = m_OriRoomID;
			msgJT.m_TableID   = m_OriTableID;
			m_clientsocket.SendMsg(msgJT);

			DebugInfo("请求加入桌子 TableID=%d",msgJT.m_TableID);
		}
		else if ( m_SitID == 0 )
		{
			ReqPlayerAct msgAT;
			msgAT.m_TableID        = m_OriTableID;
			msgAT.m_SitID          = m_OriSitID;
			msgAT.m_Action         = msgAT.SITDOWN;
			msgAT.m_nTakeMoney     = m_nBigBlind*100;
			m_clientsocket.SendMsg(msgAT);

			DebugInfo("请求坐下 TableID=%d SitID=%d",msgAT.m_TableID,msgAT.m_SitID);
		}
		else
		{
			if ( m_CurPlayerSitID == m_SitID )
			{
				GameDeZhou_ReqAddChip msgAC;
				msgAC.m_CurPlayerSitID = m_SitID;

				if ( Tool::CRandom::GetChangce(3,1))
				{
					msgAC.m_ChipAction = 0;
					msgAC.m_nCurChipMoney = 0;
				}
				else
				{
					msgAC.m_ChipAction = ACTION_AddChip;
					msgAC.m_nCurChipMoney = m_nBigBlind;
				}

				m_clientsocket.SendGameLogicMsg(msgAC);
			}
			else
			{
				if ( Tool::CRandom::GetChangce(20,1) )
				{
					if (CRandom::GetChangce(3,1))
					{
						//送游戏币
						GameDeZhou_ReqSendMoney msgSM;
						msgSM.m_SendPID = m_PID;
						msgSM.m_RecvPID = (m_PID-m_SitID)+CRandom::Random_Int(1,9);
						msgSM.m_nSendMoney = CRandom::Random_Int(10,100);
						m_clientsocket.SendGameLogicMsg(msgSM);
					}
					else if ( CRandom::GetChangce(2,1))
					{
						//送礼物
						GameDeZhou_SendGift msgSG;
						msgSG.m_GiftID = CRandom::Random_Int(1,11);
						msgSG.m_SendPID = m_PID;
						msgSG.m_vecToPID.push_back((m_PID-m_SitID)+CRandom::Random_Int(1,9));
						msgSG.m_vecToPID.push_back((m_PID-m_SitID)+CRandom::Random_Int(1,9));
						msgSG.m_vecToPID.push_back((m_PID-m_SitID)+CRandom::Random_Int(1,9));
						msgSG.m_vecToPID.push_back((m_PID-m_SitID)+CRandom::Random_Int(1,9));
						m_clientsocket.SendGameLogicMsg(msgSG);
					}
					else
					{
						//发表情
						GameDeZhou_ReqShowFace msgSF;
						msgSF.m_SitID = m_SitID;
						msgSF.m_FaceID = CRandom::Random_Int(1,10);
						m_clientsocket.SendGameLogicMsg(msgSF);
					}
				}
				else
				{
					if (CRandom::GetChangce(30,1))
					{
						ReqPlayerAct msgPA;
						msgPA.m_TableID = m_TableID;
						msgPA.m_SitID = m_SitID;
						msgPA.m_Action = msgPA.STANDUP;
						m_clientsocket.SendMsg(msgPA);
					}
					else if ( CRandom::GetChangce(50,1))
					{
						ReqLeaveTable msgLT;
						msgLT.m_RoomID = m_RoomID;
						msgLT.m_TableID = m_TableID;						
						m_clientsocket.SendMsg(msgLT);
					}
				}				
			}
		}
	}

	return 0;
}

int CTestPlayer::OnGameMsg(CRecvMsgPacket& msgPack)
{
	GameXY::ClientToServerMessage CTSMsg;
	TransplainMsg(msgPack,CTSMsg);

	DebugInfo("OnGameMsg XYID=%d XYLEN=%d",CTSMsg.m_MsgID,CTSMsg.m_MsgLen);

	int ret = 0;
	switch ( CTSMsg.m_MsgID )
	{
	case GameDeZhou_ReLink::XY_ID:
		{
			ret = OnGameRelink(CTSMsg);
		}
		break;
	case GameDeZhou_PromoteTime::XY_ID:
		{

		}
		break;
	case GameDeZhou_StartInfo::XY_ID:
		{
			ret = OnStartInfo(CTSMsg);
		}
		break;
	case GameDeZhou_GameState::XY_ID:
		{
			ret = OnGameState(CTSMsg);
		}
		break;
	case GameDeZhou_BankerInfo::XY_ID:
		{

		}
		break;
	case GameDeZhou_GamePlayerInfo::XY_ID:
		{

		}
		break;
	case GameDeZhou_SitPlayerInfo::XY_ID:
		{
			ret = OnGameSitPlayerInfo(CTSMsg);
		}
		break;
	case GameDeZhou_TableMoney::XY_ID:
		{

		}
		break;
	case GameDeZhou_GameMoney::XY_ID:
		{
			ret = OnGameUpGameMoney(CTSMsg);
		}
		break;
	case GameDeZhou_MoneyPool::XY_ID:
		{

		}
		break;
	case GameDeZhou_Result::XY_ID:
		{
			m_CurPlayerSitID = 0;
		}
		break;
	case GameDeZhou_TablePai::XY_ID:
		{
			m_CurPlayerSitID = 0;
		}
		break;
	case GameDeZhou_AddChipInfo::XY_ID:
		{
			ret = OnAddChipInfo(CTSMsg);
		}
		break;
	case GameDeZhou_PlayerTurn::XY_ID:
		{
			ret = OnPlayerTurn(CTSMsg);
		}
		break;
	case GameDeZhou_PlayerPower::XY_ID:
		{
			ret = OnPlayerPower(CTSMsg);
		}
		break;
	case GameDeZhou_Start::XY_ID:
		{
			ret = OnGameStart(CTSMsg);
		}
		break;
	case GameDeZhou_Flag::XY_ID:
		{
			ret = OnGameFlag(CTSMsg);
		}
		break;
	case GameDeZhou_ReqAddMoney::XY_ID:
		{
			ret = OnReqAddMoney(CTSMsg);
		}
		break;
	case GameDeZhou_RespShowFace::XY_ID:
	case GameDeZhou_RespSendMoney::XY_ID:
	case GameDeZhou_RespSendGift::XY_ID:
	case GameDeZhou_AddMoneyInfo::XY_ID:
		{

		}
		break;
	default:
		{
			ret = 10000;
		}
	}
	if ( ret )
	{
		DebugError("OnGameMsg Error ret=%d XYID=%d XYLEN=%d",ret,CTSMsg.m_MsgID,CTSMsg.m_MsgLen);
	}

	return ret;
}

int  CTestPlayer::OnPlayerPower(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_PlayerPower msgPP;
	TransplainMsgCTS(CTS,msgPP);

	DebugInfo("轮到自已下注");
	m_CurPlayerSitID = m_SitID;

	return 0;
}

int CTestPlayer::OnGameStart(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_Start msgST;
	TransplainMsgCTS(CTS,msgST);

	return 0;
}

int CTestPlayer::OnStartInfo(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_StartInfo msgSTI;
	TransplainMsgCTS(CTS,msgSTI);

	return 0;
}

int CTestPlayer::OnGameState(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_GameState msgGS;
	TransplainMsgCTS(CTS,msgGS);

	m_GameState = msgGS.m_GameST;

	return 0;
}

int CTestPlayer::OnGameFlag(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_Flag msgFlag;
	TransplainMsgCTS(CTS,msgFlag);

	if ( msgFlag.m_Flag == msgFlag.ShowPai )
	{
		DebugInfo("你可以SHOW牌了");
	}
	else if ( msgFlag.m_Flag == msgFlag.TakeMoneyOutTime )
	{
		DebugInfo("你带钱超时了");
	}

	return 0;
}

int CTestPlayer::OnReqAddMoney(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_ReqAddMoney msgAM;
	TransplainMsgCTS(CTS,msgAM);

	GameDeZhou_RespAddMoney msgRespAM;
	msgRespAM.m_SitID = m_SitID;
	msgRespAM.m_AddMoney = m_nBigBlind*100;

	m_clientsocket.SendGameLogicMsg(msgRespAM);

	return 0;
}

int  CTestPlayer::OnPlayerTurn(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_PlayerTurn msgPT;
	TransplainMsgCTS(CTS,msgPT);

	return 0;
}

int  CTestPlayer::OnAddChipInfo(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_AddChipInfo msgACI;
	TransplainMsgCTS(CTS,msgACI);

	if ( msgACI.m_CurPlayerSitID == m_SitID )
	{
		DebugInfo("收到自已下注 Money=%d",msgACI.m_nCurChipMoney);
	}

	m_CurPlayerSitID = 0;

	return 0;
}

int  CTestPlayer::OnGameRelink(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_ReLink msgRL;
	TransplainMsgCTS(CTS,msgRL);

	if ( msgRL.m_Flag == msgRL.Start )
	{
	}
	else if ( msgRL.m_Flag == msgRL.End )
	{	
	}

	return 0;
}

int  CTestPlayer::OnGameUpGameMoney(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_GameMoney msgGM;
	TransplainMsgCTS(CTS,msgGM);

	return 0;
}

int  CTestPlayer::OnGameTableMoney(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_TableMoney msgTM;
	TransplainMsgCTS(CTS,msgTM);

	if ( msgTM.m_SitID == m_SitID )
	{
		m_nTableMoney = msgTM.m_nTableMoney;

		DebugInfo("更新自己桌上游戏币数量 Money=%d",m_nTableMoney);
	}

	return 0;
}

int CTestPlayer::OnGameSitPlayerInfo(GameXY::ClientToServerMessage& CTS)
{
	GameDeZhou_SitPlayerInfo msgSPI;
	TransplainMsgCTS(CTS,msgSPI);

	if ( msgSPI.m_PID == m_PID )
	{
		DebugInfo("收到自已坐下信息 PID=%d",msgSPI.m_PID);
	}

	return 0;
}

int CTestPlayer::OnMsg(CMoGuiClientSocket* pSocket,CRecvMsgPacket& msgPack)
{
	int ret = 0;

	DebugInfo("OnMsg XYID=%d XYLEN=%d",msgPack.m_XYID,msgPack.m_nLen);

	switch( msgPack.m_XYID )
	{
	case RespKey::XY_ID:
		{
			ret = OnRespKey(msgPack);
		}
		break;
	case Resp_Heart::XY_ID:
		{
			ret = OnRespHeart(msgPack);
		}
		break;
	case BatchProtocol::XY_ID:
		{
			ret = OnBatchProtocl(msgPack);
		}
		break;
	case GameLobbyRespPlayerConnect::XY_ID:
		{
			ret = OnRespGameConnect(msgPack);
		}
		break;
	case Game_PlayerData::XY_ID:
		{
			ret = OnPlayerData(msgPack);
		}
		break;
	case Game_PlayerDataEx::XY_ID:
		{
			ret = OnPlayerDataEX(msgPack);
		}
		break;
	case UserAwardInfoList::XY_ID:
		{
			ret = OnUserAwardInfoList(msgPack);
		}
		break;
	case Game_MsgRule::XY_ID:
		{
			ret = OnGameRule(msgPack);
		}
		break;
	case RoomInfoList::XY_ID:
		{
			ret = OnRoomInfoList(msgPack);
		}
		break;
	case RespRoomTableInfo::XY_ID:
		{
			ret = OnRespRoomTableInfo(msgPack);
		}
		break;
	case TableInfo::XY_ID:
		{
			ret = OnTableInfo(msgPack);
		}
		break;
	case TableInfoList::XY_ID:
		{
			ret = OnTableInfoList(msgPack);
		}
		break;
	case RespJoinRoom::XY_ID:
		{
			ret = OnRespJoinRoom(msgPack);
		}
		break;
	case RespJoinTable::XY_ID:
		{
			ret = OnJoinTable(msgPack);
		}
		break;
	case LeaveTable::XY_ID:
		{
			ret = OnLeaveTable(msgPack);
		}
		break;
	case LeaveRoom::XY_ID:
		{
			ret = OnLeaveRoom(msgPack);
		}
		break;
	case PlayerAct::XY_ID:
		{
			ret = OnPlayerAct(msgPack);
		}
		break;
	case PlayerBaseInfo::XY_ID:
		{
			ret = OnPlayerBaseInfo(msgPack);
		}
		break;
	case PlayerStateInfo::XY_ID:
		{
			ret = OnPlayerStateInfo(msgPack);
		}
		break;
	case PlayerGameInfo::XY_ID:
		{
			ret = OnPlayerGameInfo(msgPack);
		}
		break;
	case RespPlayerAct::XY_ID:
		{
			ret = OnRespPlayerAct(msgPack);
		}
		break;
	case TableShowList::XY_ID:
		{
			ret = OnTableShowList(msgPack);
		}
		break;
	case TableShow::XY_ID:
		{
			ret = OnTableShow(msgPack);
		}
		break;
	case TablePlayerCountList::XY_ID:
		{
			ret = OnTablePlayerCountList(msgPack);
		}
		break;
	case PlayerGameMoney::XY_ID:
		{
			ret = OnPlayerGameMoney(msgPack);
		}
		break;
	case ServerToClientMessage::XY_ID:
		{
			ret = OnGameMsg(msgPack);
		}
		break;
	case PlayerStateInfoList::XY_ID:
	case PlayerRecvGift::XY_ID:
		{

		}
		break;
	default:
		DebugError("OnMsg 未处理协议 XYID=%d,XYLEN=%d",msgPack.m_XYID,msgPack.m_nLen);
	}
	return ret;
}

int  CTestPlayer::OnPlayerGameMoney(CRecvMsgPacket& msgPack)
{
	PlayerGameMoney msgPGM;
	TransplainMsg(msgPack,msgPGM);

	if ( msgPGM.m_AID == m_AID && msgPGM.m_PID == m_PID )
	{
		m_nGameMoney = msgPGM.m_nGameMoney;
		DebugInfo("更新自己的游戏币数量 Money=%d",m_nGameMoney);
	}

	return 0;
}

int  CTestPlayer::OnTablePlayerCountList(CRecvMsgPacket& msgPack)
{
	TablePlayerCountList msgTPCL;
	TransplainMsg(msgPack,msgTPCL);

	if ( msgTPCL.m_RoomID == m_RoomID )
	{

	}
	else
	{
		DebugError("OnTablePlayerCountList");
	}

	return 0;
}

int  CTestPlayer::OnRespPlayerAct(CRecvMsgPacket& msgPack)
{
	RespPlayerAct msgPA;
	TransplainMsg(msgPack,msgPA);

	if ( msgPA.m_Flag )
	{
		DebugError("请求动作失败");
	}

	return 0;
}

int  CTestPlayer::OnPlayerStateInfo(CRecvMsgPacket& msgPack)
{
	PlayerStateInfo msgPSI;
	TransplainMsg(msgPack,msgPSI);

	if ( msgPSI.m_PID == m_PID )
	{
		DebugInfo("收到自己的状态信息 Flag=%d State=%d",msgPSI.m_Flag,msgPSI.m_State);
	}

	return 0;
}

int  CTestPlayer::OnTableShow(CRecvMsgPacket& msgPack)
{
	TableShow msgTS;
	TransplainMsg(msgPack,msgTS);

	return 0;
}

int CTestPlayer::OnTableShowList(CRecvMsgPacket& msgPack)
{
	TableShowList msgTSL;
	TransplainMsg(msgPack,msgTSL);

	return 0;
}

int  CTestPlayer::OnPlayerGameInfo(CRecvMsgPacket& msgPack)
{
	PlayerGameInfo msgPGI;
	TransplainMsg(msgPack,msgPGI);

	if ( msgPGI.m_PID == m_PID )
	{
		DebugInfo("收到自己的游戏信息 PID=%d",msgPGI.m_PID);
	}

	return 0;
}

int  CTestPlayer::OnPlayerBaseInfo(CRecvMsgPacket& msgPack)
{
	PlayerBaseInfo msgPBI;
	TransplainMsg(msgPack,msgPBI);

	if ( msgPBI.m_PID == m_PID )
	{
		DebugInfo("收到自己BASE信息 PID=%d",msgPBI.m_PID);
	}

	return 0;
}

int  CTestPlayer::OnJoinTable(CRecvMsgPacket& msgPack)
{
	RespJoinTable msgJT;
	TransplainMsg(msgPack,msgJT);

	if ( msgJT.m_Flag == msgJT.SUCCESS )
	{
		if ( msgJT.m_RoomID != m_OriRoomID || msgJT.m_TableID != m_OriTableID )
		{
			DebugError("OnJoinTable ");
		}
		m_TableID = msgJT.m_TableID;
		DebugInfo("加入桌子成功 TableID=%d",msgJT.m_TableID);
	}
	else
	{
		DebugError("OnJoinTable Flag=%d",msgJT.m_Flag);
	}

	return 0;
}

int CTestPlayer::OnLeaveTable(CRecvMsgPacket& msgPack)
{
	LeaveTable msgLT;
	TransplainMsg(msgPack,msgLT);

	if ( msgLT.m_TableID != m_TableID || msgLT.m_RoomID != m_RoomID )
	{
		DebugError("OnLeaveTable ");
	}

	if ( msgLT.m_TableID  == m_TableID && m_TableID == m_OriTableID )
	{
		if ( msgLT.m_PID == m_PID )
		{
			DebugInfo("玩家自己离开桌子");

			m_TableID = 0;
			m_SitID = 0;
		}
	}
	else
	{
		DebugError("OnLeaveTable");
	}	

	return 0;
}

int  CTestPlayer::OnLeaveRoom(CRecvMsgPacket& msgPack)
{
	LeaveRoom msgLR;
	TransplainMsg(msgPack,msgLR);

	if ( msgLR.m_PID == m_PID )
	{
		m_RoomID = 0;
		m_TableID = 0;
		m_SitID = 0;
	}

	return 0;
}

int CTestPlayer::OnPlayerAct(CRecvMsgPacket& msgPack)
{
	PlayerAct msgPA;
	TransplainMsg(msgPack,msgPA);

	if ( msgPA.m_Action == msgPA.SITDOWN )
	{
		if ( msgPA.m_TableID  == m_TableID && m_TableID == m_OriTableID )
		{
			if ( msgPA.m_PID == m_PID )
			{
				m_SitID = msgPA.m_SitID;
				DebugInfo("玩家自己坐下 SitID=%d",msgPA.m_SitID);
			}
		}
		else		
		{
			DebugError("OnPlayerAct SitDown");
		}
	}
	else if ( msgPA.m_Action == msgPA.STANDUP )
	{
		if ( msgPA.m_TableID  == m_TableID && m_TableID == m_OriTableID )
		{
			if ( msgPA.m_PID == m_PID )
			{
				m_SitID = 0;
				DebugInfo("玩家自己站起 SitID=%d",msgPA.m_SitID);
			}
		}
		else
		{
			DebugError("OnPlayerAct StandUP");
		}
	}
	else
	{
		DebugError("OnPlayerAct");
	}

	return 0;
}

int  CTestPlayer::OnRespJoinRoom(CRecvMsgPacket& msgPack)
{
	RespJoinRoom msgJR;
	TransplainMsg(msgPack,msgJR);

	if ( msgJR.m_Flag == msgJR.SUCCESS )
	{
		if ( msgJR.m_RoomID == m_OriRoomID )
		{
			m_RoomID = m_OriRoomID;
			DebugInfo("加入房间成功 RoomID=%d",msgJR.m_RoomID);
		}
	}
	else
	{
		DebugError("OnRespJoinRoom Error Flag=%d",msgJR.m_Flag);
	}

	return 0;
}

int  CTestPlayer::OnRespRoomTableInfo(CRecvMsgPacket& msgPack)
{
	RespRoomTableInfo msgRTI;
	TransplainMsg(msgPack,msgRTI);

	if ( msgRTI.m_Flag != msgRTI.SUCCESS )
	{
		DebugError("请求房间信息失败 RoomID=%d Flag=%d",m_OriRoomID,msgRTI.m_Flag);
	}

	return 0;
}
int  CTestPlayer::OnTableInfo(CRecvMsgPacket& msgPack)
{
	TableInfo msgTI;
	TransplainMsg(msgPack,msgTI);

	return 0;
}
int  CTestPlayer::OnTableInfoList(CRecvMsgPacket& msgPack)
{
	TableInfoList msgTIL;
	TransplainMsg(msgPack,msgTIL);	

	return 0;
}

int CTestPlayer::OnRoomInfoList(CRecvMsgPacket& msgPack)
{
	RoomInfoList msgRIL;
	TransplainMsg(msgPack,msgRIL);

	return 0;
}

int CTestPlayer::OnBatchProtocl(CRecvMsgPacket& msgPack)
{
	PublicXY::BatchProtocol msgBP;
	TransplainMsg(msgPack,msgBP);

	if ( msgBP.m_Flag == msgBP.START)
	{
	}
	else if ( msgBP.m_Flag == msgBP.END )
	{
		if ( msgBP.m_XYID == RoomInfoList::XY_ID )
		{
			//ReqRoomTableInfo msgRTI;
			//msgRTI.m_RoomID = m_OriRoomID;
			//m_clientsocket.SendMsg(msgRTI);
		}
	}

	return 0;
}

int  CTestPlayer::OnRespGameConnect(CRecvMsgPacket& msgPack)
{
	GameLobbyRespPlayerConnect msgGPC;
	TransplainMsg(msgPack,msgGPC);

	if (msgGPC.m_Flag == msgGPC.SUCCESS )
	{
		m_AID             = msgGPC.m_AID;
		m_PID             = msgGPC.m_PID;

		DebugInfo("OnRespGameConnect 验证成功");
	}
	else
	{
		DebugError("验证失败 Flag=%d",msgGPC.m_Flag);
	}

	return 0;
}

int CTestPlayer::OnPlayerData(CRecvMsgPacket& msgPack)
{
	Game_PlayerData msgGPD;
	TransplainMsg(msgPack,msgGPD);

	m_NickName       = msgGPD.m_NickName;
	m_nGameMoney     = msgGPD.m_nGameMoney;

	m_RoomID         = msgGPD.m_RoomID;
	m_TableID        = msgGPD.m_TableID;
	m_SitID          = msgGPD.m_SitID;
	m_PlayerState    = msgGPD.m_PlayerState;

	m_PlayerState = 1;

	DebugInfo("OnPlayerData State=%d RoomID=%d m_TableID=%d m_SitID=%d",m_PlayerState,m_RoomID,m_TableID,m_SitID);

	return 0;
}

int CTestPlayer::OnPlayerDataEX(CRecvMsgPacket& msgPack)
{
	Game_PlayerDataEx msgPDE;
	TransplainMsg(msgPack,msgPDE);

	return 0;
}

int CTestPlayer::OnUserAwardInfoList(CRecvMsgPacket& msgPack)
{
	UserAwardInfoList msgUAIL;
	TransplainMsg(msgPack,msgUAIL);

	m_nGameMoney += msgUAIL.m_nMoney;

	INT64 nMoney = 0;
	for (vector<msgUserAward>::iterator itorAward=msgUAIL.m_listAwardInfo.begin();itorAward!=msgUAIL.m_listAwardInfo.end();++itorAward)
	{
		nMoney += itorAward->m_nMoney;
	}
	assert(nMoney == msgUAIL.m_nMoney);

	return 0;
}

int  CTestPlayer::OnGameRule(CRecvMsgPacket& msgPack)
{
	Game_MsgRule msgGR;
	TransplainMsg(msgPack,msgGR);

	return 0;
}

int CTestPlayer::OnRespHeart(CRecvMsgPacket& msgPack)
{
	PublicXY::Resp_Heart resh;
	TransplainMsg(msgPack,resh);

	PublicXY::Req_Heart reh;
	reh.m_HeatID = resh.m_HeatID;
	m_clientsocket.SendMsg(reh);

	return 0;
}

int CTestPlayer::OnRespKey(CRecvMsgPacket& msgPack)
{
	PublicXY::RespKey respKey;
	TransplainMsg(msgPack,respKey);

	if ( respKey.m_Len == 0 )
	{
		m_clientsocket.SetCrypt(false);
	}
	else
	{
		m_clientsocket.SetCrypt(true);
		m_clientsocket.SetKey(respKey.m_Key,respKey.m_Len);
	}

	GameLobbyPlayerConnect msgGPC;
	msgGPC.m_ClientType = CLIENT_TYPE_PC;
	msgGPC.m_PlayerType = PLAYER_TYPE_PLAYER;
	msgGPC.m_LoginType = Login_Type_CeShi;
	msgGPC.m_AID = m_AID;
	msgGPC.m_PID = m_PID;
	m_clientsocket.SendMsg(msgGPC);

	return 0;
}