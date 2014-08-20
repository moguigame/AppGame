#include "DBWriteOperator.h"

#include "Server.h"
#include "CDBSConfig.h"
#include "DBOperatorXY.h"

using namespace MoGui::MoGuiXY::RWDB_XY;

int CDBWriteOperator::s_MaxID = 0;

volatile UINT32 CDBWriteOperator::s_WriteOperator = 0;
volatile UINT32 CDBWriteOperator::s_ReadOperator = 0;
UINT32 CDBWriteOperator::s_LastWriteOperator = 0;
UINT32 CDBWriteOperator::s_LastReadOperator = 0;
UINT32 CDBWriteOperator::s_LastTime = 0;

void CDBWriteOperator::StaticInit()
{
	s_LastTime = int(time(NULL));
}

CDBWriteOperator::CDBWriteOperator(CServer* pServer)
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

		m_strDBConfig = m_pConfig->m_strDBConfig;
		otl_connect::otl_initialize();
		try
		{
			m_DBConfigConnect.rlogon(m_strDBConfig.c_str());
		}
		catch(otl_exception& p)
		{
			CatchDBException(p);
		}

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
	}
	else
	{
		DebugError("CDBWriteOperator Init Error");
	}
}


CDBWriteOperator::~CDBWriteOperator(void)
{
	m_DBConfigConnect.logoff();
	m_DBUserConnect.logoff();
	m_DBGameConnect.logoff();
	m_DBLogConnect.logoff();
	m_DBFriendConnect.logoff();
	m_DBPropConnect.logoff();
}

void CDBWriteOperator::DebugError(const char* logstr,...)
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
void CDBWriteOperator::DebugInfo(const char* logstr,...)
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

void CDBWriteOperator::InitTableName(UINT32 curTime)
{
	string TableHead = "thirteen_";

	m_TNGameInfo = TableHead + "gameinfo";
	m_TNWinLoss = TableHead + "winloss_log_" + Tool::GetMonthString_XXXXYY(curTime);
	m_TNMoneyLog = TableHead + "money_log_" + Tool::GetMonthString_XXXXYY(curTime);

	m_TNMatchResult = TableHead + "match_log";
	m_TNMatchWinLoss = TableHead + "matchwinloss_log";

	m_TNChat = TableHead + "chat_log";
	m_TNLogin = TableHead + "login_log";
	m_TNAction = TableHead + "player_action";
	m_TNOnline = TableHead + "online";
}

int CDBWriteOperator::OnActiveDBConnect()
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

		ActiveDBConnect(m_DBConfigConnect,m_strDBConfig);
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
			DebugInfo("CDBWriteOperator Func Count=%d",vectorFT.size());
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

void CDBWriteOperator::ActiveDBConnect(otl_connect& DBConnect,std::string& strDB)
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

void CDBWriteOperator::DoConnect(otl_connect& DBConnect,std::string& strDB)
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

void CDBWriteOperator::CheckOTLException(const otl_exception &p,otl_connect& DBConnect,std::string& strDB)
{
	if ( p.code == 2006 || p.code == 0 )
	{
		DoConnect(DBConnect,strDB);
	}
}

void CDBWriteOperator::CheckDBConnect(otl_connect& DBConnect,std::string& strDB)
{
	if ( !DBConnect.connected )
	{
		DoConnect(DBConnect,strDB);
	}
}

void CDBWriteOperator::CatchDBException(const otl_exception &p)
{
	print_otl_error(p);
}
void CDBWriteOperator::print_otl_error(const otl_exception &p)
{
	char msg[2048] = {0};
	_snprintf(msg, sizeof(msg)-1, "msg=%s\n", p.msg);									// print out error message
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "stm_text=%s\n", p.stm_text);	// print out SQL that caused the error
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "sqlstate=%s\n", p.sqlstate);	// print out SQLSTATE message
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "var_info=%s\n", p.var_info);	// print out the variable that caused the error	
	cout<<"DBError "<<Tool::GetTimeString(time(NULL))<<" "<<msg<<endl;
	DebugError("CDBWriteOperator %s",msg);
}

