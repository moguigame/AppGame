#pragma once

#include "gamedezhou.h"
#include "publicdata.h"

#include "player.h"

using namespace AGBase;
using namespace std;
using namespace MoGui::Game::DeZhou::GS;

class CPlayer;
class CGameTable;
class CServer;
class CMatchRule;
class CGameServerConfig;

class CGameRoom  : public CMemoryPool_Public<CGameRoom, 1>, public boost::noncopyable
{
public:
	CGameRoom( CServer* pServer,CreateRoomInfo& cri );
	virtual ~CGameRoom(void);

	typedef RefPtr<CPlayer>	            PlayerPtr;
	typedef map< UINT16,CGameTable*>    MapTable;
	typedef map<UINT32,PlayerPtr>       MapPlayer;
	typedef list<CGameTable*>           ListTable;
	typedef map<int,list<CGameTable*> > MapListTable;
	typedef map<string,stFuncTimeLog>   MapFuncTime;

	static CMapFunctionTime             s_RoomFuncTime;

private:
	CServer*                       m_pServer;
	const CGameServerConfig*       m_pGSConfig;
	UINT16						   m_RoomID;
	string                         m_RoomName;
	string                         m_Password;               //房间密码
	string                         m_RoomRule;
	string                         m_RoomRuleEX;
	string                         m_RoomMatchRule;
	string                         m_AreaRule;

	UINT16						   m_MaxPlayerInRoom;
	UINT16		                   m_CurPlayerInRoom;
	UINT16                         m_RealPlayerInRoom;
	UINT16                         m_BotPlayerInRoom;
	UINT16                         m_MaxTableInRoom;         //最大桌子数量，如果为0则表示可以自由扩展

	long long                      m_nMinPlayMoney;
	long long                      m_nMaxPlayMoney;

	vector<INT16>				   m_vecForBidAreaSee;       //禁止访问和允许访问的区域，两个数组只有一个有效
	vector<INT16>				   m_vecAllowAreaSee;
	vector<INT16>                  m_vecForBidAreaPlay;      //充许哪些区域的玩家可以游戏
	vector<INT16>                  m_vecAllowAreaPlay;

	BYTE                           m_RoomPlace;              //指房间放在初级中级高级比赛场等
	BYTE                           m_RoomType;               //排队，随便从，单桌赛，全房间赛，自建房间
	BYTE                           m_MatchRoomState;         //比赛房间的状态

	MapTable                       m_Tables;
	MapPlayer                      m_RoomPlayers;
	MapPlayer                      m_ShowPlayers;            //主要是用于显示的玩家的的MAP
	MapListTable                   m_maplistTables;          //用来区别同一房间内不同形式的桌子(盲注房号)

	MapFuncTime                    m_mapFuncTime;

	//时间相关的变量
	UINT32                         m_CurRoomTime;
	int                            m_IntervalSendTablePC;    //发送房间人数的间隔
	UINT32                         m_timeSendTablePC;        //发送房间人数的时间

	int                            m_IntervalCheckShow;      //检查房间桌子的间隔
	UINT32                         m_timeCheckShow;          //检查房间桌子显示的时间

	int                            m_IntervalRoomPS;
	UINT32                         m_timeSendRoomPS;

	int                            m_InervalRoomPC;
	UINT32                         m_timeSendRoomPC;         //房间人数

	//比赛相关的内容
	CMatchRule*                    m_pRoomMatchRule;
	SetPID                         m_setSignMatchPID;        //指参加报名的PID
	UINT32                         m_RankPlayerTime;
	VectorMatchPlayerRank          m_vecMatchPlayerInfo;     //房间比赛玩家的排名

	BYTE                           m_WinLossRoom;            //是否是比赛输赢桌
	VectorWinLossPlayerInfo        m_vecWinLossPlayerInfo;   //指房间输赢的比赛排名

	//自建房间
	BYTE                           m_PrivateRoom;

public:
	CServer*                       GetServer()const { return m_pServer; }
	UINT16						   GetGameID()const;
	UINT16						   GetServerID()const;
	UINT16						   GetRoomID()const;
	CGameTable*                    GetTableByID(UINT16 tableid);
	CGameTable*                    GetNoUseTable();
	UINT16                         GetNoUseID();
	UINT16                         GetRoomPlayerCount()const{ return m_CurPlayerInRoom;}
	UINT16                         GetRealPlayerCount()const{ return m_RealPlayerInRoom; }
	UINT16                         GetBotPlayerCount()const{ return m_BotPlayerInRoom; }
	int                            GetTableCount()const{ return int(m_Tables.size());}
	const BYTE                     GetRoomType()const{ return m_RoomType;}
	const BYTE                     GetRoomMatchState()const{ return m_MatchRoomState; }
	string                         GetRoomRule(){ return m_RoomRule; }
	string                         GetRoomRuleEx(){ return m_RoomRuleEX; }
	CMatchRule*                    GetRoomMatchRule()const{ assert(m_pRoomMatchRule); return m_pRoomMatchRule; }
	string                         GetClientRoomRule();
	bool                           IsPlayerInRoom(PlayerPtr pPlayer);
	void                           SetShowHideTable();
	void                           SetTableMatchTableState();
	int                            GetCreateTableCount(UINT32 PID);

