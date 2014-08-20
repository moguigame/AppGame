#pragma once

#include "boost/shared_ptr.hpp"

#include "MoGuiGame.h"
#include "gamedezhou.h"

namespace MoGui
{
namespace MoGuiXY
{
	class MessageBlock : public CMemoryPool_Public<MessageBlock>
	{
	public:
		enum { MaxMsgLen = 1024 };

		short                  m_MsgID;
		short                  m_MsgLen;
		INT64                  m_nTimeCount;
		char                   m_Message[MaxMsgLen];

		MessageBlock() { ReSet();}
		virtual ~MessageBlock(){}
		void ReSet()
		{
			m_MsgID = 0;
			m_MsgLen = 0;
			m_nTimeCount = 0;
			memset(m_Message,0,MaxMsgLen);
		}
	};
	typedef boost::shared_ptr<MessageBlock> PMsgBlock;
	typedef queue<PMsgBlock>                QueueBlockMsg;
	typedef list<PMsgBlock>                 ListBlockMsg;

	template<class Txieyi>
	void MakeMsgToBlock(Txieyi& xieyi,MessageBlock& src)
	{
		src.ReSet();
		src.m_MsgID = xieyi.XY_ID;

		bostream bos;
		bos.attach(src.m_Message,src.MaxMsgLen);
		bos<<xieyi;

		src.m_MsgLen = static_cast<short>(bos.length());
	}

	template<class Txieyi>
	int ExplainBlockToMsg(MessageBlock& src,Txieyi& xieyi)
	{
		int ret = 0;

		bistream bis;
		bis.attach(src.m_Message,src.m_MsgLen);

		try
		{
			bis >> xieyi;
		}
		catch ( agproexception* e )
		{
			ret = 10000+e->m_cause;
		}
		catch ( biosexception* e )
		{
			ret = 20000+e->m_cause;
		}
		catch(...)
		{
			ret = 30000;
		}

		if ( ret )
		{
			printf_s("ExplainBlockToMsg Error Ret=%d MsgID=%d MsgLen=%d \n",ret,src.m_MsgID,src.m_MsgLen);
		}

		return ret;
	}
}
}