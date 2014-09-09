#include "GameLog.h"

using namespace std;

CGameLog::CGameLog(CGameTable* pTable)
{
	if ( pTable )
	{
		m_pTable = pTable;
	}
	else
	{
		printf_s("CGameLog Init Error---------------- \n");
	}
}

CGameLog::~CGameLog()
{

}

void CGameLog::Init()
{
	m_listGameStep.clear();
}

void CGameLog::AddStepString(const string& strStep)
{
	if ( !strStep.empty() )
	{
		m_listGameStep.push_back(strStep);
	}
}

void CGameLog::Print()
{
	m_pTable->ReleaseInfo("本局游戏记录如下：");
	for ( ListGameStep::iterator itorStep=m_listGameStep.begin();itorStep!=m_listGameStep.end();++itorStep)
	{
		m_pTable->ReleaseInfo("%s",(*itorStep).c_str());
	}
}