#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "GameDBSocket.h"
#include "Player.h"
#include "dezhoudata.h"
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

class CServer : public AGBase::IConnectPoolCallback, public boost::noncopyable
{
public:
	CServer();
	~CServer( void );

	enum { GetPID_Player=1,GetPID_BotUse=2,GetPID_BotIdle=4,};
	//enum { MaxPlayerForFriend = 100; };

	typedef AGBase::RefPtr<CPlayer>	                                     PlayerPtr;
	typedef std::map<AGBase::IConnect*, GameServerSocket*>  			 MapClientSocket;
	typedef std::map<UINT32, PlayerPtr>            						 MapPlayer;
	typedef std::map<UINT16, CGameRoom*>     						     MapRoom;
	typedef std::map<UINT32, UINT32>                                     MapIPTime;
	typedef std::list<PlayerPtr>                                         ListPlayer;
	typedef std::deque<PlayerPtr>                                        DequePlayer;
	typedef std::vector<PlayerPtr>                                       VecPlayer;
	typedef std::set<unsigned int>                                       SetIntIP;

	typedef std::map<UINT32, UINT32>                                      MapPIDErrorTimes;
	typedef std::map<UINT32, UINT32>                                      MapPIDUseTime;
	typedef std::map<std::string, UINT32>                                 MapIPErrorTimes;
	typedef std::map<std::string, UINT32>                                 MapIPUseTime;

	typedef std::map<std::string, ListPlayer>                             MapCityPlayer;
	typedef std::map<std::string, stCityTime>                             MapCityTime;

	typedef std::vector<DBServerXY::DBS_RoomInfo>                         VectorRoomInfo;
	typedef std::vector<DBServerXY::DBS_TableInfo>                        VectorTableInfo;
	typedef std::vector<DBServerXY::DBS_HonorInfo>                        VectorHonorInfo;
	typedef std::vector<DBServerXY::DBS_GameLevelInfo>                    VectorDBSGameLevelInfo;

	typedef std::map<short, DBServerXY::DBS_msgFaceInfo>                  MapFaceInfo;
	typedef std::map<short, DBServerXY::DBS_msgGiftInfo>                  MapGiftInfo;
	typedef std::map<INT16, DBServerXY::DBS_HonorInfo>                    MapDBSHonorInfo;
	typedef std::map<INT16, DBServerXY::DBS_GameLevelInfo>                MapDBSGameLevelInfo;

private:
	bool OnPriorityEvent( void );
	void OnTimer( void );
	void OnAccept(AGBase::IConnect* connect);
	void OnClose(AGBase::IConnect* nocallbackconnect, bool bactive);

public:
	void DealCloseSocket(AGBase::IConnect* connect);
	
	INT64                                m_GetBotStep1;
	INT64                                m_GetBotStep2;
	INT64                                m_GetBotFailed;
	bool                                 m_bShowRoomPlayer;

	INT32                                m_nAcceptCount;
	INT32                                m_nCloseCount;

private:
	AGBase::IConnectPool*			     m_pPool;
	CGameDBSocket*                       m_pDBSocket;
	MapRoom                              m_Rooms;                     //�����б�
	MapClientSocket          	         m_Clients;                   //�ͻ�������,�Ѿ�ͨ����֤�Ŀͻ�����
    MapClientSocket                      m_LoginSockets;              //��������֤��SOCKET
	MapPlayer                            m_Players;                   //��Ҷ���MAP ��Ҫ�����ߵģ������ߵľ�ɾ����
	MapPlayer                            m_BotPlayerInUse;            //ʹ���еĻ�����
	ListPlayer                           m_BotPlayerInIdle;           //���ŵĻ�����
	MapPlayer                            m_CanFriendPlayer;           //ָ���Թ�ע����
	MapCityPlayer                        m_mapCityPlayer;    
	MapCityTime                          m_mapCityTime;
	MapIPTime                            m_FlashIPTime;               //��Ҫ�Ǽ�¼ÿ��SOCKETͨ��FLASH���������ʱ��
	MapIPTime                            m_ForbidIP;                  //���ڽ�ֹIP��ʱ��

	MapPIDErrorTimes                     m_PIDErrorTimes;             //��¼PID����Ĵ���
	MapPIDUseTime                        m_PIDUseTime;                //��¼PID��ַ���õ�ʱ��
	MapIPErrorTimes                      m_IPErrorTimes;              //��¼IP��ַ����Ĵ���
	MapIPUseTime                         m_IPUseTime;                 //��¼IP��ַ���õ�ʱ��
	CMapFunctionTime                     m_FuncTime;

	CGameServerConfig                    m_GSConf;
	const CGameServerConfig*             m_pGSConfig;
	CMemOperator                         m_memOperator;

	std::string                          m_GameRule;

	VectorRoomInfo                       m_RoomInfoVector;
	VectorTableInfo                      m_TableInfoVector;
	MapFaceInfo                          m_mapFaceInfo;
	MapGiftInfo                          m_mapGiftInfo;
	MapMatchInfo                         m_mapMatchInfo;
	MapDBSHonorInfo                      m_mapHonorInfo;

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

