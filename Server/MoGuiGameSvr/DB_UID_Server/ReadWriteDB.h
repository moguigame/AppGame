#pragma once

#define OTL_ODBC_MYSQL
#define OTL_ODBC         // Compile OTL 4.0/ODBC
#define OTL_STL          // Turn on STL features
#define OTL_ANSI_CPP     // Turn on ANSI C++ typecasts
#define OTL_BIGINT long long
#include <otlv4.h>       // include the OTL 4.0 header file

#include "RWDBMsg.h"
#include "dezhoudata.h"
#include "Dezhoulib.h"
#include "DezhouData.h"
#include "publicdata.h"

using namespace MoGui::Game::DeZhou::DBS;
using namespace MoGui::Game::DeZhou;

class CServer;
class CDBSConfig;
class CRWDBMsgManage;
class otl_datetime;

class CDBOperator
{
public:
	CDBOperator(CServer* pServer);
	virtual ~CDBOperator(void);

	int OnActiveDBConnect();

	int OnRWDBMsg(ReadWriteDBMessage* pMsg);
	int OnWinLossMsg(ReadWriteDBMessage* pMsg);
	int OnMatchResult(ReadWriteDBMessage* pMsg);
	int OnMatchWinLoss(ReadWriteDBMessage* pMsg);
	int OnUpdateUserGameMoneyType(ReadWriteDBMessage* pMsg);
	int OnMaxPai(ReadWriteDBMessage* pMsg);
	int OnMaxWin(ReadWriteDBMessage* pMsg);
	int OnMaxMoney(ReadWriteDBMessage* pMsg);
	int OnAddGameMoney(ReadWriteDBMessage* pMsg);
	int OnWriteUserGift(ReadWriteDBMessage* pMsg);
	int OnAddLimite(ReadWriteDBMessage* pMsg);
	int OnGameRight(ReadWriteDBMessage* pMsg);
	int OnUpdateHuiYuan(ReadWriteDBMessage* pMsg);
	int OnAddPlayerAward(ReadWriteDBMessage* pMsg);
	int OnMatchInfo(ReadWriteDBMessage* pMsg);
	int OnWinLossInfo(ReadWriteDBMessage* pMsg);
	int OnMakeFriend(ReadWriteDBMessage* pMsg);
	int OnUpdateGiftInfo(ReadWriteDBMessage* pMsg);
	int OnDeleteCreateInfo(ReadWriteDBMessage* pMsg);

	int OnIncomAndPay(ReadWriteDBMessage* pMsg);
	int OnChangeBankLog(ReadWriteDBMessage* pMsg);
	int OnGameMoneyError(ReadWriteDBMessage* pMsg);

	int OnMoGuiMoneyLog(ReadWriteDBMessage* pMsg);
	int OnMoGuiMoneyError(ReadWriteDBMessage* pMsg);

	int OnUpdateAwardInfo(ReadWriteDBMessage* pMsg);
	int OnUpdateRightInfo(ReadWriteDBMessage* pMsg);
	int OnWriteRightInfo(ReadWriteDBMessage* pMsg);
	int OnUpdateAchieve(ReadWriteDBMessage* pMsg);
	int OnTableInfo(ReadWriteDBMessage* pMsg);

	int OnPlayerLogin(ReadWriteDBMessage* pMsg);
	int OnCreateGameInfo(ReadWriteDBMessage* pMsg);
	int OnChangeUserInfo(ReadWriteDBMessage* pMsg);
	int OnReportPlayerOnline(ReadWriteDBMessage* pMsg);
	int OnPlayerActionLog(ReadWriteDBMessage* pMsg);
	int OnPlayerClientErr(ReadWriteDBMessage* pMsg);
	int OnChatLog(ReadWriteDBMessage* pMsg);
	int OnUpdateWinType(ReadWriteDBMessage* pMsg);
	int OnAddPlayerInfo(ReadWriteDBMessage* pMsg);
	int OnHonorLog(ReadWriteDBMessage* pMsg);

