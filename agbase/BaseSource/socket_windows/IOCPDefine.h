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


namespace AGBase
{
	// socketϵͳ���ջ���Ĭ�ϳ���,
	const unsigned int _DEFAULT_RECV_BUFF		= 8192;
	// socketϵͳ���ͻ���Ĭ�ϳ���,
	const unsigned int _DEFAULT_SEND_BUFF		= 8192;//1024;
	// ÿ��������Ĵ�С
	const unsigned int _MAX_BUFFER_LENGTH		= 1024;
	// must > _MAX_BUFFER_LENGTH*2
	const unsigned int _MAX_RECV_BUFFER_LENGTH	= 4096;
	// must > _MAX_BUFFER_LENGTH*2
	const unsigned int _MAX_SEND_BUFFER_LENGTH	= 4096;
	// ��Ϣ������󳤶�
	const unsigned int _MAX_PACKET_LENGTH		= 4096;
	// must = _MAX_PACKET_LENGTH+_MAX_BUFFER_LENGTH
	const unsigned int _MAX_LOGIC_BUFFER_LENGTH = _MAX_PACKET_LENGTH+_MAX_BUFFER_LENGTH;
	// SELECT����ܹ����ܵ�fd��Ŀ
	const unsigned int _MAX_ACCEPT_FD_SIZE		= 20000;
	//
	const unsigned int _MAX_CONNECT_FD_SIZE		= 10000;
}

//#define AGBASE_DEBUG

// �Ƿ������ܣ�����������ܣ���ô�����һ��2�ֽڵ���Ϣͷ��
// ��������Ϣͷ�� IConnect::Send ����󳤶�ֻ��<=(_MAX_PACKET_LENGTH-4)�����ٿ��Է������޳�����Ϣ
//	��Ӧ�Ľ���IConnectCallback::OnMsgҲ����������軺������
// ��ǰ������
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

