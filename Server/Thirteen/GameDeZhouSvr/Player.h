#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "server.h"
#include "GameServerSocket.h"
#include "DBServerXY.h"

#include "GameData.h"

class CServer;
class GameServerSocket;
class CGameTable;
class CGameRoom;

using namespace MoGui::Game::DeZhou;
using namespace MoGui::Game::DeZhou::GS;

class CPlayer;
typedef std::list<CPlayer*> PLAYERLIST;
typedef RefPtr<CPlayer>	 PlayerPtr;

class CPlayer : public RefCount
{
public:
	CPlayer();
	virtual ~CPlayer(void);
protected:
	virtual void OnDestroy(void);
public:
	void Init();
	bool CloseSocket();
	int  OnPacket(CRecvMsgPacket& msgPack);

private:
	static PLAYERLIST s_pPlayerList;

public:	
	static void ClearAllPlayers();
	static void RecyclePlayer(CPlayer* pPlayer);
	static CPlayer* GetOnePlayer();
	static int  GetTotalPlayerCount();

public:
	static int                    s_State[PLAYER_ST_END];                    //���ڼ�¼����������ҵ�״̬
	static long                   s_nMoneyLogCount;

	enum { WinTypeNum = 10 };
	enum
	{
		StateInfo_Friend        = 1,
		StateInfo_City          = 2,
		StateInfo_SendSameTable = 4,
		StateInfo_ShowLobby     = 8,
	};

private:
	GameServerSocket*             m_pClientSocket;
	CServer*	                  m_pServer;
	CGameRoom*                    m_pRoom;
	CGameTable*                   m_pTable;

	BYTE                          m_PlayerType;                              //��ҵ�ǰ��״̬���ǻ����ˣ������йܣ�������������
	BYTE                          m_ClientType;
	BYTE                          m_PlayerState;                             //��Ϸ״̬��վ�ڷ��䣬�����ڷ��������������

	UINT32                        m_SocketCloseTime;                         //��¼����ʱ�䣬�������Ϊ������ҵ�ʱ����Ч

	BYTE                          m_StateInfo;                               //��Ҫ����λ��¼�߼�ֵλΪ1��ʾ��0Ϊ��

public:
	UINT32                        m_KillTime;                                //���ʧЧʱ�䣬����10���ӽ������
	UINT32                        m_LockTime;                                //ָ�����Ϸ�ұ���ס��ʱ�䣬���Ϊ���ʾû����

	std::string                   m_GSSessionKey;

public:
	GameServerSocket*             GetSocket() const { return m_pClientSocket;}
	bool                          IsHaveSocket() const { return m_pClientSocket == NULL; }
	void                          SetServer(CServer* pserver);
	CGameRoom*                    GetRoom() const { return m_pRoom; }
	void                          SetRoom(CGameRoom* pRoom){ m_pRoom = pRoom; }
	CGameTable*                   GetTable() const { return m_pTable; }
	void                          SetTable(CGameTable* pTable){ m_pTable=pTable; }

	UINT32                        GetCloseSocketTime();
	UINT32                        GetKillTime();
	void                          OnKill(UINT32 curTime);
	void                          Lock(UINT32 curTime){ m_LockTime=curTime;}
	void                          Unlock(){ m_LockTime=0; }
	bool                          Islocked()const { return m_LockTime != 0; }
	UINT32                        GetLockedTime()const{ return m_LockTime; }

	BYTE                          GetPlayerState() const { return m_PlayerState; }
	void                          SetPlayerState( BYTE st );
	BYTE                          GetPlayerType()const { return m_PlayerType;}
	void                          SetPlayerType( BYTE playertype );

	bool                          IsBotPlayer()const { return m_PlayerType == PLAYER_TYPE_BOT; }
	bool                          IsNotBotPlayer()const{ return m_PlayerType != PLAYER_TYPE_BOT; }
	bool                          IsAdmin()const { return m_PlayerType == PLAYER_TYPE_ADMIN; }
	bool                          IsCommondPlayer()const{ return m_PlayerType == PLAYER_TYPE_PLAYER; }
	bool                          IsClientPlayer()const{ return IsCommondPlayer() || IsAdmin(); }

