#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "gamedef.h"
#include "player.h"

#include "publicdata.h"
#include "dezhoulib.h"
#include "GameLog.h"
#include "MessageBlock.h"
#include "dezhoulogicxy.h"

using namespace MoGui::Game::DeZhou;
using namespace MoGui::Game::DeZhou::GS;
using namespace MoGui::MoGuiXY::GameXY::DeZhou;

class CPlayer;
class CServer;
class CGameTable;
class CGameRoom;
class CMoneyTally;
class CGameLog;
class CMatchRule;
class CGameServerConfig;

class CGamePlayer
{
public:
    enum
	{
		GameFlag_AllIn             = 1,
        GameFlag_SendMoney         = 2,
        GameFlag_ShowFace          = 4,
    };

	INT64                          m_OtherLoss;               //所有其它的损耗均记录在这里面，包括发表情，送游戏币，买礼物等
	BYTE                           m_AutoOutTimes;            //连续自动出牌三次将被踢走

	GS::stHandPai                  m_HandPai;                 //玩家手上牌
	DezhouLib::CPaiType            m_PaiType;                 //玩家手上牌值

	int                            m_MinBlind;
	int                            m_MaxBlind;

	BYTE                           m_nContinuWin;             //连续胜利的次数

	//聚宝盆功能
	int                            m_JuBaoChip;
	short                          m_JuBaoCount;

	//记录比赛的值
    std::string                    m_NickName;
	std::string                    m_HeadPicURL;

	short                          m_nPosition;               //名次

private:
	UINT32			               m_PID;                     //游戏玩家的ID，和坐位上的有可能不同，每局开始赋值一次
	INT16                          m_AID;                     //记录玩家的区域ID

	BYTE                           m_PlayerType;
	BYTE		                   m_GameState;	
	INT16		                   m_ReqMoneyTime;            //请求加钱的时候不超过一分钟	
	INT16						   m_LastAction;              //玩家最后的操作选择，如果选择是看牌，那么会把牌给所有的人亮掉	

	INT64                          m_TableMoney;
	INT64                          m_OriginMoney;             //一局游戏开始后玩家的总注码
	INT64						   m_CurChip;                 //玩家本局本轮当前下注额

    BYTE                           m_GameFlag;                //主要记录各种逻辑值

public:
	CGamePlayer() { Init(); }
	~CGamePlayer() {}
	void Init()
	{
		m_PID = 0;
		m_AID = 0;

		m_LastAction = ACTION_NONE;
		m_ReqMoneyTime = 0;
		m_GameState = GAME_PLAYER_ST_NULL;
		m_PlayerType = PLAYER_TYPE_NONE;

		m_TableMoney = 0;
		m_OriginMoney = 0;
		m_CurChip = 0;

		m_OtherLoss = 0;
		m_AutoOutTimes = 0;

		m_HandPai.Init();
		m_PaiType.Init();

		m_MinBlind = 0;
		m_MaxBlind = 0;

		m_nContinuWin = 0;

        m_GameFlag = 0;

		m_JuBaoChip = 0;
		m_JuBaoCount = 0;

		m_NickName = "";
		m_HeadPicURL = "";
		m_nPosition = 0;
	}

	bool           IsExit()const                { return m_GameState == GAME_PLAYER_ST_NULL;      }  //没有玩家或者玩家离开
	bool           IsWashOut()const             { return m_GameState == GAME_PLAYER_ST_WASHOUT;   }  //被淘汰玩家
	bool           IsWait()const                { return m_GameState == GAME_PLAYER_ST_WAITING;   }	 //等待中的玩家
	bool           IsPlaying()const             { return m_GameState == GAME_PLAYER_ST_PLAYING;   }  //正在游戏并且有钱继续
	bool           IsGiveUp()const              { return m_GameState == GAME_PLAYER_ST_GIVEUP;    }  //弃牌的玩家
	bool           IsAllIn()const               { return m_GameState == GAME_PLAYER_ST_ALLIN;     }  //ALLIN的玩家
	bool           IsInGame()const              { return IsPlaying() || IsAllIn();                }  //游戏玩家并且没有弃牌的，但包括ALLIN的玩家
	bool           IsAttendGame() const         { return IsGiveUp() || IsAllIn() || IsPlaying();  }  //是否参加过本轮游戏
	bool           IsHavePlayer()const          { return IsWait() || IsAttendGame();              }  //有玩家