	INT64                                m_OriginJuBaoMoney;           //�۱����ʼֵ
	INT64                                m_JuBaoPengMoney;             //�۱���

public:
	int OnServerMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnDBServerMsg(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnPlayerConnect(GameServerSocket* pclient,CRecvMsgPacket& msgPack);	

	void ShowTable(CGameTable* pTable);
	void HideTable(CGameTable* pTable);
private:
	int  CreateRoom(CreateRoomInfo& cri);
	
	int OnReqRoomInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqJoinRoom(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqRoomTableInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqJoinTable(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int OnReqSignMatch(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnEnterMatch(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqMatchRank(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int OnReqTailInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqPlayerGiftInfo(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqGamePlayerCount(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqRoomPlayerCount(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqCityPlayerBase(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqCityPlayerState(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int OnPlayerSendGift(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnPlayerSoldGift(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqChangeGift( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqChangeBank( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqCreateTable( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int OnChangeUserInfo( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqPlayerAward( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnReqTracePlayer( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnChangePlace( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnGameFlag( PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int OnTest(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	
	int OnXieYiList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBBatchMsg(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespServerDate(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnBotPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnRespBotPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerLoginData(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerData(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerDataEx(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerLimite(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerRight(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespPlayerInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespAddBotMoney(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespRoomTableInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBReqPlayerOnLine(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRoomInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBTableInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBFaceInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBMatchInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBGiftInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespWinLoss(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespWinLossList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBRespFinishHonor(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBPlayerMoneyCheck(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	
	int OnDBFriendInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBUserGiftInfo(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
	int OnDBUserGiftInfoList(CGameDBSocket* pDBSocket,CRecvMsgPacket& msgPack);
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
	std::string        GetDBIP() const{ return m_GSConf.m_cfServer.m_dbIp; }
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
	INT64              GetJuBaoPeng(){ return m_JuBaoPengMoney; }
	void               AddJuBaoPeng(INT64 nMoney){ m_JuBaoPengMoney = max(m_OriginJuBaoMoney,m_JuBaoPengMoney+nMoney); }

	CGameRoom*         GetRoomByID(UINT16 roomid);
	CGameRoom*         GetPrivateRoom();
	void               SetBDSocket(CGameDBSocket* pDBSocket);
	AGBase::IConnect*          Connect( const char* ip, int port );
	AGBase::IConnectPool*      GetConnectPool(){ return m_pPool; }

	bool               GetGiftInfo(INT16 GiftID,DBServerXY::DBS_msgGiftInfo& GiftInfo);
	bool               GetFaceInfo(INT16 FaceID,DBServerXY::DBS_msgFaceInfo& FaceInfo);
	//int                GetGiftMoney(const DBServerXY::DBS_msgUserGiftInfo& stDBUgi);
	bool               GetMatchInfo(int MatchID,GS::stMatchInfo& MatchInfo);
	bool               GetHonorInfo(INT16 HonorID,DBServerXY::DBS_HonorInfo& rHonorInfo);
	
	int                SendRoomInfoListToPlayer(PlayerPtr pPlayer);
	
	void               InitServerRooms();
	void               KillPlayer( PlayerPtr pPlayer );
	void               ClearPlayerData( PlayerPtr pPlayer );
	void               AddFlashIP(UINT32 ip);
	bool               CheckFlashIP(UINT32 ip);
	PlayerPtr          GetPlayerByPID(UINT32 PID,int nFlag=(GetPID_Player|GetPID_BotUse|GetPID_BotIdle));
	void               ResetClientPlayer(PlayerPtr pPlayer);        //�����ڻ����˵�RESET����ҷ��ڴ����в����κ�һ�����䴦��δʹ��״̬
	void               ReqUnlockPlayer(PlayerPtr pPlayer);          //������ҽ�����ͨ��������ҵ���Ϸ��������ȷ����ͨ

	//���� 
	void               ChangeMatchTableState(CGameTable* pTable);

	//������
	PlayerPtr          GetBotPlayer(INT64 nMinMoney,INT64 nMaxMoney,bool bShowRet=false);
	void               ResetBotPlayer(PlayerPtr pPlayer);
	void               OnTimeCheckBotPlayer();

	//������Ϸ����Ϸ��
	int                GetPlayerUserData(PlayerPtr pPlayer);

	//ͬ����Ϣ
	void               UpdateCityPlayer();
	void               AddCityPlayer(PlayerPtr pPlayer);
	void               DeleteCityPlayer(PlayerPtr pPlayer);
	void               AddCityPlayerToClient(PlayerPtr pPlayer);
	void               DeleteCityPlayerToClient(PlayerPtr pPlayer);
	void               OnTimeSendCityPlayerState(std::string strCity);
	void               SendCityPlayerStateToPlayer(PlayerPtr pPlayer);
	void               SendCityPlayerToPlayer(PlayerPtr pPlayer);

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
	inline void        DebugCeShi(const char* logstr,...);
	inline void        DebugCeShiError(const char* logstr,...);

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
	void SendMsgToAllPlayerInPlace(Txieyi& xieyi,BYTE nPlace)
	{
		PlayerPtr pPlayer;
		for ( MapPlayer::iterator itorPlayer=m_Players.begin();itorPlayer!=m_Players.end();++itorPlayer)
		{
			pPlayer = itorPlayer->second;
			if ( pPlayer->IsNeedSendMsg() && pPlayer->GetPlace()==nPlace )
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

class CDBConnectThread : public AGBase::CThread,public boost::noncopyable
{
public:
	CDBConnectThread(std::string strName = "CDBConnectThread") : CThread(strName), m_pDBSocket(0){}
	~CDBConnectThread(){}

	int Run( void );

public:
	CGameDBSocket*    m_pDBSocket;
};