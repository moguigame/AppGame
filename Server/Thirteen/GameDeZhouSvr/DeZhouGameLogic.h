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

private:
	int 			               m_PID;                     //游戏玩家的ID，和坐位上的有可能不同，每局开始赋值一次
	int                            m_AID;                     //记录玩家的区域ID
	std::string                    m_NickName;
	std::string                    m_HeadPicURL;

	int                            m_PlayerType;
	int  		                   m_GameState;

	unsigned char                  m_HandPai[HANDPAICOUNT];	
	INT64                          m_TableMoney;
public:
	CGamePlayer() { Init(); }
	~CGamePlayer() {}
	void Init()
	{
		m_PID = 0;
		m_AID = 0;
		m_NickName = "";
		m_HeadPicURL = "";

		m_GameState = GAME_PLAYER_ST_NULL;
		m_PlayerType = PLAYER_TYPE_NONE;
	}

	bool           IsExit()const                { return m_GameState == GAME_PLAYER_ST_NULL;      }  //没有玩家或者玩家离开
	bool           IsWashOut()const             { return m_GameState == GAME_PLAYER_ST_WASHOUT;   }  //被淘汰玩家
	bool           IsWait()const                { return m_GameState == GAME_PLAYER_ST_WAITING;   }	 //等待中的玩家
	bool           IsPlaying()const             { return m_GameState == GAME_PLAYER_ST_PLAYING;   }  //正在游戏并且有钱继续
	bool           IsInGame()const              { return IsPlaying();                             }  //游戏玩家并且没有弃牌的，但包括ALLIN的玩家
	bool           IsAttendGame() const         { return IsPlaying();                             }  //是否参加过本轮游戏
	bool           IsHavePlayer()const          { return IsWait() || IsAttendGame();              }  //有玩家

	bool           IsBotPlayer()const           { return m_PlayerType == PLAYER_TYPE_BOT;         }
	bool           IsNotBotPlayer()const        { return m_PlayerType != PLAYER_TYPE_BOT;         }

	BYTE           GetGameState()const          { return m_GameState;    }
	void           SetGameState(BYTE gs)        { m_GameState = gs;      }
	void           SetPlayerType(BYTE pt)       { m_PlayerType = pt;     }

	int            GetPID()const                { return m_PID;          }
	void           SetPID(int PID)              { m_PID = PID;           }
	int            GetAID()const                { return m_AID;          }
	void           SetAID(int AID)              { m_AID = AID;           }

	void SetTableMoney(INT64 nMoney)
	{
		assert( nMoney>=0 );
		if ( nMoney >= 0 )
		{
			m_TableMoney = nMoney;
		}
	}
	INT64 GetTableMoney()
	{
		return m_TableMoney;
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
};

typedef map<int,vector<short> >                   MapBotFace;
typedef map<int,vector<string> >                  MapBotChat;
typedef vector<short>                             VectorFaceID;
typedef vector<short>                             VectorGiftID;
class CDeZhouGameLogic : public CMemoryPool_Public<CDeZhouGameLogic, 1>, public boost::noncopyable
{
public:
	explicit CDeZhouGameLogic(CGameTable* pTable);
	virtual ~CDeZhouGameLogic(void);

	typedef RefPtr<CPlayer>	                          PlayerPtr;
	typedef map<UINT32,PlayerPtr>                     MapPlayer;
	typedef map<UINT32,INT64>                         MapTablePlayerWinLoss;

	static CMapFunctionTime          s_LogicFuncTime;

public:
	CServer*                       m_pServer;
	CGameTable*                    m_pTable;
	CGameRoom*                     m_pRoom;
	const CGameServerConfig*       m_pGSConfig;

	UINT32                         m_StartGameTime;                                 //游戏开始的指定时间
	UINT32                         m_CurTime;
	string                         m_ClientTableRule;                               //发送给客户端的规则内容

	int 	                       m_TableType;                               		//桌子类型，比赛，普通等	
	int  						   m_nServiceMoney;                                 //每局的服务费，通常为大盲注的十分之一
	
	int                            m_BotLossEveryTime;                              //机器人每把理论输赢数量
	INT64                          m_BotRealWinLoss;                                //机器人实际输赢结果
	INT64                          m_BotTargetWinLoss;                              //机器人目标输赢结果