	bool           IsBotPlayer()const           { return m_PlayerType == PLAYER_TYPE_BOT;         }
	bool           IsVisitorPlayer()const       { return m_PlayerType == PLAYER_TYPE_VISITOR;     }
	bool           IsNotBotPlayer()const        { return m_PlayerType != PLAYER_TYPE_BOT;         }

	BYTE           GetGameState()const          { return m_GameState;    }
	void           SetGameState(BYTE gs)        { m_GameState = gs;      }
	INT16          GetReqMoneyTime()const       { return m_ReqMoneyTime; }
	void           SetReqMoneyTime(INT16 MT)    { m_ReqMoneyTime = MT;   }
	void           SetPlayerType(BYTE pt)       { m_PlayerType = pt;     }

	UINT32         GetPID()const                { return m_PID;          }
	void           SetPID(UINT32 PID)           { m_PID = PID;           }
	INT16          GetAID()const                { return m_AID;          }
	void           SetAID(INT16 AID)            { m_AID = AID;           }
	INT16          GetLastAction()const         { return m_LastAction;   }
	void           SetLastAction(INT16 Action)  { m_LastAction = Action; }

	INT64          GetRealTableMoney()const     { assert(m_TableMoney-m_CurChip>=0);return m_TableMoney-m_CurChip;}
	INT64          GetTableMoney()const         { assert(m_TableMoney>=0); return m_TableMoney;  }
	INT64          GetOriginMoney()const        { assert(m_OriginMoney>=0);return m_OriginMoney; }
	void           SetOriginMoney(INT64 nMoney) { assert(nMoney>=0); m_OriginMoney = nMoney;     }
	INT64          GetCurChip()const            { assert(m_CurChip>=0);return m_CurChip;         }
	void           SetCurChip(INT64 nMoney)     { assert(nMoney>=0);m_CurChip = nMoney;          }

	INT64 GetWinLossMoney()
	{
		return m_TableMoney-m_CurChip+m_OtherLoss-m_OriginMoney;
	}
	INT64 GetAllHaveChipMoney()
	{
		return m_OriginMoney-m_OtherLoss-m_TableMoney+m_CurChip;
	}
	INT64 GetTotalCanChipMoney()
	{
		return m_OriginMoney-m_OtherLoss;
	}
	void SetTableMoney(INT64 nMoney)
	{
		assert( nMoney>=0 );
		if ( nMoney >= 0 )
		{
			m_TableMoney = nMoney;
		}
	}
	void AddTableMoney(INT64 nMoney)
	{
		assert(nMoney>=0);
		if (nMoney > 0)
		{
			INT64 OldMoney = m_TableMoney;
			m_TableMoney += nMoney;
			if ( m_TableMoney < OldMoney )
			{
				m_TableMoney = OldMoney;
			}
		}

		assert( m_TableMoney>=0 );
	}
	void DecTableMoney(INT64 nMoney)
	{
		assert(nMoney>=0);
		if ( nMoney > 0)
		{
			m_TableMoney -= nMoney;
			if ( m_TableMoney < 0 )
			{
				m_TableMoney = 0;
			}
		}
		assert( m_TableMoney>=0 );
	}
	void SetAllIn(bool bAllIn)
	{
		if ( bAllIn ) m_GameFlag |= GameFlag_AllIn;
		else m_GameFlag &= (~GameFlag_AllIn);
	}
	bool GetAllIn()
	{
		return (m_GameFlag&GameFlag_AllIn)==GameFlag_AllIn;
	}
	void SetHaveSendMoney(bool bSendMoney)
	{
		if ( bSendMoney ) m_GameFlag |= GameFlag_SendMoney;
		else m_GameFlag &= (~GameFlag_SendMoney);
	}
	bool GetHaveSendMoney()
	{
		return (m_GameFlag&GameFlag_SendMoney) == GameFlag_SendMoney;
	}
	void SetShowFace(bool bShowFace)
	{
		if ( bShowFace ) m_GameFlag |= GameFlag_ShowFace;
		else m_GameFlag &= (~GameFlag_ShowFace);
	}
	bool GetShowFace()
	{
		return (m_GameFlag&GameFlag_ShowFace) == GameFlag_ShowFace;
	}
	bool GetJuBaoPeng()
	{
		return m_JuBaoCount>0 && m_JuBaoChip>0;
	}
};

