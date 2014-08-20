#pragma once

#include <iostream>

using namespace std;

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
	void DebugError(const string& strLog)
	{
		cout<<strLog<<" Error----------------------------------------"<<endl;
	}
	bool CeshiMode()
	{
		return m_CeShiMode != 0;
	}

	void Init()
	{
		string strPath;
		char CurPath[256],TempBuf[256];

		::GetCurrentDirectory(255,CurPath);
		strPath = string(CurPath)+"\\"+"dbs_dz.ini";

		m_GameID = ::GetPrivateProfileInt("serverconfig","gameid",0,strPath.c_str());
		if ( m_GameID == 0 ) DebugError("m_GameID");
		cout<<"GameID="<<m_GameID<<endl;

		m_Port = ::GetPrivateProfileInt("serverconfig","port",0,strPath.c_str());
		if(m_Port == 0) DebugError("m_Port");
		cout<<"m_Port="<<m_Port<<endl;

		m_ConnectCount = ::GetPrivateProfileInt("serverconfig","connectcount",0,strPath.c_str());
		if(m_ConnectCount == 0) DebugError("m_ConnectCount");
		cout<<"m_ConnectCount="<<m_ConnectCount<<endl;

		m_OutCount = ::GetPrivateProfileInt("serverconfig","outcount",0,strPath.c_str());
		if(m_OutCount == 0) DebugError("m_OutCount");
		cout<<"m_OutCount="<<m_OutCount<<endl;

		m_MemCount = ::GetPrivateProfileInt("serverconfig","cachcount",0,strPath.c_str());
		if(m_MemCount == 0) DebugError("m_MemCount");
		cout<<"m_MemCount="<<m_MemCount<<endl;

		if( m_MemCount > 0 )
		{
			string strTemp,strKey;
			for (int nLoop=0;nLoop<m_MemCount;nLoop++)
			{
				strKey = "cach" + Tool::N2S(nLoop+1);
				::GetPrivateProfileString("serverconfig",strKey.c_str(),"",TempBuf,255,strPath.c_str());
				strTemp = string(TempBuf);
				if (strTemp.empty()) cout<<"strTemp Error"<<endl;
				cout<<"strTemp="<<strTemp<<endl;
				m_MemcachIpPort.push_back(strTemp);
			}
		}
		
		//::GetPrivateProfileString("serverconfig","connect","",TempBuf,255,strPath.c_str());
		//m_strDBConnect = string(TempBuf);
		//if (m_strDBConnect.empty()) DebugError("m_strDBConnect");
		//cout<<"m_strDBConnect="<<m_strDBConnect<<endl;

		m_ThreadCount = ::GetPrivateProfileInt("serverconfig","threadcount",0,strPath.c_str());
		if( m_ThreadCount<1 || m_ThreadCount>4 ) DebugError("m_ThreadCount");
		cout<<"m_DBCount="<<m_ThreadCount<<endl;

		m_QuickDB = ::GetPrivateProfileInt("serverconfig","quickdb",0,strPath.c_str());
		cout<<"m_QuickDB="<<m_QuickDB<<endl;

		m_CeShiMode = ::GetPrivateProfileInt("serverconfig","ceshimode",0,strPath.c_str());
		cout<<"m_CeShiMode="<<m_CeShiMode<<endl;

		std::string strName,strPW;
		::GetPrivateProfileString("serverconfig","name","",TempBuf,255,strPath.c_str());
		strName = string(TempBuf);

		::GetPrivateProfileString("serverconfig","password","",TempBuf,255,strPath.c_str());
		strPW = string(TempBuf);

		if ( strName.length() && strPW.length() )
		{
			m_NamePW = strName + "/" + strPW;

			m_strDBConfig = m_NamePW + "@b_Config";
			m_strDBUser   = m_NamePW + "@b_User";
			m_strDBGame   = m_NamePW + "@b_Game";
			m_strDBLog    = m_NamePW + "@b_Log";
			//m_strDBFriend = m_NamePW + "@Friend";
			//m_strDBProp   = m_NamePW + "@Prop";
		}
		else
		{
			DebugError(" Name Password ");
		}

		cout<<endl;
		cout<<endl;
	}

public:
	int                m_GameID;

	int                m_Port;
	int                m_ConnectCount;
	int                m_OutCount;

	int                m_MemCount;
	vector<string>     m_MemcachIpPort;

	string             m_NamePW;
	string             m_strDBConfig;
	string             m_strDBUser;
	string             m_strDBGame;
	string             m_strDBLog;
	string             m_strDBFriend;
	string             m_strDBProp;

	int                m_ThreadCount;
	int                m_QuickDB;             //指是否要通过多线程快速记录玩家的游戏币
	int                m_CeShiMode;

	int                m_InviteAward;         //邀请到玩家的奖励游戏币数量
};