	bool                          IsSeeing()const{ return m_PlayerState == PLAYER_ST_SEEING; }
	bool                          IsPlaying()const{ return ( m_PlayerState==PLAYER_ST_PALYING );}
	bool                          IsWaiting()const{ return m_PlayerState==PLAYER_ST_WAIT; }
	bool                          IsDead()const{ return m_PlayerState == PLAYER_ST_NONE; }
	bool                          IsOnLine()const{ return m_PlayerState != PLAYER_ST_NONE; }
	bool                          IsInLobby()const{ return m_PlayerState == PLAYER_ST_LOBBY;}
	bool                          IsInRoom()const{ return m_PlayerState == PLAYER_ST_ROOM;}
	bool                          IsInTable()const;
	bool                          IsPlayerSitOnTable()const;
	bool                          IsNeedSendMsg()const;
	

private:
	/////////////////////////���λ��//////////////////////////////////
	UINT16                        m_RoomID;
	UINT16                        m_TableID;
	BYTE                          m_SitID;
public:
	UINT16                        GetServerID()const;
	UINT16                        GetRoomID()const{ return m_RoomID; }
	void                          SetRoomID( UINT16 id){ m_RoomID=id; }
	UINT16                        GetTableID()const { return m_TableID; }
	void                          SetTableID( UINT16 id){ m_TableID=id; }
	BYTE                          GetSitID()const {return m_SitID;}
	void                          SetSitID( BYTE id){ assert(id>=0&&id<=MAX_PALYER_ON_TABLE);m_SitID=id; }

public:
	BYTE                          m_ChangeName;
	string                        m_NickName;
	BYTE                          m_Sex;

private:
	///////////////////////��������///////////////////////////////////
	UINT16                        m_AID;
	UINT32                        m_PID;

	int                           m_HeadID;
	string						  m_HeadPicURL;
	string                        m_HomePageURL;
	string                        m_City;

public:
    UINT16                        GetAID() const { return m_AID; }
	void                          SetAID(INT16 AID){ m_AID = AID;}
	UINT32                        GetPID() const { return m_PID; }
	void                          SetPID(UINT32 PID){ m_PID = PID;}
	int                           GetHeadID()const{ return m_HeadID; }
	const string&                 GetNickName() const { return m_NickName; }
	const string&                 GetHeadPicURL() const { return m_HeadPicURL; }
	const string&                 GetHomePageURL() const { return m_HomePageURL; }
	const string&                 GetCity()const { return m_City; }
	void                          SetCity(const string& strCity){ m_City = strCity; }

private:
	////////////////////////��Ϸ����////////////////////////////////////
	INT64                        m_nBankMoney;	
	INT32           			 m_nMoGuiMoney;                             //�������һԪǮ��Ӧ�ļ�ֵ���������ϷGameMoneyҪ���ֿ�����	
	INT64       	 			 m_nGameMoney;                              //������ϵ�Ǯ�����������ӵ�Ǯ���Ϊ��Ǯ����Ȼ����û�м������е�Ǯ
    INT64                        m_nMatchJF;                                //���ھ�����Ʒ��

	int                          m_nMedal;
	int                          m_Diamond;
	int                          m_GoldenPig;
	int                          m_PigFood;

public:
	vector<string>               m_vectorMoneyLog;

	INT64                        GetGameMoney()const{return m_nGameMoney;}; //ȡ�����Ҫ��������ʾ�����ܲ����κ���ֵ�ļ���
	void                         DecGameMoney(INT64 nMoney);
	void                         AddGameMoney(INT64 nMoney);

	INT32                        GetMoGuiMoney()const{return m_nMoGuiMoney;};
	void                         AddMoGuiMoney(INT32 nMoney);
	void                         DecMoGuiMoney(INT32 nMoney);

	INT64                        GetMatchJF()const{return m_nMatchJF;};
	void                         AddMatchJF(INT64 nJF);

    void                         AddMoneyLog(INT64 nAddMoney,string strLog);
    void                         ClearMoneyLog();

public:
	INT64       	 			 m_nJF;                                     //���� ����������������ļ�����Ҳ���ڵȼ��ļ���
	INT64       	 			 m_nEP;                                     //
	
	INT32        	 			 m_nWinTimes;
	INT32        	 			 m_nLossTimes;
	INT32        	 			 m_DrawnTimes;
	INT32        	 			 m_EscapeTimes;
	INT32                        m_nGameTime;

	string                       m_MaxPai;
	UINT32                       m_MaxPaiTime;
	INT64                        m_MaxMoney;
	UINT32                       m_MaxMoneyTime;
	INT64                        m_MaxWin;
	UINT32                       m_MaxWinTime;

	int                          m_WinRecord[WinTypeNum];
	int                          m_Achieve[4];
	vector<BYTE>                 m_listHonor;

