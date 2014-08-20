#pragma once

#include "MoGui.h"
#include "MoGuiGame.h"
#include "gamedezhou.h"

class CTestTable
{
public:
	CTestTable(void);
	~CTestTable(void);

	void SetTableRule(std::string strRule);

	void InitTable();
	bool IsAvailSitID(int SitID);
	bool IsFull()const;
	int  GetEmptySitID();

	void OnPlayerLeave(UINT32 PID);
	void OnSitPlayerInfo(UINT32 PID,int SitID);
	void OnPlayerSitDown(UINT32 PID,int SitID);
	void OnPlayerStandUp(UINT32 PID,int SitID);

	int            m_TableID;
	string         m_TableName;

	int            m_PlayerID[MAX_PALYER_ON_TABLE];	
	int      	   m_CurSitPlayerNumber;                            //当前坐下的玩家
	int      	   m_CurStandPlayerNumber;                          //当前旁观的玩家

	int            m_MaxSitPlayerNumber;
	INT32		   m_nBigBlind;                                     //本桌大盲注
	INT32		   m_nSmallBlind;                                   //本桌小盲注
	INT64		   m_nMaxTakeIn;                                    //最大带入量
	INT64		   m_nMinTakeIn;                                    //最小带入量
};

typedef map<int,CTestTable*>  MapTable;
