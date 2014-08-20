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
	int 			               m_PID;                     //��Ϸ��ҵ�ID������λ�ϵ��п��ܲ�ͬ��ÿ�ֿ�ʼ��ֵһ��
	int                            m_AID;                     //��¼��ҵ�����ID
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

	bool           IsExit()const                { return m_GameState == GAME_PLAYER_ST_NULL;      }  //û����һ�������뿪
	bool           IsWashOut()const             { return m_GameState == GAME_PLAYER_ST_WASHOUT;   }  //����̭���
	bool           IsWait()const                { return m_GameState == GAME_PLAYER_ST_WAITING;   }	 //�ȴ��е����
	bool           IsPlaying()const             { return m_GameState == GAME_PLAYER_ST_PLAYING;   }  //������Ϸ������Ǯ����
	bool           IsInGame()const              { return IsPlaying();                             }  //��Ϸ��Ҳ���û�����Ƶģ�������ALLIN�����
	bool           IsAttendGame() const         { return IsPlaying();                             }  //�Ƿ�μӹ�������Ϸ
	bool           IsHavePlayer()const          { return IsWait() || IsAttendGame();              }  //�����

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

	UINT32                         m_StartGameTime;                                 //��Ϸ��ʼ��ָ��ʱ��
	UINT32                         m_CurTime;
	string                         m_ClientTableRule;                               //���͸��ͻ��˵Ĺ�������

	int 	                       m_TableType;                               		//�������ͣ���������ͨ��	
	int  						   m_nServiceMoney;                                 //ÿ�ֵķ���ѣ�ͨ��Ϊ��äע��ʮ��֮һ
	
	int                            m_BotLossEveryTime;                              //������ÿ��������Ӯ����
	INT64                          m_BotRealWinLoss;                                //������ʵ����Ӯ���
	INT64                          m_BotTargetWinLoss;                              //������Ŀ����Ӯ���

	BYTE                           m_TableBotFlag;
	BYTE                           m_TableBotPlayerNumber;                          //ָ���������������������ڹ��������
	BYTE						   m_MaxSitPlayerNumber;                      		//�������������������Ҫ�Ƿ������5��7��9����
	BYTE						   m_MinPlayerForGame;                              //��ʼ��Ϸ���������
	BYTE						   m_WaitChipTime;                                  //�����ע�ȴ���ʱ��
	BYTE						   m_WaitStartTime;                                 //ÿ�ֽ����󣬼����ʼ
	BYTE                           m_MaxBotChipTime;                                //�����˳��Ƶ����ʱ��
	UINT32                         m_ManageBotPlayerTime;                           //��������˵�ʱ��
	UINT32                         m_timeAddBotPlayer;                              //��ʱ��ӻ�����
	
	int 						   m_GameState;                               		//��Ϸ״̬
	int                            m_CurPlayingPlayerNumber;

	INT64                          m_GameLogicGUID;                                 //��ǰ������UID��ʶ
	INT64                          m_nGameWinLossMoney;                             //������Ӯ������ܣ��϶�ҪΪ0����С��0
	INT64                          m_nGameLossMoney;                                //������Ϸ�鹫�Ĳ��֣���ע��������ߣ��������Ӯ����Ĳ���


	DezhouLib::CGamePaiLogic       m_GamePaiLogic;
	CGamePlayer                    m_GP[MAX_PALYER_ON_TABLE];
	stWinLossTally                 m_Tallys[MAX_PALYER_ON_TABLE];
	
	UINT32						   m_ActionTime;                                    //Լ����ʱʱ��
	UINT32                         m_DelayTime;                                     //������ʱ

	CGameLog*                      m_pGameLog;
	ListBlockMsg                   m_listLogicMsg;                                  //������Ϣ�ô���

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
	int                            GetAvailPlayerNumber();         //�����¹�ע��������ע��������������<=1˵����Ϸ����
	int                            GetSitPlayerNumber();           //���������ϵ���ң���������̭���뿪��
	int                            GetSitBotPlayerNumber();

	void                           WriteToDB(stWinLossTally* pWLT,int nLen);

	void                           StartGameLogic();
	void                           EndGameLogic();

	void                           InitGameData();
	bool                           CanStartGame();
	void                           StartCommonGame();
	
	bool                           GetSitPlayerInfo(PlayerPtr pPlayer,GameDeZhou_SitPlayerInfo& spi);
	void                           SendGameToPlayer(PlayerPtr pPlayer);

	void                           SendTableMoneyBySitID(const int SitID,PlayerPtr pPlayer=NULL);       //������������ϵ���Ϸ�ҵ�����
	void                           SendGameMoneyBySitID(const int SitID,PlayerPtr pPlayer=NULL);        //�������������Ϸ�ҵ�����
	
	void                           SendGameState(PlayerPtr pPlayer=NULL);

	//��������Ϸ��ʱ������Ϸ��
	bool                           IsRightStep(PlayerPtr pPlayer);
	void                           SendTimePromote(PlayerPtr pPlayer);
	void                           SitDownTimePromote(PlayerPtr pPlayer,UINT32 curTime);
	void                           StandUpTimePromote(PlayerPtr pPlayer,UINT32 curTime);

	//ͳ�������Ϊ
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

	//�����ô���
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
		int ExplainGameCTSMsg(GameXY::ClientToServerMessage& CTS,Txieyi& xieyi)//���ڽ���������װ����ϷЭ��
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