	UINT32                       m_InvitePID;                               //��ұ�˭���������Ϸ��
	UINT32                       m_JoinTime;                                //��Ҽ�����Ϸ��ʱ��
	UINT32                       m_LandTime;                                //��ҵ�ǰ��½ʱ��
	UINT8                        m_BotLevel;                                //���ƻ����������Ϸ�ҵ�����

	BYTE                         m_VipLevel;                               //ָ��һ�Ա���𣬱�����������
	BYTE                         m_GameLevel;                              //������Ϸ���������ĵȼ�
	
	INT32                        m_CurGiftIdx;                             //��ҵ�ǰ��������
	UINT32                       m_FreeFaceTime;                           //������ѵ�ʱ��
	UINT32                       m_EndBotPlayerTime;                       //ָ��������Ϸ������ʱ��

public:
	INT64                        GetJF()const {return m_nJF;}
	INT64                        GetExperience()const {return m_nEP;}
	INT32                        GetWinLossTimes()const{ return m_nWinTimes+m_nLossTimes;}
	const string&                GetMaxPai() const { return m_MaxPai; }
	UINT8                        GetBotLevel()const{ return m_BotLevel; }

	void                         CheckMaxGameMoney();
    void                         CheckMaxWinMoney(INT64 nWinMoney);
    void                         CheckMaxPai();

	//-------�趨��ҵ����ƣ�Ȩ��ϵͳ--------------------
	INT64           			 m_nUpperLimite;                            //ָ��ҵ���Ϸ�ҵ������ܶÿ�ֻ�����������ֵ�������޷ⶥ��ת�ʻή�ͣ���ֵ����������
	INT64           			 m_nLowerLimite;                            //ָ��ҵ���;ȼ��ߣ����ڴ�ֵ��Ȩֵ�Զ�����

	int                          m_RightLevel;                              //����
	int                          m_RightTimes;                              //����

    //////////////////////���������Ϣ///////////////////////////////////
	typedef map<UINT16,UINT32>    MapSendRoomTableInfo;	               
	MapSendRoomTableInfo          m_mapRoomTime;                            //��ʾ�Ѿ����͹������������б�ķ���ID��	

	bool                          HaveSendTableInfoList( UINT16 roomid);
	void                          AddRoomHaveSendTableInfo( UINT16 roomid);
	UINT32                        GetSendTime(UINT16 roomid);                //����ÿ�����������ʱ��ֵ��ͬʱҲ��ʶ���Ƿ��͹������б�
	void                          SetSendTime(UINT16 roomid,UINT32 SendTime);

	bool                          m_RoomStateChange;                         //�����ڷ��ͷ����ǩ�����״̬�仯
	bool                          m_IsRoomShowPlayer;                        //�Ƿ����б�����ʾ�����
	UINT32                        m_RegisterRoomPlayerTime;

	UINT32                        m_ReqCityPlayerTime;                       //ͬ�Ǻ��ѿ�ʼע���ʱ��
	void                          SetCityStateChange(bool bChange);
	bool                          IsCityStateChange();

	//��������Ϣ
	INT16                         m_FriendCount;                             //�ҹ�ע�Ķ��󲻳���500��
	ListPID                       m_listFollow;                              //�ҹ�ע��
	ListPID                       m_listBeFollow;                            //��ע�ҵ�
	bool                          IsFollow(UINT32 PID);
	bool                          IsBeFollow(UINT32 PID);
	void                          AddFollowFriend(UINT32 PID);
	void                          DeleteFollowFriend(UINT32 PID);
	void                          AddBeFollowFriend(UINT32 PID);
	void                          DeleteBeFollowFriend(UINT32 PID);
	void                          SetFriendStateChange(bool bChange);
	bool                          IsFriendStateChange();

	ListReqFriend                 m_listReqFriend;                           //����Ӻ��ѵ����ID
	bool                          IsPIDInReqList(UINT32 PID);
	void                          DeleteReqFriendPID(UINT32 PID);
	void                          DeletePIDByTime(UINT32 LatestTime);
	void                          AddReqFriend(UINT32 PID,UINT32 CurTime);

	//�����
	typedef map<int,DBServerXY::DBS_msgUserGiftInfo>            MapUserGiftInfo;

	MapUserGiftInfo               m_curMapUserGiftInfo;
	MapUserGiftInfo               m_outMapUserGiftInfo;

	void                          DeleteGift(int GiftIdx);
	void                          AddGift(const DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int nFlag);

