#include "./server.h"

#include <stdio.h>
#include <stdarg.h>
#include <Psapi.h>

#include "GameServerSocket.h"
#include "gameroom.h"
#include "GameTable.h"
#include "GameDBSocket.h"
#include "Player.h"
#include "DeZhouGameLogic.h"

#include "publicdata.h"

#pragma comment(lib, "psapi.lib")

int CDBConnectThread::Run( void )
{
	assert(m_pDBSocket);
	while ( IsRunning() )
	{
		if ( m_pDBSocket->GetAutoConnect() && (!m_pDBSocket->IsConnected()) && m_pDBSocket->StartConnect() )
		{
			printf_s("DBSocket Connect Succeed.\n");
		}
		Sleep(50);
	}
	return 0;
}

CServer::CServer()
{
	Tool::CRandom::InitRandSeed();                             //这个必不可少，在后面很多地方会用到rand();

	m_bIsInitOK = false;
	m_GSConf.Init();
	InitGameRule(m_GSConf);
	InitCeShiLog();

	m_pGSConfig = &m_GSConf;

	InitLogger( "dz_gs_log", true, LOGLEVEL_ALL );
	DebugLogOut("CServer::Server Init..");
	
	m_pPool = CreateConnectPool();
	m_pPool->SetCallback( this );
	m_pPool->Start( m_pGSConfig->m_cfServer.m_gsPort, m_pGSConfig->m_cfServer.m_gsConnectCount,m_pGSConfig->m_cfServer.m_gsOutCount);

	m_memOperator.Init(m_pGSConfig->m_cfServer.m_MemcachIpPort);

	m_Rooms.clear();
	m_Clients.clear();
	m_Players.clear();
	m_LoginSockets.clear();
	m_RoomInfoVector.clear();
	m_TableInfoVector.clear();

	m_BotPlayerNumber = 0;

	m_CurTime = static_cast<UINT32>(time(NULL));
	
	m_PlayerOnGame = 0;
	m_CheckPlayerActiveTime = m_CurTime;
	m_CheckRoomTime = m_CurTime;
	m_CheckPlayerLockTime = m_CurTime;
	m_CheckPlayerOnLine = m_CurTime;
	m_CheckAddFriendTime = m_CurTime;
	m_CheckXieYiInOutTime = m_CurTime;
	m_CheckXieYiSpeedTime = m_CurTime;
	m_CheckCityPlayerTime = m_CurTime;
	m_SendPlayerOnLineTime = m_CurTime;
	m_CheckFirendPlayerTime = m_CurTime;
	m_DBServerInUseTime = m_CurTime;
	m_CheckBotPlayerTime = m_CurTime;

	m_GetBotStep1 = 0;
	m_GetBotStep2 = 0;
	m_GetBotFailed = 0;

	m_nAcceptCount = 0;
	m_nCloseCount = 0;

	m_NewDayTime = UINT32(Tool::GetNewDayTime(m_CurTime));
	m_OneMinuteTime = m_CurTime + 60;
	m_TenMinuteTime = m_CurTime + 600;
	m_HourTime      = m_CurTime + 3600;

	m_bOutOfDBServer = false;
	m_bInitDBInfo = false;

	for (int i=0;i<31;i++)
	{
		if ( S_Uint32_Number[i] != S_Uint32_Number[i+1]/2)
		{
			DebugError("S_Uint32_Number i=%d",i);
		}
	}

	m_bIsInitOK = true;
	if ( !m_bIsInitOK ) DebugError("Server InitOk...");
	else DebugLogOut("Server InitOK...");

	DebugLogOut("CServer::Server Init End");
}

CServer::~CServer( void )
{
	DebugLogOut("CServer:: Destroy ...");

	MapClientSocket::iterator itorClient;
	for(itorClient=m_Clients.begin();itorClient!=m_Clients.end();++itorClient)
	{
		if( itorClient->first )
		{
			itorClient->first->Close();
		}
		if( itorClient->second )
		{
			delete itorClient->second;
		}		
	}
	m_Clients.clear();

	for(itorClient=m_LoginSockets.begin();itorClient!=m_LoginSockets.end();++itorClient)
	{
		if( itorClient->first )
		{
			itorClient->first->Close();
		}
		if( itorClient->second )
		{
			delete itorClient->second;
		}
	}
	m_LoginSockets.clear();	

	m_Players.clear();
	m_BotPlayerInIdle.clear();
	m_BotPlayerInUse.clear();
	CPlayer::ClearAllPlayers();

	MapRoom::iterator itorRoom;
	for(itorRoom=m_Rooms.begin();itorRoom!=m_Rooms.end();itorRoom++)
	{
		if( itorRoom->second )
		{
			delete itorRoom->second;
		}
	}
	m_Rooms.clear();

	m_pPool->Stop( );
	DestoryConnectPool( m_pPool );

	DebugLogOut("CServer:: Destroy End...");

	FiniLogger( );
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
		printf_s("%s Error:GameServer %s \n",Tool::GetTimeString(m_CurTime).c_str(),logbuf );
	}
}

void CServer::DebugLog(const char* logstr,...)
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

void CServer::DebugLogOut(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1]={0};
	va_list args;
	va_start(args,logstr);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if( len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_INFO,logbuf);
		printf_s("%s %s \n",Tool::GetTimeString().c_str(),logbuf);
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

void CServer::InitCeShiLog()
{
	if ( N_CeShiLog::c_FuncTimeLog )DebugLogOut("记录每个函数的运行时间");
	if ( N_CeShiLog::c_MoneyLog )DebugLogOut("记录玩家游戏币变化的每个细节");
	if ( N_CeShiLog::c_RightLog )DebugLogOut("记录玩家RIGHT变化的细节");
	if ( N_CeShiLog::c_XieYiLog )DebugLogOut("记录协议的速度大小以及协议分类的细节");
	if ( N_CeShiLog::c_NewMomory )DebugLogOut("记录内存分配回收的情况");
	if ( N_CeShiLog::c_BotPlayerLeave )DebugLogOut("测试机器人随机离开 ");
	if ( N_CeShiLog::c_PlayerAction )DebugLogOut("测试玩家的每一个动作进出房间桌子坐下站起 ");
	if ( N_CeShiLog::c_CityPlayer )DebugLogOut("记录同城相关的信息 ");
	if ( N_CeShiLog::c_GSOnTimeDiff )DebugLogOut("服务器每%d秒输出一次OnTimer运行时间",N_CeShiLog::c_GSOnTimeDiff);
	if ( N_CeShiLog::c_GSMaxOnTime )DebugLogOut("服务器OnTimer运行超过%d毫秒时将输出",N_CeShiLog::c_GSMaxOnTime);
}

void CServer::InitGameRule(const CGameServerConfig& GSConfig)
{
	m_GameRule = "";
}

bool CServer::OnPriorityEvent( void )
{
	return false;
}

IConnect* CServer::Connect( const char* ip, int port )
{
	if ( m_pPool )
	{
		return m_pPool->Connect(ip,port);
	}
	DebugError(" CServer::Connect ip=%s,port=%d",ip,port);
	return 0;
}

