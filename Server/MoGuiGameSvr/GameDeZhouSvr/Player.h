#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "server.h"
#include "GameServerSocket.h"
#include "DBServerXY.h"

#include "DezhouData.h"

class CServer;
class GameServerSocket;
class CGameTable;
class CGameRoom;

using namespace MoGui::Game::DeZhou;
using namespace MoGui::Game::DeZhou::GS;

class CPlayer;
typedef std::list<CPlayer*>      PLAYERLIST;
typedef AGBase::RefPtr<CPlayer>	 PlayerPtr;

class CPlayer : public AGBase::RefCount
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
	static int                    s_State[PLAYER_ST_END];                    //用于记录所有在线玩家的状态
	static int                    s_FinishValue[4];
	static DBS::VectorPTLevelInfo s_vectorGameLevelInfo;
	static DBS::MapPTLevelInfo    s_mapGameLevelInfo;
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

	BYTE                          m_PlayerType;                              //玩家当前的状态，是机器人，还是托管，还是真人在玩
	BYTE                          m_ClientType;
	BYTE                          m_PlayerState;                             //游戏状态，站在房间，还是在房间里，还是桌子上

	UINT32                        m_SocketCloseTime;                         //记录断线时间，仅在玩家为在线玩家的时候有效

	BYTE                          m_StateInfo;                               //主要是以位记录逻辑值位为1表示真0为假

public:
	UINT32                        m_KillTime;                                //玩家失效时间，超过10分钟将被清掉
	UINT32                        m_LockTime;                                //指玩家游戏币被锁住的时间，如果为零表示没有锁

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
	bool                          IsVisitorPlayer()const{ return m_PlayerType == PLAYER_TYPE_VISITOR; }
	bool                          IsAdmin()const { return m_PlayerType == PLAYER_TYPE_ADMIN; }
	bool                          IsCommondPlayer()const{ return m_PlayerType == PLAYER_TYPE_PLAYER; }
	bool                          IsClientPlayer()const{ return IsVisitorPlayer() || IsCommondPlayer() || IsAdmin(); }

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
	bool                          IsInMatchTable()const;
	

private:
	/////////////////////////玩家位置//////////////////////////////////
	UINT16                        m_RoomID;
	UINT16                        m_TableID;
	BYTE                          m_SitID;
	BYTE                          m_PlayerPlace;              //主要用于区别玩家客户端的位置
public:
	UINT16                        GetServerID()const;
	UINT16                        GetRoomID()const{ return m_RoomID; }
	void                          SetRoomID( UINT16 id){ m_RoomID=id; }
	UINT16                        GetTableID()const { return m_TableID; }
	void                          SetTableID( UINT16 id){ m_TableID=id; }
	BYTE                          GetSitID()const {return m_SitID;}
	void                          SetSitID( BYTE id){ assert(id>=0&&id<=MAX_PALYER_ON_TABLE);m_SitID=id; }
	void                          SetPlace(BYTE place){ m_PlayerPlace = place; }
	BYTE                          GetPlace(void)const{ return m_PlayerPlace; }

public:
	int                           m_ChangeName;
	std::string                   m_NickName;
	int                           m_Sex;

private:
	///////////////////////基本属性///////////////////////////////////
	UINT16                        m_AID;
	UINT32                        m_PID;

	std::string					  m_HeadPicURL;
	std::string                   m_HomePageURL;
	std::string                   m_City;

public:
    UINT16                        GetAID() const { return m_AID; }
	void                          SetAID(INT16 AID){ m_AID = AID;}
	UINT32                        GetPID() const { return m_PID; }
	void                          SetPID(UINT32 PID){ m_PID = PID;}
	const std::string&            GetNickName() const { return m_NickName; }
	const std::string&            GetHeadPicURL() const { return m_HeadPicURL; }
	const std::string&            GetHomePageURL() const { return m_HomePageURL; }
	const std::string&            GetCity()const { return m_City; }
	void                          SetCity(const std::string& strCity){ m_City = strCity; }

private:
	////////////////////////游戏属性////////////////////////////////////
	INT64                        m_nBankMoney;	
	INT32           			 m_nMoGuiMoney;                             //和人民币一元钱对应的价值，这个和游戏GameMoney要区分开来，	
	INT64       	 			 m_nGameMoney;                              //玩家身上的钱，和下面桌子的钱相加为总钱，当然这里没有计入银行的钱
    INT64                        m_nMatchJF;                                //用于竞换奖品的

public:
	unsigned char                m_OpenBank;
	std::vector<std::string>     m_vectorMoneyLog;

	INT64                        GetGameMoney()const{return m_nGameMoney;}; //取这个主要是用玩显示，不能参与任何数值的计算
	void                         DecGameMoney(INT64 nMoney);
	void                         AddGameMoney(INT64 nMoney);

	INT32                        GetMoGuiMoney()const{return m_nMoGuiMoney;};
	void                         AddMoGuiMoney(INT32 nMoney);
	void                         DecMoGuiMoney(INT32 nMoney);

	INT64                        GetMatchJF()const{return m_nMatchJF;};
	void                         AddMatchJF(INT64 nJF);

	INT64                        GetBankMoney(){ return m_nBankMoney;}
	void                         SetBankMoney(INT64 nMoney){ assert(nMoney>=0);if(nMoney>=0) m_nBankMoney=nMoney;}
	bool                         IsOpenBank(void){ return m_OpenBank>0; }

	void                         AddMoneyLog(INT64 nAddMoney, const std::string& strLog);
    void                         ClearMoneyLog();

