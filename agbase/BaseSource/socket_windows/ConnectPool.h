//=============================================================================
/*
*
*  @author guyongliang<guyongliang@gameabc.com>, ʢ������ 
*  @version 1.0.0.1
*  @date 2010-06-06
*
*  Copyright (C) 2010-2012 - All Rights Reserved
*/
//=============================================================================

#pragma once

//#ifndef __AGBASE_CONNECTPOOL_H__
//#define __AGBASE_CONNECTPOOL_H__


namespace AGBase
{

class CIOCP;
class CDispatcher;
class CConnect;

class CConnectPool 
	: public IConnectPool
{
public:
	CConnectPool( void );

	virtual ~CConnectPool( void );

	// ע��ص�
	virtual void SetCallback( IConnectPoolCallback* callback );

	// ��������
	virtual bool Start( int port, int clientcnt, int connectcnt );

	// ���ӽ�ֹ�����IP
	virtual void AddForbidIP( const char* ip );

	// ɾ����ֹ�����IP, ip==0ɾ��ȫ��
	virtual void DelForbidIP( const char* ip );

	// ֹͣ����
	virtual void Stop( void );

	// ���ӳ�ȥ
	virtual IConnect* Connect(const char* ip, int port);

	// �����¼�������true����ʾ���������¼�
	virtual bool OnPriorityEvent( void );

	// ��ʱ������������Ϊ1��
	virtual void OnTimer( void );

	// ���յ�����
	virtual void OnAccept( CConnect* connect );

	// û������callback��IConnect�رյ�ʱ���ص�����¼�
	virtual void OnClose( CConnect* connect, bool bactive, bool nocallback );

	// û���κ���Ϣ��ʱ��
	void OnIdle( void );

private:
	IConnectPoolCallback*	m_callback;
	CDispatcher*			m_dispatcher;
	CIOCP*					m_iocp;
	int						m_status;
};


}

//#endif //__AGBASE_CONNECTPOOL_H__

