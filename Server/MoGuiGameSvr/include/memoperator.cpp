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

std::string CMemOperator::GetKey(string str,INT16 AID,UINT32 PID)
{
	return str + "_" + Tool::N2S(AID) + "_" + Tool::N2S(PID);
}

bool CMemOperator::GetSession(INT16 AID,UINT32 PID,std::string& strSession)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		string strKey = "ss_dezhou_" + Tool::N2S(AID) + "_" + Tool::N2S(PID);
		string strKeyMD5 = Tool::GetMD5(strKey.c_str(),strKey.size());
		
		DebugInfo("GetSession PID=%d %s  %s",PID,strKey.c_str(),strKeyMD5.c_str());		
		return m_MemCached.Get_String(strKeyMD5,strSession);
	}
	return false;
}
bool CMemOperator::DelSession(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		string strKey = "ss_dezhou_" + Tool::N2S(AID) + "_" + Tool::N2S(PID);
		strKey = Tool::GetMD5(strKey.c_str(),strKey.size());
		return m_MemCached.Del(strKey);
	}
	return false;
}

bool CMemOperator::GetGSUserData(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memGSUserData& memGSUD)
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

bool CMemOperator::memGetUserDataInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserDataInfo& memUDI)
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

bool CMemOperator::memDelUserDataInfo(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		return m_MemCached.Del(GetKey("udi",AID,PID));
	}
	return false;
}

bool CMemOperator::GetUserLand(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserLandRecord& memULR)
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

bool CMemOperator::memGetUserBaseInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserBaseInfo& memUBI)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse)
	{
		memUBI.m_AID    = AID;
		memUBI.m_PID    = PID;

		return m_MemCached.Get_Data(GetKey("ubi",AID,PID),memUBI);
	}
	return false;
}
bool CMemOperator::memSetUserBaseInfo(const MemcatchXY::DeZhou::memUserBaseInfo& memUBI,time_t keeptime)
{
	assert( memUBI.m_AID>0 && memUBI.m_PID>0 );
	if ( memUBI.m_AID>0 && memUBI.m_PID>0 && m_bCanUse)
	{
		return m_MemCached.Set_Data(GetKey("ubi",memUBI.m_AID,memUBI.m_PID),memUBI,keeptime);
	}
	return false;
}
bool CMemOperator::memDelUserBaseInfo(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		return m_MemCached.Del(GetKey("ubi",AID,PID));
	}
	return false;
}

bool CMemOperator::memGetUserInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserInfo& memUI)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		memUI.m_AID   = AID;
		memUI.m_PID   = PID;

		return m_MemCached.Get_Data(GetKey("ui",AID,PID),memUI);
	}
	return false;
}
bool CMemOperator::memSetUserInfo(const MemcatchXY::DeZhou::memUserInfo& memUI,time_t keeptime)
{
	assert( memUI.m_AID>0 && memUI.m_PID>0 );
	if ( memUI.m_PID>0 && memUI.m_AID>0 && m_bCanUse)
	{
		return m_MemCached.Set_Data(GetKey("ui",memUI.m_AID,memUI.m_PID),memUI,keeptime);
	}
	return false;
}
bool CMemOperator::memDelUserInfo(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		return m_MemCached.Del(GetKey("ui",AID,PID));
	}
	return false;
}

bool CMemOperator::memGetUserMatchInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserMatchInfo& memUMI)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		memUMI.m_AID   = AID;
		memUMI.m_PID   = PID;
		return m_MemCached.Get_Data(GetKey("umi",AID,PID),memUMI);
	}
	return false;
}
bool CMemOperator::memSetUserMatchInfo(const MemcatchXY::DeZhou::memUserMatchInfo& memUMI,time_t keeptime)
{
	assert( memUMI.m_AID>0 && memUMI.m_PID>0 );
	if ( memUMI.m_PID>0 && memUMI.m_AID>0 && m_bCanUse)
	{
		return m_MemCached.Set_Data(GetKey("umi",memUMI.m_AID,memUMI.m_PID),memUMI,keeptime);
	}
	return false;
}
bool CMemOperator::memDelUserMatchInfo(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( PID>0 && AID>0 && m_bCanUse)
	{
		return m_MemCached.Del(GetKey("umi",AID,PID));
	}
	return false;
}

bool CMemOperator::memGetGift(UINT32 PID,int GiftIdx,MemcatchXY::DeZhou::memGiftInfo& memGI)
{
	assert( GiftIdx>0 );
	if ( GiftIdx > 0 && m_bCanUse )
	{
		memGI.m_GiftIdx = GiftIdx;
		string strKey = "gift_" + Tool::N2S(Tool::MakeUInt64(PID,UINT32(GiftIdx)));
		return m_MemCached.Get_Data(strKey,memGI);
	}
	return false;
}
bool CMemOperator::memSetGift(const MemcatchXY::DeZhou::memGiftInfo& memGI,time_t keeptime)
{
	assert( memGI.m_GiftIdx>0 );
	if ( memGI.m_GiftIdx > 0 && m_bCanUse )
	{
		string strKey = "gift_" + Tool::N2S(Tool::MakeUInt64(memGI.m_RecvPID,UINT32(memGI.m_GiftIdx)));
		return m_MemCached.Set_Data(strKey,memGI,keeptime);
	}
	return false;
}
bool CMemOperator::memDelGift(UINT32 PID,int GiftIdx)
{
	assert( GiftIdx>0 );
	if ( GiftIdx > 0 && m_bCanUse )
	{
		string strKey = "gift_" + Tool::N2S(Tool::MakeUInt64(PID,UINT32(GiftIdx)));
		return m_MemCached.Del(strKey);
	}
	return false;
}

