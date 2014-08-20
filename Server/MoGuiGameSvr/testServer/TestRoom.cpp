#include ".\testroom.h"

CTestRoom::CTestRoom(void)
{
}

CTestRoom::~CTestRoom(void)
{
}

CTestTable* CTestRoom::GetTableByID(int TableID)
{
	MapTable::iterator itorTable = m_mapTable.find(TableID);
	if ( itorTable != m_mapTable.end() )
	{
		return itorTable->second;
	}
	return NULL;
}

CTestTable* CTestRoom::GetEmptyTable()
{
	for ( MapTable::iterator itorTable=m_mapTable.begin();itorTable!=m_mapTable.end();++itorTable)
	{
		CTestTable* pTable = itorTable->second;
		if ( pTable->GetEmptySitID() )
		{
			return pTable;
		}
	}
	return NULL;
}