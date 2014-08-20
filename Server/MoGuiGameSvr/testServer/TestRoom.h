#pragma once

#include "MoGui.h"
#include "testtable.h"

class CTestRoom
{
public:
	CTestRoom(void);
	~CTestRoom(void);

	CTestTable*    GetTableByID(int TableID);
	CTestTable*    GetEmptyTable();

	int                      m_RoomID;
	std::string              m_RoomName;

	MapTable                 m_mapTable;
};

typedef map<int,CTestRoom*>  MapRoom;
