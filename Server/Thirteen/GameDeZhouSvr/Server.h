#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "GameDBSocket.h"
#include "Player.h"
#include "gamedata.h"
#include "dezhoulib.h"
#include "CGameServerConfig.h"
#include "memoperator.h"
#include "publicdata.h"

using namespace MoGui::MoGuiXY::PublicXY;
using namespace MoGui::Game::DeZhou;
using namespace MoGui::Game::DeZhou::GS;

class GameServerSocket;
class CGameDBSocket;
class CPlayer;
class CGameRoom;
class CGameTable;

class CServer : public IConnectPoolCallback, public boost::noncopyable
{
public:
	CServer();
	~CServer( void );

	enum { GetPID_Player=1,GetPID_BotUse=2,GetPID_BotIdle=4,};

	typedef RefPtr<CPlayer>	                                        PlayerPtr;
	typedef map<IConnect*, GameServerSocket*>  						MapClientSocket;
	typedef map<UINT32,PlayerPtr>            						MapPlayer;
	typedef map<UINT16,CGameRoom*>     						        MapRoom;
	typedef map<UINT32,UINT32>                                      MapIPTime;
	typedef list<PlayerPtr>                                         ListPlayer;
	typedef deque<PlayerPtr>                                        DequePlayer;
	typedef vector<PlayerPtr>                                       VecPlayer;
	typedef set<unsigned int>                                       SetIntIP;

	typedef map<UINT32,UINT32>                                      MapPIDErrorTimes;
	typedef map<UINT32,UINT32>                                      MapPIDUseTime;
	typedef map<string,UINT32>                                      MapIPErrorTimes;
	typedef map<string,UINT32>                                      MapIPUseTime;

	typedef map<string,ListPlayer>                                  MapCityPlayer;
	typedef map<string,stCityTime>                                  MapCityTime;

	typedef vector<DBServerXY::DBS_RoomInfo>                        VectorRoomInfo;
	typedef vector<DBServerXY::DBS_TableInfo>                       VectorTableInfo;
	typedef vector<DBServerXY::DBS_HonorInfo>                       VectorHonorInfo;
	typedef vector<DBServerXY::DBS_GameLevelInfo>                   VectorDBSGameLevelInfo;

	typedef map<short,DBServerXY::DBS_msgFaceInfo>                  MapFaceInfo;
	typedef map<short,DBServerXY::DBS_msgGiftInfo>                  MapGiftInfo;
	typedef map<INT16,DBServerXY::DBS_HonorInfo>                    MapDBSHonorInfo;
	typedef map<INT16,DBServerXY::DBS_GameLevelInfo>                MapDBSGameLevelInfo;

private:
	bool OnPriorityEvent( void );
	void OnTimer( void );
	void OnAccept( IConnect* connect );
	void OnClose( IConnect* nocallbackconnect, bool bactive );

public:
	void DealCloseSocket( IConnect* connect );
	
	INT64                                m_GetBotStep1;
	INT64                                m_GetBotStep2;
	INT64                                m_GetBotFailed;

	INT32                                m_nAcceptCount;
	INT32                                m_nCloseCount;

private:
	IConnectPool*			             m_pPool;
	CGameDBSocket*                       m_pDBSocket;
	MapRoom                              m_Rooms;                     //�����б�
	MapClientSocket          	         m_Clients;                   //�ͻ�������,�Ѿ�ͨ����֤�Ŀͻ�����
    MapClientSocket                      m_LoginSockets;              //��������֤��SOCKET
	MapPlayer                            m_Players;                   //��Ҷ���MAP ��Ҫ�����ߵģ������ߵľ�ɾ����
	MapPlayer                            m_BotPlayerInUse;            //ʹ���еĻ�����
	ListPlayer                           m_BotPlayerInIdle;           //���ŵĻ�����

	CMapFunctionTime                     m_FuncTime;

	CGameServerConfig                    m_GSConf;
	const CGameServerConfig*             m_pGSConfig;
	CMemOperator                         m_memOperator;

	string                               m_GameRule;

	VectorRoomInfo                       m_RoomInfoVector;
	VectorTableInfo                      m_TableInfoVector;

	UINT32 		                         m_CurTime;                   //��ǰʱ��
	UINT32                               m_NewDayTime;                //��һ���ʱ�����
	UINT32                               m_OneMinuteTime;
	UINT32                               m_TenMinuteTime;             //ÿ10����
	UINT32                               m_HourTime;                  //ÿСʱ

	UINT32                               m_CheckPlayerActiveTime;     //����Ծ������ʱ��
	UINT32                               m_CheckRoomTime;
	UINT32                               m_CheckPlayerLockTime;
	UINT32                               m_CheckPlayerOnLine;         //�Ƿ�������һ�������
	UINT32                               m_CheckAddFriendTime;
	UINT32                               m_CheckXieYiInOutTime;
	UINT32                               m_CheckXieYiSpeedTime;
	UINT32                               m_CheckCityPlayerTime;       //���ͬ����Ϣ��ʱ��
	UINT32                               m_SendPlayerOnLineTime;
	UINT32                               m_CheckFirendPlayerTime;     //����״̬�ı���Ϣ
	UINT32                               m_CheckBotPlayerTime;

	bool                                 m_bIsInitOK;
	bool                                 m_bInitDBInfo;

	bool                                 m_bOutOfDBServer;
	int                                  m_DBServerInUseTime;          //��¼DBSERVER�����õ�ʱ��ֵ

