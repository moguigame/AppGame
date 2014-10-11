#include ".\readwritedb.h"

#include "Server.h"
#include "CDBSConfig.h"
#include "DBOperatorXY.h"

using namespace std;
using namespace AGBase;

using namespace MoGui::MoGuiXY::RWDB_XY;

int CDBOperator::s_MaxID = 0;

volatile UINT32 CDBOperator::s_WriteOperator = 0;
volatile UINT32 CDBOperator::s_ReadOperator = 0;
UINT32 CDBOperator::s_LastWriteOperator = 0;
UINT32 CDBOperator::s_LastReadOperator = 0;
UINT32 CDBOperator::s_LastTime = 0;

vector<string> CDBOperator::s_vectorFriendTableName;
vector<string> CDBOperator::s_vectorGiftTableName;

void CDBOperator::StaticInit()
{
	for ( int i=0;i<16;i++)
	{
		s_vectorFriendTableName.push_back("friend_" + N2S(i));
		s_vectorGiftTableName.push_back("gift_" + N2S(i));
	}
	s_vectorFriendTableName.push_back("");
	s_vectorGiftTableName.push_back("");

	s_LastTime = int(time(NULL));
}
const std::string& CDBOperator::getGiftTableName(UINT32 PID)
{
	if ( PID > 0 )
		return s_vectorGiftTableName[PID%16];
	else
		return s_vectorGiftTableName[16];
}
const std::string& CDBOperator::getFriendTableName(UINT32 PID)
{
	if ( PID > 0 )	
		return s_vectorFriendTableName[PID%16];	
	else	
		return s_vectorFriendTableName[16];
}
CDBOperator::CDBOperator(CServer* pServer)
{
	m_pServer = NULL;
	if ( pServer )
	{
		m_pServer = pServer;
		m_pConfig = pServer->GetDBConfig();

		m_ID = ++s_MaxID;

		m_CurTime = UINT32(time(NULL));
		m_NewDayTime = UINT32(Tool::GetNewDayTime(m_CurTime));
		m_LastCheckTime = m_CurTime - Tool::CRandom::Random_Int(1,180);

		InitTableName(m_CurTime);

		m_strDBUser = m_pConfig->m_strDBUser;
		otl_connect::otl_initialize();
		try
		{
			m_DBUserConnect.rlogon(m_strDBUser.c_str());	
		}
		catch(otl_exception& p)
		{
			CatchDBException(p);
		}

		m_strDBGame = m_pConfig->m_strDBGame;
		otl_connect::otl_initialize();
		try
		{
			m_DBGameConnect.rlogon(m_strDBGame.c_str());	
		}
		catch(otl_exception& p)
		{
			CatchDBException(p);
		}

		m_strDBLog = m_pConfig->m_strDBLog;
		otl_connect::otl_initialize();
		try
		{
			m_DBLogConnect.rlogon(m_strDBLog.c_str());
		}
		catch(otl_exception& p)
		{
			CatchDBException(p);
		}

		m_mapFaceInfo.clear();
		m_mapGiftInfo.clear();
	}
	else
	{
		DebugError("CDBOperator Init Error");
	}
}

CDBOperator::~CDBOperator(void)
{
	m_DBUserConnect.logoff();
	m_DBGameConnect.logoff();
	m_DBLogConnect.logoff();
}

void CDBOperator::DebugError(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1]={0};
	va_list args;
	va_start(args,logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if( len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_ERROR,logbuf);
		cout<<Tool::GetTimeString(time(nullptr))+" Error "<<logbuf<<endl;
	}
}
void CDBOperator::DebugInfo(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1]={0};
	va_list args;
	va_start(args,logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if( len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_INFO,logbuf);
	}
}

void CDBOperator::InitTableName(UINT32 curTime)
{
	//struct tm Temp_tm;
	//time_t Temp_Time = curTime;
	//Tool::time_to_tm(&Temp_Time,&Temp_tm);

	m_strTableNameWinLoss = "dezhou_winloss_log_" + Tool::GetMonthString_XXXXYY(curTime);
	m_strTableNameMoneyLog = "dezhou_money_log_" + Tool::GetMonthString_XXXXYY(curTime);

	m_strTableNameMatchResult = "dezhou_match_log";
	m_strTableNameMatchWinLoss = "dezhou_matchwinloss_log";

	m_strTableNameChat = "dezhou_chat_log";
	m_strTableNameLogin = "dezhou_login_log";
	m_strTableNameAction = "dezhou_player_action";
}

int CDBOperator::OnActiveDBConnect()
{
	m_CurTime = UINT32(time(NULL));
	if ( m_CurTime >= m_NewDayTime )
	{
		m_NewDayTime = UINT32(Tool::GetNewDayTime(m_CurTime));
		InitTableName(m_CurTime);
	}

	if ( m_CurTime - m_LastCheckTime >= 600 )
	{
		m_LastCheckTime = m_CurTime;

		ActiveDBConnect(m_DBUserConnect,m_strDBUser);
		ActiveDBConnect(m_DBGameConnect,m_strDBGame);
		ActiveDBConnect(m_DBLogConnect,m_strDBLog);

		if ( N_CeShiLog::c_FuncTimeLog )
		{
			VectorFuncTime vectorFT;
			MapFuncTime& rTempMapFT = m_FuncTime.m_mapFuncTime;
			for ( MapFuncTime::iterator itorFT=rTempMapFT.begin();itorFT!=rTempMapFT.end();++itorFT)
			{
				vectorFT.push_back(itorFT->second);
			}
			sort(vectorFT.begin(),vectorFT.end(),CCompareFuncTime());
			DebugInfo("CDBOperator Func Count=%d",vectorFT.size());
			for ( size_t nPos=0;nPos<vectorFT.size();++nPos)
			{
				const stFuncTimeLog& rFT = vectorFT[nPos];
				DebugInfo("ID=%-2d %-30s TotalTicket=%-10lld Times=%-8lld Average=%-8lld Max=%-8lld Min=%-5lld",
					m_ID,
					rFT.m_strFuncName.c_str(),
					rFT.m_TotalTicket/1000,
					rFT.m_nTimes,					
					rFT.m_TotalTicket/max(rFT.m_nTimes,1),
					rFT.m_MaxTicket,
					rFT.m_MinTicket );
			}
		}
	}

	return 0;
}

void CDBOperator::ActiveDBConnect(otl_connect& DBConnect,std::string& strDB)
{
	try
	{
		otl_stream TempDBStream(1,"select 1",DBConnect,otl_implicit_select);
		TempDBStream.close();
	}
	catch(otl_exception& pp)
	{
		CatchDBException(pp);
		CheckOTLException(pp,DBConnect,strDB);
	}
}

void CDBOperator::DoConnect(otl_connect& DBConnect,std::string& strDB)
{
	try
	{
		DBConnect.logoff();
		DBConnect.rlogon(strDB.c_str());
	}
	catch(otl_exception& pp)
	{
		CatchDBException(pp);
	}
}

void CDBOperator::CheckOTLException(const otl_exception &p,otl_connect& DBConnect,std::string& strDB)
{
	if ( p.code == 2006 || p.code == 0 )
	{
		DoConnect(DBConnect,strDB);
	}
}

void CDBOperator::CheckDBConnect(otl_connect& DBConnect,std::string& strDB)
{
	if ( !DBConnect.connected )
	{
		DoConnect(DBConnect,strDB);
	}
}

void CDBOperator::CatchDBException(const otl_exception &p)
{
	print_otl_error(p);
}
void CDBOperator::print_otl_error(const otl_exception &p)
{
	char msg[2048] = {0};
	_snprintf(msg, sizeof(msg)-1, "msg=%s\n", p.msg);									// print out error message
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "stm_text=%s\n", p.stm_text);	// print out SQL that caused the error
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "sqlstate=%s\n", p.sqlstate);	// print out SQLSTATE message
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "var_info=%s\n", p.var_info);	// print out the variable that caused the error	
	cout<<"DBError "<<Tool::GetTimeString(time(NULL))<<" "<<msg<<endl;
	DebugError("CDBOperator %s",msg);
}

int CDBOperator::OnRWDBMsg(ReadWriteDBMessage* pMsg)
{
	InterlockedIncrement(&s_WriteOperator);
	
	int ret = 0;
	switch(pMsg->m_MsgID)
	{
	case RWDB_WinLossLog::XY_ID:
		{
			ret = OnWinLossMsg(pMsg);
		}
		break;
	case RWDB_MatchWinLoss::XY_ID:
		{
			ret = OnMatchWinLoss(pMsg);
		}
		break;
	case RWDB_MatchResultLog::XY_ID:
		{
			ret = OnMatchResult(pMsg);
		}
		break;
	case RWDB_UpdateUserGameMoneyType::XY_ID:
		{
			ret = OnUpdateUserGameMoneyType(pMsg);
		}
		break;
	case RWDB_MaxPai::XY_ID:
		{
			ret = OnMaxPai(pMsg);
		}
		break;
	case RWDB_MaxWin::XY_ID:
		{
			ret = OnMaxWin(pMsg);
		}
		break;
	case RWDB_MaxMoney::XY_ID:
		{
			ret = OnMaxMoney(pMsg);
		}
		break;
	case RWDB_AddGameMoney::XY_ID:
		{
			ret = OnAddGameMoney(pMsg);
		}
		break;
	case RWDB_WriteUserGift::XY_ID:
		{
			ret = OnWriteUserGift(pMsg);
		}
		break;
	case RWDB_AddLimite::XY_ID:
		{
			ret = OnAddLimite(pMsg);
		}
		break;
	case RWDB_GameRight::XY_ID:
		{
			ret = OnGameRight(pMsg);
		}
		break;
	case RWDB_DeleteCreateInfo::XY_ID:
		{
			ret = OnDeleteCreateInfo(pMsg);
		}
		break;
	case RWDB_UpdateHuiYuan::XY_ID:
		{
			ret = OnUpdateHuiYuan(pMsg);
		}
		break;
	case RWDB_ChangeBankLog::XY_ID:
		{
			ret = OnChangeBankLog(pMsg);
		}
		break;
	case RWDB_AddPlayerAward::XY_ID:
		{
			ret = OnAddPlayerAward(pMsg);
		}
		break;
	case RWDB_MatchInfo::XY_ID:
		{
			ret = OnMatchInfo(pMsg);
		}
		break;
	case RWDB_WinLossInfo::XY_ID:
		{
			ret = OnWinLossInfo(pMsg);
		}
		break;
	case RWDB_MakeFriend::XY_ID:
		{
			ret = OnMakeFriend(pMsg);
		}
		break;
	case RWDB_IncomeAndPay::XY_ID:
		{
			ret = OnIncomAndPay(pMsg);
		}
		break;
	case RWDB_TableInfo::XY_ID:
		{
			ret = OnTableInfo(pMsg);
		}
		break;
	case RWDB_UpdateGiftInfo::XY_ID:
		{
			ret = OnUpdateGiftInfo(pMsg);
		}
		break;
	case RWDB_MoGuiMoneyLog::XY_ID:
		{
			ret = OnMoGuiMoneyLog(pMsg);
		}
		break;
	case RWDB_UpdateAwardInfo::XY_ID:
		{
			ret = OnUpdateAwardInfo(pMsg);
		}
		break;
	case RWDB_UpdateRightInfo::XY_ID:
		{
			ret = OnUpdateRightInfo(pMsg);
		}
		break;
	case RWDB_WriteRightInfo::XY_ID:
		{
			ret = OnWriteRightInfo(pMsg);
		}
		break;
	case RWDB_UpdateAchieve::XY_ID:
		{
			ret = OnUpdateAchieve(pMsg);
		}
		break;
	case RWDB_ChangeUserInfo::XY_ID:
		{
			ret = OnChangeUserInfo(pMsg);
		}
		break;
	case RWDB_PlayerLogin::XY_ID:
		{
			ret = OnPlayerLogin(pMsg);
		}
		break;
	case RWDB_CreateGameInfo::XY_ID:
		{
			ret = OnCreateGameInfo(pMsg);
		}
		break;
	case RWDB_ReportPlayerOnline::XY_ID:
		{
			ret = OnReportPlayerOnline(pMsg);
		}
		break;
	case RWDB_PlayerActionLog::XY_ID:
		{
			ret = OnPlayerActionLog(pMsg);
		}
		break;
	case RWDB_GameError::XY_ID:
		{
			ret = OnGameError(pMsg);
		}
		break;
	case RWDB_ChatLog::XY_ID:
		{
			ret = OnChatLog(pMsg);
		}
		break;
	case RWDB_UpdateWinType::XY_ID:
		{
			ret = OnUpdateWinType(pMsg);
		}
		break;
	case RWDB_AddPlayerInfo::XY_ID:
		{
			ret = OnAddPlayerInfo(pMsg);
		}
		break;
	case RWDB_HonorLog::XY_ID:
		{
			ret = OnHonorLog(pMsg);
		}
		break;
	default:
		DebugError("OnRWDBMsg XYID=%d XYLen=%d",pMsg->m_MsgID,pMsg->m_MsgLen);
		break;
	}
	return 0;
}

