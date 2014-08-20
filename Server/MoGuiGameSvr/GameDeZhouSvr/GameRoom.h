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
	string                         m_Password;               //��������
	string                         m_RoomRule;
	string                         m_RoomRuleEX;
	string                         m_RoomMatchRule;
	string                         m_AreaRule;

	UINT16						   m_MaxPlayerInRoom;
	UINT16		                   m_CurPlayerInRoom;
	UINT16                         m_RealPlayerInRoom;
	UINT16                         m_BotPlayerInRoom;
	UINT16                         m_MaxTableInRoom;         //����������������Ϊ0���ʾ����������չ

	long long                      m_nMinPlayMoney;
	long long                      m_nMaxPlayMoney;

	vector<INT16>				   m_vecForBidAreaSee;       //��ֹ���ʺ�������ʵ�������������ֻ��һ����Ч
	vector<INT16>				   m_vecAllowAreaSee;
	vector<INT16>                  m_vecForBidAreaPlay;      //������Щ�������ҿ�����Ϸ
	vector<INT16>                  m_vecAllowAreaPlay;

	BYTE                           m_RoomPlace;              //ָ������ڳ����м��߼���������
	BYTE                           m_RoomType;               //�Ŷӣ����ӣ���������ȫ���������Խ�����
	BYTE                           m_MatchRoomState;         //���������״̬

	MapTable                       m_Tables;
	MapPlayer                      m_RoomPlayers;
	MapPlayer                      m_ShowPlayers;            //��Ҫ��������ʾ����ҵĵ�MAP
	MapListTable                   m_maplistTables;          //��������ͬһ�����ڲ�ͬ��ʽ������(äע����)

	MapFuncTime                    m_mapFuncTime;

	//ʱ����صı���
	UINT32                         m_CurRoomTime;
	int                            m_IntervalSendTablePC;    //���ͷ��������ļ��
	UINT32                         m_timeSendTablePC;        //���ͷ���������ʱ��

	int                            m_IntervalCheckShow;      //��鷿�����ӵļ��
	UINT32                         m_timeCheckShow;          //��鷿��������ʾ��ʱ��

	int                            m_IntervalRoomPS;
	UINT32                         m_timeSendRoomPS;

	int                            m_InervalRoomPC;
	UINT32                         m_timeSendRoomPC;         //��������

	//������ص�����
	CMatchRule*                    m_pRoomMatchRule;
	SetPID                         m_setSignMatchPID;        //ָ�μӱ�����PID
	UINT32                         m_RankPlayerTime;
	VectorMatchPlayerRank          m_vecMatchPlayerInfo;     //���������ҵ�����

	BYTE                           m_WinLossRoom;            //�Ƿ��Ǳ�����Ӯ��
	VectorWinLossPlayerInfo        m_vecWinLossPlayerInfo;   //ָ������Ӯ�ı�������

	//�Խ�����
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

	//�����Ǳ�����ز���
	void                           InitMatchTable();
	bool                           IsMatching();
	void                           StartRoomMatch();
	void                           EndRoomMatch();
	bool                           CheckChampTable();                         //�Ƿ�����ռ���
	int                            PlayerJoinMatch(PlayerPtr pPlayer);
	void                           AssignMatchPlayer();                       //�����������ҵ�λ��
	bool                           SignUpPlayer(UINT32 PID);
	bool                           SignDownPlayer(UINT32 PID);
	bool                           IsPlayerSignedMatch(UINT32 PID);
	int                            GetMatchID()const;
	bool                           GetMatchRoolState(UINT32 PID,MatchRoomState& msgMRS);
	void                           FirstAwoke();
	void                           SecondAwoke();
	CGameTable*                    CreateRoomMatchTable();
	void                           UpdateMatchRoomState();
	void                           AddPlayerMatchTableMoney(UINT32 PID,int TableMoney);     //ͳ����ұ�����Ӯ����Ϸ��

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

	//ͬ�����ǩ�б�
	void                           AddShowPlayer(PlayerPtr pPlayer);
	void                           RemoveShowPlayer(PlayerPtr pPlayer);
	void                           AddPlayerToClient(PlayerPtr pPlayer);
	void                           DeletePlayerToClient(PlayerPtr pPlayer);
	int                            SendRoomPlayerToPlayer(PlayerPtr pPlayer);                //���ͷ���������Ϣ�����
	int                            OnTimeSendRoomPlayerState();                              //���ͷ����������״̬�����
	int                            SendRoomPlayerState(PlayerPtr pPlayer);

	int                            OnRoomMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int                            OnReqJoinTable(PlayerPtr pPlayer,GameXY::ReqJoinTable& rjt);
	int                            OnReqRoomPlayerBase(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int                            OnReqMatchRank(PlayerPtr pPlayer,GameXY::ReqMatchRank& msgMR);
	int                            OnReqTablePlayerBase(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	int                            OnReqRoomPlayerState(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);

	CGameTable*                    CreateTable(CreateTableInfo& cti);
	int                            SendTableListInfoToPlayer(PlayerPtr pPlayer);             //���ͷ����������Ϣ�����

	int                            OnTimeSendTablePlayerCount();                             //���ͷ�����ÿ�����ӵ�����������
	int                            SendTablePlayerCount(PlayerPtr pPlayer);

	int                            OnTimeSendRoomPlayerCount();                              //���ڹ㲥����������

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