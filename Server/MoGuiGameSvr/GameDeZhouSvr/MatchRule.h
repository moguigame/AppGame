#pragma once
#include "DezhouData.h"

using namespace MoGui::Game::DeZhou;

//��������һ�����ݵļ��϶��ѣ���������߼���̫������ݽ���
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
	int                            m_MatchID;                       //match��ʶ����
	UINT32                         m_RegisterTime;                  //ָ�趨��ʼ������ʱ��
	int                            m_IntervalTime;                  //�������ʱ��
	INT64                          m_MatchGUID;                     //����Ψһʶ����
	std::vector<INT32>             m_vecAwardMoney;                 //ÿһ��Ӧ���õ��ʽ���Ϸ������
	std::vector<INT32>             m_vecAwardJF;                    //��������

	short                          m_FirstInterval;                 //������������Ϸ��ʼ�ļ��
	short                          m_SecondInterval;
	UINT32                         m_FirstAwoke;                    //�������ѵ�ʱ��
	UINT32                         m_SecondAwoke;  
	INT8                           m_StartSit;                      //�ʼһ�����������

	int                            m_MatchType;                     //
	int                            m_MatchTicket;
	int                            m_TakeMoney;
	int                            m_StartMoney;

	UINT32                         m_MatchStartTime;                //������ʼʱ��
	bool                           m_bMatching;                     //�����Ƿ����ڽ�����
	bool                           m_bChampTable;                   //��ǰ���Ƿ��ǹھ���

	INT16                          m_MinPlayerForMatch;             //ָ���ٿ�ʼ�������������
	UINT16                         m_TotalPlayer;                   //���������
	UINT16                         m_LeftPlayer;                    //���������

	UINT32                         m_AddBlindTime;                  //�´�����äע��ʱ���
	int                            m_BigBlind;                      //��ǰ��äע��Сäע
	int                            m_SmallBlind;

	std::vector<int>               m_vecBlind;
	std::vector<int>               m_vecTime;

	//��Ҫ�����ڲ����õģ�����ƽ��
	INT64                          m_TotalMatchMoney;               //������ȫ����Ϸ��
	INT64                          m_TotalLossMoney;                //������������������߶���ʧ����Ϸ��

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

	INT64  GetAwardGameMoney(int Position)                    //��������
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