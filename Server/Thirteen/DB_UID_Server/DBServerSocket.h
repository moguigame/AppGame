#pragma once

#include "MoGuiServerSocket.h"

class CServer;

class CDBServerSocket : public CMoGuiServerSocket,public CMemoryPool_Public<CDBServerSocket, 1>, public boost::noncopyable
{
public:
	CDBServerSocket( CServer* server, IConnect* connect );
	virtual ~CDBServerSocket(void);

public:
	CServer*	         m_pServer;

	UINT32               m_nStartTime;         //连接上来的时间
	INT64                m_nHeartID;           //上次发送心跳包的值

	int                  GetServerID()const { return m_ServerID; }
	void                 SetServerID(int SID){ m_ServerID=SID; }
	int                  GetClass()const{ return m_ServerClass; }
	void                 SetClass(int CC){m_ServerClass = CC; }
	void                 SetLogin(int login){m_Login = login; }
	bool                 IsLogin()const{ return m_Login!=0; }

	UINT32               GetActiveTime(){ return m_nActiveTime; }
	void                 SetActiveTime(UINT32 nTime){ m_nActiveTime=nTime; }
	int                  GetOutTimes()const{ return m_TimeOutTimes; }

private:
	int                  m_ServerID;
	int                  m_ServerClass;
	int                  m_Login;

	int                  m_TimeOutTimes;
	UINT32               m_nActiveTime;    //上次发心跳包的时间

public:
	int                  TransMsg( CRecvMsgPacket& msgPack );
	int                  OnRecvReqHeat( CRecvMsgPacket& msgPack );
	void                 OnCheckHeart();

public:
	void OnConnect( void );
	void OnClose( bool bactive );
	int  OnMsg( const char* buf, int len );

private:
	CDBServerSocket(const CDBServerSocket &);
	CDBServerSocket &operator=(const CDBServerSocket &);
};
