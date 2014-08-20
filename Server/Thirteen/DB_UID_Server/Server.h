#pragma once

#include "Base.h"

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "GameData.h"
#include "dbserversocket.h"
#include "DBServerXY.h"
#include "CDBSConfig.h"
#include "RWDBMsg.h"
#include "memoperator.h"
#include "DBReadOperator.h"

using namespace AGBase;
using namespace MoGui::MoGuiXY::PublicXY;
using namespace MoGui::Game::DeZhou;
using namespace MoGui::Game::DeZhou::DBS;

class CDBOperator;
class CDBServerSocket;
class CDBMsgThread;

class CServer : public IConnectPoolCallback, public boost::noncopyable
{
public:
	CServer();
	~CServer( void );
	
	typedef map<IConnect*, CDBServerSocket*>                 MapClientSocket;
	typedef map<short,vector<UINT32> >                       MapCheckOnline;
	typedef map<UINT32,INT16>                                MapPIDAID;
	typedef map<string,int>                                  MapUserNamePID;
	typedef map<string,int>                                  MapHardCodePID;

	enum eRightFlag
	{
		RightFlag_Common = 1,RightFlag_Priority = 2,
	};

protected:
	bool OnPriorityEvent( void );
	void OnTimer( void );
	void OnAccept( IConnect* connect );
	void OnClose( IConnect* nocallbackconnect, bool bactive );

public:
	void DealCloseSocket( IConnect* connect );

public:
private:
	IConnectPool*			             m_pPool;
	MapClientSocket          	         m_ClientsSocket;
	MapDBPlayerInfo                      m_PlayerInfos;
	CMemOperator                         m_memOperator;
	CDBReadOperator*                     m_pDBOperator;
	CDBSConfig                           m_DBConfig;
	bool                                 m_bInitComplete;             //始初化完成

	MapFaceInfo                          m_mapFaceInfo;
	MapGiftInfo                          m_mapGiftInfo;
	MapPIDAID                            m_mapAIDPID;
	MapUserNamePID                       m_mapUserNamePID;            //username到PID转换
	MapHardCodePID                       m_mapHardCodePID;            //HardCode到PID换转

	MapProductInfo                       m_mapProductInfo;
	MapHuiYuanInfo                       m_mapHuiYuanInfo;
	MapMoguiExchangeInfo                 m_mapMoguiExchangeInfo;
	//MapHonorInfo                         m_mapHonorInfo;
	//MapPTLevelInfo                       m_mapGameLevelInfo;
	MapAreaInfo                          m_mapAreaInfo;
	CMapFunctionTime                     m_DBSFuncTime;

	UINT32                               m_nServerStartTime;
	UINT32      			             m_CurTime;
	//UINT32                               m_NewDayTime;                //新的一天起点时间
	//UINT32                               m_CheckGSActiveTime;         //检查游戏服务器SOCKET活跃的时间
	//UINT32                               m_CheckPlayerOnlineTime;
	//UINT32                               m_CheckXieYiInOutTime;
	//UINT32                               m_CheckXieYiSpeedTime;
	//UINT32                               m_CheckBotPlayerCached;      //更新机器人玩家的Cacded信息
	//UINT32                               m_CheckUserProductTime;
	//UINT32                               m_CheckSendUserProductTime;  //发送玩家产品信息

	int                                  m_MaxProductIdx;

	bool                                 m_bStartDBThread;
	CRWDBMsgManage                       m_RWDBMsgManager;            //读写数据库消息管理器
	vector<CDBMsgThread*>                m_vecDBMsgThread;

	CLock		                         m_ckVectorGift;
	VectorDBUserGiftInfo                 m_vectorUserGift;

public:	
	unsigned int       GetCurTime() const { return m_CurTime; }
	CRWDBMsgManage*    GetRWDBManager() { return &m_RWDBMsgManager; }
	CDBSConfig*        GetDBConfig() { return &m_DBConfig; }
	int                OnGameServerMsg( CDBServerSocket* pDBSocket,CRecvMsgPacket& msgPack );
	void               PushUserGift(const stDBUserGiftInfo& stDBUGI);
	void               DoUserGift();

private:
	CDBServerSocket*   GetDBSocketByServerID(INT16 ServerID);
	DBServerPlayerInfo* GetDBPlayerInfo(UINT32 PID);
	bool               IsBotPlayer(INT16 AID,UINT32 PID);
	int                InitServerData();
	int                InitBaseInfo();
	void               GetHuiYuanInfo(DBS::stHuiYuanInfo& stHY,const string& strRule);
	void               GetMoguiExchangeInfo(DBS::stMoguiExchangeInfo& stME,const string& strRule);
	void               SendPlayerGameData(CDBServerSocket* pSocket,INT16 AID,UINT32 PID);
	void               SendPlayerFriend(CDBServerSocket* pSocket,INT16 AID,UINT32 PID);
	void               SendPlayerGift(CDBServerSocket* pSocket,INT16 AID,UINT32 PID);
	void               GetPlayerAwardInfo(INT16 AID,UINT32 PID,map<INT16,INT64>& mapAwardMoney);
	int                SendPlayerAward(INT16 AID,UINT32 PID);
	void               DoUserProduct(const DBS::stUserProduct& stUP);
	void               DoPlayerHuiYuan(const DBS::stUserProduct& stUP,const DBS::stHuiYuanInfo& stHY);
	void               DoPlayerMoguiExchange(const DBS::stUserProduct& stUP,const DBS::stMoguiExchangeInfo& stME);
	int                UserGiftToDBSGiftInfo(const stUserGiftInfo& stUGI,DBServerXY::DBS_msgUserGiftInfo& msgGI);
	void               CheckPlayerGameMoney(UINT32 PID);
	