void CServer::OnTimer( void )
{
	TraceStackPath logTP("CServer::OnTimer");

	if ( !m_bIsInitOK ) return ;
	if ( !m_bInitDBInfo ) return ;

	try
	{
		CLogFuncTime lft(m_FuncTime,"OnTimer");
		static INT64 s_nStartTime=0,s_nTimeEnd=0,s_nTotalUseTime=0,s_nOnTimeCount=0;
		static INT32 s_nUseTime=0,s_nMaxUseTime=0;

		m_CurTime = static_cast<UINT32>(time( NULL ));

		s_nOnTimeCount++;
		s_nStartTime = Tool::GetMilliSecond();
		DebugInfo("CGameServer::OnTimer Start");

		if( m_CurTime - m_CheckRoomTime >= N_Time::t_CheckRoom ) //主要是游戏循环的检查
		{
			m_CheckRoomTime = m_CurTime;
			for(MapRoom::iterator itorRoom=m_Rooms.begin();itorRoom!=m_Rooms.end();itorRoom++)
			{
				//itorRoom->second->OnTimer(m_CurTime);
			}
		}

		m_memOperator.TestConnect();

		if ( m_pDBSocket->IsConnected() )
		{
			m_bOutOfDBServer = true;
			m_DBServerInUseTime = m_CurTime;
		}
		if ( m_bOutOfDBServer && (m_CurTime - m_DBServerInUseTime >= 60) )
		{
			m_bOutOfDBServer = false;
			DebugError("OnTime Out Of DBServer InUseTime=%d CurTime=%d",m_DBServerInUseTime,m_CurTime );
		}

		if ( m_CurTime - m_CheckPlayerLockTime > 600)
		{
			m_CheckPlayerLockTime = m_CurTime;
			CheckPlayerLocked();
		}

		//好友信息更新
		if ( m_CurTime - m_CheckFirendPlayerTime >= N_Time::T_CheckFriendPlayer )
		{
			m_CheckFirendPlayerTime = m_CurTime;
			OnTimeUpdateFirendPlayerState();
		}

		//机器人玩家分配
		if ( m_CurTime - m_CheckBotPlayerTime >= 300 )
		{
			m_CheckBotPlayerTime = m_CurTime;
			OnTimeCheckBotPlayer();
		}

		//主要是针对SOCKET的心跳检测
		if( m_CurTime - m_CheckPlayerActiveTime >= N_Time::t_PlayerActive )
		{
			m_CheckPlayerActiveTime = m_CurTime;
			CheckClientSocketActive();	
		}

		if( m_CurTime - m_CheckPlayerOnLine >= N_Time::t_PlayerActive )
		{
			m_CheckPlayerOnLine = m_CurTime;
			CheckPlayerActive();
		}

		if ( m_CurTime-m_CheckAddFriendTime >= N_Time::t_CheckAddFriend )
		{
			m_CheckAddFriendTime = m_CurTime;

			//DebugInfo("CGameServer::OnTimer 玩家被请求加好友的检测");
			for( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer )
			{
				itorPlayer->second->DeletePIDByTime(m_CurTime - N_Time::t_KeepAddFriend);
			}
		}

		if ( m_CurTime - m_CheckXieYiSpeedTime >= 600 )
		{
			PrintIOSpeed();
			m_CheckXieYiSpeedTime = m_CurTime;
		}
		if ( m_CurTime - m_CheckXieYiInOutTime >= 1800 )
		{		
			PrintIOXieYi();	
			m_CheckXieYiInOutTime = m_CurTime;
		}

		if( m_CurTime >= m_NewDayTime )
		{
			OnNewDay(m_CurTime);
		}

		if ( m_CurTime >= m_OneMinuteTime )
		{
			OnOneMinite(m_CurTime);
		}
		if ( m_CurTime >= m_TenMinuteTime )
		{
			OnTenMinite(m_CurTime);
		}
		if ( m_CurTime >= m_HourTime )
		{
			OnHour(m_CurTime);
		}

		if ( m_CurTime - m_SendPlayerOnLineTime >= 60 )
		{
			m_SendPlayerOnLineTime = m_CurTime;
			SendPlayerOnline();
		}

		s_nTimeEnd = Tool::GetMilliSecond();
		s_nUseTime = INT32(s_nTimeEnd - s_nStartTime);
		s_nTotalUseTime += s_nUseTime;
		if ( s_nUseTime > s_nMaxUseTime )
		{
			s_nMaxUseTime = s_nUseTime;
			DebugLogOut("最长时间为：%d",s_nMaxUseTime);
		}
		if ( m_CurTime%N_CeShiLog::c_GSOnTimeDiff==0 || s_nUseTime>=N_CeShiLog::c_GSMaxOnTime )
		{
			DebugLogOut("%s MaxTime=%d CurUseTime=%d AverUseTime=%d",
				Tool::GetTimeString(m_CurTime).c_str(),s_nMaxUseTime,s_nUseTime,int(s_nTotalUseTime/s_nOnTimeCount) );
		}

		DebugInfo("CGameServer::OnTimer End UseTicket=%d",s_nUseTime );
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

void CServer::CheckPlayerLocked()
{
	TraceStackPath logTP("CServer::CheckPlayerLocked");
	CLogFuncTime lft(m_FuncTime,"CheckPlayerLocked");

	PlayerPtr pPlayer;
	for ( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
	{
		pPlayer = itorPlayer->second;
		if ( pPlayer->Islocked() && m_CurTime-pPlayer->GetLockedTime() >= 2 )
		{
			DebugError("CheckPlayerLocked AID=%d PID=%d Time=%d",pPlayer->GetAID(),pPlayer->GetPID(),int(m_CurTime-pPlayer->GetLockedTime()) );
			if ( m_CurTime-pPlayer->GetLockedTime() >= 60 )
			{
				ReqUnlockPlayer(pPlayer);
			}			
		}
	}
}

void CServer::SendPlayerOnline()
{
	TraceStackPath logTP("CServer::SendPlayerOnline");
	CLogFuncTime lft(m_FuncTime,"SendPlayerOnline");

	MapPlayerOnLine mapPOL;
	PlayerPtr pPlayer;
	for ( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
	{
		pPlayer = itorPlayer->second;
		short AID = pPlayer->GetAID();
		MapPlayerOnLine::iterator itorPOL = mapPOL.find(AID);
		if ( itorPOL == mapPOL.end() )
		{
			mapPOL.insert(make_pair(AID,stPlayerOnLine()));
			itorPOL = mapPOL.find(AID);
		}
		stPlayerOnLine& stPOL = itorPOL->second;

		if ( pPlayer && pPlayer->IsOnLine() )
		{
			stPOL.m_Total++;
			if ( pPlayer->IsInTable() )
			{
				if ( pPlayer->GetSitID() )
				{
					assert( pPlayer->IsPlayerSitOnTable() );					
					assert( pPlayer->IsWaiting() || pPlayer->IsPlaying() );
					stPOL.m_Game++;

				}
				else
				{
					stPOL.m_Table++;
				}
			}
			else
			{
				stPOL.m_Lobby++;
			}
		}
	}

	DBServerXY::WDB_ReportPlayerOnline msgRPOL;
	msgRPOL.m_ServerID = m_pGSConfig->m_cfServer.m_ServerID;

	stPlayerOnLine stPOL;
	for (MapPlayerOnLine::iterator itorPOL=mapPOL.begin();itorPOL!=mapPOL.end();++itorPOL )
	{
		msgRPOL.m_AID      = itorPOL->first;
		msgRPOL.m_Total    = itorPOL->second.m_Total;
		msgRPOL.m_Lobby    = itorPOL->second.m_Lobby;
		msgRPOL.m_Table    = itorPOL->second.m_Table;
		msgRPOL.m_Game     = itorPOL->second.m_Game;
		msgRPOL.m_Match    = itorPOL->second.m_Match;
		SendMsgToDBServer(msgRPOL);

		stPOL.m_Total     += itorPOL->second.m_Total;
		stPOL.m_Lobby     += itorPOL->second.m_Lobby;
		stPOL.m_Table     += itorPOL->second.m_Table;
		stPOL.m_Game      += itorPOL->second.m_Game;
		stPOL.m_Match     += itorPOL->second.m_Match;
	}

	msgRPOL.m_AID         = 99;
	msgRPOL.m_Total       = stPOL.m_Total;
	msgRPOL.m_Lobby       = stPOL.m_Lobby;
	msgRPOL.m_Table       = stPOL.m_Table;
	msgRPOL.m_Game        = stPOL.m_Game;
	msgRPOL.m_Match       = stPOL.m_Match;
	SendMsgToDBServer(msgRPOL);
}

void CServer::CheckPlayerActive()
{
	TraceStackPath logTP("CServer::CheckPlayerActive");
	CLogFuncTime lft(m_FuncTime,"CheckPlayerActive");

	//检测玩家断线的超时时间
	//DebugInfo("CGameServer::OnTimer 玩家断线的检测");
	for( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
	{
		PlayerPtr pPlayer = itorPlayer->second;
		if( pPlayer->GetSocket() == NULL && pPlayer->IsOnLine() && pPlayer->GetCloseSocketTime() >= N_Time::t_PlayerOffLine )
		{
			DebugInfo("玩家断线超时 AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID());
			KillPlayer(pPlayer);
		}
	}

	//检测保留下线玩家对象的时间，主要是考虑玩家有可能下线后立即上线，减少数据读取等
	//DebugInfo("CGameServer::OnTimer 保存玩家对象时间的检测");
	for ( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();)
	{
		PlayerPtr pPlayer = itorPlayer->second;
		if ( pPlayer->IsDead() && pPlayer->GetKillTime() >= N_Time::t_KeepPlayerData )
		{
			ClearPlayerData(pPlayer);
			itorPlayer = m_Players.erase(itorPlayer);
			pPlayer = NULL;

			if ( m_pGSConfig->CeshiMode() )
			{
				int TotalCount = 0;
				for ( int nCount=0;nCount<PLAYER_ST_END;nCount++)
				{
					TotalCount += CPlayer::s_State[nCount];
				}
				if ( TotalCount != m_BotPlayerInIdle.size()+m_BotPlayerInUse.size()+m_Players.size() )
				{
					DebugError("BotIdle=%d BotInUse=%d PlayerSize=%d Total=%d",
						m_BotPlayerInIdle.size(),m_BotPlayerInUse.size(),m_Players.size(),TotalCount);
					for ( int nCount=0;nCount<PLAYER_ST_END;nCount++)
					{
						DebugError("State=%d Count=%d",nCount,CPlayer::s_State[nCount]);
					}
				}
				DebugInfo("Player nCount=%d",CPlayer::GetTotalPlayerCount());
			}
		}
		else
		{
			++itorPlayer;
		}
	}
}
void CServer::CheckClientSocketActive()
{
	TraceStackPath logTP("CServer::CheckClientSocketActive");
	CLogFuncTime lft(m_FuncTime,"CheckClientSocketActive");

	//DebugInfo("CGameServer::OnTimer 已登陆的SOCKET的心跳检测");
	for( MapClientSocket::iterator itorClient = m_Clients.begin();itorClient != m_Clients.end();itorClient++ )
	{
		GameServerSocket* pPlayerSocket = itorClient->second;
		UINT32 nActiveTime = pPlayerSocket->GetActiveTime();
		int nPassTime = m_CurTime - nActiveTime;
#ifdef CESHI
		if ( nPassTime >= 1800 && pPlayerSocket->GetOutTimes() >= T_MaxSocketOutTimes )
#else
		if ( nPassTime >= N_Time::t_SocketActive && pPlayerSocket->GetOutTimes() >= T_MaxSocketOutTimes )
#endif
		{
			PlayerPtr pPlayer = pPlayerSocket->GetPlayer();
			if( pPlayer )
			{
				DebugError("CServer::OnTimer PassTime=%d connet=%d AID=%d PID=%d",
					nPassTime,reinterpret_cast<int>(pPlayerSocket->GetConnect()),pPlayer->GetAID(),pPlayer->GetPID() );
				pPlayer->DisAttachSocket();
				KillPlayer(pPlayer);
			}
			else
			{
				DebugError("CServer::OnTimer Can't Find Player Time=%d connet=%d",
					(m_CurTime-nActiveTime),reinterpret_cast<int>(pPlayerSocket->GetConnect()) );
			}
			pPlayerSocket->Close();
		}
		else
		{
			pPlayerSocket->SendActive();
		}
	}

	//DebugInfo("CGameServer::OnTimer 未登陆的SOCKET的心跳检测");
	for( MapClientSocket::iterator itorClient = m_LoginSockets.begin();itorClient != m_LoginSockets.end();itorClient++ )
	{
		GameServerSocket* pLoginSocket = itorClient->second;
		if ( m_CurTime - pLoginSocket->GetActiveTime() >= N_Time::t_SocketActive )
		{
			pLoginSocket->Close();
		}
		else
		{
			pLoginSocket->SendActive();
		}
	}

	//DebugInfo("CGameServer::OnTimer 已登陆的SOCKET的STOP检测");
	for( MapClientSocket::iterator itorClient = m_Clients.begin();itorClient != m_Clients.end();itorClient++ )
	{
		GameServerSocket* pClientSocket = itorClient->second;
		if ( pClientSocket->GetStopTime() && m_CurTime-pClientSocket->GetStopTime()>=N_Time::t_SocketStopTime )
		{
			pClientSocket->Close();
		}
	}

	//DebugInfo("CGameServer::OnTimer 未登陆的SOCKET的STOP检测");
	for( MapClientSocket::iterator itorClient = m_LoginSockets.begin();itorClient != m_LoginSockets.end();itorClient++ )
	{
		GameServerSocket* pClientSocket = itorClient->second;
		if ( pClientSocket->GetStopTime() && m_CurTime-pClientSocket->GetStopTime()>=N_Time::t_SocketStopTime )
		{
			pClientSocket->Close();
		}
	}
}
void CServer::PrintIOSpeed()
{
	TraceStackPath logTP("CServer::PrintIOSpeed");

	int UseTime = m_CurTime - m_CheckXieYiSpeedTime;

	if ( N_CeShiLog::c_XieYiLog )
	{
		DebugLogOut("CS总：InPacket=%-8lld InByte=%-8lld OutPacket=%-8lld OutByte=%-8lld",
			CMoGuiServerSocket::s_TotalInPacket,
			CMoGuiServerSocket::s_TotalInByte,
			CMoGuiServerSocket::s_TotalOutPacket,
			CMoGuiServerSocket::s_TotalOutByte );

		DebugLogOut("速度: InPacket=%-8lld InByte=%-8lld OutPacket=%-8lld OutByte=%-8lld",
			(CMoGuiServerSocket::s_TotalInPacket-CMoGuiServerSocket::s_LastTotalInPacket)/max(1,UseTime),
			(CMoGuiServerSocket::s_TotalInByte-CMoGuiServerSocket::s_LastTotalInByte)/max(1,UseTime),
			(CMoGuiServerSocket::s_TotalOutPacket-CMoGuiServerSocket::s_LastTotalOutPacket)/max(1,UseTime),
			(CMoGuiServerSocket::s_TotalOutByte-CMoGuiServerSocket::s_LastTotalOutByte)/max(1,UseTime) );

		DebugLogOut("DB总：InPacket=%-8lld InByte=%-8lld OutPacket=%-8lld OutByte=%-8lld",
			CMoGuiClientSocket::s_TotalInPacket,
			CMoGuiClientSocket::s_TotalInByte,
			CMoGuiClientSocket::s_TotalOutPacket,
			CMoGuiClientSocket::s_TotalOutByte );

		DebugLogOut("速度: InPacket=%-8lld InByte=%-8lld OutPacket=%-8lld OutByte=%-8lld",
			(CMoGuiClientSocket::s_TotalInPacket-CMoGuiClientSocket::s_LastTotalInPacket)/max(1,UseTime),
			(CMoGuiClientSocket::s_TotalInByte-CMoGuiClientSocket::s_LastTotalInByte)/max(1,UseTime),
			(CMoGuiClientSocket::s_TotalOutPacket-CMoGuiClientSocket::s_LastTotalOutPacket)/max(1,UseTime),		
			(CMoGuiClientSocket::s_TotalOutByte-CMoGuiClientSocket::s_LastTotalOutByte)/max(1,UseTime) );

		CMoGuiServerSocket::s_LastTotalInByte = CMoGuiServerSocket::s_TotalInByte;
		CMoGuiServerSocket::s_LastTotalInPacket = CMoGuiServerSocket::s_TotalInPacket;
		CMoGuiServerSocket::s_LastTotalOutByte = CMoGuiServerSocket::s_TotalOutByte;
		CMoGuiServerSocket::s_LastTotalOutPacket = CMoGuiServerSocket::s_TotalOutPacket;

		CMoGuiClientSocket::s_LastTotalInByte = CMoGuiClientSocket::s_TotalInByte;
		CMoGuiClientSocket::s_LastTotalInPacket = CMoGuiClientSocket::s_TotalInPacket;
		CMoGuiClientSocket::s_LastTotalOutByte = CMoGuiClientSocket::s_TotalOutByte;
		CMoGuiClientSocket::s_LastTotalOutPacket = CMoGuiClientSocket::s_TotalOutPacket;
	}	
}
void CServer::PrintIOXieYi()
{
	TraceStackPath logTP("CServer::PrintIOXieYi");

	if ( N_CeShiLog::c_XieYiLog )
	{
		DebugInfo("游戏总收发：PacketIn=%-10s PacketOut=%-10s OutByte=%-10s InByte=%-10s",
			Tool::N2S(CMoGuiServerSocket::s_TotalInPacket).c_str(),Tool::N2S(CMoGuiServerSocket::s_TotalOutPacket).c_str(),
			Tool::N2S(CMoGuiServerSocket::s_TotalOutByte).c_str(),Tool::N2S(CMoGuiServerSocket::s_TotalInByte).c_str() );

		VectorXieYiLog vectorXYLog;
		for (MapXieYiIO::iterator itorXieIO=CMoGuiServerSocket::s_InXieYi.begin();itorXieIO!=CMoGuiServerSocket::s_InXieYi.end();itorXieIO++)
		{
			stXieYiLog stXYL;
			stXYL.m_XieYiID    = itorXieIO->first;
			stXYL.m_TotalByte  = itorXieIO->second;
			stXYL.m_Percent    = INT32(itorXieIO->second*10000/max(CMoGuiServerSocket::s_TotalInByte,1));
			vectorXYLog.push_back(stXYL);
		}
		sort(vectorXYLog.begin(),vectorXYLog.end(),CCompareXieYiTotalByte());
		for ( size_t nPos=0;nPos<vectorXYLog.size();++nPos)
		{
			const stXieYiLog& rstXY = vectorXYLog[nPos];
			DebugInfo("In  XYID=%-5d XYSIZE=%-10s Percent=%d",rstXY.m_XieYiID,Tool::N2S(rstXY.m_TotalByte).c_str(),rstXY.m_Percent);
		}

		vectorXYLog.clear();
		for (MapXieYiIO::iterator itorXieIO=CMoGuiServerSocket::s_OutXieYi.begin();itorXieIO!=CMoGuiServerSocket::s_OutXieYi.end();itorXieIO++)
		{
			stXieYiLog stXYL;
			stXYL.m_XieYiID    = itorXieIO->first;
			stXYL.m_TotalByte  = itorXieIO->second;
			stXYL.m_Percent    = INT32(itorXieIO->second*10000/max(CMoGuiServerSocket::s_TotalOutByte,1));
			vectorXYLog.push_back(stXYL);
		}
		sort(vectorXYLog.begin(),vectorXYLog.end(),CCompareXieYiTotalByte());
		for ( size_t nPos=0;nPos<vectorXYLog.size();++nPos)
		{
			const stXieYiLog& rstXY = vectorXYLog[nPos];
			DebugInfo("Out XYID=%-5d XYSIZE=%-10s Percent=%d",rstXY.m_XieYiID,Tool::N2S(rstXY.m_TotalByte).c_str(),rstXY.m_Percent);
		}
	}	
}

void CServer::OnOneMinite(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnOneMinite");
	CLogFuncTime lft(m_FuncTime,"OnOneMinite");

	DebugLog("OnOneMinite");
	m_OneMinuteTime += 60;
}

void CServer::OnTenMinite(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnTenMinite");
	CLogFuncTime lft(m_FuncTime,"OnTenMinite");

	m_TenMinuteTime += 600;

	DebugLog("OnTenMinite");

	INT64 nTotal = m_GetBotStep1 + m_GetBotStep2 + m_GetBotFailed;
	nTotal = max(nTotal,1);
	DebugLogOut("Total=%d Step1=%d Step2=%d Failed=%d",int(nTotal),int(m_GetBotStep1),int(m_GetBotStep2),int(m_GetBotFailed));
	DebugLogOut("GetBot 成功率 Step1=%d Step2=%d Failed=%d",int(m_GetBotStep1*100/nTotal),int(m_GetBotStep2*100/nTotal),int(m_GetBotFailed*100/nTotal));	
	DebugLogOut("Bot Player Used=%d Idle=%d",m_BotPlayerInUse.size(),m_BotPlayerInIdle.size() );

	DebugLogOut("Accept=%d Close=%d Connect=%d Socket=%d",m_nAcceptCount,m_nCloseCount,m_nAcceptCount-m_nCloseCount,
		m_LoginSockets.size()+m_Clients.size() );

	HANDLE hCurProc = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(hCurProc, &pmc, sizeof(pmc));
	DebugLogOut("%-10d            显示内存消耗量",pmc.PagefileUsage/1024);
	DebugLogOut("%-10d            峰值内存",pmc.PeakWorkingSetSize/1024);
	DebugLogOut("%-10d %-10d 工作内存",pmc.WorkingSetSize/1024,pmc.QuotaPeakPagedPoolUsage/1024);
	DebugLogOut("%-10d %-10d 分页池",pmc.QuotaPagedPoolUsage/1024,pmc.QuotaPeakNonPagedPoolUsage/1024);
	DebugLogOut("%-10d %-10d 缓冲池",pmc.QuotaNonPagedPoolUsage/1024,pmc.PeakPagefileUsage/1024);

	int nTotalState = 0;
	for ( int nState=0;nState<PLAYER_ST_END;nState++)
	{
		nTotalState += CPlayer::s_State[nState];
	}
	int nTotalCount = m_BotPlayerInIdle.size() + m_BotPlayerInUse.size() + m_Players.size();
	DebugLogOut("TotalState=%d TotalCount=%d",nTotalState,nTotalCount);
	if ( nTotalState != nTotalCount )
	{
		DebugError("BotIdle=%d BotInUse=%d PlayerSize=%d TotalState=%d",
			m_BotPlayerInIdle.size(),m_BotPlayerInUse.size(),m_Players.size(),nTotalState);
		for ( int nState=0;nState<PLAYER_ST_END;nState++)
		{
			DebugError("State=%d Count=%d",nState,CPlayer::s_State[nState]);			
		}
	}
	DebugLogOut("Player nCount=%d",CPlayer::GetTotalPlayerCount());
	DebugLogOut("PlayerMoneyLogCount=%d",CPlayer::s_nMoneyLogCount);

	if ( N_CeShiLog::c_NewMomory )
	{
		DebugLogOut("GameLogic=%d GameRoom=%d GameTable=%d MessageBlock=%d GameSocket=%d",
			CDeZhouGameLogic::GetTotalCount(),CGameRoom::GetTotalCount(),CGameTable::GetTotalCount(),
			MessageBlock::GetTotalCount(),GameServerSocket::GetTotalCount()	);

		DebugLogOut("MessageBlock=%d Use=%d New=%lld Delete=%lld Diff=%lld",
			MessageBlock::GetTotalCount(),MessageBlock::GetUseCount(),
			MessageBlock::GetNewTimes(),MessageBlock::GetDeleteTimes(),
			MessageBlock::GetNewTimes()-MessageBlock::GetDeleteTimes() );
	}

	if ( N_CeShiLog::c_FuncTimeLog )
	{
		{
			VectorFuncTime vectorFT;
			MapFuncTime& rTempMapFT = m_FuncTime.m_mapFuncTime;
			for ( MapFuncTime::iterator itorFT=rTempMapFT.begin();itorFT!=rTempMapFT.end();++itorFT)
			{
				vectorFT.push_back(itorFT->second);
			}
			sort(vectorFT.begin(),vectorFT.end(),CCompareFuncTime());
			DebugLog("Game Server Func Count=%d",vectorFT.size());
			for ( size_t nPos=0;nPos<vectorFT.size();++nPos)
			{
				const stFuncTimeLog& rFT = vectorFT[nPos];
				DebugLog("GS %-30s TotalTicket=%-10lld Times=%-8lld Average=%-8lld Max=%-8lld Min=%-5lld",
					rFT.m_strFuncName.c_str(),
					rFT.m_TotalTicket/1000,
					rFT.m_nTimes,				
					rFT.m_TotalTicket/max(rFT.m_nTimes,1),
					rFT.m_MaxTicket,
					rFT.m_MinTicket );
			}
		}
		
		{
			VectorFuncTime vectorFT;
			MapFuncTime& rTempRoomMapFT = CGameRoom::s_RoomFuncTime.m_mapFuncTime;
			for ( MapFuncTime::iterator itorFT=rTempRoomMapFT.begin();itorFT!=rTempRoomMapFT.end();++itorFT)
			{
				vectorFT.push_back(itorFT->second);
			}
			sort(vectorFT.begin(),vectorFT.end(),CCompareFuncTime());
			DebugLog("Room Func Count=%d",vectorFT.size());
			for ( size_t nPos=0;nPos<vectorFT.size();++nPos)
			{
				const stFuncTimeLog& rFT = vectorFT[nPos];
				DebugLog("Rm %-30s TotalTicket=%-10lld Times=%-8lld Average=%-8lld Max=%-8lld Min=%-5lld",
					rFT.m_strFuncName.c_str(),
					rFT.m_TotalTicket/1000,
					rFT.m_nTimes,				
					rFT.m_TotalTicket/max(rFT.m_nTimes,1),
					rFT.m_MaxTicket,
					rFT.m_MinTicket );
			}
		}

		
		{
			VectorFuncTime vectorFT;
			MapFuncTime& rTempTableMapFT = CGameTable::s_TableFuncTime.m_mapFuncTime;
			for ( MapFuncTime::iterator itorFT=rTempTableMapFT.begin();itorFT!=rTempTableMapFT.end();++itorFT)
			{
				vectorFT.push_back(itorFT->second);
			}
			sort(vectorFT.begin(),vectorFT.end(),CCompareFuncTime());
			DebugLog("Table Func Count=%d",vectorFT.size());
			for ( size_t nPos=0;nPos<vectorFT.size();++nPos)
			{
				const stFuncTimeLog& rFT = vectorFT[nPos];	
				DebugLog("Tb %-30s TotalTicket=%-10lld Times=%-8lld Average=%-8lld Max=%-8lld Min=%-5lld",
					rFT.m_strFuncName.c_str(),
					rFT.m_TotalTicket/1000,
					rFT.m_nTimes,				
					rFT.m_TotalTicket/max(rFT.m_nTimes,1),
					rFT.m_MaxTicket,
					rFT.m_MinTicket );
			}
		}
		
		{
			VectorFuncTime vectorFT;
			MapFuncTime& rTempLogicMapFT = CDeZhouGameLogic::s_LogicFuncTime.m_mapFuncTime;
			for ( MapFuncTime::iterator itorFT=rTempLogicMapFT.begin();itorFT!=rTempLogicMapFT.end();++itorFT)
			{
				vectorFT.push_back(itorFT->second);
			}
			sort(vectorFT.begin(),vectorFT.end(),CCompareFuncTime());
			DebugLog("DeZhouGameLogic Func Count=%d",vectorFT.size());
			for ( size_t nPos=0;nPos<vectorFT.size();++nPos)
			{
				const stFuncTimeLog& rFT = vectorFT[nPos];	
				DebugLog("LG %-30s TotalTicket=%-10lld Times=%-8lld Average=%-8lld Max=%-8lld Min=%-5lld",
					rFT.m_strFuncName.c_str(),
					rFT.m_TotalTicket/1000,
					rFT.m_nTimes,
					rFT.m_TotalTicket/max(rFT.m_nTimes,1),
					rFT.m_MaxTicket,
					rFT.m_MinTicket );
			}
		}
	}
}
void CServer::OnHour(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnHour");
	CLogFuncTime lft(m_FuncTime,"OnHour");

	m_HourTime += 3600;
}
void CServer::OnNewDay(UINT32 curTime)
{
	TraceStackPath logTP("CServer::OnNewDay");
	CLogFuncTime lft(m_FuncTime,"OnNewDay");

	m_NewDayTime = UINT32(Tool::GetNewDayTime(curTime));

	UINT32 nStartTicket = ::GetTickCount();
	for( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
	{
		PlayerPtr pPlayer = itorPlayer->second;
		if( pPlayer )
		{
			pPlayer->InitEveryDay();
		}
	}

	UINT32 UsedTicket = ::GetTickCount() - nStartTicket;
	DebugLogOut("OnNewDay UsedTicket=%d",UsedTicket);
}

void CServer::OnAccept( IConnect* connect )
{
	TraceStackPath logTP("CServer::OnAccept");
	CLogFuncTime lft(m_FuncTime,"OnAccept");

	if ( m_bInitDBInfo )
	{
		try
		{
			GameServerSocket* client = new GameServerSocket( this, connect );
			if ( m_pGSConfig->m_cfServer.m_IsCrypt )
			{
				client->SetCrypt(true);
			}
			m_LoginSockets.insert( make_pair(connect, client) );
			m_nAcceptCount++;

			DebugInfo("CServer::OnAccept 接收连接 connect=%d GSSocket=%d LoginSize=%d ClientSize=%d",
				reinterpret_cast<int>(connect),reinterpret_cast<int>(client),m_LoginSockets.size(),m_Clients.size() );
		}
		catch (...)
		{
			DebugError("CServer::OnAccept 内存不足 接收连接失败");
		}
	}
	else
	{
		DebugInfo("CServer::OnAccept 关闭连接 connect=%d UnloginSize=%d",reinterpret_cast<int>(connect),m_LoginSockets.size());
		connect->Close();
	}
}

void CServer::OnClose( IConnect* nocallbackconnect, bool bactive )
{
	DebugInfo("CServer::OnClose NoCallBack Connect");
	DealCloseSocket( nocallbackconnect );
}

void CServer::DealCloseSocket( IConnect* connect )
{
	TraceStackPath logTP("CServer::DealCloseSocket");
	CLogFuncTime lft(m_FuncTime,"DealCloseSocket");

	DebugInfo("CServer::DealCloseSocket start connect=%d LoginSize=%d ClientSize=%d",
		reinterpret_cast<int>(connect),m_LoginSockets.size(),m_Clients.size());

	MapClientSocket::iterator itorConnect = m_Clients.find( connect );
	if ( itorConnect != m_Clients.end() )
	{
		PlayerPtr pPlayer = itorConnect->second->GetPlayer();
		DebugInfo("CServer::DealCloseSocket pPlayer=%d",pPlayer );
		if( pPlayer )
		{
			pPlayer->DisAttachSocket();
			if( pPlayer->CanKillNow() )
			{
				KillPlayer(pPlayer);
			}
		}
		else
		{
			DebugInfo("CServer:: Can't Find Player Connect=%d 同名登陆 ",itorConnect->second->GetConnect());
		}
		GameServerSocket *pGameSocket = itorConnect->second;
		if ( pGameSocket && pGameSocket->GetCloseFlag()>0 )
		{
			DBServerXY::WDB_PlayerClientError msgPCE;
			msgPCE.m_Flag      = pGameSocket->GetCloseFlag();
			msgPCE.m_strIP     = pGameSocket->GetPeerStringIp();
			if ( pPlayer )
			{
				msgPCE.m_AID   = pPlayer->GetAID();
				msgPCE.m_PID   = pPlayer->GetPID();
			}
			SendMsgToDBServer(msgPCE);
		}

		m_nCloseCount++;
		safe_delete(itorConnect->second);
		m_Clients.erase( itorConnect );
	}
	else
	{
		itorConnect = m_LoginSockets.find( connect );
		if ( itorConnect != m_LoginSockets.end() )
		{
			DebugInfo("CServer:: UnLogin Player GameSocket=%d PID=%d",
				reinterpret_cast<int>(itorConnect->second),itorConnect->second->GetSocketPID() );

			GameServerSocket *pGameSocket = itorConnect->second;
			if ( pGameSocket && pGameSocket->GetCloseFlag()>0 )
			{
				DBServerXY::WDB_PlayerClientError msgPCE;
				msgPCE.m_Flag      = pGameSocket->GetCloseFlag();
				msgPCE.m_strIP     = pGameSocket->GetPeerStringIp();
				SendMsgToDBServer(msgPCE);
			}

			m_nCloseCount++;
			safe_delete(itorConnect->second);
			m_LoginSockets.erase(itorConnect);
		}
		else
		{
			DebugInfo("CServer:: Can't Find Connect...connect=%d",reinterpret_cast<int>(connect));
		}
	}

	DebugInfo("CServer::DealCloseSocket end LoginSize=%d ClientSize=%d",m_LoginSockets.size(),m_Clients.size() );
}

void CServer::ClearPlayerData( PlayerPtr pPlayer )
{
	TraceStackPath logTP("CServer::ClearPlayerData");
	CLogFuncTime lft(m_FuncTime,"ClearPlayerData");

	DebugInfo("清除玩家数据 AID=%d PID=%d",pPlayer->GetAID(),pPlayer->GetPID());

	if ( pPlayer->IsNotBotPlayer() )
	{
		MemcatchXY::DeZhou::memGSUserData memGSUD;
		if ( m_memOperator.GetGSUserData(pPlayer->GetAID(),pPlayer->GetPID(),memGSUD) )
		{
			memGSUD.m_LastActTime     = m_CurTime;
			memGSUD.m_CurStep         = pPlayer->m_TimeStep;
			memGSUD.m_PassTime        = pPlayer->m_TimePass;

			memGSUD.m_TodayGameCount  = pPlayer->m_TodayGameCount;
			memGSUD.m_TodayGameSecond = pPlayer->m_TodayGameSecond;
			memGSUD.m_HaveAwardInvite = pPlayer->m_HaveAwardInvite;
			m_memOperator.SetGSUserData(memGSUD);
		}

		DBServerXY::WDB_PlayerActionLog msgPAL;
		msgPAL.m_AID              = pPlayer->GetAID();
		msgPAL.m_PID              = pPlayer->GetPID();
		msgPAL.m_OneLifeTotalTime = max(0,m_CurTime - pPlayer->m_PlayerCreateTime - N_Time::t_KeepPlayerData );
		msgPAL.m_OneLifeGameTime  = pPlayer->m_OneLifeGameTime;
		msgPAL.m_OneLifeMatchTime = pPlayer->m_OneLifeMatchTime;
		msgPAL.m_EnterRoomTimes   = pPlayer->m_EnterRoomTimes;
		msgPAL.m_EnterTableTimes  = pPlayer->m_EnterTableTimes;
		msgPAL.m_SitDownTimes     = pPlayer->m_SitDownTimes;
		msgPAL.m_PlayTimes        = pPlayer->m_OneLiftPlayTimes;
		SendMsgToDBServer(msgPAL);
	}
}

void CServer::KillPlayer( PlayerPtr pPlayer )
{
	TraceStackPath logTP("CServer::KillPlayer");
	CLogFuncTime lft(m_FuncTime,"KillPlayer");

	DebugInfo("CServer::KillPlayer start pPlayer=%d PID=%d ST=%d tid=%d sid=%d",
		pPlayer,pPlayer->GetPID(),
		pPlayer->GetPlayerState(),pPlayer->GetTableID(),pPlayer->GetSitID() );
	
	if( CGameRoom* pRoom = GetRoomByID(pPlayer->GetRoomID()) )
	{
		assert(pRoom==pPlayer->GetRoom());
		pRoom->PlayerLeaveRoom(pPlayer);
	}
	else
	{
		assert( pPlayer->GetRoomID()==0 && (!pPlayer->GetRoom()) );
	}

	pPlayer->OnKill(m_CurTime);
	pPlayer->CheckGameInfo();

	DeleteFirendToClient(pPlayer);

	DBServerXY::DBS_PlayerQuite msgPQ;
	msgPQ.m_AID = pPlayer->GetAID();
	msgPQ.m_PID = pPlayer->GetPID();
	msgPQ.m_ServerID = this->GetServerID();
	SendMsgToDBServer(msgPQ);

	DebugInfo("CServer::KillPlayer end");
}

PlayerPtr CServer::GetPlayerByPID(UINT32 PID,int nFlag)
{
	TraceStackPath logTP("CServer::GetPlayerByPID");
	CLogFuncTime lft(m_FuncTime,"GetPlayerByPID");

	PlayerPtr retPlayer = NULL;

	if ( (nFlag&GetPID_Player) == GetPID_Player )
	{
		MapPlayer::const_iterator itorPlayer = m_Players.find(PID);
		if ( itorPlayer != m_Players.end() )
		{
			retPlayer = itorPlayer->second;
		}
		if ( nFlag == GetPID_Player )
		{
			return retPlayer;
		}
	}

	if ( (!retPlayer) && (nFlag&GetPID_BotUse)==GetPID_BotUse )
	{
		MapPlayer::iterator itorPlayer = m_BotPlayerInUse.find(PID);
		if ( itorPlayer != m_BotPlayerInUse.end() )
		{
			retPlayer = itorPlayer->second;
		}
		//if ( retPlayer ) DebugInfo("GetPlayerByPID InUse Bot PID=%d Have Find ",PID);
		//else DebugInfo("GetPlayerByPID InUse Bot PID=%d Can't Find ",PID);
		
		if ( (nFlag==(GetPID_Player|GetPID_BotUse)) || nFlag==GetPID_BotUse )
		{
			return retPlayer;
		}
	}
	
	if ( (!retPlayer) && (nFlag&GetPID_BotIdle)==GetPID_BotIdle )
	{
		for (ListPlayer::iterator itorTemp=m_BotPlayerInIdle.begin();itorTemp!=m_BotPlayerInIdle.end();++itorTemp)
		{
			if ( (*itorTemp)->GetPID() == PID )
			{
				retPlayer = *itorTemp;
				break;
			}
		}
		//if ( retPlayer ) DebugInfo("GetPlayerByPID InIdle Bot PID=%d Have Find ",PID);
		//else DebugInfo("GetPlayerByPID InIdle Bot PID=%d Can't Find ",PID);		
	}

	if ( !retPlayer )
	{
		DebugError("GetPlayerByPID PID=%d",PID);
	}
	return retPlayer;
}

void CServer::ResetClientPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::ResetClientPlayer");
	CLogFuncTime lft(m_FuncTime,"ResetClientPlayer");

	DebugInfo("CServer::ResetCommondPlayer Start PID=%d",pPlayer->GetPID());

	assert(pPlayer->IsClientPlayer());
	if( CGameRoom* pRoom=GetRoomByID(pPlayer->GetRoomID()) )
	{
		assert(pRoom==pPlayer->GetRoom());
		pRoom->PlayerLeaveRoom(pPlayer);
	}
	else
	{
		assert(pPlayer->GetRoomID()==0 && (!pPlayer->GetRoom()) );
	}

	pPlayer->LeaveRoom();
	pPlayer->SetPlayerState(PLAYER_ST_LOBBY);

	DebugInfo("CServer::ResetCommondPlayer End");
}

void CServer::ReqUnlockPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::ReqUnlockPlayer");

	DebugInfo("CServer::ReqUnlockPlayer Start PID=%d",pPlayer->GetPID() );

	if ( !pPlayer->IsPlayerSitOnTable() )
	{
		DBServerXY::DBS_ReqPlayerGameMoney msgPGM;
		msgPGM.m_AID         = pPlayer->GetAID();
		msgPGM.m_PID         = pPlayer->GetPID();
		msgPGM.m_nGameMoney  = pPlayer->GetGameMoney();
		SendMsgToDBServer(msgPGM);
	}
	DebugInfo("CServer::ReqUnlockPlayer End");
}

PlayerPtr CServer::GetBotPlayer(INT64 nMinMoney,INT64 nMaxMoney,bool bShowRet)
{
	TraceStackPath logTP("CServer::GetBotPlayer");
	CLogFuncTime lft(m_FuncTime,"GetBotPlayer");

	//DebugInfo("CServer::GetBotPlayer Start");

	int nLockPlayerCount = 0;
	PlayerPtr TempPlayer = NULL , retPlayer = NULL;	
	for ( ListPlayer::iterator itorBotPlayer = m_BotPlayerInIdle.begin();itorBotPlayer!=m_BotPlayerInIdle.end();++itorBotPlayer )
	{
		TempPlayer = *itorBotPlayer;
		if ( TempPlayer->Islocked() )
		{
			nLockPlayerCount++;
			if ( m_CurTime-TempPlayer->GetLockedTime() > 0 )
			{
				//DebugError("BotPlayer Locked PID=%d PassTime=%d",TempPlayer->GetPID(),m_CurTime-TempPlayer->GetLockedTime() );
			}
			continue;
		}
		if ( TempPlayer->GetGameMoney() >= nMinMoney && TempPlayer->GetGameMoney()<= nMaxMoney )
		{
			retPlayer = TempPlayer;
			break;
		}
	}

	if ( retPlayer )
	{
		MapPlayer::iterator itorPlayer = m_BotPlayerInUse.find(retPlayer->GetPID());
		if ( itorPlayer == m_BotPlayerInUse.end() )
		{
			m_BotPlayerInUse.insert(make_pair(retPlayer->GetPID(),retPlayer));
		}
		else
		{
			DebugError("GetBotPlayer BotPlayer Alread Exist PID=%d",retPlayer->GetPID());
		}

		ListPlayer::iterator itorPlayerIdle = find(m_BotPlayerInIdle.begin(),m_BotPlayerInIdle.end(),retPlayer);		
		if ( itorPlayerIdle!=m_BotPlayerInIdle.end() )
		{
			m_BotPlayerInIdle.erase(itorPlayerIdle);
		}
		else
		{
			DebugError("GetBotPlayer Can't Find BotPlayer In Idle List PID=%d",retPlayer->GetPID());
		}
	}
	else
	{
		if( bShowRet )
		{
			DebugError("GetBotPlayer Can't Find BotPlayer MinMoney=%s MaxMoney=%s Locked=%d Used=%d Idle=%d",
				N2S(nMinMoney).c_str(),N2S(nMaxMoney).c_str(),nLockPlayerCount,m_BotPlayerInUse.size(),m_BotPlayerInIdle.size() );
		}		
	}

	if ( m_BotPlayerNumber != int(m_BotPlayerInUse.size()+m_BotPlayerInIdle.size()) )
	{
		DebugError("GetBotPlayer Number=%d use=%d Idle=%d",m_BotPlayerNumber,m_BotPlayerInUse.size(),m_BotPlayerInIdle.size());
	}

	//DebugInfo("CServer::GetBotPlayer End");

	if ( retPlayer )
	{
		SendPlayerBeFollow(retPlayer);
	}

	return retPlayer;
}

void CServer::ResetBotPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::ResetBotPlayer");
	CLogFuncTime lft(m_FuncTime,"ResetBotPlayer");

	DebugInfo("CServer::ResetBotPlayer Start PID=%d",pPlayer->GetPID());

	if( CGameRoom* pRoom=GetRoomByID(pPlayer->GetRoomID()) )
	{
		assert(pRoom==pPlayer->GetRoom());
		pRoom->PlayerLeaveRoom(pPlayer);
	}
	else
	{
		assert(pPlayer->GetRoomID()==0 && (!pPlayer->GetRoom()) );
	}

	pPlayer->LeaveRoom();
	pPlayer->SetPlayerState(PLAYER_ST_LOBBY);
	pPlayer->m_EndBotPlayerTime = 0;

	pPlayer->CheckGameInfo();
	DeleteFirendToClient(pPlayer);

	MapPlayer::iterator itorPlayer = m_BotPlayerInUse.find(pPlayer->GetPID());
	if ( itorPlayer != m_BotPlayerInUse.end() )
	{
		m_BotPlayerInUse.erase(itorPlayer);
	}
	else
	{
		DebugError("ResetBotPlayer Can't find UseBotPlayer PID=%d",pPlayer->GetPID() );
	}

	ListPlayer::iterator itorPlayerIdle = find(m_BotPlayerInIdle.begin(),m_BotPlayerInIdle.end(),pPlayer);
	if ( itorPlayerIdle == m_BotPlayerInIdle.end() )
	{
		m_BotPlayerInIdle.push_back(pPlayer);
	}
	else
	{
		DebugError("ResetBotPlayer Alread find IdleBotPlayer PID=%d",pPlayer->GetPID() );
	}

	if ( m_BotPlayerNumber != int(m_BotPlayerInUse.size()+m_BotPlayerInIdle.size()))
	{
		DebugError("ResetBotPlayer Number=%d use=%d Idle=%d",m_BotPlayerNumber,m_BotPlayerInUse.size(),m_BotPlayerInIdle.size());
	}

	DebugInfo("CServer::ResetBotPlayer End");
}

void CServer::OnTimeCheckBotPlayer()
{
	TraceStackPath logTP("CServer::OnTimeCheckBotPlayer");
	CLogFuncTime lft(m_FuncTime,"OnTimeCheckBotPlayer");

	VectorPID vecResetPID;
	PlayerPtr pTempPlayer;
	for ( MapPlayer::iterator itorPlayer=m_BotPlayerInUse.begin();itorPlayer!=m_BotPlayerInUse.end();++itorPlayer)
	{
		pTempPlayer = itorPlayer->second;
		if ( pTempPlayer && (!pTempPlayer->IsInTable()) )
		{
			vecResetPID.push_back(pTempPlayer->GetPID());
			DebugError("Used Player Not In Table PID=%d RoomID=%d TableID=%d State=%d",pTempPlayer->GetPID(),pTempPlayer->GetRoomID(),
				pTempPlayer->GetTableID(),pTempPlayer->GetPlayerState() );
		}
	}

	for (ListPlayer::iterator itorPlayer=m_BotPlayerInIdle.begin();itorPlayer!=m_BotPlayerInIdle.end();++itorPlayer)
	{
		pTempPlayer = *itorPlayer;
		if ( pTempPlayer && (pTempPlayer->IsInTable() || pTempPlayer->IsInRoom() ))
		{
			vecResetPID.push_back(pTempPlayer->GetPID());
			DebugError("Idle Player In Room or Table PID=%d RoomID=%d TableID=%d State=%d",pTempPlayer->GetPID(),pTempPlayer->GetRoomID(),
				pTempPlayer->GetTableID(),pTempPlayer->GetPlayerState() );
		}
	}

	if ( vecResetPID.size() )
	{
		for ( size_t nPos=0;nPos<vecResetPID.size();++nPos )
		{
			pTempPlayer = GetPlayerByPID(vecResetPID[nPos]);
			if ( pTempPlayer )
			{
				ResetBotPlayer(pTempPlayer);
			}
		}		
	}
}

void CServer::SetBDSocket(CGameDBSocket* pDBSocket)
{
	if ( pDBSocket )
	{
		m_pDBSocket = pDBSocket;
	}
}
void CServer::InitServerRooms()
{
	TraceStackPath logTP("CServer::InitServerRooms");
	CLogFuncTime lft(m_FuncTime,"InitServerRooms");

	DebugInfo("CServer::InitServerRooms start");
	
	for(unsigned int i=0;i<m_RoomInfoVector.size();++i)
	{
		CreateRoomInfo cri;

		cri.m_RoomID           = m_RoomInfoVector[i].m_RoomID;
		cri.m_RoomName         = m_RoomInfoVector[i].m_RoomName;
		cri.m_Password         = m_RoomInfoVector[i].m_Password;
		cri.m_RoomRule         = m_RoomInfoVector[i].m_RoomRule;
		cri.m_RoomRuleEX       = m_RoomInfoVector[i].m_RoomRuleEX;
		cri.m_MatchRule        = m_RoomInfoVector[i].m_MatchRule;
		cri.m_AreaRule         = m_RoomInfoVector[i].m_AreaRule;

		CreateRoom(cri);
	}

	DebugInfo("CServer::InitServerRooms end");
}

int CServer::CreateRoom(CreateRoomInfo& cri)
{
	TraceStackPath logTP("CServer::CreateRoom");
	CLogFuncTime lft(m_FuncTime,"CreateRoom");
	
	DebugInfo("CServer::CreateRoom start");

	CGameRoom* pRoom = GetRoomByID(cri.m_RoomID);
	if( pRoom == NULL )
	{
		try
		{
			//DebugInfo("CreateRoom RoomID=%d Rule=%s RuleEx=%s Match=%s",cri.m_RoomID,cri.m_RoomRule.c_str(),cri.m_RoomRuleEX.c_str(),cri.m_MatchRule.c_str());
			pRoom = new CGameRoom(this,cri);
			if ( pRoom )
			{			
				m_Rooms.insert( make_pair(cri.m_RoomID,pRoom) );
				int RoomTableCount = 0;
				for(unsigned int i=0;i<m_TableInfoVector.size();++i)
				{
					if ( m_TableInfoVector[i].m_RoomID == cri.m_RoomID )
					{
						++RoomTableCount;
					}
				}
				if ( RoomTableCount )
				{
					for(unsigned int i=0;i<m_TableInfoVector.size();++i)
					{
						if ( m_TableInfoVector[i].m_RoomID != cri.m_RoomID )
						{
							continue;
						}

						CreateTableInfo cti;
						cti.m_TableID        = m_TableInfoVector[i].m_TableID;
						cti.m_TableName      = m_TableInfoVector[i].m_TableName;
						cti.m_Password       = m_TableInfoVector[i].m_Password;
						cti.m_TableRule      = m_TableInfoVector[i].m_TableRule;
						cti.m_FoundByWho     = m_TableInfoVector[i].m_FoundByWho;
						cti.m_EndTime        = m_TableInfoVector[i].m_TableEndTime;

						pRoom->CreateTable(cti);
					}
				}
				else
				{
					for ( int nCount=0;nCount<pRoom->GetMaxTableCount();++nCount )
					{
						CreateTableInfo cti;
						cti.m_TableID        = nCount + 1;
						cti.m_TableName      = pRoom->GetRoomName()+N2S(cti.m_TableID);
						cti.m_Password       = "";
						cti.m_TableRule      = pRoom->GetRoomRuleEx();
						cti.m_FoundByWho     = 0;
						cti.m_EndTime        = 0;

						pRoom->CreateTable(cti);
					}
				}

				DebugLogOut("RoomID=%d Tables=%-5d",pRoom->GetRoomID(),pRoom->GetTableCount());
			}
			else
			{
				DebugError("CreateRoom RoomID=%d",cri.m_RoomID);
			}
		}
		catch (...)
		{
			safe_delete(pRoom);
			DebugError("CServer::CreateRoom 创建房间失败");
		}
	}
	else
	{
		DebugError("CreateRoom Creat a exist Room id=%d",cri.m_RoomID);
	}

	DebugInfo("CServer::CreateRoom end");
	return 0;
}

CGameRoom* CServer::GetRoomByID( UINT16 roomid)
{
	TraceStackPath logTP("CServer::GetRoomByID");
	MapRoom::iterator itorRoom = m_Rooms.find(roomid);
	if( itorRoom != m_Rooms.end() )
	{
		return itorRoom->second;
	}

	return NULL;
}

int CServer::GetTotalPlayerCount()
{
	return int(m_Players.size() + m_BotPlayerInIdle.size() + m_BotPlayerInUse.size());
}

int CServer::OnPlayerConnect( GameServerSocket* pclient,CRecvMsgPacket& msgPack )
{
	TraceStackPath logTP("CServer::OnPlayerConnect");
	CLogFuncTime lft(m_FuncTime,"OnPlayerConnect");
	
	DebugInfo("CServer::OnPlayerConnect start TickCount=%d",GetTickCount());

	GameLobbyPlayerConnect MsgPC;
	TransplainMsg(msgPack,MsgPC);
	MapClientSocket::iterator itorLgSocket = m_LoginSockets.find( pclient->GetConnect() );
	if ( itorLgSocket != m_LoginSockets.end() && MsgPC.m_PID > 0 )
	{
		if ( MsgPC.m_PlayerType == PLAYER_TYPE_PLAYER )
		{
			pclient->m_LoginTime = ::GetTickCount();

			if ( MsgPC.m_LoginType == Login_Type_Relink )
			{
				GameLobbyRespPlayerConnect RespPC;
				RespPC.m_AID = MsgPC.m_AID;
				RespPC.m_PID = MsgPC.m_PID;
				RespPC.m_Flag = RespPC.Failed;

				MapPlayer::iterator itorPlayer = m_Players.find( MsgPC.m_PID );
				if( itorPlayer != m_Players.end() )
				{
					DebugInfo("Relink 找到老玩家：PID=%d",MsgPC.m_PID);

					PlayerPtr pPlayer = itorPlayer->second;
					if ( MsgPC.m_strSession == pPlayer->m_GSSessionKey )
					{
						if ( pPlayer->IsDead() )
						{
							pPlayer->SetPlayerState(PLAYER_ST_LOBBY);
						}

						pPlayer->SetServer(this);
						pPlayer->AttachSocket( pclient );
						m_Clients.insert(make_pair(itorLgSocket->first,itorLgSocket->second));
						m_LoginSockets.erase(itorLgSocket);

						string strGUID;
						Tool::GetGUID(strGUID);
						RespPC.m_Flag = RespPC.SUCCESS;
						RespPC.m_strSession = strGUID;
						pPlayer->SendMsg(RespPC);

						pPlayer->m_GSSessionKey = strGUID;

						pPlayer->SendPlayerData();

						//pPlayer->SendPlayerDataEx();


						//SendRoomInfoListToPlayer( pPlayer );

						//SendPlayerFollow(pPlayer);
						//SendPlayerBeFollow(pPlayer);

						if( pPlayer->GetRoomID()>0 )
						{
							assert(pPlayer->GetRoom());
							if ( CGameRoom* pRoom=GetRoomByID(pPlayer->GetRoomID()) )
							{
								assert(pPlayer->GetRoom()==pRoom);
								pRoom->AttachPlayerRoom( pPlayer );
							}
						}
						else
						{
							assert(!pPlayer->GetRoom());
						}						
					}					
				}
				else
				{
					RespPC.m_Flag = RespPC.NOPLAYER;					
				}

				if ( RespPC.m_Flag != RespPC.SUCCESS )
				{
					pclient->SendMsg(RespPC);
					pclient->Stop(m_CurTime);
				}

				UINT32 UsedTicket = UINT32(::GetTickCount()) - pclient->m_LoginTime;
				if ( UsedTicket > 100 )
				{
					DebugLogOut("Login PID=%d Flag=%d Type=%d UsedTicket=%d",MsgPC.m_PID,RespPC.m_Flag,MsgPC.m_LoginType,UsedTicket );
				}				
			}
			else if( MsgPC.m_LoginType == Login_Type_Session
				     || MsgPC.m_LoginType == Login_Type_CeShi
					 || MsgPC.m_LoginType == Login_Type_UserName
					 || MsgPC.m_LoginType == Login_Type_HardCode )
			{
				if ( m_pDBSocket->CanUse() )
				{
					bool bFindPlayerData = false;
					MapPlayer::iterator itorPlayer = m_Players.find( MsgPC.m_PID );
					if ( itorPlayer != m_Players.end() )
					{
						bFindPlayerData = true;
						if ( itorPlayer->second->IsDead() )
						{
							itorPlayer->second->m_KillTime = m_CurTime;
						}
					}

					UINT32 TempPID = MsgPC.m_PID;
					UINT32 TempSocketNum = reinterpret_cast<UINT32>(pclient->GetConnect());
					UINT64 TempMicroSecond = Tool::GetMicroSecond();
					string strKey = N2S(MsgPC.m_PID)+"_"+N2S(TempMicroSecond)+"_"+N2S(TempSocketNum);
					strKey = Tool::GetMD5(strKey.c_str(),int(strKey.size()));

					itorLgSocket->second->SetSocketPID(MsgPC.m_PID);
					itorLgSocket->second->SetSessionKey(strKey);

					DBServerXY::DBS_ReqServerPlayerLogin spl;

					spl.m_AID              = MsgPC.m_AID;
					spl.m_PID              = MsgPC.m_PID;
					spl.m_ClientType       = MsgPC.m_ClientType;
					spl.m_LoginType        = MsgPC.m_LoginType;
					spl.m_PlayerType       = MsgPC.m_PlayerType;
					spl.m_Socket           = TempSocketNum;
					spl.m_strLoginKey      = strKey;
					spl.m_IP               = pclient->GetPeerStringIp();
					spl.m_UserName         = MsgPC.m_UserName;
					spl.m_Password         = MsgPC.m_Password;
					spl.m_HardCode         = MsgPC.m_HardCode;
					if ( bFindPlayerData ) spl.m_HaveData = 1;

					string str    = Tool::N2S(MsgPC.m_AID)+"|"+Tool::N2S(MsgPC.m_PID)+"|"+MsgPC.m_strSession+"|Thirteen@u";
					string TempSS = Tool::GetMD5(str.c_str(),int(str.size()));
					spl.m_strSession          = TempSS;

					DebugLogOut("PlayerConnect PID=%d %s %s",MsgPC.m_PID,str.c_str(),TempSS.c_str());

					m_pDBSocket->SendMsg(spl);
				}
				else
				{
					//itorLgSocket->second->Close();
					return MSG_ERROR_DBNOTUSED;
				}
			}
			else
			{
				return MSG_ERROR_LOGIC;
			}
		}
	}
	else
	{
		DebugError("PlayerType=%d ClientType=%d LoginType=%d AID=%d PID=%d Sess=%s",MsgPC.m_PlayerType,MsgPC.m_ClientType,MsgPC.m_LoginType,
		MsgPC.m_AID,MsgPC.m_PID,MsgPC.m_strSession.c_str() );
		//pclient->Close();
		return SOCKET_MSG_ERROR_NOSOCKET;
	}

	DebugInfo("CServer::OnPlayerConnect end");
	return 0;
}

int CServer::OnDBServerMsg(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBServerMsg");
	int ret = 0;
	switch ( msgPack.m_XYID )
	{
	case PublicXY::BatchProtocol::XY_ID:
		{
			ret = OnDBBatchMsg(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespServerPlayerLoginData::XY_ID:
		{
			ret = OnDBPlayerLoginData(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_PlayerData::XY_ID:
		{
			ret = OnDBPlayerData(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_PlayerLimite::XY_ID:
		{
			ret = OnDBPlayerLimite(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_PlayerRight::XY_ID:
		{
			ret = OnDBPlayerRight(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespPlayerInfo::XY_ID:
		{
			ret = OnDBRespPlayerInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespAddBotMoney::XY_ID:
		{
			ret = OnDBRespAddBotMoney(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_UserFriendInfo::XY_ID:
		{
			ret = OnDBFriendInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespPlayerGameMoney::XY_ID:
		{
			ret = OnDBRespPlayerGameMoney(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_To_GS_Flag::XY_ID:
		{
			ret = OnDBSToGSFlag(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_UserAwardInfoList::XY_ID:
		{
			ret = OnUserAwardInfoList(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_UserHuiYuan::XY_ID:
		{
			ret = OnDBUserHuiYuan(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_UserMoguiExchange::XY_ID:
		{
			ret = OnDBMoguiExchange(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_ReqPlayerOnLine::XY_ID:
		{
			ret = OnDBReqPlayerOnLine(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespRoomTableInfo::XY_ID:
		{
			ret = OnDBRespRoomTableInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_BotPlayerData::XY_ID:
		{
			ret = OnBotPlayerInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespBotPlayer::XY_ID:
		{
			ret = OnRespBotPlayerInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RoomInfo::XY_ID:
		{
			ret = OnDBRoomInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_TableInfo::XY_ID:
		{
			ret = OnDBTableInfo(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_RespWinLoss::XY_ID:
		{
			ret = OnDBRespWinLoss(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::WDB_RespWinLossList::XY_ID:
		{
			ret = OnDBRespWinLossList(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_RespServerConnectData::XY_ID:
		{
			ret = OnDBRespServerDate(pDBSocket,msgPack);
		}
		break;
	case DBServerXY::DBS_PlayerMoneyCheck::XY_ID:
		{
			ret = OnDBPlayerMoneyCheck(pDBSocket,msgPack);
		}
		break;
	case MOGUI_XieYiList:
		{
			ret = OnXieYiList(pDBSocket,msgPack);
		}
		break;
	default:
		{			
			DebugError("OnDBServerMsg:无法处理协议 id=%d len=%d",msgPack.m_XYID,msgPack.m_nLen);			
		}
	}

	if ( ret )
	{
		DebugError("OnDBServerMsg ret=%d xyid=%d xylen=%d ",ret,msgPack.m_XYID,msgPack.m_nLen);
	}

	return ret;
}
int CServer::OnXieYiList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnXieYiList");
	CLogFuncTime lft(m_FuncTime,"OnXieYiList");	

	int nRet = 0;

	bistream bis;
	bis.attach(msgPack.m_DataBuf,msgPack.m_nLen);

	try
	{
		short XieYiID;
		bis >> XieYiID;

		switch( XieYiID )
		{
		case DBServerXY::DBS_HonorInfo::XY_ID:
			{
			}
			break;
		case DBServerXY::DBS_GameLevelInfo::XY_ID:
			{

			}
			break;
		default:
			{
				DebugError("OnGSXieYiList 未处理协议  XieYiID=%d",XieYiID);
			}
		}
	}
	catch (...)
	{
		DebugError("OnGSXieYiList");
	}

	return nRet; 
}
int CServer::OnDBBatchMsg(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBBatchMsg");

	int ret = 0;

	PublicXY::BatchProtocol bp;
	TransplainMsg(msgPack,bp);

	switch(bp.m_XYID)
	{
	case DBServerXY::DBS_ReqRoomTableInfo::XY_ID:
		{
			if ( bp.m_Flag == bp.START )
			{
				m_RoomInfoVector.clear();
				m_TableInfoVector.clear();
			}
			else if (bp.m_Flag == bp.END )
			{
				InitServerRooms();
				m_bInitDBInfo = true;
				DebugLogOut("Init Rooms Init DBInfo OK = true");
			}
		}
		break;
	case DBServerXY::DBS_BotPlayerData::XY_ID:
		{
			if ( bp.m_Flag == bp.START )
			{
			}
			else
			{
				DebugLogOut("DBS_BotPlayerData End Batch");

				DBServerXY::DBS_ReqRoomTableInfo rrti;
				rrti.m_ServerID = GetServerID();
				m_pDBSocket->SendMsg(rrti);

				DebugLogOut("ReqRoomTableInfo......");

				m_BotPlayerNumber = int(m_BotPlayerInIdle.size());				
				DebugLogOut("Resp BotPlayer Count=%d",m_BotPlayerNumber);

				int nSize = int(m_BotPlayerInIdle.size());
				if ( nSize >= 10 )
				{
					VecPlayer vecPlayer;
					for (ListPlayer::iterator itorPlayer=m_BotPlayerInIdle.begin();itorPlayer!=m_BotPlayerInIdle.end();itorPlayer++)
					{
						vecPlayer.push_back(*itorPlayer);
					}
					m_BotPlayerInIdle.clear();

					int nLeft,nRight;
					PlayerPtr TempPlayer;
					for ( int i=0;i<nSize*10;i++ )
					{
						nLeft = Tool::CRandom::Random_Int(0,nSize-1);
						nRight = Tool::CRandom::Random_Int(0,nSize-1);
						if ( nLeft != nRight )
						{
							swap(vecPlayer[nLeft],vecPlayer[nRight]);
						}					
					}			
					unique_copy(vecPlayer.begin(),vecPlayer.end(),back_inserter(m_BotPlayerInIdle));					
					DebugLogOut("Rank BotPlayer Count=%d",m_BotPlayerInIdle.size());
				}
			}
		}
		break;
	default:
		break;
	}

	return ret;
}

int CServer::OnDBRespRoomTableInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBRespRoomTableInfo");

	int ret = 0;

	DBServerXY::DBS_RespRoomTableInfo rspRTI;
	TransplainMsg(msgPack,rspRTI);

	if ( rspRTI.m_Flag == rspRTI.SERVERIDERROR )
	{
		DebugError("OnDBRespRoomTableInfo Error");
	}

	return ret;
}
int CServer::OnDBRoomInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBRoomInfo");

	int ret = 0;

	DBServerXY::DBS_RoomInfo ri;
	TransplainMsg(msgPack,ri);

	m_RoomInfoVector.push_back(ri);

	return ret;
}
int CServer::OnDBTableInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBTableInfo");

	int ret = 0;

	DBServerXY::DBS_TableInfo ti;
	TransplainMsg(msgPack,ti);

	if ( m_TableInfoVector.size() < 10000 )
	{
		m_TableInfoVector.push_back(ti);
	}

	return ret;
}

int CServer::OnDBRespWinLoss(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBRespWinLoss");

	int ret = 0;

	DBServerXY::WDB_RespWinLoss RespWL;
	TransplainMsg(msgPack,RespWL);

	DebugInfo("OnDBRespWinLoss PID=%d",RespWL.m_PID);

	PlayerPtr TempPlayer = GetPlayerByPID(RespWL.m_PID);
	if ( TempPlayer )
	{
		UINT32 nUseSecond = UINT32(time(NULL)) - TempPlayer->GetLockedTime();
		if ( nUseSecond > 1 )
		{
			DebugError("OnDBRespWinLoss PID=%d UsedSecond=%d",RespWL.m_PID,nUseSecond);
		}
		TempPlayer->Unlock();
	}
	else
	{
		DebugError("OnDBRespWinLoss PID=%d",RespWL.m_PID);
	}

	return ret;
}

int CServer::OnDBRespWinLossList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBRespWinLossList");

	int ret = 0;

	DBServerXY::WDB_RespWinLossList RespWL;
	TransplainMsg(msgPack,RespWL);

	for (unsigned int i=0;i<RespWL.m_vecPID.size();++i)
	{
		//DebugInfo("OnDBRespWinLossList PID=%d",RespWL.m_vecPID[i]);

		PlayerPtr TempPlayer = GetPlayerByPID(RespWL.m_vecPID[i]);
		if ( TempPlayer )
		{
			UINT32 nUseSecond = UINT32(time(NULL)) - TempPlayer->GetLockedTime();
			if ( nUseSecond > 1 )
			{
				DebugError("OnDBRespWinLossList PID=%d nUseSecond=%d",TempPlayer->GetPID(),nUseSecond);
			}

			TempPlayer->Unlock();
		}
		else
		{
			DebugError("OnDBRespWinLossList PID=%d",RespWL.m_vecPID[i]);
		}
	}

	return ret;
}

int CServer::OnDBPlayerMoneyCheck(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBPlayerMoneyCheck");
	CLogFuncTime lft(m_FuncTime,"OnDBPlayerMoneyCheck");

	DBServerXY::DBS_PlayerMoneyCheck msgPMC;
	TransplainMsg(msgPack,msgPMC);

	PlayerPtr pPlayer = GetPlayerByPID(msgPMC.m_PID);
	if ( pPlayer )
	{
		DebugInfo("OnDBPlayerMoneyCheck PID=%d",msgPMC.m_PID);
		if ( msgPMC.m_GameMoney != pPlayer->GetGameMoney() )
		{
			DebugError("OnDBPlayerMoneyCheck PID=%d DBGameMoney=%s GSGameMoney=%s LogSize=%d",
				msgPMC.m_PID,N2S(msgPMC.m_GameMoney.get()).c_str(), N2S(pPlayer->GetGameMoney()).c_str(),pPlayer->m_vectorMoneyLog.size() );

			for ( size_t nPos=0;nPos<pPlayer->m_vectorMoneyLog.size();++nPos )
			{
				DebugInfo("%s",pPlayer->m_vectorMoneyLog[nPos].c_str());
			}
		}
		pPlayer->ClearMoneyLog();
	}

	return 0;
}
int CServer::OnBotPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnBotPlayerInfo");
	CLogFuncTime lft(m_FuncTime,"OnBotPlayerInfo");

	int ret = 0;
	
	DBServerXY::DBS_BotPlayerData bpd;
	TransplainMsg(msgPack,bpd);

	PlayerPtr pPlayer = CPlayer::GetOnePlayer();
	if ( pPlayer )
	{
		pPlayer->SetServer(this);
		pPlayer->SetBotPlayerData(bpd);
		pPlayer->SetPlayerState(PLAYER_ST_LOBBY);
		pPlayer->SetPlayerType(PLAYER_TYPE_BOT);

		m_BotPlayerInIdle.push_back(pPlayer);
		if ( pPlayer->GetGameMoney() <= 0 )
		{
			DebugError("机器人游戏币数量不对 PID=%d GameMoney=%s",pPlayer->GetPID(),N2S(pPlayer->GetGameMoney()).c_str());
		}
	}
	else
	{
		DebugError("OnBotPlayerInfo pPlayer=null");
		ret = 1000;
	}
	return ret;
}
int CServer::OnRespBotPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	int ret = 0;

	return ret;
}
int CServer::OnDBRespServerDate(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBRespServerDate");
	CLogFuncTime lft(m_FuncTime,"OnDBRespServerDate");
	DebugInfo("CServer::OnDBRespServerDate Start");

	int ret = 0;

	DBServerXY::DBS_RespServerConnectData rscd;
	TransplainMsg(msgPack,rscd);

	if ( rscd.m_Flag == rscd.SUCCESS )
	{
		m_pDBSocket->SetCanUse(true);

		if ( !m_bInitDBInfo )
		{
			if ( m_pGSConfig->m_cfServer.m_StartPID>0 && m_pGSConfig->m_cfServer.m_EndPID>m_pGSConfig->m_cfServer.m_StartPID )
			{
				DebugLogOut("ReqBotPlayer......");
				DBServerXY::DBS_ReqBotPlayer rbp;
				rbp.m_ServerID = GetServerID();
				rbp.m_StartPlayerNumber   = m_pGSConfig->m_cfServer.m_StartPID;
				rbp.m_EndPlayerNumber     = m_pGSConfig->m_cfServer.m_EndPID;
				m_pDBSocket->SendMsg(rbp);
			}
		}
	}
	else
	{
		pDBSocket->Close();

		if ( rscd.m_Flag == rscd.SERVEREXIST )
		{
			DebugError(" OnDBRespServerDate SERVEREXIST");			
		}
		else if ( rscd.m_Flag == rscd.KEYERROR )
		{
			DebugError(" OnDBRespServerDate KEYERROR");	
		}
	}

	DebugInfo("CServer::OnDBRespServerDate End");
	return ret;
}

int CServer::OnDBPlayerLoginData(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBPlayerLoginData");
	CLogFuncTime lft(m_FuncTime,"OnDBPlayerLoginData");
	UINT32 nStartTicket = ::GetTickCount();
	DebugInfo("CServer::OnDBPlayerLoginData Start Ticket=%d ClientSize=%d,LoginSize=%d",nStartTicket,m_Clients.size(),m_LoginSockets.size());

	DBServerXY::DBS_RespServerPlayerLoginData spld;
	TransplainMsg(msgPack,spld);

	GameLobbyRespPlayerConnect glpc;
	glpc.m_Flag = spld.m_Flag;
	glpc.m_AID  = spld.m_AID;
	glpc.m_PID  = spld.m_PID;

	//DBServerXY::DBS_ReqPlayerInfo msgReqPI;
	//msgReqPI.m_AID = spld.m_AID;
	//msgReqPI.m_PID = spld.m_PID;

	GameServerSocket* pClientSocket = NULL;
	MapClientSocket::iterator itorLoginSocket = m_LoginSockets.find(reinterpret_cast<IConnect*>(spld.m_Socket));
	if ( itorLoginSocket != m_LoginSockets.end() )
	{
		UINT32 TempPID = itorLoginSocket->second->GetSocketPID();
		string strKey  = itorLoginSocket->second->GetSessionKey();
		if ( TempPID == spld.m_PID && strKey == spld.m_SessionKey )
		{
			pClientSocket = itorLoginSocket->second;
		}
		else
		{
			DebugError("Find Socket But PID SessionKey Error PID=%d",TempPID);
		}
	}
	if ( !pClientSocket )
	{
		glpc.m_Flag = glpc.LOGINSOCKET;
		DebugError("Can't Find LoginSocket PID=%d",spld.m_PID);
	}

	bool FindOldPlayer = false;
	PlayerPtr pPlayer;
	if ( glpc.m_Flag == glpc.SUCCESS )
	{
		MapPlayer::iterator itorPlayer = m_Players.find( spld.m_PID );
		if( itorPlayer != m_Players.end() )
		{
			DebugInfo("找到老玩家：PID=%d",spld.m_PID);

			FindOldPlayer = true;
			pPlayer = itorPlayer->second;
			if ( pPlayer->IsDead() )
			{
				pPlayer->SetPlayerState(PLAYER_ST_LOBBY);
			}
		}
		else
		{
			FindOldPlayer = false;
			pPlayer = CPlayer::GetOnePlayer();
			if ( pPlayer )
			{
				DebugInfo("增加新玩家：AID=%d PID=%d",glpc.m_AID,glpc.m_PID);

				pPlayer->SetAID(glpc.m_AID);
				pPlayer->SetPID(glpc.m_PID);
				pPlayer->SetPlayerState(PLAYER_ST_LOBBY);
				pPlayer->m_PlayerCreateTime = m_CurTime;
				GetPlayerUserData( pPlayer );
			}
			else
			{
				glpc.m_Flag = glpc.LOGINERROR;
			}
		}
	}

	if ( glpc.m_Flag == glpc.SUCCESS )
	{
		pPlayer->SetServer(this);
		pPlayer->AttachSocket( pClientSocket );
		m_Clients.insert(make_pair(itorLoginSocket->first,itorLoginSocket->second));
		m_LoginSockets.erase(itorLoginSocket);

		pPlayer->SetSendSameTablePlayer(false);
		pPlayer->SetShowLobby(false);

		string strGUID;
		Tool::GetGUID(strGUID);
		glpc.m_strSession = strGUID;
		pPlayer->SendMsg(glpc);

		pPlayer->m_GSSessionKey = strGUID;		

		Game_MsgRule msgGR;
		msgGR.m_strRule = m_GameRule;
		pPlayer->SendMsg(msgGR);

		if ( FindOldPlayer == false )
		{
			m_Players.insert( make_pair(pPlayer->GetPID(),pPlayer) );
		}

		SendRoomInfoListToPlayer( pPlayer );

		if ( FindOldPlayer )
		{
			pPlayer->SendPlayerData();
			pPlayer->SendPlayerDataEx();

			SendPlayerFollow(pPlayer);
			if ( pPlayer->IsDead() ) //没有DEAD说明没有删除不需要重发
			{
				SendPlayerBeFollow(pPlayer);
			}

			if( pPlayer->GetRoomID()>0 )
			{
				assert(pPlayer->GetRoom());
				if ( CGameRoom* pRoom=GetRoomByID(pPlayer->GetRoomID()) )
				{
					assert(pPlayer->GetRoom()==pRoom);
					pRoom->AttachPlayerRoom( pPlayer );
				}
			}
			else
			{
				assert( !pPlayer->GetRoom() );
			}
		}
	}
	else
	{
		if ( pClientSocket )
		{
			pClientSocket->SendMsg(glpc);
			//pClientSocket->Close();
			pClientSocket->Stop(m_CurTime);
		}
		if ( (!FindOldPlayer) )
		{
			pPlayer = NULL;
		}
	}

	UINT32 nEndTicket = ::GetTickCount();
	UINT32 UsedTicket = nEndTicket - pClientSocket->m_LoginTime;
	if ( !pClientSocket )
		DebugLogOut("OnDBPlayerLoginData PID=%d Flag=%d UsedTicket=%d",glpc.m_PID,glpc.m_Flag,UsedTicket );
	else
		DebugLogOut("OnDBPlayerLoginData PID=%d Flag=%d UsedTicket=%d",glpc.m_PID,glpc.m_Flag,UsedTicket);
	
	DebugInfo("CServer::OnDBPlayerLoginData End TickCount=%d ClientSize=%d,LoginSize=%d",nEndTicket,m_Clients.size(),m_LoginSockets.size());
	return 0;
}

int CServer::OnDBRespPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	DBServerXY::DBS_RespPlayerInfo msgPI;
	TransplainMsg(msgPack,msgPI);

	return 0;
}

int CServer::OnDBRespAddBotMoney(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBRespAddBotMoney");
	DebugInfo("CServer::OnDBRespAddBotMoney Start");	

	DBServerXY::DBS_RespAddBotMoney msgABM;
	TransplainMsg(msgPack,msgABM);

	DebugInfo("PID=%d",msgABM.m_PID);

	PlayerPtr pPlayer = GetPlayerByPID(msgABM.m_PID);
	if ( pPlayer )
	{
		pPlayer->Unlock();
		if ( msgABM.m_Flag==msgABM.SUCCESS )
		{
			pPlayer->AddGameMoney(msgABM.m_nGameMoney);
			pPlayer->AddMoneyLog(msgABM.m_nGameMoney,"OnDBRespAddBotMoney");
		}		
	}
	DebugInfo("CServer::OnDBRespAddBotMoney End");

	return 0;
}

int CServer::OnDBPlayerData(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBPlayerData");
	CLogFuncTime lft(m_FuncTime,"OnDBPlayerData");

	DBServerXY::DBS_PlayerData msgPD;
	TransplainMsg(msgPack,msgPD);

	MapPlayer::iterator itorPlayer = m_Players.find( msgPD.m_PID );
	if ( itorPlayer != m_Players.end() )
	{
		PlayerPtr pPlayer = itorPlayer->second;
		string strCity = pPlayer->GetCity();

		pPlayer->SetPlayerData(msgPD);
		pPlayer->SendPlayerData();

		if( pPlayer->GetRoomID()>0 )
		{
			assert(pPlayer->GetRoom());
			if ( CGameRoom* pRoom=GetRoomByID(pPlayer->GetRoomID()) )
			{
				assert(pPlayer->GetRoom()==pRoom);
				pRoom->AttachPlayerRoom( pPlayer );
			}
		}
		else
		{
			assert(!pPlayer->GetRoom());
		}
		{
			GameServerSocket* pGsSocket = pPlayer->GetSocket();
			if ( pGsSocket )
			{
				UINT32 UsedTicket = UINT32(::GetTickCount()) - pGsSocket->m_LoginTime;
				//if ( UsedTicket > 100 )
				{
					DebugLogOut("OnDBPlayerData PID=%d UsedTicket=%d",pPlayer->GetPID(),UsedTicket);
				}
			}
		}
	}
	else
	{
		DebugError("OnDBPlayerData Can't Find Player AID=%d PID=%d",msgPD.m_AID,msgPD.m_PID);
	}
	return 0;
}

int CServer::OnDBPlayerLimite(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBPlayerLimite");

	DBServerXY::DBS_PlayerLimite msgPL;
	TransplainMsg(msgPack,msgPL);

	MapPlayer::iterator itorPlayer = m_Players.find( msgPL.m_PID );
	if ( itorPlayer != m_Players.end() )
	{
		PlayerPtr pPlayer = itorPlayer->second;
		pPlayer->m_nUpperLimite   += msgPL.m_nUpperLimite;
		pPlayer->m_nLowerLimite   += msgPL.m_nLowerLimite;
	}
	else
	{
		DebugError("OnDBPlayerLimite AID=%d PID=%d",msgPL.m_AID,msgPL.m_PID);
	}

	return 0;
}

int CServer::OnDBPlayerRight(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBPlayerRight");

	DBServerXY::DBS_PlayerRight msgPR;
	TransplainMsg(msgPack,msgPR);

	MapPlayer::iterator itorPlayer = m_Players.find( msgPR.m_PID );
	if ( itorPlayer != m_Players.end() )
	{
		DebugInfo("OnDBPlayerRight PID=%d RightLevel=%d RightTimes=%d",msgPR.m_PID,msgPR.m_Level,msgPR.m_Times);
		itorPlayer->second->m_RightLevel = msgPR.m_Level;
		itorPlayer->second->m_RightTimes = msgPR.m_Times;
	}
	else
	{
		DebugError("OnDBPlayerRight AID=%d PID=%d",msgPR.m_AID,msgPR.m_PID);
	}

	return 0;
}

int CServer::GetPlayerUserData(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::GetPlayerUserData");
	CLogFuncTime lft(m_FuncTime,"GetPlayerUserData");

	bool NewTime = false;
	MemcatchXY::DeZhou::memGSUserData memGSUD;	
	if ( m_memOperator.GetGSUserData(pPlayer->GetAID(),pPlayer->GetPID(),memGSUD) )
	{
		if ( m_CurTime - memGSUD.m_LastActTime > N_Time::Hour_1 * 6 )
		{
			NewTime = true;
		}
		else
		{
			pPlayer->m_TimeStep = memGSUD.m_CurStep;
			pPlayer->m_TimePass = memGSUD.m_PassTime;
		}

		if ( Tool::IsSameDay(memGSUD.m_FirstTimeToday,m_CurTime) )
		{
			pPlayer->m_TodayGameSecond = memGSUD.m_TodayGameSecond;
			pPlayer->m_TodayGameCount  = memGSUD.m_TodayGameCount;
			pPlayer->m_HaveAwardInvite = memGSUD.m_HaveAwardInvite;			
		}
		else
		{
			memGSUD.m_FirstTimeToday   = m_CurTime;
			memGSUD.m_TodayGameSecond  = 0;
			memGSUD.m_TodayGameCount   = 0;
			memGSUD.m_HaveAwardInvite  = 0;
		}
	}
	else
	{
		NewTime = true;
	}

	if ( NewTime )
	{
		pPlayer->m_TimeStep = 1;
		pPlayer->m_TimePass = 0;
	}

	if ( memGSUD.m_FirstTimeToday==0 )
	{
		memGSUD.m_FirstTimeToday = m_CurTime;
	}
	memGSUD.m_LastActTime     = m_CurTime;
	memGSUD.m_CurStep         = pPlayer->m_TimeStep;
	memGSUD.m_PassTime        = pPlayer->m_TimePass;
	m_memOperator.SetGSUserData(memGSUD);

	return 0;
}

int CServer::OnDBReqPlayerOnLine(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBReqPlayerOnLine");
	CLogFuncTime lft(m_FuncTime,"OnDBReqPlayerOnLine");
	DebugInfo("CServer::OnDBReqPlayerOnLine Start");

	DBServerXY::DBS_ReqPlayerOnLine msgReqOnline;
	TransplainMsg(msgPack,msgReqOnline);

	if ( msgReqOnline.m_ServerID == GetServerID() )
	{
		DBServerXY::DBS_RespPlayerOnLine msgRespOnline;
		msgRespOnline.m_ServerID = GetServerID();
		for (vector<UINT32>::iterator itorPID=msgReqOnline.m_vecPID.begin();itorPID!=msgReqOnline.m_vecPID.end();++itorPID)
		{
			UINT32 PID = *itorPID;
			MapPlayer::iterator itorPlayer = m_Players.find(PID);
			if ( itorPlayer == m_Players.end() )
			{
				msgRespOnline.m_vecOutPID.push_back(PID);
			}
			else
			{
				msgRespOnline.m_vecOnlinePID.push_back(PID);
			}
		}
		pDBSocket->SendMsg(msgRespOnline);
	}
	else
	{
		DebugError("OnDBReqPlayerOnLine ");
	}	

	DebugInfo("CServer::OnDBReqPlayerOnLine End");
	return 0;
}
int CServer::OnDBUserHuiYuan(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBUserHuiYuan");
	CLogFuncTime lft(m_FuncTime,"OnDBUserHuiYuan");
	DebugInfo("CServer::OnUserAwardInfoList Start");

	DBServerXY::DBS_UserHuiYuan dbmsgHY;
	TransplainMsg(msgPack,dbmsgHY);

	MapPlayer::iterator itorPlayer = m_Players.find(dbmsgHY.m_PID);
	if ( itorPlayer != m_Players.end() )
	{
		INT64 nAddGameMoney = dbmsgHY.m_AddMoney;
		INT64 nDayMoney     = dbmsgHY.m_DayMoney;

		PlayerPtr pPlayer = itorPlayer->second;
		pPlayer->AddGameMoney(nAddGameMoney);
		pPlayer->AddMoneyLog(nAddGameMoney,"OnDBUserHuiYuan");
		pPlayer->m_VipLevel = dbmsgHY.m_VipLevel;
		pPlayer->m_FreeFaceTime = dbmsgHY.m_FreeFaceTime;

		pPlayer->m_nUpperLimite += nAddGameMoney*10;
		pPlayer->m_nLowerLimite += nAddGameMoney/10;

		GameXY::UserHuiYuan msgYH;
		msgYH.m_PID           = pPlayer->GetPID();
		msgYH.m_nAddMoney     = nAddGameMoney;
		msgYH.m_nDayGivenMoney= nDayMoney;
		msgYH.m_nGameMoney    = pPlayer->GetGameMoney();
		msgYH.m_FreeFaceTime  = pPlayer->m_FreeFaceTime;
		msgYH.m_VipLevel      = pPlayer->m_VipLevel;
		msgYH.m_VipEndTime    = dbmsgHY.m_VipEndTime;
		msgYH.m_ActionTime    = dbmsgHY.m_ActionTime;
		pPlayer->SendMsg(msgYH);

		pPlayer->CheckMaxGameMoney();
	}
	return 0;
}
int CServer::OnDBMoguiExchange(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBMoguiExchange");
	CLogFuncTime lft(m_FuncTime,"OnDBMoguiExchange");
	DebugInfo("CServer::OnUserAwardInfoList Start");

	DBServerXY::DBS_UserMoguiExchange dbmsgUME;
	TransplainMsg(msgPack,dbmsgUME);

	MapPlayer::iterator itorPlayer = m_Players.find(dbmsgUME.m_PID);
	if ( itorPlayer != m_Players.end() )
	{
		int AddMoguiMoney = dbmsgUME.m_AddMoguiMoney;

		PlayerPtr pPlayer = itorPlayer->second;
		pPlayer->AddMoGuiMoney(AddMoguiMoney);
		pPlayer->UpdateMoGuiMoney();

		pPlayer->m_nUpperLimite += INT64(AddMoguiMoney)*10000;
		pPlayer->m_nLowerLimite += INT64(AddMoguiMoney)*1000;

		GameXY::UserMoguiExchange msgUME;
		msgUME.m_PID            = pPlayer->GetPID();
		msgUME.m_nAddMoguiMoney = AddMoguiMoney;
		msgUME.m_nMoguiMoney    = pPlayer->GetMoGuiMoney();
		pPlayer->SendMsg(msgUME);
	}
	return 0;
}
int CServer::OnUserAwardInfoList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnUserAwardInfoList");
	CLogFuncTime lft(m_FuncTime,"OnUserAwardInfoList");
	DebugInfo("CServer::OnUserAwardInfoList Start");

	DBServerXY::DBS_UserAwardInfoList dbmsgUAIL;
	TransplainMsg(msgPack,dbmsgUAIL);

	PlayerPtr pPlayer = GetPlayerByPID(dbmsgUAIL.m_PID);
	if ( pPlayer )
	{
		GameXY::UserAwardInfoList msgRespUAIL;
		msgRespUAIL.m_PID    = dbmsgUAIL.m_PID;
		msgRespUAIL.m_nMoney = dbmsgUAIL.m_Money;
		msgRespUAIL.m_nCount = dbmsgUAIL.m_nCount;

		GameXY::msgUserAward stUA;
		for (UINT32 i=0;i<dbmsgUAIL.m_listAwardInfo.size();++i)
		{
			stUA.m_MoneyFlag = dbmsgUAIL.m_listAwardInfo[i].m_MoneyFlag;
			stUA.m_nMoney    = dbmsgUAIL.m_listAwardInfo[i].m_nMoney;

			msgRespUAIL.m_listAwardInfo.push_back(stUA);
		}
		pPlayer->SendMsg(msgRespUAIL);

		pPlayer->AddGameMoney(dbmsgUAIL.m_Money);
		pPlayer->AddMoneyLog(dbmsgUAIL.m_Money,"OnUserAwardInfoList");
		pPlayer->UpdateGameMoney();
		pPlayer->CheckMaxGameMoney();

		DebugInfo("OnUserAwardInfoList PlayerMoney=%s",Tool::N2S(pPlayer->GetGameMoney()).c_str() );
	}	

	DebugInfo("CServer::OnUserAwardInfoList End");
	return 0;
}

int CServer::OnDBRespPlayerGameMoney(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBRespPlayerGameMoney");
	CLogFuncTime lft(m_FuncTime,"OnDBRespPlayerGameMoney");
	DebugInfo("CServer::OnDBRespPlayerGameMoney Start");

	DBServerXY::DBS_RespPlayerGameMoney msgPGM;
	TransplainMsg(msgPack,msgPGM);

	if ( msgPGM.m_AID>0 && msgPGM.m_PID>0 )
	{
		PlayerPtr pTempPlayer = GetPlayerByPID(msgPGM.m_PID);
		if ( pTempPlayer )
		{
			DebugLog("RespGameMoney AID=%d PID=%d Money=%s",pTempPlayer->GetAID(),pTempPlayer->GetPID(),Tool::N2S(msgPGM.m_nDiffMoney.get()).c_str() );

			pTempPlayer->Unlock();
			if ( msgPGM.m_nDiffMoney && (!pTempPlayer->IsPlayerSitOnTable()) )
			{
				pTempPlayer->AddGameMoney(msgPGM.m_nDiffMoney);
				pTempPlayer->AddMoneyLog(msgPGM.m_nDiffMoney,"OnDBRespPlayerGameMoney");
				pTempPlayer->UpdateGameMoney();
			}
		}
	}

	DebugInfo("CServer::OnDBRespPlayerGameMoney End");
	return 0;
}

int CServer::OnDBSToGSFlag(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBSToGSFlag");
	CLogFuncTime lft(m_FuncTime,"OnDBSToGSFlag");
	DebugInfo("CServer::OnDBSToGSFlag Start");

	DBServerXY::DBS_To_GS_Flag msgGSF;
	TransplainMsg(msgPack,msgGSF);

	switch(msgGSF.m_Flag)
	{
	case msgGSF.SendFriendOver:
		{
			MapPlayer::iterator itorPlayer = m_Players.find(msgGSF.m_PID);
			if ( itorPlayer != m_Players.end() )
			{
				PlayerPtr pPlayer = itorPlayer->second;
				SendPlayerFollow(pPlayer);
				SendPlayerBeFollow(pPlayer);
			}
		}
		break;
	case msgGSF.CheckClientExist:
		{
			MapPlayer::iterator itorPlayer = m_Players.find(msgGSF.m_PID);
			if ( itorPlayer != m_Players.end() )
			{
				PlayerPtr pPlayer = itorPlayer->second;
				GameXY::Game_MsgFlag msgMF;
				msgMF.m_Flag      = msgMF.CheckClientExist;
				msgMF.m_nValue    = msgGSF.m_PID;
				pPlayer->SendMsg(msgMF);
			}
		}
		break;
	default:
		DebugError("OnDBSToGSFlag Flag=%d PID=%d",msgGSF.m_Flag,msgGSF.m_PID);
	}

	DebugInfo("CServer::OnDBSToGSFlag End");
	return 0;
}

int CServer::OnDBFriendInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnDBFriendInfo");
	CLogFuncTime lft(m_FuncTime,"OnDBFriendInfo");
	DebugInfo("CServer::OnDBFriendInfo Start");

	DBServerXY::DBS_UserFriendInfo msgUF;
	TransplainMsg(msgPack,msgUF);

	UINT32 LeftPlayerID = msgUF.m_PID;
	MapPlayer::iterator itorLeftPlayer = m_Players.find(LeftPlayerID);
	if ( itorLeftPlayer != m_Players.end() )
	{
		PlayerPtr pLeftPlayer = itorLeftPlayer->second;
		if ( msgUF.m_Flag == N_Friend::Follow)
		{
			int nFriendCount = int(msgUF.m_vecFriendID.size());
			for ( int nLoop=0;nLoop<nFriendCount;nLoop++ )
			{
				UINT32 RightPlayerID = msgUF.m_vecFriendID.at(nLoop);
				ListPID::iterator itorRightID = find(pLeftPlayer->m_listFollow.begin(),pLeftPlayer->m_listFollow.end(),RightPlayerID);
				if( itorRightID == pLeftPlayer->m_listFollow.end() )
				{
					pLeftPlayer->m_listFollow.push_back(msgUF.m_vecFriendID.at(nLoop));
				}

				MapPlayer::iterator itorRightPlayer = m_Players.find(RightPlayerID);
				if ( itorRightPlayer != m_Players.end() )
				{
					PlayerPtr pRightPlayer = itorRightPlayer->second;
					ListPID::iterator itorLeftID = find(pRightPlayer->m_listBeFollow.begin(),pRightPlayer->m_listBeFollow.end(),LeftPlayerID);
					if (itorLeftID != pRightPlayer->m_listBeFollow.end() )
					{
						pRightPlayer->m_listBeFollow.push_back(LeftPlayerID);
					}					
				}
			}
		}
		else if ( msgUF.m_Flag == N_Friend::BeFollow )
		{
			int nFriendCount = int(msgUF.m_vecFriendID.size());
			for ( int nLoop=0;nLoop<nFriendCount;nLoop++ )
			{
				UINT32 RightPlayerID = msgUF.m_vecFriendID.at(nLoop);
				ListPID::iterator itorRightID = find(pLeftPlayer->m_listBeFollow.begin(),pLeftPlayer->m_listBeFollow.end(),RightPlayerID);
				if( itorRightID == pLeftPlayer->m_listBeFollow.end() )
				{
					pLeftPlayer->m_listBeFollow.push_back(RightPlayerID);
				}

				MapPlayer::iterator itorRightPlayer = m_Players.find(RightPlayerID);
				if (itorRightPlayer != m_Players.end())
				{
					PlayerPtr pRightPlayer = itorRightPlayer->second;
					ListPID::iterator itorLeftID = find(pRightPlayer->m_listFollow.begin(),pRightPlayer->m_listFollow.end(),LeftPlayerID);
					if (itorLeftID != pRightPlayer->m_listFollow.end() )
					{
						pRightPlayer->m_listFollow.push_back(LeftPlayerID);
					}
				}
			}
		}
	}
	return 0;
}
void CServer::SendPlayerFollow(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::SendPlayerFollow");
	CLogFuncTime lft(m_FuncTime,"SendPlayerFollow");
	PlayerBaseInfo msgPBI;
	PlayerStateInfo msgPSI;
	for ( ListPID::iterator itorFID=pPlayer->m_listFollow.begin();itorFID!=pPlayer->m_listFollow.end();++itorFID )
	{
		PlayerPtr TempPlayer = nullptr;
		TempPlayer = GetPlayerByPID(*itorFID,GetPID_Player|GetPID_BotUse);
		
		if ( TempPlayer )
		{
			TempPlayer->GetPlayerBaseInfo(msgPBI);
			msgPBI.m_Flag = msgPBI.Friend;
			pPlayer->SendMsg(msgPBI);

			msgPSI.m_Flag = msgPSI.Friend;
			TempPlayer->GetPlayerStateInfo(msgPSI);
			pPlayer->SendMsg(msgPSI);
		}
	}
}
void CServer::SendPlayerBeFollow(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::SendPlayerBeFollow");
	CLogFuncTime lft(m_FuncTime,"SendPlayerBeFollow");
	PlayerBaseInfo msgPBI;
	pPlayer->GetPlayerBaseInfo(msgPBI);
	msgPBI.m_Flag = msgPBI.Friend;

	PlayerStateInfo msgPSI;
	msgPSI.m_Flag = msgPSI.Friend;
	pPlayer->GetPlayerStateInfo(msgPSI);

	for ( ListPID::iterator itorFID=pPlayer->m_listBeFollow.begin();itorFID!=pPlayer->m_listBeFollow.end();++itorFID )
	{
		UINT32 RightPlayerID = *itorFID;
		MapPlayer::iterator itorRightPlayer = m_Players.find(RightPlayerID);
		if ( itorRightPlayer != m_Players.end() )
		{
			itorRightPlayer->second->SendMsg(msgPBI);
			itorRightPlayer->second->SendMsg(msgPSI);
		}
	}
}
void CServer::OnFriendLand(PlayerPtr pPlayer,PlayerPtr FriendPlayer)
{
	TraceStackPath logTP("CServer::OnFriendLand");
	CLogFuncTime lft(m_FuncTime,"OnFriendLand");
	if ( pPlayer && FriendPlayer )
	{
		PlayerBaseInfo msgPBI;
		FriendPlayer->GetPlayerBaseInfo(msgPBI);
		msgPBI.m_Flag = msgPBI.Friend;
		pPlayer->SendMsg(msgPBI);

		PlayerStateInfo msgPSI;
		msgPSI.m_Flag = msgPSI.Friend;
		FriendPlayer->GetPlayerStateInfo(msgPSI);		
		pPlayer->SendMsg(msgPSI);
	}
}
void CServer::SendReqAddFriendList(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::SendReqAddFriendList");
	CLogFuncTime lft(m_FuncTime,"SendReqAddFriendList");
	if ( pPlayer && pPlayer->m_listReqFriend.size() )
	{
		GameDeZhou_AddFriendInfo msgAFI;
		for (ListReqFriend::iterator itorAFi=pPlayer->m_listReqFriend.begin();itorAFi!=pPlayer->m_listReqFriend.end();++itorAFi)
		{
			if ( itorAFi->m_ReqPID > 0 )
			{
				msgAFI.m_vecReqPID.push_back(itorAFi->m_ReqPID);
			}
		}
		pPlayer->SendMsg(msgAFI);
	}
}
void CServer::DeleteFirendToClient(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::DeleteFirendToClient");
	CLogFuncTime lft(m_FuncTime,"DeleteFirendToClient");
	assert(pPlayer);
	PlayerStateInfo msgPSI;
	msgPSI.m_Flag = msgPSI.Friend;
	pPlayer->GetPlayerStateInfo(msgPSI);
	msgPSI.m_PlayerState = PLAYER_ST_NONE;

	for ( ListPID::iterator itorFID=pPlayer->m_listBeFollow.begin();itorFID!=pPlayer->m_listBeFollow.end();++itorFID )
	{
		UINT32 RightPlayerID = *itorFID;
		MapPlayer::iterator itorRightPlayer = m_Players.find(RightPlayerID);
		if ( itorRightPlayer != m_Players.end() )
		{
			itorRightPlayer->second->SendMsg(msgPSI);
		}
	}
}

void CServer::SendPlayerStateToFengSi(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::SendPlayerStateToFengSi");
	CLogFuncTime lft(m_FuncTime,"SendPlayerStateToFengSi");

	PlayerStateInfo msgPSI;
	msgPSI.m_Flag = msgPSI.Friend;
	pPlayer->GetPlayerStateInfo(msgPSI);	

	for ( ListPID::iterator itorFID=pPlayer->m_listBeFollow.begin();itorFID!=pPlayer->m_listBeFollow.end();++itorFID )
	{
		UINT32 RightPlayerID = *itorFID;
		MapPlayer::iterator itorRightPlayer = m_Players.find(RightPlayerID);
		if ( itorRightPlayer != m_Players.end() )
		{
			itorRightPlayer->second->SendMsg(msgPSI);
		}
	}
}
void CServer::OnTimeUpdateFirendPlayerState()
{
	TraceStackPath logTP("CServer::OnTimeUpdateFirendPlayerState");
	CLogFuncTime lft(m_FuncTime,"OnTimeUpdateFirendPlayerState");

	int nMod = 10;
	int LuckNumber = (m_CheckFirendPlayerTime/N_Time::T_CheckFriendPlayer)%nMod;
	for (MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
	{
		PlayerPtr pPlayer = itorPlayer->second;
		if ( pPlayer->GetPID()%nMod == LuckNumber && pPlayer->IsFriendStateChange() )
		{
			pPlayer->SetFriendStateChange(false);
			if ( pPlayer->IsOnLine() && pPlayer->IsPlayerSitOnTable() )
			{
				SendPlayerStateToFengSi(pPlayer);
			}
		}
	}
}

int CServer::OnServerMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnServerMsg");
	int ret = 0;
	switch ( msgPack.m_XYID )
	{
	case ReqRoomInfo::XY_ID:
		{
			ret = OnReqRoomInfo(pPlayer,msgPack);
		}
		break;	
	case ReqRoomTableInfo::XY_ID:
		{
			ret = OnReqRoomTableInfo(pPlayer,msgPack);
		}
		break;
	case ReqJoinRoom::XY_ID:
		{
			ret = OnReqJoinRoom(pPlayer,msgPack);
		}
		break;
	case ReqLeaveRoom::XY_ID:
		{
			ret = OnReqLeaveRoom(pPlayer,msgPack);
		}
		break;
	case ReqJoinTable::XY_ID:
		{
			ret = OnReqJoinTable(pPlayer,msgPack);
		}
		break;
	case ReqChangeTable::XY_ID:
		{

		}
		break;
	case ReqTailInfo::XY_ID:
		{
			ret = OnReqTailInfo(pPlayer,msgPack);			
		}
		break;
	case GameXY::ChangeUserInfo::XY_ID:
		{
			ret = OnChangeUserInfo(pPlayer,msgPack);
		}
		break;
	case GameXY::ReqPlayerAward::XY_ID:
		{
			ret = OnReqPlayerAward(pPlayer,msgPack);
		}
		break;
	case GameXY::ReqTracePlayer::XY_ID:
		{
			ret = OnReqTracePlayer(pPlayer,msgPack);
		}
		break;
	case GameXY::Game_MsgFlag::XY_ID:
		{
			ret = OnGameFlag(pPlayer,msgPack);
		}
		break;
	case GameXY::Game_Test::XY_ID:
		{
			ret = OnTest(pPlayer,msgPack);
		}
		break;
	default:
		{
			DebugError("OnServerMsg:无法处理协议 XYID=%d XYLen=%d",msgPack.m_XYID,msgPack.m_nLen);
		}
	}
	if ( ret )
	{
		DebugError("OnServerMsg Ret=%d XYID=%d XYLen=%d",ret,msgPack.m_XYID,msgPack.m_nLen);
	}
	return ret;
}

int CServer::SendRoomInfoListToPlayer(PlayerPtr pPlayer)
{
	TraceStackPath logTP("CServer::SendRoomInfoListToPlayer");
	CLogFuncTime lft(m_FuncTime,"SendRoomInfoListToPlayer");
	DebugInfo("CServer::SendRoomInfoListToPlayer start");

	//assert(pPlayer);
	//assert(pPlayer->GetSocket());

	if( pPlayer && pPlayer->GetSocket() )
	{
		int nCount = 0;
		MapRoom::iterator itorRoom;
		for ( itorRoom=m_Rooms.begin();itorRoom!=m_Rooms.end();itorRoom++ )
		{
			if( itorRoom->second->PlayerCanSee(pPlayer) )
			{
				nCount++;
			}
		}

		if( nCount > 0 )
		{
			BatchProtocol bp;
			bp.m_Flag = bp.START;
			bp.m_XYID = RoomInfoList::XY_ID;
			bp.m_nCount = nCount/RoomInfoList::MAX_ROOMLIST;
			if( nCount%RoomInfoList::MAX_ROOMLIST > 0 )
			{
				bp.m_nCount++;
			}
			pPlayer->SendMsg(bp);

			RoomInfo ri;
			RoomInfoList ril;
			int CountK = 0;
			for( itorRoom = m_Rooms.begin();itorRoom != m_Rooms.end();itorRoom++ )
			{
				if( itorRoom->second->PlayerCanSee(pPlayer) )
				{
					if( itorRoom->second->GetRoomInfo(pPlayer,ri) )
					{
						ril.m_RoomList[CountK++] = ri;
					}

					if( CountK >= ril.MAX_ROOMLIST )
					{
						ril.m_nCount = CountK;
						CountK = 0;
						pPlayer->SendMsg(ril);
						ril.ReSet();
					}
				}				
			}
			if( CountK > 0 )
			{
				ril.m_nCount = CountK;
				CountK = 0;
				pPlayer->SendMsg(ril);
			}

			bp.m_Flag = bp.END;
			bp.m_nCount = 0;
			pPlayer->SendMsg(bp);
		}
	}
	else
	{
		DebugError("SendRoomInfoListToPlayer PID=%d",pPlayer->GetPID());
	}

	DebugInfo("CServer::SendRoomInfoListToPlayer end");
	return 0;
}

int CServer::OnReqRoomInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqRoomInfo");
	CLogFuncTime lft(m_FuncTime,"OnReqRoomInfo");
	DebugInfo("CServer::OnReqRoomInfo start");

	GameXY::ReqRoomInfo rri;
	TransplainMsg(msgPack,rri);

	if( rri.m_RoomID == 0 )
	{
		DebugError("OnReqRoomInfo RoomID=%d", rri.m_RoomID);		
	}
	else
	{
		CGameRoom* pRoom = GetRoomByID(rri.m_RoomID);
		if( pRoom )
		{
			GameXY::RespRoomInfo rsri;
			GameXY::RoomInfo ri;

			if( pRoom->PlayerCanSee(pPlayer) )
			{
				pRoom->GetRoomInfo(ri);
				rsri.m_Flag = GameXY::RespRoomInfo::SUCCESS;
				pPlayer->SendMsg(rsri);

				pPlayer->SendMsg(ri);
			}
			else
			{
				rsri.m_Flag = GameXY::RespRoomInfo::NORIGHT;
				pPlayer->SendMsg(rsri);
			}
		}
		else
		{
			GameXY::RespRoomInfo rsri;
			rsri.m_Flag = GameXY::RespRoomInfo::NOROOM;
			pPlayer->SendMsg(rsri);
		}
	}

	DebugInfo("CServer::OnReqRoomInfo end");
	return 0;
}
int CServer::OnReqJoinRoom(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqJoinRoom");
	CLogFuncTime lft(m_FuncTime,"OnReqRoomInfo");
	DebugInfo("CServer::OnReqJoinRoom start ReqPID=%d",pPlayer->GetPID());

	GameXY::ReqJoinRoom rjr;
	TransplainMsg(msgPack,rjr);

	if ( rjr.m_ActionFlag == rjr.Action_Common )
	{
		GameXY::RespJoinRoom rsjr;
		if( pPlayer->GetRoomID() == 0 )
		{
			assert(!pPlayer->GetRoom());
			rsjr.ReSet();
			rsjr.m_Type = rsjr.JOIN;
			rsjr.m_RoomID = rjr.m_RoomID;

			CGameRoom* pRoom = GetRoomByID( rjr.m_RoomID );
			if( pRoom )
			{
				if( pRoom->IsRoomPlayerFull() )
				{
					rsjr.m_Flag     = rsjr.PLAYERFULL;
					rsjr.m_ShowFlag = ShowMsg_Show;
					rsjr.m_strMsg   = "本房间人数已满，请选择其它房间游戏";
				}
				else if( pRoom->IsNeedPassword() && (!pRoom->CheckPassword(string(rjr.m_Password))) )
				{			
					rsjr.m_Flag     = rsjr.PASSWORDERROR;
					rsjr.m_ShowFlag = ShowMsg_Show;
					rsjr.m_strMsg   = "房间密码错误";
				}
				else
				{
					rsjr.m_Flag = rsjr.SUCCESS;
					rsjr.m_RoomID = rjr.m_RoomID;
					rsjr.m_Type = rsjr.JOIN;
				}
			}
			else
			{
				rsjr.m_Flag = rsjr.NOROOM;
				rsjr.m_ShowFlag = ShowMsg_Show;
				rsjr.m_strMsg   = "您选择的房间不存在";
			}

			pPlayer->SendMsg(rsjr);

			if( rsjr.m_Flag == rsjr.SUCCESS )
			{
				pRoom = GetRoomByID( rjr.m_RoomID );
				if( pRoom )
				{
					pRoom->PlayerJoinRoom(pPlayer);
				}
			}
		}
		else
		{
			rsjr.ReSet();
			rsjr.m_Type = rsjr.CHANGE;
			rsjr.m_RoomID = rjr.m_RoomID;

			if( pPlayer->GetRoomID() == rjr.m_RoomID )
			{
				rsjr.m_Flag     = rsjr.AREADYINROOM;
				rsjr.m_ShowFlag = ShowMsg_None;
				rsjr.m_strMsg   = "您已经在当前房间里面了";
			}
			else
			{
				if( pPlayer->IsPlaying() )
				{
					rsjr.m_Flag = rsjr.PLAYERINPLAYING;
				}

				CGameRoom* pRoom = GetRoomByID( rjr.m_RoomID );
				if( pRoom )
				{
					if( pRoom->IsRoomPlayerFull() )
					{
						rsjr.m_Flag     = rsjr.PLAYERFULL;
						rsjr.m_ShowFlag = ShowMsg_Show;
						rsjr.m_strMsg   = "本房间人数已满，请选择其它房间游戏";
					}
					else if( pRoom->IsNeedPassword() && !pRoom->CheckPassword(string(rjr.m_Password)) )
					{			
						rsjr.m_Flag     = rsjr.PASSWORDERROR;
						rsjr.m_ShowFlag = ShowMsg_Show;
						rsjr.m_strMsg   = "房间密码错误";
					}
					else
					{
						rsjr.m_Flag = rsjr.SUCCESS;
						rsjr.m_RoomID = rjr.m_RoomID;
						rsjr.m_Type = rsjr.CHANGE;
					}
				}
				else
				{
					rsjr.m_Flag     = rsjr.NOROOM;
					rsjr.m_ShowFlag = ShowMsg_Show;
					rsjr.m_strMsg   = "您选择的房间不存在";
				}			
			}

			pPlayer->SendMsg(rsjr);

			if( rsjr.m_Flag == rsjr.SUCCESS )
			{
				CGameRoom* pRoom = GetRoomByID( pPlayer->GetRoomID() );
				if( pRoom )
				{
					assert(pPlayer->GetRoom() == pRoom);
					pRoom->PlayerLeaveRoom(pPlayer);				
				}
				else
				{
					assert( pPlayer->GetRoomID()==0 && (!pPlayer->GetRoom()) );
				}

				pRoom = GetRoomByID( rjr.m_RoomID );
				if( pRoom )
				{
					pRoom->PlayerJoinRoom(pPlayer);
				}
			}
		}
	}
	else if ( rjr.m_ActionFlag == rjr.Action_AutoSit )
	{
		bool bSuccess = false;
		CGameRoom* pReqRoom = GetRoomByID(rjr.m_RoomID);

		GameXY::RespJoinRoom rsjr;
		if ( pPlayer->GetRoomID()>0 )
		{
			if ( pPlayer->GetRoomID()==rjr.m_RoomID )
			{
				rsjr.m_Flag = rsjr.AREADYINROOM;			
			}
			else
			{
				pPlayer->GetRoom()->PlayerLeaveRoom(pPlayer);
			}
		}
		if ( pPlayer->GetRoomID()==0 && rsjr.m_RoomID>0 )
		{			
			if ( pReqRoom )
			{
				int nFlag = pReqRoom->PlayerCanJoinRoom(pPlayer);
				if ( nFlag == 0 )
				{
					bSuccess = true;
				}
				else
				{
					rsjr.m_Flag = nFlag;
				}				
			}
		}
		if ( bSuccess )
		{
			pReqRoom->PlayerJoinRoom(pPlayer);
			pReqRoom->PlayerChangeTable(pPlayer);
		}
		else
		{
			pPlayer->SendMsg(rsjr);
		}
	}
	else
	{
		DebugError("OnReqJoinRoom PID=%d ActionFlag=%d",pPlayer->GetPID(),rjr.m_ActionFlag);
	}	

	DebugInfo("CServer::OnReqJoinRoom end");
	return 0;
}

int CServer::OnReqLeaveRoom(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqLeaveRoom");
	CLogFuncTime lft(m_FuncTime,"OnReqLeaveRoom");
	DebugInfo("CServer::OnReqLeaveRoom start");

	GameXY::ReqLeaveRoom rlr;
	TransplainMsg(msgPack,rlr);

	if ( pPlayer->GetRoomID() && pPlayer->GetRoomID()== rlr.m_RoomID )
	{
		pPlayer->GetRoom()->PlayerLeaveRoom(pPlayer);
	}

	DebugInfo("CServer::OnReqLeaveRoom end");
	return 0;
}

int CServer::OnReqRoomTableInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqRoomTableInfo");
	CLogFuncTime lft(m_FuncTime,"OnReqRoomTableInfo");
	DebugInfo("CServer::OnReqRoomTableInfo start");

	GameXY::ReqRoomTableInfo rrti;
	TransplainMsg(msgPack,rrti);

	GameXY::RespRoomTableInfo rsrti;
	CGameRoom* pRoom = GetRoomByID(rrti.m_RoomID);
	if( pRoom )
	{
		if( pRoom->PlayerCanSee(pPlayer) )
		{
			if ( pPlayer->HaveSendTableInfoList(rrti.m_RoomID) )
			{
				rsrti.m_Flag = rsrti.REQAGAIN;
			}
			else
			{
				if ( pPlayer->IsShowLobby() == false )
				{
					Game_MsgFlag msgMF;
					msgMF.m_Flag = msgMF.LobbyReady;
					pPlayer->SendMsg(msgMF);

					pPlayer->SetShowLobby(true);
				}
			}
		}
		else
		{
			rsrti.m_Flag = rsrti.NORIGHT;
		}
	}
	else
	{
		rsrti.m_Flag = rsrti.NOROOM;
	}

	if ( rsrti.m_Flag != rsrti.SUCCESS )
	{
		pPlayer->SendMsg(rsrti);
	}

	DebugInfo("CServer::OnReqRoomTableInfo end");
	return 0;
}

int CServer::OnReqJoinTable(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqJoinTable");
	CLogFuncTime lft(m_FuncTime,"OnReqJoinTable");
	DebugInfo("CServer::onReqJoinTable start ReqPID=%d",pPlayer->GetPID());

	GameXY::ReqJoinTable rjt;
	TransplainMsg(msgPack,rjt);

	GameXY::RespJoinTable rsjt;
	if ( pPlayer->IsPlaying() )
	{
		rsjt.m_Flag      = rsjt.PLAYERINPLAYING;
		rsjt.m_ShowFlag  = ShowMsg_Show;
		rsjt.m_strMsg    = "您正游戏中";
	}
	else
	{
		CGameRoom* pRoom = GetRoomByID(rjt.m_RoomID);
		if ( pRoom )
		{
			if ( rjt.m_RoomID != pPlayer->GetRoomID() )
			{
				if ( pPlayer->GetRoomID() > 0 )
				{
					assert(pPlayer->GetRoom());
					CGameRoom* pOldRoom = GetRoomByID(pPlayer->GetRoomID());
					if ( pOldRoom )
					{
						assert(pOldRoom==pPlayer->GetRoom());
						pOldRoom->PlayerLeaveRoom(pPlayer);
					}
				}
				else
				{
					assert(pPlayer->GetRoomID()==0 && (!pPlayer->GetRoom()) );
				}
				pRoom->PlayerJoinRoom(pPlayer);
			}
			else
			{
				if ( rjt.m_TableID == pPlayer->GetTableID() )
				{
					rsjt.m_Flag = rsjt.AREADYINTABLE;
					rsjt.m_ShowFlag  = ShowMsg_Text;
					rsjt.m_strMsg    = "您已经在桌子里面了";
				}
			}
			if( rsjt.m_Flag == rsjt.SUCCESS )
			{
				pRoom->OnReqJoinTable(pPlayer,rjt);
			}			
		}
		else
		{
			rsjt.m_Flag = rsjt.NOROOM;
			rsjt.m_ShowFlag  = ShowMsg_Show;
			rsjt.m_strMsg    = "找不到您要加入的房间";
		}
	}

	if(rsjt.m_Flag != rsjt.SUCCESS )
	{
		pPlayer->SendMsg(rsjt);
	}

	DebugInfo("CServer::onReqJoinTable end");
	return 0;
}

int CServer::OnReqChangeTable(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqChangeTable");
	CLogFuncTime lft(m_FuncTime,"OnReqChangeTable");
	DebugInfo("CServer::OnReqChangeTable start ReqPID=%d",pPlayer->GetPID());

	GameXY::ReqChangeTable rct;
	TransplainMsg(msgPack,rct);

	GameXY::RespChangeTable rsct;
	if ( pPlayer->IsPlaying() )
	{
		rsct.m_Flag      = rsct.Playing;
	}
	else
	{
		if ( pPlayer->GetRoomID() )
		{
			CGameRoom* pRoom = GetRoomByID(rct.m_RoomID);
			if ( pRoom && pRoom==pPlayer->GetRoom() && rct.m_RoomID==pPlayer->GetRoomID() )
			{
				pRoom->PlayerChangeTable(pPlayer);
			}
		}
		else
		{
			rsct.m_Flag = rsct.NotInRoom;
		}
	}

	if(rsct.m_Flag != rsct.Success )
	{
		pPlayer->SendMsg(rsct);
	}

	DebugInfo("CServer::onReqJoinTable end");
	return 0;
}

int CServer::OnReqTailInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqTailInfo");
	CLogFuncTime lft(m_FuncTime,"OnReqTailInfo");
	DebugInfo("CServer::OnReqTailInfo start");

	GameXY::ReqTailInfo msgTI;
	TransplainMsg(msgPack,msgTI);

	if ( (msgTI.m_Flag&msgTI.TailInfo) == msgTI.TailInfo )
	{
		PlayerPtr pTempPlayer = GetPlayerByPID(msgTI.m_nValue);
		if ( pTempPlayer && pPlayer )
		{
			GameXY::PlayerTailInfo msgPTI;
			pTempPlayer->GetPlayerTailInfo(msgPTI);
			pPlayer->SendMsg(msgPTI);
		}
	}

	if ( (msgTI.m_Flag&msgTI.GiftInfo) == msgTI.GiftInfo )
	{
		PlayerPtr TempPlayer = GetPlayerByPID(msgTI.m_nValue);
		if ( TempPlayer && pPlayer )
		{
		}
	}

	DebugInfo("CServer::OnReqTailInfo End");
	return 0;
}

int CServer::OnReqPlayerAward( PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqPlayerAward");
	CLogFuncTime lft(m_FuncTime,"OnReqPlayerAward");

	//DebugInfo("CServer::OnReqPlayerAward Start");

	//GameXY::ReqPlayerAward msgRPA;
	//TransplainMsg(msgPack,msgRPA);

	//if ( msgRPA.m_AID>0 && msgRPA.m_PID>0 && pPlayer->GetPID()==msgRPA.m_PID && pPlayer->IsOnLine() )
	//{
	//	DBServerXY::DBS_ReqPlayerAward dbs_msgPA;
	//	dbs_msgPA.m_AID     = msgRPA.m_AID;
	//	dbs_msgPA.m_PID     = msgRPA.m_PID;
	//	SendMsgToDBServer(dbs_msgPA);
	//}

	//DebugInfo("CServer::OnReqPlayerAward End");

	return 0;
}

int CServer::OnChangeUserInfo( PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnChangeUserInfo");
	CLogFuncTime lft(m_FuncTime,"OnChangeUserInfo");
	DebugInfo("CServer::OnChangeUserInfo Start");

	GameXY::ChangeUserInfo msgCN;
	TransplainMsg(msgPack,msgCN);

	if ( pPlayer && pPlayer->GetPID()==msgCN.m_PID )
	{
		PlayerPtr TempPlayer = GetPlayerByPID(pPlayer->GetPID());
		if ( pPlayer == TempPlayer && pPlayer->m_ChangeName>0 )
		{
			GameXY::RespChangeUserInfo msgRespCUI;
			msgRespCUI.m_PID       = msgCN.m_PID;

			if ( (pPlayer->m_ChangeName&ChangeInfo_NickName) == ChangeInfo_NickName )
			{
				if ( msgCN.m_NickName.size() == 0 )
				{
					msgRespCUI.m_Flag = msgRespCUI.Error_NickName;
				}
			}
			if ( (pPlayer->m_ChangeName&ChangeInfo_City) == ChangeInfo_City )
			{
				if ( msgCN.m_City.size() == 0 )
				{
					msgRespCUI.m_Flag = msgRespCUI.Error_City;
				}
			}
			if ( (pPlayer->m_ChangeName&ChangeInfo_HeadPic) == ChangeInfo_HeadPic )
			{
				if ( msgCN.m_HeadPicUrl.size() == 0 )
				{
					msgRespCUI.m_Flag = msgRespCUI.Error_HeadPicUrl;
				}
			}

			if ( msgRespCUI.m_Flag == msgRespCUI.SUCCESS )
			{
				DBServerXY::WDB_ChangeUserInfo dbmsgCN;
				dbmsgCN.m_AID            = pPlayer->GetAID();
				dbmsgCN.m_PID            = pPlayer->GetPID();
				dbmsgCN.m_Sex            = pPlayer->m_Sex;
				dbmsgCN.m_NickName       = pPlayer->m_NickName;
				dbmsgCN.m_HeadPicUrl     = pPlayer->GetHeadPicURL();
				dbmsgCN.m_City           = pPlayer->GetCity();

				msgRespCUI.m_Sex         = pPlayer->m_Sex;
				msgRespCUI.m_NickName    = pPlayer->m_NickName;
				msgRespCUI.m_HeadPicUrl  = pPlayer->GetHeadPicURL();
				msgRespCUI.m_City        = pPlayer->GetCity();				

				if ( (pPlayer->m_ChangeName&ChangeInfo_NickName) == ChangeInfo_NickName )
				{
					pPlayer->m_NickName   = msgCN.m_NickName;
					msgRespCUI.m_NickName = msgCN.m_NickName;
					dbmsgCN.m_NickName    = msgCN.m_NickName;
				}

				if ( (pPlayer->m_ChangeName&ChangeInfo_Sex) == ChangeInfo_Sex )
				{
					msgCN.m_Sex = max(msgCN.m_Sex,Sex_None);
					msgCN.m_Sex = min(msgCN.m_Sex,Sex_FEMAIL);

					pPlayer->m_Sex      = msgCN.m_Sex;
					msgRespCUI.m_Sex    = msgCN.m_Sex;
					dbmsgCN.m_Sex       = msgCN.m_Sex;
				}

				if ( (pPlayer->m_ChangeName&ChangeInfo_City) == ChangeInfo_City )
				{
					pPlayer->SetCity(msgCN.m_City);

					msgRespCUI.m_City      = msgCN.m_City;
					dbmsgCN.m_City         = msgCN.m_City;
				}

				pPlayer->m_ChangeName = 0;

				SendMsgToDBServer(dbmsgCN);

				INT64 DengJiAward = 50000;
				pPlayer->AddGameMoney(DengJiAward);
				pPlayer->UpdateGameMoney();

				UserAwardInfoList msgUAIL;
				msgUserAward stUA;
				stUA.m_nMoney      = DengJiAward;
				stUA.m_MoneyFlag   = N_IncomeAndPay::UpdateZiLiao;
				msgUAIL.m_PID      = pPlayer->GetPID();
				msgUAIL.m_nMoney   = DengJiAward;
				msgUAIL.m_listAwardInfo.push_back(stUA);
				pPlayer->SendMsg(msgUAIL);

				DBServerXY::WDB_IncomeAndPay msgIP;
				msgIP.m_AID         = pPlayer->GetAID();
				msgIP.m_PID         = pPlayer->GetPID();
				msgIP.m_nMoney      = DengJiAward;
				msgIP.m_Flag        = N_IncomeAndPay::UpdateZiLiao;
				SendMsgToDBServer(msgIP);
			}
			pPlayer->SendMsg(msgRespCUI);			
		}
		else
		{
			DebugError("OnChangeUserInfo PID=%d",pPlayer->GetPID());
		}
	}

	DebugInfo("CServer::OnChangeUserInfo End");
	return 0;
}

int CServer::OnGameFlag( PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnGameFlag");
	GameXY::Game_MsgFlag msgMF;
	TransplainMsg(msgPack,msgMF);

	switch(msgMF.m_Flag)
	{
	case msgMF.CheckClientExist:
		{
			if ( pPlayer->GetPID() == msgMF.m_nValue )
			{
				DBServerXY::GS_To_DBS_Flag msgGSF;
				msgGSF.m_AID     = pPlayer->GetAID();
				msgGSF.m_PID     = pPlayer->GetPID();
				msgGSF.m_Flag    = msgGSF.CheckClientExist;
				msgGSF.m_nValue  = pPlayer->GetPID();
				SendMsgToDBServer(msgGSF);
			}
			else
			{
				DebugError("CheckClientExist GetPID=%d Value=%d",pPlayer->GetPID(),msgMF.m_nValue);
			}
		}
		break;
	default:
		DebugError("OnGameFlag Flag=%d Value=%d",msgMF.m_Flag,msgMF.m_nValue);
	}

	return 0;
}

int CServer::OnTest(PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	if ( m_pGSConfig->CeshiMode() )
	{
		GameXY::Game_Test msgTest;
		TransplainMsg(msgPack,msgTest);
	}
	return 0;
}

int CServer::OnReqTracePlayer( PlayerPtr pPlayer,CRecvMsgPacket& msgPack)
{
	TraceStackPath logTP("CServer::OnReqTracePlayer");
	CLogFuncTime lft(m_FuncTime,"OnReqTracePlayer");
	
	GameXY::ReqTracePlayer msgTP;
	TransplainMsg(msgPack,msgTP);

	DebugInfo("OnReqTracePlayer Start PID=%d BeTracePID=%d",pPlayer->GetPID(),msgTP.m_PID);

	GameXY::RespTracePlayer msgRespTP;
	msgRespTP.m_Flag = msgRespTP.Success;

	PlayerPtr pTempPlayer = GetPlayerByPID(msgTP.m_PID);

	if ( pTempPlayer && pTempPlayer!=pPlayer 
		&& !(pPlayer->GetRoomID()==pTempPlayer->GetRoomID() && pPlayer->GetTableID()==pTempPlayer->GetTableID()) )
	{
		CGameRoom* pRoom = pTempPlayer->GetRoom();
		CGameTable* pTable = pTempPlayer->GetTable();
		if ( !pRoom )
		{
			msgRespTP.m_Flag     = msgRespTP.NotInRoom;
			msgRespTP.m_ShowFlag = ShowMsg_Show;
			msgRespTP.m_strMsg   = "玩家不在游戏房间里面";
		}
		else if ( !pTable )
		{
			msgRespTP.m_Flag     = msgRespTP.NotInTable;
			msgRespTP.m_ShowFlag = ShowMsg_Show;
			msgRespTP.m_strMsg   = "玩家不在游戏桌里面";
		}

		if ( msgRespTP.m_Flag == msgRespTP.Success )
		{
			int DestRoomID = pTempPlayer->GetRoomID();
			int DestTableID = pTempPlayer->GetTableID();

			if ( DestRoomID>0 && DestTableID>0 )
			{
				CGameRoom* pNewRoom = nullptr;
				CGameTable* pNewTable = nullptr;

				pNewRoom = GetRoomByID(DestRoomID);
				if ( pNewRoom )
				{
					pNewTable = pNewRoom->GetTableByID(DestTableID);
				}

				if ( pNewRoom && pNewTable )
				{
					if ( DestRoomID != pPlayer->GetRoomID() && pPlayer->GetRoomID()>0 )
					{
						assert(pPlayer->GetRoom());
						CGameRoom* pOldRoom = GetRoomByID(pPlayer->GetRoomID());
						if ( pOldRoom )
						{
							assert(pOldRoom == pPlayer->GetRoom());
							pOldRoom->PlayerLeaveRoom(pPlayer);
						}
						else
						{
							DebugError("OnReqTracePlayer Trace RoomID=%d",pPlayer->GetRoomID());
						}
					}

					if ( pPlayer->GetRoom() != pNewRoom )
					{
						pNewRoom->PlayerJoinRoom(pPlayer);
					}
					if ( pPlayer->GetRoom()==pNewRoom && pPlayer->GetTable()!=pNewTable && pPlayer->GetTableID()>0 )
					{
						CGameTable* pOldTable = pNewRoom->GetTableByID(pPlayer->GetTableID());
						if ( pOldTable )
						{
							assert(pOldTable == pPlayer->GetTable());
							pOldTable->PlayerLeaveTable(pPlayer);
						}
					}
					pNewTable->DoPlayerJoinTable(pPlayer);
				}
			}
		}
	}


	if ( msgRespTP.m_Flag != msgRespTP.Success)
	{
		if ( msgRespTP.m_strMsg == "" )
		{
			msgRespTP.m_ShowFlag = ShowMsg_None;
			msgRespTP.m_strMsg   = "玩家不在游戏房间里面";
		}

		pPlayer->SendMsg(msgRespTP);

		if ( pTempPlayer )
		{
			GameXY::PlayerStateInfo msgPS;
			msgPS.m_Flag = msgTP.m_Flag;
			pTempPlayer->GetPlayerStateInfo(msgPS);
			pPlayer->SendMsg(msgPS);
		}		
	}

	DebugInfo("OnReqTracePlayer End");
	return 0;
}
