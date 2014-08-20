#pragma once

#include "DBServerXY.h"
#include "CDBSConfig.h"
#include "ReadWriteDB.h"

#include "MessageBlock.h"

using namespace MoGui::Game::DeZhou;
using namespace MoGui::MoGuiXY::PublicXY;
using namespace std;

class CDBOperator;
class CServer;

class CDBTest
{
public:
	CDBTest(CServer* pServer)
	{
		m_pServer = pServer;
		m_pDBOperator = new CDBOperator(pServer);
	}
	~CDBTest()
	{
		safe_delete(m_pDBOperator);
	}
	void Test()
	{
		return ;
	}
	CServer*       m_pServer;
	CDBOperator*   m_pDBOperator;
};

/*//测试MEMCACHED

*/

//添加桌子信息
/*
{
	stTableInfo ti;
	for (int i=100;i<101;i++)
	{
		ti.m_ServerID = 99;
		ti.m_RoomID = 10131;
		ti.m_TableID = i+401;
		ti.m_TableName = "高级赛桌" + N2S(i+21);
		ti.m_TableRule = "minplayer=9;";
		ti.m_MatchRule = "matchid=101051;";

		//int BotNum = Tool::CRandom::Random_Int(7,9);
		//int BotTime = Tool::CRandom::Random_Int(3,6);
		//ti.m_TableRuleEX = "botnum="+Tool::N2S(BotNum)+";botchiptime="+Tool::N2S(BotTime)+";";

		m_pDBOperator->WriteTableInfo(ti);
	}
}
*/