typedef std::map<int, std::vector<short> >                   MapBotFace;
typedef std::map<int, std::vector<std::string> >             MapBotChat;
typedef std::vector<short>                                   VectorFaceID;
typedef std::vector<short>                                   VectorGiftID;
class CDeZhouGameLogic : public AGBase::CMemoryPool_Public<CDeZhouGameLogic, 1>, public boost::noncopyable
{
public:
	explicit CDeZhouGameLogic(CGameTable* pTable);
	virtual ~CDeZhouGameLogic(void);

	typedef AGBase::RefPtr<CPlayer>	                        PlayerPtr;
	typedef std::map<UINT32, PlayerPtr>                     MapPlayer;
	typedef std::map<UINT32, INT64>                         MapTablePlayerWinLoss;

	static int s_nWinTimes[MAX_PALYER_ON_TABLE];
	static MapBotFace                s_mapBotFace;
	static MapBotChat                s_mapBotChat;
	static CMapFunctionTime          s_LogicFuncTime;

	static VectorFaceID              s_KaiXinFaceID;
	static VectorFaceID              s_ShangXinFaceID;
	static VectorFaceID              s_WuLiaoFaceID;
	static VectorFaceID              s_WinFaceID;
	static VectorFaceID              s_LossFaceID;
	static VectorFaceID              s_ThankFaceID;

	static VectorGiftID              s_ThankGiftID;

	static int  InitStaticData();

public:
	CServer*                       m_pServer;
	CGameTable*                    m_pTable;
	CGameRoom*                     m_pRoom;
	const CGameServerConfig*       m_pGSConfig;

	UINT32                         m_StartGameTime;                                 //游戏开始的指定时间
	UINT32                         m_CurTime;
	std::string                    m_ClientTableRule;                               //发送给客户端的规则内容

	BYTE	                       m_TableType;                               		//桌子类型，比赛，普通等
	BYTE						   m_Limite;                                        //是否为有限桌

	INT32						   m_nBigBlind;                                     //本桌大盲注
	INT32						   m_nSmallBlind;                                   //本桌小盲注
	INT64						   m_nMaxTakeIn;                                    //最大带入量
	INT64			               m_nMinTakeIn;                                    //最小带入量
	INT32						   m_nPotMoney;                                     //有底桌开始的下注量
	INT32						   m_nMinLeftForGame;                               //指玩家最小桌子上的钱，不能输光，小于这些钱将停止发牌
	INT32						   m_nServiceMoney;                                 //每局的服务费，通常为大盲注的十分之一
	
	int                            m_BotLossEveryTime;                              //机器人每把理论输赢数量
	INT64                          m_BotRealWinLoss;                                //机器人实际输赢结果
	INT64                          m_BotTargetWinLoss;                              //机器人目标输赢结果

	INT32                          m_GiftBase;
	INT32                          m_FaceBase;

	BYTE                           m_TableBotFlag;
	BYTE                           m_TableBotPlayerNumber;                          //指本桌机器人数量，仅用于管理机器人
	BYTE						   m_MaxSitPlayerNumber;                      		//本桌最多坐的人数，主要是方便配成5，7，9人桌
	BYTE						   m_MinPlayerForGame;                              //开始游戏的最少玩家
	BYTE						   m_WaitChipTime;                                  //玩家下注等待的时间
	BYTE						   m_WaitStartTime;                                 //每局结束后，几秒后开始
	BYTE                           m_MaxBotChipTime;                                //机器人出牌的最大时间
	UINT32                         m_ManageBotPlayerTime;                           //管理机器人的时间
	UINT32                         m_timeAddBotPlayer;                              //按时间加机器人
	bool                           m_bEndTable;                                     //是否游戏时间已经到了
	bool                           m_bAllKanPai;
	bool                           m_bNeedBotWin;
	
	BYTE						   m_GameState;                               		//游戏状态
	BYTE                           m_CurPlayingPlayerNumber;

	INT64                          m_GameLogicGUID;                                 //当前局数的UID标识

	INT64                          m_nGameWinLossMoney;                             //本局输赢结果汇总，肯定要为0或者小于0
	INT64                          m_nGameLossMoney;                                //本局游戏归公的部分，下注后玩家逃走，其它玩家赢不起的部分
	
	//比赛模式
	CMatchRule*                    m_pMatchRule;

	DezhouLib::CGamePaiLogic       m_GamePaiLogic;
	TablePoolMoney                 m_TablePoolMoneys[MAX_PALYER_ON_TABLE];          //桌面的分池
	CGamePlayer                    m_GP[MAX_PALYER_ON_TABLE];
	stWinLossTally                 m_Tallys[MAX_PALYER_ON_TABLE];	

