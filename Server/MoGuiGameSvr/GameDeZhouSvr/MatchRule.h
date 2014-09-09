#pragma once
#include "DezhouData.h"

using namespace MoGui::Game::DeZhou;

//仅仅是是一个数据的集合而已，以免和主逻辑有太多的数据交集
class CMatchRule
{
public:
	CMatchRule(){Init();}
	~CMatchRule(){}

	void Init()
	{
		m_MatchID = 0;
		m_RegisterTime = 0;
		m_IntervalTime = 0;
		m_MatchGUID = 0;

		m_FirstInterval = 0;
		m_SecondInterval = 0;
		m_FirstAwoke = 0;
		m_SecondAwoke = 0;
		m_StartSit = 0;

		m_MatchStartTime = 0;	
		
		m_MatchType = 0;
		m_MatchTicket = 0;
		m_TakeMoney = 0;
		m_StartMoney = 0;

		m_bMatching = false;
		m_bChampTable = false;

		m_MinPlayerForMatch = 2;
		m_TotalPlayer = 0;
		m_LeftPlayer = 0;

		m_AddBlindTime = 0;
		m_BigBlind = 0;
		m_SmallBlind = 0;

		m_vecAwardMoney.clear();
		m_vecAwardJF.clear();
		m_vecBlind.clear();
		m_vecTime.clear();

		m_TotalMatchMoney = 0;
		m_TotalLossMoney = 0;
	}

public:
	int                            m_MatchID;                       //match的识别编号
	UINT32                         m_RegisterTime;                  //指设定开始比赛的时间
	int                            m_IntervalTime;                  //比赛间隔时间
	INT64                          m_MatchGUID;                     //比赛唯一识别码
	std::vector<INT32>             m_vecAwardMoney;                 //每一名应该拿的资金游戏币数量
	std::vector<INT32>             m_vecAwardJF;                    //比赛积分

	short                          m_FirstInterval;                 //两次提醒离游戏开始的间隔
	short                          m_SecondInterval;
	UINT32                         m_FirstAwoke;                    //两次提醒的时间
	UINT32                         m_SecondAwoke;  
	INT8                           m_StartSit;                      //最开始一桌坐几个玩家

	int                            m_MatchType;                     //
	int                            m_MatchTicket;
	int                            m_TakeMoney;
	int                            m_StartMoney;

	UINT32                         m_MatchStartTime;                //比赛开始时间
	bool                           m_bMatching;                     //比赛是否正在进行中
	bool                           m_bChampTable;                   //当前桌是否是冠军桌

	INT16                          m_MinPlayerForMatch;             //指最少开始比赛的玩家数量
	UINT16                         m_TotalPlayer;                   //参赛玩家数
	UINT16                         m_LeftPlayer;                    //余下玩家数

	UINT32                         m_AddBlindTime;                  //下次增加盲注的时间点
	int                            m_BigBlind;                      //当前大盲注和小盲注
	int                            m_SmallBlind;

	std::vector<int>               m_vecBlind;
	std::vector<int>               m_vecTime;

	//主要是用于测试用的，计算平衡
	INT64                          m_TotalMatchMoney;               //比赛的全部游戏币
	INT64                          m_TotalLossMoney;                //比赛过程中因玩家逃走而损失的游戏币

public:
	bool                           IsMatching()   const { return m_bMatching;   }
	int                            GetMatchID()   const { return m_MatchID;     }
	bool                           IsChampTable() const { return m_bChampTable; }
	int                            GetMatchType() const { return m_MatchType;   }
	int                            GetTicket()    const { return m_MatchTicket; }
	int                            GetTakeMoney() const { return m_TakeMoney;   }
	int                            GetStartMoney()const { return m_StartMoney;  }
	int                            GetMatchCost() const { return m_MatchTicket+m_TakeMoney; }

	void ResetMatchData()
	{
		m_bMatching = false;
		m_bChampTable = false;

		m_TotalPlayer = 0;
		m_LeftPlayer = 0;

		m_AddBlindTime = 0;
		m_BigBlind = 0;
		m_SmallBlind = 0;

		m_TotalMatchMoney = 0;
		m_TotalLossMoney = 0;
	}

	INT64  GetAwardGameMoney(int Position)                    //奖金数量
	{
		if ( Position>0 && Position<=int(m_vecAwardMoney.size()) )
		{
			return m_vecAwardMoney[Position-1];
		}
		return 0;
	}
	INT32 GetAwardJF(int Position)
	{
		if ( Position>0 && Position<=int(m_vecAwardJF.size()) )
		{
			return m_vecAwardJF[Position-1];
		}
		return 0;
	}
	void InitRegisterTime(UINT32 curTime)
	{
		while ( m_RegisterTime <= curTime )
		{
			m_RegisterTime += m_IntervalTime;
		}
		m_FirstAwoke = m_RegisterTime - m_FirstInterval;
		m_SecondAwoke = m_RegisterTime - m_SecondInterval;

		assert(m_FirstAwoke>0 && m_FirstInterval>0);
		assert(m_SecondAwoke>0 && m_SecondInterval>0);
	}

	void InitBlind(UINT32 curTime)
	{
		m_BigBlind = m_vecBlind[0];
		m_SmallBlind = m_BigBlind/2;

		m_AddBlindTime = curTime+m_vecTime[0];
	}

	bool AddBlind(UINT32 curTime)
	{
		if ( curTime > m_AddBlindTime )
		{
			for (int Pos=0;Pos<int(m_vecBlind.size());++Pos)
			{
				if ( m_vecBlind[Pos]>=m_BigBlind )
				{
					if ( Pos < int(m_vecBlind.size()-1) )
					{
						m_BigBlind = m_vecBlind[Pos+1];
						m_SmallBlind = m_BigBlind/2;

						m_AddBlindTime += m_vecTime[Pos+1];

						return true;
					}
					break;
				}
			}
		}

		return false;
	}
};


class CTableMatchRule
{
public:
	CTableMatchRule(){ Init(); }
	~CTableMatchRule(){}

	void Init()
	{

	}
};

class CRoomMatchRule
{
public:
	CRoomMatchRule(){ Init(); }
	~CRoomMatchRule(){}

	void Init()
	{

	}
};