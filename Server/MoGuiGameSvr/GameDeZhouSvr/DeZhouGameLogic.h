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

	INT64                          m_OtherLoss;               //������������ľ���¼�������棬���������飬����Ϸ�ң��������
	BYTE                           m_AutoOutTimes;            //�����Զ��������ν�������

	GS::stHandPai                  m_HandPai;                 //���������
	DezhouLib::CPaiType            m_PaiType;                 //���������ֵ

	int                            m_MinBlind;
	int                            m_MaxBlind;

	BYTE                           m_nContinuWin;             //����ʤ���Ĵ���

	//�۱��蹦��
	int                            m_JuBaoChip;
	short                          m_JuBaoCount;

	//��¼������ֵ
    std::string                    m_NickName;
	std::string                    m_HeadPicURL;

	short                          m_nPosition;               //����

private:
	UINT32			               m_PID;                     //��Ϸ��ҵ�ID������λ�ϵ��п��ܲ�ͬ��ÿ�ֿ�ʼ��ֵһ��
	INT16                          m_AID;                     //��¼��ҵ�����ID

	BYTE                           m_PlayerType;
	BYTE		                   m_GameState;	
	INT16		                   m_ReqMoneyTime;            //�����Ǯ��ʱ�򲻳���һ����	
	INT16						   m_LastAction;              //������Ĳ���ѡ�����ѡ���ǿ��ƣ���ô����Ƹ����е�������	

	INT64                          m_TableMoney;
	INT64                          m_OriginMoney;             //һ����Ϸ��ʼ����ҵ���ע��
	INT64						   m_CurChip;                 //��ұ��ֱ��ֵ�ǰ��ע��

    BYTE                           m_GameFlag;                //��Ҫ��¼�����߼�ֵ

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

	bool           IsExit()const                { return m_GameState == GAME_PLAYER_ST_NULL;      }  //û����һ�������뿪
	bool           IsWashOut()const             { return m_GameState == GAME_PLAYER_ST_WASHOUT;   }  //����̭���
	bool           IsWait()const                { return m_GameState == GAME_PLAYER_ST_WAITING;   }	 //�ȴ��е����
	bool           IsPlaying()const             { return m_GameState == GAME_PLAYER_ST_PLAYING;   }  //������Ϸ������Ǯ����
	bool           IsGiveUp()const              { return m_GameState == GAME_PLAYER_ST_GIVEUP;    }  //���Ƶ����
	bool           IsAllIn()const               { return m_GameState == GAME_PLAYER_ST_ALLIN;     }  //ALLIN�����
	bool           IsInGame()const              { return IsPlaying() || IsAllIn();                }  //��Ϸ��Ҳ���û�����Ƶģ�������ALLIN�����
	bool           IsAttendGame() const         { return IsGiveUp() || IsAllIn() || IsPlaying();  }  //�Ƿ�μӹ�������Ϸ
	bool           IsHavePlayer()const          { return IsWait() || IsAttendGame();              }  //�����

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

	UINT32                         m_StartGameTime;                                 //��Ϸ��ʼ��ָ��ʱ��
	UINT32                         m_CurTime;
	std::string                    m_ClientTableRule;                               //���͸��ͻ��˵Ĺ�������

	BYTE	                       m_TableType;                               		//�������ͣ���������ͨ��
	BYTE						   m_Limite;                                        //�Ƿ�Ϊ������

	INT32						   m_nBigBlind;                                     //������äע
	INT32						   m_nSmallBlind;                                   //����Сäע
	INT64						   m_nMaxTakeIn;                                    //��������
	INT64			               m_nMinTakeIn;                                    //��С������
	INT32						   m_nPotMoney;                                     //�е�����ʼ����ע��
	INT32						   m_nMinLeftForGame;                               //ָ�����С�����ϵ�Ǯ��������⣬С����ЩǮ��ֹͣ����
	INT32						   m_nServiceMoney;                                 //ÿ�ֵķ���ѣ�ͨ��Ϊ��äע��ʮ��֮һ
	
	int                            m_BotLossEveryTime;                              //������ÿ��������Ӯ����
	INT64                          m_BotRealWinLoss;                                //������ʵ����Ӯ���
	INT64                          m_BotTargetWinLoss;                              //������Ŀ����Ӯ���

	INT32                          m_GiftBase;
	INT32                          m_FaceBase;

	BYTE                           m_TableBotFlag;
	BYTE                           m_TableBotPlayerNumber;                          //ָ���������������������ڹ��������
	BYTE						   m_MaxSitPlayerNumber;                      		//�������������������Ҫ�Ƿ������5��7��9����
	BYTE						   m_MinPlayerForGame;                              //��ʼ��Ϸ���������
	BYTE						   m_WaitChipTime;                                  //�����ע�ȴ���ʱ��
	BYTE						   m_WaitStartTime;                                 //ÿ�ֽ����󣬼����ʼ
	BYTE                           m_MaxBotChipTime;                                //�����˳��Ƶ����ʱ��
	UINT32                         m_ManageBotPlayerTime;                           //��������˵�ʱ��
	UINT32                         m_timeAddBotPlayer;                              //��ʱ��ӻ�����
	bool                           m_bEndTable;                                     //�Ƿ���Ϸʱ���Ѿ�����
	bool                           m_bAllKanPai;
	bool                           m_bNeedBotWin;
	
	BYTE						   m_GameState;                               		//��Ϸ״̬
	BYTE                           m_CurPlayingPlayerNumber;

	INT64                          m_GameLogicGUID;                                 //��ǰ������UID��ʶ

	INT64                          m_nGameWinLossMoney;                             //������Ӯ������ܣ��϶�ҪΪ0����С��0
	INT64                          m_nGameLossMoney;                                //������Ϸ�鹫�Ĳ��֣���ע��������ߣ��������Ӯ����Ĳ���
	
	//����ģʽ
	CMatchRule*                    m_pMatchRule;

	DezhouLib::CGamePaiLogic       m_GamePaiLogic;
	TablePoolMoney                 m_TablePoolMoneys[MAX_PALYER_ON_TABLE];          //����ķֳ�
	CGamePlayer                    m_GP[MAX_PALYER_ON_TABLE];
	stWinLossTally                 m_Tallys[MAX_PALYER_ON_TABLE];	

	BYTE		                   m_BankerSitID;                                  	//ׯ������
	BYTE		                   m_BigBlindSitID;                                	//��äע����
	BYTE		                   m_SmallBlindSitID;                              	//Сäע����
	BYTE		                   m_CurPlayerSitID;                               	//��ǰ�������

	BYTE		                   m_CanShowPaiSitID;                               //����л������Ƶ����
	BYTE		                   m_FirstPlayerSitID;                              //ÿһ�ֵĵ�һλ���
	BYTE		                   m_EndPlayerSitID;                                //���������

	INT64						   m_MaxChipOnTable;                                //��ǰ�µ����ע
	INT64			               m_MinAddChip;                                    //��ǰ��С��ע��
	
	UINT32						   m_ActionTime;                                    //Լ����ʱʱ��
	UINT32                         m_DelayTime;                                     //������ʱ

	CGameLog*                      m_pGameLog;
	ListBlockMsg                   m_listLogicMsg;                                  //������Ϣ�ô���
	
	//�����Ҫ���ṩ���Խ����䳡������õ�
	VectorPID                      m_vectorWinLossPID;                              //���ڼ�¼��ǰ���б仯�����
	MapTablePlayerWinLoss          m_mapTableWinLoss;                               //���ڼ�¼����ڱ�������Ӯ����������Խ�����

	VectorPID                      m_vecKikOutPID;                                  //�����߳������
	std::list<stForbidenPlayer>    m_ForbidenPlayerList;                            //��ֹ���������б�

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
	int                            GetAvailPlayerNumber();         //�����¹�ע��������ע��������������<=1˵����Ϸ����
	int                            GetSitPlayerNumber();           //���������ϵ���ң���������̭���뿪��
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
	void                           OnDivideMoneyEnd();    //����Ϸ�������������󣬸���������ݣ���ʼ�����õ�

	bool                           IsSitWin(int Sit);
	void                           SetBotPlayerBlind();   //��Ҫ��ָ����ͨ���Ļ����˶��Ե�
	void                           AddBotJuBaoPeng();
	
	bool                           GetSitPlayerInfo(PlayerPtr pPlayer,GameDeZhou_SitPlayerInfo& spi);
	void                           SendGameToPlayer(PlayerPtr pPlayer);
	void                           SendMatchInfoToPlayer(PlayerPtr pPlayer);
	void                           SendPlayerPower(PlayerPtr pPlayer=NULL);
	void                           SendTableMoneyBySitID(const int& SitID,PlayerPtr pPlayer=NULL);       //������������ϵ���Ϸ�ҵ�����
	void                           SendGameMoneyBySitID(const int& SitID,PlayerPtr pPlayer=NULL);        //�������������Ϸ�ҵ�����
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
	void                           OnTimeAddTableMatchBotPlayer();

	//��������
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