	int WriteTableInfo(stTableInfo& stTI);
	
	int AddUserMoGuiMoney(INT16 AID,UINT32 PID,int nAddMoney);
	int DBAddUserGameMoney(INT16 AID,UINT32 PID,INT64 nAddMoney);
	int UpdateWinLossMoney(const stWinLossMoney& stWLM);

	int UpdateChangeBankMoney(INT16 AID,UINT32 PID,INT64 nGameMoney,INT64 nBankMoney);
	int CreatePlayerGameInfo(DezhouLib::stCreatePlayerGameInfo& stCPGI);
	int UpdateUserProduct(int Idx,INT16 UseFlag);

	INT16 ReadUserAID(UINT32 PID);
	int ReadUserGameInfo(INT16 AID,UINT32 PID,stUserGameInfo& stUGI);
	int ReadUserDataInfo(INT16 AID,UINT32 PID,stUserDataInfo& stUDI);
	int ReadBotPlayerData(INT16 AID,UINT32 StartPID,UINT32 EndPID,VectorBotPlayerData& listPD);
	int ReadFaceInfo(VectorFaceInfo& vectorFI);
	int ReadGiftInfo(VectorGiftInfo& vectorGI);
	int ReadProductInfo(VectorProductInfo& vectorPI);
	int ReadMatchInfo(INT16 ServerID,VectorDBMatchInfo& vectorMI);
	int ReadHonorInfo(VectorHonorInfo& vectorHI);
	int ReadGameLevelInfo(VectorPTLevelInfo& vectorGLI);
    int ReadAreaInfo(VectorAreaInfo& vectorArea);
	
	int ReadUserMoguiMoney(INT16 AID,UINT32 PID,stDBMoguiInfo& stMI);
	int ReadUserGameMoney(INT16 AID,UINT32 PID,stDBGameMoney& stGM);
	int ReadUserProduct(INT16 AID,UINT32 PID,VectorUserProduct& vectorUP);
	int ReadAllUserProduct(int MaxIdx,VectorUserProduct& vectorUP);
	int ReadFriendInfo(INT16 AID,UINT32 PID,VectorFriendInfo& vectorFI);
	int ReadRoomInfo(INT16 ServerID,VectorRoomInfo& vectorRI);
	int ReadTableInfo(INT16 ServerID,VectorTableInfo& vectorTI);
	int ReadUserGiftInfo(INT16 AID,UINT32 PID,VectorDBUserGiftInfo& vectorUGI);
	int ReadUserAward(INT16 AID,UINT32 PID,VectorAward& vectorAward);
	int ReadUserRight(INT16 AID,UINT32 PID,VectorRight& vectorRight);

	int RWDB_Test(INT16 AID,UINT32 PID,std::string strCity);
	inline void DebugError(const char* logstr,...);
	inline void DebugInfo(const char* logstr,...);

private:
	void               CheckDBConnect(otl_connect& DBConnect,std::string& strDB);
	void               ActiveDBConnect(otl_connect& DBConnect,std::string& strDB);
	void               DoConnect(otl_connect& DBConnect,std::string& strDB);
	void               CheckOTLException(const otl_exception &p,otl_connect& DBConnect,std::string& strDB);
	const std::string& getGiftTableName(UINT32 PID);
	const std::string& getFriendTableName(UINT32 PID);
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

	static std::vector<std::string>     s_vectorFriendTableName;
	static std::vector<std::string>     s_vectorGiftTableName;

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

	MapFaceInfo                          m_mapFaceInfo;
	MapGiftInfo                          m_mapGiftInfo;

	CMapFunctionTime                     m_FuncTime;

	//数据表的名称
	std::string                          m_strTableNameWinLoss;
	std::string                          m_strTableNameMoneyLog;
	std::string                          m_strTableNameMatchResult;
	std::string                          m_strTableNameMatchWinLoss;

	std::string                          m_strTableNameChat;
	std::string                          m_strTableNameLogin;
	std::string                          m_strTableNameAction;
};
