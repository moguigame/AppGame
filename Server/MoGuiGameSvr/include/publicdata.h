#pragma once

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdint.h>

#include "tool.h"

namespace MoGui
{
	struct stFuncTimeLog
	{
		std::string                           m_strFuncName;

		long long                             m_TotalTicket;
		long long                             m_nTimes;
		long long                             m_MaxTicket;
		long long                             m_MinTicket;

		long long                             m_nStartTicket;	

		stFuncTimeLog(const std::string& strName = "")
			: m_strFuncName(strName),m_TotalTicket(0),m_nTimes(0),m_MaxTicket(0),m_MinTicket(0),m_nStartTicket(0)
		{
		}
		void Init()
		{
			m_strFuncName = "";
			m_TotalTicket = 0;
			m_nTimes = 0;
			m_MaxTicket = 0;
			m_MinTicket = 0;

			m_nStartTicket = 0;
		}
	};
	typedef std::map<std::string, stFuncTimeLog>          MapFuncTime;
	typedef std::vector<stFuncTimeLog>                       VectorFuncTime;

	class CCompareFuncTime
	{
	public:
		bool operator()(stFuncTimeLog& lSrc,stFuncTimeLog& rSrc)
		{
			return rSrc.m_nTimes < lSrc.m_nTimes;
		}
	};

	class CMapFunctionTime
	{
	public:
		CMapFunctionTime(){	m_mapFuncTime.clear();}
		~CMapFunctionTime(){ m_mapFuncTime.clear();}

		MapFuncTime m_mapFuncTime;

		void EnterFunc(const std::string& strFuncName);
		void ExitFunc(const std::string& strFuncName);
	};

	class CLogFuncTime
	{
	public:
		static bool s_bUseLogFuncTime;

		CMapFunctionTime& m_rMFT;
		std::string       m_FuncName;

		explicit CLogFuncTime(CMapFunctionTime& rMFT,const std::string& FuncName);	
		~CLogFuncTime();
	};

	typedef std::vector<std::string> VectorString;

	class TraceStackPath
	{
	public:
		static bool s_bUseTrace;
		static VectorString s_vectorPath;

		static void PrintPath();

	public:
		TraceStackPath(const std::string& strEnter);
		~TraceStackPath();

	private:
		std::string m_FuncName;
	};
}