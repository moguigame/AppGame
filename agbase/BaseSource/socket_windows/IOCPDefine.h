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


namespace AGBase
{
	// socket系统接收缓存默认长度,
	const unsigned int _DEFAULT_RECV_BUFF		= 8192;
	// socket系统发送缓存默认长度,
	const unsigned int _DEFAULT_SEND_BUFF		= 8192;//1024;
	// 每个缓存包的大小
	const unsigned int _MAX_BUFFER_LENGTH		= 1024;
	// must > _MAX_BUFFER_LENGTH*2
	const unsigned int _MAX_RECV_BUFFER_LENGTH	= 4096;
	// must > _MAX_BUFFER_LENGTH*2
	const unsigned int _MAX_SEND_BUFFER_LENGTH	= 4096;
	// 消息包的最大长度
	const unsigned int _MAX_PACKET_LENGTH		= 4096;
	// must = _MAX_PACKET_LENGTH+_MAX_BUFFER_LENGTH
	const unsigned int _MAX_LOGIC_BUFFER_LENGTH = _MAX_PACKET_LENGTH+_MAX_BUFFER_LENGTH;
	// SELECT最大能够接受的fd数目
	const unsigned int _MAX_ACCEPT_FD_SIZE		= 20000;
	//
	const unsigned int _MAX_CONNECT_FD_SIZE		= 10000;
}

//#define AGBASE_DEBUG

// 是否开启加密，如果开启加密，那么会产生一个2字节的消息头，
// 定义了消息头后 IConnect::Send 的最大长度只能<=(_MAX_PACKET_LENGTH-4)，不再可以发送无限长度消息
//	相应的接收IConnectCallback::OnMsg也不会出现自设缓存的情况
// 当前不可用
//#define AGBASE_OPEN_ENCRYPTION

#include <deque>
#include <set>

#include "../common/InsideBase.h"

#include <Include/base/SocketInterface.h>
#include <Include/base/MemoryPool.h>

#include <Include/base/Utils.h>
#include <Include/base/Lock.h>
#include <Include/base/Condition.h>
#include <Include/base/Thread.h>

#include "Packet.h"

