#include "server.h"
#include "DSTest.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib") 

#include "boost/shared_ptr.hpp"

#include "DBWriteOperator.h"
#include "gamefunc.h"

long DBServerPlayerInfo::s_nMoneyLogCount = 0;

int CDBMsgThread::Run( void )
{
	assert(m_pDBServer);
	CRWDBMsgManage *pManager = m_pDBServer->GetRWDBManager();
	boost::shared_ptr<CDBWriteOperator> pDoDBMsg(new CDBWriteOperator(m_pDBServer));

	while ( IsRunning() )
	{
		pDoDBMsg->OnActiveDBConnect();
		if ( PRWDBMsg pMsg = pManager->PopRWDBMsg() )
		{
			pDoDBMsg->OnRWDBMsg(pMsg.get());
		}
	}
	return 0;
}

CServer::CServer()
{
	InitLogger( "dz_dbs_log", true, LOGLEVEL_ALL );
	DebugLog("DBServer Init................................................");

	Tool::CRandom::InitRandSeed();
	CDBReadOperator::StaticInit();
	CDBWriteOperator::StaticInit();

	m_bInitComplete = false;
	m_bStartDBThread = false;

	m_DBConfig.Init();
	InitCeShiLog();

	m_pPool = CreateConnectPool( );
	m_pPool->SetCallback( this );
	m_pPool->Start( m_DBConfig.m_Port, m_DBConfig.m_ConnectCount, m_DBConfig.m_OutCount );

	m_CurTime = (UINT32)(time( NULL ));
	m_nServerStartTime = m_CurTime;
	//m_CheckGSActiveTime = m_CurTime;
	//m_CheckPlayerOnlineTime = m_CurTime;
	//m_CheckXieYiInOutTime = m_CurTime;
	//m_CheckXieYiSpeedTime = m_CurTime;
	//m_CheckUserProductTime = m_CurTime;
	//m_CheckSendUserProductTime = m_CurTime;
	//m_CheckBotPlayerCached = m_CurTime+3600;

	//m_OneMinuteTime = m_CurTime + 60;
	//m_TenMinuteTime = m_CurTime + 600;

	m_MaxProductIdx = 0;

	m_memOperator.Init(m_DBConfig.m_MemcachIpPort);
	m_pDBOperator = new CDBReadOperator(this);
	
	for ( int nCount=0;nCount<m_DBConfig.m_ThreadCount;++nCount)
	{
		CDBMsgThread* pMsgThread = new CDBMsgThread("Write Log "+Tool::N2S(nCount+1));
		pMsgThread->m_pDBServer = this;
		m_vecDBMsgThread.push_back(pMsgThread);
	}

	InitBaseInfo();
	m_bInitComplete = true;

	DebugLogOut("InitComplete = true");
}

void CServer::DebugError(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1]={0};
	va_list args;
	va_start(args,logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if( len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_ERROR,logbuf);
		printf_s("%s Error: %s \n",Tool::GetTimeString().c_str(),logbuf);
	}
}

void CServer::DebugInfo(const char* logstr,...)
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

void CServer::DebugLogOut(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_INFO,logbuf);
		printf_s("%s %s \n",Tool::GetTimeString().c_str(),logbuf);
	}
}

void CServer::DebugLog(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
	va_list args;
	va_start(args, logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if (len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_INFO,logbuf);
	}
}

void CServer::InitCeShiLog()
{
	if ( N_CeShiLog::c_NoCheckLogin )DebugLogOut("对测试区的玩家不检验认证直接通过");
	if ( N_CeShiLog::c_RWDBLog )DebugLogOut("记录读写数据库的速度以及RWDB的容量及压力");
	if ( N_CeShiLog::c_DBOnTimeDiff )DebugLogOut("服务器每%d秒输出一次OnTimer运行时间",N_CeShiLog::c_DBOnTimeDiff);
	if ( N_CeShiLog::c_DBMaxOnTime )DebugLogOut("服务器OnTimer运行超过%d毫秒时将输出",N_CeShiLog::c_DBMaxOnTime);
	if ( N_CeShiLog::c_FuncTimeLog )DebugLogOut("记录每个函数的运行时间");
	if ( N_CeShiLog::c_MoneyLog )DebugLogOut("记录玩家游戏币变化的每个细节");
	if ( N_CeShiLog::c_RightLog )DebugLogOut("记录玩家RIGHT变化的细节");
	if ( N_CeShiLog::c_XieYiLog )DebugLogOut("记录协议的速度大小以及协议分类的细节");
	if ( N_CeShiLog::c_NewMomory )DebugLogOut("记录内存分配回收的情况");
}

CServer::~CServer( void )
{
	MapClientSocket::iterator itorClient;
	for(itorClient=m_ClientsSocket.begin();itorClient!=m_ClientsSocket.end();++itorClient)
	{
		if( itorClient->first )
		{
			itorClient->first->Close();
		}
		safe_delete(itorClient->second);
	}
	m_ClientsSocket.clear();

	MapDBPlayerInfo::iterator itorPlayerInfo;
	for (itorPlayerInfo=m_PlayerInfos.begin();itorPlayerInfo!=m_PlayerInfos.end();++itorPlayerInfo)
	{
		safe_delete(itorPlayerInfo->second);
	}
	m_PlayerInfos.clear();

	m_pPool->Stop( );
	DestoryConnectPool( m_pPool );

	for ( size_t Pos=0;Pos<m_vecDBMsgThread.size();++Pos )
	{
		m_vecDBMsgThread[Pos]->Terminate(50);
		safe_delete(m_vecDBMsgThread[Pos]);
	}

	safe_delete(m_pDBOperator);
	
	DebugLog("CServer DB Destroy End............");
	FiniLogger( );
}
bool CServer::OnPriorityEvent( void )
{
	return false;
}
void CServer::OnOneMinite(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnOneMinite");

	static UINT32 s_OneMiniteTime = 0;
	if ( s_OneMiniteTime == 0 )
	{
		s_OneMiniteTime += curTime + 60;
	}
	else if ( curTime > s_OneMiniteTime )
	{
		s_OneMiniteTime += curTime + 60;
		
		CLogFuncTime lft(m_DBSFuncTime,"OnOneMinite");

		m_pDBOperator->OnActiveDBConnect();
	}
}
void CServer::OnTenMinite(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnTenMinite");

	static UINT32 s_TemMiniteTime = 0;
	if ( s_TemMiniteTime == 0 )
	{
		s_TemMiniteTime += curTime + 600;
	}
	else if ( curTime > s_TemMiniteTime )
	{
		s_TemMiniteTime += curTime + 600;

		CLogFuncTime lft(m_DBSFuncTime,"OnTenMinite");
		DebugInfo("OnTenMinite Start...");

		HANDLE hCurProc = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(hCurProc, &pmc, sizeof(pmc));
		DebugLog("%-10d            显示内存消耗量",pmc.PagefileUsage/1024);
		DebugLog("%-10d            峰值内存",pmc.PeakWorkingSetSize/1024);
		DebugLog("%-10d %-10d 工作内存",pmc.WorkingSetSize/1024,pmc.QuotaPeakPagedPoolUsage/1024);
		DebugLog("%-10d %-10d 分页池",pmc.QuotaPagedPoolUsage/1024,pmc.QuotaPeakNonPagedPoolUsage/1024);
		DebugLog("%-10d %-10d 缓冲池",pmc.QuotaNonPagedPoolUsage/1024,pmc.PeakPagefileUsage/1024);

		int nOnLineCount = 0;
		for ( MapDBPlayerInfo::iterator itorPlayerInfo=m_PlayerInfos.begin();itorPlayerInfo!=m_PlayerInfos.end();itorPlayerInfo++ )
		{
			if ( itorPlayerInfo->second->IsOnLine() )
			{
				nOnLineCount++;
			}
		}
		DebugLogOut("总数 %d 在线 %d",m_PlayerInfos.size(),nOnLineCount);
		DebugLogOut("PlayerMoneyLogCount=%d",DBServerPlayerInfo::s_nMoneyLogCount);

		if ( N_CeShiLog::c_RWDBLog )
		{
			int nUseTime = curTime - CDBWriteOperator::s_LastTime;
			DebugLogOut("Read  Total=%-10s Speed=%-5d",Tool::N2S(CDBWriteOperator::s_ReadOperator).c_str(),
				int((CDBWriteOperator::s_ReadOperator-CDBWriteOperator::s_LastReadOperator)/nUseTime));

			DebugLogOut("Write Total=%-10s Speed=%-5d",Tool::N2S(CDBWriteOperator::s_WriteOperator).c_str(),
				int((CDBWriteOperator::s_WriteOperator-CDBWriteOperator::s_LastWriteOperator)/nUseTime));

			CDBWriteOperator::s_LastTime = curTime;
			CDBWriteOperator::s_LastReadOperator  = CDBWriteOperator::s_ReadOperator;
			CDBWriteOperator::s_LastWriteOperator = CDBWriteOperator::s_WriteOperator;


			nUseTime = curTime - CDBReadOperator::s_LastTime;
			DebugLogOut("Read  Total=%-10s Speed=%-5d",Tool::N2S(CDBReadOperator::s_ReadOperator).c_str(),
				int((CDBReadOperator::s_ReadOperator-CDBReadOperator::s_LastReadOperator)/nUseTime));

			DebugLogOut("Write Total=%-10s Speed=%-5d",Tool::N2S(CDBReadOperator::s_WriteOperator).c_str(),
				int((CDBReadOperator::s_WriteOperator-CDBReadOperator::s_LastWriteOperator)/nUseTime));

			CDBReadOperator::s_LastTime = curTime;
			CDBReadOperator::s_LastReadOperator  = CDBReadOperator::s_ReadOperator;
			CDBReadOperator::s_LastWriteOperator = CDBReadOperator::s_WriteOperator;

			DebugLogOut("RWDBMessage=%d DBPlayerInfo=%d CDBSSocket=%d MsgBlock=%d",
				ReadWriteDBMessage::GetTotalCount(),DBServerPlayerInfo::GetTotalCount(),
				CDBServerSocket::GetTotalCount(),MessageBlock::GetTotalCount() );

			DebugLogOut("RWDBMessage=%d Use=%d New=%s Delete=%s Diff=%s",
				ReadWriteDBMessage::GetTotalCount(),ReadWriteDBMessage::GetUseCount(),
				N2S(ReadWriteDBMessage::GetNewTimes()).c_str(),N2S(ReadWriteDBMessage::GetDeleteTimes()).c_str(),
				N2S(ReadWriteDBMessage::GetNewTimes()-ReadWriteDBMessage::GetDeleteTimes()).c_str() );
		}

		if ( N_CeShiLog::c_FuncTimeLog )
		{
			VectorFuncTime vectorFT;
			MapFuncTime& rTempMapFT = m_DBSFuncTime.m_mapFuncTime;
			for ( MapFuncTime::iterator itorFT=rTempMapFT.begin();itorFT!=rTempMapFT.end();++itorFT)
			{
				vectorFT.push_back(itorFT->second);
			}
			sort(vectorFT.begin(),vectorFT.end(),CCompareFuncTime());
			DebugLog("Func Count=%d",rTempMapFT.size());
			for ( size_t nPos=0;nPos<vectorFT.size();++nPos)
			{
				const stFuncTimeLog& rFT = vectorFT[nPos];
				DebugLog("DBS %-30s TotalTicket=%-10lld Times=%-8lld Average=%-8lld Max=%-8lld Min=%-5lld",
					rFT.m_strFuncName.c_str(),
					rFT.m_TotalTicket/1000,
					rFT.m_nTimes,
					rFT.m_TotalTicket/max(rFT.m_nTimes,1),
					rFT.m_MaxTicket,
					rFT.m_MinTicket );
			}
		}

		DebugLog("OnTenMinite End");
	}	
}
void CServer::OnHour(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnHour");

	static UINT32 s_HourTime = 0;
	if ( s_HourTime == 0 )
	{
		s_HourTime += curTime + 3600;
	}
	else if ( curTime > s_HourTime )
	{
		s_HourTime += curTime + 3600;
	}
}

void CServer::OnNewDay(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnNewDay");

	static UINT32 s_NewDayTime = 0;
	if ( s_NewDayTime == 0 )
	{
		s_NewDayTime = UINT32(Tool::GetNewDayTime(curTime));
	}
	else if ( curTime > s_NewDayTime )
	{
		CLogFuncTime lft(m_DBSFuncTime,"OnNewDay");		

		s_NewDayTime = UINT32(Tool::GetNewDayTime(m_CurTime));
	}	
}

void CServer::CheckClientSocketActive(UINT32 curTime)
{
	TraceStackPath logTP("CServer::CheckClientSocketActive");

	static UINT32 s_CheckGSActiveTime = 0;
	if ( s_CheckGSActiveTime == 0 )
	{
		s_CheckGSActiveTime = curTime + TIME_CHECK_SERVERACTIVE;
	}
	else if ( curTime >= s_CheckGSActiveTime )
	{
		s_CheckGSActiveTime = curTime + TIME_CHECK_SERVERACTIVE;

		CLogFuncTime lft(m_DBSFuncTime,"CheckClientSocketActive");
		DebugInfo("CheckClientSocketActive Start...");

		CDBServerSocket *pDBSocket = nullptr;
		for ( MapClientSocket::iterator itorClient=m_ClientsSocket.begin();itorClient!=m_ClientsSocket.end();itorClient++ )
		{
			pDBSocket = itorClient->second;
			UINT32 nActiveTime = pDBSocket->GetActiveTime();
			int nPassTime = curTime - nActiveTime;
#ifdef CESHI
			if ( nPassTime>=1800 )
#else
			if ( nPassTime>=TIME_SERVER_ONLINE )
#endif
			{
				if ( pDBSocket->GetOutTimes() >= T_MaxSocketOutTimes )
				{
					DebugError("DB GS Cur=%s ActiveTime=%s OutTime=%d connect=%d ",
						Tool::GetTimeString(curTime).c_str(),
						Tool::GetTimeString(nActiveTime).c_str(),
						nPassTime, reinterpret_cast<int>(itorClient->first) );
					pDBSocket->Close();
				}
				else
				{
					pDBSocket->OnCheckHeart();
				}		
			}		
		}
		DebugInfo("CheckClientSocketActive End");
	}	
}

void CServer::CheckPlayerOnLine(UINT32 curTime)
{
	TraceStackPath logTP("CServer::CheckPlayerOnLine");

	UINT32 MaxCheckTime = max(600,UINT32(INT64(600)*m_PlayerInfos.size()/Max_PlayerData_In_DBS));
	MaxCheckTime = min(1800,MaxCheckTime);

	static UINT32 s_CheckPlayerOnlineTime = 0;
	if ( s_CheckPlayerOnlineTime == 0 )
	{
		s_CheckPlayerOnlineTime = curTime;
	}
	else if ( curTime - s_CheckPlayerOnlineTime >= MaxCheckTime && m_PlayerInfos.size() )
	{
		s_CheckPlayerOnlineTime = curTime ;

		CLogFuncTime lft(m_DBSFuncTime,"CheckPlayerOnLine");
		DebugLog("CheckPlayerOnLine Start");

		vector<UINT32> vecDeletePD;
		DBServerPlayerInfo* pPlayerInfo;
		MapCheckOnline PlayerNotActive;
		MapDBPlayerInfo::iterator itorPlayerInfo;

		UINT32 MaxExistTime = UINT32(INT64(Max_PlayerData_In_DBS)*N_Time::Day_1/std::max<size_t>(m_PlayerInfos.size(),1));
		MaxExistTime = min(MaxExistTime,N_Time::Week_1);
		if ( m_DBConfig.CeshiMode() )
		{
			MaxExistTime = min(MaxExistTime,N_Time::Day_1);
		}
		for ( itorPlayerInfo=m_PlayerInfos.begin();itorPlayerInfo!=m_PlayerInfos.end();itorPlayerInfo++)
		{
			pPlayerInfo = itorPlayerInfo->second;

			if ( pPlayerInfo->IsBotPlayer() )
			{
				continue;
			}
			else if ( pPlayerInfo->m_ServerID <= 0 || pPlayerInfo->m_PID <= 0 )
			{
				DebugError("CheckPlayerOnlineTime PID=%d SID=%d",pPlayerInfo->m_PID,pPlayerInfo->m_ServerID);
				vecDeletePD.push_back(pPlayerInfo->m_PID);
				continue;
			}

			if ( curTime-pPlayerInfo->GetActiveTime()>3600 && pPlayerInfo->IsOnLine() )
			{
				pPlayerInfo->m_bOnLine = false;
			}

			if ( curTime-pPlayerInfo->GetActiveTime()>600 && pPlayerInfo->IsOnLine() )
			{
				MapCheckOnline::iterator itorPID = PlayerNotActive.find(pPlayerInfo->m_ServerID);
				if ( itorPID == PlayerNotActive.end() )
				{
					PlayerNotActive.insert(make_pair(pPlayerInfo->m_ServerID,vector<UINT32>()));
					itorPID = PlayerNotActive.find(pPlayerInfo->m_ServerID);
				}
				itorPID->second.push_back(pPlayerInfo->m_PID);
			}

			if ( curTime-pPlayerInfo->GetActiveTime()>MaxExistTime && pPlayerInfo->IsDead() ) //超时不在线的玩家将被删除掉
			{
				vecDeletePD.push_back(pPlayerInfo->m_PID);
			}
		}

		//将下线超过一定时间的玩家数据清理掉
		MapDBPlayerInfo::iterator itorTempPI;
		for ( size_t nLoop=0;nLoop<vecDeletePD.size();++nLoop)
		{
			itorTempPI = m_PlayerInfos.find(vecDeletePD[nLoop]);
			if (itorTempPI != m_PlayerInfos.end())
			{
				DBServerPlayerInfo* pDownPlayerInfo = itorTempPI->second;
				//UpdatePlayerDataToDB(pDownPlayerInfo);//因为在onquit里面已经处理过

				DBServerPlayerInfo* pRightPlayerInfo = nullptr;			
				for (size_t nPos=0;nPos<pPlayerInfo->m_vectorFriend.size();++nPos)
				{
					pRightPlayerInfo = GetDBPlayerInfo(pPlayerInfo->m_vectorFriend[nPos].m_PID);
					if ( pRightPlayerInfo )
					{
						pRightPlayerInfo->DeleteFriend(pPlayerInfo->m_PID);
					}
				}

				safe_delete(itorTempPI->second);
				m_PlayerInfos.erase(itorTempPI);
			}
		}

		for ( MapCheckOnline::iterator itorCheck=PlayerNotActive.begin();itorCheck!=PlayerNotActive.end();++itorCheck )
		{
			int ServerID = itorCheck->first;
			CDBServerSocket* pSocket = GetDBSocketByServerID(ServerID);
			if ( pSocket )
			{
				DBServerXY::DBS_ReqPlayerOnLine msgRPOL;
				msgRPOL.m_ServerID = ServerID;

				vector<UINT32>& vecPID = itorCheck->second;
				for(UINT32 i=0;i<vecPID.size();++i)
				{
					msgRPOL.m_vecPID.push_back(vecPID.at(i));
					if ( msgRPOL.m_vecPID.size() >= msgRPOL.MAX_COUNT )
					{
						pSocket->SendMsg(msgRPOL);
						msgRPOL.m_vecPID.clear();
					}
				}
				if ( msgRPOL.m_vecPID.size() )
				{
					pSocket->SendMsg(msgRPOL);
					msgRPOL.m_vecPID.clear();
				}
			}
			else
			{
				DebugError("CheckPlayerOnLine Can't Find GS Socket ServerID=%d PlayerSize=%d",ServerID,itorCheck->second.size());
			}
		}

		DebugLog("CheckPlayerOnLine End");
	}	
}
void CServer::CheckBotPlayerCached(UINT32 curTime)
{
	TraceStackPath logTP("CServer::CheckBotPlayerCached");

	UINT32 nIntervalTime = 1200;
	int nMod = 20;

	static UINT32 s_CheckBotPlayerCached = 0;
	if ( s_CheckBotPlayerCached == 0 )
	{
		s_CheckBotPlayerCached = curTime;
	}
	else if ( curTime - s_CheckBotPlayerCached >= nIntervalTime )
	{
		s_CheckBotPlayerCached = curTime ;

		CLogFuncTime lft(m_DBSFuncTime,"CheckBotPlayerCached");
		DebugInfo("CheckBotPlayerCached Start...");

		int nCount = m_CurTime/nIntervalTime;

		DBServerPlayerInfo* pPlayerInfo;
		for ( MapDBPlayerInfo::iterator itorPlayerInfo=m_PlayerInfos.begin();itorPlayerInfo!=m_PlayerInfos.end();itorPlayerInfo++)
		{
			pPlayerInfo = itorPlayerInfo->second;
			if ( pPlayerInfo->IsBotPlayer() && pPlayerInfo->m_PID%nMod == nCount%nMod )
			{
				UpdatePlayerDataToMemcach(pPlayerInfo);
			}
		}
		DebugInfo("CheckBotPlayerCached End");
	}
}
void CServer::PushUserGift(const stDBUserGiftInfo& stDBUGI)
{
	CSelfLock l(m_ckVectorGift);

	if ( m_vectorUserGift.size() == 0 )
	{
		m_vectorUserGift.push_back(stDBUGI);
	}
	else
	{
		bool bSuccess = false;
		for ( size_t nPos=0;nPos<m_vectorUserGift.size();++nPos )
		{
			if ( m_vectorUserGift[nPos].m_GiftIdx == 0 )
			{
				bSuccess = true;
				m_vectorUserGift[nPos] = stDBUGI;
				break;
			}
		}
		if ( bSuccess == false )
		{
			m_vectorUserGift.push_back(stDBUGI);
		}
	}
}
void CServer::DoUserGift()
{
	VectorDBUserGiftInfo listUGI;
	{
		CSelfLock l(m_ckVectorGift);
		for ( size_t nPos=0;nPos<m_vectorUserGift.size();++nPos )
		{
			if ( m_vectorUserGift[nPos].m_GiftIdx > 0 )
			{
				listUGI.push_back(m_vectorUserGift[nPos]);
				m_vectorUserGift[nPos].Init();
			}
		}
	}

	if ( listUGI.size() )
	{
		DBServerPlayerInfo* pPlayerInfo = nullptr;
		for ( size_t nPos=0;nPos<listUGI.size();++nPos )
		{
			stDBUserGiftInfo& rDBUGI = listUGI[nPos];

			pPlayerInfo = GetDBPlayerInfo(rDBUGI.m_RecvPID);
			if ( pPlayerInfo )
			{
				stUserGiftInfo stGI;
				stGI.m_GiftID     = rDBUGI.m_GiftID;
				stGI.m_GiftIdx    = rDBUGI.m_GiftIdx;
				stGI.m_Price      = rDBUGI.m_Price;
				stGI.m_NickName   = rDBUGI.m_NickName;
				stGI.m_ActionTime = rDBUGI.m_ActionTime;
				pPlayerInfo->AddGift(stGI);

				DBServerXY::DBS_msgUserGiftInfo stUGI;
				DBServerXY::DBS_UserGiftInfo msgUGi;
				msgUGi.m_AID                     = pPlayerInfo->m_AID;
				msgUGi.m_PID                     = pPlayerInfo->m_PID;

				int nRet = UserGiftToDBSGiftInfo(stGI,stUGI);
				if ( nRet == 1 )
				{
					msgUGi.m_GiftInfo = stUGI;
					SendMsgToPlayer(msgUGi,msgUGi.m_PID);
				}
				else
				{
					DebugError("DoUserGift GiftIdx=%d GiftID=%d",rDBUGI.m_GiftIdx,rDBUGI.m_GiftID);
				}				
			}
		}
	}
}

