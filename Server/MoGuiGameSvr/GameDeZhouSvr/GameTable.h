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
	bool                           m_bCountChange;                                  //房间玩家数是否有变化
	bool                           m_TableShow;                                     //房间是否显示在RULESHOW不为1的情况下有效
	bool                           m_IsHaveCommonPlayer;                            //是否有普通玩家

	bool                           m_bPrivateFound;                                 //是否是自建桌
	bool                           m_bInUsed;                                       //当前桌子是否被使用
	BYTE                           m_RuleShow;                                      //桌子默认情况下是否显示
	bool                           m_bTableGameError;                               //桌子或者游戏局是否出错，每局重置一次
private:
	CGameRoom*                     m_pRoom;
	CServer*                       m_pServer;
	const CGameServerConfig*       m_pGSConfig;
	INT16						   m_GameID;
	INT16						   m_ServerID;
	UINT16						   m_RoomID;
	UINT16						   m_TableID;
	UINT32                         m_TableIdx;                                      //独一无二的桌子序列，唯一识别码

	std::string                    m_TableName;
	std::string                    m_Password;                                      //桌子密码
	std::string                    m_TableRule;
	std::string                    m_TableRuleEX;
	std::string                    m_TableMatchRule;                                //桌子的比赛规则
	
	UINT32                         m_FoundByWho;                                    //桌子的创建者
	
	BYTE 						   m_MaxSitPlayerNumber;                      		//本桌最多坐的人数，主要是方便配成5，7，9人桌
	UINT16                         m_MaxInPlayerNumber;                             //本桌能进入的的最多人数包括旁观和玩游戏的

	BYTE                           m_EnterLevel;                                    //加入房间的级别
	INT64                          m_EnterMoney;                                    //加入房间

	int                            m_TableState;                              		//桌子状态
	int                            m_TableMatchState;                               //单桌比赛的状态

	UINT32			               m_TableCurTime;                                  //当前时间
	UINT32                         m_TableEndTime;                                  //当前桌结束时间 针对自建房间而言
	UINT32                         m_TimeToClearTable;                              //比赛结束后清理桌子的时间
	UINT32                         m_TenMinuteTime;                                 //十分钟响应时间

	MapSitPlayer                   m_SitPlayers;                                    //所有坐着的玩家，和正在游戏的玩家不定同，不用数组主要是弹性考虑
	MapPlayer                      m_TablePlayers;                                  //所有玩家的MAP
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

	///////////////自建游戏桌相关函数/////////////////////////////////
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

	//不可在循环中调用
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