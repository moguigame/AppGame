#pragma once

#include <string.h>

namespace DezhouLib
{
	typedef unsigned int					UINT;
	typedef unsigned long					DWORD;
	typedef	char							CHAR;
	typedef short							SHORT;
	typedef long							LONG;
	typedef int								BOOL;

	typedef unsigned char                   BYTE;
	typedef unsigned short					WORD;

	typedef signed char                     INT8;
	typedef signed short                    INT16;
	typedef signed int                      INT32;
	typedef signed long long                INT64;

	typedef unsigned char                   UINT8;
	typedef unsigned short                  UINT16;
	typedef unsigned int                    UINT32;
	typedef unsigned long long              UINT64;

	const short    MAX_PALYER_ON_TABLE      = 9;

	const short    HAND_CARDS               = 2;
	const short    TABLE_CARDS              = 5;
	const short    RESULT_CARDS             = 5;
	const short    SELECT_CARDS             = HAND_CARDS+TABLE_CARDS;

	const short    XIPAI_CARDS              = 52;
	const short    XIPAI_POOLS              = 200;

	class CPaiType
	{
	public:
		CPaiType(){	Init();	}
		CPaiType(INT64 PaiVectorValue);

		void    Init();
		void    UpdateValue();
		INT64   GetValue();
		INT64   GetPaiVectorValue();
		BYTE    GetType() const;
		bool    IsEmpty() const;

		BYTE	m_Type;
		INT64   m_PaiTypeValue;
		BYTE	m_Pai[RESULT_CARDS];
		INT64   m_PaiVectorValue;	

	public:
		friend bool operator==(const CPaiType lhs,const CPaiType rhs);
		friend bool operator>(const CPaiType lhs,const CPaiType rhs);
		friend bool operator<(const CPaiType lhs,const CPaiType rhs);
	};

	struct stHandPai
	{
		BYTE            m_Pai[2];
		stHandPai(){ Init();}
		stHandPai(BYTE Lpai,BYTE Rpai)
		{
			m_Pai[0] = Lpai;
			m_Pai[1] = Rpai;
		}
		void Init()	{m_Pai[0] = m_Pai[1] = 0;}
		bool IsEmptyPai() const;
		friend bool operator==(const stHandPai lhs,const stHandPai rhs);
	};
	struct stTablePai
	{
		BYTE            m_TablePai[5];
		stTablePai(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	class CGamePaiLogic
	{
	public:
		static BYTE m_ServerPaiPool[XIPAI_POOLS][XIPAI_CARDS];
		static void InitServerPai(int nCount=XIPAI_POOLS);
		static void MixArrayPai( BYTE *pPai,int nLen,int nTimes );
		static void CheckArrayPai( BYTE *pPai );

	private:
		stHandPai                 m_HandPai[MAX_PALYER_ON_TABLE];
		stTablePai                m_TablePai;

		CPaiType                  m_MaxPaiType[MAX_PALYER_ON_TABLE];
		BYTE                      m_PlayerRight[MAX_PALYER_ON_TABLE];

		bool IsHavePlayer( BYTE sit);

	public:
		CGamePaiLogic()	{ Init();}
		void Init();
		void InitPai();
		const stHandPai&     GetHandPai( BYTE sit);
		const stTablePai&    GetTablePai();
		const CPaiType&      GetPlayerPaiType( BYTE sit);

		void                 SetPlayerRightBySit( BYTE Sit,BYTE rt);
		BYTE                 GetPlayerRight(BYTE Sit);
		BYTE                 GetTablePai(int idx);
		INT64                GetPlayerPaiTypeInNumber( BYTE sit);
		
		int                  GetHandPaiLevel(int sit);
		void                 MakePai(bool bUsed=false);

		int                  GetBlindBeiShu(int Sit);
		int                  GetHandPaiChangce(int Sit);
		bool                 IsSitWin(int Sit);
	};
	extern int GetPaiValue(int nPai);
	extern int GetPaiHuaSe(int nPai);
	extern CPaiType GetMaxPaiType(const BYTE *pPai);

	struct stCreatePlayerGameInfo
	{
		INT16             m_AID;
		UINT32            m_PID;

		INT64             m_nUpperLimite;
		INT64             m_nLowerLimite;

		INT64             m_OrigenMoney;
		INT64             m_LandMoney;

		INT64             m_Right;

		stCreatePlayerGameInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	extern void GetCreatePlayerGameInfo(UINT8 Level,stCreatePlayerGameInfo& stCPGI);

	extern bool IsRightBotLevel(int BotLevel);
	extern INT64 GetBotLevelMoney(int nLevel);

	//class CPlayerRightInfo
	//{
	//public:
	//	enum{ RIGHTCOUNT = 9 };
	//	CPlayerRightInfo();
	//	~CPlayerRightInfo(){}

	//	void InitRight(long long nRight);
	//	void AddRight(int nLevel,int nTimes);
	//	void ReduceRightTime();
	//	
	//	long long GetRight();
	//	int GetRightLevel();
	//	int GetRightTimes();
	//	void UpdateLevelTimes();

	//	unsigned char  m_Right[RIGHTCOUNT];
	//	unsigned char  m_RightLevel;
	//};

	//extern int  GetPlayerRightUseRank(int nRightLevel);

	extern bool IsValidPaiType(int nType);
	
	extern bool IsValidJuBaoPaiType(int nType);

	extern bool IsAllFinish(int nAchieve);
	extern bool IsValidHonorID(int HonorID);
	extern bool IsValidType(int nType);
	extern bool IsValidIdx(int nIdx);
	extern int  GetType(int HonorID);
	extern int  GetIdx(int HonorID);
	extern int  GetHonorID(int nType,int Idx);

	extern bool IsValidPayIncomeFlag(int nFlag);
	extern bool IsPayFlag(int nFlag);
	extern bool IsIncomeFlag(int nFlag);
}

 