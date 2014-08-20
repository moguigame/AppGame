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

	int 						   m_MaxPlayerInRoom;
	int                            m_MaxTableInRoom;         //����������������Ϊ0���ʾ����������չ
	 
	int		                       m_CurPlayerInRoom;
	int                            m_RealPlayerInRoom;
	int                            m_BotPlayerInRoom;

	INT64                          m_RoomMinTake;
	INT64                          m_RoomMaxTake;

	vector<INT16>				   m_vecForBidAreaSee;       //��ֹ���ʺ�������ʵ�������������ֻ��һ����Ч
	vector<INT16>				   m_vecAllowAreaSee;
	vector<INT16>                  m_vecForBidAreaPlay;      //������Щ�������ҿ�����Ϸ
	vector<INT16>                  m_vecAllowAreaPlay;

	int                            m_RoomPlace;              //ָ������ڳ����м��߼���������
	int                            m_RoomType;               //�Ŷӣ����ӣ���������ȫ������

	MapTable                       m_Tables;
	MapPlayer                      m_RoomPlayers;
	
	MapListTable                   m_maplistTables;          //��������ͬһ�����ڲ�ͬ��ʽ������(äע����)

	MapFuncTime                    m_mapFuncTime;

	//ʱ����صı���
	UINT32                         m_CurRoomTime;

public:
	CServer*                       GetServer()const { return m_pServer; }
	UINT16						   GetGameID()const;
	UINT16						   GetServerID()const;
	UINT16						   GetRoomID()const;
	CGameTable*                    GetTableByID(UINT16 tableid);
	CGameTable*                    CreateTable(CreateTableInfo& cti);
	UINT16                         GetNoUseID();
	int                            GetRoomPlayerCount()const{ return m_CurPlayerInRoom; }
	int                            GetRealPlayerCount()const{ return m_RealPlayerInRoom; }
	int                            GetBotPlayerCount()const{ return m_BotPlayerInRoom; }
	int                            GetTableCount()const{ return int(m_Tables.size()); }
	int                            GetMaxTableCount()const { return m_MaxTableInRoom; }
	const BYTE                     GetRoomType()const{ return m_RoomType;}
	
	const string&                  GetRoomRule(){ return m_RoomRule; }
	const string&                  GetRoomRuleEx(){ return m_RoomRuleEX; }
	string                         GetClientRoomRule();
	const string&                  GetRoomName()const { return m_RoomName; }

	bool                           IsRoomPlayerFull() const { return m_CurPlayerInRoom >= m_MaxPlayerInRoom; }
	bool                           IsRoomTableFull()const{ return int(m_Tables.size())>=m_MaxTableInRoom;}

	bool                           IsNeedPassword() const { return m_Password.length() > 0; }
	bool                           CheckPassword(const string& pwd){ return m_Password == pwd; }

	bool                           PlayerCanSee(PlayerPtr pPlayer);
	bool                           PlayerCanPlay(PlayerPtr pPlayer);

	bool                           GetRoomInfo(RoomInfo& ri);
	bool                           GetRoomInfo(PlayerPtr pPlayer,RoomInfo& ri);

	void                           DebugError(const char* str,...);
	void                           DebugInfo(const char* str,...);

	bool                           OnTimer(UINT32 curTime);

	int                            OnRoomMsg(PlayerPtr pPlayer,CRecvMsgPacket& msgPack);
	int                            OnReqJoinTable(PlayerPtr pPlayer,GameXY::ReqJoinTable& rjt);
	
	int                            PlayerCanJoinRoom(PlayerPtr pPlayer);
	bool                           PlayerCanLeaveRoom(PlayerPtr pPlayer);
	bool                           PlayerJoinRoom(PlayerPtr pPlayer);
	bool                           PlayerLeaveRoom(PlayerPtr pPlayer);
	bool                           AttachPlayerRoom(PlayerPtr pPlayer);
	bool                           PlayerChangeTable(PlayerPtr pPlayer);

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