bool CMemOperator::memGetUserGameInfo(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserGameInfo& memUGI)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		memUGI.m_AID  = AID;
		memUGI.m_PID  = PID;

		return m_MemCached.Get_Data(GetKey("ugi",AID,PID),memUGI);
	}
	return false;
}
bool CMemOperator::memSetUserGameInfo(MemcatchXY::DeZhou::memUserGameInfo& memUGI,time_t keeptime)
{
	assert( memUGI.m_AID>0 && memUGI.m_PID>0 );
	if ( memUGI.m_AID>0 && memUGI.m_PID>0 && m_bCanUse )
	{
		return m_MemCached.Set_Data(GetKey("ugi",memUGI.m_AID,memUGI.m_PID),memUGI,keeptime);
	}
	DebugError("memSetUserGameInfo AID=%d PID=%d",memUGI.m_AID,memUGI.m_PID);
	return false;
}
bool CMemOperator::memDelUserGameInfo(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		return m_MemCached.Del(GetKey("ugi",AID,PID));
	}
	return false;
}

bool CMemOperator::memGetUserGameInfoEx(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserGameInfoEx& memUGIE)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		memUGIE.m_AID  = AID;
		memUGIE.m_PID  = PID;

		return m_MemCached.Get_Data(GetKey("ugie",AID,PID),memUGIE);
	}
	return false;
}
bool CMemOperator::memSetUserGameInfoEx(MemcatchXY::DeZhou::memUserGameInfoEx& memUGIE,time_t keeptime)
{
	assert( memUGIE.m_AID>0 && memUGIE.m_PID>0 );
	if ( memUGIE.m_AID>0 && memUGIE.m_PID>0 && m_bCanUse )
	{
		return m_MemCached.Set_Data(GetKey("ugie",memUGIE.m_AID,memUGIE.m_PID),memUGIE,keeptime);
	}
	return false;
}
bool CMemOperator::memDelUserGameInfoEx(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		return m_MemCached.Del(GetKey("ugie",AID,PID));
	}
	return false;
}

bool CMemOperator::memGetUserGameWinRecord(INT16 AID,UINT32 PID,MemcatchXY::DeZhou::memUserGameWinRecord& memUGWR)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		memUGWR.m_AID  = AID;
		memUGWR.m_PID  = PID;

		return m_MemCached.Get_Data(GetKey("ugwr",AID,PID),memUGWR);
	}
	return false;
}
bool CMemOperator::memSetUserGameWinRecord(MemcatchXY::DeZhou::memUserGameWinRecord& memUGWR,time_t keeptime)
{
	assert( memUGWR.m_AID>0 && memUGWR.m_PID>0 );
	if ( memUGWR.m_AID>0 && memUGWR.m_PID>0 && m_bCanUse )
	{
		return m_MemCached.Set_Data(GetKey("ugwr",memUGWR.m_AID,memUGWR.m_PID),memUGWR,keeptime);
	}
	return false;
}
bool CMemOperator::memDelUserGameWinRecord(INT16 AID,UINT32 PID)
{
	assert( AID>0 && PID>0 );
	if ( AID>0 && PID>0 && m_bCanUse )
	{
		return m_MemCached.Del(GetKey("ugwr",AID,PID));
	}
	return false;
}

bool CMemOperator::memAddUserGameMoney(INT16 AID,UINT32 PID,INT64 nMoney)
{
	memUserGameInfo memUGI;
	if ( memGetUserGameInfo(AID,PID,memUGI) && memUGI.m_nGameMoney+nMoney>=0 )
	{
		memUGI.m_nGameMoney += nMoney;
		memSetUserGameInfo(memUGI);
		return true;
	}
	DebugError("memAddUserGameMoney AID=%d PID=%d nMoney=%s",AID,PID,Tool::N2S(nMoney).c_str() );
	return false;
}
bool CMemOperator::memAddUserMoguiMoney(INT16 AID,UINT32 PID,INT32 nMoney)
{
	memUserGameInfo memUGI;
	if ( memGetUserGameInfo(AID,PID,memUGI) && memUGI.m_nMoguiMoney+nMoney>=0 )
	{
		memUGI.m_nMoguiMoney += nMoney;
		memSetUserGameInfo(memUGI);
		return true;
	}
	DebugError("memAddUserMoguiMoney AID=%d PID=%d nMoguiMoney=%d",AID,PID,nMoney);
	return false;
}