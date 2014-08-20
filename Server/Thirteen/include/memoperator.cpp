#include "memoperator.h"

#include <stdio.h>
#include <wtypes.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <vector>

#include "Base.h"
#include "Tool.h"

using namespace std;
using namespace AGBase;
using namespace MoGui;
using namespace MoGui::MemcatchXY::DeZhou;

CMemOperator::CMemOperator(void)
{
	m_bCanUse = false;
}

CMemOperator::~CMemOperator(void)
{
}

void CMemOperator::DebugInfo(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1]={0};
	va_list args;
	va_start(args,logstr);
	//int len = _vsnprintf(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if( len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_INFO,logbuf);
	}
}
void CMemOperator::DebugError(const char* logstr,...)
{
	static char logbuf[MAX_LOG_BUF_SIZE+1]={0};
	va_list args;
	va_start(args,logstr);
	//int len = _vsnprintf(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
	va_end(args);
	if( len>0 && len<=MAX_LOG_BUF_SIZE )
	{
		Log_Text(LOGLEVEL_ERROR,logbuf);
		fprintf_s(stderr,"%s %s\n",Tool::GetTimeString(time(nullptr)).c_str(),logbuf);
	}
}

void CMemOperator::Init(vector<string> vecServers)
{
	m_vecMemIPPort = vecServers;
	Connect();
}

void CMemOperator::TestConnect()
{
	if ( !m_MemCached.Set_String("test_game_start",Tool::N2S(int(time(NULL)))) )
	{
		DebugError("Can't set memcached ");
		Connect();
	}
}

void CMemOperator::Connect()
{
	m_bCanUse = true;

	if ( m_vecMemIPPort.size() > 1 )
	{
		int nSize = int(m_vecMemIPPort.size());
		std::string* pIpPort = new string[nSize];
		for (int nLoop=0;nLoop<nSize;nLoop++)
		{
			pIpPort[nLoop] = m_vecMemIPPort.at(nLoop);
		}
		if ( m_MemCached.Connect(pIpPort,nSize) == false )
		{
			DebugError("Can't connect memcached ");
		}
		safe_delete_arr(pIpPort);
	}
	else if ( m_vecMemIPPort.size() == 1 )
	{
		if ( m_MemCached.Connect(m_vecMemIPPort.at(0)) == false )
		{
			DebugError("Can't connect memcached ");
		}
	}

	if ( !m_MemCached.Set_String("test_game_start",Tool::N2S(int(time(NULL)))) )
	{
		m_bCanUse = false;
		DebugError("Can't set memcached ");
	}
}

std::string CMemOperator::GetKey(string str,int AID,int PID)
{
	return str + "_" + Tool::N2S(AID) + "_" + Tool::N2S(PID);
}

bool CMemOperator::GetSession(int AID,int PID,std::string& strSession)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		string strKey = "ss_thirteen_" + Tool::N2S(AID) + "_" + Tool::N2S(PID);
		string strKeyMD5 = Tool::GetMD5(strKey.c_str(),strKey.size());
		
		DebugInfo("GetSession PID=%d %s  %s",PID,strKey.c_str(),strKeyMD5.c_str());		
		return m_MemCached.Get_String(strKeyMD5,strSession);
	}
	return false;
}
bool CMemOperator::DelSession(int AID,int PID)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		string strKey = "ss_thirteen_" + Tool::N2S(AID) + "_" + Tool::N2S(PID);
		strKey = Tool::GetMD5(strKey.c_str(),strKey.size());
		return m_MemCached.Del(strKey);
	}
	return false;
}

bool CMemOperator::GetGSUserData(int AID,int PID,MemcatchXY::DeZhou::memGSUserData& memGSUD)
{
	assert( AID>0 && PID>0 );
	if (PID>0 && AID>0 && m_bCanUse)
	{
		memGSUD.m_AID = AID;
		memGSUD.m_PID = PID;
		
		return m_MemCached.Get_Data( GetKey("gsud",AID,PID),memGSUD );
	}
	return false;
}
bool CMemOperator::SetGSUserData(MemcatchXY::DeZhou::memGSUserData& memGSUD,time_t KeepTime)
{
	assert( memGSUD.m_AID>0 && memGSUD.m_PID>0 );
	if ( memGSUD.m_AID>0 && memGSUD.m_PID>0 && m_bCanUse )
	{
		return m_MemCached.Set_Data(GetKey("gsud",memGSUD.m_AID,memGSUD.m_PID),memGSUD,KeepTime);
	}
	return false;
}

bool CMemOperator::memGetUserDataInfo(int AID,int PID,MemcatchXY::DeZhou::memUserDataInfo& memUDI)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		memUDI.m_AID = AID;
		memUDI.m_PID = PID;
		return m_MemCached.Get_Data(GetKey("udi",AID,PID),memUDI);
	}
	return false;
}
bool CMemOperator::memSetUserDataInfo(MemcatchXY::DeZhou::memUserDataInfo& memUDI,time_t KeepTime)
{
	assert( memUDI.m_AID>0 && memUDI.m_PID>0 );
	if ( memUDI.m_AID>0 && memUDI.m_PID>0 && m_bCanUse)
	{
		return m_MemCached.Set_Data(GetKey("udi",memUDI.m_AID,memUDI.m_PID),memUDI,KeepTime);
	}
	return false;
}

bool CMemOperator::memDelUserDataInfo(int AID,int PID)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		return m_MemCached.Del(GetKey("udi",AID,PID));
	}
	return false;
}

bool CMemOperator::GetUserLand(int AID,int PID,MemcatchXY::DeZhou::memUserLandRecord& memULR)
{
	assert( AID>0 && PID>0 );
	if (PID>0 && AID>0 && m_bCanUse)
	{
		memULR.m_AID = AID;
		memULR.m_PID = PID;

		return m_MemCached.Get_Data(GetKey("land",AID,PID),memULR);
	}
	return false;
}
bool CMemOperator::SetUserLand(MemcatchXY::DeZhou::memUserLandRecord& memULR,time_t KeepTime)
{
	assert( memULR.m_AID>0 && memULR.m_PID>0 );
	if (memULR.m_AID>0 && memULR.m_PID>0 && m_bCanUse)
	{
		return m_MemCached.Set_Data(GetKey("land",memULR.m_AID,memULR.m_PID),memULR,KeepTime);
	}
	return false;
}