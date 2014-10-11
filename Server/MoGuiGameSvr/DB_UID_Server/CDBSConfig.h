#pragma once

#include <iostream>
#include <string>
#include <vector>

class CDBSConfig
{
public:
	CDBSConfig()
	{
		m_GameID = 0;

		m_Port = 0;
		m_ConnectCount = 0;
		m_OutCount = 0;

		m_MemCount = 0;
		m_MemcachIpPort.clear();

		m_NamePW = "xuyingming/xuyingming";

		m_InviteAward = 10000;

		m_CeShiMode = 0;
		m_QuickDB = 0;
		m_ThreadCount = 2;
		OnNewDay(UINT32(time(NULL)));
	}
	~CDBSConfig(){}

	void OnNewDay(UINT32 CurTime)
	{
	}
	void DebugError(const std::string& strLog)
	{
		std::cout << strLog << " Error----------------------------------------" << std::endl;
	}
	bool CeshiMode()
	{
		return m_CeShiMode != 0;
	}

	void Init()
	{
		std::string strPath;
		char CurPath[256],TempBuf[256];

		::GetCurrentDirectory(255,CurPath);
		strPath = std::string(CurPath) + "\\" + "dbs_dz.ini";

		m_GameID = ::GetPrivateProfileInt("serverconfig","gameid",0,strPath.c_str());
		if ( m_GameID == 0 ) DebugError("m_GameID");
		std::cout << "GameID=" << m_GameID << std::endl;

		m_Port = ::GetPrivateProfileInt("serverconfig","port",0,strPath.c_str());
		if(m_Port == 0) DebugError("m_Port");
		std::cout << "m_Port=" << m_Port << std::endl;

		m_ConnectCount = ::GetPrivateProfileInt("serverconfig","connectcount",0,strPath.c_str());
		if(m_ConnectCount == 0) DebugError("m_ConnectCount");
		std::cout << "m_ConnectCount=" << m_ConnectCount << std::endl;

		m_OutCount = ::GetPrivateProfileInt("serverconfig","outcount",0,strPath.c_str());
		if(m_OutCount == 0) DebugError("m_OutCount");
		std::cout << "m_OutCount=" << m_OutCount << std::endl;

		m_MemCount = ::GetPrivateProfileInt("serverconfig","cachcount",0,strPath.c_str());
		if(m_MemCount == 0) DebugError("m_MemCount");
		std::cout << "m_MemCount=" << m_MemCount << std::endl;

		if( m_MemCount > 0 )
		{
			std::string strTemp, strKey;
			for (int nLoop=0;nLoop<m_MemCount;nLoop++)
			{
				strKey = "cach" + Tool::N2S(nLoop+1);
				::GetPrivateProfileString("serverconfig",strKey.c_str(),"",TempBuf,255,strPath.c_str());
				strTemp = std::string(TempBuf);
				if (strTemp.empty()) std::cout << "strTemp Error" << std::endl;
				std::cout << "strTemp=" << strTemp << std::endl;
				m_MemcachIpPort.push_back(strTemp);
			}
		}
		
		//::GetPrivateProfileString("serverconfig","connect","",TempBuf,255,strPath.c_str());
		//m_strDBConnect = std::string(TempBuf);
		//if (m_strDBConnect.empty()) DebugError("m_strDBConnect");
		//cout<<"m_strDBConnect="<<m_strDBConnect<<endl;

		m_ThreadCount = ::GetPrivateProfileInt("serverconfig","threadcount",0,strPath.c_str());
		if( m_ThreadCount<1 || m_ThreadCount>4 ) DebugError("m_ThreadCount");
		std::cout << "m_DBCount=" << m_ThreadCount << std::endl;

		m_QuickDB = ::GetPrivateProfileInt("serverconfig","quickdb",0,strPath.c_str());
		std::cout << "m_QuickDB=" << m_QuickDB << std::endl;

		m_CeShiMode = ::GetPrivateProfileInt("serverconfig","ceshimode",0,strPath.c_str());
		std::cout << "m_CeShiMode=" << m_CeShiMode << std::endl;

		std::string strName,strPW;
		::GetPrivateProfileString("serverconfig","name","",TempBuf,255,strPath.c_str());
		strName = std::string(TempBuf);

		::GetPrivateProfileString("serverconfig","password","",TempBuf,255,strPath.c_str());
		strPW = std::string(TempBuf);

		if ( strName.length() && strPW.length() )
		{
			m_NamePW = strName + "/" + strPW;

			m_strDBUser   = m_NamePW + "@User";
			m_strDBGame   = m_NamePW + "@Game";
			m_strDBLog    = m_NamePW + "@Log";
		}
		else
		{
			DebugError(" Name Password ");
		}

		std::cout << std::endl;
		std::cout << std::endl;
	}

public:
	int                m_GameID;

	int                m_Port;
	int                m_ConnectCount;
	int                m_OutCount;

	int                m_MemCount;

	std::vector<std::string>     m_MemcachIpPort;

	std::string             m_NamePW;
	std::string             m_strDBUser;
	std::string             m_strDBGame;
	std::string             m_strDBLog;

	int                m_ThreadCount;
	int                m_QuickDB;             //指是否要通过多线程快速记录玩家的游戏币
	int                m_CeShiMode;

	int                m_InviteAward;         //邀请到玩家的奖励游戏币数量
};