int CDBWriteOperator::OnRWDBMsg(ReadWriteDBMessage* pMsg)
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
	case RWDB_AddPlayerAward::XY_ID:
		{
			ret = OnAddPlayerAward(pMsg);
		}
		break;
	case RWDB_WinLossInfo::XY_ID:
		{
			ret = OnWinLossInfo(pMsg);
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
	case RWDB_GameMoneyError::XY_ID:
		{
			ret = OnGameMoneyError(pMsg);
		}
		break;
	case RWDB_UpdateAwardInfo::XY_ID:
		{
			ret = OnUpdateAwardInfo(pMsg);
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
	case RWDB_PlayerClientError::XY_ID:
		{
			ret = OnPlayerClientErr(pMsg);
		}
		break;
	case RWDB_ChatLog::XY_ID:
		{
			ret = OnChatLog(pMsg);
		}
		break;
	case RWDB_AddPlayerInfo::XY_ID:
		{
			ret = OnAddPlayerInfo(pMsg);
		}
		break;
	default:
		DebugError("OnRWDBMsg XYID=%d XYLen=%d",pMsg->m_MsgID,pMsg->m_MsgLen);
		break;
	}
	return 0;
}

int CDBWriteOperator::OnWinLossMsg(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnWinLossMsg");

	RWDB_WinLossLog msgWLL;
	ExplainRWDBMsg(*pMsg,msgWLL);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into " + m_TNWinLoss + 
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

int CDBWriteOperator::OnMatchResult(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMatchResult");

	RWDB_MatchResultLog msgMRL;
	ExplainRWDBMsg(*pMsg,msgMRL);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into " + m_TNMatchResult + " (MatchGUID,AID,PID,RoomID,TableID,MatchID,Position,AwardMoney,AwardJF,Ticket,TakeMoney,DateTime) \
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

int CDBWriteOperator::OnMatchWinLoss(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMatchWinLoss");

	RWDB_MatchWinLoss msgMWL;
	ExplainRWDBMsg(*pMsg,msgMWL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into " + m_TNMatchWinLoss + " (MatchGUID,GameGUID,AID,PID,RoomID,TableID,SitID,WinLoss,EndFlag,PaiType,DateTime) \
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

int CDBWriteOperator::OnMaxPai(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMaxPai");

	RWDB_MaxPai msgMP;
	ExplainRWDBMsg(*pMsg,msgMP);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update " + m_TNGameInfo + " set MaxPai=:f1<char[255]>,MaxPaiTime=:f2<unsigned int> where PID=:f99<unsigned int> ";
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
int CDBWriteOperator::OnMaxWin(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMaxWin");

	RWDB_MaxWin msgMW;
	ExplainRWDBMsg(*pMsg,msgMW);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update " + m_TNGameInfo + " set MaxWin=:f1<bigint>,MaxWinTime=:f2<unsigned int> where PID=:f99<unsigned int> ";
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
int CDBWriteOperator::OnMaxMoney(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnMaxMoney");

	RWDB_MaxMoney msgMM;
	ExplainRWDBMsg(*pMsg,msgMM);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update " + m_TNGameInfo + " set MaxMoney=:f1<bigint>,MaxMoneyTime=:f2<unsigned int> where PID=:f99<unsigned int> ";
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
int CDBWriteOperator::OnAddGameMoney(ReadWriteDBMessage* pMsg)
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
				RWDB_GameMoneyError rwdbGME;
				rwdbGME.m_AID            = AID;
				rwdbGME.m_PID            = PID;
				rwdbGME.m_nAddMoney      = nAddMoney;
				rwdbGME.m_nCurGameMoney  = stGM.m_GameMoney;
				rwdbGME.m_nAddHongBao    = nHongBao;
				rwdbGME.m_nCurHongBao    = stGM.m_HongBao;
				rwdbGME.m_strLogMsg      = "OnAddGameMoney";
				CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
				if ( pMsgManager )
				{
					pMsgManager->PushRWDBMsg(rwdbGME);
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

int CDBWriteOperator::OnAddPlayerAward(ReadWriteDBMessage* pMsg)
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
int CDBWriteOperator::OnUpdateHuiYuan(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnUpdateHuiYuan");

	RWDB_UpdateHuiYuan msgUHY;
	ExplainRWDBMsg(*pMsg,msgUHY);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		//string strSQL = "update dezhou_gameinfoex set FreeFaceTime=:f1<unsinged int>,VipLevel=:f2<short>,\
		//				VipEndTime=:f3<unsigned int> where PID=:f101<unsigned int> ";
		//otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		//TempDBStream<<msgUHY.m_FreeFaceTime<<msgUHY.m_VipLevel<<msgUHY.m_VipEndTime<<msgUHY.m_PID;
		//TempDBStream.flush();

		//if ( TempDBStream.get_rpc() == 0 )
		//{
		//	DebugError("OnUpdateHuiYuan PID=%d FreeFaceTime=%d VipLevel=%d VipEndTime=%d",
		//		msgUHY.m_PID,msgUHY.m_FreeFaceTime,msgUHY.m_VipLevel,msgUHY.m_VipEndTime);
		//}
		//TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBGameConnect,m_strDBGame);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}
int CDBWriteOperator::OnDeleteCreateInfo(ReadWriteDBMessage* pMsg)
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

int CDBWriteOperator::OnWinLossInfo(ReadWriteDBMessage* pMsg)
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
						EP=EP+:f4<bigint> \
						where PID=:f99<unsigned int>";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );
		TempDBStream<<msgWT.m_WinTimes<<msgWT.m_LossTimes<<msgWT.m_JF<<msgWT.m_EP<<msgWT.m_PID;
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

int CDBWriteOperator::OnAddPlayerInfo(ReadWriteDBMessage* pMsg)
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
				//string strSQL = "update " + m_TNGameInfo + " set ShowFace=ShowFace+1 where PID=:f101<unsigned int>";
				//otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				//TempDBStream<<msgAPI.m_PID;
				//TempDBStream.close();
			}
			break;
		case msgAPI.WinTimes:
			{
				string strSQL = "update " + m_TNGameInfo + " set WinTimes=WinTimes+1 where PID=:f101<unsigned int>";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.LossTimes:
			{
				string strSQL = "update " + m_TNGameInfo + " set LossTimes=LossTimes+1 where PID=:f101<unsigned int>";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.GameTime:
			{
				string strSQL = "update " + m_TNGameInfo + " set GameTime=GameTime+:f1<int> where PID=:f101<unsigned int>";
				otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				TempDBStream<<int(msgAPI.m_AddValue)<<msgAPI.m_PID;
				TempDBStream.close();
			}
			break;
		case msgAPI.InvitePlayer:
			{
				//string strSQL = "update " + m_TNGameInfo + " set TotalInvite = TotalInvite+1 where PID=:f101<unsigned int> ";
				//otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
				//TempDBStream<<msgAPI.m_PID;
				//TempDBStream.close();
			}
			break;
		case msgAPI.LandTime:
			{
				string strSQL = "update " + m_TNGameInfo + " set LandTime=now() where PID=:f102<unsigned int> ";
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

int CDBWriteOperator::OnIncomAndPay(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnIncomAndPay");

	RWDB_IncomeAndPay msgInPay;
	ExplainRWDBMsg(*pMsg,msgInPay);

	if ( !N_IncomeAndPay::IsValidPayIncomeFlag(msgInPay.m_Flag) || msgInPay.m_nMoney==0 )
	{
		DebugError("OnIncomAndPay PID=%d Flag=%d Money=%s",msgInPay.m_PID,msgInPay.m_Flag,N2S(msgInPay.m_nMoney).c_str());
	}

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{		
		string strSQL = "insert into " + m_TNMoneyLog + " (PID,AID,Money,Flag,Log,ActionTime) \
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

int CDBWriteOperator::OnTableInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnTableInfo");

	RWDB_TableInfo msgTI;
	ExplainRWDBMsg(*pMsg,msgTI);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
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

		otl_stream DBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
		DBStream<<msgTI.m_TableID<<msgTI.m_TableName<<msgTI.m_Password<<msgTI.m_TableRule
			<<msgTI.m_TableRuleEX<<msgTI.m_TableMatchRule<<msgTI.m_FoundByWho<<TempOTLTime<<Idx;
		DBStream.flush();

		if ( DBStream.get_rpc() == 0 )
		{
			string TableName = "dezhou_tableprivate_" + Tool::N2S(msgTI.m_ServerID);
			strSQL = "insert into " + TableName + " (Idx,ServerID,RoomID,TableID,TableName,PassWord,TableRule,TableRuleEX,MatchRule,FoundByWho,EndTime) "
				+ " values(:f1<int>,:f2<short>,:f3<short>,:f4<short>,:f5<char[50]>,:f6<char[10]>,:f7<char[255]>,:f8<char[255]>,:f28<char[255]>,:f9<unsigned int>,:f10<timestamp> ) ";
			otl_stream tempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
			tempDBStream<<Idx<<msgTI.m_ServerID<<msgTI.m_RoomID<<msgTI.m_TableID<<msgTI.m_TableName<<msgTI.m_Password
				<<msgTI.m_TableRule<<msgTI.m_TableRuleEX<<msgTI.m_TableMatchRule<<msgTI.m_FoundByWho<<TempOTLTime;
		}

		DBStream.close();
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		nRet = DB_RESULT_DBERROR;
	}

	return nRet;
}

int CDBWriteOperator::OnGameMoneyError(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnGameMoneyError");

	RWDB_GameMoneyError msgGME;
	ExplainRWDBMsg(*pMsg,msgGME);

	int nRet = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		string strSQL = "insert into dezhou_money_error (AID,PID,AddMoney,CurMoney,AddHongBao,CurHongBao,LogMsg,ActionTime) \
						values(:f1<short>,:f2<unsigned int>,:f11<bigint>,:f12<bigint>,:f21<bigint>,:f22<bigint>,:f31<char[255]>,now())";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect);
		TempDBStream<<msgGME.m_AID<<msgGME.m_PID<<msgGME.m_nAddMoney<<msgGME.m_nCurGameMoney<<msgGME.m_nAddHongBao<<msgGME.m_nCurHongBao<<msgGME.m_strLogMsg;
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

int CDBWriteOperator::OnUpdateAwardInfo(ReadWriteDBMessage* pMsg)
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

int CDBWriteOperator::OnChangeUserInfo(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnChangeUserInfo");

	RWDB_ChangeUserInfo msgCN;
	ExplainRWDBMsg(*pMsg,msgCN);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		{
			std::string strSQL = "update user_root set NickName=:f1<char[255]>,ChangeName=0 where PID=:f2<unsigned int>";
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
int CDBWriteOperator::OnPlayerLogin(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnPlayerLogin");

	RWDB_PlayerLogin msgPL;
	ExplainRWDBMsg(*pMsg,msgPL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into " + m_TNLogin + " (AID,PID,IP,LandTime) values(:f10<int>,:f11<int>,:f12<char[20]>,now())";
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

int CDBWriteOperator::OnCreateGameInfo(ReadWriteDBMessage* pMsg)
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

int CDBWriteOperator::OnReportPlayerOnline(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnReportPlayerOnline");

	RWDB_ReportPlayerOnline msgRPOL;
	ExplainRWDBMsg(*pMsg,msgRPOL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into " + m_TNOnline + " (ServerID,AID,Total,InLobby,InTable,InGame,InMatch,LogTime) \
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
int CDBWriteOperator::OnChatLog(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnChatLog");

	RWDB_ChatLog msgCL;
	ExplainRWDBMsg(*pMsg,msgCL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into " + m_TNChat + " (ServerID,RoomID,TableID,SitID,ChatPID,ToPID,Msg,ActionTime) \
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
int CDBWriteOperator::OnPlayerActionLog(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnPlayerActionLog");

	RWDB_PlayerActionLog msgPAL;
	ExplainRWDBMsg(*pMsg,msgPAL);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into " + m_TNAction + " (AID,PID,EnterRoom,EnterTable,SitDown,PlayTimes,GameTime,MatchTime,TotalTime,LogTime) \
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

int CDBWriteOperator::OnPlayerClientErr(ReadWriteDBMessage* pMsg)
{
	CLogFuncTime lft(m_FuncTime,"OnPlayerClientErr");

	RWDB_PlayerClientError msgPCE;
	ExplainRWDBMsg(*pMsg,msgPCE);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBLogConnect,m_strDBLog);
	try
	{
		std::string strSQL = "insert into dezhou_error_log (AID,PID,IP,Flag,LogTime) \
							 values(:f1<short>,:f2<unsigned int>,:f3<char[20]>,:f4<short>,now())";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBLogConnect );
		TempDBStream<<msgPCE.m_AID<<msgPCE.m_PID<<msgPCE.m_IP<<msgPCE.m_Flag;
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

int CDBWriteOperator::ReadUserGameMoney(INT16 AID,UINT32 PID,stDBGameMoney& stGM)
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

int CDBWriteOperator::UpdateChangeBankMoney(INT16 AID,UINT32 PID,INT64 nGameMoney,INT64 nBankMoney)
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