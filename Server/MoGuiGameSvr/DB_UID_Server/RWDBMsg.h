#pragma once

#include "boost/utility.hpp"

#include "Base.h"

#include "DBOperatorXY.h"
#include "Tool.h"

#include "gamedef.h"

using namespace MoGui::MoGuiXY::RWDB_XY;
using namespace MoGui::Game;

class CRWDBMsgManage : public boost::noncopyable
{
public:
	CRWDBMsgManage()
	{
		UINT32 nowTime = UINT32(time(NULL));
		m_StartTime = nowTime;
		m_CheckTime = nowTime;
		m_nFinishCount = 0;

		m_MaxTicketCount = 0;
		m_TotalWaitTicket = 0;
	}
	~CRWDBMsgManage()
	{
	}
	void DebugLogOut(const char* logstr,...)
	{
		static char logbuf[MAX_LOG_BUF_SIZE+1] = {0};
		va_list args;
		va_start(args, logstr);
		int len = _vsnprintf_s(logbuf, MAX_LOG_BUF_SIZE, logstr, args);
		va_end(args);
		if (len>0 && len<=MAX_LOG_BUF_SIZE )
		{
			AGBase::Log_Text(AGBase::LOGLEVEL_INFO, logbuf);
			fprintf_s(stderr,"%s %s\n",Tool::GetTimeString(m_CheckTime).c_str(),logbuf);
		}
	}

public:
	template<class Txieyi>
		void PushRWDBMsg(Txieyi& xieyi)
	{
		PRWDBMsg pMsg(new ReadWriteDBMessage());
		if ( pMsg )
		{
			MakeRWDBMsg(xieyi,*pMsg);
			pMsg->m_nTimeCount = Tool::GetMilliSecond();
			{
				CSelfLock lock(m_lockMsg);
				m_queueMsg.push(pMsg);

				m_semaphore.Post();

				//int nMsgSize = m_queueMsg.size();
				//int nMemSize = ReadWriteDBMessage::GetTotalCount();
				//cout<<"Push "<<nMsgSize<<" "<<nMemSize<<" "<<m_nFinishCount<<endl;
			}
		}
		else
		{
			cout<<"PushRWDBMsg Error"<<endl;
		}
	}
	PRWDBMsg PopRWDBMsg()
	{
		PRWDBMsg pMsg;
		if ( m_semaphore.Wait(INFINITE) )
		{
			AGBase::CSelfLock l(m_lockMsg);
			pMsg = m_queueMsg.front();
			m_queueMsg.pop();

			//int nMsgSize = m_queueMsg.size();
			//int nMemSize = ReadWriteDBMessage::GetTotalCount();
			//cout<<"Pop  "<<nMsgSize<<" "<<nMemSize<<" "<<m_nFinishCount<<endl;

			m_nFinishCount++;
			INT64 nTicketTime = (Tool::GetMilliSecond() - pMsg->m_nTimeCount);
			if ( nTicketTime > m_MaxTicketCount )
			{
				m_MaxTicketCount = nTicketTime;
			}
			m_TotalWaitTicket += nTicketTime;

			UINT32 nowTime = UINT32(time(NULL));
			if ( (nowTime - m_CheckTime) >= 600 )
			{
				m_CheckTime = nowTime;

				if ( N_CeShiLog::c_RWDBLog )
				{
					//因为只是记日志，所以这里没有对多线程访问的情况进行处理
					DebugLogOut("TotalFinishMsg=%-5d  Speed=%-5d",m_nFinishCount,m_nFinishCount/(max(1,nowTime-m_StartTime)));
					DebugLogOut("MaxWait=%-5s AvaWait=%-5s",N2S(m_MaxTicketCount).c_str(),N2S(m_TotalWaitTicket/max(m_nFinishCount,1)).c_str() );
					DebugLogOut("QueueSize=%-5d TotalBlock=%d UseBlock=%d",m_queueMsg.size(),
						ReadWriteDBMessage::GetTotalCount(),ReadWriteDBMessage::GetUseCount() );
				}	
			}
		}
		return pMsg;
	}

private:
	AGBase::CLock                m_lockMsg;
	AGBase::CCondition           m_condMsg;
	AGBase::CSemaphore           m_semaphore;
	QueueRWDBMsg                 m_queueMsg;

	UINT32               m_StartTime;
	UINT32               m_CheckTime;
	int                  m_nFinishCount;

	INT64                m_MaxTicketCount;
	INT64                m_TotalWaitTicket;
};