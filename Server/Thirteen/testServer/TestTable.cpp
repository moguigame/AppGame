#include ".\testtable.h"

CTestTable::CTestTable(void)
{
	m_TableID           = 0;
	m_TableName         = "";

	m_MaxSitPlayerNumber           = 0;
	m_CurSitPlayerNumber           = 0;
	m_CurStandPlayerNumber         = 0;

	m_nBigBlind            = 0;
	m_nSmallBlind          = 0;
	m_nMaxTakeIn           = 0;
	m_nMinTakeIn           = 0;
	
	for ( int i=0;i<MAX_PALYER_ON_TABLE;i++)
	{
		m_PlayerID[i] = 0;
	}
}

CTestTable::~CTestTable(void)
{

}

void CTestTable::InitTable()
{
	m_CurSitPlayerNumber = 0;
	m_CurStandPlayerNumber = 0;
	for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit )
	{
		m_PlayerID[Sit] = 0;
	}
}

bool CTestTable::IsFull()const
{
	return m_CurSitPlayerNumber>=m_MaxSitPlayerNumber;
}

bool CTestTable::IsAvailSitID(int SitID)
{
	if ( SitID>=0 && SitID<=m_MaxSitPlayerNumber)
	{
		return true;
	}
	return false;
}

int  CTestTable::GetEmptySitID()
{
	int RetSitID = 0;
	for ( int sit=0;sit<m_MaxSitPlayerNumber;sit++)
	{
		if ( m_PlayerID[sit] == 0 )
		{
            RetSitID = sit+1;
			break;
		}
	}
	return RetSitID;
}

void CTestTable::OnPlayerLeave(UINT32 PID)
{
	for ( int Sit=0;Sit<m_MaxSitPlayerNumber;++Sit)
	{
		if ( m_PlayerID[Sit] == PID )
		{
			m_PlayerID[Sit] = 0;
			break;
		}
	}
}
void CTestTable::OnSitPlayerInfo(UINT32 PID,int SitID)
{
	if ( IsAvailSitID(SitID) && m_PlayerID[SitID-1]==0 )
	{
		m_PlayerID[SitID-1] = PID;
	}
	else
	{
		cout<<"OnSitPlayerInfo RRRRRRRRRRRRRRRRRRRRR"<<endl;
	}
}
void CTestTable::OnPlayerSitDown(UINT32 PID,int SitID)
{
	if ( IsAvailSitID(SitID) && m_PlayerID[SitID-1]==0 )
	{
		m_PlayerID[SitID-1] = PID;
	}
	else
	{
		cout<<"OnPlayerSitDown RRRRRRRRRRRRRRRRRRRRR"<<endl;
	}
}
void CTestTable::OnPlayerStandUp(UINT32 PID,int SitID)
{
	if ( IsAvailSitID(SitID) && m_PlayerID[SitID-1]==PID )
	{
		m_PlayerID[SitID-1] = 0;
	}
	else
	{
		cout<<"OnPlayerStandUp RRRRRRRRRRRRRRRRRRRRR"<<endl;
	}
}

void CTestTable::SetTableRule(std::string strRule)
{
	std::map<string,INT64> mapRule = Tool::GetRuleMap(strRule,string(";"),string("="));
	std::map<string,INT64>::iterator itorKey;

	itorKey = mapRule.find("smallblind");
	if ( itorKey != mapRule.end())
	{
		m_nSmallBlind = INT32(itorKey->second);
	}
	else
	{
		cout<<"Init Logic Error m_nSmallBlind"<<endl;
	}

	itorKey = mapRule.find("bigblind");
	if ( itorKey != mapRule.end())
	{
		m_nBigBlind = INT32(itorKey->second);
	}
	else
	{
		cout<<"Init Logic Error m_nBigBlind"<<endl;
	}

	itorKey = mapRule.find("mintake");
	if ( itorKey != mapRule.end() && m_nBigBlind > 0 )
	{
		m_nMinTakeIn = (itorKey->second)*m_nBigBlind;
	}
	else
	{
		cout<<"Init Logic Error m_nMinTakeIn"<<endl;
	}

	itorKey = mapRule.find("maxtake");
	if ( itorKey != mapRule.end() && m_nBigBlind > 0 )
	{
		m_nMaxTakeIn = (itorKey->second)*m_nBigBlind;
	}
	else
	{
		cout<<"Init Logic Error m_nMaxTakeIn"<<endl;
	}

	if ( !(m_nBigBlind>0&&m_nSmallBlind>0&&m_nMinTakeIn>0&&m_nMaxTakeIn>0))
	{
		cout<<"Init Logic Error"<<endl;
	}
}
