//=============================================================================
/*
*
*  @author guyongliang<guyongliang@gameabc.com>, 盛大网络 
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

	// 注册回调
	virtual void SetCallback( IConnectPoolCallback* callback );

	// 启动服务
	virtual bool Start( int port, int clientcnt, int connectcnt );

	// 增加禁止进入的IP
	virtual void AddForbidIP( const char* ip );

	// 删除禁止进入的IP, ip==0删除全部
	virtual void DelForbidIP( const char* ip );

	// 停止服务
	virtual void Stop( void );

	// 连接出去
	virtual IConnect* Connect(const char* ip, int port);

	// 优先事件，返回true，表示存在优先事件
	virtual bool OnPriorityEvent( void );

	// 定时器触发，周期为1秒
	virtual void OnTimer( void );

	// 接收到连接
	virtual void OnAccept( CConnect* connect );

	// 没有设置callback的IConnect关闭的时候会回调这个事件
	virtual void OnClose( CConnect* connect, bool bactive, bool nocallback );

	// 没有任何消息的时候
	void OnIdle( void );

private:
	IConnectPoolCallback*	m_callback;
	CDispatcher*			m_dispatcher;
	CIOCP*					m_iocp;
	int						m_status;
};


}

//#endif //__AGBASE_CONNECTPOOL_H__

