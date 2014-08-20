#pragma once

#define OTL_ODBC_MYSQL
#define OTL_ODBC         // Compile OTL 4.0/ODBC
#define OTL_STL          // Turn on STL features
#define OTL_ANSI_CPP     // Turn on ANSI C++ typecasts
#define OTL_BIGINT long long
#include <otlv4.h>       // include the OTL 4.0 header file

#include "RWDBMsg.h"
#include "gamedata.h"
#include "Dezhoulib.h"
#include "GameData.h"
#include "publicdata.h"

using namespace MoGui::Game::DeZhou::DBS;
using namespace MoGui::Game::DeZhou;

class CServer;
class CDBSConfig;
class CRWDBMsgManage;
class otl_datetime;

class CDBWriteOperator
{
public:
	CDBWriteOperator(CServer* pServer);
	~CDBWriteOperator(void);

	int OnActiveDBConnect();

	int OnRWDBMsg(ReadWriteDBMessage* pMsg);
	int OnWinLossMsg(ReadWriteDBMessage* pMsg);
	int OnMatchResult(ReadWriteDBMessage* pMsg);
	int OnMatchWinLoss(ReadWriteDBMessage* pMsg);

	int OnMaxPai(ReadWriteDBMessage* pMsg);
	int OnMaxWin(ReadWriteDBMessage* pMsg);
	int OnMaxMoney(ReadWriteDBMessage* pMsg);
	int OnAddGameMoney(ReadWriteDBMessage* pMsg);
	
	int OnGameRight(ReadWriteDBMessage* pMsg);
	int OnUpdateHuiYuan(ReadWriteDBMessage* pMsg);
	int OnAddPlayerAward(ReadWriteDBMessage* pMsg);
	
	int OnWinLossInfo(ReadWriteDBMessage* pMsg);
	
	int OnDeleteCreateInfo(ReadWriteDBMessage* pMsg);

	int OnIncomAndPay(ReadWriteDBMessage* pMsg);
	
	int OnGameMoneyError(ReadWriteDBMessage* pMsg);


	int OnUpdateAwardInfo(ReadWriteDBMessage* pMsg);
	
	int OnTableInfo(ReadWriteDBMessage* pMsg);

	int OnPlayerLogin(ReadWriteDBMessage* pMsg);
	int OnCreateGameInfo(ReadWriteDBMessage* pMsg);
	int OnChangeUserInfo(ReadWriteDBMessage* pMsg);
	int OnReportPlayerOnline(ReadWriteDBMessage* pMsg);
	int OnPlayerActionLog(ReadWriteDBMessage* pMsg);
	int OnPlayerClientErr(ReadWriteDBMessage* pMsg);
	int OnChatLog(ReadWriteDBMessage* pMsg);
	
	int OnAddPlayerInfo(ReadWriteDBMessage* pMsg);
	

	int ReadUserGameMoney(INT16 AID,UINT32 PID,stDBGameMoney& stGM);
	int UpdateChangeBankMoney(INT16 AID,UINT32 PID,INT64 nGameMoney,INT64 nBankMoney);

	inline void DebugError(const char* logstr,...);
	inline void DebugInfo(const char* logstr,...);

private:
	void               CheckDBConnect(otl_connect& DBConnect,std::string& strDB);
	void               ActiveDBConnect(otl_connect& DBConnect,std::string& strDB);
	void               DoConnect(otl_connect& DBConnect,std::string& strDB);
	void               CheckOTLException(const otl_exception &p,otl_connect& DBConnect,std::string& strDB);
	void               InitTableName(UINT32 curTime);

	inline void        CatchDBException(const otl_exception &p);
	inline void        print_otl_error(const otl_exception &p);

public:
	static int                s_MaxID;
	static volatile UINT32    s_WriteOperator;
	static volatile UINT32    s_ReadOperator;

	static UINT32             s_LastWriteOperator;
	static UINT32             s_LastReadOperator;
	static UINT32             s_LastTime;

	static void StaticInit();

private:
	CServer*                             m_pServer;
	CDBSConfig*                          m_pConfig;

	int                                  m_ID;
	UINT32                               m_CurTime;
	UINT32                               m_NewDayTime;
	UINT32                               m_LastCheckTime;

	std::string                          m_strDBConfig;
	otl_connect     					 m_DBConfigConnect;

	std::string                          m_strDBUser;
	otl_connect     					 m_DBUserConnect;

	std::string                          m_strDBGame;
	otl_connect     					 m_DBGameConnect;

	std::string                          m_strDBLog;
	otl_connect     					 m_DBLogConnect;

	std::string                          m_strDBFriend;
	otl_connect     					 m_DBFriendConnect;

	std::string                          m_strDBProp;
	otl_connect     					 m_DBPropConnect;

	CMapFunctionTime                     m_FuncTime;

	//数据表的名称
	std::string                          m_TNMoney;
	std::string                          m_TNGameInfo;
	std::string                          m_TNWinLoss;
	std::string                          m_TNMoneyLog;
	std::string                          m_TNMatchResult;
	std::string                          m_TNMatchWinLoss;

	std::string                          m_TNChat;
	std::string                          m_TNLogin;
	std::string                          m_TNAction;
	std::string                          m_TNOnline;
};