	BYTE                           m_TableBotFlag;
	BYTE                           m_TableBotPlayerNumber;                          //指本桌机器人数量，仅用于管理机器人
	BYTE						   m_MaxSitPlayerNumber;                      		//本桌最多坐的人数，主要是方便配成5，7，9人桌
	BYTE						   m_MinPlayerForGame;                              //开始游戏的最少玩家
	BYTE						   m_WaitChipTime;                                  //玩家下注等待的时间
	BYTE						   m_WaitStartTime;                                 //每局结束后，几秒后开始
	BYTE                           m_MaxBotChipTime;                                //机器人出牌的最大时间
	UINT32                         m_ManageBotPlayerTime;                           //管理机器人的时间
	UINT32                         m_timeAddBotPlayer;                              //按时间加机器人
	
	int 						   m_GameState;                               		//游戏状态
	int                            m_CurPlayingPlayerNumber;

	INT64                          m_GameLogicGUID;                                 //当前局数的UID标识
	INT64                          m_nGameWinLossMoney;                             //本局输赢结果汇总，肯定要为0或者小于0
	INT64                          m_nGameLossMoney;                                //本局游戏归公的部分，下注后玩家逃走，其它玩家赢不起的部分


	DezhouLib::CGamePaiLogic       m_GamePaiLogic;
	CGamePlayer                    m_GP[MAX_PALYER_ON_TABLE];
	stWinLossTally                 m_Tallys[MAX_PALYER_ON_TABLE];
	
	UINT32						   m_ActionTime;                                    //约定计时时间
	UINT32                         m_DelayTime;                                     //动作延时

	CGameLog*                      m_pGameLog;
	ListBlockMsg                   m_listLogicMsg;                                  //用于消息泵处理

public:
	void                           OnTimer(UINT32 curTime);
	string                         GetGameRule();
	int                            InitGameRule();
	int                            InitGameLogic();
	
	bool                           IsCommondTable()const{return m_TableType==TABLE_TYPE_COMMON;}

	bool                           IsGamePlaying()const { return m_GameState>=GAME_ST_HAND && m_GameState<=GAME_ST_RIVER; }
	bool                           IsGameIdle()const { return m_GameState==GAME_ST_NONE || m_GameState==GAME_ST_WAIT || m_GameState == GAME_ST_END; }
	inline bool                    IsAvailSitID(int SitID){ return SitID>0 && SitID<=m_MaxSitPlayerNumber; }
	bool                           IsGameLogicStop()const;
	
	int                            GetSitPlayerState(int SitID,UINT32 PID);

	bool                           IsSameTable(PlayerPtr LeftPlayer,PlayerPtr RightPlayer);
	bool                           IsFriend(PlayerPtr LeftPlayer,PlayerPtr RightPlayer);
	bool                           IsFriend(UINT32 LPID,UINT32 RPID);
	bool                           IsPlayerInTable(UINT32 PID);

    void                           DebugLogicState();
	void                           DebugAllPlayer();
	void                           DebugSitPlayer(int SitID);

	int                  		   GetPlayingPlayerNumber();
	int                            GetAvailPlayerNumber();         //本轮下过注或者能下注的玩家总数，如果<=1说明游戏结束
	int                            GetSitPlayerNumber();           //坐在桌子上的玩家（不包括淘汰和离开的
	int                            GetSitBotPlayerNumber();

	void                           WriteToDB(stWinLossTally* pWLT,int nLen);

	void                           StartGameLogic();
	void                           EndGameLogic();

	void                           InitGameData();
	bool                           CanStartGame();
	void                           StartCommonGame();
	
	bool                           GetSitPlayerInfo(PlayerPtr pPlayer,GameDeZhou_SitPlayerInfo& spi);
	void                           SendGameToPlayer(PlayerPtr pPlayer);

	void                           SendTableMoneyBySitID(const int SitID,PlayerPtr pPlayer=NULL);       //更新玩家桌面上的游戏币的数量
	void                           SendGameMoneyBySitID(const int SitID,PlayerPtr pPlayer=NULL);        //更新玩家总体游戏币的数量
	
	void                           SendGameState(PlayerPtr pPlayer=NULL);

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

	int                            OnGameMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int                            OnPlayerAddChip(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnReqShowPai(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnTableChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnPrivateChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnTableQuickChat(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnShowFace(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnSendMoney(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnSendGift(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnAddFriend(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnReqPromoteTime(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);
	int                            OnLogicReqTailInfo(PlayerPtr pPlayer,GameXY::ClientToServerMessage& CTS);

	void                           PlayerJoinGame(PlayerPtr pPlayer);
	void                           LogicStandUpPlayer(PlayerPtr pPlayer,int SitID,int Flag=Action_Flag_Player);
	void                           GamePlayerStandUp(PlayerPtr pPlayer,int SitID,int Flag=Action_Flag_Player);
	void                           GamePlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag=Action_Flag_Player);

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
