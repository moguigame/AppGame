#pragma once

#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "gametable.h"
class CGameTable;

class CGameLog : public boost::noncopyable
{
public:
	explicit CGameLog(CGameTable* pTable);
	~CGameLog();

	typedef  list<string>                  ListGameStep;

private:
	CGameTable*             m_pTable;
	ListGameStep            m_listGameStep;

public:
	void Init();
	void Print();
	void AddStepString(const string& strStep);
};