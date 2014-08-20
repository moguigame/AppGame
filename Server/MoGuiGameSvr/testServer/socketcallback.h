#pragma once

#include "MoGui.h"
#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "MoGuiClientSocket.h"

class CSocketCallBack
{
public:
	virtual int OnMsg(CMoGuiClientSocket* pSocket,CRecvMsgPacket& msgPack) = 0;
	virtual int OnTimer(UINT32) = 0;
	virtual int StartMsg() = 0;
};