int CDBOperator::OnWinLossMsg(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnWinLossMsg");

	RWDB_WinLossLog msgWLL;
	ExplainRWDBMsg(*pMsg,msgWLL);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into " + m_strTableNameWinLoss + 
			" (GameGUID,AID,PID,RoomID,TableID,SitID,WinLoss,Service,Flag,LeftPai,RightPai,PaiType,DateTime) \
			values(:f1<bigint>,:f9<short>,:f2<unsigned int>,:f7<short>,:f8<short>,:f21<short>,:f3<bigint>,:f4<int>,:f5<short>,:f22<short>,:f23<short>,:f24<short>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgWLL.m_GameGUID
			<<msgWLL.m_AID
			<<msgWLL.m_PID
			<<msgWLL.m_RoomID
			<<msgWLL.m_TableID
			<<msgWLL.m_SitID
			<<msgWLL.m_nWinLossMoney
			<<msgWLL.m_nServiceMoney
			<<msgWLL.m_EndGameFlag
			<<msgWLL.m_LeftPai
			<<msgWLL.m_RightPai
			<<msgWLL.m_PaiType;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnMatchResult(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMatchResult");

	RWDB_MatchResultLog msgMRL;
	ExplainRWDBMsg(*pMsg,msgMRL);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into " + m_strTableNameMatchResult + " (MatchGUID,AID,PID,RoomID,TableID,MatchID,Position,AwardMoney,AwardJF,Ticket,TakeMoney,DateTime) \
				values(:f1<bigint>,:f9<short>,:f2<unsigned int>,:f7<short>,:f8<short>,:f21<int>,:f22<int>,:f23<bigint>,:f34<bigint>,:f24<int>,:f25<int>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgMRL.m_MatchGUID
			<<msgMRL.m_AID
			<<msgMRL.m_PID
			<<msgMRL.m_RoomID
			<<msgMRL.m_TableID
			<<msgMRL.m_MatchID
			<<msgMRL.m_Position
			<<msgMRL.m_AwardMoney
			<<msgMRL.m_AwardJF
			<<msgMRL.m_Ticket
			<<msgMRL.m_TakeMoney;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnMatchWinLoss(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMatchWinLoss");

	RWDB_MatchWinLoss msgMWL;
	ExplainRWDBMsg(*pMsg,msgMWL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into " + m_strTableNameMatchWinLoss + " (MatchGUID,GameGUID,AID,PID,RoomID,TableID,SitID,WinLoss,EndFlag,PaiType,DateTime) \
						values(:f1<bigint>,:f2<bigint>,:f3<short>,:f4<unsigned int>,:f5<short>,:f6<short>,:f7<short>,:f8<int>,:f9<short>,:f10<short>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgMWL.m_MatchGUID
			<<msgMWL.m_GameGUID
			<<msgMWL.m_AID
			<<msgMWL.m_PID
			<<msgMWL.m_RoomID
			<<msgMWL.m_TableID
			<<short(msgMWL.m_SitID)
			<<msgMWL.m_nWinLossMoney
			<<msgMWL.m_EndGameFlag
			<<msgMWL.m_PaiType;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::OnMaxPai(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMaxPai");

	RWDB_MaxPai msgMP;
	ExplainRWDBMsg(*pMsg,msgMP);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfoex set MaxPai=:f1<bigint>,MaxPaiTime=:f2<unsigned int> where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgMP.m_MaxPai<<msgMP.m_MaxPaiTime<<msgMP.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnMaxWin(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMaxWin");

	RWDB_MaxWin msgMW;
	ExplainRWDBMsg(*pMsg,msgMW);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfoex set MaxWin=:f1<bigint>,MaxWinTime=:f2<unsigned int> where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgMW.m_MaxWin<<msgMW.m_MaxWinTime<<msgMW.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnMaxMoney(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMaxMoney");

	RWDB_MaxMoney msgMM;
	ExplainRWDBMsg(*pMsg,msgMM);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfoex set MaxMoney=:f1<bigint>,MaxMoneyTime=:f2<unsigned int> where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgMM.m_MaxMoney<<msgMM.m_MaxMoneyTime<<msgMM.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnAddGameMoney(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnAddGameMoney");

	RWDB_AddGameMoney msgAGM;
	ExplainRWDBMsg(*pMsg,msgAGM);

	short AID       = msgAGM.m_AID;
	UINT32 PID      = msgAGM.m_PID;
	INT64 nAddMoney = msgAGM.m_GameMoney;
	INT64 nHongBao  = msgAGM.m_HongBao;

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);

	if ( nAddMoney==0 && nHongBao==0 ) return ret;
	try
	{
		int RpcLine = 0;
		if( nHongBao == 0 )
		{
			string strSQL = "update dezhou_money set GameMoney=GameMoney+:f1<bigint> where PID=:f102<unsigned int> and GameMoney+:f101<bigint> >= 0";
			otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
			TempDBStream<<nAddMoney<<PID<<nAddMoney;
			TempDBStream.flush();
			RpcLine = TempDBStream.get_rpc();
		}
		else if( nAddMoney == 0 )
		{
			string strSQL = "update dezhou_money set HongBao=HongBao+:f1<bigint> where PID=:f102<unsigned int> ";
			otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
			TempDBStream<<nHongBao<<PID;
			TempDBStream.flush();
			RpcLine = TempDBStream.get_rpc();
		}
		else
		{
			string strSQL = "update dezhou_money set GameMoney=GameMoney+:f1<bigint>,HongBao=HongBao+:f2<bigint> \
							  where PID=:f102<unsigned int> and GameMoney+:f101<bigint> >= 0";
			otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
			TempDBStream<<nAddMoney<<nHongBao<<PID<<nAddMoney;
			TempDBStream.flush();
			RpcLine = TempDBStream.get_rpc();
		}	

		if ( RpcLine == 0 )
		{
			stDBGameMoney stGM;
			if ( ReadUserGameMoney(AID,PID,stGM) == DB_RESULT_SUCCESS )
			{
				RWDB_GameError rwdbGE;
				rwdbGE.m_AID = AID;
				rwdbGE.m_PID = PID;
				rwdbGE.m_Flag = GameError_AddMoney;
				rwdbGE.m_Des = "AddMoney=" + N2S(nAddMoney) + " CurMoney=" + N2S(stGM.m_GameMoney) + " AddHaoBao=" + N2S(nHongBao) + " CurHaoBao=" + N2S(stGM.m_HongBao);
				rwdbGE.m_Key = "OnAddGameMoney";
				CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
				if ( pMsgManager ){
					pMsgManager->PushRWDBMsg(rwdbGE);
				}

				DebugError("OnAddGameMoney AID=%d PID=%d nAddMoney=%s CurMoney=%s AddHongBao=%s CurHongBao=%s",
					AID,PID,N2S(nAddMoney).c_str(),N2S(stGM.m_GameMoney).c_str(),N2S(nHongBao).c_str(),N2S(stGM.m_HongBao).c_str() );
			}
		}
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::OnWriteUserGift(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnWriteUserGift");

	RWDB_WriteUserGift msgWUG;
	ExplainRWDBMsg(*pMsg,msgWUG);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		std::string strGiftTableName = getGiftTableName(msgWUG.m_RecvPID);
		if (strGiftTableName.length() > 0)
		{
			string strSQL = "insert into " + strGiftTableName +
				" (RecvPID,SendPID,GiftID,Price,Flag,Name,CreateTime) \
				values(:f1<unsigned int>,:f2<unsigned int>,:f3<short>,:f4<int>,:f5<short>,:f6<char[50]>,now()) ";
			otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
			TempDBStream<<msgWUG.m_RecvPID<<msgWUG.m_SendPID<<msgWUG.m_GiftID<<msgWUG.m_Price<<short(N_Gift::GiftST_OnSale)<<msgWUG.m_NickName;
			TempDBStream.close();

			strSQL = "select @@IDENTITY";
			//strSQL = "select SCOPE_IDENTITY()";
			TempDBStream.open(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
			if( !TempDBStream.eof() )
			{
				TempDBStream >> msgWUG.m_GiftIdx;

				stDBUserGiftInfo stDBUGI;
				stDBUGI.m_RecvPID         = msgWUG.m_RecvPID;
				stDBUGI.m_GiftID          = msgWUG.m_GiftID;
				stDBUGI.m_GiftIdx         = msgWUG.m_GiftIdx;
				stDBUGI.m_Price           = msgWUG.m_Price;
				stDBUGI.m_SendPID         = msgWUG.m_SendPID;
				stDBUGI.m_NickName        = msgWUG.m_NickName;
				stDBUGI.m_ActionTime      = msgWUG.m_ActionTime;				
				m_pServer->PushUserGift(stDBUGI);
			}
		}
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::OnAddLimite(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnAddLimite");

	RWDB_AddLimite msgAL;
	ExplainRWDBMsg(*pMsg,msgAL);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfo set \
						TopLimite = TopLimite + :f1<bigint>,\
						LowLimite = LowLimite + :f2<bigint> \
						where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgAL.m_nUpperLimite<<msgAL.m_nLowerLimite<<msgAL.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnAddPlayerAward(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnAddPlayerAward");

	RWDB_AddPlayerAward msgAPA;
	ExplainRWDBMsg(*pMsg,msgAPA);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{			
		otl_datetime OriginOTLTime = Tool::GetOTLTimeFromCurTime(msgAPA.m_EndTime);
		string strOriginAward     = "insert into dezhou_award (PID,AID,Money,MoneyFlag,PayMode,CanUse,EndTime,Log) \
									values(:f1<unsigned int>,:f2<short>,:f3<bigint>,:f4<short>,:f5<short>,:f6<short>,:f7<timestamp>,:f8<char[250]> )";
		otl_stream AwardOriginStream( OTL_STREAMBUF_SIZE,strOriginAward.c_str(),m_DBGameConnect );
		AwardOriginStream<<msgAPA.m_PID<<msgAPA.m_AID<<msgAPA.m_Money<<msgAPA.m_nFlag<<msgAPA.m_PayMode<<msgAPA.m_CanUse<<OriginOTLTime<<msgAPA.m_strLog;
		AwardOriginStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnUpdateHuiYuan(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateHuiYuan");

	RWDB_UpdateHuiYuan msgUHY;
	ExplainRWDBMsg(*pMsg,msgUHY);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfoex set FreeFaceTime=:f1<unsinged int>,VipLevel=:f2<short>,\
						VipEndTime=:f3<unsigned int> where PID=:f101<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgUHY.m_FreeFaceTime<<msgUHY.m_VipLevel<<msgUHY.m_VipEndTime<<msgUHY.m_PID;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			DebugError("OnUpdateHuiYuan PID=%d FreeFaceTime=%d VipLevel=%d VipEndTime=%d",
				msgUHY.m_PID,msgUHY.m_FreeFaceTime,msgUHY.m_VipLevel,msgUHY.m_VipEndTime);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnDeleteCreateInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnDeleteCreateInfo");

	RWDB_DeleteCreateInfo msgDCI;
	ExplainRWDBMsg(*pMsg,msgDCI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);

	try
	{
		string strSQL = "delete from dezhou_award where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgDCI.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	try
	{
		string strSQL = "delete from dezhou_right where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgDCI.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnGameRight(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnGameRight");

	RWDB_GameRight msgGR;
	ExplainRWDBMsg(*pMsg,msgGR);

	if ( msgGR.m_Right < 0 )
	{
		DebugError("OnUpdateRightInfo AID=%d PID=%d GameRight=%d",msgGR.m_AID,msgGR.m_PID,msgGR.m_Right);
		msgGR.m_Right = 0;
	}

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfo set GameRight = :f1<bigint> where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgGR.m_Right<<msgGR.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnMatchInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMatchInfo");

	RWDB_MatchInfo msgMI;
	ExplainRWDBMsg(*pMsg,msgMI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_matchinfo set \
						TJTimes    = :f1<int>,\
						TJWin      = :f2<bigint>,\
						TJBest     = :f3<int>,\
						TJBestTime = :f4<unsigned int>,\
						JBTimes    = :f5<int>,\
						JBWin      = :f6<bigint>,\
						JBBest     = :f7<int>,\
						JBBestTime = :f8<unsigned int>,\
						GJTimes    = :f9<int>,\
						GJWin      = :f10<bigint>,\
						GJBest     = :f11<int>,\
						GJBestTime = :f12<unsigned int> \
						where PID=:f99<unsigned int> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgMI.m_TaoJinTimes   <<msgMI.m_TaoJinWinMoney   <<msgMI.m_TaoJinBest   <<msgMI.m_TaoJinBestTime
			        <<msgMI.m_JingBiaoTimes <<msgMI.m_JingBiaoWinMoney <<msgMI.m_JingBiaoBest <<msgMI.m_JingBiaoBestTime
					<<msgMI.m_GuanJunTimes  <<msgMI.m_GuanJunWinMoney  <<msgMI.m_GuanJunBest  <<msgMI.m_GuanJunBestTime
					<<msgMI.m_PID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnWinLossInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnWinLossInfo");

	RWDB_WinLossInfo msgWT;
	ExplainRWDBMsg(*pMsg,msgWT);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfo set \
						WinTimes=WinTimes+:f1<int>, \
						LossTimes=LossTimes+:f2<int>, \
						JF=JF+:f3<bigint>, \
						EP=EP+:f4<bigint>, \
						TopLimite=TopLimite+:f7<bigint>,\
						LowLimite=LowLimite+:f8<bigint> \
						where PID=:f99<unsigned int>";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
		TempDBStream<<msgWT.m_WinTimes<<msgWT.m_LossTimes<<msgWT.m_JF<<msgWT.m_EP<<msgWT.m_nUpLimite<<msgWT.m_nLowLimite<<msgWT.m_PID;
		TempDBStream.flush();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnUpdateWinType(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateWinType");

	RWDB_UpdateWinType msgAWT;
	ExplainRWDBMsg(*pMsg,msgAWT);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfoex set \
						One     =:f1<int>,\
						Two     =:f2<int>,\
						TwoTwo  =:f3<int>,\
						Three   =:f4<int>,\
						Str     =:f5<int>,\
						Hua     =:f6<int>,\
						ThreeTwo=:f7<int>,\
						Four    =:f8<int>,\
						HuaStr  =:f9<int>,\
						King    =:f10<int>\
						where PID=:f102<unsigned int>";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<msgAWT.m_WinRecord[0]<<msgAWT.m_WinRecord[1]<<msgAWT.m_WinRecord[2]<<msgAWT.m_WinRecord[3]<<msgAWT.m_WinRecord[4]
		            <<msgAWT.m_WinRecord[5]<<msgAWT.m_WinRecord[6]<<msgAWT.m_WinRecord[7]<<msgAWT.m_WinRecord[8]<<msgAWT.m_WinRecord[9]
		            <<msgAWT.m_PID;
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::OnAddPlayerInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnAddPlayerInfo");

	RWDB_AddPlayerInfo msgAPI;
	ExplainRWDBMsg(*pMsg,msgAPI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);

	try
	{
		switch(msgAPI.m_Flag)
		{
		case msgAPI.ShowFace:
			{
				string strSQL = "update dezhou_gameinfoex set ShowFace=ShowFace+1 where PID=:f101<unsigned int>";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.WinTimes:
			{
				string strSQL = "update dezhou_gameinfo set WinTimes=WinTimes+1 where PID=:f101<unsigned int>";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.LossTimes:
			{
				string strSQL = "update dezhou_gameinfo set LossTimes=LossTimes+1 where PID=:f101<unsigned int>";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.GameTime:
			{
				string strSQL = "update dezhou_gameinfo set GameTime=GameTime+:f1<int> where PID=:f101<unsigned int>";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<int(msgAPI.m_AddValue)<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.InvitePlayer:
			{
				string strSQL = "update dezhou_gameinfoex set TotalInvite = TotalInvite+1 where PID=:f101<unsigned int> ";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.OpenBank:
			{
				string strSQL = "update dezhou_gameinfo set OpenBank=:f1<short> where PID=:f102<unsigned int> ";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<short(msgAPI.m_AddValue)<<msgAPI.m_PID;
				TempDBStream.close();			
			}
			break;
		case msgAPI.LandTime:
			{
				string strSQL = "update dezhou_gameinfo set LandTime=now() where PID=:f102<unsigned int> ";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		default:
			DebugError("OnAddPlayerInfo AID=%d PID=%d Flag=%d Value=%d",msgAPI.m_AID,msgAPI.m_PID,msgAPI.m_Flag,msgAPI.m_AddValue);
		}
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnUpdateUserGameMoneyType(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateUserGameMoneyType");

	RWDB_UpdateUserGameMoneyType msgUUGM;
	ExplainRWDBMsg(*pMsg,msgUUGM);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_gameinfoex set \
						TotalWin=TotalWin+:f2<bigint>,\
						TotalLoss=TotalLoss+:f3<bigint>,\
						TotalService=TotalService+:f4<bigint>,\
						TotalBeGive=TotalBeGive+:f5<bigint>,\
						TotalIncome=TotalIncome+:f6<bigint>,\
						TotalExpend=TotalExpend+:f7<bigint> \
						where PID=:f102<unsigned int> and AID=:f100<short> ";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);

		TempDBStream<<msgUUGM.m_nWinMoney
			<<msgUUGM.m_nLossMoney
			<<msgUUGM.m_nServiceMoney
			<<msgUUGM.m_nGivenMoney
			<<msgUUGM.m_nIncomeMoney
			<<msgUUGM.m_nExpendMoney
			<<msgUUGM.m_PID
			<<msgUUGM.m_AID;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnMakeFriend(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMakeFriend");

	RWDB_MakeFriend msgMF;
	ExplainRWDBMsg(*pMsg,msgMF);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		otl_stream TempDBStream;
		string strSQL = "update " + getFriendTableName(msgMF.m_LeftPID) + " set Flag=:f1<short> where LeftPID=:f2<unsinged int> and RightPID=:f3<unsinged int>";
		TempDBStream.open(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		TempDBStream <<msgMF.m_Flag<<msgMF.m_LeftPID<<msgMF.m_RightPID;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			otl_stream DBStream;
			strSQL = "insert into " + getFriendTableName(msgMF.m_LeftPID) + " (LeftPID,RightPID,Flag) values(:f1<unsigned int>,:f2<unsigned int>,:f3<short>) ";
			DBStream.open(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
			DBStream <<msgMF.m_LeftPID<<msgMF.m_RightPID<<msgMF.m_Flag;
			DBStream.close();
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnChangeBankLog(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnChangeBankLog");

	RWDB_ChangeBankLog msgCBL;
	ExplainRWDBMsg(*pMsg,msgCBL);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into dezhou_bank_log (PID,AID,Money,Flag,LogTime) \
						values(:f1<unsigned int>,:f11<short>,:f2<bigint>,:f3<short>,now())";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect);
		TempDBStream<<msgCBL.m_PID<<msgCBL.m_AID<<msgCBL.m_Money<<msgCBL.m_Flag;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::OnIncomAndPay(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnIncomAndPay");

	RWDB_IncomeAndPay msgInPay;
	ExplainRWDBMsg(*pMsg,msgInPay);

	if ( !DezhouLib::IsValidPayIncomeFlag(msgInPay.m_Flag) || msgInPay.m_nMoney==0 )
	{
		DebugError("OnIncomAndPay PID=%d Flag=%d Money=%s",msgInPay.m_PID,msgInPay.m_Flag,N2S(msgInPay.m_nMoney).c_str());
	}
	
	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{		
		string strSQL = "insert into " + m_strTableNameMoneyLog + " (PID,AID,Money,Flag,Log,ActionTime) \
						values(:f1<unsigned int>,:f11<short>,:f2<bigint>,:f3<short>,:f5<char[255]>,now())";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect);
		TempDBStream<<msgInPay.m_PID<<msgInPay.m_AID<<msgInPay.m_nMoney<<msgInPay.m_Flag<<msgInPay.m_strReMark;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnHonorLog(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnHonorLog");

	RWDB_HonorLog msgHL;
	ExplainRWDBMsg(*pMsg,msgHL);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{		
		string strSQL = "insert into dezhou_honor_log (AID,PID,HonorID,Money,LogTime) \
						values(:f1<short>,:f2<unsigned int>,:f3<int>,:f4<bigint>,now())";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect);
		TempDBStream<<msgHL.m_AID<<msgHL.m_PID<<msgHL.m_HonorID<<msgHL.m_Money;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnTableInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnTableInfo");

	RWDB_TableInfo msgTI;
	ExplainRWDBMsg(*pMsg,msgTI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		string TableName = "dezhou_tableprivate_" + Tool::N2S(msgTI.m_ServerID);
		otl_datetime TempOTLTime = Tool::GetOTLTimeFromCurTime(msgTI.m_EndTime);
		int Idx = msgTI.m_RoomID*10000+msgTI.m_TableID;

		string strSQL = "update " + TableName + " set \
						TableID     = :f1<short>,    \
						TableName   = :f2<char[50]>, \
						PassWord    = :f3<char[10]>, \
						TableRule   = :f4<char[255]>, \
						TableRuleEX = :f5<char[255]>, \
						MatchRule   = :f25<char[255]>,\
						FoundByWho  = :f6<unsigned int>, \
						EndTime     = :f7<timestamp>\
						where Idx=:f8<int> ";

		otl_stream DBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		DBStream<<msgTI.m_TableID<<msgTI.m_TableName<<msgTI.m_Password<<msgTI.m_TableRule
			<<msgTI.m_TableRuleEX<<msgTI.m_TableMatchRule<<msgTI.m_FoundByWho<<TempOTLTime<<Idx;
		DBStream.flush();

		if ( DBStream.get_rpc() == 0 )
		{
			string TableName = "dezhou_tableprivate_" + Tool::N2S(msgTI.m_ServerID);
			strSQL = "insert into " + TableName + " (Idx,ServerID,RoomID,TableID,TableName,PassWord,TableRule,TableRuleEX,MatchRule,FoundByWho,EndTime) "
				+ " values(:f1<int>,:f2<short>,:f3<short>,:f4<short>,:f5<char[50]>,:f6<char[10]>,:f7<char[255]>,:f8<char[255]>,:f28<char[255]>,:f9<unsigned int>,:f10<timestamp> ) ";
			otl_stream tempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
			tempDBStream<<Idx<<msgTI.m_ServerID<<msgTI.m_RoomID<<msgTI.m_TableID<<msgTI.m_TableName<<msgTI.m_Password
				<<msgTI.m_TableRule<<msgTI.m_TableRuleEX<<msgTI.m_TableMatchRule<<msgTI.m_FoundByWho<<TempOTLTime;
		}

		DBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnUpdateGiftInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateGiftInfo");

	RWDB_UpdateGiftInfo msgUGI;
	ExplainRWDBMsg(*pMsg,msgUGI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		std::string strSQL = "update " + getGiftTableName(msgUGI.m_PID) + " set Flag=:f1<short> where Idx=:f2<int>";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		TempDBStream<<msgUGI.m_Flag<<msgUGI.m_GiftIdx;

		TempDBStream.close();		
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBOperator::UpdateChangeBankMoney(INT16 AID,UINT32 PID,INT64 nGameMoney,INT64 nBankMoney)
{
	CLogFuncTime lft(m_FuncTime,"UpdateChangeBankMoney");

	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_money set GameMoney=GameMoney+:f1<bigint>,BankMoney=BankMoney+:f2<bigint> \
						where PID=:f102<unsigned int> and GameMoney+:f103<bigint> >= 0 and BankMoney+:f104<bigint> >= 0";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<nGameMoney<<nBankMoney<<PID<<nGameMoney<<nBankMoney;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			DebugError("UpdateChangeBankMoney AID=%d PID=%d nGameMoney=%s nBankMoney=%s",
				AID,PID,Tool::N2S(nGameMoney).c_str(),Tool::N2S(nBankMoney).c_str() );
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::OnMoGuiMoneyLog(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMoGuiMoneyLog");

	RWDB_MoGuiMoneyLog msgMML;
	ExplainRWDBMsg(*pMsg,msgMML);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into dezhou_mogui_log (PID,AID,Money,Flag,Log,ActionTime) \
						values(:f1<unsigned int>,:f2<short>,:f4<int>,:f5<short>,:f6<char[255]>,now())";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect);
		TempDBStream<<msgMML.m_PID<<msgMML.m_AID<<msgMML.m_Money<<msgMML.m_Flag<<msgMML.m_strLog;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnUpdateAwardInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateAwardInfo");

	RWDB_UpdateAwardInfo msgUAI;
	ExplainRWDBMsg(*pMsg,msgUAI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		std::string strSQL = "update dezhou_award set CanUse=:f1<short> where Idx=:f2<int>";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
		TempDBStream<<msgUAI.m_Flag<<msgUAI.m_AwardIdx;

		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnUpdateRightInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateRightInfo");

	RWDB_UpdateRightInfo msgRI;
	ExplainRWDBMsg(*pMsg,msgRI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		std::string strSQL = "update dezhou_right set CanUse=0 where Idx=:f2<int>";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
		TempDBStream<<msgRI.m_RightIdx;

		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnWriteRightInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnWriteRightInfo");

	RWDB_WriteRightInfo msgWRI;
	ExplainRWDBMsg(*pMsg,msgWRI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		otl_datetime TempOTLTime = Tool::GetOTLTimeFromCurTime(msgWRI.m_EndTime);
		std::string strSQL = "insert into dezhou_right (AID,PID,GameRight,Flag,CanUse,EndTime) \
							 values(:f1<short>,:f2<unsigned int>,:f3<int>,:f4<short>,:f5<short>,:f6<timestamp>) ";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
		TempDBStream<<msgWRI.m_AID<<msgWRI.m_PID<<msgWRI.m_Right<<msgWRI.m_Times<<msgWRI.m_CanUse<<TempOTLTime;

		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBOperator::OnUpdateAchieve(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateAchieve");

	RWDB_UpdateAchieve msgUA;
	ExplainRWDBMsg(*pMsg,msgUA);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		if ( DezhouLib::IsValidType(msgUA.m_HonorType) )
		{
			string strAchieve = "Achieve_" + N2S(msgUA.m_HonorType);
			string strSQL = "update dezhou_gameinfoex set " + strAchieve + "=" + strAchieve + "|:f1<int> where PID=:f2<unsigned int>";
			otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
			TempDBStream<<msgUA.m_HonorValue<<msgUA.m_PID;
			TempDBStream.close();
		}		
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::OnChangeUserInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnChangeUserInfo");

	RWDB_ChangeUserInfo msgCN;
	ExplainRWDBMsg(*pMsg,msgCN);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		{
			std::string strSQL = "update user_root set NickName=:f1<char[255]>,InfoFlag=0 where PID=:f2<unsigned int>";
			otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
			TempDBStream<<msgCN.m_NickName<<msgCN.m_PID;
			TempDBStream.flush();

			if ( TempDBStream.get_rpc() == 0 )
			{
				DebugError("OnChangeUserInfo PID=%d NickName=%s",msgCN.m_PID,msgCN.m_NickName.c_str());
			}
			TempDBStream.close();
		}

		{
			std::string strSQL = "update user_root_ex set Sex=:f1<short>,HeadPicURL=:f3<char[255]>,City=:f4<char[255]> where PID=:f21<unsigned int>";
			otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
			TempDBStream<<short(msgCN.m_Sex)<<msgCN.m_HeadPicUrl<<msgCN.m_City<<msgCN.m_PID;
			TempDBStream.close();
		}		
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::OnPlayerLogin(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnPlayerLogin");

	RWDB_PlayerLogin msgPL;
	ExplainRWDBMsg(*pMsg,msgPL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into " + m_strTableNameLogin + " (AID,PID,IP,LandTime) values(:f10<short>,:f11<unsigned int>,:f12<char[20]>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgPL.m_AID<<msgPL.m_PID<<msgPL.m_IP;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::OnCreateGameInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnCreateGameInfo");

	RWDB_CreateGameInfo msgCGI;
	ExplainRWDBMsg(*pMsg,msgCGI);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "update user_dezhou set GameInfo=GameInfo+1 where PID=:f2<unsigned int>";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<msgCGI.m_PID;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			DebugError("OnCreateGameInfo AID=%d PID=%d",msgCGI.m_AID,msgCGI.m_PID);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::OnReportPlayerOnline(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnReportPlayerOnline");

	RWDB_ReportPlayerOnline msgRPOL;
	ExplainRWDBMsg(*pMsg,msgRPOL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into dezhou_online (ServerID,AID,Total,InLobby,InTable,InGame,InMatch,LogTime) \
							 values(:f10<short>,:f21<short>,:f11<short>,:f12<short>,:f23<short>,:f13<short>,:f14<short>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgRPOL.m_ServerID<<msgRPOL.m_AID<<msgRPOL.m_Total<<msgRPOL.m_Lobby<<msgRPOL.m_Table<<msgRPOL.m_Game<<msgRPOL.m_Match;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::OnChatLog(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnChatLog");

	RWDB_ChatLog msgCL;
	ExplainRWDBMsg(*pMsg,msgCL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into " + m_strTableNameChat + " (ServerID,RoomID,TableID,SitID,ChatPID,ToPID,Msg,ActionTime) \
							 values(:f1<short>,:f2<short>,:f3<short>,:f4<short>,:f5<unsigned int>,:f6<unsigned int>,:f7<char[255]>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgCL.m_ServerID<<msgCL.m_RoomID<<msgCL.m_TableID<<short(msgCL.m_SitID)<<msgCL.m_PID<<msgCL.m_ToPID<<msgCL.m_ChatMsg;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::OnPlayerActionLog(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnPlayerActionLog");

	RWDB_PlayerActionLog msgPAL;
	ExplainRWDBMsg(*pMsg,msgPAL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into " + m_strTableNameAction + " (AID,PID,EnterRoom,EnterTable,SitDown,PlayTimes,GameTime,MatchTime,TotalTime,LogTime) \
							 values(:f1<short>,:f2<unsigned int>,:f10<short>,:f11<short>,:f12<short>,:f13<short>,:f23<int>,:f24<int>,:f25<int>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgPAL.m_AID<<msgPAL.m_PID
			<<msgPAL.m_EnterRoomTimes<<msgPAL.m_EnterTableTimes<<msgPAL.m_SitDownTimes<<msgPAL.m_PlayTimes
			<<msgPAL.m_OneLifeGameTime<<msgPAL.m_OneLifeMatchTime<<msgPAL.m_OneLifeTotalTime;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::OnGameError(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnGameError");

	RWDB_GameError msgPCE;
	ExplainRWDBMsg(*pMsg,msgPCE);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into dezhou_error_log (AID,PID,Flag,Key,Des,ActionTime) \
							 values(:f1<short>,:f2<unsigned int>,:f3<int>,:f4<char[50]>,:f5<char[255]>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream << msgPCE.m_AID << msgPCE.m_PID << msgPCE.m_Flag << msgPCE.m_Key << msgPCE.m_Des;
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBLogConnect,m_strDBLog);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::RWDB_Test(INT16 AID,UINT32 PID,string strCity)
{
	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::ReadUserMoguiMoney(INT16 AID,UINT32 PID,stDBMoguiInfo& stMI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserMoguiMoney");

	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,
			"select MoGuiMoney from dezhou_money where AID=:f2<short> and PID=:f1<unsigned int> limit 1",
			m_DBGameConnect);
		TempDBStream<<AID<<PID;

		if ( !TempDBStream.eof() )
		{
			stMI.m_AID = AID;
			stMI.m_PID = PID;
			TempDBStream >> stMI.m_MoGuiMoney;
		}
		else
		{
			DebugError("ReadUserMoguiMoney AID=%d PID=%d",AID,PID);
			ret = DB_RESULT_NOPLAYER;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::AddUserMoGuiMoney(INT16 AID,UINT32 PID,int nAddMoney)
{
	CLogFuncTime lft(m_FuncTime,"AddUserMoGuiMoney");

	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	if( nAddMoney == 0) return ret;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update dezhou_money set MoGuiMoney=MoGuiMoney+:f1<int> where PID=:f2<unsigned int> and MoGuiMoney+:f11<int> >= 0";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<nAddMoney<<PID<<nAddMoney;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 ){
			stDBMoguiInfo stDBMM;
			ReadUserMoguiMoney(AID,PID,stDBMM);

			RWDB_GameError rwdbGE;
			rwdbGE.m_AID = AID;
			rwdbGE.m_PID = PID;
			rwdbGE.m_Flag = GameError_AddMoguiMoney;
			rwdbGE.m_Des = "AddMogui=" + N2S(nAddMoney) + " " + "CurMogui=" + N2S(stDBMM.m_MoGuiMoney);
			rwdbGE.m_Key = "AddUserMoGuiMoney";
			
			CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
			if ( pMsgManager ){
				pMsgManager->PushRWDBMsg(rwdbGE);
			}

			DebugError("AddUserMoGuiMoney AID=%d PID=%d nMoguiMoney=%d CurMoney=%d",AID,PID,nAddMoney,stDBMM.m_MoGuiMoney);
		}

		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::ReadUserGameMoney(INT16 AID,UINT32 PID,stDBGameMoney& stGM)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserGameMoney");
	
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,
			"select GameMoney,HongBao from dezhou_money where PID=:f1<unsigned int> limit 1",m_DBGameConnect);
		TempDBStream<<PID;

		if ( !TempDBStream.eof() )
		{
			stGM.m_AID = AID;
			stGM.m_PID = PID;
			TempDBStream >> stGM.m_GameMoney;
			TempDBStream >> stGM.m_HongBao;
		}
		else
		{
			DebugError("ReadUserGameMoney AID=%d PID=%d",AID,PID);
			ret = DB_RESULT_NOPLAYER;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::DBAddUserGameMoney(INT16 AID,UINT32 PID,INT64 nAddMoney)
{
	CLogFuncTime lft(m_FuncTime,"DBAddUserGameMoney");
	
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);

	if ( nAddMoney == 0 ) return ret;
	try
	{
		string strSQL = "update dezhou_money set GameMoney=GameMoney+:f1<bigint> \
		where PID=:f102<unsigned int> and GameMoney+:f101<bigint> >= 0";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<nAddMoney<<PID<<nAddMoney;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			stDBGameMoney stGM;
			if ( ReadUserGameMoney(AID,PID,stGM) == DB_RESULT_SUCCESS )
			{
				RWDB_GameError rwdbGE;
				rwdbGE.m_AID = AID;
				rwdbGE.m_PID = PID;
				rwdbGE.m_Flag = GameError_AddMoney;
				rwdbGE.m_Des = "AddMoney=" + N2S(nAddMoney) + " CurMoney=" + N2S(stGM.m_GameMoney);
				rwdbGE.m_Key = "DBAddUserGameMoney";
				CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
				if ( pMsgManager ){
					pMsgManager->PushRWDBMsg(rwdbGE);
				}

				DebugError("DBAddUserGameMoney AID=%d PID=%d nAddMoney=%s CurMoney=%s",
					AID,PID,N2S(nAddMoney).c_str(),N2S(stGM.m_GameMoney).c_str() );
			}		
		}

		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::ReadUserGameInfo(INT16 AID,UINT32 PID,stUserGameInfo& stUGI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserGameInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL =
		"select Forbid,BankMoney,OpenBank,MoGuiMoney,GameMoney,HongBao,JF,EP,WinTimes,LossTimes,GameTime,TopLimite,LowLimite,GameRight, \
		TJTimes,TJWin,TJBest,TJBestTime, \
		JBTimes,JBWin,JBBest,JBBestTime, \
		GJTimes,GJWin,GJBest,GJBestTime, \
		MaxMoney,MaxMoneyTime,MaxPai,MaxPaiTime,MaxWin,MaxWinTime, \
		One,Two,TwoTwo,Three,Str,Hua,ThreeTwo,Four,HuaStr,King, \
		Achieve_1,Achieve_2,Achieve_3,Achieve_4,\
		SendGift,RecvGift,TotalInvite,ShowFace, \
		FreeFaceTime,VipLevel,VipEndTime \
		from v_dezhou_usergameinfo where PID =:f1<unsigned int> limit 1";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );

		TempDBStream<<PID;
		if ( !TempDBStream.eof() )
		{
			stUGI.m_PID         = PID;
			stUGI.m_AID         = AID;

			TempDBStream >> stUGI.m_stGameInfo.m_Forbid;
			TempDBStream >> stUGI.m_stGameInfo.m_BankMoney;
			TempDBStream >> stUGI.m_stGameInfo.m_OpenBank;
			TempDBStream >> stUGI.m_stGameInfo.m_nMoGuiMoney;
			TempDBStream >> stUGI.m_stGameInfo.m_nGameMoney;			
			TempDBStream >> stUGI.m_stGameInfo.m_nMatchJF;
			TempDBStream >> stUGI.m_stGameInfo.m_nJF;
			TempDBStream >> stUGI.m_stGameInfo.m_nEP;
			TempDBStream >> stUGI.m_stGameInfo.m_nWinTimes;
			TempDBStream >> stUGI.m_stGameInfo.m_nLossTimes;
			TempDBStream >> stUGI.m_stGameInfo.m_nGameTime;
			TempDBStream >> stUGI.m_stGameInfo.m_nUpperLimite;
			TempDBStream >> stUGI.m_stGameInfo.m_nLowerLimite;
			TempDBStream >> stUGI.m_stGameInfo.m_Right;

			TempDBStream >> stUGI.m_stGameMatchInfo.m_TaoJinTimes;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_TaoJinWinMoney;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_TaoJinBest;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_TaoJinBestTime;

			TempDBStream >> stUGI.m_stGameMatchInfo.m_JingBiaoTimes;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_JingBiaoWinMoney;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_JingBiaoBest;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_JingBiaoBestTime;

			TempDBStream >> stUGI.m_stGameMatchInfo.m_GuanJunTimes;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_GuanJunWinMoney;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_GuanJunBest;
			TempDBStream >> stUGI.m_stGameMatchInfo.m_GuanJunBestTime;

			TempDBStream >> stUGI.m_stGameInfoEX.m_MaxMoney;
			TempDBStream >> stUGI.m_stGameInfoEX.m_MaxMoneyTime;
			TempDBStream >> stUGI.m_stGameInfoEX.m_MaxPai;
			TempDBStream >> stUGI.m_stGameInfoEX.m_MaxPaiTime;
			TempDBStream >> stUGI.m_stGameInfoEX.m_MaxWin;
			TempDBStream >> stUGI.m_stGameInfoEX.m_MaxWinTime;

			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[0];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[1];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[2];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[3];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[4];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[5];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[6];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[7];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[8];
			TempDBStream >> stUGI.m_stGameInfoEX.m_WinRecord[9];

			TempDBStream >> stUGI.m_stGameInfoEX.m_Achieve[0];
			TempDBStream >> stUGI.m_stGameInfoEX.m_Achieve[1];
			TempDBStream >> stUGI.m_stGameInfoEX.m_Achieve[2];
			TempDBStream >> stUGI.m_stGameInfoEX.m_Achieve[3];

			TempDBStream >> stUGI.m_stGameInfoEX.m_SendGift;
			TempDBStream >> stUGI.m_stGameInfoEX.m_RecvGift;
			TempDBStream >> stUGI.m_stGameInfoEX.m_TotalInvite;
			TempDBStream >> stUGI.m_stGameInfoEX.m_ShowFace;

			TempDBStream >> stUGI.m_stGameInfoEX.m_FreeFaceTime;
			TempDBStream >> stUGI.m_stGameInfoEX.m_VipLevel;
			TempDBStream >> stUGI.m_stGameInfoEX.m_VipEndTime;
		}
		else
		{
			DebugError("ReadUserGameInfo No Player AID=%d PID=%d",AID,PID);
			ret = DB_RESULT_NOPLAYER;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::UpdateWinLossMoney(const stWinLossMoney& stWLM)
{
	CLogFuncTime lft(m_FuncTime,"UpdateWinLossMoney");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);

	if( stWLM.m_nGameMoney!=0 || stWLM.m_nHongBao!=0 )
	{
		try
		{
			string strSQL = "update dezhou_money set GameMoney=GameMoney+:f1<bigint>,HongBao=HongBao+:f2<bigint> \
							where PID=:f101<unsigned int> and GameMoney+:f102<bigint> >= 0 and HongBao+:f103<bigint> >= 0";

			otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
			TempDBStream<<stWLM.m_nGameMoney<<stWLM.m_nHongBao<<stWLM.m_PID<<stWLM.m_nGameMoney<<stWLM.m_nHongBao;
			TempDBStream.flush();

			if ( TempDBStream.get_rpc() == 0 )
			{
				stDBGameMoney stGM;
				if ( ReadUserGameMoney(stWLM.m_AID,stWLM.m_PID,stGM) == DB_RESULT_SUCCESS )
				{
					RWDB_GameError rwdbGE;
					rwdbGE.m_AID = stWLM.m_AID;
					rwdbGE.m_PID = stWLM.m_PID;
					rwdbGE.m_Flag = GameError_AddMoney;
					rwdbGE.m_Des = "AddMoney=" + N2S(stWLM.m_nGameMoney) + " CurMoney=" + N2S(stGM.m_GameMoney) + " AddHaoBao=" + N2S(stWLM.m_nHongBao) + " CurHaoBao=" + N2S(stGM.m_HongBao);
					rwdbGE.m_Key = "UpdateWinLossMoney";
					CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
					if ( pMsgManager ){
						pMsgManager->PushRWDBMsg(rwdbGE);
					}

					DebugError("UpdateWinLossMoney AID=%d PID=%d nAddMoney=%s CurMoney=%s AddHongBao=%s CurHongBao=%s",
						stWLM.m_AID,stWLM.m_PID,N2S(stWLM.m_nGameMoney).c_str(),N2S(stGM.m_GameMoney).c_str(),
						N2S(stWLM.m_nHongBao).c_str(),N2S(stGM.m_HongBao).c_str() );
				}			
			}

			TempDBStream.close();
		}
		catch(otl_exception &p)
		{
			CatchDBException(p);
			CheckOTLException(p,m_DBGameConnect,m_strDBGame);
			ret = DB_RESULT_DBERROR;
		}
	}

	return ret;
}
int CDBOperator::WriteTableInfo(stTableInfo& stTI)
{
	CLogFuncTime lft(m_FuncTime,"WriteTableInfo");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);

	try
	{
		string strSQL;
		otl_stream TempDBStream;
		int Idx = stTI.m_RoomID*10000 + stTI.m_TableID;

		string strTableName = "dezhou_tableinfo_"+Tool::N2S(stTI.m_ServerID);

		strSQL = "update " + strTableName + " set TableName=:f21<char[50]>,PassWord=:f22<char[20]>,TableRule=:f6<char[255]>,\
											TableRuleEX=:f7<char[255]>,MatchRule=:f8<char[255]> where Idx=:f1<int>";
		TempDBStream.open(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		TempDBStream<<stTI.m_TableName<<stTI.m_Password<<stTI.m_TableRule<<stTI.m_TableRuleEX<<stTI.m_MatchRule<<Idx;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			TempDBStream.close();

			strSQL = "insert into " + strTableName + 
				" (Idx,ServerID,RoomID,TableID,TableName,TableRule,TableRuleEX,MatchRule,FoundByWho) \
				values(:f1<int>,:f2<short>,:f3<short>,:f4<short>,:f5<char[50]>,:f6<char[255]>,:f7<char[255]>,:f8<char[255]>,:f9<unsigned int> )";
			TempDBStream.open(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);

			TempDBStream<<Idx
				<<stTI.m_ServerID
				<<stTI.m_RoomID
				<<stTI.m_TableID
				<<stTI.m_TableName
				<<stTI.m_TableRule
				<<stTI.m_TableRuleEX
				<<stTI.m_MatchRule
				<<stTI.m_FoundByWho;
			TempDBStream.close();
		}
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::ReadFaceInfo(VectorFaceInfo& vectorFI)
{
	CLogFuncTime lft(m_FuncTime,"ReadFaceInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		vectorFI.clear();

		stFaceInfo stFI;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, "select FaceID,Type,PriceFlag,Price,MinPrice,MaxPrice from dezhou_info_face ", m_DBGameConnect);
		while ( !TempDBStream.eof() )
		{
			stFI.Init();

			TempDBStream >> stFI.m_FaceID;
			TempDBStream >> stFI.m_Type;
			TempDBStream >> stFI.m_PriceFlag;
			TempDBStream >> stFI.m_Price;
			TempDBStream >> stFI.m_MinPrice;
			TempDBStream >> stFI.m_MaxPrice;

			vectorFI.push_back(stFI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorFI.clear();
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	if ( vectorFI.size()>0 && m_mapFaceInfo.size()==0 )
	{
		for (int i=0;i<(int)vectorFI.size();++i)
		{
			m_mapFaceInfo.insert(make_pair(vectorFI[i].m_FaceID,vectorFI[i]));
		}
	}

	return ret;
}
int CDBOperator::ReadHonorInfo(VectorHonorInfo& vectorHI)
{
	CLogFuncTime lft(m_FuncTime,"ReadHonorInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		vectorHI.clear();

		stHonorInfo stHI;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, "select ID,Type,Idx,Money from dezhou_info_honor ", m_DBGameConnect);
		while ( !TempDBStream.eof() )
		{
			stHI.Init();
			TempDBStream >> stHI.m_ID;
			TempDBStream >> stHI.m_Type;
			TempDBStream >> stHI.m_Idx;
			TempDBStream >> stHI.m_HonorMoney;
			vectorHI.push_back(stHI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorHI.clear();
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadGameLevelInfo(VectorPTLevelInfo& vectorGLI)
{
	CLogFuncTime lft(m_FuncTime,"ReadGameLevelInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		vectorGLI.clear();

		stPlayTimesInfo stGLI;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, "select Level,TotalPlay,AddPlay,Money from dezhou_info_playtimes ", m_DBGameConnect);
		while ( !TempDBStream.eof() )
		{
			stGLI.Init();
			TempDBStream >> stGLI.m_PTLevel;
			TempDBStream >> stGLI.m_TotalPT;
			TempDBStream >> stGLI.m_AddPT;
			TempDBStream >> stGLI.m_AwardMoney;
			vectorGLI.push_back(stGLI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorGLI.clear();
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadAreaInfo(VectorAreaInfo& vectorArea)
{
	CLogFuncTime lft(m_FuncTime,"ReadAreaInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		vectorArea.clear();

		stAreaInfo stAI;
		string strLandMoney,strJoinMoney;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, "select AID,JoinMoney,LandMoney from dezhou_info_area ", m_DBGameConnect);
		while ( !TempDBStream.eof() )
		{
			stAI.Init();
			TempDBStream >> stAI.m_AID;
			TempDBStream >> strJoinMoney;
			TempDBStream >> strLandMoney;

			stAI.InitLandMoney(strLandMoney);
			stAI.InitJoinMoney(strJoinMoney);

			vectorArea.push_back(stAI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorArea.clear();
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadProductInfo(VectorProductInfo& vectorPI)
{
	CLogFuncTime lft(m_FuncTime,"ReadProductInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		vectorPI.clear();

		ProductInfo stPI;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, "select ProductID,Price,ProductType,ProductRule from dezhou_info_product ", m_DBGameConnect);
		while ( !TempDBStream.eof() )
		{
			stPI.Init();
			TempDBStream >> stPI.m_ProductID;
			TempDBStream >> stPI.m_Price;
			TempDBStream >> stPI.m_ProductType;
			TempDBStream >> stPI.m_ProductRule;
			vectorPI.push_back(stPI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorPI.clear();
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::UpdateUserProduct(int Idx,INT16 UseFlag)
{
	CLogFuncTime lft(m_FuncTime,"UpdateUserProduct");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "update user_dezhou_product set UseFlag=:f1<short> where Idx=:f2<int>";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<UseFlag<<Idx;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 ){
			DebugError("UpdateUserProduct Idx=%d UseFlag=%d",Idx,UseFlag);
		}		
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadUserProduct(INT16 AID,UINT32 PID,VectorUserProduct& vectorUP)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserProduct");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		vectorUP.clear();

		string strSQL = "select Idx,ProductID,SellMoney,ActionTime from user_dezhou_product where PID=:f2<unsigned int> and UseFlag=1";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<PID;

		stUserProduct stUP;
		otl_datetime TempOTLTime;
		while ( !TempDBStream.eof() )
		{
			stUP.Init();
			stUP.m_AID = AID;
			stUP.m_PID = PID;

			TempDBStream >> stUP.m_Idx;
			TempDBStream >> stUP.m_ProductID;
			TempDBStream >> stUP.m_SellMoney;
			TempDBStream >> TempOTLTime;
			stUP.m_ActionTime = UINT32(Tool::GetTimeFromOTLTime(TempOTLTime));

			vectorUP.push_back(stUP);
		}
		
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		//vectorUP.clear();
		CatchDBException( p );
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadAllUserProduct(int MaxIdx,VectorUserProduct& vectorUP)
{
	CLogFuncTime lft(m_FuncTime,"ReadAllUserProduct");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		vectorUP.clear();

		string strSQL = "select Idx,AID,PID,ProductID,SellMoney,ActionTime from user_dezhou_product where Idx>:f2<int> and UseFlag=1";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<MaxIdx;

		stUserProduct stUP;
		otl_datetime TempOTLTime;
		while ( !TempDBStream.eof() )
		{
			stUP.Init();

			TempDBStream >> stUP.m_Idx;
			TempDBStream >> stUP.m_AID;
			TempDBStream >> stUP.m_PID;
			TempDBStream >> stUP.m_ProductID;
			TempDBStream >> stUP.m_SellMoney;
			TempDBStream >> TempOTLTime;
			stUP.m_ActionTime = UINT32(Tool::GetTimeFromOTLTime(TempOTLTime));

			vectorUP.push_back(stUP);
		}

		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		//vectorUP.clear();
		CatchDBException( p );
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadGiftInfo(VectorGiftInfo& vectorGI)
{
	CLogFuncTime lft(m_FuncTime,"ReadGiftInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		vectorGI.clear();

		stGiftInfo stGI;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, "select GiftID,Type,PriceFlag,Price,MinPrice,MaxPrice,Rebate,CurLastTime,TotalLastTime from dezhou_info_gift ", m_DBGameConnect);
		while ( !TempDBStream.eof() )
		{
			stGI.Init();

			TempDBStream >> stGI.m_GiftID;
			TempDBStream >> stGI.m_Type;
			TempDBStream >> stGI.m_PriceFlag;
			TempDBStream >> stGI.m_Price;
			TempDBStream >> stGI.m_MinPrice;
			TempDBStream >> stGI.m_MaxPrice;
			TempDBStream >> stGI.m_Rebate;
			TempDBStream >> stGI.m_CurLastTime;
			TempDBStream >> stGI.m_TotalLastTime;

			vectorGI.push_back(stGI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorGI.clear();
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	if ( vectorGI.size()>0 && m_mapGiftInfo.size()==0 )
	{
		for (int i=0;i<(int)vectorGI.size();++i)		
		{
			m_mapGiftInfo.insert(make_pair(vectorGI[i].m_GiftID,vectorGI[i]));
		}
	}

	return ret;
}

int CDBOperator::ReadMatchInfo(INT16 ServerID,VectorDBMatchInfo& vectorMI)
{
	CLogFuncTime lft(m_FuncTime,"ReadMatchInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		stDBMatchInfo stMI;
		string strBlind,strAward;
		otl_datetime oltTime;

		string strTableName = "dezhou_matchinfo_"+N2S(ServerID);
		string strSQL = "select MatchID,MatchType,Ticket,TakeMoney,StartMoney,Blind,Award,StrRule,StartTime,StartInterval from " + strTableName;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);

		while ( !TempDBStream.eof() )
		{
			stMI.Init();

			TempDBStream >> stMI.m_MatchID;
			TempDBStream >> stMI.m_MatchType;
			TempDBStream >> stMI.m_Ticket;
			TempDBStream >> stMI.m_nTakeMoney;
			TempDBStream >> stMI.m_StartMoney;
			TempDBStream >> stMI.m_strBlind;
			TempDBStream >> stMI.m_strAward;
			TempDBStream >> stMI.m_StrRule;
			TempDBStream >> oltTime;
			TempDBStream >> stMI.m_StartInterval;

			stMI.m_StartTime = UINT32(Tool::GetTimeFromOTLTime(oltTime));
			vectorMI.push_back(stMI);			
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorMI.clear();
		CatchDBException(p);
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}
	
	return ret;
}

int CDBOperator::ReadFriendInfo(INT16 AID,UINT32 PID,VectorFriendInfo& vectorFI)
{
	CLogFuncTime lft(m_FuncTime,"ReadFriendInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		std::string strSQL = "select RightPID,Flag from " + getFriendTableName(PID)
			+ " where LeftPID=:f1<unsigned int> and Flag>0 ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		TempDBStream<<PID;

		stUserFriendInfo stFI;
		while ( !TempDBStream.eof() )
		{
			stFI.Init();
			stFI.m_LeftPID = PID;
			TempDBStream >> stFI.m_RightPID;
			TempDBStream >> stFI.m_Flag;

			vectorFI.push_back(stFI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorFI.clear();

		CatchDBException( p );
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadRoomInfo(INT16 ServerID,VectorRoomInfo& vectorRI)
{
	CLogFuncTime lft(m_FuncTime,"ReadRoomInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;

	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		UINT32 curTime = UINT32(time(NULL));
		string strTableName = "dezhou_roominfo_"+N2S(ServerID);
		string strSQL = "select ServerID,RoomID,RoomName,Password,AreaRule,RoomRule,RoomRuleEX,MatchRule,EndTime from "
			+ strTableName + " where ServerID=:f<short> ";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);

		TempDBStream << ServerID;
		while ( !TempDBStream.eof() )
		{
			stRoomInfo RI;
			otl_datetime otl_endtime;

			TempDBStream >> RI.m_ServerID;
			TempDBStream >> RI.m_RoomID;
			TempDBStream >> RI.m_RoomName;
			TempDBStream >> RI.m_Password;			
			TempDBStream >> RI.m_AreaRule;
			TempDBStream >> RI.m_RoomRule;
			TempDBStream >> RI.m_RoomRuleEX;
			TempDBStream >> RI.m_MatchRule;
			TempDBStream >> otl_endtime;

			RI.m_EndTime = UINT32(Tool::GetTimeFromOTLTime(otl_endtime));

			if ( RI.m_EndTime == 0 || RI.m_EndTime > curTime )
			{
				vectorRI.push_back(RI);
			}
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{	
		vectorRI.clear();
		CatchDBException( p );
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadTableInfo(INT16 ServerID,VectorTableInfo& vectorTI)
{
	CLogFuncTime lft(m_FuncTime,"ReadTableInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		UINT32 curTime = UINT32(time(NULL));
		string strTableName = "dezhou_tableinfo_"+N2S(ServerID);
		string strSQL = "select RoomID,TableID,TableName,PassWord,TableRule,TableRuleEX,MatchRule,FoundByWho,EndTime from " + strTableName +
			" where ServerID=:f<short>";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		TempDBStream<<ServerID;
		while ( !TempDBStream.eof() )
		{
			stTableInfo TI;
			otl_datetime otl_endtime;

			TI.m_ServerID = ServerID;

			TempDBStream >> TI.m_RoomID;
			TempDBStream >> TI.m_TableID;
			TempDBStream >> TI.m_TableName;
			TempDBStream >> TI.m_Password;
			TempDBStream >> TI.m_TableRule;
			TempDBStream >> TI.m_TableRuleEX;
			TempDBStream >> TI.m_MatchRule;
			TempDBStream >> TI.m_FoundByWho;
			TempDBStream >> otl_endtime;

			TI.m_TableEndTime = UINT32(Tool::GetTimeFromOTLTime(otl_endtime));

			if ( TI.m_TableEndTime == 0 || TI.m_TableEndTime > curTime )
			{
				vectorTI.push_back(TI);
			}
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		ret = DB_RESULT_DBERROR;
		vectorTI.clear();
		CatchDBException( p );
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
	}

	//读取自建桌子的数据
	try
	{
		UINT32 curTime = UINT32(time(NULL));
		string strTableName = "dezhou_tableprivate_"+N2S(ServerID);
		string strSQL = "select RoomID,TableID,TableName,PassWord,TableRule,TableRuleEX,MatchRule,FoundByWho,EndTime from " + strTableName +
			" where ServerID=:f<short>";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		TempDBStream<<ServerID;
		while ( !TempDBStream.eof() )
		{
			stTableInfo TI;
			otl_datetime otl_endtime;

			TI.m_ServerID = ServerID;

			TempDBStream >> TI.m_RoomID;
			TempDBStream >> TI.m_TableID;
			TempDBStream >> TI.m_TableName;
			TempDBStream >> TI.m_Password;
			TempDBStream >> TI.m_TableRule;
			TempDBStream >> TI.m_TableRuleEX;
			TempDBStream >> TI.m_MatchRule;
			TempDBStream >> TI.m_FoundByWho;
			TempDBStream >> otl_endtime;

			TI.m_TableEndTime = UINT32(Tool::GetTimeFromOTLTime(otl_endtime));

			if ( TI.m_TableEndTime == 0 || TI.m_TableEndTime > curTime )
			{
				vectorTI.push_back(TI);
			}
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		ret = DB_RESULT_DBERROR;
		CatchDBException( p );
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
	}

	return ret;
}
int CDBOperator::ReadUserGiftInfo(INT16 AID,UINT32 PID,VectorDBUserGiftInfo& vectorUGI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserGiftInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect, m_strDBGame);
	try
	{
		UINT32 curTime = UINT32(time(NULL));
	
		std::string strSQL = "select Idx,SendPID,GiftID,Price,Name,CreateTime from " + getGiftTableName(PID) + 
			" where RecvPID=:f1<unsigned int> and Flag=1";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE, strSQL.c_str(), m_DBGameConnect);
		TempDBStream<<PID;

		UINT32 nEndTime = 0;
		vector<int> listInvalidGift;
		stDBUserGiftInfo stUGI;
		otl_datetime OTLTime;
		MapGiftInfo::iterator itorGI;
		while ( !TempDBStream.eof() )
		{
			stUGI.Init();
			stUGI.m_RecvPID = PID;
			TempDBStream >> stUGI.m_GiftIdx;
			TempDBStream >> stUGI.m_SendPID;
			TempDBStream >> stUGI.m_GiftID;
			TempDBStream >> stUGI.m_Price;
			TempDBStream >> stUGI.m_NickName;
			TempDBStream >> OTLTime;

			itorGI = m_mapGiftInfo.find(stUGI.m_GiftID);
			if ( itorGI != m_mapGiftInfo.end() )
			{
				stUGI.m_ActionTime = UINT32(Tool::GetTimeFromOTLTime(OTLTime));
				nEndTime = stUGI.m_ActionTime + itorGI->second.m_TotalLastTime;
				if ( nEndTime > curTime )
					vectorUGI.push_back(stUGI);				
				else				
					listInvalidGift.push_back(stUGI.m_GiftIdx);				
			}
		}
		TempDBStream.close();

		if ( listInvalidGift.size() )
		{
			CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
			if ( pMsgManager )
			{
				RWDB_UpdateGiftInfo msgUGI;
				for (size_t i=0;i<listInvalidGift.size();++i)
				{
					msgUGI.ReSet();
					msgUGI.m_PID         = PID;
					msgUGI.m_GiftIdx     = listInvalidGift[i];
					msgUGI.m_Flag        = N_Gift::GiftST_OutTime;
					pMsgManager->PushRWDBMsg(msgUGI);
				}				
			}
		}
	}
	catch(otl_exception &p)
	{
		vectorUGI.clear();
		CatchDBException( p );
		CheckOTLException(p, m_DBGameConnect, m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBOperator::ReadUserAward(INT16 AID,UINT32 PID,VectorAward& vectorAward)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserAward");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{		
		std::string strSQL = "select Idx,Money,MoneyFlag,PayMode,EndTime from dezhou_award where PID=:f1<unsigned int> and CanUse=1";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
		TempDBStream<<PID;

		otl_datetime TempOTLTime;
		stDBUserAward stUA;
		while ( !TempDBStream.eof() )
		{
			stUA.Init();
			stUA.m_PID = PID;

			TempDBStream >> stUA.m_Idx;
			TempDBStream >> stUA.m_nMoney;
			TempDBStream >> stUA.m_MoneyFlag;
			TempDBStream >> stUA.m_PayMode;
			TempDBStream >> TempOTLTime;

			stUA.m_EndTime = UINT32(Tool::GetTimeFromOTLTime(TempOTLTime));

			vectorAward.push_back(stUA);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorAward.clear();
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::ReadUserRight(INT16 AID,UINT32 PID,VectorRight& vectorRight)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserRight");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{		
		std::string strSQL = "select Idx,Times,Level,EndTime from dezhou_right where PID=:f1<unsigned int> and CanUse=1";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
		TempDBStream<<PID;

		otl_datetime TempOTLTime;
		stDBGameRight stGR;
		while ( !TempDBStream.eof() )
		{
			stGR.Init();
			stGR.m_PID = PID;
			stGR.m_AID = AID;

			TempDBStream >> stGR.m_Idx;
			TempDBStream >> stGR.m_Times;
			TempDBStream >> stGR.m_Level;
			TempDBStream >> TempOTLTime;

			stGR.m_EndTime = UINT32(Tool::GetTimeFromOTLTime(TempOTLTime));

			vectorRight.push_back(stGR);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorRight.clear();
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::CreatePlayerGameInfo(DezhouLib::stCreatePlayerGameInfo& stCPGI)
{
	CLogFuncTime lft(m_FuncTime,"CreatePlayerGameInfo");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		{
			INT64 nGameMoney = stCPGI.m_OrigenMoney + stCPGI.m_LandMoney;
			string strGameInfo = "insert into dezhou_money (PID,AID,GameMoney) \
								 select :f1<unsigned int>,:f2<short>,:f11<bigint> \
								 from dual where not exists \
								 (select * from dezhou_money where dezhou_money.PID = :f11<unsigned int>)";
			otl_stream GameInfoStream( OTL_STREAMBUF_SIZE,strGameInfo.c_str(),m_DBGameConnect );
			GameInfoStream<<stCPGI.m_PID<<stCPGI.m_AID<<nGameMoney<<stCPGI.m_PID;
		}
		{
			string strGameInfo = "insert into dezhou_gameinfo (PID,AID,TopLimite,LowLimite,GameRight) \
								 select :f1<unsigned int>,:f2<short>,:f3<bigint>,:f4<bigint>,:f5<bigint> \
								 from dual where not exists \
								 (select * from dezhou_gameinfo where dezhou_gameinfo.PID = :f11<unsigned int>)";
			otl_stream GameInfoStream( OTL_STREAMBUF_SIZE,strGameInfo.c_str(),m_DBGameConnect );
			GameInfoStream<<stCPGI.m_PID<<stCPGI.m_AID<<stCPGI.m_nUpperLimite<<stCPGI.m_nLowerLimite<<stCPGI.m_Right<<stCPGI.m_PID;
		}
		{
			string strGameInfoEx = "insert into dezhou_gameinfoex (PID,AID) select :f1<unsigned int>,:f2<short> \
								   from dual where not exists \
								   (select * from dezhou_gameinfoex where dezhou_gameinfoex.PID = :f11<unsigned int>)";
			otl_stream GameInfoExStream( OTL_STREAMBUF_SIZE,strGameInfoEx.c_str(),m_DBGameConnect );
			GameInfoExStream<<stCPGI.m_PID<<stCPGI.m_AID<<stCPGI.m_PID;
		}
		{
			string strMatchInfo = "insert into dezhou_matchinfo (PID,AID) select :f1<unsigned int>,:f2<short> \
								  from dual where not exists \
								  (select * from dezhou_matchinfo where dezhou_matchinfo.PID = :f11<unsigned int>)";
			otl_stream MatchInfoStream( OTL_STREAMBUF_SIZE,strMatchInfo.c_str(),m_DBGameConnect );
			MatchInfoStream<<stCPGI.m_PID<<stCPGI.m_AID<<stCPGI.m_PID;
		}
	}
	catch(otl_exception &p)
	{
		ret = DB_RESULT_DBERROR;
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
	}

	return ret;
}

INT16 CDBOperator::ReadUserAID(UINT32 PID)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserAID");
	InterlockedIncrement(&s_ReadOperator);

	INT16 retAID = 0;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "select AID	from user_root where PID=:f1<unsigned int> limit 1";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<PID;

		if ( !TempDBStream.eof() )
		{
			TempDBStream >> retAID;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
	}

	return retAID;
}

int CDBOperator::ReadUserDataInfo(INT16 AID,UINT32 PID,stUserDataInfo& stUDI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserDataInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "select NickName,InfoFlag,Sex,HeadPicURL,HomeURL,City,GameInfo,InviteAID,InvitePID,PlayerLevel,JoinTime \
						from v_userdatainfo where PID=:f1<unsigned int> limit 1";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<PID;

		if ( !TempDBStream.eof() )
		{
			otl_datetime OTLTime;

			TempDBStream >> stUDI.m_NickName;
			TempDBStream >> stUDI.m_ChangeName;
			TempDBStream >> stUDI.m_Sex;
			TempDBStream >> stUDI.m_HeadPicURL;
			TempDBStream >> stUDI.m_HomePageURL;
			TempDBStream >> stUDI.m_City;

			TempDBStream >> stUDI.m_HaveGameInfo;
			TempDBStream >> stUDI.m_InviteAID;
			TempDBStream >> stUDI.m_InvitePID;
			TempDBStream >> stUDI.m_PlayerLevel;
			TempDBStream >> OTLTime;

			stUDI.m_JoinTime = UINT32(Tool::GetTimeFromOTLTime(OTLTime));
		}
		else
		{
			DebugError("ReadUserDataInfo No Player AID=%d PID=%d",AID,PID);
			ret = DB_RESULT_NOPLAYER;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBOperator::ReadBotPlayerData(INT16 AID,UINT32 StartPID,UINT32 EndPID,VectorBotPlayerData& vectorPD)
{
	CLogFuncTime lft(m_FuncTime,"ReadBotPlayerData");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,
			"select PID,NickName,Sex,HeadPicURL,HomeURL,City,InvitePID,JoinTime, \
			MoGuiMoney,GameMoney,HongBao,JF,EP,WinTimes,LossTimes,GameTime,TopLimite,LowLimite,GameRight,BotLevel,\
			MaxMoney,MaxMoneyTime,MaxPai,MaxPaiTime,MaxWin,MaxWinTime,\
			One,Two,TwoTwo,Three,Str,Hua,ThreeTwo,Four,HuaStr,King,Achieve_1,Achieve_2,Achieve_3,Achieve_4,\
			VipLevel,\
			TJTimes,TJWin,TJBest,TJBestTime, \
			JBTimes,JBWin,JBBest,JBBestTime, \
			GJTimes,GJWin,GJBest,GJBestTime \
			from v_dezhou_botplayerdata where AID=:f3<short> and PID>=:f1<unsigned int> and PID<=:f2<unsigned int> ",
			m_DBGameConnect );

		TempDBStream<<AID<<StartPID<<EndPID;

		while ( !TempDBStream.eof() )
		{
			string strRule;
			otl_datetime TempOTLTime;
			stBotPlayerData stPD;
			short TempBotLevel;

			stPD.m_AID = AID;
			TempDBStream >> stPD.m_PID;
			TempDBStream >> stPD.m_stUserDataInfo.m_NickName;
			TempDBStream >> stPD.m_stUserDataInfo.m_Sex;
			TempDBStream >> stPD.m_stUserDataInfo.m_HeadPicURL;
			TempDBStream >> stPD.m_stUserDataInfo.m_HomePageURL;
			TempDBStream >> stPD.m_stUserDataInfo.m_City;
			TempDBStream >> stPD.m_stUserDataInfo.m_InvitePID;
			TempDBStream >> TempOTLTime;

			TempDBStream >> stPD.m_stGameInfo.m_nMoGuiMoney;
			TempDBStream >> stPD.m_stGameInfo.m_nGameMoney;
			TempDBStream >> stPD.m_stGameInfo.m_nMatchJF;

			TempDBStream >> stPD.m_stGameInfo.m_nJF;
			TempDBStream >> stPD.m_stGameInfo.m_nEP;
			TempDBStream >> stPD.m_stGameInfo.m_nWinTimes;
			TempDBStream >> stPD.m_stGameInfo.m_nLossTimes;
			TempDBStream >> stPD.m_stGameInfo.m_nGameTime;
			TempDBStream >> stPD.m_stGameInfo.m_nUpperLimite;
			TempDBStream >> stPD.m_stGameInfo.m_nLowerLimite;
			TempDBStream >> stPD.m_stGameInfo.m_Right;
			TempDBStream >> TempBotLevel;

			TempDBStream >> stPD.m_stGameInfoEX.m_MaxMoney;
			TempDBStream >> stPD.m_stGameInfoEX.m_MaxMoneyTime;
			TempDBStream >> stPD.m_stGameInfoEX.m_MaxPai;
			TempDBStream >> stPD.m_stGameInfoEX.m_MaxPaiTime;
			TempDBStream >> stPD.m_stGameInfoEX.m_MaxWin;
			TempDBStream >> stPD.m_stGameInfoEX.m_MaxWinTime;

			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[0];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[1];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[2];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[3];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[4];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[5];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[6];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[7];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[8];
			TempDBStream >> stPD.m_stGameInfoEX.m_WinRecord[9];

			TempDBStream >> stPD.m_stGameInfoEX.m_Achieve[0];
			TempDBStream >> stPD.m_stGameInfoEX.m_Achieve[1];
			TempDBStream >> stPD.m_stGameInfoEX.m_Achieve[2];
			TempDBStream >> stPD.m_stGameInfoEX.m_Achieve[3];

			TempDBStream >> stPD.m_stGameInfoEX.m_VipLevel;

			TempDBStream >> stPD.m_stGameMatchInfo.m_TaoJinTimes;
			TempDBStream >> stPD.m_stGameMatchInfo.m_TaoJinWinMoney;
			TempDBStream >> stPD.m_stGameMatchInfo.m_TaoJinBest;
			TempDBStream >> stPD.m_stGameMatchInfo.m_TaoJinBestTime;	

			TempDBStream >> stPD.m_stGameMatchInfo.m_JingBiaoTimes;
			TempDBStream >> stPD.m_stGameMatchInfo.m_JingBiaoWinMoney;
			TempDBStream >> stPD.m_stGameMatchInfo.m_JingBiaoBest;
			TempDBStream >> stPD.m_stGameMatchInfo.m_JingBiaoBestTime;	

			TempDBStream >> stPD.m_stGameMatchInfo.m_GuanJunTimes;
			TempDBStream >> stPD.m_stGameMatchInfo.m_GuanJunWinMoney;
			TempDBStream >> stPD.m_stGameMatchInfo.m_GuanJunBest;
			TempDBStream >> stPD.m_stGameMatchInfo.m_GuanJunBestTime;

			stPD.m_stUserDataInfo.m_JoinTime = UINT32(Tool::GetTimeFromOTLTime(TempOTLTime));
			stPD.m_BotLevel = UINT8(TempBotLevel);

			vectorPD.push_back(stPD);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException( p );
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}