public:
	INT64       	 			 m_nJF;                                     //积分 这个可以用消费数的计量，也用于等级的计算
	INT64       	 			 m_nEP;                                     //
	
	INT32        	 			 m_nWinTimes;
	INT32        	 			 m_nLossTimes;
	INT32                        m_nGameTime;

	INT64                        m_MaxPai;
	UINT32                       m_MaxPaiTime;
	INT64                        m_MaxMoney;
	UINT32                       m_MaxMoneyTime;
	INT64                        m_MaxWin;
	UINT32                       m_MaxWinTime;

	int                          m_WinRecord[WinTypeNum];
	int                          m_Achieve[4];
	std::vector<BYTE>            m_listHonor;

	UINT32                       m_InvitePID;                               //玩家被谁邀请进来游戏的
	UINT32                       m_JoinTime;                                //玩家加入游戏的时间
	UINT32                       m_LandTime;                                //玩家当前登陆时间
	UINT8                        m_BotLevel;                                //控制机器人玩家游戏币的数量

	BYTE                         m_VipLevel;                               //指玩家会员级别，比如红钻蓝钻等
	BYTE                         m_GameLevel;                              //基于游戏局数产生的等级
	
	int                          m_GiftID;
	INT32                        m_CurGiftIdx;                             //玩家当前戴的礼物
	UINT32                       m_FreeFaceTime;                           //表情免费的时间
	UINT32                       m_EndBotPlayerTime;                       //指机器人游戏结束的时间

public:
	INT64                        GetJF()const {return m_nJF;}
	INT64                        GetExperience()const {return m_nEP;}
	INT32                        GetWinLossTimes()const{ return m_nWinTimes+m_nLossTimes;}
	INT64                        GetMaxPai()const {return m_MaxPai; }
	UINT8                        GetBotLevel()const{ return m_BotLevel; }
    void                         UpdateGameLevel(bool bCheck);

	void                         CheckMaxGameMoney();
    void                         CheckMaxWinMoney(INT64 nWinMoney);
    void                         CheckMaxPai(DezhouLib::CPaiType& stPaiType);

	int                          IsHonorFinish(INT16 HonorID);
	void                         FinishHonor(INT16 HonorID);
	void                         DoFinishHonor(INT16 HonorID);
	bool                         IsFinishAchieve(int nSort);

	INT64                        m_WinLossMoneyInMatchRoom;                 //用于统计玩家在赢游币比赛场的输赢数量


	//-------设定玩家的运势，权限系统--------------------
	INT64           			 m_nUpperLimite;                            //指玩家的游戏币的上限总额，每局会增长但不充值会有上限封顶，转帐会降低，充值将放量上涨
	INT64           			 m_nLowerLimite;                            //指玩家的最低救济线，低于此值将权值自动上升

	int                          m_RightLevel;                              //级别
	int                          m_RightTimes;                              //次数

    //////////////////////玩家其它信息///////////////////////////////////
	typedef std::map<UINT16, UINT32>    MapSendRoomTableInfo;
	MapSendRoomTableInfo          m_mapRoomTime;                            //表示已经发送过房间里桌子列表的房间ID号	

	bool                          HaveSendTableInfoList( UINT16 roomid);
	void                          AddRoomHaveSendTableInfo( UINT16 roomid);
	UINT32                        GetSendTime(UINT16 roomid);                //发送每桌子玩家数的时间值，同时也标识了是否发送过桌子列表
	void                          SetSendTime(UINT16 roomid,UINT32 SendTime);

	bool                          m_RoomStateChange;                         //仅用于发送房间标签的玩家状态变化
	bool                          m_IsRoomShowPlayer;                        //是否是列表中显示的玩家
	UINT32                        m_RegisterRoomPlayerTime;

	UINT32                        m_ReqCityPlayerTime;                       //同城好友开始注册的时间
	void                          SetCityStateChange(bool bChange);
	bool                          IsCityStateChange();

	//好友相信息
	INT16                         m_FriendCount;                             //我关注的对象不超过500人
	ListPID                       m_listFollow;                              //我关注的
	ListPID                       m_listBeFollow;                            //关注我的
	bool                          IsFollow(UINT32 PID);
	bool                          IsBeFollow(UINT32 PID);
	void                          AddFollowFriend(UINT32 PID);
	void                          DeleteFollowFriend(UINT32 PID);
	void                          AddBeFollowFriend(UINT32 PID);
	void                          DeleteBeFollowFriend(UINT32 PID);
	void                          SetFriendStateChange(bool bChange);
	bool                          IsFriendStateChange();

	ListReqFriend                 m_listReqFriend;                           //请求加好友的玩家ID
	bool                          IsPIDInReqList(UINT32 PID);
	void                          DeleteReqFriendPID(UINT32 PID);
	void                          DeletePIDByTime(UINT32 LatestTime);
	void                          AddReqFriend(UINT32 PID,UINT32 CurTime);

	//礼物表
	typedef std::map<int,DBServerXY::DBS_msgUserGiftInfo>            MapUserGiftInfo;

	MapUserGiftInfo               m_curMapUserGiftInfo;
	MapUserGiftInfo               m_outMapUserGiftInfo;
	void                          DeleteGift(int GiftIdx);
	void                          AddGift(const DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int nFlag);
	bool                          GetCurUserGift(DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int GiftIdx);
	bool                          GetOutUserGift(DBServerXY::DBS_msgUserGiftInfo& stDBUgi,int GiftIdx);