	bool                           IsCommonRoom()const{ return m_RoomType==ROOM_TYPE_Common;}
	bool                           IsRoomMatch()const{ return m_RoomType==ROOM_TYPE_RoomMatch;}
	bool                           IsTableMatch()const{ return m_RoomType==ROOM_TYPE_TableMatch; }
	bool                           IsMatchRoom()const { return IsRoomMatch() || IsTableMatch(); }
	bool                           IsPrivateRoom()const{ return m_PrivateRoom != 0; }
	bool                           IsWinLossRoom()const{ return m_WinLossRoom != 0; }

	//以下是比赛相关部分
	void                           InitMatchTable();
	bool                           IsMatching();
	void                           StartRoomMatch();
	void                           EndRoomMatch();
	bool                           CheckChampTable();                         //是否比赛终极桌
	int                            PlayerJoinMatch(PlayerPtr pPlayer);
	void                           AssignMatchPlayer();                       //分配比赛中玩家的位置
	bool                           SignUpPlayer(UINT32 PID);
	bool                           SignDownPlayer(UINT32 PID);
	bool                           IsPlayerSignedMatch(UINT32 PID);
	int                            GetMatchID()const;
	bool                           GetMatchRoolState(UINT32 PID,MatchRoomState& msgMRS);
	void                           FirstAwoke();
	void                           SecondAwoke();
	CGameTable*                    CreateRoomMatchTable();
	void                           UpdateMatchRoomState();
	void                           AddPlayerMatchTableMoney(UINT32 PID,int TableMoney);     //统计玩家比赛输赢的游戏币

	bool                           IsRoomPlayerFull() const { return m_CurPlayerInRoom >= m_MaxPlayerInRoom; }
	bool                           IsRoomTableFull()const{ return m_Tables.size()>=m_MaxTableInRoom;}
	bool                           IsNeedPassword() const { return m_Password.length() > 0; }
	bool                           CheckPassword(const string& pwd){ return m_Password == pwd; }
	bool                           PlayerCanSee(PlayerPtr pPlayer);
	bool                           PlayerCanPlay(PlayerPtr pPlayer);

	bool                           GetRoomInfo(RoomInfo& ri);
	bool                           GetRoomInfo(PlayerPtr pPlayer,RoomInfo& ri);

	void                           DebugError(const char* str,...);
	void                           DebugInfo(const char* str,...);
	void                           CeShiInfo(const char* str,...);

	void                           Clear();
	bool                           OnTimer(UINT32 curTime);
	UINT32                         GetTime() const;

	//同房间标签列表
	void                           AddShowPlayer(PlayerPtr pPlayer);
	void                           RemoveShowPlayer(PlayerPtr pPlayer);
	void                           AddPlayerToClient(PlayerPtr pPlayer);
	void                           DeletePlayerToClient(PlayerPtr pPlayer);
	int                            SendRoomPlayerToPlayer(PlayerPtr pPlayer);                //发送房间的玩家信息给玩家
	int                            OnTimeSendRoomPlayerState();                              //发送房间表里的玩家状态给玩家
	int                            SendRoomPlayerState(PlayerPtr pPlayer);

	int                            OnRoomMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int                            OnReqJoinTable(PlayerPtr pPlayer,GameXY::ReqJoinTable& rjt);
	int                            OnReqRoomPlayerBase(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int                            OnReqMatchRank(PlayerPtr pPlayer,GameXY::ReqMatchRank& msgMR);
	int                            OnReqTablePlayerBase(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int                            OnReqRoomPlayerState(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	CGameTable*                    CreateTable(CreateTableInfo& cti);
	int                            SendTableListInfoToPlayer(PlayerPtr pPlayer);             //发送房间的桌子信息给玩家

	int                            OnTimeSendTablePlayerCount();                             //发送房间内每张桌子的玩家数给玩家
	int                            SendTablePlayerCount(PlayerPtr pPlayer);

	int                            OnTimeSendRoomPlayerCount();                              //用于广播比赛人数的

	bool                           PlayerJoinRoom(PlayerPtr pPlayer);
	bool                           PlayerLeaveRoom(PlayerPtr pPlayer);
	bool                           AttachPlayerRoom(PlayerPtr pPlayer);

	template<class Txieyi>
	void SendMsgToAllPlayerInRoomInTable(Txieyi& xieyi)
	{
		PlayerPtr pPlayer;
		for ( MapPlayer::iterator itorPlayer=m_RoomPlayers.begin();itorPlayer!=m_RoomPlayers.end();itorPlayer++)
		{
			pPlayer = itorPlayer->second;
			if ( pPlayer && pPlayer->IsNeedSendMsg() && pPlayer->IsInTable() )
			{
				itorPlayer->second->SendMsg(xieyi);
			}
		}
	}

	template<class Txieyi>
	void SendMsgToAllPlayerInRoom(Txieyi& xieyi)
	{
		PlayerPtr pPlayer;
		for ( MapPlayer::iterator itorPlayer=m_RoomPlayers.begin();itorPlayer!=m_RoomPlayers.end();itorPlayer++)
		{
			pPlayer = itorPlayer->second;
			if ( pPlayer && pPlayer->IsNeedSendMsg() )
			{
				itorPlayer->second->SendMsg(xieyi);
			}
		}
	}
};