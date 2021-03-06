#pragma once

#include "Base.h"

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "dezhoudata.h"
#include "dbserversocket.h"
#include "DBServerXY.h"
#include "CDBSConfig.h"
#include "RWDBMsg.h"
#include "ReadWriteDB.h"
#include "memoperator.h"

using namespace MoGui::MoGuiXY::PublicXY;
using namespace MoGui::Game::DeZhou;
using namespace MoGui::Game::DeZhou::DBS;

class CDBOperator;
class CDBServerSocket;
class CDBMsgThread;

class CServer : public AGBase::IConnectPoolCallback, public boost::noncopyable
{
public:
	CServer();
	~CServer( void );
	
	typedef std::map<AGBase::IConnect*, CDBServerSocket*>               MapClientSocket;
	typedef std::map<short, std::vector<UINT32> >                       MapCheckOnline;
	typedef std::map<UINT32, INT16>                                     MapPIDAID;

	enum eRightFlag
	{
		RightFlag_Common = 1,RightFlag_Priority = 2,
	};

protected:
	bool OnPriorityEvent( void );
	void OnTimer( void );
	void OnAccept(AGBase::IConnect* connect);
	void OnClose(AGBase::IConnect* nocallbackconnect, bool bactive);

public:
	void DealCloseSocket(AGBase::IConnect* connect);

public:
private:
	AGBase::IConnectPool*			     m_pPool;
	MapClientSocket          	         m_ClientsSocket;
	MapDBPlayerInfo                      m_PlayerInfos;
	CMemOperator                         m_memOperator;
	CDBOperator*                         m_pDBOperator;
	CDBSConfig                           m_DBConfig;
	bool                                 m_bInitComplete;             //始初化完成

	MapFaceInfo                          m_mapFaceInfo;
	MapGiftInfo                          m_mapGiftInfo;
	MapPIDAID                            m_mapAIDPID;
	MapProductInfo                       m_mapProductInfo;
	MapHuiYuanInfo                       m_mapHuiYuanInfo;
	MapMoguiExchangeInfo                 m_mapMoguiExchangeInfo;
	MapHonorInfo                         m_mapHonorInfo;
	MapPTLevelInfo                       m_mapGameLevelInfo;
	MapAreaInfo                          m_mapAreaInfo;
	CMapFunctionTime                     m_DBSFuncTime;

	UINT32                               m_nServerStartTime;
	UINT32      			             m_CurTime;
	UINT32                               m_NewDayTime;                //新的一天起点时间
	UINT32                               m_CheckGSActiveTime;         //检查游戏服务器SOCKET活跃的时间
	UINT32                               m_CheckPlayerOnlineTime;	
	UINT32                               m_CheckXieYiInOutTime;
	UINT32                               m_CheckXieYiSpeedTime;
	UINT32                               m_CheckBotPlayerCached;      //更新机器人玩家的Cacded信息
	UINT32                               m_CheckUserProductTime;
	UINT32                               m_CheckSendUserProductTime;  //发送玩家产品信息
	UINT32                               m_OneMinuteTime;             //每分钟
	UINT32                               m_TenMinuteTime;             //每10分钟
	UINT32                               m_HourTime;                  //每小时

	int                                  m_MaxProductIdx;

	bool                                 m_bStartDBThread;
	CRWDBMsgManage                       m_RWDBMsgManager;            //读写数据库消息管理器
	std::vector<CDBMsgThread*>                m_vecDBMsgThread;

	AGBase::CLock		                 m_ckVectorGift;
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
	INT16              GetAIDByPID(UINT32 PID);
	bool               IsBotPlayer(INT16 AID,UINT32 PID);
	int                InitServerData();
	int                InitBaseInfo();
	void               GetHuiYuanInfo(DBS::stHuiYuanInfo& stHY, const std::string& strRule);
	void               GetMoguiExchangeInfo(DBS::stMoguiExchangeInfo& stME, const std::string& strRule);
	void               SendPlayerGameData(CDBServerSocket* pSocket,INT16 AID,UINT32 PID);
	void               SendPlayerFriend(CDBServerSocket* pSocket,INT16 AID,UINT32 PID);
	void               SendPlayerGift(CDBServerSocket* pSocket,INT16 AID,UINT32 PID);
	void               GetPlayerAwardInfo(INT16 AID, UINT32 PID, std::map<INT16, INT64>& mapAwardMoney);
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
	void               PrintIOSpeed();
	void               PrintIOXieYi();
	void               OnCheckUserProduct();
	void               OnSendUserProduct();
	
	void               AddUserGameMoney(INT16 AID,UINT32 PID,INT64 nMoney,bool bUpToDB=true);
	void               UpdatePlayerDataToMemcach(DBServerPlayerInfo* pPlayerInfo);
	void               AddPlayerMoneyLog(UINT32 PID,INT64 nAddMoney,const std::string& strLog);
	void               PrintPlayerMoneyLog(DBServerPlayerInfo* pPlayerInfo);

	INT64              GetPlayerRight(DBServerPlayerInfo* pPlayerInfo);
	int                UpdatePlayerRight(DBServerPlayerInfo* pPlayerInfo);
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
	int                OnWDBChangeBankMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMaxPai( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMaxWin( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMaxMoney( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBMatchWinLoss( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBPlayerActionLog( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBPlayerAward( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBCheckGameInfo( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBPlayerClientError( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnWDBFinishHonor( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnGSToDBSFlag( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );
	int                OnXieYiList( CDBServerSocket* pSocket,CRecvMsgPacket& msgPack );

	int                WDBMatchWinLoss(DBServerXY::WDB_MatchWinLoss& msgMWL);
	int                WDBWinLoss(DBServerXY::WDB_WinLoss& msgWL);
	int                OnAddContinuPlay(INT16 AID,UINT32 PID);

	void               InitCeShiLog();
	
public:
	inline void        DebugError(const char* logstr,...);
	inline void        DebugLogOut(const char* logstr,...);
	inline void        DebugLog(const char* logstr,...);
	inline void        DebugInfo(const char* logstr,...);
	inline void        CeShiInfo(const char* logstr,...);

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

class CDBMsgThread : public AGBase::CThread,public boost::noncopyable
{
public:
	CDBMsgThread(std::string strName = "CDBMsgThread") : CThread(strName), m_pDBServer(0){}
	~CDBMsgThread(){}

	int Run( void );
public:
	CServer*    m_pDBServer;
};