	bool                          GetCurUserGift(DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int GiftIdx);
	bool                          GetOutUserGift(DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int GiftIdx);

public:
	//�����Ǹ����й������Ϸ�����Լ����ܵ��޶�����

	//��Ǯ�
	UINT32                        m_PromoteTimeStart;                       //��ʱ��ʼ�����ʱ�䣬�������¿�ʼ��ʱ               
	int                           m_TimeStep;                               //������ǰ���е��ǵڼ�����
	int                           m_TimePass;                               //ָ��ǰ����ʱ��ʼ���ѹ�ȥ��ʱ�䣬ÿһ����ȡ����0

	//������½�Լ�������Ϸ
	int                           m_ContinuPlay;
	int                           m_ContinuLogin;

	//��¼�����Ϊ������
	UINT32                        m_PlayerCreateTime;                        //��¼��Ϸ������������ҵ�ʱ��
	UINT32                        m_PlayerSitTime;                           //�������λ���ϵ�ʱ��

	int                           m_OneLifeGameTime;
	int                           m_OneLifeMatchTime;
	int                           m_EnterRoomTimes;
	int                           m_EnterTableTimes;
	int                           m_SitDownTimes;
	int                           m_OneLiftPlayTimes;

	int                           m_TodayGameSecond;                         //������Ϸʱ�䰴���㳤��
	int                           m_TodayGameCount;                          //������Ϸʱ�䰴������
	int                           m_HaveAwardInvite;                         //�Ƿ��Ѿ�������

public:
	void     InitEveryDay();

	void     GetPlayerBaseInfo(PlayerBaseInfo& pbi);
	void     GetPlayerGameInfo(PlayerGameInfo& pgi);
	void     GetPlayerStateInfo(PlayerStateInfo& psi);

	void     SetPlayerData(const MoGui::MoGuiXY::DBServerXY::DBS_PlayerData& msgPD);

	void     SetBotPlayerData(const MoGui::MoGuiXY::DBServerXY::DBS_BotPlayerData& bpd);

	void     GetPlayerData(GameXY::Game_PlayerData& game_msgPD);
	void     GetPlayerDataEx(GameXY::Game_PlayerDataEx& game_msgPDEx);
	
	void     GetPlayerTailInfo(GameXY::PlayerTailInfo& msgPTI);

	bool     IsSendSameTablePlayer();
	void     SetSendSameTablePlayer(bool bSend);
	bool     IsShowLobby();
	void     SetShowLobby(bool bShow);//��ʾ�ͻ�����ʾ����

	void     SendMaxPai();
	void     SendMaxWin();
	void     SendMaxMoney();
	void     SendPlayerData();
	void     SendPlayerDataEx();

	void     UpdateGameMoney();
	void     UpdateMoGuiMoney();
	void     UpdateGameInfo();

	void     CheckContinuPlay();

	void     LeaveTable();
	void     LeaveRoom();
	void     JoinTable(CGameTable* pTable);
	void     JoinRoom(CGameRoom* pRoom);
	void     CheckGameInfo();
public:
	bool     CanKillNow();
	void     AttachSocket(GameServerSocket* cs);
	void     DisAttachSocket();
	
	UINT32   GetCurTime();

	void     DebugError(const char* logstr,...);
	void     DebugInfo(const char* logstr,...);

	template<class Txieyi>
	inline int PlayerSendMsgToDBServer(Txieyi& xieyi)
	{
		if ( m_pServer )
		{
			m_pServer->SendMsgToDBServer(xieyi);
		}
		else
		{
			DebugError("AID=%d PID=%d MsgID=%d",m_AID,m_PID,xieyi.XY_ID);
		}
		return 0;
	}

	template<class Txieyi>
		inline int SendMsg(Txieyi& xieyi)
	{
		if( !IsNeedSendMsg() ){return 0;}
		if( m_pClientSocket )
		{
			m_pClientSocket->SendMsg(xieyi);
		}
		return 0;
	}
	template<class Txieyi>
		inline int SendGameLogicMsg(Txieyi& xieyi)
	{
		assert( xieyi.XY_ID < GameXYID_End );

		GameXY::ServerToClientMessage STC;
		STC.m_MsgID = xieyi.XY_ID;
		
		bostream	bos;
		bos.attach(STC.m_Message,MAX_MSGDTDA_LEN);
		bos<<xieyi;
		STC.m_MsgLen = short(bos.length());

		SendMsg(STC);

		return 0;
	}
};