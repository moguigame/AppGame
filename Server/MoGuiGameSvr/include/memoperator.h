#pragma once

#include<vector>
#include <string>

#include "boost/utility.hpp"

#include "MemcatchXY.h"
#include "aglibmemcached.h"

class CMemOperator : public boost::noncopyable
{
public:

	CMemOperator(void);
	~CMemOperator(void);

	void Init(std::vector<std::string> vecServers);
	void TestConnect();

private:
	std::string GetKey(const std::string& str, INT16 AID, UINT32 PID);
	void Connect();
	void DebugInfo(const char* logstr,...);
	void DebugError(const char* logstr,...);

	//将所有的关键字KEY值列出，以免重复
	//ss_dezhou_aid_pid
	//gsud_aid_pid
	//udi_aid_pid
	//land_aid_pid
	//ubi_aid_pid
	//ui_aid_pid
	//ugi_aid_pid
	//gift_idx
	//umi_aid_pid

public:
	bool GetSession(INT16 AID,UINT32 PID,std::string& strSession);
	bool DelSession(INT16 AID,UINT32 PID);

	bool GetGSUserData(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memGSUserData& memGSUD);
	bool SetGSUserData(MemcatchXY::DeZhou::memGSUserData& memGSUD,time_t KeepTime=N_Time::DAY_2);

	bool GetUserLand(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserLandRecord& memULR);
	bool SetUserLand(MemcatchXY::DeZhou::memUserLandRecord& memULR,time_t KeepTime=N_Time::Day_1);

	bool memGetUserDataInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserDataInfo& memUDI);
	bool memSetUserDataInfo(MemcatchXY::DeZhou::memUserDataInfo& memUDI,time_t KeepTime=N_Time::Month_1);
	bool memDelUserDataInfo(INT16 AID,UINT32 PID);

	bool memGetUserBaseInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserBaseInfo& memUBI);
	bool memSetUserBaseInfo(const MemcatchXY::DeZhou::memUserBaseInfo& memUBI,time_t keeptime=N_Time::Month_1);
	bool memDelUserBaseInfo(INT16 AID,UINT32 PID);

	bool memGetUserInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserInfo& memUI);
	bool memSetUserInfo(const MemcatchXY::DeZhou::memUserInfo& memUI,time_t keeptime=N_Time::Month_1);
	bool memDelUserInfo(INT16 AID,UINT32 PID);

	bool memGetUserMatchInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserMatchInfo& memUMI);
	bool memSetUserMatchInfo(const MemcatchXY::DeZhou::memUserMatchInfo& memUMI,time_t keeptime=N_Time::DAY_2);
	bool memDelUserMatchInfo(INT16 AID,UINT32 PID);

	bool memGetUserGameInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserGameInfo& memUGI);
	bool memSetUserGameInfo(MemcatchXY::DeZhou::memUserGameInfo& memUGIE,time_t keeptime=N_Time::DAY_2);
	bool memDelUserGameInfo(INT16 AID,UINT32 PID);

	bool memGetUserGameInfoEx(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserGameInfoEx& memUGIE);
	bool memSetUserGameInfoEx(MemcatchXY::DeZhou::memUserGameInfoEx& memUGIE,time_t keeptime=N_Time::DAY_2);
	bool memDelUserGameInfoEx(INT16 AID,UINT32 PID);

	bool memGetUserGameWinRecord(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserGameWinRecord& memUGWR);
	bool memSetUserGameWinRecord(MemcatchXY::DeZhou::memUserGameWinRecord& memUGWR,time_t keeptime=N_Time::DAY_2);
	bool memDelUserGameWinRecord(INT16 AID,UINT32 PID);

	bool memGetGift(UINT32 PID,int Idx,MemcatchXY::DeZhou::memGiftInfo& memGI);
	bool memSetGift(const MemcatchXY::DeZhou::memGiftInfo& memGI,time_t keeptime=N_Time::DAY_2);
	bool memDelGift(UINT32 PID,int GiftIdx);

	bool memAddUserGameMoney(INT16 AID,UINT32 PID,INT64 nMoney);
	bool memAddUserMoguiMoney(INT16 AID,UINT32 PID,INT32 nMoney);

public:
	bool                                 m_bCanUse;
	std::vector<std::string>             m_vecMemIPPort;
	CAGLibmemcached                      m_MemCached;
};