	BYTE		                   m_BankerSitID;                                  	//庄家坐号
	BYTE		                   m_BigBlindSitID;                                	//大盲注坐号
	BYTE		                   m_SmallBlindSitID;                              	//小盲注坐号
	BYTE		                   m_CurPlayerSitID;                               	//当前玩家坐号

	BYTE		                   m_CanShowPaiSitID;                               //最后有机会亮牌的玩家
	BYTE		                   m_FirstPlayerSitID;                              //每一轮的第一位玩家
	BYTE		                   m_EndPlayerSitID;                                //结束的玩家

	INT64						   m_MaxChipOnTable;                                //当前下的最大注
	INT64			               m_MinAddChip;                                    //当前最小加注量
	
	UINT32						   m_ActionTime;                                    //约定计时时间
	UINT32                         m_DelayTime;                                     //动作延时

	CGameLog*                      m_pGameLog;
	ListBlockMsg                   m_listLogicMsg;                                  //用于消息泵处理
	
	//这个主要是提供给自建房间场的玩家用的
	VectorPID                      m_vectorWinLossPID;                              //用于记录当前局有变化的玩家
	MapTablePlayerWinLoss          m_mapTableWinLoss;                               //用于记录玩家在本桌的输赢情况，用于自建桌等

	VectorPID                      m_vecKikOutPID;                                  //将被踢出的玩家
	std::list<stForbidenPlayer>    m_ForbidenPlayerList;                            //禁止进入的玩家列表

public:
	void                           OnTimer(UINT32 curTime);
	std::string                    GetGameRule();
	int                            InitGameRule();
	int                            InitGameLogic();
	bool                           IsTableMatch()const{return m_TableType==TABLE_TYPE_TableMatch;}
	bool                           IsRoomMatch()const{return m_TableType==TABLE_TYPE_RoomMatch;}
	bool                           IsMatchTable()const{return IsTableMatch()|| IsRoomMatch();}
	bool                           IsCommondTable()const{return m_TableType==TABLE_TYPE_COMMON;}
	bool                           IsGamePlaying()const { return m_GameState>=GAME_ST_HAND && m_GameState<=GAME_ST_RIVER; }
	bool                           IsGameIdle()const { return m_GameState==GAME_ST_NONE || m_GameState==GAME_ST_WAIT || m_GameState == GAME_ST_END; }
	inline bool                    IsAvailSitID(int SitID){ return SitID>0 && SitID<=m_MaxSitPlayerNumber; }
	INT64                          GetPlayerRealTableMoneyBySitID(int SitID);
	bool                           IsMatching()const;
	bool                           IsGameLogicStop()const;
	void                           RecordWinLoss(UINT32 PID,INT64 nWinLoss);
	void                           ChangeBotBlind();
	INT64                          GetCurMaxChipOnTable();
	int                            GetSitPlayerState(int SitID,UINT32 PID);

	bool                           IsSameTable(PlayerPtr LeftPlayer,PlayerPtr RightPlayer);
	bool                           IsFriend(PlayerPtr LeftPlayer,PlayerPtr RightPlayer);
	bool                           IsFriend(UINT32 LPID,UINT32 RPID);
	bool                           IsPlayerInTable(UINT32 PID);

	bool                           GetGiftInfo(INT16 GiftID,DBServerXY::DBS_msgGiftInfo& GiftInfo);
	bool                           GetFaceInfo(INT16 FaceID,DBServerXY::DBS_msgFaceInfo& FaceInfo);

    void                           DebugLogicState();
	void                           DebugAllPlayer();
	void                           DebugSitPlayer(int SitID);

	BYTE                  		   GetNextPlayingPlayerSitID( BYTE CurSitID);
	BYTE                  		   GetPrePlayingPlayerSitID( BYTE CurSitID);

	int                  		   GetPlayingPlayerNumber();
	int                            GetAvailPlayerNumber();         //本轮下过注或者能下注的玩家总数，如果<=1说明游戏结束
	int                            GetSitPlayerNumber();           //坐在桌子上的玩家（不包括淘汰和离开的
	int                            GetSitBotPlayerNumber();

	BYTE                           CanSitDown( PlayerPtr pPlayer,BYTE SitID,INT64& nTakeMoney );
	void                           SysWinMoney(INT64 nMoney);
	void                           WriteToDB(stWinLossTally* pWLT,int nLen);

	void                           StartGameLogic();
	void                           EndGameLogic();