public:
	//以下是各类有关玩家游戏任务以及功能的限定设置

	//送钱活动
	UINT32                        m_PromoteTimeStart;                       //记时开始的起点时间，比如坐下开始记时               
	INT8                          m_TimeStep;                               //表明当前进行的是第几步，
	INT32                         m_TimePass;                               //指当前步记时开始后已过去的时间，每一步领取后清0

	//连续登陆以及连续游戏
	UINT8                         m_ContinuPlay;
	UINT8                         m_ContinuLogin;

	//比赛
	UINT32                        m_ReqMatchRankTime;                       //指玩家请求比赛排行榜
	bool                          CanReqMatchRank(UINT32 curTime);

	//记录玩家行为的数据
	UINT32                        m_PlayerCreateTime;                        //记录游戏服务器创建玩家的时间
	UINT32                        m_PlayerSitTime;                           //玩家坐在位置上的时间

	int                           m_OneLifeGameTime;
	int                           m_OneLifeMatchTime;
	INT16                         m_EnterRoomTimes;
	INT16                         m_EnterTableTimes;
	INT16                         m_SitDownTimes;
	INT16                         m_OneLiftPlayTimes;

	UINT16                        m_TodayGameSecond;                         //当天游戏时间按秒算长度
	short                         m_TodayGameCount;                          //当天游戏时间按局数算
	UINT8                         m_HaveAwardInvite;                         //是否已经奖励过
	
	INT8                          m_JiuMingTimes;                            //当日赠送低保的次数

	//和任务奖章有关的信息
	int                           m_SendGiftTimes;
	int                           m_RecvGiftTimes;
	int                           m_TotalInvite;
	int                           m_ShowFaceTimes;

public:
	void     InitEveryDay();

	void     GetPlayerBaseInfo(PlayerBaseInfo& pbi);
	void     GetPlayerGameInfo(PlayerGameInfo& pgi);
	void     GetPlayerStateInfo(PlayerStateInfo& psi);

	void     SetPlayerData(const MoGui::MoGuiXY::DBServerXY::DBS_PlayerData& msgPD);
	void     SetPlayerDataEx(const MoGui::MoGuiXY::DBServerXY::DBS_PlayerDataEx& msgPDEx);
	void     SetBotPlayerData(const MoGui::MoGuiXY::DBServerXY::DBS_BotPlayerData& bpd);
	//void     SetPlayerMatchData(const MoGui::MoGuiXY::DBServerXY::DBS_PlayerMatchData& msgPMD);

	void     GetPlayerData(GameXY::Game_PlayerData& game_msgPD);
	void     GetPlayerDataEx(GameXY::Game_PlayerDataEx& game_msgPDEx);
	void     GetPlayerTailInfo(GameXY::PlayerTailInfo& msgPTI);

	bool     IsSendSameTablePlayer();
	void     SetSendSameTablePlayer(bool bSend);
	bool     IsShowLobby();
	void     SetShowLobby(bool bShow);//提示客户端显示大厅

	void     SendMaxPai();
	void     SendMaxWin();
	void     SendMaxMoney();
	void     SendPlayerData();
	void     SendPlayerDataEx();
	void     SendPlayerHonor();
	void     SendHonorToPlayer(PlayerPtr pPlayer);

	void     SendLastGiftInfo(PlayerPtr pPlayer);
	void     SendCurGiftInfo(PlayerPtr pPlayer);
	void     UpdateGameMoney();
	void     UpdateMoGuiMoney();
	void     UpdateGameInfo();

	void     GetHonorList(VectorHonorID& listHI);
	void     CheckPlayTimes();
	void     CheckWinTimes();
	void     CheckWinRecord(int nType);
	void     CheckSendGift();
	void     CheckRecvGift();
	void     CheckShowFace();
	void     CheckFriend();
	void     CheckWinMoney(INT64 nWinMoney);
	void     CheckOwnMoneyHonor();
	void     CheckJuBaoPeng(int nType);
	void     CheckContinuWin(int nContinuWin);
	void     CheckPlayAndInviteAward();
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