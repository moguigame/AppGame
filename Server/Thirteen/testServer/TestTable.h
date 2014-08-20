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
	int      	   m_CurSitPlayerNumber;                            //��ǰ���µ����
	int      	   m_CurStandPlayerNumber;                          //��ǰ�Թ۵����

	int            m_MaxSitPlayerNumber;
	INT32		   m_nBigBlind;                                     //������äע
	INT32		   m_nSmallBlind;                                   //����Сäע
	INT64		   m_nMaxTakeIn;                                    //��������
	INT64		   m_nMinTakeIn;                                    //��С������
};

typedef map<int,CTestTable*>  MapTable;
