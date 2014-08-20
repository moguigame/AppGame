#pragma once

#include<vector>
#include <string>

#include "boost/utility.hpp"

#include "MemcatchXY.h"
#include "aglibmemcached.h"

using namespace MoGui;
using namespace std;

class CMemOperator : public boost::noncopyable
{
public:

	CMemOperator(void);
	~CMemOperator(void);

	void Init(vector<string> vecServers);
	void TestConnect();

private:
	std::string GetKey(string str,int AID,int PID);
	void Connect();
	void DebugInfo(const char* logstr,...);
	void DebugError(const char* logstr,...);

	//将所有的关键字KEY值列出，以免重复
	//ss_thirteen_aid_pid
	//gsud_aid_pid
	//udi_aid_pid
	//land_aid_pid
	//ubi_aid_pid
	//ui_aid_pid
	//ugi_aid_pid
	//gift_idx
	//umi_aid_pid

public:
	bool GetSession(int AID,int PID,std::string& strSession);
	bool DelSession(int AID,int PID);

	bool GetGSUserData(int AID,int PID,MemcatchXY::DeZhou::memGSUserData& memGSUD);
	bool SetGSUserData(MemcatchXY::DeZhou::memGSUserData& memGSUD,time_t KeepTime=N_Time::DAY_2);

	bool GetUserLand(int AID,int PID,MemcatchXY::DeZhou::memUserLandRecord& memULR);
	bool SetUserLand(MemcatchXY::DeZhou::memUserLandRecord& memULR,time_t KeepTime=N_Time::Day_1);

	bool memGetUserDataInfo(int AID,int PID,MemcatchXY::DeZhou::memUserDataInfo& memUDI);
	bool memSetUserDataInfo(MemcatchXY::DeZhou::memUserDataInfo& memUDI,time_t KeepTime=N_Time::Month_1);
	bool memDelUserDataInfo(int AID,int PID);

public:
	bool                                 m_bCanUse;
	vector<string>                       m_vecMemIPPort;
	CAGLibmemcached                      m_MemCached;
};