	void               OnNewDay(UINT32 curTime);
	void               OnOneMinite(UINT32 curTime);
	void               OnTenMinite(UINT32 curTime);
	void               OnHour(UINT32 curTime);
	void               PrintIOSpeed(UINT32 curTime);
	void               PrintIOXieYi(UINT32 curTime);
	void               OnCheckUserProduct(UINT32 curTime);
	void               OnSendUserProduct(UINT32 curTime);
	
	void               AddUserGameMoney(INT16 AID,UINT32 PID,INT64 nMoney,bool bUpToDB=true);
	void               UpdatePlayerDataToMemcach(DBServerPlayerInfo* pPlayerInfo);
	void               AddPlayerMoneyLog(UINT32 PID,INT64 nAddMoney,const string& strLog);
	void               PrintPlayerMoneyLog(DBServerPlayerInfo* pPlayerInfo);

	int                UpdatePlayerDataToDB(DBServerPlayerInfo* pPlayerInfo);//因为中间没有更新过，和GameTime类似

	void               CheckClientSocketActive(UINT32 curTime);
	void               CheckPlayerOnLine(UINT32 curTime);
	void               CheckBotPlayerCached(UINT32 curTime);
	
	int                OnReqServerConnect( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqRoomTableInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqBotPlayerInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqPlayerLogin( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqPlayerInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqAddBotMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnPlayerQuit( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnPlayerDataNotExist( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnRespPlayerOnLine( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqUserGiftInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqPlayerGameMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	
	int                OnWDBWinLoss( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBWinLossList( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMatchResult( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBUserFriend( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBSendMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBShowFace( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBSendGift( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBSoldGift( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBIncomeAndPay( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBChatMsg( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );	
	int                OnWDBCreateTable( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBChangeName( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnReqPlayerAward( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBReportPlayerOnLine( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMaxPai( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMaxWin( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMaxMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMatchWinLoss( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBPlayerActionLog( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBPlayerAward( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBCheckGameInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBPlayerClientError( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	
	int                OnGSToDBSFlag( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnXieYiList( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );

	int                WDBMatchWinLoss(DBServerXY::WDB_MatchWinLoss& msgMWL);
	int                WDBWinLoss(DBServerXY::WDB_WinLoss& msgWL);
	int                OnAddContinuPlay(INT16 AID,UINT32 PID);

	void               InitCeShiLog();
	
	inline void        DebugError(const char* logstr,...);
	inline void        DebugLogOut(const char* logstr,...);
	inline void        DebugLog(const char* logstr,...);
	inline void        DebugInfo(const char* logstr,...);

	template<class Txieyi>
	inline void SendMsgToPlayer(Txieyi& xieyi,UINT32 PID)
	{
		DBServerPlayerInfo* pPlayerInfo = GetDBPlayerInfo(PID);
		if ( pPlayerInfo )
		{
			CDBServerSocket* pSocket = GetDBSocketByServerID(pPlayerInfo->m_ServerID);
			if ( pSocket )
			{
				pSocket->SendMsg(xieyi);
			}
			else
			{
				DebugError("SendMsgToPlayer ServerID=%d",pPlayerInfo->m_ServerID);
			}
		}
		else
		{
			DebugError("SendMsgToPlayer PID=%d");
		}
	}
};

class CDBMsgThread : public CThread,public boost::noncopyable
{
public:
	CDBMsgThread(string strName="CDBMsgThread") : CThread(strName),m_pDBServer(0){}
	~CDBMsgThread(){}

	int Run( void );
public:
	CServer*    m_pDBServer;
};