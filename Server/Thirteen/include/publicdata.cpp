#include "publicdata.h"

namespace MoGui
{
	using namespace std;

	void CMapFunctionTime::EnterFunc(const string& strFuncName)
	{
		MapFuncTime::iterator itorFT = m_mapFuncTime.find(strFuncName);
		if ( itorFT == m_mapFuncTime.end() )
		{
			m_mapFuncTime.insert(make_pair(strFuncName,stFuncTimeLog(strFuncName)));
			itorFT = m_mapFuncTime.find(strFuncName);
		}
		itorFT->second.m_nStartTicket = Tool::GetMicroSecond();
	}
	void CMapFunctionTime::ExitFunc(const string& strFuncName)
	{
		MapFuncTime::iterator itorFT = m_mapFuncTime.find(strFuncName);
		if ( itorFT != m_mapFuncTime.end() )
		{
			int64_t nStartTicket = itorFT->second.m_nStartTicket;
			int64_t nEndTicket   = Tool::GetMicroSecond();
			int64_t nUseTicket   = nEndTicket - nStartTicket;

			if ( nStartTicket>0 && nUseTicket>=0 && nEndTicket >= nStartTicket )
			{
				itorFT->second.m_MaxTicket = std::max<int64_t>(itorFT->second.m_MaxTicket,nUseTicket);
				itorFT->second.m_MinTicket = std::min<int64_t>(itorFT->second.m_MinTicket,nUseTicket);

				itorFT->second.m_nTimes++;
				itorFT->second.m_TotalTicket += nUseTicket;
				itorFT->second.m_nStartTicket = 0;
			}
			else
			{
				fprintf_s(stderr,"%s ExitFunc Error %s nStartTicket=%lld nEndTicket=%lld \n",
					Tool::GetTimeString().c_str(),strFuncName.c_str(), nStartTicket, nEndTicket );
			}
		}
		else
		{
			fprintf_s(stderr,"%s ExitFunc Can't Find Func %s \n",Tool::GetTimeString().c_str(),strFuncName.c_str());
		}
	}

	bool CLogFuncTime::s_bUseLogFuncTime = true;

	CLogFuncTime::CLogFuncTime(CMapFunctionTime& rMFT,const std::string& FuncName)
		: m_rMFT(rMFT), m_FuncName(FuncName)
	{
		if ( s_bUseLogFuncTime )
		{
			m_rMFT.EnterFunc(m_FuncName);
		}
	}
	CLogFuncTime::~CLogFuncTime()
	{
		if ( s_bUseLogFuncTime )
		{
			m_rMFT.ExitFunc(m_FuncName);
		}
	}


	VectorString TraceStackPath::s_vectorPath;
	bool TraceStackPath::s_bUseTrace = true;

	void TraceStackPath::PrintPath()
	{
		if ( s_bUseTrace )
		{
			for ( size_t nSize=0;nSize<s_vectorPath.size();++nSize )
			{
				fprintf_s(stderr,"%s \n",s_vectorPath[nSize].c_str() );
			}
		}
	}

	TraceStackPath::TraceStackPath(const std::string& strEnter)
	{
		if ( s_bUseTrace )
		{
			m_FuncName = strEnter;
			s_vectorPath.push_back(strEnter);
		}
	}
	TraceStackPath::~TraceStackPath()
	{
		if ( s_bUseTrace && s_vectorPath.size() )
		{
			if ( s_vectorPath.back() != m_FuncName )
			{
				PrintPath();
			}
			s_vectorPath.pop_back();
		}
	}
}