	int                                  m_BotPlayerNumber;
	int                                  m_PlayerOnGame;

public:
	int OnServerMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnDBServerMsg(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnPlayerConnect(GameServerSocket* pclient,CRecvMsgPacket& msgPack);
private:
	int  CreateRoom(CreateRoomInfo& cri);
	
	int OnReqRoomInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqJoinRoom(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqLeaveRoom(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqRoomTableInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqJoinTable(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqChangeTable(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int OnReqTailInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	//int OnReqPlayerGiftInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqGamePlayerCount(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqRoomPlayerCount(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int OnChangeUserInfo( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqPlayerAward( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqTracePlayer( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnGameFlag( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnTest(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	
	int OnXieYiList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBBatchMsg(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespServerDate(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnBotPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnRespBotPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerLoginData(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerData(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	
	int OnDBPlayerLimite(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerRight(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespAddBotMoney(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespRoomTableInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBReqPlayerOnLine(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRoomInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBTableInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	
	int OnDBRespWinLoss(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespWinLossList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerMoneyCheck(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	
	int OnDBFriendInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnUserAwardInfoList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespPlayerGameMoney(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBUserHuiYuan(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBMoguiExchange(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBSToGSFlag(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);

public:
	const CGameServerConfig*    GetGSConfig(){ return &m_GSConf;}
	CMemOperator*      GetMemOperator(){ return &m_memOperator; }
	UINT32             GetCurTime() const { return m_CurTime;}
	UINT16             GetGameID()  const { return m_GSConf.m_cfServer.m_GameID;}
	UINT16             GetServerID() const { return m_GSConf.m_cfServer.m_ServerID;}
	int                GetDBPort() const{ return m_GSConf.m_cfServer.m_dbPort;}
	string             GetDBIP() const{ return m_GSConf.m_cfServer.m_dbIp;}
	int                GetTotalPlayerCount();
	void               InitGameRule(const CGameServerConfig& GSConfig);
	void               OnNewDay(UINT32 curTime);
	void               OnOneMinite(UINT32 curTime);
	void               OnTenMinite(UINT32 curTime);
	void               OnHour(UINT32 curTime);
	void               PrintIOSpeed();
	void               PrintIOXieYi();
	void               CheckClientSocketActive();
	void               CheckPlayerActive();
	void               SendPlayerOnline();
	void               CheckPlayerLocked();

	CGameRoom*         GetRoomByID(UINT16 roomid);
	void               SetBDSocket(CGameDBSocket* pDBSocket);
	IConnect*          Connect( const char* ip, int port );
	IConnectPool*      GetConnectPool(){ return m_pPool; }
	
	int                SendRoomInfoListToPlayer(PlayerPtr pPlayer);
	
	void               InitServerRooms();
	void               KillPlayer( PlayerPtr pPlayer );
	void               ClearPlayerData( PlayerPtr pPlayer );
	PlayerPtr          GetPlayerByPID(UINT32 PID,int nFlag=(GetPID_Player|GetPID_BotUse|GetPID_BotIdle));
	void               ResetClientPlayer(PlayerPtr pPlayer);        //�����ڻ����˵�RESET����ҷ��ڴ����в����κ�һ�����䴦��δʹ��״̬
	void               ReqUnlockPlayer(PlayerPtr pPlayer);          //������ҽ�����ͨ��������ҵ���Ϸ��������ȷ����ͨ

	//������
	PlayerPtr          GetBotPlayer(INT64 nMinMoney,INT64 nMaxMoney,bool bShowRet=false);
	void               ResetBotPlayer(PlayerPtr pPlayer);
	void               OnTimeCheckBotPlayer();

	//������Ϸ����Ϸ��
	int                GetPlayerUserData(PlayerPtr pPlayer);

	//������Ϣ
	void               SendPlayerFollow(PlayerPtr pPlayer);           //����ҹ�ע�������Ϣ�������
	void               SendPlayerBeFollow(PlayerPtr pPlayer);         //����ҵ���Ϣ������ע��ҵ����
	void               SendReqAddFriendList(PlayerPtr pPlayer);	
	void               DeleteFirendToClient(PlayerPtr pPlayer);
	void               OnFriendLand(PlayerPtr pPlayer,PlayerPtr FriendPlayer);
	void               SendPlayerStateToFengSi(PlayerPtr pPlayer);
	void               OnTimeUpdateFirendPlayerState();

	void               InitCeShiLog();
	inline void        DebugError(const char* logstr,...);
	inline void        DebugLogOut(const char* logstr,...);
	inline void        DebugLog(const char* logstr,...);
	inline void        DebugInfo(const char* logstr,...);

	template<class Txieyi>
	void SendMsgToAllPlayerInServer(Txieyi& xieyi)
	{
		PlayerPtr pPlayer;
		for ( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
		{
			pPlayer = itorPlayer->second;
			if ( pPlayer->IsNeedSendMsg() )
			{
				pPlayer->SendMsg(xieyi);
			}
		}
	}

	template<class Txieyi>
	void SendMsgToAllPlayerHaveInRoom(Txieyi& xieyi,int RoomID)
	{
		PlayerPtr pPlayer;
		for ( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
		{
			pPlayer = itorPlayer->second;
			if ( pPlayer->IsNeedSendMsg() && pPlayer->HaveSendTableInfoList(RoomID) )
			{
				pPlayer->SendMsg(xieyi);
			}
		}
	}

	template<class Txieyi>
		void SendMsgToDBServer(Txieyi& xieyi)
	{
		if ( m_pDBSocket && m_pDBSocket->CanUse() )
		{
			m_pDBSocket->SendMsg(xieyi);
		}
		else
		{
			DebugError("SendMsgToDBServer DBSocket Can't Used XYID=%d",xieyi.XY_ID);
		}
	}
};

class CDBConnectThread : public CThread,public boost::noncopyable
{
public:
	CDBConnectThread(string strName="CDBConnectThread") : CThread(strName),m_pDBSocket(0){}
	~CDBConnectThread(){}

	int Run( void );

public:
	CGameDBSocket*    m_pDBSocket;
};