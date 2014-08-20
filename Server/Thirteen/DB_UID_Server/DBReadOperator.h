#pragma once

#define OTL_ODBC_MYSQL
#define OTL_ODBC         // Compile OTL 4.0/ODBC
#define OTL_STL          // Turn on STL features
#define OTL_ANSI_CPP     // Turn on ANSI C++ typecasts
#define OTL_BIGINT long long
#include <otlv4.h>       // include the OTL 4.0 header file

#include "GameData.h"
#include "publicdata.h"

using namespace MoGui::Game::DeZhou::DBS;
using namespace MoGui::Game::DeZhou;

class CServer;
class CDBSConfig;
class CRWDBMsgManage;
class otl_datetime;

class CDBReadOperator
{
public:
	CDBReadOperator(CServer* pServer);
	virtual ~CDBReadOperator(void);

	int OnActiveDBConnect();

	inline void DebugError(const char* logstr,...);
	inline void DebugInfo(const char* logstr,...);

private:
	inline void        CheckDBConnect(otl_connect& DBConnect,std::string& strDB);
	inline void        ActiveDBConnect(otl_connect& DBConnect,std::string& strDB);
	inline void        DoConnect(otl_connect& DBConnect,std::string& strDB);
	inline void        CheckOTLException(const otl_exception &p,otl_connect& DBConnect,std::string& strDB);
	inline void        CatchDBException(const otl_exception &p);
	inline void        print_otl_error(const otl_exception &p);

	void               InitTableName(UINT32 curTime);

public:
	static volatile UINT32    s_WriteOperator;
	static volatile UINT32    s_ReadOperator;

	static UINT32             s_LastWriteOperator;
	static UINT32             s_LastReadOperator;
	static UINT32             s_LastTime;

	static void StaticInit();

	int WriteTableInfo(stTableInfo& stTI);

	int DBAddUserGameMoney(int AID,int PID,INT64 nAddMoney);
	int UpdateWinLossMoney(const stWinLossMoney& stWLM);

	int UpdateChangeBankMoney(INT16 AID,UINT32 PID,INT64 nGameMoney,INT64 nBankMoney);
	int CreatePlayerGameInfo(stCreateGameInfo& stCPGI);
	int UpdateUserProduct(int Idx,INT16 UseFlag);

	int ReadUserGameInfo(int AID,int PID,stDBGameInfo& stUGI);
	int ReadUserDataInfoByPID(int PID,stDBUserDataInfo& stUDI);
	int ReadUserDataInfoByUserName(int AID,const string& UserName,stDBUserDataInfo& stUDI);
	int ReadUserDataInfoByHardCode(const string& HardCode,stDBUserDataInfo& stUDI);

	int ReadBotPlayerData(INT16 AID,UINT32 StartPID,UINT32 EndPID,VectorBotPlayerData& listPD);
	int ReadFaceInfo(VectorFaceInfo& vectorFI);
	int ReadGiftInfo(VectorGiftInfo& vectorGI);
	int ReadProductInfo(VectorProductInfo& vectorPI);
	int ReadMatchInfo(INT16 ServerID,VectorDBMatchInfo& vectorMI);
	int ReadAreaInfo(VectorDBAreaInfo& vectorArea);
	int RegisterUser(const stRegisterUser& stRU);

	int ReadUserGameMoney(INT16 AID,UINT32 PID,stDBGameMoney& stGM);
	int ReadUserProduct(INT16 AID,UINT32 PID,VectorUserProduct& vectorUP);
	int ReadAllUserProduct(int MaxIdx,VectorUserProduct& vectorUP);
	int ReadRoomInfo(INT16 ServerID,VectorRoomInfo& vectorRI);
	int ReadTableInfo(INT16 ServerID,VectorTableInfo& vectorTI);	
	int ReadUserAward(INT16 AID,UINT32 PID,VectorAward& vectorAward);
	
private:
	CServer*                             m_pServer;
	CDBSConfig*                          m_pConfig;

	UINT32                               m_CurTime;

	std::string                          m_strDBConfig;
	otl_connect     					 m_DBConfigConnect;

	std::string                          m_strDBUser;
	otl_connect     					 m_DBUserConnect;

	std::string                          m_strDBGame;
	otl_connect     					 m_DBGameConnect;

	CMapFunctionTime                     m_FuncTime;

	//数据表的名称
	std::string                          m_TNMoney;
	std::string                          m_TNGameInfo;
	std::string                          m_TNWinLoss;
	std::string                          m_TNMoneyLog;
	std::string                          m_TNMatchResult;
	std::string                          m_TNMatchWinLoss;

	std::string                          m_TNBotInfo;

	std::string                          m_TNRoomInfo;
	std::string                          m_TNTableInfo;
	std::string                          m_TNMatchInfo;

	std::string                          m_TNChat;
	std::string                          m_TNLogin;
	std::string                          m_TNAction;

	std::string                          m_TNFaceInfo;
	std::string                          m_TNGiftInfo;
	std::string                          m_TNProductInfo;
	std::string                          m_TNAreaInfo;

	std::string                          m_TNUserProduct;
};

