#include "DBReadOperator.h"

#include "Server.h"
#include "CDBSConfig.h"
#include "DBOperatorXY.h"

using namespace MoGui::MoGuiXY::RWDB_XY;

volatile UINT32 CDBReadOperator::s_WriteOperator = 0;
volatile UINT32 CDBReadOperator::s_ReadOperator = 0;
UINT32 CDBReadOperator::s_LastWriteOperator = 0;
UINT32 CDBReadOperator::s_LastReadOperator = 0;
UINT32 CDBReadOperator::s_LastTime = 0;

void CDBReadOperator::StaticInit()
{
	s_LastTime = int(time(NULL));
}

CDBReadOperator::CDBReadOperator(CServer* pServer)
{
	m_pServer = NULL;
	if ( pServer )
	{
		m_pServer = pServer;
		m_pConfig = pServer->GetDBConfig();

		m_CurTime = UINT32(time(NULL));
		//m_NewDayTime = UINT32(Tool::GetNewDayTime(m_CurTime));
		//m_LastCheckTime = m_CurTime - Tool::CRandom::Random_Int(1,180);

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
	}
	else
	{
		DebugError("CDBReadOperator Init Error");
	}
}


CDBReadOperator::~CDBReadOperator(void)
{
	m_DBConfigConnect.logoff();
	m_DBUserConnect.logoff();
	m_DBGameConnect.logoff();
}

void CDBReadOperator::DebugError(const char* logstr,...)
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
void CDBReadOperator::DebugInfo(const char* logstr,...)
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

void CDBReadOperator::InitTableName(UINT32 curTime)
{
	string TableHead = "thirteen_";

	m_TNGameInfo = TableHead + "gameinfo";
	m_TNWinLoss = TableHead + "winloss_log_" + Tool::GetMonthString_XXXXYY(curTime);
	m_TNMoneyLog = TableHead + "money_log_" + Tool::GetMonthString_XXXXYY(curTime);

	m_TNMatchResult = TableHead + "match_log";
	m_TNMatchWinLoss = TableHead + "matchwinloss_log";

	m_TNBotInfo      = "v_" + TableHead + "botgameinfo";

	m_TNRoomInfo = TableHead + "roomInfo";
	m_TNTableInfo = TableHead + "tableInfo";
	m_TNMoney     = "user_money";

	m_TNChat = TableHead + "chat_log";
	m_TNLogin = TableHead + "login_log";
	m_TNAction = TableHead + "player_action";

	m_TNUserProduct = TableHead + "product";

	m_TNProductInfo = TableHead + "info_product";
	m_TNFaceInfo = TableHead + "info_face";
	m_TNGiftInfo = TableHead + "info_gift";
	m_TNAreaInfo = TableHead + "info_area";
}

