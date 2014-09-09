#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"
#include "publicdata.h"

#include "player.h"
#include "Server.h"

using namespace GS;

class CPlayer;
class CGameRoom;
class CServer;
class CDeZhouGameLogic;
class CGameServerConfig;

class CGameTable : public AGBase::CMemoryPool_Public<CGameTable, 1>, public boost::noncopyable
{
public:
	CGameTable(CGameRoom* pRoom,CreateTableInfo& cti);
	virtual ~CGameTable(void);

	typedef AGBase::RefPtr<CPlayer>	                PlayerPtr;
	typedef std::map<UINT32,PlayerPtr>              MapPlayer;
	typedef std::map<BYTE, PlayerPtr>               MapSitPlayer;
	typedef std::map<std::string, stFuncTimeLog>    MapFuncTime;

	static CMapFunctionTime                    s_TableFuncTime;

public:
	bool                           m_bCountChange;                                  //����������Ƿ��б仯
	bool                           m_TableShow;                                     //�����Ƿ���ʾ��RULESHOW��Ϊ1���������Ч
	bool                           m_IsHaveCommonPlayer;                            //�Ƿ�����ͨ���

	bool                           m_bPrivateFound;                                 //�Ƿ����Խ���
	bool                           m_bInUsed;                                       //��ǰ�����Ƿ�ʹ��
	BYTE                           m_RuleShow;                                      //����Ĭ��������Ƿ���ʾ
	bool                           m_bTableGameError;                               //���ӻ�����Ϸ���Ƿ����ÿ������һ��
private:
	CGameRoom*                     m_pRoom;
	CServer*                       m_pServer;
	const CGameServerConfig*       m_pGSConfig;
	INT16						   m_GameID;
	INT16						   m_ServerID;
	UINT16						   m_RoomID;
	UINT16						   m_TableID;
	UINT32                         m_TableIdx;                                      //��һ�޶����������У�Ψһʶ����

	std::string                    m_TableName;
	std::string                    m_Password;                                      //��������
	std::string                    m_TableRule;
	std::string                    m_TableRuleEX;
	std::string                    m_TableMatchRule;                                //���ӵı�������
	
	UINT32                         m_FoundByWho;                                    //���ӵĴ�����
	
	BYTE 						   m_MaxSitPlayerNumber;                      		//�������������������Ҫ�Ƿ������5��7��9����
	UINT16                         m_MaxInPlayerNumber;                             //�����ܽ���ĵ�������������Թۺ�����Ϸ��

	BYTE                           m_EnterLevel;                                    //���뷿��ļ���
	INT64                          m_EnterMoney;                                    //���뷿��

	int                            m_TableState;                              		//����״̬
	int                            m_TableMatchState;                               //����������״̬

	UINT32			               m_TableCurTime;                                  //��ǰʱ��
	UINT32                         m_TableEndTime;                                  //��ǰ������ʱ�� ����Խ��������
	UINT32                         m_TimeToClearTable;                              //�����������������ӵ�ʱ��
	UINT32                         m_TenMinuteTime;                                 //ʮ������Ӧʱ��

	MapSitPlayer                   m_SitPlayers;                                    //�������ŵ���ң���������Ϸ����Ҳ���ͬ������������Ҫ�ǵ��Կ���
	MapPlayer                      m_TablePlayers;                                  //������ҵ�MAP
	CDeZhouGameLogic*              m_pGameLogic;

public:
	CServer*                       GetServer()const { return m_pServer; }
	CGameRoom*                     GetRoom()const { return m_pRoom; }
	UINT16				           GetTableID()const { return m_TableID; }
	UINT16                         GetRoomID()const{ return m_RoomID;}
	int                            GetTableMatchState()const{ return m_TableMatchState; }
	void                           SetTableMatchState(int State){ m_TableMatchState = State; }
	void                           SetClearTableTime(UINT32 nTime){ m_TimeToClearTable = nTime; }
	const std::string&             GetTableName(){ return m_TableName; }
	const std::string&             GetTableRule() const { return m_TableRule; }
	const std::string&             GetTableRuleEX()const { return m_TableRuleEX; }
	const std::string&             GetTableMatchRule()const { return m_TableMatchRule; }
	const std::string&             GetTablePassword() const { return m_Password; }
	UINT32                         GetFoundPID()const{ return m_FoundByWho; }

	int                            InitTable(CreateTableInfo& cti);
	int                            InitTableRule();