	void                           EndMatchPlayer(PlayerPtr pPlayer,int SitID);
	void                           EndTableMatch(PlayerPtr pWinPlayer,int SitID);
	void                           EndRoomMatch(PlayerPtr pPlayer,int SitID);

	void                           InitGameData();
	bool                           CanStartGame();
	void                           StartCommonGame();
	void                           StartMatchGame();

	void                           OnHand();
	void                           AutoChip();
	void                           BotPlayerChip();
	void                           PackMoney();
	void                           OnFlop();
	void                           OnTurn();
	void                           OnRiver();
	void                           EndGame();
	void                           OnDivideMoneyEnd();    //在游戏结束并分完筹码后，更新玩家数据，初始化等用的

	bool                           IsSitWin(int Sit);
	void                           SetBotPlayerBlind();   //主要是指对普通桌的机器人而言的
	void                           AddBotJuBaoPeng();
	
	bool                           GetSitPlayerInfo(PlayerPtr pPlayer,GameDeZhou_SitPlayerInfo& spi);
	void                           SendGameToPlayer(PlayerPtr pPlayer);
	void                           SendMatchInfoToPlayer(PlayerPtr pPlayer);
	void                           SendPlayerPower(PlayerPtr pPlayer=NULL);
	void                           SendTableMoneyBySitID(const int& SitID,PlayerPtr pPlayer=NULL);       //更新玩家桌面上的游戏币的数量
	void                           SendGameMoneyBySitID(const int& SitID,PlayerPtr pPlayer=NULL);        //更新玩家总体游戏币的数量
	void                           SendTablePai(BYTE State,PlayerPtr pPlayer=NULL);
	void                           SendBankerInfo(PlayerPtr pPlayer=NULL);
	void                           SendMoneyPool(PlayerPtr pPlayer=NULL);
	void                           SendGameState(PlayerPtr pPlayer=NULL);
	void                           SendAllJuBaoPeng(PlayerPtr pPlayer);
	
	void                           SendReqMoney(int SitID,UINT32 PID);
	void                           SendWashOutPlayer(int SitID,PlayerPtr pPlayer=NULL);
	void                           SendTableWinLoss(PlayerPtr pPlayer);

    void                           CheckPlayerJuBaoPeng(int SitID,int nPaiType,msgWinJuBaoPeng& msgWJBP);
	void                           BotShowFace(int SitID,int FaceID);

	//在线玩游戏按时长送游戏币
	bool                           IsRightStep(PlayerPtr pPlayer);
	void                           SendTimePromote(PlayerPtr pPlayer);
	void                           SitDownTimePromote(PlayerPtr pPlayer,UINT32 curTime);
	void                           StandUpTimePromote(PlayerPtr pPlayer,UINT32 curTime);

	//统计玩家行为
	void                           SitDownPlayerAction(PlayerPtr pPlayer);
	void                           StandUpPlayerAction(PlayerPtr pPlayer);

	void                           OnTimeAutoChip();
	void                           InitBotPlayer();
	void                           OnTimeManageBotPlayer(UINT32 curTime);
	void                           OnTimeAddTableMatchBotPlayer();

	//比赛部分
	void                           SendChangeBlind(int BigBlind,int SmallBlind,PlayerPtr pPlayer=NULL);
	void                           InitMatchData();