int CDBReadOperator::OnActiveDBConnect()
{
	m_CurTime = UINT32(time(NULL));

	static UINT32 s_NewDayTime = UINT32(Tool::GetNewDayTime(m_CurTime));
	if ( m_CurTime >= s_NewDayTime )
	{
		s_NewDayTime = UINT32(Tool::GetNewDayTime(m_CurTime));
		InitTableName(m_CurTime);
	}

	static UINT32 s_LastCheckTime = m_CurTime - Tool::CRandom::Random_Int(1,180);
	if ( m_CurTime - s_LastCheckTime >= 600 )
	{
		s_LastCheckTime = m_CurTime;

		ActiveDBConnect(m_DBConfigConnect,m_strDBConfig);
		ActiveDBConnect(m_DBUserConnect,m_strDBUser);
		ActiveDBConnect(m_DBGameConnect,m_strDBGame);

		if ( N_CeShiLog::c_FuncTimeLog )
		{
			VectorFuncTime vectorFT;
			MapFuncTime& rTempMapFT = m_FuncTime.m_mapFuncTime;
			for ( MapFuncTime::iterator itorFT=rTempMapFT.begin();itorFT!=rTempMapFT.end();++itorFT)
			{
				vectorFT.push_back(itorFT->second);
			}
			sort(vectorFT.begin(),vectorFT.end(),CCompareFuncTime());
			DebugInfo("CDBReadOperator Func Count=%d",vectorFT.size());
			for ( size_t nPos=0;nPos<vectorFT.size();++nPos)
			{
				const stFuncTimeLog& rFT = vectorFT[nPos];
				DebugInfo("%-30s TotalTicket=%-10lld Times=%-8lld Average=%-8lld Max=%-8lld Min=%-5lld",
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

void CDBReadOperator::ActiveDBConnect(otl_connect& DBConnect,std::string& strDB)
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

void CDBReadOperator::DoConnect(otl_connect& DBConnect,std::string& strDB)
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

void CDBReadOperator::CheckOTLException(const otl_exception &p,otl_connect& DBConnect,std::string& strDB)
{
	if ( p.code == 2006 || p.code == 0 )
	{
		DoConnect(DBConnect,strDB);
	}
}

void CDBReadOperator::CheckDBConnect(otl_connect& DBConnect,std::string& strDB)
{
	if ( !DBConnect.connected )
	{
		DoConnect(DBConnect,strDB);
	}
}

void CDBReadOperator::CatchDBException(const otl_exception &p)
{
	print_otl_error(p);
}
void CDBReadOperator::print_otl_error(const otl_exception &p)
{
	char msg[2048] = {0};
	_snprintf(msg, sizeof(msg)-1, "msg=%s\n", p.msg);									// print out error message
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "stm_text=%s\n", p.stm_text);	// print out SQL that caused the error
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "sqlstate=%s\n", p.sqlstate);	// print out SQLSTATE message
	_snprintf(msg+strlen(msg), sizeof(msg)-strlen(msg)-1, "var_info=%s\n", p.var_info);	// print out the variable that caused the error	
	cout<<"DBError "<<Tool::GetTimeString(time(NULL))<<" "<<msg<<endl;
	DebugError("CDBReadOperator %s",msg);
}

int CDBReadOperator::WriteTableInfo(stTableInfo& stTI)
{
	CLogFuncTime lft(m_FuncTime,"WriteTableInfo");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);

	try
	{
		string strSQL;
		otl_stream TempDBStream;
		int Idx = stTI.m_RoomID*10000 + stTI.m_TableID;

		strSQL = "update " + m_TNTableInfo + " set TableName=:f21<char[50]>,PassWord=:f22<char[20]>,TableRule=:f6<char[255]> where Idx=:f1<int>";
		TempDBStream.open(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
		TempDBStream<<stTI.m_TableName<<stTI.m_Password<<stTI.m_TableRule<<Idx;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			TempDBStream.close();

			strSQL = "insert into " + m_TNTableInfo + 
				" (Idx,ServerID,RoomID,TableID,TableName,TableRule,FoundByWho) \
				values(:f1<int>,:f2<short>,:f3<short>,:f4<short>,:f5<char[50]>,:f6<char[255]>,:f9<unsigned int> )";
			TempDBStream.open(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);

			TempDBStream<<Idx
				<<stTI.m_ServerID
				<<stTI.m_RoomID
				<<stTI.m_TableID
				<<stTI.m_TableName
				<<stTI.m_TableRule
				<<stTI.m_FoundByWho;
			TempDBStream.close();
		}
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBReadOperator::DBAddUserGameMoney(int AID,int PID,INT64 nAddMoney)
{
	CLogFuncTime lft(m_FuncTime,"DBAddUserGameMoney");

	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);

	if ( nAddMoney == 0 ) return ret;
	try
	{
		string strSQL = "update " + m_TNMoney +" set GameMoney=GameMoney+:f1<bigint> \
						where PID=:f102<int> and GameMoney+:f101<bigint> >= 0";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
		TempDBStream<<nAddMoney<<PID<<nAddMoney;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
			stDBGameMoney stGM;
			if ( ReadUserGameMoney(AID,PID,stGM) == DB_RESULT_SUCCESS )
			{
				RWDB_GameMoneyError rwdbGME;
				rwdbGME.m_AID            = AID;
				rwdbGME.m_PID            = PID;
				rwdbGME.m_nAddMoney      = nAddMoney;
				rwdbGME.m_nCurGameMoney  = stGM.m_GameMoney;
				rwdbGME.m_strLogMsg      = "DBAddUserGameMoney";
				CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
				if ( pMsgManager )
				{
					pMsgManager->PushRWDBMsg(rwdbGME);
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
int CDBReadOperator::UpdateWinLossMoney(const stWinLossMoney& stWLM)
{
	CLogFuncTime lft(m_FuncTime,"UpdateWinLossMoney");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);

	if( stWLM.m_nGameMoney!=0 || stWLM.m_nHongBao!=0 )
	{
		try
		{
			string strSQL = "update " + m_TNMoney + " set GameMoney=GameMoney+:f1<bigint>,HongBao=HongBao+:f2<bigint> \
							where PID=:f101<unsigned int> and GameMoney+:f102<bigint> >= 0 and HongBao+:f103<bigint> >= 0";

			otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
			TempDBStream<<stWLM.m_nGameMoney<<stWLM.m_nHongBao<<stWLM.m_PID<<stWLM.m_nGameMoney<<stWLM.m_nHongBao;
			TempDBStream.flush();

			if ( TempDBStream.get_rpc() == 0 )
			{
				stDBGameMoney stGM;
				if ( ReadUserGameMoney(stWLM.m_AID,stWLM.m_PID,stGM) == DB_RESULT_SUCCESS )
				{
					RWDB_GameMoneyError rwdbGME;
					rwdbGME.m_AID            = stWLM.m_AID;
					rwdbGME.m_PID            = stWLM.m_PID;
					rwdbGME.m_nAddMoney      = stWLM.m_nGameMoney;
					rwdbGME.m_nCurGameMoney  = stGM.m_GameMoney;
					rwdbGME.m_nAddHongBao    = stWLM.m_nHongBao;
					rwdbGME.m_nCurHongBao    = stGM.m_HongBao;
					rwdbGME.m_strLogMsg      = "UpdateWinLossMoney";
					CRWDBMsgManage*    pMsgManager = m_pServer->GetRWDBManager();
					if ( pMsgManager )
					{
						pMsgManager->PushRWDBMsg(rwdbGME);
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

int CDBReadOperator::UpdateChangeBankMoney(INT16 AID,UINT32 PID,INT64 nGameMoney,INT64 nBankMoney)
{
	CLogFuncTime lft(m_FuncTime,"UpdateChangeBankMoney");

	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "update " + m_TNMoney + " set GameMoney=GameMoney+:f1<bigint>,BankMoney=BankMoney+:f2<bigint> \
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

int CDBReadOperator::CreatePlayerGameInfo(stCreateGameInfo& stCPGI)
{
	CLogFuncTime lft(m_FuncTime,"CreatePlayerGameInfo");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		{
			const string& TN = m_TNMoney;
			INT64 nGameMoney = stCPGI.m_OrigenMoney + stCPGI.m_LandMoney;
			string strGameInfo = "insert into " + TN + " (PID,AID,GameMoney) \
								 select :f1<int>,:f2<int>,:f11<bigint> \
								 from dual where not exists \
								 (select * from " + TN + " where " + TN + ".PID = :f11<int>)";
			otl_stream GameInfoStream( OTL_STREAMBUF_SIZE,strGameInfo.c_str(),m_DBGameConnect );
			GameInfoStream<<stCPGI.m_PID<<stCPGI.m_AID<<nGameMoney<<stCPGI.m_PID;
		}
		{
			const string& TN = m_TNGameInfo;
			string strGameInfo = "insert into " + TN + " (PID,AID) select :f1<int>,:f2<int> \
								 from dual where not exists \
								 (select * from " + TN  + " where " + TN +".PID = :f11<int>)";
			otl_stream GameInfoStream( OTL_STREAMBUF_SIZE,strGameInfo.c_str(),m_DBGameConnect );
			GameInfoStream<<stCPGI.m_PID<<stCPGI.m_AID<<stCPGI.m_PID;
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

int CDBReadOperator::UpdateUserProduct(int Idx,INT16 UseFlag)
{
	CLogFuncTime lft(m_FuncTime,"UpdateUserProduct");
	InterlockedIncrement(&s_WriteOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "update " + m_TNUserProduct +" set UseFlag=:f1<short> where Idx=:f2<int>";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<UseFlag<<Idx;
		TempDBStream.flush();

		if ( TempDBStream.get_rpc() == 0 )
		{
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

int CDBReadOperator::ReadUserGameInfo(int AID,int PID,stDBGameInfo& stUGI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserGameInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL =
			"select PID,AID,GameMoney,Medal,Diamond,GoldenPig,PigFood,Forbid,JF,EP,WinTimes,LossTimes,DrawnTimes,EscapeTimes,GameTime,\
			MaxMoney,MaxMoneyTime,MaxPai,MaxPaiTime,MaxWin,MaxWinTime,JoinTime \
			from v_thirteen_gameinfo where PID =:f1<int> limit 1";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );

		TempDBStream<<PID;
		if ( !TempDBStream.eof() )
		{
			otl_datetime TempOTLTime;

			TempDBStream >> stUGI.m_PID;
			TempDBStream >> stUGI.m_AID;
			TempDBStream >> stUGI.m_stGameInfo.m_nGameMoney;
			TempDBStream >> stUGI.m_stGameInfo.m_nMedal;
			TempDBStream >> stUGI.m_stGameInfo.m_Diamond;
			TempDBStream >> stUGI.m_stGameInfo.m_GoldenPig;
			TempDBStream >> stUGI.m_stGameInfo.m_PigFood;

			TempDBStream >> stUGI.m_stGameInfo.m_Forbid;
			TempDBStream >> stUGI.m_stGameInfo.m_nJF;
			TempDBStream >> stUGI.m_stGameInfo.m_nEP;
			TempDBStream >> stUGI.m_stGameInfo.m_nWinTimes;
			TempDBStream >> stUGI.m_stGameInfo.m_nLossTimes;
			TempDBStream >> stUGI.m_stGameInfo.m_DrawnTimes;
			TempDBStream >> stUGI.m_stGameInfo.m_EscapeTimes;
			TempDBStream >> stUGI.m_stGameInfo.m_nGameTime;

			TempDBStream >> stUGI.m_stGameInfo.m_MaxMoney;
			TempDBStream >> stUGI.m_stGameInfo.m_MaxMoneyTime;
			TempDBStream >> stUGI.m_stGameInfo.m_MaxPai;
			TempDBStream >> stUGI.m_stGameInfo.m_MaxPaiTime;
			TempDBStream >> stUGI.m_stGameInfo.m_MaxWin;
			TempDBStream >> stUGI.m_stGameInfo.m_MaxWinTime;
			TempDBStream >> TempOTLTime;

			stUGI.m_stGameInfo.m_JoinTime = UINT32(Tool::GetTimeFromOTLTime(TempOTLTime));
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

int CDBReadOperator::ReadUserDataInfoByPID(int PID,stDBUserDataInfo& stUDI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserDataInfoByPID");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "select PID,AID,Sex,UserName,Password,NickName,HardCode,HeadID,HeadPicURL,HomePageURL,City,InvitePID \
						from user_root where PID=:f1<int> limit 1";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<PID;

		if ( !TempDBStream.eof() )
		{
			otl_datetime OTLTime;

			TempDBStream >> stUDI.m_PID;
			TempDBStream >> stUDI.m_AID;
			TempDBStream >> stUDI.m_stUDI.m_Sex;
			TempDBStream >> stUDI.m_stUDI.m_UserName;
			TempDBStream >> stUDI.m_stUDI.m_Password;
			TempDBStream >> stUDI.m_stUDI.m_NickName;
			TempDBStream >> stUDI.m_stUDI.m_HardCode;
			TempDBStream >> stUDI.m_stUDI.m_HeadID;
			TempDBStream >> stUDI.m_stUDI.m_HeadPicURL;
			TempDBStream >> stUDI.m_stUDI.m_HomePageURL;
			TempDBStream >> stUDI.m_stUDI.m_City;
			TempDBStream >> stUDI.m_stUDI.m_InvitePID;
		}
		else
		{
			DebugError("ReadUserDataInfoByPID No Player PID=%d",PID);
			ret = DB_RESULT_NOPLAYER;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		stUDI.Init();
		CatchDBException(p);
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBReadOperator::ReadUserDataInfoByUserName(int AID,const string& UserName,stDBUserDataInfo& stUDI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserDataInfoByUserName");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "select PID,AID,Sex,UserName,Password,NickName,HardCode,HeadID,HeadPicURL,HomePageURL,City,InvitePID \
						from user_root where AID=:f1<int> and UserName=:f2<char[255]> limit 1";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<AID<<UserName;

		if ( !TempDBStream.eof() )
		{
			otl_datetime OTLTime;

			TempDBStream >> stUDI.m_PID;
			TempDBStream >> stUDI.m_AID;
			TempDBStream >> stUDI.m_stUDI.m_Sex;
			TempDBStream >> stUDI.m_stUDI.m_UserName;
			TempDBStream >> stUDI.m_stUDI.m_Password;
			TempDBStream >> stUDI.m_stUDI.m_NickName;
			TempDBStream >> stUDI.m_stUDI.m_HardCode;
			TempDBStream >> stUDI.m_stUDI.m_HeadID;
			TempDBStream >> stUDI.m_stUDI.m_HeadPicURL;
			TempDBStream >> stUDI.m_stUDI.m_HomePageURL;
			TempDBStream >> stUDI.m_stUDI.m_City;
			TempDBStream >> stUDI.m_stUDI.m_InvitePID;
		}
		else
		{
			DebugError("ReadUserDataInfoByUserName No Player UserName=%s",UserName.c_str());
			ret = DB_RESULT_NOPLAYER;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		stUDI.Init();
		CatchDBException(p);
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBReadOperator::ReadUserDataInfoByHardCode(const string& HardCode,stDBUserDataInfo& stUDI)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserDataInfoByHardCode");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		string strSQL = "select PID,AID,Sex,UserName,Password,NickName,HardCode,HeadID,HeadPicURL,HomePageURL,City,InvitePID \
						from user_root where UserName=:f2<char[255]> limit 1";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect );
		TempDBStream<<HardCode;

		if ( !TempDBStream.eof() )
		{
			otl_datetime OTLTime;

			TempDBStream >> stUDI.m_PID;
			TempDBStream >> stUDI.m_AID;
			TempDBStream >> stUDI.m_stUDI.m_Sex;
			TempDBStream >> stUDI.m_stUDI.m_UserName;
			TempDBStream >> stUDI.m_stUDI.m_Password;
			TempDBStream >> stUDI.m_stUDI.m_NickName;
			TempDBStream >> stUDI.m_stUDI.m_HardCode;
			TempDBStream >> stUDI.m_stUDI.m_HeadID;
			TempDBStream >> stUDI.m_stUDI.m_HeadPicURL;
			TempDBStream >> stUDI.m_stUDI.m_HomePageURL;
			TempDBStream >> stUDI.m_stUDI.m_City;
			TempDBStream >> stUDI.m_stUDI.m_InvitePID;
		}
		else
		{
			DebugError("ReadUserDataInfoByHardCode No Player HardCode=%s",HardCode.c_str());
			ret = DB_RESULT_NOPLAYER;
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		stUDI.Init();
		CatchDBException(p);
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBReadOperator::ReadBotPlayerData(INT16 AID,UINT32 StartPID,UINT32 EndPID,VectorBotPlayerData& vectorPD)
{
	CLogFuncTime lft(m_FuncTime,"ReadBotPlayerData");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "select PID,Sex,NickName,HeadID,HeadPicURL,HomePageURL,City,InvitePID, \
						GameMoney,Medal,Diamond,GoldenPig,PigFood, \
						JF,EP,WinTimes,LossTimes,DrawTimes,EscapeTimes,GameTime,\
						MaxMoney,MaxMoneyTime,MaxPai,MaxPaiTime,MaxWin,MaxWinTime,BotLevel,JoinTime from " 
						+ m_TNBotInfo + " where AID=:f3<short> and PID>=:f1<unsigned int> and PID<=:f2<unsigned int> ";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect );

		TempDBStream<<AID<<StartPID<<EndPID;

		while ( !TempDBStream.eof() )
		{
			otl_datetime TempOTLTime;
			stBotPlayerData stPD;

			stPD.m_AID    = AID;
			TempDBStream >> stPD.m_PID;
			TempDBStream >> stPD.m_stUserDataInfo.m_Sex;
			TempDBStream >> stPD.m_stUserDataInfo.m_NickName;
			TempDBStream >> stPD.m_stUserDataInfo.m_HeadID;
			TempDBStream >> stPD.m_stUserDataInfo.m_HeadPicURL;
			TempDBStream >> stPD.m_stUserDataInfo.m_HomePageURL;
			TempDBStream >> stPD.m_stUserDataInfo.m_City;
			TempDBStream >> stPD.m_stUserDataInfo.m_InvitePID;			

			TempDBStream >> stPD.m_stGameInfo.m_nGameMoney;
			TempDBStream >> stPD.m_stGameInfo.m_nMedal;
			TempDBStream >> stPD.m_stGameInfo.m_Diamond;
			TempDBStream >> stPD.m_stGameInfo.m_GoldenPig;
			TempDBStream >> stPD.m_stGameInfo.m_PigFood;

			TempDBStream >> stPD.m_stGameInfo.m_nJF;
			TempDBStream >> stPD.m_stGameInfo.m_nEP;
			TempDBStream >> stPD.m_stGameInfo.m_nWinTimes;
			TempDBStream >> stPD.m_stGameInfo.m_nLossTimes;			
			TempDBStream >> stPD.m_stGameInfo.m_DrawnTimes;
			TempDBStream >> stPD.m_stGameInfo.m_EscapeTimes;
			TempDBStream >> stPD.m_stGameInfo.m_nGameTime;			

			TempDBStream >> stPD.m_stGameInfo.m_MaxMoney;
			TempDBStream >> stPD.m_stGameInfo.m_MaxMoneyTime;
			TempDBStream >> stPD.m_stGameInfo.m_MaxPai;
			TempDBStream >> stPD.m_stGameInfo.m_MaxPaiTime;
			TempDBStream >> stPD.m_stGameInfo.m_MaxWin;
			TempDBStream >> stPD.m_stGameInfo.m_MaxWinTime;

			TempDBStream >> stPD.m_BotLevel;
			TempDBStream >> TempOTLTime;
			stPD.m_stGameInfo.m_JoinTime = UINT32(Tool::GetTimeFromOTLTime(TempOTLTime));

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

int CDBReadOperator::ReadFaceInfo(VectorFaceInfo& vectorFI)
{
	CLogFuncTime lft(m_FuncTime,"ReadFaceInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
	try
	{
		vectorFI.clear();

		stFaceInfo stFI;
		string strSQL = "select FaceID,Type,PriceFlag,Price,MinPrice,MaxPrice from " + m_TNFaceInfo;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
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
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBReadOperator::ReadGiftInfo(VectorGiftInfo& vectorGI)
{
	CLogFuncTime lft(m_FuncTime,"ReadGiftInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
	try
	{
		vectorGI.clear();

		stGiftInfo stGI;
		string strSQL = "select GiftID,Type,PriceFlag,Price,MinPrice,MaxPrice,Rebate,CurLastTime,TotalLastTime from " + m_TNGiftInfo;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
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
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBReadOperator::ReadProductInfo(VectorProductInfo& vectorPI)
{
	CLogFuncTime lft(m_FuncTime,"ReadProductInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
	try
	{
		vectorPI.clear();

		ProductInfo stPI;
		string strSQL = "select ProductID,Price,ProductType,ProductRule from " + m_TNProductInfo;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
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
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBReadOperator::ReadMatchInfo(INT16 ServerID,VectorDBMatchInfo& vectorMI)
{
	CLogFuncTime lft(m_FuncTime,"ReadMatchInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
	try
	{
		stDBMatchInfo stMI;
		string strBlind,strAward;
		otl_datetime oltTime;

		string strSQL = "select MatchID,MatchType,Ticket,TakeMoney,StartMoney,Blind,Award,StrRule,StartTime,StartInterval from " + m_TNMatchInfo;
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect );

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
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBReadOperator::ReadAreaInfo(VectorDBAreaInfo& vectorArea)
{
	CLogFuncTime lft(m_FuncTime,"ReadAreaInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
	try
	{
		vectorArea.clear();

		stDBAreaInfo stAI;
		string strSQL = "select AID,AreaName,JoinMoney,LandMoney from " + m_TNAreaInfo;
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
		while ( !TempDBStream.eof() )
		{
			stAI.Init();
			TempDBStream >> stAI.m_AID;
			TempDBStream >> stAI.m_AreaName;
			TempDBStream >> stAI.m_JoinRule;
			TempDBStream >> stAI.m_LandRule;

			vectorArea.push_back(stAI);
		}
		TempDBStream.close();
	}
	catch(otl_exception &p)
	{
		vectorArea.clear();
		CatchDBException(p);
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}
int CDBReadOperator::RegisterUser(const stRegisterUser& stRU)
{
	CLogFuncTime lft(m_FuncTime,"RegisterUser");

	int retPID = 0;
	CheckDBConnect(m_DBUserConnect,m_strDBUser);
	try
	{
		if ( stRU.m_nFlag == stRegisterUser::Flag_HardCode )
		{
			string strSQL = "insert into user_root (AID,NickName,HardCode,JoinTime) \
							values(:f1<int>,:f2<char[50]>,:f3<char[50]>,now()) ";
			otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect);
			TempDBStream<<stRU.m_AID<<stRU.m_NickName<<stRU.m_HardCode;
			TempDBStream.close();

			strSQL = "select @@IDENTITY";
			TempDBStream.open(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect);
			if( !TempDBStream.eof() )
			{
				TempDBStream >> retPID;
			}
		}
		else if ( stRU.m_nFlag == stRegisterUser::Flag_UserName )
		{
			string strSQL = "insert into user_root (AID,UserName,Password,NickName,JoinTime) \
							values(:f1<int>,:f2<char[50]>,:f3<char[50]>,:f4<char[50]>,now() ) ";
			otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect);
			TempDBStream<<stRU.m_AID<<stRU.m_UserName<<stRU.m_Password<<stRU.m_NickName;
			TempDBStream.close();

			strSQL = "select @@IDENTITY";
			TempDBStream.open(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBUserConnect);
			if( !TempDBStream.eof() )
			{
				TempDBStream >> retPID;
			}
		}
	}
	catch(otl_exception &p)
	{
		CatchDBException(p);
		CheckOTLException(p,m_DBUserConnect,m_strDBUser);
	}
	return retPID;
}

int CDBReadOperator::ReadUserGameMoney(INT16 AID,UINT32 PID,stDBGameMoney& stGM)
{
	CLogFuncTime lft(m_FuncTime,"ReadUserGameMoney");

	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBGameConnect,m_strDBGame);
	try
	{
		string strSQL = "select GameMoney,HongBao from " + m_TNMoney + " where PID=:f1<unsigned int> limit 1";
		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBGameConnect);
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

int CDBReadOperator::ReadUserProduct(INT16 AID,UINT32 PID,VectorUserProduct& vectorUP)
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

int CDBReadOperator::ReadAllUserProduct(int MaxIdx,VectorUserProduct& vectorUP)
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

int CDBReadOperator::ReadRoomInfo(INT16 ServerID,VectorRoomInfo& vectorRI)
{
	CLogFuncTime lft(m_FuncTime,"ReadRoomInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;

	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
	try
	{
		UINT32 curTime = UINT32(time(NULL));
		string strSQL = "select ServerID,RoomID,RoomName,Password,AreaRule,RoomRule,RoomRuleEX,MatchRule,EndTime from "
			+ m_TNRoomInfo + " where ServerID=:f<short> ";
		otl_stream TempDBStream( OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect );

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
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
		ret = DB_RESULT_DBERROR;
	}

	return ret;
}

int CDBReadOperator::ReadTableInfo(INT16 ServerID,VectorTableInfo& vectorTI)
{
	CLogFuncTime lft(m_FuncTime,"ReadTableInfo");
	InterlockedIncrement(&s_ReadOperator);

	int ret = DB_RESULT_SUCCESS;
	CheckDBConnect(m_DBConfigConnect,m_strDBConfig);
	try
	{
		UINT32 curTime = UINT32(time(NULL));
		string strSQL = "select RoomID,TableID,TableName,PassWord,TableRule,FoundByWho,EndTime from " + m_TNTableInfo +
			" where ServerID=:f<short>";

		otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
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
		CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
	}

	//读取自建桌子的数据
	//try
	//{
	//	UINT32 curTime = UINT32(time(NULL));
	//	string strTableName = "dezhou_tableprivate_"+N2S(ServerID);
	//	string strSQL = "select RoomID,TableID,TableName,PassWord,TableRule,TableRuleEX,MatchRule,FoundByWho,EndTime from " + strTableName +
	//		" where ServerID=:f<short>";

	//	otl_stream TempDBStream(OTL_STREAMBUF_SIZE,strSQL.c_str(),m_DBConfigConnect);
	//	TempDBStream<<ServerID;
	//	while ( !TempDBStream.eof() )
	//	{
	//		stTableInfo TI;
	//		otl_datetime otl_endtime;

	//		TI.m_ServerID = ServerID;

	//		TempDBStream >> TI.m_RoomID;
	//		TempDBStream >> TI.m_TableID;
	//		TempDBStream >> TI.m_TableName;
	//		TempDBStream >> TI.m_Password;
	//		TempDBStream >> TI.m_TableRule;
	//		TempDBStream >> TI.m_TableRuleEX;
	//		TempDBStream >> TI.m_MatchRule;
	//		TempDBStream >> TI.m_FoundByWho;
	//		TempDBStream >> otl_endtime;

	//		TI.m_TableEndTime = UINT32(Tool::GetTimeFromOTLTime(otl_endtime));

	//		if ( TI.m_TableEndTime == 0 || TI.m_TableEndTime > curTime )
	//		{
	//			vectorTI.push_back(TI);
	//		}
	//	}
	//	TempDBStream.close();
	//}
	//catch(otl_exception &p)
	//{
	//	ret = DB_RESULT_DBERROR;
	//	CatchDBException( p );
	//	CheckOTLException(p,m_DBConfigConnect,m_strDBConfig);
	//}

	return ret;
}

int CDBReadOperator::ReadUserAward(INT16 AID,UINT32 PID,VectorAward& vectorAward)
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