	bool                           IsTableLogicStop()const;
	void                           StartTableLogic();
	bool                           IsMatchTable()const;
	bool                           IsTableMatching()const;
	bool                           IsTablePlaying()const { return m_TableState == TABLE_ST_PLAYING; }
	void                           SetTableState(int State){ m_TableState = State; }
	bool                           IsTableShow()const{ return m_TableShow || m_RuleShow; }
	bool                           IsTableRuleShow()const{ return m_RuleShow == 1; }
	bool				           IsAvailSitID(int SitID){ return SitID>0 && SitID <= m_MaxSitPlayerNumber; }
	bool                           IsSitIDEmpty(BYTE SitID) const;	

	PlayerPtr                      GetSitPlayerBySitID(BYTE SitID)const;
	bool                           AddSitPlayerBySitID(PlayerPtr pPlayer,BYTE SitID);
	bool                           DeleteSitPlayerBySitID(PlayerPtr pPlayer,BYTE SitID);
	void                           AddPlayerToTable(PlayerPtr pPlayer);
	void                           DeletePlayerFromTable(PlayerPtr pPlayer);
	int                            AddBotPlayer(INT64 nBigBlind);
	int                            RemoveBotPlayer(UINT32 PID);

	PlayerPtr                      GetTablePlayerByPID(UINT32 PID) const;
	PlayerPtr                      GetNextPlayer(PlayerPtr pPlayer);

public:
	bool                           IsTablePlayerFull();
	bool                           IsNeedPassword();
	
	BYTE 		                   GetSitPlayerNumber();
	INT16	                       GetSeePlayerNumber();
	INT16 		                   GetTotalPlayer();
	int                            GetEmptySitNumber();
	bool                           IsTableEmpty();

	///////////////�Խ���Ϸ����غ���/////////////////////////////////
	void                           AddTableTime(int nAddTime);
	UINT32                         GetTableLeftTime();
	bool                           IsTableInUsed()const{ return m_bInUsed;}
	void                           SetTableUsed(bool bUse){m_bInUsed=bUse;}
	bool                           IsTablePrivate()const{return m_bPrivateFound;}
	void                           SetTablePrivate(bool bPrivate){m_bPrivateFound=bPrivate;}

	BYTE						   GetEmptySitID(BYTE FromSitID=0) const;
	bool                           GetTableInfo(TableInfo& ti);

	void                           reportStartGame();
	void                           reportEndGame();
	void                           reportEndTable();

	bool                           OnTimer(UINT32 curTime);
	UINT32		                   GetTime() const ;
	int                            OnTableMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int                            OnReqPlayerAct(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int                            OnPlayerLeaveTable(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	void                           SendPlayerBaseInfoToPlayer(PlayerPtr pPlayer);
	bool                           AttachPlayerTable(PlayerPtr pPlayer);
	bool                           PlayerJoinTable(PlayerPtr pPlayer);
	bool                           PlayerLeaveTable(PlayerPtr pPlayer,int Flag=Action_Flag_Player);
	bool                           PlayerSitDown(PlayerPtr pPlayer,int SitID,INT64 nTakeMoney,int Flag=Action_Flag_Player);
	bool                           PlayerStandUp(PlayerPtr pPlayer,int SitID,int Flag=Action_Flag_Player);
	bool                           DoPlayerJoinTable(PlayerPtr pPlayer);

	void                           InitMatchTableData();
	void                           ClearAllPlayer();
	bool                           PutPlayerSitDown(PlayerPtr pPlayer,INT64 nTableMoney);
	int                            GetPlayerTableMoney(std::vector<PlayerPtr>&, std::vector<INT64>&);
	void                           OnChangeBlind(int BigBlind,int SmallBlind);

	void                           TestPlayerNumber();
	void                           DebugError(const char* str,...);
	void                           CeShiInfo(const char* str,...);
	void                           DebugInfo(const char* str,...);
	void                           ReleaseInfo(const char* str,...);

public:
	template<class Txieyi>
		void SendMsgToDBServer(Txieyi& xieyi)
	{
		m_pServer->SendMsgToDBServer(xieyi);
	}

	//������ѭ���е���
	template<class Txieyi>
		void SendMsgToAllPlayerInTable(Txieyi& xieyi)
	{
		MapPlayer::iterator itorPlayer;
		for ( itorPlayer=m_TablePlayers.begin();itorPlayer!=m_TablePlayers.end();itorPlayer++)
		{
			if ( itorPlayer->second )
			{
				itorPlayer->second->SendMsg(xieyi);
			}
		}
	}
};