void CServer::OnTimer( void )
{
	TraceStackPath logTP("CServer::OnTimer");

	if ( !m_bInitComplete )
	{
		return ;
	}
	if ( !m_bStartDBThread )
	{
		m_bStartDBThread = true;
		for (size_t Pos=0;Pos<m_vecDBMsgThread.size();++Pos)
		{
			m_vecDBMsgThread[Pos]->Start();
		}
		DebugLogOut("StartDBThread = true");
		return ;
	}

	try
	{
		CLogFuncTime lft(m_DBSFuncTime,"OnTimer");

		static INT64 s_nStartTime=0,s_nTimeEnd=0,s_nTotalUseTime=0,s_nOnTimeCount=0;
		static INT32 s_nUseTime=0,s_nMaxUseTime=0;

		m_CurTime = static_cast<UINT32>(time( NULL ));

		s_nOnTimeCount++;
		s_nStartTime = Tool::GetMilliSecond();
		DebugInfo("DBserver OnTime Start");

		m_memOperator.TestConnect();

		OnNewDay(m_CurTime);
		CheckClientSocketActive(m_CurTime);
		CheckPlayerOnLine(m_CurTime);

		CheckBotPlayerCached(m_CurTime);
		DoUserGift();

		PrintIOXieYi(m_CurTime);	
		PrintIOSpeed(m_CurTime);
		//OnCheckUserProduct(m_CurTime);
		//OnSendUserProduct(m_CurTime);	
		OnOneMinite(m_CurTime);
		OnTenMinite(m_CurTime);
		OnHour(m_CurTime);

		s_nTimeEnd = Tool::GetMilliSecond();
		s_nUseTime = INT32(s_nTimeEnd - s_nStartTime);
		s_nTotalUseTime += s_nUseTime;
		if ( s_nUseTime > s_nMaxUseTime )
		{
			s_nMaxUseTime = s_nUseTime;
			DebugLogOut("DBserver OnTime CurUseTime=%d",s_nUseTime);
		}
		if ( m_CurTime%N_CeShiLog::c_DBOnTimeDiff==0 || s_nUseTime>=N_CeShiLog::c_DBMaxOnTime )
		{
			DebugLogOut("DBserver OnTime MaxTime=%d CurUseTime=%d AveUseTime=%d",s_nMaxUseTime,s_nUseTime,int(s_nTotalUseTime/s_nOnTimeCount));
		}
		DebugInfo( "DBserver OnTime End ");
	}
	catch(...)
	{
		DebugError("Stack Start OnTime .....................................................................");

		VectorString& rVS = TraceStackPath::s_vectorPath;
		while ( rVS.size() )
		{
			for ( size_t nSize=0;nSize<rVS.size();++nSize )
			{
				DebugInfo("%s",rVS[nSize].c_str());
			}
			rVS.clear();
		}

		DebugError("Stack End OnTime .....................................................................");
	}	
}
void CServer::OnCheckUserProduct(UINT32 curTime)
{
	static UINT32 s_CheckUserProductTime = 0;
	if ( s_CheckUserProductTime == 0 )
	{
		s_CheckUserProductTime += curTime + 5;
	}
	else if ( curTime > s_CheckUserProductTime )
	{
		s_CheckUserProductTime += curTime + 5;

		CLogFuncTime lft(m_DBSFuncTime,"OnCheckUserProduct");
		VectorUserProduct vectorUP;
		if ( m_pDBOperator->ReadAllUserProduct(m_MaxProductIdx,vectorUP) != DB_RESULT_SUCCESS )
		{
			DebugError("ReadAllUserProduct MaxProductIdx=%d",m_MaxProductIdx);
		}
		if ( vectorUP.size() )
		{
			DBServerPlayerInfo* pPlayerInfo = nullptr;
			for ( size_t nPos=0;nPos<vectorUP.size();nPos++)
			{
				const stUserProduct& rUP = vectorUP[nPos];
				if ( rUP.m_Idx > m_MaxProductIdx )
				{	m_MaxProductIdx = rUP.m_Idx; }

				pPlayerInfo = GetDBPlayerInfo(rUP.m_PID);
				if ( pPlayerInfo )
				{
					pPlayerInfo->AddUserProduct(rUP);

					DBServerXY::DBS_To_GS_Flag msgGSF;
					msgGSF.m_AID        = pPlayerInfo->m_AID;
					msgGSF.m_PID        = pPlayerInfo->m_PID;
					msgGSF.m_Flag       = msgGSF.CheckClientExist;
					msgGSF.m_nValue     = pPlayerInfo->m_PID;
					SendMsgToPlayer(msgGSF,pPlayerInfo->m_PID);

					//DoUserProduct(rUP);
				}
			}
		}
	}	
}
void CServer::OnSendUserProduct(UINT32 curTime)
{
	static UINT32 s_CheckSendUserProductTime = 0;
	if ( s_CheckSendUserProductTime == 0 )
	{
		s_CheckSendUserProductTime += curTime + 60;
	}
	else if ( curTime > s_CheckSendUserProductTime )
	{
		s_CheckSendUserProductTime += curTime + 60;

		CLogFuncTime lft(m_DBSFuncTime,"OnSendUserProduct");

		DBServerXY::DBS_To_GS_Flag msgGSF;
		PDBServerPlayerInfo pPlayerInfo;
		for ( MapDBPlayerInfo::iterator itorPlayerInfo=m_PlayerInfos.begin();itorPlayerInfo!=m_PlayerInfos.end();itorPlayerInfo++)
		{
			pPlayerInfo = itorPlayerInfo->second;
			if ( pPlayerInfo->m_vectorUserProduct.size() && pPlayerInfo->IsOnLine() )
			{
				msgGSF.ReSet();
				msgGSF.m_AID         = pPlayerInfo->m_AID;
				msgGSF.m_PID         = pPlayerInfo->m_PID;
				msgGSF.m_Flag        = msgGSF.CheckClientExist;
				msgGSF.m_nValue      = pPlayerInfo->m_PID;
				SendMsgToPlayer(msgGSF,pPlayerInfo->m_PID);
			}
		}
	}
}
void CServer::PrintIOSpeed(UINT32 curTime)
{
	static UINT32 s_CheckXieYiSpeedTime = 0;
	if ( s_CheckXieYiSpeedTime == 0 )
	{
		s_CheckXieYiSpeedTime += curTime + 600;
	}
	else if ( curTime > s_CheckXieYiSpeedTime )
	{
		s_CheckXieYiSpeedTime += curTime + 600;

		int UserTime = max(m_CurTime - m_nServerStartTime,1);

		DebugLogOut("InPacket=%-8lld  InByte=%-10lld    OutPacket=%-8lld OutByte=%-10lld",
			CDBServerSocket::s_TotalInPacket,
			CDBServerSocket::s_TotalInByte,
			CDBServerSocket::s_TotalOutPacket,
			CDBServerSocket::s_TotalOutByte );

		DebugLogOut("In  InPacket=%-8lld  PacketLen=%-8lld Byte=%lld UseTime=%d",
			CDBServerSocket::s_TotalInPacket/UserTime,
			CDBServerSocket::s_TotalInByte/max(CDBServerSocket::s_TotalInPacket,1),
			CDBServerSocket::s_TotalInByte/UserTime,UserTime );

		DebugLogOut("Out OutPacket=%-8lld PacketLen=%-8lld Byte=%lld UseTime=%d",
			CDBServerSocket::s_TotalOutPacket/UserTime,
			CDBServerSocket::s_TotalOutByte/max(CDBServerSocket::s_TotalOutPacket,1),
			CDBServerSocket::s_TotalOutByte/UserTime,UserTime );
	}	
}
void CServer::PrintIOXieYi(UINT32 curTime)
{
	static UINT32 s_CheckXieYiInOutTime = 0;
	if ( s_CheckXieYiInOutTime == 0 )
	{
		s_CheckXieYiInOutTime += curTime + 1800;
	}
	else if ( curTime > s_CheckXieYiInOutTime )
	{
		s_CheckXieYiInOutTime += curTime + 1800;

		DebugInfo("Total:PacketIn=%-8lld PacketOut=%-8lld OutByte=%-8lld InByte=%-8lld",
			CDBServerSocket::s_TotalInPacket,CDBServerSocket::s_TotalOutPacket,
			CDBServerSocket::s_TotalOutByte,CDBServerSocket::s_TotalInByte );

		VectorXieYiLog vectorXYLog;
		for (MapXieYiIO::iterator itorXieIO=CDBServerSocket::s_InXieYi.begin();itorXieIO!=CDBServerSocket::s_InXieYi.end();itorXieIO++)
		{
			stXieYiLog stXYL;
			stXYL.m_XieYiID    = itorXieIO->first;
			stXYL.m_TotalByte  = itorXieIO->second;
			stXYL.m_Percent    = INT32(itorXieIO->second*10000/max(CDBServerSocket::s_TotalInByte,1));
			vectorXYLog.push_back(stXYL);
		}
		sort(vectorXYLog.begin(),vectorXYLog.end(),CCompareXieYiTotalByte());
		for ( size_t nPos=0;nPos<vectorXYLog.size();++nPos)
		{
			const stXieYiLog& rstXY = vectorXYLog[nPos];
			DebugInfo("In  XYID=%-5d XYSIZE=%-10lld Percent=%d",rstXY.m_XieYiID,rstXY.m_TotalByte,rstXY.m_Percent);
		}

		vectorXYLog.clear();
		for (MapXieYiIO::iterator itorXieIO=CDBServerSocket::s_OutXieYi.begin();itorXieIO!=CDBServerSocket::s_OutXieYi.end();itorXieIO++)
		{
			stXieYiLog stXYL;
			stXYL.m_XieYiID    = itorXieIO->first;
			stXYL.m_TotalByte  = itorXieIO->second;
			stXYL.m_Percent    = INT32(itorXieIO->second*10000/max(CDBServerSocket::s_TotalOutByte,1));
			vectorXYLog.push_back(stXYL);
		}
		sort(vectorXYLog.begin(),vectorXYLog.end(),CCompareXieYiTotalByte());
		for ( size_t nPos=0;nPos<vectorXYLog.size();++nPos)
		{
			const stXieYiLog& rstXY = vectorXYLog[nPos];
			DebugInfo("Out XYID=%-5d XYSIZE=%-10lld Percent=%d",rstXY.m_XieYiID,rstXY.m_TotalByte,rstXY.m_Percent);
		}
	}
}

void CServer::OnAccept( IConnect* connect )
{
	CDBServerSocket* client = nullptr;
	try
	{
		if ( m_bInitComplete )
		{
			client = new CDBServerSocket( this, connect );
			m_ClientsSocket.insert( make_pair(connect, client) );
		}
		else
		{
			connect->Close();
		}
	}
	catch (...)
	{
		DebugError("OnAccept DBS memory failed");
	}

	DebugInfo("OnAccept connect=%d client=%d",reinterpret_cast<int>(connect),reinterpret_cast<int>(client) );
}

void CServer::OnClose( IConnect* nocallbackconnect, bool bactive )
{
	DebugInfo("CServer::OnClose NoCallBack Connect");
	DealCloseSocket( nocallbackconnect );
}

void CServer::DealCloseSocket( IConnect* connect )
{
	DebugInfo("CServer::DealCloseSocket start");

	MapClientSocket::iterator itorConnect = m_ClientsSocket.find( connect );
	if ( itorConnect != m_ClientsSocket.end() )
	{
		if( itorConnect->second )
		{
			DebugInfo("CServer::DealCloseSocket Closesocket");

			itorConnect->second->Close();
		}
		safe_delete(itorConnect->second);
		m_ClientsSocket.erase( itorConnect );
	}

	DebugInfo("CServer::DealCloseSocket end");
}

CDBServerSocket* CServer::GetDBSocketByServerID(INT16 ServerID)
{
	CDBServerSocket* pSocket = NULL;
	for (MapClientSocket::iterator itorSocket=m_ClientsSocket.begin();itorSocket!=m_ClientsSocket.end();++itorSocket)
	{
		if ( itorSocket->second->GetServerID() == ServerID )
		{
			pSocket = itorSocket->second;
			break;
		}
	}
	return pSocket;
}
DBServerPlayerInfo* CServer::GetDBPlayerInfo(UINT32 PID)
{
	MapDBPlayerInfo::iterator itorPI = m_PlayerInfos.find(PID);
	if ( itorPI != m_PlayerInfos.end() )
	{
		return itorPI->second;
	}
	return nullptr;
}
void CServer::GetHuiYuanInfo(stHuiYuanInfo& stHY,const string& strRule)
{
	std::map<string,INT64> mapRule;
	std::map<string,INT64>::iterator itorKey;
	mapRule = Tool::GetRuleMapInData(strRule,string(";"),string("="));

	itorKey = mapRule.find("viplevel");
	if ( itorKey != mapRule.end())
	{
		stHY.m_VipLevel = BYTE(itorKey->second);
	}

	itorKey = mapRule.find("gamemoney");
	if ( itorKey != mapRule.end())
	{
		stHY.m_SendOneceMoney = itorKey->second;
	}

	itorKey = mapRule.find("givenmoney");
	if ( itorKey != mapRule.end())
	{
		stHY.m_GivenDaysMoney = INT32(itorKey->second);
	}

	itorKey = mapRule.find("bankbox");
	if ( itorKey != mapRule.end())
	{
		stHY.m_BankBox = BYTE(itorKey->second);
	}

	itorKey = mapRule.find("freeface");
	if ( itorKey != mapRule.end())
	{
		stHY.m_FreeFace = BYTE(itorKey->second);
	}

	itorKey = mapRule.find("duringtime");
	if ( itorKey != mapRule.end())
	{
		stHY.m_DuringTime = UINT32(itorKey->second);
	}
}
void CServer::GetMoguiExchangeInfo(stMoguiExchangeInfo& stME,const string& strRule)
{
	std::map<string,INT64> mapRule;
	std::map<string,INT64>::iterator itorKey;
	mapRule = Tool::GetRuleMapInData(strRule,string(";"),string("="));

	itorKey = mapRule.find("mogui");
	if ( itorKey != mapRule.end())
	{
		stME.m_MoGuiMoney = int(itorKey->second);
	}
}
int CServer::InitBaseInfo()
{
	//VectorFaceInfo vectorFI;
	//if ( m_pDBOperator->ReadFaceInfo(vectorFI) != DB_RESULT_SUCCESS )
	//{
	//	DebugError("ReadFaceInfo Error Error");
	//}
	//if ( vectorFI.size() )
	//{
	//	m_mapFaceInfo.clear();
	//	for (int i=0;i<(int)vectorFI.size();++i)
	//	{
	//		m_mapFaceInfo.insert(make_pair(vectorFI[i].m_FaceID,vectorFI[i]));
	//	}
	//}

	//VectorGiftInfo vectorGI;
	//if ( m_pDBOperator->ReadGiftInfo(vectorGI) != DB_RESULT_SUCCESS )
	//{
	//	DebugError("ReadFaceInfo Error Error");
	//}
	//if ( vectorGI.size() )
	//{
	//	m_mapGiftInfo.clear();
	//	for(int i=0;i<(int)vectorGI.size();++i)
	//	{
	//		m_mapGiftInfo.insert(make_pair(vectorGI[i].m_GiftID,vectorGI[i]));
	//	}
	//}

	//VectorProductInfo vectorPI;
	//if ( m_pDBOperator->ReadProductInfo(vectorPI) != DB_RESULT_SUCCESS )
	//{
	//	DebugError("ReadFaceInfo Error Error");
	//}
	//if ( vectorPI.size() )
	//{
	//	m_mapHuiYuanInfo.clear();
	//	m_mapMoguiExchangeInfo.clear();
	//	m_mapProductInfo.clear();

	//	stHuiYuanInfo stHYInfo;
	//	stMoguiExchangeInfo stMEInfo;
	//	for ( size_t nPos=0;nPos<vectorPI.size();nPos++)
	//	{
	//		ProductInfo& rPI = vectorPI[nPos];
	//		m_mapProductInfo.insert(make_pair(rPI.m_ProductID,rPI));

	//		if ( rPI.m_ProductType == ProductType_HuiYuan )
	//		{
	//			stHYInfo.Init();
	//			GetHuiYuanInfo(stHYInfo,rPI.m_ProductRule);
	//			stHYInfo.m_ProductID = rPI.m_ProductID;
	//			m_mapHuiYuanInfo.insert(make_pair(stHYInfo.m_ProductID,stHYInfo));
	//		}
	//		else if ( rPI.m_ProductType == ProductType_MoguiExchange )
	//		{
	//			stMEInfo.Init();
	//			GetMoguiExchangeInfo(stMEInfo,rPI.m_ProductRule);
	//			stMEInfo.m_ProductID = rPI.m_ProductID;
	//			m_mapMoguiExchangeInfo.insert(make_pair(stMEInfo.m_ProductID,stMEInfo));
	//		}
	//	}
	//}

	//VectorHonorInfo vetorHI;
	//if ( m_pDBOperator->ReadHonorInfo(vetorHI) != DB_RESULT_SUCCESS )
	//{
	//	DebugError("ReadHonorInfo Error Error");
	//}
	//if ( vetorHI.size() )
	//{
	//	m_mapHonorInfo.clear();
	//	for ( size_t nPos=0;nPos<vetorHI.size();nPos++)
	//	{
	//		stHonorInfo& r_HI = vetorHI[nPos];
	//		assert( (r_HI.m_ID/50+1)==r_HI.m_Type && r_HI.m_ID%50==r_HI.m_Idx );
	//		m_mapHonorInfo.insert(make_pair(r_HI.m_ID,r_HI));
	//	}
	//}

	//VectorPTLevelInfo vectorGLI;
	//if ( m_pDBOperator->ReadGameLevelInfo(vectorGLI) != DB_RESULT_SUCCESS )
	//{
	//	DebugError("ReadGameLevelInfo Error Error");
	//}
	//if ( vectorGLI.size() )
	//{
	//	m_mapGameLevelInfo.clear();
	//	for ( size_t nPos=0;nPos<vectorGLI.size();nPos++)
	//	{
	//		stPlayTimesInfo& r_GLI = vectorGLI[nPos];
	//		m_mapGameLevelInfo.insert(make_pair(r_GLI.m_PTLevel,r_GLI));
	//	}
	//}
	
	//VectorDBAreaInfo vectorAI;
	//if ( m_pDBOperator->ReadAreaInfo(vectorAI) != DB_RESULT_SUCCESS )
	//{
	//	DebugError("ReadAreaInfo Error Error");
	//}
	//if ( vectorAI.size() )
	//{
	//	stAreaInfo stAI;
	//	m_mapAreaInfo.clear();
	//	for ( size_t nPos=0;nPos<vectorAI.size();++nPos )
	//	{
	//		stAI.Init();
	//		stAI.m_AID         = vectorAI[nPos].m_AID;
	//		stAI.m_AreaName    = vectorAI[nPos].m_AreaName;
	//		stAI.InitJoinMoney(vectorAI[nPos].m_JoinRule);
	//		stAI.InitLandMoney(vectorAI[nPos].m_LandRule);
	//		
	//		m_mapAreaInfo.insert(make_pair(stAI.m_AID,stAI));
	//	}
	//}

	return 0;
}