	int                            OnGameMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int                            OnPlayerAddChip(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnReqShowPai(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnAddMoney(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnTableChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnPrivateChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnTableQuickChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnShowFace(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnSendMoney(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnSendGift(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnAddFriend(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnReqPromoteTime(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnLogicReqTailInfo(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnReqChangeGift(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnReqAddTableTime(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnJoinJuBaoPeng(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);

	void                           DoJuBaoPengMsg();
	void                           DoAddMoney();
	void                           DoWashOutPlayer();
	void                           CheckLogicMsg();
	void                           DoShowFaceMsg();
	void                           DoSendGiftMsg();

	void                           PlayerJoinGame(PlayerPtr pPlayer);
	void                           LogicStandUpPlayer(PlayerPtr pPlayer,int SitID,int Flag=Action_Flag_Player);
	void                           GamePlayerStandUp(PlayerPtr pPlayer,int SitID,int Flag=Action_Flag_Player);
	void                           MatchPlayerStandUp(PlayerPtr pPlayer,int SitID,int Flag=Action_Flag_Player);
	void                           GamePlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag=Action_Flag_Player);
	void                           MatchPlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag=Action_Flag_Player);

	//测试用代码
	void                           TestSitPlayer();
	void                           TestGPPlayer(PlayerPtr pPlayer,int SitID);

	template<class Txieyi>
	inline void PushMsgInList(Txieyi& xieyi)
	{
		PMsgBlock pMsg(new MessageBlock());
		if ( pMsg.get() )
		{
			MakeMsgToBlock(xieyi,*pMsg);
			pMsg->m_nTimeCount = Tool::GetMilliSecond();
			m_listLogicMsg.push_back(pMsg);
		}
		else
		{
			printf_s("Error PushMsgInList ID=%d\n",xieyi.XY_ID);			
		}
	}

	template<class Txieyi>
		inline void LogicSendMsgToDBServer(Txieyi& xieyi)
	{
		if ( m_pServer )
		{
			m_pServer->SendMsgToDBServer(xieyi);
		}
		else
		{
			m_pTable->DebugError("LogicSendMsgToDBServer Error!");
		}
	}

	template<class Txieyi>
		void SendLogicMsgToOnePlayer(Txieyi& xieyi,UINT32 PID)
	{
		PlayerPtr pPlayer = m_pTable->GetTablePlayerByPID(PID);
		if( pPlayer && pPlayer->IsNeedSendMsg() )
		{
			pPlayer->SendGameLogicMsg(xieyi);
		}
	}
	template<class Txieyi>
		void SendLogicMsgToOnePlayer(Txieyi& xieyi,PlayerPtr pPlayer)
	{		
		if( pPlayer && pPlayer->IsNeedSendMsg())
		{
			pPlayer->SendGameLogicMsg(xieyi);
		}	
	}
	template<class Txieyi>
		void SendLogicMsgToAllPlayer(Txieyi& xieyi)
	{
		PlayerPtr pPlayer,TempPlayer = NULL;
		while( pPlayer = m_pTable->GetNextPlayer( TempPlayer ) )
		{
			TempPlayer = pPlayer;
			if ( pPlayer->IsNeedSendMsg() )
			{
				pPlayer->SendGameLogicMsg(xieyi);
			}
		}
	}
	template<class Txieyi>
		void SendLogicMsgToSeePlayer(Txieyi& xieyi)
	{
		PlayerPtr pPlayer,TempPlayer = NULL;
		while( pPlayer = m_pTable->GetNextPlayer( TempPlayer ) )
		{
			TempPlayer = pPlayer;
			if ( pPlayer->IsSeeing() && pPlayer->IsNeedSendMsg() )
			{
				pPlayer->SendGameLogicMsg(xieyi);
			}		
		}
	}
	template<class Txieyi>
		void SendLogicMsgToPlayingPlayer(Txieyi& xieyi)
	{
		PlayerPtr pPlayer,TempPlayer = NULL;
		while( pPlayer = m_pTable->GetNextPlayer( TempPlayer ) )
		{
			TempPlayer = pPlayer;
			if ( pPlayer->IsPlaying() && (!pPlayer->IsBotPlayer()) )
			{
				pPlayer->SendGameLogicMsg(xieyi);
			}		
		}
	}

	template<class Txieyi>
		void MakeLogicCTS(Txieyi& xieyi,GameXY::ClientToServerMessage& CTS)
	{
		CTS.ReSet();
		CTS.m_MsgID = xieyi.XY_ID;

		bostream bos;
		bos.attach(CTS.m_Message,MAX_MSGDTDA_LEN);
		bos<<xieyi;

		CTS.m_MsgLen = static_cast<short>(bos.length());
	}
	template<class Txieyi>
		int ExplainGameCTSMsg(GameXY::ClientToServerMessage& CTS,Txieyi& xieyi)//用于解释有外层包装的游戏协议
	{
		int ret = 0;

		bistream bis;
		bis.attach(CTS.m_Message,CTS.m_MsgLen);

		try
		{
			bis >> xieyi;
		}
		catch ( agproexception* e )
		{
			ret = 10000+e->m_cause;
		}
		catch ( biosexception* e )
		{
			ret = 20000+e->m_cause;
		}
		catch(...)
		{
			ret = 30000;
		}

		if ( ret )
		{
			m_pTable->DebugError("ExplainGameCTSMsg id=%d len=%d",CTS.m_MsgID,CTS.m_MsgLen);
		}

		return ret;
	}

#define TransplainMsgCTS(CTS,xieyi)        \
	if ( ExplainGameCTSMsg(CTS,xieyi) )    \
	{	                                   \
		return SOCKET_MSG_ERROR_STREAM;    \
	}
};