int CServer::OnGameServerMsg( CDBServerSocket* pDBSocket,CRecvMsgPacket& msgPack )
{
	assert(pDBSocket);
	if ( !pDBSocket )
	{
		DebugError("OnGameServerMsg errorid=%d",10000);
		return 10000;
	}

	DebugInfo("OnGameServerMsg Start XYID=%d XYLen=%d",msgPack.m_XYID,msgPack.m_nLen);

	int ret = 0;

	switch ( msgPack.m_XYID )
	{
	case DBServerXY::DBS_ReqServerPlayerLogin::XY_ID:
		{
			ret = OnReqPlayerLogin(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqPlayerInfo::XY_ID:
		{
			ret = OnReqPlayerInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqAddBotMoney::XY_ID:
		{
			ret = OnReqAddBotMoney(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqPlayerGameMoney::XY_ID:
		{
			ret = OnReqPlayerGameMoney(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqRoomTableInfo::XY_ID:
		{
			ret = OnReqRoomTableInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqBotPlayer::XY_ID:
		{
			ret = OnReqBotPlayerInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqServerConnect::XY_ID:
		{
			ret = OnReqServerConnect(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_PlayerQuite::XY_ID:
		{
			ret = OnPlayerQuit(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_PlayerDataNotExist::XY_ID:
		{
			ret = OnPlayerDataNotExist(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespPlayerOnLine::XY_ID:
		{
			ret = OnRespPlayerOnLine(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_UserFriend::XY_ID:
		{
			ret = OnWDBUserFriend(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_WinLoss::XY_ID:
		{
			ret = OnWDBWinLoss(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_WinLossList::XY_ID:
		{
			ret = OnWDBWinLossList(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_MatchResult::XY_ID:
		{
			ret = OnWDBMatchResult(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_SendMoney::XY_ID:
		{
			ret = OnWDBSendMoney(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_ShowFace::XY_ID:
		{
			ret = OnWDBShowFace(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_SendGift::XY_ID:
		{
			ret = OnWDBSendGift(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_SoldGift::XY_ID:
		{
			ret = OnWDBSoldGift(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_IncomeAndPay::XY_ID:
		{
			ret = OnWDBIncomeAndPay(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_ChatMsg::XY_ID:
		{
			ret = OnWDBChatMsg(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_CreateTable::XY_ID:
		{
			ret = OnWDBCreateTable(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_ChangeUserInfo::XY_ID:
		{
			ret = OnWDBChangeName(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqPlayerAward::XY_ID:
		{
			ret = OnReqPlayerAward(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_ReportPlayerOnline::XY_ID:
		{
			ret = OnWDBReportPlayerOnLine(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_MaxPai::XY_ID:
		{
			ret = OnWDBMaxPai(pDBSocket,msgPack); 
		}
		break;
	case DBServerXY::WDB_MaxWin::XY_ID:
		{
			ret = OnWDBMaxWin(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_MaxMoney::XY_ID:
		{
			ret = OnWDBMaxMoney(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_MatchWinLoss::XY_ID:
		{
			ret = OnWDBMatchWinLoss(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_PlayerActionLog::XY_ID:
		{
			ret = OnWDBPlayerActionLog(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_PlayerAward::XY_ID:
		{
			ret = OnWDBPlayerAward(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_ReqCheckGameInfo::XY_ID:
		{
			ret = OnWDBCheckGameInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_PlayerClientError::XY_ID:
		{
			ret = OnWDBPlayerClientError(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::GS_To_DBS_Flag::XY_ID:
		{
			ret = OnGSToDBSFlag(pDBSocket,msgPack);
		}
		break;
	case MOGUI_XieYiList:
		{
			ret = OnXieYiList(pDBSocket,msgPack);
		}
		break;
	default:
		{
			DebugError("OnGameServerMsg 无法处理协议 id=%d len=%d",msgPack.m_XYID,msgPack.m_nLen);
		}
	}
	if ( ret )
	{
		DebugError("OnGameServerMsg id=%d,len=%d ret=%d",msgPack.m_XYID,msgPack.m_nLen,ret);
	}

	DebugInfo("OnGameServerMsg End");
	return 0;
}

int CServer::OnReqServerConnect( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqServerConnect");

	DBServerXY::DBS_ReqServerConnect rsc;
	TransplainMsg(msgPack,rsc);

	DebugLog("OnReqServerConnect ServerID=%d",rsc.m_ServerID);
	DebugInfo("CServer::OnReqServerConnect Start ServerID=%d",rsc.m_ServerID);

	DBServerXY::DBS_RespServerConnectData rscd;

	MapClientSocket::iterator itorClient;
	for (itorClient=m_ClientsSocket.begin();itorClient!=m_ClientsSocket.end();++itorClient)
	{
		if ( itorClient->second->GetServerID() == rsc.m_ServerID )
		{
			rscd.m_Flag = rscd.SERVEREXIST;
			break;
		}
	}
	if ( rscd.m_Flag != rscd.SERVEREXIST )
	{
		if ( rsc.m_ServerClass == CLIENT_CLASS_GAMESERVER )
		{
			//此处应该加上验证SESSION的内容

			pSocket->SetClass(rsc.m_ServerClass);
			pSocket->SetServerID(rsc.m_ServerID);
			pSocket->SetLogin(true);

			rscd.m_Flag = rscd.SUCCESS;
		}
		else
		{
			rscd.m_Flag = rscd.KEYERROR;
		}
	}

	pSocket->SendMsg(rscd);

	//if ( rscd.m_Flag == rscd.SUCCESS )
	//{
	//	if ( m_mapGameLevelInfo.size() )
	//	{
	//		XieYiList<DBServerXY::DBS_GameLevelInfo,100> vectorGameLevelList;
	//		DBServerXY::DBS_GameLevelInfo msgGLI;
	//		for ( MapPTLevelInfo::iterator itorGLI=m_mapGameLevelInfo.begin();itorGLI!=m_mapGameLevelInfo.end();++itorGLI )
	//		{
	//			msgGLI.m_PTLevel      = itorGLI->second.m_PTLevel;
	//			msgGLI.m_TotalPT      = itorGLI->second.m_TotalPT;
	//			msgGLI.m_AddPT        = itorGLI->second.m_AddPT;
	//			msgGLI.m_AwardMoney   = itorGLI->second.m_AwardMoney;		
	//			vectorGameLevelList.m_XieYiList.push_back(msgGLI);
	//		}
	//		pSocket->SendMsg(vectorGameLevelList);
	//	}
	//}

	DebugInfo("CServer::OnReqServerConnect End");

	return 0;
}

int CServer::InitServerData()
{
	return 0;
}

int CServer::OnReqBotPlayerInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqBotPlayerInfo");

	DBServerXY::DBS_ReqBotPlayer msgRbp;
	TransplainMsg(msgPack,msgRbp);

	DebugInfo("CServer::OnReqBotPlayerInfo Start ServerID=%d StartPID=%d EndPID=%d",
		msgRbp.m_ServerID,msgRbp.m_StartPlayerNumber,msgRbp.m_EndPlayerNumber );

	DBServerXY::DBS_RespBotPlayer msgRespBP;

	DBServerPlayerInfo* pPlayerInfo;
	for (MapDBPlayerInfo::iterator itorPlayer=m_PlayerInfos.begin();itorPlayer!=m_PlayerInfos.end();++itorPlayer)
	{
		pPlayerInfo = itorPlayer->second;
		if ( pPlayerInfo->m_AID==N_AREAID::AREAID_BOT 
			&& pPlayerInfo->m_PID>=msgRbp.m_StartPlayerNumber && pPlayerInfo->m_PID<=msgRbp.m_EndPlayerNumber 
			&& pPlayerInfo->m_ServerID!=0 && pPlayerInfo->m_ServerID!=msgRbp.m_ServerID )
		{
			msgRespBP.m_Flag = msgRespBP.RANGERROR;
			break;
		}
	}

	if ( msgRbp.m_ServerID==pSocket->GetServerID() && msgRespBP.m_Flag==msgRespBP.SUCCESS )
	{
		VectorBotPlayerData vectorPD;
		m_pDBOperator->ReadBotPlayerData(N_AREAID::AREAID_BOT,msgRbp.m_StartPlayerNumber,msgRbp.m_EndPlayerNumber,vectorPD);

		for ( int Pos=0;Pos<(int)vectorPD.size();++Pos )
		{
			stBotPlayerData& TempBotPlayerData = vectorPD[Pos];

			int    BotLevel = TempBotPlayerData.m_BotLevel;
			UINT32 TempPID  = TempBotPlayerData.m_PID;

			if ( BotLevel <= 0 )
			{
				//DebugError("BotPlayer Level PID=%d",TempPID);
				continue;
			}

			int nAddMoney = 0;
			INT64 nGameMoney = TempBotPlayerData.m_stGameInfo.m_nGameMoney;
			INT64 nMaxMoney = GameFunc::GetBotLevelMoney(BotLevel);
			INT64 nMinMoney = GameFunc::GetBotLevelMoney(BotLevel-1);

			if ( nGameMoney > nMaxMoney*2 )
			{
				nAddMoney = int(-(nGameMoney-nMaxMoney)/2);
			}
			else if ( TempBotPlayerData.m_stGameInfo.m_nGameMoney < nMinMoney )
			{
				nAddMoney = int(nMinMoney);
			}

			if ( nAddMoney != 0 )
			{
				if ( TempBotPlayerData.m_stGameInfo.m_nGameMoney + nAddMoney >= 0 )
				{
					DebugInfo("Add Bot Money AID=%d PID=%d Level=%-2d Money=%s AddMoney=%s",TempBotPlayerData.m_AID,TempBotPlayerData.m_PID,
						TempBotPlayerData.m_BotLevel,N2S(TempBotPlayerData.m_stGameInfo.m_nGameMoney,10).c_str(),N2S(nAddMoney,10).c_str() );

					TempBotPlayerData.m_stGameInfo.m_nGameMoney += nAddMoney;
					m_pDBOperator->DBAddUserGameMoney(N_AREAID::AREAID_BOT,TempPID,nAddMoney);

					//RWDB_UpdateUserGameMoneyType rwdbmsgUGMT;
					//rwdbmsgUGMT.m_AID         = N_AREAID::AREAID_BOT;
					//rwdbmsgUGMT.m_PID         = TempPID;
					//rwdbmsgUGMT.m_nGivenMoney = nAddMoney;
					//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);

					RWDB_IncomeAndPay dbmsgInPay;
					dbmsgInPay.m_AID = N_AREAID::AREAID_BOT;
					dbmsgInPay.m_PID = TempPID;
					dbmsgInPay.m_nMoney = nAddMoney;
					if ( nAddMoney >= 0 ) dbmsgInPay.m_Flag = N_IncomeAndPay::BotAdd;
					else dbmsgInPay.m_Flag = N_IncomeAndPay::BotDec;
					m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);					
				}
				else
				{
					DebugError("Add Bot Money AID=%d PID=%d Level=%d",TempBotPlayerData.m_AID,TempBotPlayerData.m_PID,TempBotPlayerData.m_BotLevel);
				}
			}
		}

		pSocket->SendMsg(msgRespBP);

		PublicXY::BatchProtocol bp;
		bp.m_Flag = bp.START;
		bp.m_XYID = DBServerXY::DBS_BotPlayerData::XY_ID;
		pSocket->SendMsg(bp);

		DBServerXY::DBS_BotPlayerData BotPlayerData;
		for ( VectorBotPlayerData::iterator itorPlayerData=vectorPD.begin();itorPlayerData!=vectorPD.end();++itorPlayerData )
		{
			BotPlayerData.ReSet();

			BotPlayerData.m_PID                   = itorPlayerData->m_PID;
			BotPlayerData.m_AID                   = itorPlayerData->m_AID;			

			BotPlayerData.m_Sex                   = UINT8(itorPlayerData->m_stUserDataInfo.m_Sex);
			BotPlayerData.m_HeadID                = itorPlayerData->m_stUserDataInfo.m_HeadID;
			BotPlayerData.m_NickName              = itorPlayerData->m_stUserDataInfo.m_NickName;
			BotPlayerData.m_HeadPicURL            = itorPlayerData->m_stUserDataInfo.m_HeadPicURL;
			BotPlayerData.m_HomePageURL           = itorPlayerData->m_stUserDataInfo.m_HomePageURL;
			BotPlayerData.m_City                  = itorPlayerData->m_stUserDataInfo.m_City;			

			BotPlayerData.m_nGameMoney            = itorPlayerData->m_stGameInfo.m_nGameMoney;
			BotPlayerData.m_nMedal                = itorPlayerData->m_stGameInfo.m_nMedal;
			BotPlayerData.m_Diamond               = itorPlayerData->m_stGameInfo.m_Diamond;
			BotPlayerData.m_GoldenPig             = itorPlayerData->m_stGameInfo.m_GoldenPig;
			BotPlayerData.m_PigFood               = itorPlayerData->m_stGameInfo.m_PigFood;

			BotPlayerData.m_nJF                   = itorPlayerData->m_stGameInfo.m_nJF;
			BotPlayerData.m_nEP                   = itorPlayerData->m_stGameInfo.m_nEP;
			BotPlayerData.m_nWinTimes             = itorPlayerData->m_stGameInfo.m_nWinTimes;
			BotPlayerData.m_nLossTimes            = itorPlayerData->m_stGameInfo.m_nLossTimes;
			BotPlayerData.m_DrawnTimes            = itorPlayerData->m_stGameInfo.m_DrawnTimes;
			BotPlayerData.m_EscapeTimes           = itorPlayerData->m_stGameInfo.m_EscapeTimes;
			BotPlayerData.m_nGameTime             = itorPlayerData->m_stGameInfo.m_nGameTime;

			BotPlayerData.m_MaxPai                = itorPlayerData->m_stGameInfo.m_MaxPai;
			BotPlayerData.m_MaxPaiTime            = itorPlayerData->m_stGameInfo.m_MaxPaiTime;
			BotPlayerData.m_MaxMoney              = itorPlayerData->m_stGameInfo.m_MaxMoney;
			BotPlayerData.m_MaxMoneyTime          = itorPlayerData->m_stGameInfo.m_MaxMoneyTime;
			BotPlayerData.m_MaxWin                = itorPlayerData->m_stGameInfo.m_MaxWin;
			BotPlayerData.m_MaxWinTime            = itorPlayerData->m_stGameInfo.m_MaxWinTime;

			BotPlayerData.m_nWinTimes             = BotPlayerData.m_nWinTimes/10;
			BotPlayerData.m_nLossTimes            = BotPlayerData.m_nLossTimes/10;

			BotPlayerData.m_JoinTime              = itorPlayerData->m_stGameInfo.m_JoinTime;
			BotPlayerData.m_InvitePID             = itorPlayerData->m_stUserDataInfo.m_InvitePID;
			BotPlayerData.m_BotLevel              = itorPlayerData->m_BotLevel;

			pSocket->SendMsg(BotPlayerData);

			DBServerPlayerInfo *pPlayerInfo = GetDBPlayerInfo(BotPlayerData.m_PID);
			if ( pPlayerInfo == nullptr )
			{
				pPlayerInfo = new DBServerPlayerInfo();
				m_PlayerInfos.insert(make_pair(BotPlayerData.m_PID,pPlayerInfo));
			}
			
			pPlayerInfo->m_AID            = BotPlayerData.m_AID;
			pPlayerInfo->m_PID            = BotPlayerData.m_PID;
			pPlayerInfo->m_ServerID       = pSocket->GetServerID();
			pPlayerInfo->m_PlayerType     = PLAYER_TYPE_BOT;
			pPlayerInfo->m_bOnLine        = true;
			pPlayerInfo->m_ActiveTime     = m_CurTime;
			pPlayerInfo->m_BotLevel       = BotPlayerData.m_BotLevel;

			pPlayerInfo->m_stUserDataInfo = itorPlayerData->m_stUserDataInfo;
			pPlayerInfo->m_stGameInfo     = itorPlayerData->m_stGameInfo;

			pPlayerInfo->ClearMoneyLog();
			pPlayerInfo->AddMoneyLog(itorPlayerData->m_stGameInfo.m_nGameMoney,"OnReqBotPlayerInfo");

			UpdatePlayerDataToMemcach(pPlayerInfo);
		}

		bp.m_Flag = bp.END;
		bp.m_XYID = DBServerXY::DBS_BotPlayerData::XY_ID;
		pSocket->SendMsg(bp);
	}
	else
	{
		pSocket->SendMsg(msgRespBP);
	}

	DebugInfo("CServer::OnReqBotPlayerInfo End");

	return 0;
}
void CServer::AddPlayerMoneyLog(UINT32 PID,INT64 nAddMoney,const string& strLog)
{
	if ( N_CeShiLog::c_MoneyLog )
	{
		PDBServerPlayerInfo pPlayerInfo = GetDBPlayerInfo(PID);
		if ( pPlayerInfo )
			pPlayerInfo->AddMoneyLog(nAddMoney,strLog);
		else
			DebugError("Error AddPlayerMoneyLog PID=%d Money=%s",PID,N2S(nAddMoney).c_str() );
	}
}
void CServer::PrintPlayerMoneyLog(DBServerPlayerInfo* pPlayerInfo)
{
	if ( N_CeShiLog::c_MoneyLog && pPlayerInfo )
	{
		DebugLog("PrintPlayerMoneyLog PID=%d LogSize=%d",pPlayerInfo->m_PID,pPlayerInfo->m_vectorMoneyLog.size());
		for ( size_t nPos=0;nPos<pPlayerInfo->m_vectorMoneyLog.size();++nPos )
		{
			DebugLog("%s",pPlayerInfo->m_vectorMoneyLog[nPos].c_str());
		}
		pPlayerInfo->ClearMoneyLog();
	}
}

void CServer::UpdatePlayerDataToMemcach(DBServerPlayerInfo* pPlayerInfo)
{
	CLogFuncTime lft(m_DBSFuncTime,"UpdatePlayerDataToMemcach");

	if ( pPlayerInfo && m_CurTime-pPlayerInfo->m_UpdateMemCachTime>N_Time::Day_1 )
	{
		pPlayerInfo->m_UpdateMemCachTime = m_CurTime;

		MemcatchXY::DeZhou::memUserLandRecord memULR;
		memULR.m_AID                = pPlayerInfo->m_AID;
		memULR.m_PID                = pPlayerInfo->m_PID;
		memULR.m_ContinueLogin      = pPlayerInfo->m_stLoginInfo.m_ContinueLogin;
		memULR.m_FirstTodayTime     = pPlayerInfo->m_stLoginInfo.m_FirstTodayTime;
		memULR.m_ContinuePlay       = pPlayerInfo->m_stLoginInfo.m_ContinuePlay;
		m_memOperator.SetUserLand(memULR);

		MemcatchXY::DeZhou::memUserDataInfo memUDI;
		memUDI.m_AID                = pPlayerInfo->m_AID;
		memUDI.m_PID                = pPlayerInfo->m_PID;
		//memUDI.m_ChangeName         = BYTE(pPlayerInfo->m_stUserDataInfo.m_ChangeName);
		//memUDI.m_HaveGameInfo       = BYTE(pPlayerInfo->m_stUserDataInfo.m_HaveGameInfo);
		memUDI.m_Sex                = BYTE(pPlayerInfo->m_stUserDataInfo.m_Sex);
		
		memUDI.m_InvitePID          = pPlayerInfo->m_stUserDataInfo.m_InvitePID;
		//memUDI.m_JoinTime           = pPlayerInfo->m_stUserDataInfo.m_JoinTime;
		//memUDI.m_PlayerLevel        = BYTE(pPlayerInfo->m_stUserDataInfo.m_PlayerLevel);
		memUDI.m_NickName           = pPlayerInfo->m_stUserDataInfo.m_NickName;
		memUDI.m_HeadPicURL         = pPlayerInfo->m_stUserDataInfo.m_HeadPicURL;
		memUDI.m_HomePageURL        = pPlayerInfo->m_stUserDataInfo.m_HomePageURL;
		memUDI.m_City               = pPlayerInfo->m_stUserDataInfo.m_City;
		m_memOperator.memSetUserDataInfo(memUDI);
	}
}

int CServer::OnPlayerDataNotExist( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	DBServerXY::DBS_PlayerDataNotExist msgPNE;
	TransplainMsg(msgPack,msgPNE);

	return 0;
}

int CServer::OnReqUserGiftInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	DBServerXY::DBS_ReqUserGiftInfo msgUGI;
	TransplainMsg(msgPack,msgUGI);

	return 0;
}

int CServer::OnRespPlayerOnLine( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnRespPlayerOnLine");

	DBServerXY::DBS_RespPlayerOnLine msgRespOnLine;
	TransplainMsg(msgPack,msgRespOnLine);

	DebugInfo("OnRespPlayerOnLine Start ServerID=%d",msgRespOnLine.m_ServerID);

	if ( msgRespOnLine.m_ServerID == pSocket->GetServerID() )
	{
		for (vector<UINT32>::iterator itorPID=msgRespOnLine.m_vecOnlinePID.begin();itorPID!=msgRespOnLine.m_vecOnlinePID.end();++itorPID)
		{
			DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(*itorPID);
			if ( pPlayerInfo )
			{
				pPlayerInfo->SetActiveTime(m_CurTime);
				pPlayerInfo->m_bOnLine = true;
			}
			else
			{
				DebugError("OnRespPlayerOnLine Can't Find PlayerInfo PID=%d",*itorPID);
			}
		}
		for (vector<UINT32>::iterator itorPID=msgRespOnLine.m_vecOutPID.begin();itorPID!=msgRespOnLine.m_vecOutPID.end();++itorPID)
		{
			DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(*itorPID);
			if ( pPlayerInfo )
			{
				pPlayerInfo->m_bOnLine = false;
			}
			else
			{
				DebugError("OnRespPlayerOnLine Can't Find PlayerInfo PID=%d",*itorPID);
			}
		}
	}
	else
	{
		DebugError("OnRespPlayerOnLine msgserverid=%d socketserverid=%d",msgRespOnLine.m_ServerID,pSocket->GetServerID());
	}

	DebugInfo("OnRespPlayerOnLine End");

	return 0;
}

int CServer::OnPlayerQuit( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnPlayerQuit");

	DBServerXY::DBS_PlayerQuite msgPQ;
	TransplainMsg(msgPack,msgPQ);

	DebugInfo("OnPlayerQuit Start ServerID=%d AID=%d PID=%d",msgPQ.m_ServerID,msgPQ.m_AID,msgPQ.m_PID);

	if ( pSocket->GetServerID() == msgPQ.m_ServerID )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgPQ.m_PID);
		if ( pPlayerInfo )
		{
			pPlayerInfo->m_bOnLine = false;

			UpdatePlayerDataToMemcach(pPlayerInfo);
			UpdatePlayerDataToDB(pPlayerInfo);
		}
	}

	DebugInfo("OnPlayerQuit End");
	return 0;
}

int CServer::OnReqRoomTableInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqRoomTableInfo");

	DBServerXY::DBS_ReqRoomTableInfo rrti;
	TransplainMsg(msgPack,rrti);

	DebugInfo("CServer::OnReqRoomTableInfo Start ServerID=%d",rrti.m_ServerID);
	
	int Ret = 0;
	if ( rrti.m_ServerID != pSocket->GetServerID() )
	{
		DBServerXY::DBS_RespRoomTableInfo rspRTI;
		rspRTI.m_Flag = rspRTI.SERVERIDERROR;
		pSocket->SendMsg(rspRTI);

		DebugError("CServer::OnReqRoomTableInfo rrti.m_ServerID=%d pSocket->m_ServerID=%d",rrti.m_ServerID,pSocket->GetServerID());

		return 100;
	}	
	
	//if ( m_mapFaceInfo.size() )
	//{
	//	DBServerXY::DBS_msgFaceInfo msgInfo;
	//	DBServerXY::DBS_FaceInfoList msgFIList;
	//	
	//	for(MapFaceInfo::iterator itorFI=m_mapFaceInfo.begin();itorFI!=m_mapFaceInfo.end();++itorFI)
	//	{
	//		stFaceInfo& rFI = itorFI->second;
	//		msgInfo.m_FaceID       = rFI.m_FaceID;
	//		msgInfo.m_Type         = rFI.m_Type;
	//		msgInfo.m_PriceFlag    = rFI.m_PriceFlag;
	//		msgInfo.m_Price        = rFI.m_Price;
	//		msgInfo.m_MinPrice     = rFI.m_MinPrice;
	//		msgInfo.m_MaxPrice     = rFI.m_MaxPrice;

	//		msgFIList.m_FIList[msgFIList.m_nCount++] = msgInfo;

	//		if ( msgFIList.m_nCount >= msgFIList.MAX_COUNT )
	//		{
	//			pSocket->SendMsg(msgFIList);
	//			msgFIList.ReSet();
	//		}
	//	}
	//	if ( msgFIList.m_nCount )
	//	{
	//		pSocket->SendMsg(msgFIList);
	//		msgFIList.ReSet();
	//	}
	//}

	//if ( m_mapGiftInfo.size() )
	//{
	//	DBServerXY::DBS_msgGiftInfo msgGI;
	//	DBServerXY::DBS_GiftInfoList msgGIList;
	//	for (MapGiftInfo::iterator itorGI=m_mapGiftInfo.begin();itorGI!=m_mapGiftInfo.end();++itorGI)
	//	{
	//		stGiftInfo& rGI = itorGI->second;
	//		msgGI.m_GiftID       = rGI.m_GiftID;
	//		msgGI.m_Type         = rGI.m_Type;
	//		msgGI.m_PriceFlag    = rGI.m_PriceFlag;
	//		msgGI.m_Price        = rGI.m_Price;
	//		msgGI.m_MinPrice     = rGI.m_MinPrice;
	//		msgGI.m_MaxPrice     = rGI.m_MaxPrice;
	//		msgGI.m_Rebate       = rGI.m_Rebate;
	//		msgGI.m_CurLastTime  = rGI.m_CurLastTime;
	//		msgGI.m_LastTime     = rGI.m_TotalLastTime;

	//		msgGIList.m_GIList[msgGIList.m_nCount++] = msgGI;

	//		if ( msgGIList.m_nCount >= msgGIList.MAX_COUNT )
	//		{
	//			pSocket->SendMsg(msgGIList);
	//			msgGIList.ReSet();
	//		}
	//	}
	//	if ( msgGIList.m_nCount )
	//	{
	//		pSocket->SendMsg(msgGIList);
	//		msgGIList.ReSet();
	//	}
	//}

	//if ( m_mapHonorInfo.size() )
	//{
	//	XieYiList<DBServerXY::DBS_HonorInfo,250> vectorHonorList;
	//	DBServerXY::DBS_HonorInfo msgHI;
	//	for ( MapHonorInfo::iterator itorHI=m_mapHonorInfo.begin();itorHI!=m_mapHonorInfo.end();++itorHI )
	//	{
	//		msgHI.m_HonorID    = itorHI->second.m_ID;
	//		msgHI.m_Type       = BYTE(itorHI->second.m_Type);
	//		msgHI.m_Idx        = BYTE(itorHI->second.m_Idx);
	//		vectorHonorList.m_XieYiList.push_back(msgHI);
	//	}
	//	pSocket->SendMsg(vectorHonorList);
	//}

	//VectorDBMatchInfo vectorMI;
	//if ( m_pDBOperator->ReadMatchInfo(rrti.m_ServerID,vectorMI) != DB_RESULT_SUCCESS )
	//{
	//	DebugError("ReadFaceInfo Error Error");
	//}
	//if ( vectorMI.size() )
	//{
	//	DBServerXY::DBS_MatchInfo msgMI;
	//	for (VectorDBMatchInfo::iterator itorMI=vectorMI.begin();itorMI!=vectorMI.end();++itorMI)
	//	{
	//		stDBMatchInfo& rMI = *itorMI;

	//		msgMI.m_MatchID      = rMI.m_MatchID;
	//		msgMI.m_MatchType    = rMI.m_MatchType;
	//		msgMI.m_Ticket       = rMI.m_Ticket;
	//		msgMI.m_TakeMoney    = rMI.m_nTakeMoney;
	//		msgMI.m_StartMoney   = rMI.m_StartMoney;
	//		msgMI.m_strBlind     = rMI.m_strBlind;
	//		msgMI.m_strAward     = rMI.m_strAward;
	//		msgMI.m_StrRule      = rMI.m_StrRule;
	//		msgMI.m_StartTime    = rMI.m_StartTime;
	//		msgMI.m_StartInterval= rMI.m_StartInterval;
	//		
	//		pSocket->SendMsg(msgMI);
	//	}
	//}

	VectorRoomInfo                       TempRoomInfoVector;
	VectorTableInfo                      TempTableInfoVector;
	m_pDBOperator->ReadRoomInfo(rrti.m_ServerID,TempRoomInfoVector);
	m_pDBOperator->ReadTableInfo(rrti.m_ServerID,TempTableInfoVector);

	DebugLogOut("OnReqRoomTableInfo ServerID=%d Room Size=%d Table Size=%d ",rrti.m_ServerID,TempRoomInfoVector.size(),TempTableInfoVector.size() );
	if ( TempRoomInfoVector.size()>0 )
	{
		DBServerXY::DBS_RespRoomTableInfo rspRTI;
		rspRTI.m_Flag = rspRTI.SUCCESS;
		pSocket->SendMsg(rspRTI);

		PublicXY::BatchProtocol bp;
		bp.m_Flag = bp.START;
		bp.m_XYID = rrti.XY_ID;
		pSocket->SendMsg(bp);

		DBServerXY::DBS_RoomInfo ri;
		for (unsigned int i=0;i<TempRoomInfoVector.size();++i)
		{
			if ( TempRoomInfoVector[i].m_ServerID == pSocket->GetServerID() )
			{
				ri.ReSet();				

				ri.m_RoomID      = TempRoomInfoVector[i].m_RoomID;
				ri.m_RoomName    = TempRoomInfoVector[i].m_RoomName;
				ri.m_Password    = TempRoomInfoVector[i].m_Password;
				ri.m_AreaRule    = TempRoomInfoVector[i].m_AreaRule;
				ri.m_RoomRule    = TempRoomInfoVector[i].m_RoomRule;
				ri.m_RoomRuleEX  = TempRoomInfoVector[i].m_RoomRuleEX;
				ri.m_MatchRule   = TempRoomInfoVector[i].m_MatchRule;
				ri.m_EndTime     = TempRoomInfoVector[i].m_EndTime;

				pSocket->SendMsg(ri);
			}
		}

		if ( TempTableInfoVector.size()>0 )
		{
			DBServerXY::DBS_TableInfo ti;
			for ( unsigned int i=0;i<TempTableInfoVector.size();++i)
			{
				if ( TempTableInfoVector[i].m_ServerID == pSocket->GetServerID() )
				{
					ti.ReSet();

					ti.m_RoomID           = TempTableInfoVector[i].m_RoomID;
					ti.m_TableID          = TempTableInfoVector[i].m_TableID;
					ti.m_TableName        = TempTableInfoVector[i].m_TableName;
					ti.m_Password         = TempTableInfoVector[i].m_Password;
					ti.m_TableRule        = TempTableInfoVector[i].m_TableRule;
					ti.m_FoundByWho       = TempTableInfoVector[i].m_FoundByWho;
					ti.m_TableEndTime     = TempTableInfoVector[i].m_TableEndTime;

					pSocket->SendMsg(ti);
				}
			}
		}		

		bp.m_Flag = bp.END;
		bp.m_XYID = rrti.XY_ID;
		if (pSocket)pSocket->SendMsg(bp);
	}
	else
	{
		DBServerXY::DBS_RespRoomTableInfo rspRTI;
		rspRTI.m_Flag = rspRTI.NOROOMTABLEINFO;
		pSocket->SendMsg(rspRTI);

		DebugError("CServer::OnReqRoomTableInfo No RoomTableInfo rrti.m_ServerID=%d pSocket->m_ServerID=%d",rrti.m_ServerID,pSocket->GetServerID());

		Ret = 100;
	}

	DebugInfo("CServer::OnReqRoomTableInfo End");

	return Ret;
}

void CServer::GetPlayerAwardInfo(INT16 AID,UINT32 PID,map<INT16,INT64>& mapAwardMoney)
{
	CLogFuncTime lft(m_DBSFuncTime,"GetPlayerAwardInfo");

	VectorAward vecUA;
	//m_pDBOperator->ReadUserAward(AID,PID,vecUA);

	vector<int> listUpdate;
	for (UINT i=0;i<vecUA.size();++i)
	{
		stDBUserAward& rUA = vecUA[i];

		if ( rUA.m_EndTime<=m_CurTime )
		{
			listUpdate.push_back(rUA.m_Idx);
			continue;
		}

		if ( rUA.m_PayMode == PM_OneTime )
		{
			listUpdate.push_back(rUA.m_Idx);
		}

		map<INT16,INT64>::iterator itorMoney = mapAwardMoney.find(rUA.m_MoneyFlag);
		if ( itorMoney == mapAwardMoney.end() )
		{
			mapAwardMoney.insert(make_pair(rUA.m_MoneyFlag,0));
			itorMoney = mapAwardMoney.find(rUA.m_MoneyFlag);
		}
		itorMoney->second += rUA.m_nMoney;
	}

	RWDB_UpdateAwardInfo msgUAI;
	for(int i=0;i<(int)listUpdate.size();++i)
	{
		msgUAI.ReSet();
		msgUAI.m_AwardIdx = listUpdate[i];
		m_RWDBMsgManager.PushRWDBMsg(msgUAI);
	}
}

int CServer::OnReqPlayerLogin(CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqPlayerLogin");

	DBServerXY::DBS_ReqServerPlayerLogin msgLogin;
	TransplainMsg(msgPack,msgLogin);
	
	UINT32 nStartTicket = ::GetTickCount();
	DebugInfo("CServer::OnReqPlayerLogin StartTicket=%d",nStartTicket);
	DebugLog("OnReqPlayerLogin AID=%d PID=%d PT=%d CT=%d LT=%d UserName=%s Password=%s HardCode=%s Session=%s",
		msgLogin.m_AID,msgLogin.m_PID,msgLogin.m_PlayerType,msgLogin.m_ClientType,msgLogin.m_LoginType,
		msgLogin.m_UserName.c_str(),msgLogin.m_Password.c_str(),msgLogin.m_HardCode.c_str(),msgLogin.m_strSession.c_str() );

	DBServerXY::DBS_RespServerPlayerLoginData msgBackData;
	msgBackData.m_AID = msgLogin.m_AID;
	msgBackData.m_PID = msgLogin.m_PID;
	msgBackData.m_Flag = msgBackData.SUCCESS;
	msgBackData.m_Socket = msgLogin.m_Socket;
	msgBackData.m_SessionKey = msgLogin.m_strLoginKey;

	INT64 nLandMoney     = 0;
	INT64 nOriginMoney   = 0;

	bool bRegisterUser   = false; //注册玩家
	bool bCreatePlayer   = false; //是否新创建玩家游戏数据
	bool bHaveFindPlayer = false; //服务器上存有玩家数据
	bool bFirstDayTime   = false; //当天第一次登陆

	Game::DeZhou::DBS::stDBUserDataInfo stUDI; //从数据库中取得的玩家数据
	if ( msgBackData.m_Flag == msgBackData.SUCCESS )
	{
		msgBackData.m_Flag = msgBackData.Error_Failed;
		if ( msgLogin.m_LoginType == Login_Type_Session )
		{
			if ( !(msgLogin.m_AID>0 && msgLogin.m_PID>0) )
			{
				DebugError("OnReqPlayerLogin AID=%d PID=%d",msgLogin.m_AID,msgLogin.m_PID);
			}
			string strGetSession = "";
			if ( m_memOperator.GetSession(msgLogin.m_AID,msgLogin.m_PID,strGetSession))
			{
				DebugLog("OnReqPlayerLogin PID=%d getSS=%s msgSS=%s",msgLogin.m_PID,strGetSession.c_str(),msgLogin.m_strSession.c_str() );
				if ( strGetSession.length() && strGetSession == msgLogin.m_strSession )
				{
					msgBackData.m_Flag = msgBackData.SUCCESS;
					if ( m_DBConfig.CeshiMode() == false )
					{
						m_memOperator.DelSession(msgLogin.m_AID,msgLogin.m_PID);
					}
				}
				else msgBackData.m_Flag = msgBackData.Error_SessionError;
			}
			else msgBackData.m_Flag = msgBackData.Error_NoSession;
		}
		else if ( msgLogin.m_LoginType == Login_Type_HardCode )
		{
			if ( msgLogin.m_AID>0 && msgLogin.m_HardCode.size()==MAX_SESSION_SIZE )
			{
				int ret = m_pDBOperator->ReadUserDataInfoByHardCode(msgLogin.m_HardCode,stUDI);
				if ( ret != DB_RESULT_SUCCESS )
				{
					msgBackData.m_Flag = msgBackData.Error_DBError;
				}
				else
				{
					if ( stUDI.m_PID == 0 ) //没有玩家信息自动给玩家注册
					{
						stRegisterUser stRU;
						stRU.m_AID       = msgLogin.m_AID;
						stRU.m_HardCode  = msgLogin.m_HardCode;
						stRU.m_NickName  = string("游客") + N2S(m_CurTime).substr(4,9);
						int retPID = m_pDBOperator->RegisterUser(stRU);
						if ( retPID > 0 )
						{
							bRegisterUser = true;
							msgBackData.m_Flag = msgBackData.SUCCESS;
							msgLogin.m_AID = msgLogin.m_AID;
							msgLogin.m_PID = retPID;

							stUDI.m_AID                  = msgLogin.m_AID;
							stUDI.m_PID                  = msgLogin.m_PID;
							stUDI.m_stUDI.m_UserName     = stRU.m_UserName;
							stUDI.m_stUDI.m_Password     = stRU.m_Password;
							stUDI.m_stUDI.m_HardCode     = stRU.m_HardCode;
							stUDI.m_stUDI.m_NickName     = stRU.m_NickName;
						}
					}
					else
					{
						if ( stUDI.m_stUDI.m_HardCode == msgLogin.m_HardCode )
						{
							msgBackData.m_Flag = msgBackData.SUCCESS;
							msgLogin.m_AID = stUDI.m_AID;
							msgLogin.m_PID = stUDI.m_PID;
						}
						else
						{
							msgBackData.m_Flag = msgBackData.Error_HardCode;							
							DebugError("OnReqPlayerLogin HardCode=%s DBHardCode=%s",msgLogin.m_HardCode.c_str(),stUDI.m_stUDI.m_HardCode.c_str() );
						}
					}
				}
			}
			else
			{
				DebugError("OnReqPlayerLogin AID=%d HardCode=%s",msgLogin.m_AID,msgLogin.m_HardCode.c_str() );
			}
		}
		else if ( msgLogin.m_LoginType == Login_Type_UserName )
		{
			if ( msgLogin.m_AID>0 && msgLogin.m_UserName.size() && msgLogin.m_Password.size() )
			{
				int ret = m_pDBOperator->ReadUserDataInfoByUserName(msgLogin.m_AID,msgLogin.m_UserName,stUDI);
				if ( ret != DB_RESULT_SUCCESS )
				{
					msgBackData.m_Flag = msgBackData.Error_DBError;
				}
				else
				{
					if ( stUDI.m_PID == 0 ) //没有玩家信息自动给玩家注册
					{
						stRegisterUser stRU;
						stRU.m_AID       = msgLogin.m_AID;
						stRU.m_UserName  = msgLogin.m_UserName;
						stRU.m_Password  = msgLogin.m_Password;
						stRU.m_NickName  = string("游客") + N2S(m_CurTime).substr(4,9);

						int retPID = m_pDBOperator->RegisterUser(stRU);
						if ( retPID > 0 )
						{
							bRegisterUser = true;

							msgBackData.m_Flag = msgBackData.SUCCESS;
							msgLogin.m_AID = msgLogin.m_AID;
							msgLogin.m_PID = retPID;

							stUDI.m_AID                  = msgLogin.m_AID;
							stUDI.m_PID                  = msgLogin.m_PID;
							stUDI.m_stUDI.m_UserName     = stRU.m_UserName;
							stUDI.m_stUDI.m_Password     = stRU.m_Password;
							stUDI.m_stUDI.m_HardCode     = stRU.m_HardCode;
							stUDI.m_stUDI.m_NickName     = stRU.m_NickName;
						}
					}
					else
					{
						if ( stUDI.m_stUDI.m_Password == msgLogin.m_Password )
						{
							msgBackData.m_Flag = msgBackData.SUCCESS;
							msgLogin.m_AID = stUDI.m_AID;
							msgLogin.m_PID = stUDI.m_PID;
						}
						else
						{
							msgBackData.m_Flag = msgBackData.Error_Password;
							DebugError("OnReqPlayerLogin Password=%s DBPassword=%s",msgLogin.m_Password.c_str(),stUDI.m_stUDI.m_Password.c_str() );
						}
					}
				}
			}
			else
			{
				DebugError("OnReqPlayerLogin AID=%d HardCode=%s",msgLogin.m_AID,msgLogin.m_HardCode.c_str() );
			}
		}
		else if ( msgLogin.m_LoginType == Login_Type_CeShi )
		{
			if ( N_CeShiLog::c_NoCheckLogin && msgLogin.m_AID==N_AREAID::AREAID_CESHI && msgLogin.m_PID>=1 && msgLogin.m_PID<=1000 )
			{
				msgBackData.m_Flag = msgBackData.SUCCESS;
			}
		}
	}

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgLogin.m_PID);
	if ( pPlayerInfo )
	{
		bHaveFindPlayer = true;

		if ( pPlayerInfo->m_AID != msgLogin.m_AID )
		{
			msgLogin.m_AID = pPlayerInfo->m_AID;
			//msgBackData.m_Flag = msgBackData.Error_AID;			
			DebugError("pPlayerInfo->m_AID=%d msgLogin.m_AID=%d",pPlayerInfo->m_AID,msgLogin.m_AID );
		}
		//如果原来的玩家对象还活着，而且不是同一台服务器上来的将判断认证不通过
		//这里相当于将限制玩家跨服务器游戏，还需要再进一步考虑，目前在同一区人数不多的情况不考虑复杂化
		if ( pPlayerInfo->m_ServerID != pSocket->GetServerID() )
		{
			DebugError("PISID=%d SocketSID=%d AID=%d PID=%d",pPlayerInfo->m_ServerID,pSocket->GetServerID(),msgLogin.m_AID,msgLogin.m_PID );
			msgBackData.m_Flag = msgBackData.Error_ServerID;
		}
	}
	else
	{
		bHaveFindPlayer = false;
		try
		{
			pPlayerInfo = new DBServerPlayerInfo();
		}
		catch (...){}
		
		if ( !pPlayerInfo )
		{
			msgBackData.m_Flag = msgBackData.Error_OutMemory;
		}
		else
		{
			pPlayerInfo->m_AID            = msgLogin.m_AID;
			pPlayerInfo->m_PID            = msgLogin.m_PID;
			pPlayerInfo->m_ServerID       = pSocket->GetServerID();
		}
	}	

	if ( msgBackData.m_Flag == msgBackData.SUCCESS )
	{
		MemcatchXY::DeZhou::memUserDataInfo memUDI;
		if ( bHaveFindPlayer==false )
		{
			if ( stUDI.m_PID )
			{
				pPlayerInfo->m_stUserDataInfo = stUDI.m_stUDI;
			}
			else
			{
				if ( m_memOperator.memGetUserDataInfo(msgLogin.m_AID,msgLogin.m_PID,memUDI) == false )
				{
					int ret = m_pDBOperator->ReadUserDataInfoByPID(msgLogin.m_PID,stUDI);
					if ( ret != DB_RESULT_SUCCESS )
					{
						msgBackData.m_Flag = msgBackData.Error_DBError;
					}
					else
					{
						pPlayerInfo->m_stUserDataInfo = stUDI.m_stUDI;
					}
				}
				else
				{
					pPlayerInfo->m_stUserDataInfo.m_Sex             = memUDI.m_Sex;
					pPlayerInfo->m_stUserDataInfo.m_HeadID          = memUDI.m_HeadID;
					pPlayerInfo->m_stUserDataInfo.m_InvitePID       = memUDI.m_InvitePID;

					pPlayerInfo->m_stUserDataInfo.m_UserName        = memUDI.m_UserName;
					pPlayerInfo->m_stUserDataInfo.m_Password        = memUDI.m_Password;
					pPlayerInfo->m_stUserDataInfo.m_HardCode        = memUDI.m_HardCode;
					pPlayerInfo->m_stUserDataInfo.m_NickName        = memUDI.m_NickName;
					pPlayerInfo->m_stUserDataInfo.m_HeadPicURL      = memUDI.m_HeadPicURL;
					pPlayerInfo->m_stUserDataInfo.m_HomePageURL     = memUDI.m_HomePageURL;
					pPlayerInfo->m_stUserDataInfo.m_City            = memUDI.m_City;
				}
			}			
		}

		if ( msgBackData.m_Flag == msgBackData.SUCCESS )
		{
			Game::DeZhou::DBS::stDBGameInfo stUGI;

			bCreatePlayer = bRegisterUser;
			if ( bHaveFindPlayer == false && bCreatePlayer == false )
			{
				int ret = m_pDBOperator->ReadUserGameInfo(msgLogin.m_AID,msgLogin.m_PID,stUGI);
				if ( ret != DB_RESULT_SUCCESS )
				{
					DebugError("ReadUserGameInfo AID=%d PID=%d",msgLogin.m_AID,msgLogin.m_PID);
					if ( ret == DB_RESULT_NOPLAYER ) msgBackData.m_Flag = msgBackData.Error_NoPlayer;
					else msgBackData.m_Flag = msgBackData.Error_PlayerData;
				}
				else 
				{
					if ( stUGI.m_stGameInfo.m_Forbid == 1 )	
						msgBackData.m_Flag = msgBackData.Error_Forbid;					
					else
					{
						if ( stUGI.m_PID )
							pPlayerInfo->m_stGameInfo      = stUGI.m_stGameInfo;
						else
							bCreatePlayer = true;
					}
				}
			}

			if ( bCreatePlayer )
			{
				nOriginMoney = 50000;
				nLandMoney   = 5000;

				stCreateGameInfo stCPGI;
				stCPGI.m_AID           = msgLogin.m_AID;
				stCPGI.m_PID           = msgLogin.m_PID;
				stCPGI.m_LandMoney     = nLandMoney;
				stCPGI.m_OrigenMoney   = nOriginMoney;
				if ( m_pDBOperator->CreatePlayerGameInfo(stCPGI) != DB_RESULT_SUCCESS )//创建的时候会把玩家的注册及登陆奖励送进去
				{
					msgBackData.m_Flag = msgBackData.Error_CreateGamePlayer;

					//RWDB_DeleteCreateInfo rwdbmsgDCI;
					//rwdbmsgDCI.m_AID     = msgLogin.m_AID;
					//rwdbmsgDCI.m_PID     = msgLogin.m_PID;
					//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgDCI);
				}
				else
				{
					AddPlayerMoneyLog(msgLogin.m_PID,nOriginMoney+nLandMoney,"CreatePlayer");

					stUGI.m_AID                     = msgLogin.m_AID;
					stUGI.m_PID                     = msgLogin.m_PID;
					stUGI.m_stGameInfo.m_nGameMoney = nLandMoney + nOriginMoney;

					pPlayerInfo->m_stGameInfo       = stUGI.m_stGameInfo;
				}
			}			
		}
	}

	if ( msgBackData.m_Flag == msgBackData.SUCCESS && bHaveFindPlayer==false )
	{
		MemcatchXY::DeZhou::memUserLandRecord memULR;
		if ( m_memOperator.GetUserLand(msgLogin.m_AID,msgLogin.m_PID,memULR) )
		{
			pPlayerInfo->m_stLoginInfo.m_FirstTodayTime = memULR.m_FirstTodayTime;
			pPlayerInfo->m_stLoginInfo.m_ContinueLogin  = memULR.m_ContinueLogin;
			pPlayerInfo->m_stLoginInfo.m_ContinuePlay   = memULR.m_ContinuePlay;
		}
	}

	if ( msgBackData.m_Flag == msgBackData.SUCCESS )
	{
		if ( !bHaveFindPlayer )
		{
			m_PlayerInfos.insert(make_pair(pPlayerInfo->m_PID,pPlayerInfo));
		}
		pPlayerInfo->m_bOnLine = true;
		pPlayerInfo->SetActiveTime(m_CurTime);		
		pPlayerInfo->m_PlayerType = PLAYER_TYPE_PLAYER;

		UpdatePlayerDataToMemcach(pPlayerInfo);
	}
	else
	{		
		if ( !bHaveFindPlayer ) //登陆不成功将玩家数据清掉
		{
			safe_delete(pPlayerInfo);
		}
	}

	pSocket->SendMsg(msgBackData);

	//第一次登陆读取玩家相关信息
	if ( msgBackData.m_Flag == msgBackData.SUCCESS && (!bHaveFindPlayer) )
	{
		//VectorFriendInfo listFR;
		//m_pDBOperator->ReadFriendInfo(msgLogin.m_AID,msgLogin.m_PID,listFR);
		//if ( listFR.size() > 0 )
		//{
		//	pPlayerInfo->m_FriendCount = 0;
		//	pPlayerInfo->m_vectorFriend.clear();

		//	stUserFriendFlag stFF;
		//	for(int i=0;i<(int)listFR.size();++i)
		//	{
		//		stFF.m_PID = listFR[i].m_RightPID;
		//		stFF.m_Flag = INT8(listFR[i].m_Flag);
		//		if ( stFF.m_Flag==N_Friend::Follow || stFF.m_Flag==N_Friend::Both )
		//		{
		//			pPlayerInfo->m_FriendCount++;
		//		}

		//		DBServerPlayerInfo* pTempPI = GetDBPlayerInfo( stFF.m_PID );
		//		if ( pTempPI /* && pTempPI->IsOnLine() */ )  //玩家在DB中并且在线
		//		{
		//			pPlayerInfo->m_vectorFriend.push_back(stFF);
		//			pTempPI->UpdateFriendFlag(pPlayerInfo->m_PID,N_Friend::GetAnti(stFF.m_Flag));
		//		}
		//	}
		//}

		//VectorDBUserGiftInfo listUGI;
		//m_pDBOperator->ReadUserGiftInfo(msgLogin.m_AID,msgLogin.m_PID,listUGI);
		//if ( listUGI.size() )
		//{
		//	if ( pPlayerInfo->m_vectorUserGiftInfo.size() )
		//	{
		//		pPlayerInfo->m_vectorUserGiftInfo.clear();
		//	}
		//	if ( listUGI.size() > N_Gift::MaxGiftCount )
		//	{
		//		sort(listUGI.begin(),listUGI.end(),CCompareUGI());
		//	}
		//	int EndPos = max(0,int(listUGI.size()-N_Gift::MaxGiftCount));

		//	stUserGiftInfo stUGI;
		//	for(int i=listUGI.size()-1;i>=EndPos;--i)
		//	{
		//		const stDBUserGiftInfo& rDBUGI = listUGI[i];

		//		stUGI.Init();
		//		stUGI.m_GiftID        = rDBUGI.m_GiftID;
		//		stUGI.m_GiftIdx       = rDBUGI.m_GiftIdx;
		//		stUGI.m_Price         = rDBUGI.m_Price;
		//		stUGI.m_ActionTime    = rDBUGI.m_ActionTime;
		//		stUGI.m_NickName      = rDBUGI.m_NickName;
		//		pPlayerInfo->AddGift(stUGI);
		//	}
		//}
	}

	if ( msgBackData.m_Flag == msgBackData.SUCCESS )  //将玩家的数据发送给玩家
	{
		if ( msgLogin.m_HaveData == 0 )
		{
			SendPlayerGameData(pSocket,msgLogin.m_AID,msgLogin.m_PID);
			//SendPlayerFriend(pSocket,msgLogin.m_AID,msgLogin.m_PID);
			//SendPlayerGift(pSocket,msgLogin.m_AID,msgLogin.m_PID);
		}

		if ( Tool::IsSameDay(pPlayerInfo->m_stLoginInfo.m_FirstTodayTime,m_CurTime) == false )
		{
			if ( Tool::IsNearDay(pPlayerInfo->m_stLoginInfo.m_FirstTodayTime,m_CurTime) )
			{
				pPlayerInfo->m_stLoginInfo.m_ContinueLogin++;
			}
			else 
			{
				pPlayerInfo->m_stLoginInfo.m_ContinueLogin = 1;
				pPlayerInfo->m_stLoginInfo.m_ContinuePlay = 0;
			}

			pPlayerInfo->m_stLoginInfo.m_FirstTodayTime = m_CurTime;
			bFirstDayTime = true;

			MemcatchXY::DeZhou::memUserLandRecord memULR;
			memULR.m_AID                = pPlayerInfo->m_AID;
			memULR.m_PID                = pPlayerInfo->m_PID;
			memULR.m_ContinueLogin      = pPlayerInfo->m_stLoginInfo.m_ContinueLogin;
			memULR.m_FirstTodayTime     = pPlayerInfo->m_stLoginInfo.m_FirstTodayTime;
			memULR.m_ContinuePlay       = pPlayerInfo->m_stLoginInfo.m_ContinuePlay;
			m_memOperator.SetUserLand(memULR);

			if ( bCreatePlayer == false )
			{
				SendPlayerAward(msgLogin.m_AID,msgLogin.m_PID);
			}
		}

		if ( bCreatePlayer )
		{
			DBServerXY::msgUserAward stUA;
			DBServerXY::DBS_UserAwardInfoList msgUAIL;

			msgUAIL.m_AID    = msgLogin.m_AID;
			msgUAIL.m_PID    = msgLogin.m_PID;
			if ( nOriginMoney > 0 )
			{
				stUA.ReSet();
				stUA.m_MoneyFlag   = N_IncomeAndPay::JoinAward;
				stUA.m_nMoney      = nOriginMoney;

				msgUAIL.m_nCount++;
				msgUAIL.m_Money += nOriginMoney;
				msgUAIL.m_listAwardInfo.push_back(stUA);

				RWDB_IncomeAndPay msgIP;
				msgIP.m_AID             = msgLogin.m_AID;
				msgIP.m_PID             = msgLogin.m_PID;
				msgIP.m_Flag            = MoGui::N_IncomeAndPay::JoinAward;
				msgIP.m_nMoney          = nOriginMoney;
				m_RWDBMsgManager.PushRWDBMsg(msgIP);
			}
			if ( nLandMoney > 0 )
			{
				stUA.ReSet();
				stUA.m_MoneyFlag   = N_IncomeAndPay::LandAward;
				stUA.m_nMoney      = nLandMoney;

				msgUAIL.m_nCount++;
				msgUAIL.m_Money += nLandMoney;
				msgUAIL.m_listAwardInfo.push_back(stUA);

				RWDB_IncomeAndPay msgIP;
				msgIP.m_AID             = msgLogin.m_AID;
				msgIP.m_PID             = msgLogin.m_PID;
				msgIP.m_Flag            = MoGui::N_IncomeAndPay::LandAward;
				msgIP.m_nMoney          = nLandMoney;
				m_RWDBMsgManager.PushRWDBMsg(msgIP);
			}

			if ( msgUAIL.m_Money > 0 ) pSocket->SendMsg(msgUAIL);
			
			//AddUserGameMoney(msgLogin.m_AID,msgLogin.m_PID,nOriginMoney+nLandMoney);
			//CheckPlayerGameMoney(pPlayerInfo->m_PID);
		}
	}

	if ( msgBackData.m_Flag )
	{
		DebugError("PlayerLogin Failed Flag=%d AID=%d PID=%d Create=%d Find=%d DayFirst=%d",
			msgBackData.m_Flag,msgBackData.m_AID,msgBackData.m_PID,bCreatePlayer,bHaveFindPlayer,bFirstDayTime );
	}
	else
	{
		DebugLogOut("PlayerLogin Success AID=%d PID=%d Create=%d Find=%d DayFirst=%d",
			msgBackData.m_AID,msgBackData.m_PID,bCreatePlayer,bHaveFindPlayer,bFirstDayTime );

		RWDB_PlayerLogin rwdbMsgPL;
		rwdbMsgPL.m_AID      = msgLogin.m_AID;
		rwdbMsgPL.m_PID      = msgLogin.m_PID;
		rwdbMsgPL.m_IP       = msgLogin.m_IP;
		m_RWDBMsgManager.PushRWDBMsg(rwdbMsgPL);

		RWDB_AddPlayerInfo msgAPI;
		msgAPI.m_AID         = msgLogin.m_AID;
		msgAPI.m_PID         = msgLogin.m_PID;
		msgAPI.m_Flag        = msgAPI.LandTime;
		m_RWDBMsgManager.PushRWDBMsg(msgAPI);
	}

	UINT32 nEndTicket = UINT32(::GetTickCount());
	DebugInfo("CServer::OnReqPlayerLogin EndTicket=%d UseTicket=%d",nEndTicket,nEndTicket-nStartTicket);
	return 0;
}
void CServer::DoUserProduct(const DBS::stUserProduct& stUP)
{
	CLogFuncTime lft(m_DBSFuncTime,"DoUserProduct");

	int TempProductID = stUP.m_ProductID;
	int TempProductIdx = stUP.m_Idx;

	INT16 ProType = 0;
	MapProductInfo::iterator itorPI = m_mapProductInfo.find(TempProductID);
	if ( itorPI != m_mapProductInfo.end() )
	{
		ProType = itorPI->second.m_ProductType;
	}
	else
	{
		m_pDBOperator->UpdateUserProduct(TempProductIdx,4);
		DebugError("Find m_mapProductInfo ProID=%d ProIdx=%d",TempProductID,TempProductIdx);
	}
	if ( ProType > 0 )
	{
		if ( ProType == ProductType_HuiYuan )
		{
			MapHuiYuanInfo::iterator itorHY = m_mapHuiYuanInfo.find(TempProductID);
			if ( itorHY != m_mapHuiYuanInfo.end() )
			{
				DoPlayerHuiYuan(stUP,itorHY->second);
				m_pDBOperator->UpdateUserProduct(TempProductIdx,0);
			}
			else
			{
				DebugError("Find m_mapHuiYuanInfo ProID=%d ProIdx=%d",TempProductID,TempProductIdx);
			}
		}
		else if ( ProType == ProductType_MoguiExchange )
		{
			MapMoguiExchangeInfo::iterator itorME = m_mapMoguiExchangeInfo.find(TempProductID);
			if ( itorME != m_mapMoguiExchangeInfo.end() )
			{
				DoPlayerMoguiExchange(stUP,itorME->second);
				m_pDBOperator->UpdateUserProduct(TempProductIdx,0);
			}
		}
		else
		{
			DebugError("Unknow ProType=%d ProID=%d ProIdx=%d",ProType,TempProductID,TempProductIdx);
		}
	}
}
void CServer::DoPlayerHuiYuan(const DBS::stUserProduct& stUP,const DBS::stHuiYuanInfo& stHY)
{
	CLogFuncTime lft(m_DBSFuncTime,"DoPlayerHuiYuan");
	DebugInfo("DoPlayerHuiYuan Start");

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(stUP.m_PID);
	if ( pPlayerInfo )
	{
		INT64 nAddMoney = stHY.m_SendOneceMoney;
		INT32 nDayMoney = stHY.m_GivenDaysMoney;

		if ( nAddMoney > 0 )
		{
			AddPlayerMoneyLog(pPlayerInfo->m_PID,nAddMoney,"DoPlayerHuiYuan");
			AddUserGameMoney(pPlayerInfo->m_AID,pPlayerInfo->m_PID,nAddMoney);

			//pPlayerInfo->m_stGameInfo.m_nUpperLimite   += nAddMoney*10;
			//pPlayerInfo->m_stGameInfo.m_nLowerLimite   += nAddMoney/10;

			RWDB_IncomeAndPay msgIP;
			msgIP.m_AID             = pPlayerInfo->m_AID;
			msgIP.m_PID             = pPlayerInfo->m_PID;
			msgIP.m_nMoney          = nAddMoney;
			msgIP.m_Flag            = N_IncomeAndPay::PayMoney;
			m_RWDBMsgManager.PushRWDBMsg(msgIP);
		}
		if ( stHY.m_BankBox == 1 )
		{
		}

		if ( stHY.m_FreeFace == 1 )
		{
			//pPlayerInfo->m_stGameInfoEX.m_FreeFaceTime = max(m_CurTime,pPlayerInfo->m_stGameInfoEX.m_FreeFaceTime) + stHY.m_DuringTime;
		}
		if ( stHY.m_VipLevel > 0 )
		{
			//pPlayerInfo->m_stGameInfoEX.m_VipLevel = max(pPlayerInfo->m_stGameInfoEX.m_VipLevel,stHY.m_VipLevel);
			//pPlayerInfo->m_stGameInfoEX.m_VipEndTime = max(m_CurTime,pPlayerInfo->m_stGameInfoEX.m_VipEndTime) + stHY.m_DuringTime;
		}

		if ( nDayMoney > 0 )
		{
			RWDB_AddPlayerAward rwdbAPA;
			rwdbAPA.m_AID      = pPlayerInfo->m_AID;
			rwdbAPA.m_PID      = pPlayerInfo->m_PID;

			rwdbAPA.m_Money    = nDayMoney;
			rwdbAPA.m_nFlag    = N_IncomeAndPay::PayMoneyAward;
			rwdbAPA.m_PayMode  = PM_EveryDay;
			rwdbAPA.m_CanUse   = 1;
			rwdbAPA.m_EndTime  = m_CurTime+N_Time::Year_1;
			rwdbAPA.m_strLog   = "会员="+N2S(stHY.m_VipLevel);

			m_RWDBMsgManager.PushRWDBMsg(rwdbAPA);
		}

		RWDB_UpdateHuiYuan msgUHY;
		msgUHY.m_AID           = pPlayerInfo->m_AID;
		msgUHY.m_PID           = pPlayerInfo->m_PID;
		//msgUHY.m_FreeFaceTime  = pPlayerInfo->m_stGameInfoEX.m_FreeFaceTime;
		//msgUHY.m_VipLevel      = pPlayerInfo->m_stGameInfoEX.m_VipLevel;
		//msgUHY.m_VipEndTime    = pPlayerInfo->m_stGameInfoEX.m_VipEndTime;
		m_RWDBMsgManager.PushRWDBMsg(msgUHY);

		DBServerXY::DBS_UserHuiYuan DBS_msgUHY;
		DBS_msgUHY.m_AID            = pPlayerInfo->m_AID;
		DBS_msgUHY.m_PID            = pPlayerInfo->m_PID;
		DBS_msgUHY.m_AddMoney       = nAddMoney;
		DBS_msgUHY.m_DayMoney       = nDayMoney;
		DBS_msgUHY.m_ActionTime     = stUP.m_ActionTime;
		SendMsgToPlayer(DBS_msgUHY,pPlayerInfo->m_PID);
	}
	else
	{
		DebugError("DoPlayerHuiYuan PID=%d ProIdx=%d",stUP.m_PID,stUP.m_Idx);
	}
	DebugInfo("DoPlayerHuiYuan End");
}
void CServer::DoPlayerMoguiExchange(const DBS::stUserProduct& stUP,const DBS::stMoguiExchangeInfo& stME)
{
	CLogFuncTime lft(m_DBSFuncTime,"DoPlayerMoguiExchange");
	DebugInfo("DoPlayerMoguiExchange Start");

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(stUP.m_PID);
	if ( pPlayerInfo )
	{
		int nAddMoguiMoney = stME.m_MoGuiMoney;
		if ( nAddMoguiMoney > 0 )
		{			
			//pPlayerInfo->m_stGameInfo.m_nMoGuiMoney     += nAddMoguiMoney;
			//m_pDBOperator->AddUserMoGuiMoney(pPlayerInfo->m_AID,pPlayerInfo->m_PID,nAddMoguiMoney);

			//pPlayerInfo->m_stGameInfo.m_nUpperLimite    += nAddMoguiMoney*10000;
			//pPlayerInfo->m_stGameInfo.m_nLowerLimite    += nAddMoguiMoney*1000;

			//RWDB_MoGuiMoneyLog msgMoguiLog;		
			//msgMoguiLog.m_AID             = pPlayerInfo->m_AID;
			//msgMoguiLog.m_PID             = pPlayerInfo->m_PID;
			//msgMoguiLog.m_Money           = nAddMoguiMoney;
			//msgMoguiLog.m_Flag            = N_IncomeAndPay::Mogui_PayMoney;
			//m_RWDBMsgManager.PushRWDBMsg(msgMoguiLog);
		}

		DBServerXY::DBS_UserMoguiExchange DBS_msgME;
		DBS_msgME.m_AID            = pPlayerInfo->m_AID;
		DBS_msgME.m_PID            = pPlayerInfo->m_PID;
		DBS_msgME.m_AddMoguiMoney  = nAddMoguiMoney;
		DBS_msgME.m_ActionTime     = stUP.m_ActionTime;
		SendMsgToPlayer(DBS_msgME,pPlayerInfo->m_PID);
	}
	else
	{
		DebugError("DoPlayerMoguiExchange PID=%d ProIdx=%d",stUP.m_PID,stUP.m_Idx);
	}
	DebugInfo("DoPlayerMoguiExchange End");
}
int CServer::SendPlayerAward(INT16 AID,UINT32 PID)
{
	CLogFuncTime lft(m_DBSFuncTime,"SendPlayerAward");
	DebugInfo("SendPlayerAward Start");

	CDBServerSocket* pSocket = nullptr;
	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(PID);
	if ( pPlayerInfo )
	{
		pSocket = GetDBSocketByServerID(pPlayerInfo->m_ServerID);
	}
	if ( pSocket )
	{
		map<INT16,INT64> mapAwardMoney;
		GetPlayerAwardInfo( AID,PID,mapAwardMoney );

		if ( mapAwardMoney.size() )
		{
			INT64 nAddMoney = 0;
			INT64 nUpLimite = 0;
			INT64 nLowLimite = 0;
			DBServerXY::msgUserAward stUA;
			DBServerXY::DBS_UserAwardInfoList msgUAIL;

			msgUAIL.m_AID    = AID;
			msgUAIL.m_PID    = PID;
			msgUAIL.m_nCount = short(mapAwardMoney.size());

			for ( map<INT16,INT64>::iterator itorMoney=mapAwardMoney.begin();itorMoney!=mapAwardMoney.end();++itorMoney )
			{
				stUA.ReSet();
				stUA.m_MoneyFlag   = itorMoney->first;
				stUA.m_nMoney      = itorMoney->second;
				msgUAIL.m_listAwardInfo.push_back(stUA);

				nAddMoney += itorMoney->second;

				if ( stUA.m_MoneyFlag == N_IncomeAndPay::PayMoney )
				{
					nUpLimite += stUA.m_nMoney*2;
					nLowLimite += stUA.m_nMoney/10;
				}
			}

			if ( nAddMoney > 0)
			{
				msgUAIL.m_Money = nAddMoney;
				pSocket->SendMsg(msgUAIL);

				//RWDB_UpdateUserGameMoneyType rwdbmsgUGMT;
				//rwdbmsgUGMT.m_AID         = AID;
				//rwdbmsgUGMT.m_PID         = PID;
				//rwdbmsgUGMT.m_nGivenMoney = nAddMoney;
				//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);

				AddPlayerMoneyLog(PID,nAddMoney,"SendPlayerAward");
				AddUserGameMoney(AID,PID,nAddMoney);			
			}

			if ( nUpLimite>0 || nLowLimite>0 )
			{
				//RWDB_AddLimite rwdbMsgAL;
				//rwdbMsgAL.m_AID          = AID;
				//rwdbMsgAL.m_PID          = PID;
				//rwdbMsgAL.m_nUpperLimite = nUpLimite;
				//rwdbMsgAL.m_nLowerLimite = nLowLimite;
				//m_RWDBMsgManager.PushRWDBMsg(rwdbMsgAL);

				//pPlayerInfo->m_stGameInfo.m_nUpperLimite += nUpLimite;
				//pPlayerInfo->m_stGameInfo.m_nLowerLimite += nLowLimite;

				DBServerXY::DBS_PlayerLimite msgPL;
				msgPL.m_AID          = AID;
				msgPL.m_PID          = PID;
				msgPL.m_nUpperLimite = nUpLimite;
				msgPL.m_nLowerLimite = nLowLimite;
				pSocket->SendMsg(msgPL);
			}
		}
		RWDB_IncomeAndPay msgInPay;
		for (map<INT16,INT64>::iterator itorMoney=mapAwardMoney.begin();itorMoney!=mapAwardMoney.end();++itorMoney)
		{
			msgInPay.ReSet();
			msgInPay.m_AID = AID;
			msgInPay.m_PID = PID;
			msgInPay.m_Flag = itorMoney->first;
			msgInPay.m_nMoney = itorMoney->second;
			m_RWDBMsgManager.PushRWDBMsg(msgInPay);
		}
	}
	DebugInfo("SendPlayerAward End");
	return 0;
}

void CServer::SendPlayerGameData(CDBServerSocket* pSocket,INT16 AID,UINT32 PID)
{
	CLogFuncTime lft(m_DBSFuncTime,"SendPlayerGameData");
	DebugInfo("SendPlayerGameData Start");

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(PID);
	if ( pPlayerInfo )
	{
		DBServerXY::DBS_PlayerData msgPD;
		msgPD.m_AID              = pPlayerInfo->m_AID;
		msgPD.m_PID              = pPlayerInfo->m_PID;

		msgPD.m_Sex              = pPlayerInfo->m_stUserDataInfo.m_Sex;
		msgPD.m_HeadID           = pPlayerInfo->m_stUserDataInfo.m_HeadID;
		msgPD.m_PlayerType       = pPlayerInfo->m_PlayerType;
		msgPD.m_InvitePID        = pPlayerInfo->m_stUserDataInfo.m_InvitePID;

		msgPD.m_NickName         = pPlayerInfo->m_stUserDataInfo.m_NickName;
		msgPD.m_HeadPicURL       = pPlayerInfo->m_stUserDataInfo.m_HeadPicURL;
		msgPD.m_HomePageURL      = pPlayerInfo->m_stUserDataInfo.m_HomePageURL;
		msgPD.m_City             = pPlayerInfo->m_stUserDataInfo.m_City;

		msgPD.m_nGameMoney       = pPlayerInfo->m_stGameInfo.m_nGameMoney;
		msgPD.m_nMedal           = pPlayerInfo->m_stGameInfo.m_nMedal;
		msgPD.m_Diamond          = pPlayerInfo->m_stGameInfo.m_Diamond;
		msgPD.m_GoldenPig        = pPlayerInfo->m_stGameInfo.m_GoldenPig;
		msgPD.m_PigFood          = pPlayerInfo->m_stGameInfo.m_PigFood;
		
		msgPD.m_nJF              = pPlayerInfo->m_stGameInfo.m_nJF;
		msgPD.m_nEP              = pPlayerInfo->m_stGameInfo.m_nEP;
		msgPD.m_nWinTimes        = pPlayerInfo->m_stGameInfo.m_nWinTimes;
		msgPD.m_nLossTimes       = pPlayerInfo->m_stGameInfo.m_nLossTimes;
		msgPD.m_DrawnTimes       = pPlayerInfo->m_stGameInfo.m_DrawnTimes;
		msgPD.m_EscapeTimes      = pPlayerInfo->m_stGameInfo.m_EscapeTimes;
		msgPD.m_nGameTime        = pPlayerInfo->m_stGameInfo.m_nGameTime;

		msgPD.m_MaxPai           = pPlayerInfo->m_stGameInfo.m_MaxPai;
		msgPD.m_MaxPaiTime       = pPlayerInfo->m_stGameInfo.m_MaxPaiTime;
		msgPD.m_MaxMoney         = pPlayerInfo->m_stGameInfo.m_MaxMoney;
		msgPD.m_MaxMoneyTime     = pPlayerInfo->m_stGameInfo.m_MaxMoneyTime;
		msgPD.m_MaxWin           = pPlayerInfo->m_stGameInfo.m_MaxWin;
		msgPD.m_MaxWinTime       = pPlayerInfo->m_stGameInfo.m_MaxWinTime;

		msgPD.m_JoinTime         = pPlayerInfo->m_stGameInfo.m_JoinTime;

		msgPD.m_ContinuLogin     = pPlayerInfo->m_stLoginInfo.m_ContinueLogin;
		msgPD.m_ContinuPlay      = pPlayerInfo->m_stLoginInfo.m_ContinuePlay;

		pPlayerInfo->ClearMoneyLog();
		pPlayerInfo->AddMoneyLog(msgPD.m_nGameMoney,"SendPlayerGameData");
		
		pSocket->SendMsg(msgPD);
	}
	DebugInfo("SendPlayerGameData End");
}

int CServer::OnReqPlayerInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqPlayerInfo");

	DBServerXY::DBS_ReqPlayerInfo msgPI;
	TransplainMsg(msgPack,msgPI);

	UINT32 nStartTicket = ::GetTickCount();
	DebugInfo("CServer::OnReqPlayerInfo Start ServerID=%d AID=%d PID=%d",pSocket->GetServerID(),msgPI.m_AID,msgPI.m_PID);

	DBServerXY::DBS_RespPlayerInfo msgRespPI;
	msgRespPI.m_AID = msgPI.m_AID;
	msgRespPI.m_PID = msgPI.m_PID;

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgPI.m_PID);
	if ( !pPlayerInfo )
	{
		DebugError("OnReqPlayerInfo AID=%d PID=%d Can't Find pPData",msgPI.m_AID,msgPI.m_PID);
		msgPI.m_DataFlag = 0;
	}
	
	if ( (msgPI.m_DataFlag & UINT16(msgPI.PlayerData)) && msgRespPI.m_DataFlag == 0 )
	{
		
	}

	if ( (msgPI.m_DataFlag & UINT16(msgPI.Award)) && msgRespPI.m_DataFlag == 0 )
	{
		
	}

	if ( (msgPI.m_DataFlag & UINT16(msgPI.PlayerDataEx)) && msgRespPI.m_DataFlag == 0 )
	{

	}

	if ( (msgPI.m_DataFlag & UINT16(msgPI.PlayerMatchData)) && msgRespPI.m_DataFlag == 0 )
	{
	}

	//if ( (msgPI.m_DataFlag & UINT16(msgPI.Friend)) && msgRespPI.m_DataFlag == 0 )
	//{
	//	SendPlayerFriend(pSocket,msgRespPI.m_AID,msgRespPI.m_PID);
	//}
	//if ( (msgPI.m_DataFlag & UINT16(msgPI.Gift)) && msgRespPI.m_DataFlag == 0 )
	//{
	//	SendPlayerGift(pSocket,msgRespPI.m_AID,msgRespPI.m_PID);
	//}

	DebugInfo("CServer::OnReqPlayerInfo End UserTicket=%d",UINT32(::GetTickCount())-nStartTicket);
	return 0;
}

int CServer::OnReqAddBotMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqAddBotMoney");
	DBServerXY::DBS_ReqAddBotMoney msgABM;
	TransplainMsg(msgPack,msgABM);

	DebugInfo("CServer::OnReqAddBotMoney Start ServerID=%d AID=%d PID=%d Level=%d",
		pSocket->GetServerID(),msgABM.m_AID,msgABM.m_PID,msgABM.m_nLevel);

	DBServerXY::DBS_RespAddBotMoney msgRespABM;
	msgRespABM.m_AID = msgABM.m_AID;
	msgRespABM.m_PID = msgABM.m_PID;
	msgRespABM.m_nGameMoney = 0;
	msgRespABM.m_Flag = msgRespABM.UNSUCCESS;

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgABM.m_PID);
	if ( pPlayerInfo )
	{		
		if ( pPlayerInfo->m_PlayerType == PLAYER_TYPE_BOT && pPlayerInfo->m_AID == msgABM.m_AID
			&& pPlayerInfo->m_PID == msgABM.m_PID && pPlayerInfo->m_BotLevel == msgABM.m_nLevel )
		{
			int   BotLevel = pPlayerInfo->m_BotLevel;
			INT64 MaxMoney = GameFunc::GetBotLevelMoney(BotLevel);
			INT64 MinMoney = GameFunc::GetBotLevelMoney(BotLevel-1);
			INT64 nAddGameMoney = 0;

			if ( pPlayerInfo->m_stGameInfo.m_nGameMoney == msgABM.m_nGameMoney )
			{
				if ( msgABM.m_nGameMoney >= MaxMoney*2 )
				{
					nAddGameMoney = -((msgABM.m_nGameMoney-MaxMoney)/2);
				}
				else if ( msgABM.m_nGameMoney < MinMoney )
				{
					nAddGameMoney = MinMoney;
				}

				if ( nAddGameMoney != 0 )
				{
					msgRespABM.m_nGameMoney = nAddGameMoney;
					msgRespABM.m_Flag = msgRespABM.SUCCESS;
					pSocket->SendMsg(msgRespABM);

					CheckPlayerGameMoney(pPlayerInfo->m_PID);

					RWDB_IncomeAndPay dbmsgInPay;
					dbmsgInPay.m_AID = msgABM.m_AID;
					dbmsgInPay.m_PID = msgABM.m_PID;
					dbmsgInPay.m_nMoney = nAddGameMoney;
					if ( nAddGameMoney >= 0 ) dbmsgInPay.m_Flag = N_IncomeAndPay::BotAdd;
					else dbmsgInPay.m_Flag = N_IncomeAndPay::BotDec;
					m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);
					
					AddPlayerMoneyLog(msgABM.m_PID,nAddGameMoney,"OnReqAddBotMoney");
					AddUserGameMoney(msgABM.m_AID,msgABM.m_PID,nAddGameMoney);
				}
			}
			else
			{
				msgRespABM.m_Flag = msgRespABM.Money_error;
				DebugError("OnReqAddBotMoney AID=%d PID=%d Level=%d GameMoney=%s nAddMoney=%s",
					msgABM.m_AID,msgABM.m_PID,msgABM.m_nLevel,Tool::N2S(msgABM.m_nGameMoney.get()).c_str(),Tool::N2S(nAddGameMoney).c_str());
			}
		}
	}

	if ( msgRespABM.m_Flag != msgRespABM.SUCCESS )
	{
		pSocket->SendMsg(msgRespABM);
	}

	DebugInfo("CServer::OnReqAddBotMoney End");
	return 0;
}

int CServer::OnReqPlayerGameMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqPlayerGameMoney");

	DBServerXY::DBS_ReqPlayerGameMoney msgPGM;
	TransplainMsg(msgPack,msgPGM);

	DebugInfo("CServer::OnReqPlayerGameMoney Start ServerID=%d AID=%d PID=%d nMoney=%s",
		pSocket->GetServerID(),msgPGM.m_AID,msgPGM.m_PID,Tool::N2S(msgPGM.m_nGameMoney.get()).c_str() );

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgPGM.m_PID);
	if ( pPlayerInfo )
	{
		DBServerXY::DBS_RespPlayerGameMoney respPGM;
		respPGM.m_AID = msgPGM.m_AID;
		respPGM.m_PID = msgPGM.m_PID;
		respPGM.m_nDiffMoney = pPlayerInfo->m_stGameInfo.m_nGameMoney - msgPGM.m_nGameMoney;
		pSocket->SendMsg(respPGM);

		if ( m_DBConfig.CeshiMode() )
		{
			stDBGameMoney stGM;
			if ( m_pDBOperator->ReadUserGameMoney(msgPGM.m_AID,msgPGM.m_PID,stGM) == DB_RESULT_SUCCESS )
			{
				if ( stGM.m_GameMoney != pPlayerInfo->m_stGameInfo.m_nGameMoney || stGM.m_GameMoney != msgPGM.m_nGameMoney )
				{
					DebugError("OnReqPlayerGameMoney dbMoney=%s memMoney=%s gsMoney=%s",
						N2S(stGM.m_GameMoney).c_str(),N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(msgPGM.m_nGameMoney.get()).c_str() );
				}
			}		
		}
	}

	DebugInfo("CServer::OnReqPlayerGameMoney End");

	return 0;
}

void CServer::SendPlayerFriend(CDBServerSocket* pSocket,INT16 AID,UINT32 PID)
{
	CLogFuncTime lft(m_DBSFuncTime,"SendPlayerFriend");
	DebugInfo("CServer::SendPlayerFriend Start ServerID=%d AID=%d PID=%d",pSocket->GetServerID(),AID,PID);

	if ( AID>0 && PID>0 )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(PID);
		if ( pPlayerInfo )
		{
			DBServerXY::DBS_UserFriendInfo msgUFI;
			msgUFI.m_AID     = pPlayerInfo->m_AID;
			msgUFI.m_PID     = pPlayerInfo->m_PID;
			msgUFI.m_Flag    = N_Friend::Follow;
			for (size_t nPos=0;nPos<pPlayerInfo->m_vectorFriend.size();++nPos)
			{
				const stUserFriendFlag& rUFF = pPlayerInfo->m_vectorFriend[nPos];
				if ( rUFF.m_Flag==N_Friend::Follow || rUFF.m_Flag==N_Friend::Both )
				{
					UINT32 TempPID = rUFF.m_PID;
					DBServerPlayerInfo* pTempPI = GetDBPlayerInfo(TempPID);
					if ( pTempPI && pTempPI->IsOnLine() )
					{
						msgUFI.m_vecFriendID.push_back(TempPID);
						if ( msgUFI.m_vecFriendID.size() >= msgUFI.MAX_FRIENDNUM )
						{
							pSocket->SendMsg(msgUFI);
							msgUFI.m_vecFriendID.clear();
						}
					}
				}
			}
			if ( msgUFI.m_vecFriendID.size() )
			{
				pSocket->SendMsg(msgUFI);
				msgUFI.m_vecFriendID.clear();
			}

			msgUFI.m_AID     = pPlayerInfo->m_AID;
			msgUFI.m_PID     = pPlayerInfo->m_PID;
			msgUFI.m_Flag    = N_Friend::BeFollow;
			for (size_t nPos=0;nPos<pPlayerInfo->m_vectorFriend.size();++nPos)
			{
				const stUserFriendFlag& rUFF = pPlayerInfo->m_vectorFriend[nPos];
				if ( rUFF.m_Flag == N_Friend::BeFollow || rUFF.m_Flag == N_Friend::Both )
				{
					UINT32 TempPID = rUFF.m_PID;
					DBServerPlayerInfo* pTempPI = GetDBPlayerInfo(TempPID);				
					if ( pTempPI && pTempPI->IsOnLine() )
					{
						msgUFI.m_vecFriendID.push_back(TempPID);
						if ( msgUFI.m_vecFriendID.size() >= msgUFI.MAX_FRIENDNUM )
						{
							pSocket->SendMsg(msgUFI);
							msgUFI.m_vecFriendID.clear();
						}
					}
				}
			}
			if ( msgUFI.m_vecFriendID.size() > 0 )
			{
				pSocket->SendMsg(msgUFI);
				msgUFI.m_vecFriendID.clear();
			}

			DBServerXY::DBS_To_GS_Flag msgGSF;
			msgGSF.m_AID            = pPlayerInfo->m_AID;
			msgGSF.m_PID            = pPlayerInfo->m_PID;
			msgGSF.m_Flag           = msgGSF.SendFriendOver;
			pSocket->SendMsg(msgGSF);
		}
	}
	DebugInfo("CServer::SendPlayerFriend End");
}
int CServer::UserGiftToDBSGiftInfo(const stUserGiftInfo& stUGI,DBServerXY::DBS_msgUserGiftInfo& msgGI)
{
	UINT32 curTime = m_CurTime;
	int nRet = 0;
	MapGiftInfo::iterator itorGI = m_mapGiftInfo.find(stUGI.m_GiftID);
	if ( itorGI != m_mapGiftInfo.end() )
	{
		const stGiftInfo& r_stGI = itorGI->second;

		msgGI.m_GiftID       = stUGI.m_GiftID;
		msgGI.m_GiftIdx      = stUGI.m_GiftIdx;
		msgGI.m_ActionTime   = stUGI.m_ActionTime;
		msgGI.m_NickName     = stUGI.m_NickName;

		if ( curTime < stUGI.m_ActionTime + r_stGI.m_CurLastTime )
		{
			nRet = 1;
		}
		else if ( curTime < stUGI.m_ActionTime + r_stGI.m_TotalLastTime )
		{
			nRet = 2;
		}
		else 
		{
			nRet = 3;
		}

		if ( nRet==1 || nRet==2 )
		{
			
			int GiftRealPrice = 0;
			if ( r_stGI.m_PriceFlag == N_Gift::PriceFlag_MoGui )
			{
				GiftRealPrice = r_stGI.m_Price*N_Gift::M2G_Rate;
			}
			else
			{
				GiftRealPrice = stUGI.m_Price;
			}

			int nPrice = 0;
			double FirstRate = 0;
			double SecondRate = 0;
			if ( nRet == 1 )
			{
				FirstRate = double(r_stGI.m_Rebate)/100.0;
				nPrice = int( GiftRealPrice*FirstRate );
			}
			else if ( nRet == 2 )
			{
				UINT32 nEndTime = stUGI.m_ActionTime + r_stGI.m_TotalLastTime;
				FirstRate    = double(r_stGI.m_Rebate)/100.0;
				SecondRate   = double(nEndTime-curTime)/(r_stGI.m_TotalLastTime*1.0);
				SecondRate   = min(1.0,SecondRate);
				nPrice       = int(GiftRealPrice*FirstRate*SecondRate);
			}

			if ( nPrice <= 0 )
			{
				//DebugError("UserGiftToDBSGiftInfo nRet=%d GiftIdx=%d RealPrice=%d Price=%d EndTime=%s ",
				//	nRet,stUGI.m_GiftIdx,GiftRealPrice,nPrice,Tool::GetTimeString(stUGI.m_ActionTime+r_stGI.m_TotalLastTime).c_str() );
			}

			nPrice        = max(nPrice,1);
			msgGI.m_Price = max(nPrice,GiftRealPrice/5);
		}
	}
	return nRet;
}
void CServer::CheckPlayerGameMoney(UINT32 PID)
{
	if ( N_CeShiLog::c_MoneyLog )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(PID);
		if ( pPlayerInfo )
		{
			DBServerXY::DBS_PlayerMoneyCheck msgPMC;
			msgPMC.m_PID        = PID;
			msgPMC.m_GameMoney  = pPlayerInfo->m_stGameInfo.m_nGameMoney;
			SendMsgToPlayer(msgPMC,PID);
		}
	}
}
void CServer::SendPlayerGift(CDBServerSocket* pSocket,INT16 AID,UINT32 PID)
{
	CLogFuncTime lft(m_DBSFuncTime,"SendPlayerGift");
	DebugInfo("CServer::SendPlayerGift Start ServerID=%d AID=%d PID=%d",pSocket->GetServerID(),AID,PID);

	if ( AID>0 && PID>0 )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(PID);
		if ( pPlayerInfo )
		{
			DBServerXY::DBS_UserGiftInfoList msgCurUGIL,msgOutUGIL;
			DBServerXY::DBS_msgUserGiftInfo  stUGI;
			vector<int> vecDeleteGIdx;

			msgCurUGIL.m_Flag = N_Gift::GiftTime_Cur;
			msgOutUGIL.m_Flag = N_Gift::GiftTime_Out;
			msgOutUGIL.m_AID = msgCurUGIL.m_AID = pPlayerInfo->m_AID;
			msgOutUGIL.m_PID = msgCurUGIL.m_PID = pPlayerInfo->m_PID;
			for ( size_t nPos=0;nPos<pPlayerInfo->m_vectorUserGiftInfo.size();++nPos )
			{
				const stUserGiftInfo& rUGI = pPlayerInfo->m_vectorUserGiftInfo[nPos];
				stUGI.ReSet();				
				int nRet = UserGiftToDBSGiftInfo(rUGI,stUGI);
				if ( nRet == 1 )
				{
					msgCurUGIL.m_listGiftInfo.push_back(stUGI);
					if ( msgCurUGIL.m_listGiftInfo.size() >= msgCurUGIL.MAX_COUNT )
					{
						pSocket->SendMsg(msgCurUGIL);
						msgCurUGIL.m_listGiftInfo.clear();
					}
				}
				else if ( nRet == 2 )
				{
					msgOutUGIL.m_listGiftInfo.push_back(stUGI);
					if ( msgOutUGIL.m_listGiftInfo.size() >= msgOutUGIL.MAX_COUNT )
					{
						pSocket->SendMsg(msgOutUGIL);
						msgOutUGIL.m_listGiftInfo.clear();
					}
				}
				else
				{
					vecDeleteGIdx.push_back(rUGI.m_GiftIdx);
				}
			}
			if ( msgCurUGIL.m_listGiftInfo.size() )
			{
				pSocket->SendMsg(msgCurUGIL);
			}
			if ( msgOutUGIL.m_listGiftInfo.size() )
			{
				pSocket->SendMsg(msgOutUGIL);
			}

			RWDB_UpdateGiftInfo dbmsgUGI;
			for (UINT32 i=0;i<vecDeleteGIdx.size();++i)
			{
				pPlayerInfo->DeleteGift(vecDeleteGIdx.at(i));

				dbmsgUGI.ReSet();
				dbmsgUGI.m_GiftIdx = vecDeleteGIdx[i];
				dbmsgUGI.m_Flag    = N_Gift::GiftST_OutTime;
				dbmsgUGI.m_PID     = pPlayerInfo->m_PID;
				m_RWDBMsgManager.PushRWDBMsg(dbmsgUGI);
			}
		}
	}

	DebugInfo("CServer::SendPlayerGift End");
}
void CServer::AddUserGameMoney(INT16 AID,UINT32 PID,INT64 nMoney,bool bUpToDB)
{
	CLogFuncTime lft(m_DBSFuncTime,"AddUserGameMoney");

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(PID);
	if ( pPlayerInfo )
	{
		pPlayerInfo->m_stGameInfo.m_nGameMoney += nMoney;
		if ( pPlayerInfo->m_stGameInfo.m_nGameMoney < 0 )
		{
			DebugError("AddUserGameMoney PID=%d GameMoney=%s nAddMoney=%s",
				PID,N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(nMoney).c_str() );

			CheckPlayerGameMoney(pPlayerInfo->m_PID);
			PrintPlayerMoneyLog(pPlayerInfo);
			pPlayerInfo->m_stGameInfo.m_nGameMoney = 0;
		}

		if ( bUpToDB )
		{
			if( m_DBConfig.m_QuickDB )
			{
				RWDB_AddGameMoney rwdbmsgAGM;
				rwdbmsgAGM.m_AID            = AID;
				rwdbmsgAGM.m_PID            = PID;
				rwdbmsgAGM.m_GameMoney      = nMoney;
				rwdbmsgAGM.m_HongBao        = 0;
				m_RWDBMsgManager.PushRWDBMsg(rwdbmsgAGM);
			}
			else
			{
				m_pDBOperator->DBAddUserGameMoney(AID,PID,nMoney);
			}
		}
		
		if ( N_CeShiLog::c_MoneyLog && m_DBConfig.m_QuickDB==0 )
		{
			stDBGameMoney stGM;
			m_pDBOperator->ReadUserGameMoney(AID,PID,stGM);
			if ( stGM.m_GameMoney != pPlayerInfo->m_stGameInfo.m_nGameMoney )
			{
				DebugError("PlayerMoney=%s DBMoney=%s",N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(stGM.m_GameMoney).c_str());
				PrintPlayerMoneyLog(pPlayerInfo);
			}
		}
	}
	else
	{
		DebugError("AddUserGameMoney PID=%d nMoney=%s",PID,N2S(nMoney).c_str());
	}
}

int CServer::UpdatePlayerDataToDB(DBServerPlayerInfo* pPlayerInfo)
{
	assert(pPlayerInfo);

	//RWDB_GameRight rwdbMR;
	//rwdbMR.m_AID    = pPlayerInfo->m_AID;
	//rwdbMR.m_PID    = pPlayerInfo->m_PID;
	//rwdbMR.m_Right  = pPlayerInfo->m_RightInfo.GetRight();
	//m_RWDBMsgManager.PushRWDBMsg(rwdbMR);

	//RWDB_UpdateWinType rwdbUWT;
	//rwdbUWT.m_AID    = pPlayerInfo->m_AID;
	//rwdbUWT.m_PID    = pPlayerInfo->m_PID;
	////memcpy(rwdbUWT.m_WinRecord,pPlayerInfo->m_stGameInfoEX.m_WinRecord,sizeof(rwdbUWT.m_WinRecord));
	//m_RWDBMsgManager.PushRWDBMsg(rwdbUWT);

	return 0;
}

int CServer::WDBWinLoss(DBServerXY::WDB_WinLoss& wl)
{
	CLogFuncTime lft(m_DBSFuncTime,"WDBWinLoss");
	DebugInfo("WDBWinLoss PID=%d Money=%s",wl.m_PID,Tool::N2S(wl.m_Money.get()).c_str());
	if ( wl.m_PID > 0 )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(wl.m_PID);

		INT64 nUpLimite   = wl.m_nServiceMoney*10;
		INT64 nLowLimite  = max(1,wl.m_nServiceMoney/10);
		INT64 nAddMoeny   = wl.m_nWinLossMoney-wl.m_nServiceMoney;

		//RWDB_UpdateUserGameMoneyType rwdbMsgUUGMT;
		//rwdbMsgUUGMT.m_AID             = wl.m_AID;
		//rwdbMsgUUGMT.m_PID             = wl.m_PID;
		//rwdbMsgUUGMT.m_nServiceMoney   = int(wl.m_nServiceMoney);
		//if ( wl.m_EndGameFlag == TALLY_WIN )
		//{
		//	rwdbMsgUUGMT.m_nWinMoney = wl.m_nWinLossMoney;
		//	rwdbMsgUUGMT.m_nLossMoney = 0;
		//}
		//else
		//{
		//	rwdbMsgUUGMT.m_nWinMoney = 0;
		//	rwdbMsgUUGMT.m_nLossMoney = -wl.m_nWinLossMoney;
		//}
		//m_RWDBMsgManager.PushRWDBMsg(rwdbMsgUUGMT);

		RWDB_WinLossInfo rwdbWLI;
		rwdbWLI.m_AID           = wl.m_AID;
		rwdbWLI.m_PID           = wl.m_PID;
		rwdbWLI.m_JF            = wl.m_JF;
		rwdbWLI.m_EP            = wl.m_EP;
		if ( wl.m_EndGameFlag == TALLY_WIN )
		{
			rwdbWLI.m_WinTimes = 1;
			rwdbWLI.m_LossTimes = 0;
		}
		else
		{
			rwdbWLI.m_WinTimes = 0;
			rwdbWLI.m_LossTimes = 1;
		}
		//rwdbWLI.m_nUpLimite       = nUpLimite;
		//rwdbWLI.m_nLowLimite      = nLowLimite;
		m_RWDBMsgManager.PushRWDBMsg(rwdbWLI);

		if ( m_DBConfig.m_QuickDB )
		{
			RWDB_AddGameMoney rwdbAGM;
			rwdbAGM.m_AID        = wl.m_AID;
			rwdbAGM.m_PID        = wl.m_PID;
			rwdbAGM.m_GameMoney  = nAddMoeny;
			rwdbAGM.m_HongBao    = 0;
			m_RWDBMsgManager.PushRWDBMsg(rwdbAGM);
		}
		else
		{
			stWinLossMoney stWLM;
			stWLM.m_AID             = wl.m_AID;
			stWLM.m_PID             = wl.m_PID;
			stWLM.m_nGameMoney      = nAddMoeny;
			stWLM.m_nHongBao        = 0;
			int nDBRet = m_pDBOperator->UpdateWinLossMoney(stWLM);
			if ( pPlayerInfo && nDBRet!=DB_RESULT_SUCCESS )
			{			
				DebugError("WDBWinLoss CurMoney=%s AddMoney=%s",N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(nAddMoeny).c_str() );

				PrintPlayerMoneyLog(pPlayerInfo);
				CheckPlayerGameMoney(pPlayerInfo->m_PID);
			}
		}

		if ( pPlayerInfo )
		{
			pPlayerInfo->SetActiveTime(m_CurTime);

			AddPlayerMoneyLog(pPlayerInfo->m_PID,nAddMoeny,"WDBWinLoss");
			AddUserGameMoney(pPlayerInfo->m_AID,pPlayerInfo->m_PID,nAddMoeny,false);

			pPlayerInfo->m_stGameInfo.m_nJF                += wl.m_JF;
			pPlayerInfo->m_stGameInfo.m_nEP                += wl.m_EP;
			if ( wl.m_EndGameFlag == TALLY_WIN )
			{
				pPlayerInfo->m_stGameInfo.m_nWinTimes++;
				
			}
			else
			{
				pPlayerInfo->m_stGameInfo.m_nLossTimes++;
			}
			//pPlayerInfo->m_stGameInfo.m_nUpperLimite       += nUpLimite;
			//pPlayerInfo->m_stGameInfo.m_nLowerLimite       += nLowLimite;

			//UpdatePlayerRight(pPlayerInfo);
		}
		else
		{
			DebugError("WDBWinLoss PID=%d",wl.m_PID);
		}		
	}

	RWDB_WinLossLog dbmsg_WL;
	dbmsg_WL.m_GameGUID       = wl.m_GameGUID;
	dbmsg_WL.m_AID            = wl.m_AID;
	dbmsg_WL.m_PID            = wl.m_PID;
	dbmsg_WL.m_RoomID         = wl.m_RoomID;
	dbmsg_WL.m_TableID        = wl.m_TableID;
	dbmsg_WL.m_SitID          = wl.m_SitID;
	dbmsg_WL.m_nServiceMoney  = int(wl.m_nServiceMoney);
	dbmsg_WL.m_nWinLossMoney  = wl.m_nWinLossMoney;
	dbmsg_WL.m_EndGameFlag    = wl.m_EndGameFlag;
	dbmsg_WL.m_LeftPai        = wl.m_LeftPai;
	dbmsg_WL.m_RightPai       = wl.m_RightPai;
	dbmsg_WL.m_PaiType        = wl.m_PaiType;
	m_RWDBMsgManager.PushRWDBMsg(dbmsg_WL);

	DebugInfo("WDBWinLoss End");
	return 0;
}

int CServer::OnWDBWinLoss( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBWinLoss");

	DBServerXY::WDB_WinLoss wl;
	TransplainMsg(msgPack,wl);

	DebugInfo("CServer::OnWDBWinLoss Start ServerID=%d AID=%d PID=%d Service=%s Money=%s",pSocket->GetServerID(),
		wl.m_AID,wl.m_PID,Tool::N2S(wl.m_nServiceMoney.get()).c_str(),Tool::N2S(wl.m_nWinLossMoney.get()).c_str() );

	if ( wl.m_PID > 0 )
	{
		DBServerXY::WDB_RespWinLoss respWL;
		respWL.m_PID = wl.m_PID;
		pSocket->SendMsg(respWL);
	}

	WDBWinLoss(wl);

	DebugInfo("CServer::OnWDBWinLoss End");
	return 0;
}

int CServer::OnWDBWinLossList( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBWinLossList");

	DBServerXY::WDB_WinLossList wll;
	TransplainMsg(msgPack,wll);

	int nStartTicket = GetTickCount();
	DebugInfo("CServer::OnWDBWinLossList Start ServerID=%d",pSocket->GetServerID() );

	DBServerXY::WDB_RespWinLossList respWL;
	for (int i=0;i<wll.m_nCount;i++)
	{
		respWL.m_vecPID.push_back(wll.m_WinLoss[i].m_PID);
	}
	pSocket->SendMsg(respWL);
	
	for ( int i=0;i<wll.m_nCount;i++)
	{
		DBServerXY::WDB_WinLoss& wl = wll.m_WinLoss[i];
		WDBWinLoss(wl);
	}

	int nEndTicket = GetTickCount() - nStartTicket;
	DebugInfo("CServer::OnWDBWinLossList End PlayerCount=%d UseTicket=%d",wll.m_nCount,nEndTicket);
	return 0;
}

int CServer::OnWDBMatchResult( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBMatchResult");

	DBServerXY::WDB_MatchResult msgMR;
	TransplainMsg(msgPack,msgMR);

	DebugInfo("CServer::OnWDBMatchResult Start ServerID=%d AID=%d PID=%d Money=%s",
		pSocket->GetServerID(),msgMR.m_AID,msgMR.m_PID,Tool::N2S(msgMR.m_Money.get()).c_str());

	DBServerXY::WDB_RespWinLoss respWL;
	respWL.m_PID = msgMR.m_PID;
	pSocket->SendMsg(respWL);

	if ( msgMR.m_PID>0 && msgMR.m_AID>0 )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgMR.m_PID);

		RWDB_MatchResultLog dbmsg_MRL;
		dbmsg_MRL.m_MatchGUID   = msgMR.m_MatchGUID;
		dbmsg_MRL.m_AID         = msgMR.m_AID;
		dbmsg_MRL.m_PID         = msgMR.m_PID;
		dbmsg_MRL.m_RoomID      = msgMR.m_RoomID;
		dbmsg_MRL.m_TableID     = msgMR.m_TableID;
		dbmsg_MRL.m_MatchID     = msgMR.m_MatchID;
		dbmsg_MRL.m_Ticket      = int(msgMR.m_Ticket);
		dbmsg_MRL.m_TakeMoney   = int(msgMR.m_TakeMoney);
		dbmsg_MRL.m_AwardMoney  = msgMR.m_AwardMoney;
		dbmsg_MRL.m_AwardJF     = msgMR.m_AwardJF;
		dbmsg_MRL.m_Position    = msgMR.m_Position;
		m_RWDBMsgManager.PushRWDBMsg(dbmsg_MRL);

		if ( msgMR.m_Ticket > 0 )
		{
			RWDB_IncomeAndPay dbmsg_IP;
			dbmsg_IP.m_AID          = msgMR.m_AID;
			dbmsg_IP.m_PID          = msgMR.m_PID;
			dbmsg_IP.m_nMoney       = -msgMR.m_Ticket;
			dbmsg_IP.m_Flag         = N_IncomeAndPay::MatchTicket;
			m_RWDBMsgManager.PushRWDBMsg(dbmsg_IP);
		}
		if ( msgMR.m_TakeMoney==0 && msgMR.m_AwardMoney>0 )
		{
			RWDB_IncomeAndPay dbmsg_IP;
			dbmsg_IP.m_AID          = msgMR.m_AID;
			dbmsg_IP.m_PID          = msgMR.m_PID;
			dbmsg_IP.m_nMoney       = msgMR.m_AwardMoney;
			dbmsg_IP.m_Flag         = N_IncomeAndPay::MatchAward;
			m_RWDBMsgManager.PushRWDBMsg(dbmsg_IP);
		}

		INT64 nUpLimite   = msgMR.m_Ticket*10;
		INT64 nLowLimite  = max(1,msgMR.m_Ticket/10);

		RWDB_WinLossInfo rwdbWLI;
		rwdbWLI.m_AID           = msgMR.m_AID;
		rwdbWLI.m_PID           = msgMR.m_PID;
		rwdbWLI.m_JF            = msgMR.m_JF;
		rwdbWLI.m_EP            = msgMR.m_EP;
		rwdbWLI.m_WinTimes      = 0;
		rwdbWLI.m_LossTimes     = 0;
		//rwdbWLI.m_nUpLimite     = nUpLimite;
		//rwdbWLI.m_nLowLimite    = nLowLimite;
		m_RWDBMsgManager.PushRWDBMsg(rwdbWLI);

		if ( m_DBConfig.m_QuickDB )
		{
			RWDB_AddGameMoney rwdbAGM;
			rwdbAGM.m_AID        = msgMR.m_AID;
			rwdbAGM.m_PID        = msgMR.m_PID;
			rwdbAGM.m_GameMoney  = msgMR.m_Money;;
			rwdbAGM.m_HongBao    = msgMR.m_AwardJF;
			m_RWDBMsgManager.PushRWDBMsg(rwdbAGM);
		}
		else
		{
			stWinLossMoney stWLM;
			stWLM.m_AID             = msgMR.m_AID;
			stWLM.m_PID             = msgMR.m_PID;
			stWLM.m_nGameMoney      = msgMR.m_Money;
			stWLM.m_nHongBao        = msgMR.m_AwardJF;
			int nDBRet = m_pDBOperator->UpdateWinLossMoney(stWLM);
			if ( pPlayerInfo && nDBRet!=DB_RESULT_SUCCESS )
			{
				DebugError("OnWDBMatchResult CurMoney=%s AddMoney=%s",
					N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(msgMR.m_Money.get()).c_str() );
				PrintPlayerMoneyLog(pPlayerInfo);
				CheckPlayerGameMoney(pPlayerInfo->m_PID);
			}
		}
		
		if ( pPlayerInfo )
		{
			AddPlayerMoneyLog(pPlayerInfo->m_PID,msgMR.m_Money,"OnWDBMatchResult");
			AddUserGameMoney(pPlayerInfo->m_AID,pPlayerInfo->m_PID,msgMR.m_Money,false);

			//pPlayerInfo->m_stGameInfo.m_nMatchJF           += msgMR.m_AwardJF;
			pPlayerInfo->m_stGameInfo.m_nEP                += msgMR.m_EP;
			pPlayerInfo->m_stGameInfo.m_nJF                += msgMR.m_JF;
			//pPlayerInfo->m_stGameInfo.m_nUpperLimite       += nUpLimite;
			//pPlayerInfo->m_stGameInfo.m_nLowerLimite       += nLowLimite;
		}
	}
	DebugInfo("CServer::OnWDBMatchResult End");
	return 0;
}

int CServer::OnWDBSendMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBSendMoney");

	DBServerXY::WDB_SendMoney msgSM;
	TransplainMsg(msgPack,msgSM);

	DebugInfo("CServer::OnWDBSendMoney Start ServerID=%d SendAID=%d SendPID=%d RecvAID=%d RecvPID=%d Service=%s Money=%s",
		pSocket->GetServerID(),msgSM.m_SendAID,msgSM.m_SendPID,msgSM.m_RecvAID,msgSM.m_RecvPID,
		Tool::N2S(msgSM.m_Service.get()).c_str(),Tool::N2S(msgSM.m_nSendMoney.get()).c_str() );

	if ( msgSM.m_SendPID>0 && msgSM.m_RecvPID>0 && msgSM.m_nSendMoney>0)
	{
		INT64 nSendUseMoney = msgSM.m_nSendMoney+msgSM.m_Service;

		RWDB_IncomeAndPay dbmsgInPay;
		dbmsgInPay.m_AID    = msgSM.m_SendAID;
		dbmsgInPay.m_PID    = msgSM.m_SendPID;
		dbmsgInPay.m_nMoney = -nSendUseMoney;
		dbmsgInPay.m_Flag   = N_IncomeAndPay::SendMoney;
		m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);

		//RWDB_UpdateUserGameMoneyType rwdbmsgUGMT;
		//rwdbmsgUGMT.m_AID           = msgSM.m_SendAID;
		//rwdbmsgUGMT.m_PID           = msgSM.m_SendPID;
		//rwdbmsgUGMT.m_nExpendMoney  = nSendUseMoney;
		//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);
		
		AddPlayerMoneyLog(msgSM.m_SendPID,-nSendUseMoney,"OnWDBSendMoney");
		AddUserGameMoney(msgSM.m_SendAID,msgSM.m_SendPID,-nSendUseMoney);

		dbmsgInPay.ReSet();
		dbmsgInPay.m_PID    = msgSM.m_RecvPID;
		dbmsgInPay.m_nMoney = msgSM.m_nSendMoney;
		dbmsgInPay.m_Flag   = N_IncomeAndPay::RecvMoney;
		MapDBPlayerInfo::iterator itorRecvPlayer = m_PlayerInfos.find(msgSM.m_RecvPID);
		if ( itorRecvPlayer != m_PlayerInfos.end() )
		{
			dbmsgInPay.m_AID = itorRecvPlayer->second->m_AID;
		}
		m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);

		//rwdbmsgUGMT.ReSet();
		//rwdbmsgUGMT.m_AID           = msgSM.m_RecvAID;
		//rwdbmsgUGMT.m_PID           = msgSM.m_RecvPID;
		//rwdbmsgUGMT.m_nIncomeMoney  = msgSM.m_nSendMoney;
		//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);
	
		AddPlayerMoneyLog(msgSM.m_RecvPID,msgSM.m_nSendMoney,"OnWDBRecvMoney");
		AddUserGameMoney(msgSM.m_RecvAID,msgSM.m_RecvPID,msgSM.m_nSendMoney);		
	}

	DebugInfo("CServer::OnWDBSendMoney Start ");

	return 0;
}

int CServer::OnWDBShowFace( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBShowFace");

	DBServerXY::WDB_ShowFace msgSF;
	TransplainMsg(msgPack,msgSF);

	DebugInfo("CServer::OnWDBShowFace Start ServerID=%d AID=%d PID=%d FaceID=%d PayMoney=%d",
		pSocket->GetServerID(),msgSF.m_AID,msgSF.m_PID,msgSF.m_FaceID,Tool::N2S(msgSF.m_PayMoney.get()).c_str() );

	if ( msgSF.m_PID )
	{
		RWDB_AddPlayerInfo rwdbAPI;
		rwdbAPI.m_AID      = msgSF.m_AID;
		rwdbAPI.m_PID      = msgSF.m_PID;
		rwdbAPI.m_Flag     = rwdbAPI.ShowFace;
		rwdbAPI.m_AddValue = 1;
		m_RWDBMsgManager.PushRWDBMsg(rwdbAPI);

		if ( msgSF.m_PayMoney > 0 )
		{
			RWDB_IncomeAndPay dbmsgInPay;
			dbmsgInPay.m_PID    = msgSF.m_PID;
			dbmsgInPay.m_AID    = msgSF.m_AID;
			dbmsgInPay.m_nMoney = -msgSF.m_PayMoney;
			dbmsgInPay.m_Flag   = N_IncomeAndPay::ShowFace;
			dbmsgInPay.m_strReMark = "FaceID=" + Tool::N2S(msgSF.m_FaceID);
			m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);

			//RWDB_UpdateUserGameMoneyType rwdbmsgUGMT;
			//rwdbmsgUGMT.m_AID           = msgSF.m_AID;
			//rwdbmsgUGMT.m_PID           = msgSF.m_PID;
			//rwdbmsgUGMT.m_nExpendMoney  = msgSF.m_PayMoney;
			//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);

			AddPlayerMoneyLog(msgSF.m_PID,-msgSF.m_PayMoney,"OnWDBShowFace");
			AddUserGameMoney(msgSF.m_AID,msgSF.m_PID,-msgSF.m_PayMoney);			
		}
	}

	DebugInfo("CServer::OnWDBShowFace End");
	return 0;
}

int CServer::OnWDBSendGift( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBSendGift");
	DBServerXY::WDB_SendGift msgSG;
	TransplainMsg(msgPack,msgSG);

	DebugInfo("CServer::OnWDBSendGift Start ServerID=%d AID=%d PID=%d PayMoney=%s",pSocket->GetServerID(),msgSG.m_AID,
		msgSG.m_PID,Tool::N2S(msgSG.m_PayMoney.get()).c_str() );

	MapGiftInfo::iterator itorGI = m_mapGiftInfo.find(msgSG.m_GiftID);
	if ( itorGI == m_mapGiftInfo.end() )
	{
		DebugError("OnWDBSendGift Can't Find Gift GiftID=%d",msgSG.m_GiftID);
		return 0;
	}

	if ( msgSG.m_PID && msgSG.m_GiftID && msgSG.m_Price && msgSG.m_PayMoney && msgSG.m_vecToPID.size() )
	{
		stGiftInfo stGI = itorGI->second;
		if ( msgSG.m_PriceFlag == N_Gift::PriceFlag_Chang || msgSG.m_PriceFlag == N_Gift::PriceFlag_Fixed )
		{
			//RWDB_UpdateUserGameMoneyType rwdbmsgUGMT;
			//rwdbmsgUGMT.m_AID           = msgSG.m_AID;
			//rwdbmsgUGMT.m_PID           = msgSG.m_PID;
			//rwdbmsgUGMT.m_nExpendMoney  = msgSG.m_PayMoney;
			//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);
			
			AddPlayerMoneyLog(msgSG.m_PID,-msgSG.m_PayMoney,"OnWDBSendGift");
			AddUserGameMoney(msgSG.m_AID,msgSG.m_PID,-msgSG.m_PayMoney);

			RWDB_IncomeAndPay dbmsgInPay;
			dbmsgInPay.m_PID        = msgSG.m_PID;
			dbmsgInPay.m_AID        = msgSG.m_AID;
			dbmsgInPay.m_nMoney     = -msgSG.m_PayMoney;
			dbmsgInPay.m_Flag       = N_IncomeAndPay::SendGift;
			m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);
		}
		else if ( msgSG.m_PriceFlag == N_Gift::PriceFlag_MoGui )
		{
			//m_pDBOperator->AddUserMoGuiMoney(msgSG.m_AID,msgSG.m_PID,int(-msgSG.m_PayMoney));
		}

		DBServerPlayerInfo* pSendPlayerInfo = GetDBPlayerInfo(msgSG.m_PID);
		string strSendNickName = "";
		if ( pSendPlayerInfo )
		{
			strSendNickName = pSendPlayerInfo->m_stUserDataInfo.m_NickName;
		}
		for ( int Idx=0;Idx<int(msgSG.m_vecToPID.size());Idx++)
		{
			unsigned RecvPID = msgSG.m_vecToPID[Idx];

			RWDB_WriteUserGift rwdbWUG;
			rwdbWUG.m_RecvPID       = RecvPID;
			rwdbWUG.m_GiftID        = msgSG.m_GiftID;
			rwdbWUG.m_Price         = msgSG.m_Price;
			if ( RecvPID == msgSG.m_PID )
				rwdbWUG.m_NickName  = "自已";
			else
				rwdbWUG.m_NickName  = strSendNickName;
			rwdbWUG.m_SendPID       = msgSG.m_PID;
			rwdbWUG.m_ActionTime    = m_CurTime;
			m_RWDBMsgManager.PushRWDBMsg(rwdbWUG);
		}		
	}
	else
	{
		DebugError("OnWDBSendGift Msg Error PID=%d GiftID=%d Price=%d PayMoney=%s PIDSize=%d",
			msgSG.m_PID,msgSG.m_GiftID,msgSG.m_Price,N2S(msgSG.m_PayMoney.get()).c_str(),msgSG.m_vecToPID.size() );
	}

	DebugInfo("CServer::OnWDBSendGift End");
	return 0;
}

int CServer::OnWDBSoldGift( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBSoldGift");

	DBServerXY::WDB_SoldGift msgDBSG;
	TransplainMsg(msgPack,msgDBSG);

	DebugInfo("CServer::OnWDBSoldGift Start ServerID=%d AID=%d PID=%d PayMoney=%s",pSocket->GetServerID(),
		msgDBSG.m_AID,msgDBSG.m_PID,Tool::N2S(msgDBSG.m_nMoney.get()).c_str() );

	if ( msgDBSG.m_PID && msgDBSG.m_vecGiftIdx.size() )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgDBSG.m_PID);
		if ( pPlayerInfo )
		{
			RWDB_IncomeAndPay dbmsgInPay;			
			dbmsgInPay.m_PID        = pPlayerInfo->m_PID;
			dbmsgInPay.m_AID        = pPlayerInfo->m_AID;
			dbmsgInPay.m_nMoney     = msgDBSG.m_nMoney;
			dbmsgInPay.m_Flag       = N_IncomeAndPay::SoldGift;
			m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);

			//RWDB_UpdateUserGameMoneyType rwdbmsgUGMT;
			//rwdbmsgUGMT.m_AID           = msgDBSG.m_AID;
			//rwdbmsgUGMT.m_PID           = msgDBSG.m_PID;
			//rwdbmsgUGMT.m_nIncomeMoney  = msgDBSG.m_nMoney;
			//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);			
			
			AddPlayerMoneyLog(msgDBSG.m_PID,msgDBSG.m_nMoney,"OnWDBSoldGift");
			AddUserGameMoney(msgDBSG.m_AID,msgDBSG.m_PID,msgDBSG.m_nMoney);			

			RWDB_UpdateGiftInfo dbmsgUGI;
			for (int Idx=0;Idx<int(msgDBSG.m_vecGiftIdx.size());Idx++)
			{
				pPlayerInfo->DeleteGift(msgDBSG.m_vecGiftIdx[Idx]);

				dbmsgUGI.ReSet();
				dbmsgUGI.m_GiftIdx = msgDBSG.m_vecGiftIdx[Idx];
				dbmsgUGI.m_Flag    = N_Gift::GiftST_Solded;
				dbmsgUGI.m_PID     = msgDBSG.m_PID;
				m_RWDBMsgManager.PushRWDBMsg(dbmsgUGI);
			}
		}
		else
		{
			DebugError("OnWDBSoldGift Cant't find PID=%d",msgDBSG.m_PID);
		}
	}
	else
	{
		DebugError("OnWDBSoldGift pid=%d giftidsize=%d",msgDBSG.m_PID,msgDBSG.m_vecGiftIdx.size());
	}

	DebugInfo("CServer::OnWDBSoldGift End");
	return 0;
}
int CServer::OnWDBChatMsg( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBChatMsg");

	DBServerXY::WDB_ChatMsg msgChat;
	TransplainMsg(msgPack,msgChat);

	RWDB_XY::RWDB_ChatLog msgCL;
	msgCL.m_ServerID         = pSocket->GetServerID();
	msgCL.m_RoomID           = msgChat.m_RoomID;
	msgCL.m_TableID          = msgChat.m_TableID;
	msgCL.m_SitID            = msgChat.m_SitID;
	msgCL.m_PID              = msgChat.m_PID;
	msgCL.m_ToPID            = msgChat.m_ToPID;
	msgCL.m_ChatMsg          = msgChat.m_ChatMsg;
	m_RWDBMsgManager.PushRWDBMsg(msgCL);

	return 0;
}
int CServer::OnWDBIncomeAndPay( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBIncomeAndPay");

	DBServerXY::WDB_IncomeAndPay msgIP;
	TransplainMsg(msgPack,msgIP);

	DebugInfo("CServer::OnWDBIncomeAndPay Start ServerID=%d AID=%d PID=%d Flag=%d Money=%s",pSocket->GetServerID(),
		msgIP.m_AID,msgIP.m_PID,msgIP.m_Flag,Tool::N2S(msgIP.m_nMoney.get()).c_str() );

	if ( msgIP.m_AID>0 && msgIP.m_PID>0 )
	{
		if ( msgIP.m_nMoney <= 0 || !N_IncomeAndPay::IsValidPayIncomeFlag(msgIP.m_Flag) )
		{
			DebugError("OnWDBIncomeAndPay PID=%d Flag=%d Money=%s",msgIP.m_PID,msgIP.m_Flag,N2S(msgIP.m_nMoney.get()).c_str());
		}

		if ( msgIP.m_Flag>N_IncomeAndPay::InPay_Start && msgIP.m_Flag<N_IncomeAndPay::InPay_Mid )
		{
			msgIP.m_nMoney = max(msgIP.m_nMoney,-msgIP.m_nMoney);
		}
		else if ( msgIP.m_Flag>N_IncomeAndPay::InPay_Mid && msgIP.m_Flag<N_IncomeAndPay::InPay_Max )
		{
			msgIP.m_nMoney = min(msgIP.m_nMoney,-msgIP.m_nMoney);
		}

		RWDB_IncomeAndPay dbmsgInPay;
		dbmsgInPay.m_AID       = msgIP.m_AID;
		dbmsgInPay.m_PID       = msgIP.m_PID;
		dbmsgInPay.m_nMoney    = msgIP.m_nMoney;
		dbmsgInPay.m_Flag      = msgIP.m_Flag;
		dbmsgInPay.m_strReMark = msgIP.m_strReMark;
		m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);

		AddPlayerMoneyLog(msgIP.m_PID,msgIP.m_nMoney,"OnWDBIncomeAndPay Flag="+N2S(msgIP.m_Flag));
		AddUserGameMoney(msgIP.m_AID,msgIP.m_PID,msgIP.m_nMoney);		
	}

	DebugInfo("CServer::OnWDBIncomeAndPay End");
	return 0;
}

int CServer::OnWDBCreateTable( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBCreateTable");

	DBServerXY::WDB_CreateTable msgCT;
	TransplainMsg(msgPack,msgCT);

	DebugInfo("CServer::OnWDBCreateTable Start ServerID=%d AID=%d PID=%d Money=%s",pSocket->GetServerID(),
		msgCT.m_AID,msgCT.m_FoundByWho,Tool::N2S(msgCT.m_nMoney.get()).c_str() );

	//RWDB_UpdateUserGameMoneyType rwdbmsgUGMT;
	//rwdbmsgUGMT.m_AID           = msgCT.m_AID;
	//rwdbmsgUGMT.m_PID           = msgCT.m_FoundByWho;
	//rwdbmsgUGMT.m_nExpendMoney  = msgCT.m_nMoney;
	//m_RWDBMsgManager.PushRWDBMsg(rwdbmsgUGMT);

	RWDB_IncomeAndPay dbmsgInPay;
	dbmsgInPay.m_AID           = msgCT.m_AID;
	dbmsgInPay.m_PID           = msgCT.m_FoundByWho;
	dbmsgInPay.m_nMoney        = -msgCT.m_nMoney;
	dbmsgInPay.m_Flag          = N_IncomeAndPay::CreateTable;
	m_RWDBMsgManager.PushRWDBMsg(dbmsgInPay);

	RWDB_TableInfo dbmsgTI;	
	dbmsgTI.m_FoundByWho       = msgCT.m_FoundByWho;
	dbmsgTI.m_ServerID         = msgCT.m_ServerID;
	dbmsgTI.m_RoomID           = msgCT.m_RoomID;
	dbmsgTI.m_TableID          = msgCT.m_TableID;

	dbmsgTI.m_TableName        = msgCT.m_TableName;
	dbmsgTI.m_Password         = msgCT.m_Password;
	dbmsgTI.m_TableRule        = msgCT.m_TableRule;
	dbmsgTI.m_TableRuleEX      = msgCT.m_TableRuleEX;
	dbmsgTI.m_TableMatchRule   = msgCT.m_TableMatchRule;
	dbmsgTI.m_EndTime          = msgCT.m_EndTime;
	m_RWDBMsgManager.PushRWDBMsg(dbmsgTI);
	
	AddPlayerMoneyLog(msgCT.m_FoundByWho,-msgCT.m_nMoney,"OnWDBCreateTable");
	AddUserGameMoney(msgCT.m_AID,msgCT.m_FoundByWho,-msgCT.m_nMoney);	

	DebugInfo("CServer::OnWDBCreateTable End");
	return 0;
}
int CServer::OnReqPlayerAward( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnReqPlayerAward");

	DBServerXY::DBS_ReqPlayerAward msgPA;
	TransplainMsg(msgPack,msgPA);

	DebugInfo("CServer::OnReqPlayerAward Start ServerID=%d AID=%d PID=%d",pSocket->GetServerID(),msgPA.m_AID,msgPA.m_PID);

	//SendPlayerAward(msgPA.m_AID,msgPA.m_PID);

	DebugInfo("CServer::OnReqPlayerAward End");
	return 0;
}
int CServer::OnWDBChangeName( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBChangeName");

	DBServerXY::WDB_ChangeUserInfo msgCN;
	TransplainMsg(msgPack,msgCN);

	DebugInfo("CServer::OnWDBChangeName Start ServerID=%d AID=%d PID=%d Sex=%d Name=%s HeadURL=%s City=%s",
		pSocket->GetServerID(),msgCN.m_AID,msgCN.m_PID,msgCN.m_Sex,
		msgCN.m_NickName.c_str(),msgCN.m_HeadPicUrl.c_str(),msgCN.m_City.c_str() );

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgCN.m_PID);
	if ( pPlayerInfo/* && pPlayerInfo->m_stUserDataInfo.m_ChangeName > 0 */)
	{
		//pPlayerInfo->m_stUserDataInfo.m_ChangeName    = 0;

		pPlayerInfo->m_stUserDataInfo.m_Sex           = msgCN.m_Sex;
		pPlayerInfo->m_stUserDataInfo.m_NickName      = msgCN.m_NickName;
		pPlayerInfo->m_stUserDataInfo.m_HeadPicURL    = msgCN.m_HeadPicUrl;
		pPlayerInfo->m_stUserDataInfo.m_City          = msgCN.m_City;

		MemcatchXY::DeZhou::memUserDataInfo memUDI;
		if ( m_memOperator.memGetUserDataInfo(msgCN.m_AID,msgCN.m_PID,memUDI) )
		{
			//memUDI.m_ChangeName        = 0;

			memUDI.m_Sex               = msgCN.m_Sex;
			memUDI.m_NickName          = msgCN.m_NickName;
			memUDI.m_HeadPicURL        = msgCN.m_HeadPicUrl;
			memUDI.m_City              = msgCN.m_City;

			m_memOperator.memSetUserDataInfo(memUDI);
		}

		RWDB_ChangeUserInfo msgrwdbCN;
		msgrwdbCN.m_AID = msgCN.m_AID;
		msgrwdbCN.m_PID = msgCN.m_PID;
		msgrwdbCN.m_Sex = msgCN.m_Sex;
		msgrwdbCN.m_NickName = msgCN.m_NickName;
		msgrwdbCN.m_HeadPicUrl = msgCN.m_HeadPicUrl;
		msgrwdbCN.m_City = msgCN.m_City;
		m_RWDBMsgManager.PushRWDBMsg(msgrwdbCN);
	}

	DebugInfo("CServer::OnWDBChangeName End");

	return 0;
}

int CServer::OnWDBReportPlayerOnLine( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBReportPlayerOnLine");

	DBServerXY::WDB_ReportPlayerOnline msgRPOL;
	TransplainMsg(msgPack,msgRPOL);

	DebugInfo("CServer::OnWDBReportPlayerOnLine Start ServerID=%d",pSocket->GetServerID());

	RWDB_ReportPlayerOnline msgrwdbRPOL;
	msgrwdbRPOL.m_ServerID   = msgRPOL.m_ServerID;
	msgrwdbRPOL.m_AID        = msgRPOL.m_AID;
	msgrwdbRPOL.m_Total      = msgRPOL.m_Total;
	msgrwdbRPOL.m_Lobby      = msgRPOL.m_Lobby;
	msgrwdbRPOL.m_Table      = msgRPOL.m_Table;
	msgrwdbRPOL.m_Game       = msgRPOL.m_Game;
	msgrwdbRPOL.m_Match      = msgRPOL.m_Match;

	m_RWDBMsgManager.PushRWDBMsg(msgrwdbRPOL);

	DebugInfo("CServer::OnWDBReportPlayerOnLine End");

	return 0;
}
int CServer::OnWDBMaxPai( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBMaxPai");

	DBServerXY::WDB_MaxPai msgMP ;
	TransplainMsg(msgPack,msgMP);

	DebugInfo("CServer::OnWDBMaxPai Start ServerID=%d,AID=%d PID=%d MaxPai=%s",pSocket->GetServerID(),
		msgMP.m_AID,msgMP.m_PID,msgMP.m_MaxPai.c_str() );

	RWDB_MaxPai rwdbMP;
	rwdbMP.m_AID          = msgMP.m_AID;
	rwdbMP.m_PID          = msgMP.m_PID;
	rwdbMP.m_MaxPaiTime   = msgMP.m_MaxPaiTime;
	rwdbMP.m_MaxPai       = msgMP.m_MaxPai;
	m_RWDBMsgManager.PushRWDBMsg(rwdbMP);

	PDBServerPlayerInfo pPlayerInfo = GetDBPlayerInfo(msgMP.m_PID);
	if ( pPlayerInfo )
	{
		pPlayerInfo->m_stGameInfo.m_MaxPai     = msgMP.m_MaxPai;
		pPlayerInfo->m_stGameInfo.m_MaxPaiTime = msgMP.m_MaxPaiTime;
	}

	DebugInfo("CServer::OnWDBMaxPai End");

	return 0;
}

int CServer::OnWDBMaxWin( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBMaxWin");
	DBServerXY::WDB_MaxWin msgMW;
	TransplainMsg(msgPack,msgMW);

	DebugInfo("CServer::OnWDBMaxWin Start ServerID=%d,AID=%d PID=%d MaxWin=%s",pSocket->GetServerID(),
		msgMW.m_AID,msgMW.m_PID,Tool::N2S(msgMW.m_MaxWin).c_str() );

	RWDB_MaxWin rwdbMW;
	rwdbMW.m_AID          = msgMW.m_AID;
	rwdbMW.m_PID          = msgMW.m_PID;
	rwdbMW.m_MaxWinTime   = msgMW.m_MaxWinTime;
	rwdbMW.m_MaxWin       = msgMW.m_MaxWin;
	m_RWDBMsgManager.PushRWDBMsg(rwdbMW);

	PDBServerPlayerInfo pPlayerInfo = GetDBPlayerInfo(msgMW.m_PID);
	if ( pPlayerInfo )
	{
		//pPlayerInfo->m_stGameInfoEX.m_MaxWin     = msgMW.m_MaxWin;
		//pPlayerInfo->m_stGameInfoEX.m_MaxWinTime = msgMW.m_MaxWinTime;
	}

	DebugInfo("CServer::OnWDBMaxWin End");

	return 0;
}
int CServer::OnWDBMaxMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBMaxMoney");
	DBServerXY::WDB_MaxMoney msgMM;
	TransplainMsg(msgPack,msgMM);

	DebugInfo("CServer::OnWDBMaxMoney Start ServerID=%d,AID=%d PID=%d MaxMoney=%s",pSocket->GetServerID(),
		msgMM.m_AID,msgMM.m_PID,Tool::N2S(msgMM.m_MaxMoney).c_str() );

	RWDB_MaxMoney rwdbMM;
	rwdbMM.m_AID          = msgMM.m_AID;
	rwdbMM.m_PID          = msgMM.m_PID;
	rwdbMM.m_MaxMoneyTime = msgMM.m_MaxMoneyTime;
	rwdbMM.m_MaxMoney     = msgMM.m_MaxMoney;
	m_RWDBMsgManager.PushRWDBMsg(rwdbMM);

	PDBServerPlayerInfo pPlayerInfo = GetDBPlayerInfo(msgMM.m_PID);
	if ( pPlayerInfo )
	{
		//pPlayerInfo->m_stGameInfoEX.m_MaxMoney     = msgMM.m_MaxMoney;
		//pPlayerInfo->m_stGameInfoEX.m_MaxMoneyTime = msgMM.m_MaxMoneyTime;
	}

	DebugInfo("CServer::OnWDBMaxMoney End");

	return 0;
}

int CServer::WDBMatchWinLoss(DBServerXY::WDB_MatchWinLoss& msgMWL)
{
	CLogFuncTime lft(m_DBSFuncTime,"WDBMatchWinLoss");
	DebugInfo("WDBMatchWinLoss Start GUID=%s ",N2S(msgMWL.m_MatchGUID).c_str());

	RWDB_MatchWinLoss rwdbMWL;
	rwdbMWL.m_MatchGUID          = msgMWL.m_MatchGUID;
	rwdbMWL.m_GameGUID           = msgMWL.m_GameGUID;
	rwdbMWL.m_AID                = msgMWL.m_AID;
	rwdbMWL.m_PID                = msgMWL.m_PID;
	rwdbMWL.m_RoomID             = msgMWL.m_RoomID;
	rwdbMWL.m_TableID            = msgMWL.m_TableID;
	rwdbMWL.m_SitID              = msgMWL.m_SitID;
	rwdbMWL.m_nWinLossMoney      = int(msgMWL.m_nWinLossMoney);
	rwdbMWL.m_EndGameFlag        = msgMWL.m_EndGameFlag;
	rwdbMWL.m_PaiType            = msgMWL.m_PaiType;
	m_RWDBMsgManager.PushRWDBMsg(rwdbMWL);

	if ( msgMWL.m_PID )
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgMWL.m_PID);
		assert(pPlayerInfo);
		if ( pPlayerInfo )
		{
			pPlayerInfo->SetActiveTime(m_CurTime);

			RWDB_AddPlayerInfo rwdbAPI;
			rwdbAPI.m_AID        = pPlayerInfo->m_AID;
			rwdbAPI.m_PID        = pPlayerInfo->m_PID;
			rwdbAPI.m_AddValue   = 1;
			if ( msgMWL.m_nWinLossMoney > 0 )
			{
				rwdbAPI.m_Flag = rwdbAPI.WinTimes;
				pPlayerInfo->m_stGameInfo.m_nWinTimes++;
			}
			else
			{
				rwdbAPI.m_Flag  = rwdbAPI.LossTimes;
				pPlayerInfo->m_stGameInfo.m_nLossTimes++;
			}
			m_RWDBMsgManager.PushRWDBMsg(rwdbAPI);

			//UpdatePlayerRight(pPlayerInfo);
		}
	}
	DebugInfo("WDBMatchWinLoss End");

	return 0;
}

int CServer::OnWDBMatchWinLoss( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	DBServerXY::WDB_MatchWinLoss msgMWL;
	TransplainMsg(msgPack,msgMWL);

	DebugInfo("CServer::OnWDBMatchWinLoss Start ServerID=%d AID=%d PID=%d Money=%s",pSocket->GetServerID(),
		msgMWL.m_AID,msgMWL.m_PID,Tool::N2S(msgMWL.m_nWinLossMoney.get()).c_str() );

	WDBMatchWinLoss(msgMWL);

	DebugInfo("CServer::OnWDBMatchWinLoss End");

	return 0;
}

int CServer::OnWDBCheckGameInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBCheckGameInfo");

	DBServerXY::WDB_ReqCheckGameInfo msgCGI;
	TransplainMsg(msgPack,msgCGI);

	DebugInfo("OnWDBCheckGameInfo Start AID=%d PID=%d",msgCGI.m_AID,msgCGI.m_PID);

	DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(msgCGI.m_PID);
	if ( pPlayerInfo )
	{
		if ( msgCGI.m_nGameTime > pPlayerInfo->m_stGameInfo.m_nGameTime )
		{
			RWDB_AddPlayerInfo rwdbAPI;
			rwdbAPI.m_AID       = pPlayerInfo->m_AID;
			rwdbAPI.m_PID       = pPlayerInfo->m_PID;
			rwdbAPI.m_Flag       = rwdbAPI.GameTime;
			rwdbAPI.m_AddValue   = msgCGI.m_nGameTime - pPlayerInfo->m_stGameInfo.m_nGameTime;
			m_RWDBMsgManager.PushRWDBMsg(rwdbAPI);
			
			pPlayerInfo->m_stGameInfo.m_nGameTime        = INT32(msgCGI.m_nGameTime);
		}

		//if ( pPlayerInfo->m_stGameInfo.m_nMoGuiMoney != msgCGI.m_nMoGuiMoney )
		//{
		//	DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBMogui=%d GSMogui=%d",msgCGI.m_AID,msgCGI.m_PID,
		//		pPlayerInfo->m_stGameInfo.m_nMoGuiMoney,INT32(msgCGI.m_nMoGuiMoney) );
		//}
		if ( pPlayerInfo->m_stGameInfo.m_nGameMoney != msgCGI.m_nGameMoney )
		{
			DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBGameMoney=%s GSGameMoney=%s",msgCGI.m_AID,msgCGI.m_PID,
				N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(msgCGI.m_nGameMoney.get()).c_str() );
		}
		//if ( pPlayerInfo->m_stGameInfo.m_nMatchJF != msgCGI.m_nMatchJF )
		//{
		//	DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBMatchJF=%s GSMatchJF=%s",msgCGI.m_AID,msgCGI.m_PID,
		//		N2S(pPlayerInfo->m_stGameInfo.m_nMatchJF).c_str(),N2S(msgCGI.m_nMatchJF.get()).c_str() );
		//}
		if ( pPlayerInfo->m_stGameInfo.m_nJF != msgCGI.m_nJF )
		{
			DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBJF=%s GSJF=%s",msgCGI.m_AID,msgCGI.m_PID,
				N2S(pPlayerInfo->m_stGameInfo.m_nJF).c_str(),N2S(msgCGI.m_nJF.get()).c_str() );
		}
		if ( pPlayerInfo->m_stGameInfo.m_nEP != msgCGI.m_nEP )
		{
			DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBEP=%s GSEP=%s",msgCGI.m_AID,msgCGI.m_PID,
				N2S(pPlayerInfo->m_stGameInfo.m_nEP).c_str(),N2S(msgCGI.m_nEP.get()).c_str() );
		}
		//if ( pPlayerInfo->m_stGameInfo.m_nUpperLimite != msgCGI.m_nUpperLimite )
		//{
		//	DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBUplimite=%s GSUplimite=%s",msgCGI.m_AID,msgCGI.m_PID,
		//		N2S(pPlayerInfo->m_stGameInfo.m_nUpperLimite).c_str(),N2S(msgCGI.m_nUpperLimite.get()).c_str() );
		//}
		//if ( pPlayerInfo->m_stGameInfo.m_nLowerLimite != msgCGI.m_nLowerLimite )
		//{
		//	DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBLowlimite=%s GSLowlimite=%s",msgCGI.m_AID,msgCGI.m_PID,
		//		N2S(pPlayerInfo->m_stGameInfo.m_nLowerLimite).c_str(),N2S(msgCGI.m_nLowerLimite.get()).c_str() );
		//}

		if ( pPlayerInfo->m_PlayerType != PLAYER_TYPE_BOT )
		{
			if ( pPlayerInfo->m_stGameInfo.m_nWinTimes != INT32(msgCGI.m_nWinTimes) )
			{
				DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBWinTime=%d GSWinTime=%d",msgCGI.m_AID,msgCGI.m_PID,
					pPlayerInfo->m_stGameInfo.m_nWinTimes,INT32(msgCGI.m_nWinTimes) );
			}
			if ( pPlayerInfo->m_stGameInfo.m_nLossTimes != INT32(msgCGI.m_nLossTimes) )
			{
				DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBLossTime=%d GSLossTime=%d",msgCGI.m_AID,msgCGI.m_PID,
					pPlayerInfo->m_stGameInfo.m_nLossTimes,INT32(msgCGI.m_nLossTimes) );
			}
		}		

		//if ( pPlayerInfo->m_RightInfo.GetRightLevel() != PLAYER_RIGHT_COMMON )
		//{
		//	if ( (pPlayerInfo->m_RightInfo.GetRightLevel() != msgCGI.m_RightLevel)
		//		|| (pPlayerInfo->m_RightInfo.GetRightTimes() != msgCGI.m_RightTimes) )
		//	{
		//		DebugError("OnWDBCheckGameInfo AID=%d PID=%d DBRightLevel=%d DBRightTimes=%d GSRightLevel=%d GSRightTimes=%d",
		//			msgCGI.m_AID,msgCGI.m_PID,pPlayerInfo->m_RightInfo.GetRightLevel(),
		//			pPlayerInfo->m_RightInfo.GetRightTimes(),msgCGI.m_RightLevel,msgCGI.m_RightTimes );
		//	}
		//}

		if ( N_CeShiLog::c_MoneyLog )
		{
			//if ( m_DBConfig.m_QuickDB==0 || (m_DBConfig.m_QuickDB && pPlayerInfo->m_stGameInfo.m_nMoGuiMoney!=msgCGI.m_nMoGuiMoney) )
			//{
			//	PrintPlayerMoneyLog(pPlayerInfo);

			//	stDBGameMoney stGM;
			//	if ( m_pDBOperator->ReadUserGameMoney(pPlayerInfo->m_AID,pPlayerInfo->m_PID,stGM) == DB_RESULT_SUCCESS )
			//	{
			//		if ( stGM.m_GameMoney != pPlayerInfo->m_stGameInfo.m_nGameMoney )
			//		{
			//			DebugError("AID=%d PID=%d PlayerMoney=%s DBMoney=%s",pPlayerInfo->m_AID,pPlayerInfo->m_PID,
			//				N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(stGM.m_GameMoney).c_str() );
			//		}
			//		else
			//		{
			//			DebugLog("游戏币相等 AID=%d PID=%d PlayerMoney=%s DBMoney=%s",pPlayerInfo->m_AID,pPlayerInfo->m_PID,
			//				N2S(pPlayerInfo->m_stGameInfo.m_nGameMoney).c_str(),N2S(stGM.m_GameMoney).c_str() );
			//		}
			//	}
			//}
		}

		pPlayerInfo->ClearMoneyLog();
		CheckPlayerGameMoney(pPlayerInfo->m_PID);
	}
	else
	{
		DebugError("OnWDBCheckGameInfo AID=%d PID=%d",msgCGI.m_AID,msgCGI.m_PID);
	}
	DebugInfo("OnWDBCheckGameInfo End");

	return 0;
}
int CServer::OnWDBPlayerAward( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBPlayerAward");

	DBServerXY::WDB_PlayerAward msgPA;
	TransplainMsg(msgPack,msgPA);

	RWDB_AddPlayerAward msgAPA;
	msgAPA.m_AID      = msgPA.m_AID;
	msgAPA.m_PID      = msgPA.m_PID;
	msgAPA.m_nFlag    = msgPA.m_nFlag;
	msgAPA.m_PayMode  = msgPA.m_PayMode;
	msgAPA.m_Money    = msgPA.m_nMoney;
	msgAPA.m_CanUse   = 1;
	msgAPA.m_EndTime  = msgPA.m_EndTime;
	m_RWDBMsgManager.PushRWDBMsg(msgAPA);

	return 0;
}
int CServer::OnWDBPlayerActionLog( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBPlayerActionLog");

	DBServerXY::WDB_PlayerActionLog msgPAL;
	TransplainMsg(msgPack,msgPAL);

	RWDB_PlayerActionLog rwdbMsgPAL;
	rwdbMsgPAL.m_AID               = msgPAL.m_AID;
	rwdbMsgPAL.m_PID               = msgPAL.m_PID;
	rwdbMsgPAL.m_OneLifeTotalTime  = msgPAL.m_OneLifeTotalTime;
	rwdbMsgPAL.m_OneLifeGameTime   = msgPAL.m_OneLifeGameTime;
	rwdbMsgPAL.m_OneLifeMatchTime  = msgPAL.m_OneLifeMatchTime;
	rwdbMsgPAL.m_EnterRoomTimes    = msgPAL.m_EnterRoomTimes;
	rwdbMsgPAL.m_EnterTableTimes   = msgPAL.m_EnterTableTimes;
	rwdbMsgPAL.m_SitDownTimes      = msgPAL.m_SitDownTimes;
	rwdbMsgPAL.m_PlayTimes         = msgPAL.m_PlayTimes;
	m_RWDBMsgManager.PushRWDBMsg(rwdbMsgPAL);

	return 0;
}
int CServer::OnWDBPlayerClientError( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBPlayerClientError");

	DBServerXY::WDB_PlayerClientError msgPCE;
	TransplainMsg(msgPack,msgPCE);

	RWDB_PlayerClientError rwdbMsgPCE;
	rwdbMsgPCE.m_AID     = msgPCE.m_AID;
	rwdbMsgPCE.m_PID     = msgPCE.m_PID;
	rwdbMsgPCE.m_IP      = msgPCE.m_strIP;
	rwdbMsgPCE.m_Flag    = msgPCE.m_Flag;

	m_RWDBMsgManager.PushRWDBMsg(rwdbMsgPCE);

	return 0;
}
int CServer::OnGSToDBSFlag( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnGSToDBSFlag");
	DebugInfo("OnGSToDBSFlag Start");

	DBServerXY::GS_To_DBS_Flag msgGSF;
	TransplainMsg(msgPack,msgGSF);

	switch(msgGSF.m_Flag)
	{
	case DBServerXY::GS_To_DBS_Flag::AddContinuPlay:
		{
			OnAddContinuPlay(msgGSF.m_AID,msgGSF.m_PID);
		}
		break;
	case DBServerXY::GS_To_DBS_Flag::CheckClientExist:
		{
			PDBServerPlayerInfo pPlayerInfo = GetDBPlayerInfo(msgGSF.m_PID);
			if ( pPlayerInfo && pPlayerInfo->m_vectorUserProduct.size() )
			{
				for ( size_t nPos=0;nPos<pPlayerInfo->m_vectorUserProduct.size();++nPos )
				{
					stUserProduct& rUP = pPlayerInfo->m_vectorUserProduct[nPos];
					DoUserProduct(rUP);
					rUP.Init();
				}
				pPlayerInfo->m_vectorUserProduct.clear();
			}
		}
		break;
	case DBServerXY::GS_To_DBS_Flag::AwardPlayerInvite:
		{
			PDBServerPlayerInfo pPlayerInfo = GetDBPlayerInfo(msgGSF.m_PID);
			if ( pPlayerInfo )
			{
				RWDB_AddPlayerAward msgAPA;
				msgAPA.m_AID      = pPlayerInfo->m_stUserDataInfo.m_InvitePID;
				msgAPA.m_PID      = pPlayerInfo->m_stUserDataInfo.m_InvitePID;
				msgAPA.m_nFlag    = N_IncomeAndPay::FenHongAward;
				msgAPA.m_PayMode  = PM_OneTime;
				msgAPA.m_Money    = 100;
				msgAPA.m_CanUse   = 1;
				msgAPA.m_EndTime  = UINT32(time(NULL) + time_t(N_Time::Week_1));
				m_RWDBMsgManager.PushRWDBMsg(msgAPA);
			}
		}
		break;
	default:
		DebugError("OnGSToDBSFlag AID=%d PID=%d Flag=%d",msgGSF.m_AID,msgGSF.m_PID,msgGSF.m_Flag);
		break;
	}

	DebugInfo("OnGSToDBSFlag End");

	return 0;
}
int CServer::OnXieYiList( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnXieYiList");
	DebugInfo("CServer::OnXieYiList Start ServerID=%d,XYID=%d XYLen=%d",pSocket->GetServerID(),msgPack.m_XYID,msgPack.m_nLen );

	bistream bis;
	bis.attach(msgPack.m_DataBuf,msgPack.m_nLen);

	try
	{
		short XieYiID;
		bis >> XieYiID;

		switch( XieYiID )
		{
		case DBServerXY::WDB_MatchWinLoss::XY_ID:
			{
				vector<DBServerXY::WDB_MatchWinLoss> vectorMWL;
				OutVector(bis,vectorMWL,250);
				assert(bis.avail() == 0);			
				for ( size_t nPos=0;nPos<vectorMWL.size();++nPos)
				{
					WDBMatchWinLoss(vectorMWL[nPos]);
				}
			}
			break;
		default:
			DebugError("OnXieYiList XieYiID=%d",XieYiID);
		}
	}
	catch (...)
	{
		DebugError("OnXieYiList");
	}

	DebugInfo("CServer::OnXieYiList End");

	return 0;
}
int CServer::OnAddContinuPlay(INT16 AID,UINT32 PID)
{
	PDBServerPlayerInfo pPlayerInfo = GetDBPlayerInfo(PID);
	if ( pPlayerInfo )
	{
		pPlayerInfo->m_stLoginInfo.m_ContinuePlay++;
		MemcatchXY::DeZhou::memUserLandRecord memULR;
		if ( m_memOperator.GetUserLand(AID,PID,memULR) )
		{
			memULR.m_ContinuePlay = pPlayerInfo->m_stLoginInfo.m_ContinuePlay++;
		}
		else
		{
			memULR.m_AID              = AID;
			memULR.m_PID              = PID;
			memULR.m_FirstTodayTime   = m_CurTime;
			memULR.m_ContinueLogin    = pPlayerInfo->m_stLoginInfo.m_ContinueLogin;
			memULR.m_ContinuePlay     = pPlayerInfo->m_stLoginInfo.m_ContinuePlay;
			m_memOperator.SetUserLand(memULR);
		}
	}
	return 0;
}

int CServer::OnWDBUserFriend( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack )
{
	CLogFuncTime lft(m_DBSFuncTime,"OnWDBUserFriend");

	DBServerXY::WDB_UserFriend uf;
	TransplainMsg(msgPack,uf);

	DebugInfo("CServer::OnWDBUserFriend Start ServerID=%d,LeftPID=%d RightPID=%d Flag=%d",
		pSocket->GetServerID(),uf.m_LeftPID,uf.m_RightPID,uf.m_Flag );

	RWDB_MakeFriend dbmsgMF;
	if ( uf.m_LeftPID > 0 && uf.m_RightPID > 0 )
	{
		DBServerPlayerInfo* pLeftPlayerInfo = GetDBPlayerInfo(uf.m_LeftPID);
		DBServerPlayerInfo* pRightPlayerInfo = GetDBPlayerInfo(uf.m_RightPID);		
		if ( pLeftPlayerInfo )
		{
			pLeftPlayerInfo->SetActiveTime(m_CurTime);
		}

		dbmsgMF.ReSet();
		dbmsgMF.m_LeftPID   = uf.m_LeftPID;
		dbmsgMF.m_RightPID  = uf.m_RightPID;
		dbmsgMF.m_Flag      = uf.m_Flag;
		m_RWDBMsgManager.PushRWDBMsg(dbmsgMF);

		dbmsgMF.ReSet();
		dbmsgMF.m_LeftPID   = uf.m_RightPID;
		dbmsgMF.m_RightPID  = uf.m_LeftPID;
		dbmsgMF.m_Flag      = N_Friend::GetAnti(uf.m_Flag);
		m_RWDBMsgManager.PushRWDBMsg(dbmsgMF);

		if ( pLeftPlayerInfo )
		{
			pLeftPlayerInfo->m_FriendCount++;
			pLeftPlayerInfo->UpdateFriendFlag(uf.m_RightPID,uf.m_Flag);
		}
		if ( pRightPlayerInfo )
		{
			pRightPlayerInfo->m_FriendCount++;
			pRightPlayerInfo->UpdateFriendFlag(uf.m_LeftPID,N_Friend::GetAnti(uf.m_Flag));
		}
	}

	DebugInfo("CServer::OnWDBUserFriend End");

	return 0;
}