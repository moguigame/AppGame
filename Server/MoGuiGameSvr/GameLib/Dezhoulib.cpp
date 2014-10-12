#include "Dezhoulib.h"

#include "Tool.h"

#include <ctime>
#include <iostream>
#include <cassert>


using namespace std;
using namespace Tool;

namespace DezhouLib
{
	enum PlayerRight
	{
		PLAYER_RIGHT_NONE = 0,
		PLAYER_RIGHT_LOW_1,
		PLAYER_RIGHT_LOW_2,
		PLAYER_RIGHT_COMMON,
		PLAYER_RIGHT_HIGH_1,
		PLAYER_RIGHT_HIGH_2,
		PLAYER_RIGHT_HIGH_3,
		PLAYER_RIGHT_HIGH_4,
		PLAYER_RIGHT_KING
	};

	//这个反应RIGHT抢到大小牌的分量
	const int PlayerRightValueChangce[] = 
	{
		0,
		8,
		2,
		0,
		1,
		2,
		4,
		8,
		64
	};

	//这个反应RIGHT使是否分牌的权值分量
	const int PlayerRightMakePaiChangce[] = 
	{
		0,
		12,
		4,
		0,
		2,
		3,
		4,
		6,
		12
	};

	const int MaxMakePaiValue = 12;

	const int c_TopLimit[] = 
	{
		3000000,
		4000000,
		5000000,
		7000000,
		10000000,
		15000000,
		20000000,
		30000000,
		50000000
	};
	const int c_OrigenMoney[] = 
	{
		50000,
		100000,
		150000,
		200000,
		300000,
		500000,
		1000000,
		2000000,
		5000000
	};
	const int c_LandMoney[] = 
	{
		5000,
		10000,
		15000,
		20000,
		30000,
		50000,
		100000,
		200000,
		500000
	};

	const int HandPai_Level[] = {10,5,2,1};

	enum PAIVALUE
	{
		PAI_VALUE_NONE = 0,
		PAI_VALUE_BACK = 55,
	};

	enum
	{
		SORT_DaXiao = 1,            //按大小排序
		SORT_DuoShao,               //按多少排序
		SORT_HuaSe                  //花色排序
	};
	enum
	{
		HuaSe_Fang = 1,              //方块
		HuaSe_Hua,                   //梅花
		HuaSe_Hong,                  //红桃
		HuaSe_Hei                    //黑桃
	};

	enum
	{
		PAI_TYPE_NONE,
		PAI_TYPE_ONE,                               //高牌
		PAI_TYPE_TWO,                               //对子
		PAI_TYPE_TWOTWO,                            //两对
		PAI_TYPE_THREE,                             //三个
		PAI_TYPE_STR,                               //顺子
		PAI_TYPE_HUA,                               //同花
		PAI_TYPE_THREETWO,                          //葫芦
		PAI_TYPE_FOUR,                              //四个
		PAI_TYPE_HUASTR,                            //同花顺
		PAI_TYPE_KING,                              //皇家同花顺
		PAI_TYPE_MAX
	};

	enum
	{
		HandPai_Level_1 = 1,
		HandPai_Level_2 = 2,
		HandPai_Level_3 = 3,
		HandPai_Level_4 = 4
	};

	static int Pai_Level_1[][3] = 
	{
		{VALUE_A,VALUE_A,0},
		{VALUE_K,VALUE_K,0},
		{VALUE_Q,VALUE_Q,0},
		{VALUE_A,VALUE_K,1},
		{VALUE_A,VALUE_Q,1},
		{VALUE_J,VALUE_J,0},
		{VALUE_K,VALUE_Q,1},
		{VALUE_A,VALUE_J,1},
		{VALUE_K,VALUE_J,1},
		{VALUE_A,VALUE_S,1},
		{VALUE_A,VALUE_K,0}
	};

	static int Pai_Level_2[][3] = 
	{
		{VALUE_S,VALUE_S,0},
		{VALUE_Q,VALUE_J,1},
		{VALUE_K,VALUE_S,1},
		{VALUE_Q,VALUE_S,1},
		{VALUE_J,VALUE_S,1},
		{VALUE_9,VALUE_9,0},
		{VALUE_A,VALUE_Q,0},
		{VALUE_A,VALUE_9,1},
		{VALUE_K,VALUE_Q,0},
		{VALUE_S,VALUE_9,1},
		{VALUE_A,VALUE_8,1},
		{VALUE_K,VALUE_9,1},
		{VALUE_J,VALUE_9,1},
		{VALUE_A,VALUE_5,1},
		{VALUE_Q,VALUE_9,1},
		{VALUE_8,VALUE_8,0},
		{VALUE_A,VALUE_J,0},
		{VALUE_A,VALUE_7,1},
		{VALUE_A,VALUE_4,1},
		{VALUE_A,VALUE_6,1},
		{VALUE_A,VALUE_3,1},
		{VALUE_K,VALUE_J,0},
		{VALUE_7,VALUE_7,0},
		{VALUE_S,VALUE_8,1},
		{VALUE_K,VALUE_8,1},
		{VALUE_A,VALUE_S,0}
	};

	static int Pai_Level_3[][3] = 
	{
		{VALUE_A,VALUE_2,1},
		{VALUE_9,VALUE_8,1},
		{VALUE_K,VALUE_7,1},
		{VALUE_Q,VALUE_8,1},
		{VALUE_J,VALUE_8,1},
		{VALUE_K,VALUE_S,0},
		{VALUE_J,VALUE_S,0},
		{VALUE_6,VALUE_6,0},
		{VALUE_Q,VALUE_S,0},
		{VALUE_K,VALUE_6,1},
		{VALUE_8,VALUE_7,1},
		{VALUE_K,VALUE_5,1},
		{VALUE_9,VALUE_7,1},
		{VALUE_S,VALUE_7,1},
		{VALUE_K,VALUE_4,1},
		{VALUE_7,VALUE_6,1},
		{VALUE_5,VALUE_5,0},
		{VALUE_K,VALUE_3,1},
		{VALUE_Q,VALUE_7,1},
		{VALUE_4,VALUE_4,0},
		{VALUE_J,VALUE_7,1},
		{VALUE_3,VALUE_3,0},
		{VALUE_2,VALUE_2,0},
		{VALUE_K,VALUE_2,1},
		{VALUE_8,VALUE_6,1},
		{VALUE_6,VALUE_5,1},
		{VALUE_Q,VALUE_6,1},
		{VALUE_5,VALUE_4,1},
		{VALUE_Q,VALUE_5,1},
		{VALUE_9,VALUE_6,1},
		{VALUE_7,VALUE_5,1},
		{VALUE_Q,VALUE_4,1},
		{VALUE_S,VALUE_9,0},
		{VALUE_A,VALUE_9,0},
		{VALUE_S,VALUE_6,1},
		{VALUE_Q,VALUE_3,1},
		{VALUE_J,VALUE_6,1},
		{VALUE_6,VALUE_4,1},
		{VALUE_Q,VALUE_2,1},
		{VALUE_8,VALUE_5,1},
		{VALUE_K,VALUE_9,0},
		{VALUE_J,VALUE_9,0},
		{VALUE_J,VALUE_5,1},
		{VALUE_5,VALUE_3,1},
		{VALUE_Q,VALUE_9,0},
		{VALUE_A,VALUE_8,0},
		{VALUE_J,VALUE_4,1},
		{VALUE_J,VALUE_3,1},
		{VALUE_7,VALUE_4,1},
		{VALUE_9,VALUE_5,1},
		{VALUE_4,VALUE_3,1},
		{VALUE_J,VALUE_2,1},
		{VALUE_S,VALUE_5,1},
		{VALUE_S,VALUE_4,1},
		{VALUE_6,VALUE_3,1},
		{VALUE_S,VALUE_8,0},
		{VALUE_9,VALUE_8,0},
		{VALUE_A,VALUE_4,0}
	};

	int GetPaiHuaSe( int PaiNum ){
		assert(PaiNum>=1 && PaiNum<=52);
		if( PaiNum>=1 && PaiNum<=52)
		{
			return c_PaiValue[PaiNum];
		}
		return 0;					
	}
	int GetPaiValue( int PaiNum){
		assert(PaiNum >= 1 && PaiNum <= 52);
		if( PaiNum>=1 && PaiNum<=52 )
		{
			return c_PaiValue[PaiNum];
		}
		return 0;
	}

	static void SortPai( BYTE *pPai,BYTE Type=SORT_DaXiao,int nLen=SELECT_CARDS)
	{
		int i,j,k,m,n;
		if( Type == SORT_DaXiao )
		{
			for(i=0;i<nLen-1;i++)
			{
				for(j=i+1;j<nLen;j++)
				{
					if( (c_PaiValue[pPai[i]]<c_PaiValue[pPai[j]])
						|| ( (c_PaiValue[pPai[i]]==c_PaiValue[pPai[j]]) && pPai[i]<pPai[j] ) )
					{
						swap(pPai[i],pPai[j]);						
					}
				}
			}
		}
		else if( Type == SORT_DuoShao )
		{
			BYTE SaveArray[SELECT_CARDS];
			BYTE TempArray[VALUE_MAX];

			memset(SaveArray,0,sizeof(SaveArray));
			memset(TempArray,0,sizeof(TempArray));			

			n = 0;
			for(i=0;i<nLen;i++)
			{
				TempArray[c_PaiValue[pPai[i]]]++;							
			}

			k = 0;
			for(;;)
			{
				j = 1;
				m = 0;
				for(i=VALUE_2;i<VALUE_MAX;i++) //去掉数组中的空牌，值为0
				{
					if( TempArray[i] >= j ) //==用来从大到小排列
					{
						j = TempArray[i];
						m = i;
					}
				}

				if( m>0 )
				{
					TempArray[m] = 0;
					for(i=0;i<nLen;i++)
					{
						if( c_PaiValue[pPai[i]] == m )
						{
							SaveArray[k++] = pPai[i];
						}
					}
				}
				else
				{
					for(i=0;i<nLen;i++)
					{
						pPai[i] = SaveArray[i];
					}
					break;
				}
			}
		}
		else if(  Type == SORT_HuaSe )
		{						
			for(i=0;i<nLen-1;i++)
			{
				for(j=i+1;j<nLen;j++)
				{
					if( c_PaiHuaSe[pPai[i]] < c_PaiHuaSe[pPai[j]]
						|| ( c_PaiHuaSe[pPai[i]] == c_PaiHuaSe[pPai[j]] && c_PaiValue[pPai[i]] < c_PaiValue[pPai[j]] ) )
					{
						swap(pPai[i],pPai[j]);						
					}
				}
			}
		}
	}

	bool stHandPai::IsEmptyPai() const
	{
		return (m_Pai[0] == m_Pai[1]) && (m_Pai[0] == 0);
	}

	void CPaiType::Init()
	{
		memset(this,0,sizeof(*this));
	}
	CPaiType::CPaiType(INT64 PaiVectorValue)
	{
		BYTE* pData = reinterpret_cast<BYTE*>(&PaiVectorValue);
		if ( PaiVectorValue > 0 ){
			assert(pData[0]>0 && pData[1]>0 && pData[2]>0 && pData[3]>0 && pData[4]>0 && pData[5]>0);
			if (pData[0] > 0 && pData[1] > 0 && pData[2] > 0 && pData[3] > 0 && pData[4] > 0 && pData[5] > 0){

				m_PaiVectorValue = PaiVectorValue;

				m_Type = pData[5];
				assert(m_Type > PAI_TYPE_NONE && m_Type < PAI_TYPE_MAX );
				m_Pai[0] = pData[4];
				m_Pai[1] = pData[3];
				m_Pai[2] = pData[2];
				m_Pai[3] = pData[1];
				m_Pai[4] = pData[0];

				m_PaiTypeValue = 0;
				BYTE* pDataValue = reinterpret_cast<BYTE*>(&m_PaiTypeValue);
				pDataValue[5] = m_Type;
				pDataValue[4] = c_PaiValue[m_Pai[0]];
				pDataValue[3] = c_PaiValue[m_Pai[1]];
				pDataValue[2] = c_PaiValue[m_Pai[2]];
				pDataValue[1] = c_PaiValue[m_Pai[3]];
				pDataValue[0] = c_PaiValue[m_Pai[4]];
			}			
		}
	}

	void CPaiType::UpdateValue()
	{
		m_PaiTypeValue = 0;
		BYTE* pPai = reinterpret_cast<BYTE*>(&m_PaiTypeValue);
		pPai[5] = m_Type;
		pPai[4] = m_Pai[0];
		pPai[3] = m_Pai[1];
		pPai[2] = m_Pai[2];
		pPai[1] = m_Pai[3];
		pPai[0] = m_Pai[4];

		m_PaiVectorValue = 0;
		BYTE* pPaiValue = reinterpret_cast<BYTE*>(&m_PaiVectorValue);
		pPaiValue[5] = m_Type;
		pPaiValue[4] = c_PaiValue[m_Pai[0]];
		pPaiValue[3] = c_PaiValue[m_Pai[1]];
		pPaiValue[2] = c_PaiValue[m_Pai[2]];
		pPaiValue[1] = c_PaiValue[m_Pai[3]];
		pPaiValue[0] = c_PaiValue[m_Pai[4]];
	}

	INT64 CPaiType::GetValue()
	{
		if ( m_PaiTypeValue == 0 )
		{
			UpdateValue();
		}
		return m_PaiTypeValue;
	}
	INT64 CPaiType::GetPaiVectorValue()
	{
		if ( m_PaiVectorValue == 0 )
		{
			UpdateValue();
		}
		return m_PaiVectorValue;
	}
	BYTE CPaiType::GetType() const
	{
		return m_Type;
	}
	bool CPaiType::IsEmpty()  const
	{
		return m_PaiTypeValue == 0;
	}

	bool operator==(const CPaiType lhs,const CPaiType rhs)
	{
		return lhs.m_PaiTypeValue == rhs.m_PaiTypeValue;
	}
	bool operator>(const CPaiType lhs,const CPaiType rhs)
	{
		return lhs.m_PaiTypeValue > rhs.m_PaiTypeValue;
	}
	bool operator<(const CPaiType lhs,const CPaiType rhs)
	{
		return lhs.m_PaiTypeValue < rhs.m_PaiTypeValue;
	}

	bool operator==(const stHandPai lhs,const stHandPai rhs)
	{
		return lhs.m_Pai[0] == rhs.m_Pai[0] && lhs.m_Pai[1] == rhs.m_Pai[1];
	}

	struct CAllPaiType
	{
	public:		
		BYTE m_Type[PAI_TYPE_MAX];                      //统计可能牌型
		BYTE m_Pai[PAI_TYPE_MAX][RESULT_CARDS];         //牌型对应的值
	public:
		CAllPaiType()
		{
			Init();
		}
		void Init()
		{
			memset(this,0,sizeof(*this));
		}
		CAllPaiType(const CAllPaiType& src)
		{
			Copy(src);
		}
		CAllPaiType& operator=(const CAllPaiType& rhs)
		{
			if( this != &rhs )
			{
				Copy(rhs);
			}
			return (*this);
		}
		void Copy(const CAllPaiType& rhs)
		{			
			memcpy(m_Type,rhs.m_Type,sizeof(m_Type));
			memcpy(m_Pai,rhs.m_Pai,sizeof(m_Pai));
		}

		CPaiType GetMaxPaiType()
		{
			CPaiType pt;
			for (int i=PAI_TYPE_KING;i>=PAI_TYPE_ONE;i--)
			{
				if ( m_Type[i] == 1 )
				{
					pt.m_Type = BYTE(i);
					for (int j=0;j<RESULT_CARDS;j++)
					{
						pt.m_Pai[j] = m_Pai[i][j];
					}
					pt.UpdateValue();
					break;
				}
			}
			return pt;
		}
	};

	CAllPaiType GetAllPaiType(const BYTE *pPai)
	{
		CAllPaiType retPaiType;

		int i,j;
		BYTE HuaSe;
		int MaxType = PAI_TYPE_NONE;

		BYTE OriginPai[SELECT_CARDS];
		BYTE TempPaiA[SELECT_CARDS];
		memcpy(OriginPai,pPai,sizeof(OriginPai));
		SortPai(OriginPai);
		memcpy(TempPaiA,OriginPai,sizeof(TempPaiA));

		//首先看有没有同花的情况
		HuaSe = 0;
		SortPai(TempPaiA,SORT_HuaSe);
		for(i=0;i<3;i++)
		{
			if( c_PaiHuaSe[TempPaiA[i]]>0
				&& (c_PaiHuaSe[TempPaiA[i]] == c_PaiHuaSe[TempPaiA[i+4]]) )
			{
				MaxType = PAI_TYPE_HUA;
				retPaiType.m_Type[PAI_TYPE_HUA] = 1;
				HuaSe = c_PaiHuaSe[TempPaiA[i]];
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_HUA][j] = TempPaiA[i+j];
				}
				break;
			}
		}
		if( HuaSe > 0 )//如果是同花判断是否是同花顺
		{
			for(i=0;i<SELECT_CARDS;i++)//去掉非同花的牌,并将同花按值排序
			{
				if( c_PaiHuaSe[TempPaiA[i]] != HuaSe )
				{
					TempPaiA[i] = 0;
				}
			}
			SortPai(TempPaiA); //此行因为是同花排序
			for (i=0;i<3;i++)
			{
				if( c_PaiValue[TempPaiA[i]]>0 && c_PaiValue[TempPaiA[i+4]]>0 
					&& (c_PaiValue[TempPaiA[i]]-c_PaiValue[TempPaiA[i+4]]==4) )
				{
					MaxType = PAI_TYPE_HUASTR;
					retPaiType.m_Type[PAI_TYPE_HUASTR] = 1;
					for(j=0;j<5;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_HUASTR][j] = TempPaiA[i+j];
					}
					break;
				}
			}			
			//是否皇家同花顺
			if( retPaiType.m_Type[PAI_TYPE_HUASTR] == 1
				&& c_PaiValue[TempPaiA[0]] == VALUE_A && c_PaiValue[TempPaiA[4]] == VALUE_S )
			{
				MaxType = PAI_TYPE_KING;
				retPaiType.m_Type[PAI_TYPE_KING] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_KING][j] = TempPaiA[j];
				}
			}
			//是否带12345的顺子
			if( retPaiType.m_Type[PAI_TYPE_HUASTR] != 1	&& c_PaiValue[TempPaiA[0]] == VALUE_A )
			{
				int TempCharPai = TempPaiA[0];
				TempPaiA[0] = 0;
				for(i=0;i<SELECT_CARDS;i++)//去掉多余的情况
				{
					if( c_PaiValue[TempPaiA[i]] > 5 )
					{
						TempPaiA[i] = 0;
					}
				}
				SortPai(TempPaiA);
				if( c_PaiValue[TempPaiA[0]] == VALUE_5 && c_PaiValue[TempPaiA[3]] == VALUE_2 )
				{
					MaxType = PAI_TYPE_HUASTR;
					retPaiType.m_Type[PAI_TYPE_HUASTR] = 1;								
					for(j=0;j<4;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_HUASTR][j] = TempPaiA[j];
					}
					retPaiType.m_Pai[PAI_TYPE_HUASTR][4] = TempCharPai;
				}
			}
		}

		memcpy(TempPaiA,OriginPai,sizeof(TempPaiA));
		SortPai(TempPaiA,SORT_DuoShao);

		if( c_PaiValue[TempPaiA[0]] == c_PaiValue[TempPaiA[3]] )
		{
			MaxType = PAI_TYPE_FOUR;
			retPaiType.m_Type[PAI_TYPE_FOUR] = 1;
			for(j=0;j<4;j++)
			{
				retPaiType.m_Pai[PAI_TYPE_FOUR][j] = TempPaiA[j];
			}

			char TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[5]]?TempPaiA[4]:TempPaiA[5];
			TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[6]]?TempPaiA[4]:TempPaiA[6];
			retPaiType.m_Pai[PAI_TYPE_FOUR][4] = TempBigChar;
		}
		else if ( c_PaiValue[TempPaiA[0]] == c_PaiValue[TempPaiA[2]] )
		{
			if ( c_PaiValue[TempPaiA[3]] == c_PaiValue[TempPaiA[4]] )
			{
				MaxType = PAI_TYPE_THREETWO;
				retPaiType.m_Type[PAI_TYPE_THREETWO] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_THREETWO][j] = TempPaiA[j];
				}
			}
			else
			{
				MaxType = PAI_TYPE_THREE;
				retPaiType.m_Type[PAI_TYPE_THREE] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_THREE][j] = TempPaiA[j];
				}
			}
		}
		else if ( c_PaiValue[TempPaiA[0]] == c_PaiValue[TempPaiA[1]] )
		{
			if ( c_PaiValue[TempPaiA[2]] == c_PaiValue[TempPaiA[3]] )
			{
				MaxType = PAI_TYPE_TWOTWO;
				retPaiType.m_Type[PAI_TYPE_TWOTWO] = 1;
				for(j=0;j<4;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_TWOTWO][j] = TempPaiA[j];
				}

				char TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[5]]?TempPaiA[4]:TempPaiA[5];
				TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[6]]?TempPaiA[4]:TempPaiA[6];
				retPaiType.m_Pai[PAI_TYPE_TWOTWO][4] = TempBigChar;						
			}
			else
			{
				MaxType = PAI_TYPE_TWO;
				retPaiType.m_Type[PAI_TYPE_TWO] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_TWO][j] = TempPaiA[j];
				}
			}
		}

		if ( MaxType < PAI_TYPE_STR )
		{
			//是否顺
			for(i=0;i<SELECT_CARDS-1;i++)
			{
				for(j=i+1;j<SELECT_CARDS;j++)
				{
					if( c_PaiValue[TempPaiA[i]] == c_PaiValue[TempPaiA[j]] )
					{
						TempPaiA[j] = 0;
					}
				}
			}
			SortPai(TempPaiA);
			for(i=0;i<3;i++)
			{
				if( c_PaiValue[TempPaiA[i]]>0 && c_PaiValue[TempPaiA[i+4]]>0 
					&& (c_PaiValue[TempPaiA[i]] - c_PaiValue[TempPaiA[i+4]]) == 4 )
				{
					MaxType = PAI_TYPE_STR;
					retPaiType.m_Type[PAI_TYPE_STR] = 1;
					for(j=0;j<5;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_STR][j] = TempPaiA[i+j];
					}
					break;
				}
			}
			//是否A2345的顺子
			if( retPaiType.m_Type[PAI_TYPE_STR] != 1 && c_PaiValue[TempPaiA[0]] == VALUE_A )
			{
				char TempCharPai = TempPaiA[0];
				TempPaiA[0] = 0;
				for(i=0;i<SELECT_CARDS;i++)//去掉多余的情况
				{
					if( c_PaiValue[TempPaiA[i]] > 5 )
					{
						TempPaiA[i] = 0;
					}
				}
				SortPai(TempPaiA);
				if( c_PaiValue[TempPaiA[0]] == VALUE_5 && c_PaiValue[TempPaiA[3]] == VALUE_2 )
				{
					MaxType = PAI_TYPE_STR;
					retPaiType.m_Type[PAI_TYPE_STR] = 1;
					for(j=0;j<4;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_STR][j] = TempPaiA[j];
					}
					retPaiType.m_Pai[PAI_TYPE_STR][4] = TempCharPai;
				}
			}
		}

		if( MaxType == PAI_TYPE_NONE )
		{
			memcpy(TempPaiA,OriginPai,sizeof(TempPaiA));
			SortPai(TempPaiA);

			MaxType = PAI_TYPE_ONE;
			retPaiType.m_Type[PAI_TYPE_ONE] = 1;
			for(j=0;j<5;j++)
			{
				retPaiType.m_Pai[PAI_TYPE_ONE][j] = TempPaiA[j];
			}
		}
		return retPaiType;
	}

	CPaiType GetMaxPaiType(const BYTE *pPai)
	{
		CAllPaiType retPaiType;

		int nPaiCount = 0;
		for ( int i=0;i<SELECT_CARDS;i++){
			if ( pPai[i]>0 && pPai[i]<=52 ){
				nPaiCount++;
			}
		}
		if ( nPaiCount < 5 ){
			return CPaiType();
		}

		int i,j;
		BYTE HuaSe;
		int MaxType = PAI_TYPE_NONE;

		BYTE OriginPai[SELECT_CARDS];
		BYTE TempPaiA[SELECT_CARDS];
		memcpy(OriginPai,pPai,sizeof(OriginPai));
		SortPai(OriginPai);
		memcpy(TempPaiA,OriginPai,sizeof(TempPaiA));

		//首先看有没有同花的情况
		HuaSe = 0;
		SortPai(TempPaiA,SORT_HuaSe);
		for(i=0;i<3;i++)
		{
			if( c_PaiHuaSe[TempPaiA[i]]>0
				&& (c_PaiHuaSe[TempPaiA[i]] == c_PaiHuaSe[TempPaiA[i+4]]) )
			{
				MaxType = PAI_TYPE_HUA;
				retPaiType.m_Type[PAI_TYPE_HUA] = 1;
				HuaSe = c_PaiHuaSe[TempPaiA[i]];
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_HUA][j] = TempPaiA[i+j];
				}
				break;
			}
		}
		if( HuaSe > 0 )//如果是同花判断是否是同花顺
		{
			for(i=0;i<SELECT_CARDS;i++)//去掉非同花的牌,并将同花按值排序
			{
				if( c_PaiHuaSe[TempPaiA[i]] != HuaSe )
				{
					TempPaiA[i] = 0;
				}
			}
			SortPai(TempPaiA); //此行因为是同花排序
			for (i=0;i<3;i++)
			{
				if( c_PaiValue[TempPaiA[i]]>0 && c_PaiValue[TempPaiA[i+4]]>0 
					&& (c_PaiValue[TempPaiA[i]]-c_PaiValue[TempPaiA[i+4]]==4) )
				{
					MaxType = PAI_TYPE_HUASTR;
					retPaiType.m_Type[PAI_TYPE_HUASTR] = 1;
					for(j=0;j<5;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_HUASTR][j] = TempPaiA[i+j];
					}
					break;
				}
			}			
			//是否皇家同花顺
			if( retPaiType.m_Type[PAI_TYPE_HUASTR] == 1
				&& c_PaiValue[TempPaiA[0]] == VALUE_A && c_PaiValue[TempPaiA[4]] == VALUE_S )
			{
				MaxType = PAI_TYPE_KING;
				retPaiType.m_Type[PAI_TYPE_KING] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_KING][j] = TempPaiA[j];
				}
			}
			//是否带12345的顺子
			if( retPaiType.m_Type[PAI_TYPE_HUASTR] != 1	&& c_PaiValue[TempPaiA[0]] == VALUE_A )
			{
				int TempCharPai = TempPaiA[0];
				TempPaiA[0] = 0;
				for(i=0;i<SELECT_CARDS;i++)//去掉多余的情况
				{
					if( c_PaiValue[TempPaiA[i]] > 5 )
					{
						TempPaiA[i] = 0;
					}
				}
				SortPai(TempPaiA);
				if( c_PaiValue[TempPaiA[0]] == VALUE_5 && c_PaiValue[TempPaiA[3]] == VALUE_2 )
				{
					MaxType = PAI_TYPE_HUASTR;
					retPaiType.m_Type[PAI_TYPE_HUASTR] = 1;								
					for(j=0;j<4;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_HUASTR][j] = TempPaiA[j];
					}
					retPaiType.m_Pai[PAI_TYPE_HUASTR][4] = TempCharPai;
				}
			}
			return retPaiType.GetMaxPaiType();
		}

		memcpy(TempPaiA,OriginPai,sizeof(TempPaiA));
		SortPai(TempPaiA,SORT_DuoShao);

		if( c_PaiValue[TempPaiA[0]] == c_PaiValue[TempPaiA[3]] )
		{
			MaxType = PAI_TYPE_FOUR;
			retPaiType.m_Type[PAI_TYPE_FOUR] = 1;
			for(j=0;j<4;j++)
			{
				retPaiType.m_Pai[PAI_TYPE_FOUR][j] = TempPaiA[j];
			}

			char TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[5]]?TempPaiA[4]:TempPaiA[5];
			TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[6]]?TempPaiA[4]:TempPaiA[6];
			retPaiType.m_Pai[PAI_TYPE_FOUR][4] = TempBigChar;
		}
		else if ( c_PaiValue[TempPaiA[0]] == c_PaiValue[TempPaiA[2]] )
		{
			if ( c_PaiValue[TempPaiA[3]] == c_PaiValue[TempPaiA[4]] )
			{
				MaxType = PAI_TYPE_THREETWO;
				retPaiType.m_Type[PAI_TYPE_THREETWO] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_THREETWO][j] = TempPaiA[j];
				}
			}
			else
			{
				MaxType = PAI_TYPE_THREE;
				retPaiType.m_Type[PAI_TYPE_THREE] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_THREE][j] = TempPaiA[j];
				}
			}

		}
		else if ( c_PaiValue[TempPaiA[0]] == c_PaiValue[TempPaiA[1]] )
		{
			if ( c_PaiValue[TempPaiA[2]] == c_PaiValue[TempPaiA[3]] )
			{
				MaxType = PAI_TYPE_TWOTWO;
				retPaiType.m_Type[PAI_TYPE_TWOTWO] = 1;
				for(j=0;j<4;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_TWOTWO][j] = TempPaiA[j];
				}

				char TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[5]]?TempPaiA[4]:TempPaiA[5];
				TempBigChar = c_PaiValue[TempPaiA[4]]>=c_PaiValue[TempPaiA[6]]?TempPaiA[4]:TempPaiA[6];
				retPaiType.m_Pai[PAI_TYPE_TWOTWO][4] = TempBigChar;						
			}
			else
			{
				MaxType = PAI_TYPE_TWO;
				retPaiType.m_Type[PAI_TYPE_TWO] = 1;
				for(j=0;j<5;j++)
				{
					retPaiType.m_Pai[PAI_TYPE_TWO][j] = TempPaiA[j];
				}
			}
		}

		if ( MaxType < PAI_TYPE_STR )
		{
			//是否顺
			for(i=0;i<SELECT_CARDS-1;i++)
			{
				for(j=i+1;j<SELECT_CARDS;j++)
				{
					if( c_PaiValue[TempPaiA[i]] == c_PaiValue[TempPaiA[j]] )
					{
						TempPaiA[j] = 0;
					}
				}
			}
			SortPai(TempPaiA);
			for(i=0;i<3;i++)
			{
				if( c_PaiValue[TempPaiA[i]]>0 && c_PaiValue[TempPaiA[i+4]]>0 
					&& (c_PaiValue[TempPaiA[i]] - c_PaiValue[TempPaiA[i+4]]) == 4 )
				{
					MaxType = PAI_TYPE_STR;
					retPaiType.m_Type[PAI_TYPE_STR] = 1;
					for(j=0;j<5;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_STR][j] = TempPaiA[i+j];
					}
					break;
				}
			}
			//是否A2345的顺子
			if( retPaiType.m_Type[PAI_TYPE_STR] != 1 && c_PaiValue[TempPaiA[0]] == VALUE_A )
			{
				char TempCharPai = TempPaiA[0];
				TempPaiA[0] = 0;
				for(i=0;i<SELECT_CARDS;i++)//去掉多余的情况
				{
					if( c_PaiValue[TempPaiA[i]] > 5 )
					{
						TempPaiA[i] = 0;
					}
				}
				SortPai(TempPaiA);
				if( c_PaiValue[TempPaiA[0]] == VALUE_5 && c_PaiValue[TempPaiA[3]] == VALUE_2 )
				{
					MaxType = PAI_TYPE_STR;
					retPaiType.m_Type[PAI_TYPE_STR] = 1;
					for(j=0;j<4;j++)
					{
						retPaiType.m_Pai[PAI_TYPE_STR][j] = TempPaiA[j];
					}
					retPaiType.m_Pai[PAI_TYPE_STR][4] = TempCharPai;
				}
			}
		}
		if( MaxType == PAI_TYPE_NONE )
		{
			memcpy(TempPaiA,OriginPai,sizeof(TempPaiA));
			SortPai(TempPaiA);

			MaxType = PAI_TYPE_ONE;
			retPaiType.m_Type[PAI_TYPE_ONE] = 1;
			for(j=0;j<5;j++)
			{
				retPaiType.m_Pai[PAI_TYPE_ONE][j] = TempPaiA[j];
			}
		}
		return retPaiType.GetMaxPaiType();
	}

	static CPaiType GetMaxPaiType(stHandPai hp,stTablePai tp)
	{
		int i;
		BYTE TestPai[SELECT_CARDS];

		memset(TestPai,0,sizeof(TestPai));
		for (i=0;i<HAND_CARDS;i++)
		{
			TestPai[i] = hp.m_Pai[i];
		}
		for (i=0;i<TABLE_CARDS;i++)
		{
			TestPai[i+2] = tp.m_TablePai[i];
		}		

		return GetMaxPaiType(TestPai);
	}

	void CGamePaiLogic::InitServerPai(int nCount)
	{
		CRandom::InitRandSeed();

		int i,j;
		for(i=0;i<XIPAI_POOLS;i++)
		{
			for(j=0;j<XIPAI_CARDS;j++)
			{
				m_ServerPaiPool[i][j] = j+1;
			}
		}
		for(i=0;i<XIPAI_POOLS;i++)
		{
			MixArrayPai( m_ServerPaiPool[i], XIPAI_CARDS, 1000 );
			CheckArrayPai(m_ServerPaiPool[i]);
		}
	}
	void CGamePaiLogic::MixArrayPai( BYTE *pPai,int nLen,int nTimes )
	{
		int i,j;
		int k,m;

		for(i=0;i<nTimes;i++)
		{
			k = nLen;
			for( j=nLen-1;j>0;j--)
			{
				m = CRandom::Random_Int(0,k-1);
				swap(pPai[k-1],pPai[m]);
				k--;
			}
		}
	}
	void CGamePaiLogic::CheckArrayPai( BYTE *pPai )
	{
		int i;
		int Sum[XIPAI_CARDS] = {0};

		for(i=0;i<XIPAI_CARDS;i++)
		{
			if( pPai[i] >0 && pPai[i] <= XIPAI_CARDS )
			{
				Sum[pPai[i]-1] = 1;
			}
		}
		for(i=0;i<XIPAI_CARDS;i++)
		{
			if( Sum[i] == 0 )
			{
				fprintf_s(stderr,"Error CheckArrayPai \n");
				assert(0);				
			}
		}
	}
	BYTE CGamePaiLogic::m_ServerPaiPool[XIPAI_POOLS][XIPAI_CARDS] = {0};

	void CGamePaiLogic::Init()
	{
		m_TablePai.Init();
		for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
		{
			m_HandPai[i].Init();
			m_MaxPaiType[i].Init();
			m_PlayerRight[i] = PLAYER_RIGHT_NONE;
		}
	}
	void CGamePaiLogic::InitPai()
	{
		m_TablePai.Init();
		for (int i=0;i<MAX_PALYER_ON_TABLE;i++)
		{
			m_HandPai[i].Init();
			m_MaxPaiType[i].Init();
		}
	}

	const stHandPai& CGamePaiLogic::GetHandPai( BYTE sit)
	{
		assert(IsHavePlayer(sit));
		return m_HandPai[sit];
	}
	const stTablePai& CGamePaiLogic::GetTablePai()
	{
		return m_TablePai;
	}
	bool CGamePaiLogic::IsHavePlayer( BYTE sit)
	{
		assert( sit>=0 && sit<MAX_PALYER_ON_TABLE );
		return ( m_PlayerRight[sit] > PLAYER_RIGHT_NONE && m_PlayerRight[sit] <= PLAYER_RIGHT_KING );
	}

	void CGamePaiLogic::SetPlayerRightBySit( BYTE Sit,BYTE rt)
	{
		assert( Sit>=0 && Sit<MAX_PALYER_ON_TABLE );
		assert( rt>=PLAYER_RIGHT_NONE && rt<=PLAYER_RIGHT_KING );

		if ( Sit>=0 && Sit<MAX_PALYER_ON_TABLE)
		{
			m_PlayerRight[Sit] = rt;
		}	
	}
	BYTE CGamePaiLogic::GetPlayerRight(BYTE Sit)
	{
		assert( Sit>=0 && Sit<MAX_PALYER_ON_TABLE );
		if ( Sit>=0 && Sit<MAX_PALYER_ON_TABLE)
		{
			return m_PlayerRight[Sit];
		}
		return PAI_TYPE_NONE;
	}
	BYTE CGamePaiLogic::GetTablePai(int idx)
	{
		assert(idx>=0&&idx<TABLE_CARDS);
		return m_TablePai.m_TablePai[idx];
	}
	INT64 CGamePaiLogic::GetPlayerPaiTypeInNumber( BYTE sit)
	{
		assert(IsHavePlayer(sit));
		return m_MaxPaiType[sit].m_PaiTypeValue;		
	}
	const CPaiType& CGamePaiLogic::GetPlayerPaiType( BYTE sit)
	{
		assert(IsHavePlayer(sit));
		return m_MaxPaiType[sit];
	}

	int CGamePaiLogic::GetHandPaiLevel(int sit)
	{
		int TongHua = (c_PaiHuaSe[m_HandPai[sit].m_Pai[0]]==c_PaiHuaSe[m_HandPai[sit].m_Pai[1]])?1:0;
		int LeftPaiValue  = max(c_PaiValue[m_HandPai[sit].m_Pai[0]],c_PaiValue[m_HandPai[sit].m_Pai[1]]);
		int RightPaiValue = min(c_PaiValue[m_HandPai[sit].m_Pai[0]],c_PaiValue[m_HandPai[sit].m_Pai[1]]);

		int nCount = int(sizeof(Pai_Level_1)/3);
		for ( int nIdx=0;nIdx<nCount;nIdx++)
		{
			if ( Pai_Level_1[nIdx][0]==LeftPaiValue
				&& Pai_Level_1[nIdx][1]==RightPaiValue
				&& TongHua>=Pai_Level_1[nIdx][2] )
			{
				return HandPai_Level_1;
			}
		}

		nCount = int(sizeof(Pai_Level_2)/3);
		for ( int nIdx=0;nIdx<nCount;nIdx++)
		{
			if ( Pai_Level_2[nIdx][0]==LeftPaiValue
				&& Pai_Level_2[nIdx][1]==RightPaiValue
				&& TongHua>=Pai_Level_2[nIdx][2] )
			{
				return HandPai_Level_2;
			}
		}

		nCount = int(sizeof(Pai_Level_3)/3);
		for ( int nIdx=0;nIdx<nCount;nIdx++)
		{
			if ( Pai_Level_3[nIdx][0]==LeftPaiValue
				&& Pai_Level_3[nIdx][1]==RightPaiValue
				&& TongHua>=Pai_Level_3[nIdx][2] )
			{
				return HandPai_Level_3;
			}
		}
		return HandPai_Level_4;
	}

	bool IsLossRight(BYTE PR)
	{
		return PR>PLAYER_RIGHT_NONE && PR<PLAYER_RIGHT_COMMON;
	}
	bool IsWinRight(BYTE PR)
	{
		return PR>PLAYER_RIGHT_COMMON && PR<=PLAYER_RIGHT_KING;
	}
	bool IsCommonRight(BYTE PR)
	{
		return PR==PLAYER_RIGHT_COMMON;
	}
	bool IsNoneRight(BYTE PR)
	{
		return PR==PLAYER_RIGHT_NONE;
	}

	bool CGamePaiLogic::IsSitWin(int Sit)
	{
		if ( Sit>=0 && Sit<MAX_PALYER_ON_TABLE && IsHavePlayer(Sit) )
		{
			INT64 MaxPai = 0;
			for ( int i=0;i<MAX_PALYER_ON_TABLE;i++)
			{
				if ( IsHavePlayer(Sit) && m_MaxPaiType[i].GetValue() > MaxPai )
				{
					MaxPai = m_MaxPaiType[i].GetValue();
				}
			}
			if ( m_MaxPaiType[Sit].GetValue() == MaxPai )
			{
				return true;
			}
		}
		return false;
	}

	void CGamePaiLogic::MakePai(bool bUseRight)
	{
		int RandK = CRandom::Random_Int(0,XIPAI_POOLS-1);
		MixArrayPai(m_ServerPaiPool[RandK],XIPAI_CARDS,5);

		for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
		{
			m_MaxPaiType[Sit].Init();
		}
		for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
		{
			BYTE PaiL = m_ServerPaiPool[RandK][Sit*2];
			BYTE PaiR = m_ServerPaiPool[RandK][Sit*2+1];

			if ( c_PaiValue[PaiL] > c_PaiValue[PaiR] 
				|| (c_PaiValue[PaiL]==c_PaiValue[PaiR] && PaiL>PaiR) )
			{
				m_HandPai[Sit].m_Pai[0] = PaiL;
				m_HandPai[Sit].m_Pai[1] = PaiR;
			}
			else
			{
				m_HandPai[Sit].m_Pai[0] = PaiR;
				m_HandPai[Sit].m_Pai[1] = PaiL;
			}
		}
		for(int PaiIndex=0;PaiIndex<TABLE_CARDS;PaiIndex++)
		{
			m_TablePai.m_TablePai[PaiIndex] = m_ServerPaiPool[RandK][MAX_PALYER_ON_TABLE*2+PaiIndex];
		}
		int PlayerCount = 0;
		for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
		{
			if ( IsHavePlayer(Sit) )
			{
				PlayerCount++;
				m_MaxPaiType[Sit] = GetMaxPaiType(m_HandPai[Sit],m_TablePai);
			}
			else 
			{  
				//将没有玩家的牌值全清掉减少后续计算量
				m_MaxPaiType[Sit].Init();
				m_HandPai[Sit].Init();
			}
		}
		//确定是否需要对大牌或者小牌进行选择		
		bool bMaxRight=false,bMinRight=false;
		if ( bUseRight )
		{
			int MinRight = PLAYER_RIGHT_COMMON;
			int MaxRight = PLAYER_RIGHT_COMMON;
			int TotalLossRight = 0;
			int TotalWinRight = 0;
			for (int SitIndex=0;SitIndex<MAX_PALYER_ON_TABLE;SitIndex++)
			{
				BYTE nRight = m_PlayerRight[SitIndex];
				if ( IsHavePlayer(SitIndex) && (!IsCommonRight(nRight)) )
				{
					if ( IsLossRight(nRight) )
					{
						TotalLossRight += PlayerRightMakePaiChangce[nRight];
						if ( nRight<MinRight ){ MinRight = nRight; };		
					}
					else if ( IsWinRight(nRight) && nRight>MaxRight )
					{
						TotalWinRight += PlayerRightMakePaiChangce[nRight];
						if ( nRight>MaxRight ){ MaxRight = nRight;}	
					}
				}
			}
			if ( IsLossRight(MinRight) && CRandom::GetChangce(MaxMakePaiValue,min(MaxMakePaiValue,TotalLossRight)) )
			{
				bMinRight = true;
			}
			if ( IsWinRight(MaxRight) && CRandom::GetChangce(MaxMakePaiValue,min(MaxMakePaiValue,TotalWinRight)) )
			{
				bMaxRight = true;
			}
		}
		int MaxPlayerSit = -1;
		if ( bMaxRight ) //选出哪个坐位号的玩家将获得最大牌
		{			
			int TotalChangce = 0;
			for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit)
			{
				int nRight = m_PlayerRight[Sit];
				if ( IsWinRight(nRight) )
				{
					TotalChangce += PlayerRightValueChangce[nRight];
				}
			}
			int LuckValue = CRandom::Random_Int(1,max(1,TotalChangce));
			int CurValue = 0;
			for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit)
			{
				int nRight = m_PlayerRight[Sit];
				if ( IsWinRight(nRight) )
				{
					CurValue += PlayerRightValueChangce[nRight];
					if ( LuckValue <= CurValue )
					{
						MaxPlayerSit = Sit;
						break;
					}
				}
			}
			assert(MaxPlayerSit!=-1);
		}
		int MinPlayerSit = -1;
		if ( bMinRight ) //选出哪个坐位号的玩家将获得最小牌
		{
			int TotalChangce = 0;
			for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit)
			{
				int nRight = m_PlayerRight[Sit];
				if ( IsLossRight(nRight) )
				{
					TotalChangce += PlayerRightValueChangce[nRight];
				}
			}
			int LuckValue = CRandom::Random_Int(1,max(1,TotalChangce));
			int CurValue = 0;
			for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit)
			{
				int nRight = m_PlayerRight[Sit];
				if ( IsLossRight(nRight) )
				{
					CurValue += PlayerRightValueChangce[nRight];
					if ( LuckValue <= CurValue )
					{
						MinPlayerSit = Sit;
						break;
					}
				}
			}
			assert(MinPlayerSit!=-1);
		}

		if ( MinPlayerSit!=-1 || MaxPlayerSit!=-1 )
		{
			stHandPai TempHandPai[MAX_PALYER_ON_TABLE];
			memset(TempHandPai,0,sizeof(TempHandPai));
			memcpy(TempHandPai,m_HandPai,sizeof(TempHandPai));
			memset(m_HandPai,0,sizeof(m_HandPai));

			CPaiType TempPaiValue[MAX_PALYER_ON_TABLE];
			memset(TempPaiValue,0,sizeof(TempPaiValue));
			memcpy(TempPaiValue,m_MaxPaiType,sizeof(TempPaiValue));
			
			BYTE TempSort[MAX_PALYER_ON_TABLE];
			memset(TempSort,0,sizeof(TempSort));

			//将牌的大小按坐位号排序
			int CountK=0,MinIdx;
			for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
			{
				CPaiType MaxPai;
				for (int k=0;k<MAX_PALYER_ON_TABLE;k++)
				{
					if ( IsHavePlayer(k) && TempPaiValue[k]>MaxPai )
					{
						MaxPai =  m_MaxPaiType[k];
						TempSort[CountK] = k+1;
					}
				}
				if ( TempSort[CountK] > 0 )
				{
					MinIdx = CountK;
					TempPaiValue[TempSort[CountK]-1].Init();
					CountK++;
				}
				else
				{
					break;
				}
			}
			assert(MinIdx == PlayerCount-1);
			memcpy(TempPaiValue,m_MaxPaiType,sizeof(TempPaiValue));
			memset(m_MaxPaiType,0,sizeof(m_MaxPaiType));

			int EmptyPai = 0,EmptyValue = 0;
			for ( int Sit=0;Sit<MAX_PALYER_ON_TABLE;++Sit )
			{
				if ( TempHandPai[Sit].IsEmptyPai()) EmptyPai++;
				if ( TempPaiValue[Sit].IsEmpty() ) EmptyValue++;
			}

			if ( MaxPlayerSit != -1 )
			{
				int MaxPaiIdx = TempSort[0] - 1;

				m_HandPai[MaxPlayerSit]     = TempHandPai[MaxPaiIdx];
				TempHandPai[MaxPaiIdx].Init();

				m_MaxPaiType[MaxPlayerSit]  = TempPaiValue[MaxPaiIdx];
				TempPaiValue[MaxPaiIdx].Init();
			}

			if ( MinPlayerSit != -1 )
			{				
				int MinPaiIdx = TempSort[MinIdx]-1;			

				m_HandPai[MinPlayerSit]     = TempHandPai[MinPaiIdx];
				TempHandPai[MinPaiIdx].Init();

				m_MaxPaiType[MinPlayerSit]  = TempPaiValue[MinPaiIdx];
				TempPaiValue[MinPaiIdx].Init();
			}

			if ( MinPlayerSit!=-1 && MaxPlayerSit!=-1 )
			{
				assert(!(m_MaxPaiType[MaxPlayerSit]<m_MaxPaiType[MinPlayerSit]));
			}

			int CountPai = 0,CountValue = 0;
			for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
			{
				if ( IsHavePlayer(Sit) )
				{
					if ( Sit==MaxPlayerSit || Sit==MinPlayerSit )
					{
						continue;
					}
					while ( TempHandPai[CountPai].IsEmptyPai() )
					{
						CountPai++;
					}
					while ( TempPaiValue[CountValue].IsEmpty() )
					{
						CountValue++;
					}
					if ( CountPai>=MAX_PALYER_ON_TABLE || CountValue>=MAX_PALYER_ON_TABLE )
					{
						fprintf_s(stderr,"Error CountPai=%d CountValue=%d \n",CountPai,CountValue);
					}
					assert(CountPai<MAX_PALYER_ON_TABLE);
					assert(CountValue<MAX_PALYER_ON_TABLE);
					
					m_HandPai[Sit] = TempHandPai[CountPai++];
					m_MaxPaiType[Sit] = TempPaiValue[CountValue++];					
				}		
			}
		}

		for ( int i=0;i<MAX_PALYER_ON_TABLE-1;i++)
		{
			for (int j=i+1;j<MAX_PALYER_ON_TABLE;j++)
			{
				if ( IsHavePlayer(i) && IsHavePlayer(j) )
				{
					assert(!(m_HandPai[i]==m_HandPai[j]));
				}
			}
		}

		for (int Sit=0;Sit<MAX_PALYER_ON_TABLE;Sit++)
		{
			if ( IsHavePlayer(Sit) ) m_PlayerRight[Sit] = PLAYER_RIGHT_COMMON;
			else m_PlayerRight[Sit] = PLAYER_RIGHT_NONE;
		}
	}

	int CGamePaiLogic::GetBlindBeiShu(int Sit)
	{
		int  HandPaiLevel = GetHandPaiLevel(Sit);
		int  nPaiValueType = m_MaxPaiType[Sit].m_Type;		
		return CRandom::Random_Int(1,max(HandPaiLevel*nPaiValueType/6,1));
	}
	int CGamePaiLogic::GetHandPaiChangce(int Sit)
	{
		int HandPaiLevel = GetHandPaiLevel(Sit);
		return m_MaxPaiType[Sit].m_Type * HandPai_Level[HandPaiLevel-1];
	}

	void GetCreatePlayerGameInfo(UINT8 Level,stCreatePlayerGameInfo& stCPGI)
	{
		int LevelIdx = Level-1;

		stCPGI.m_nUpperLimite  = c_TopLimit[LevelIdx];
		stCPGI.m_nLowerLimite  = Level*10000;

		stCPGI.m_OrigenMoney   = c_OrigenMoney[LevelIdx];
		stCPGI.m_LandMoney     = c_LandMoney[LevelIdx];

		CPlayerRightInfo TempPRT;
		TempPRT.m_Right[PLAYER_RIGHT_HIGH_1] = Level*25;
		TempPRT.m_Right[PLAYER_RIGHT_HIGH_2] = Level*10;
		TempPRT.m_Right[PLAYER_RIGHT_HIGH_3] = Level*5;
		TempPRT.m_Right[PLAYER_RIGHT_HIGH_4] = Level*2;
		TempPRT.m_Right[PLAYER_RIGHT_KING]   = 1;
		stCPGI.m_Right = TempPRT.GetRight();
	}

	const   INT64 s_BotLevelMoney[] = 
	{
		30000,
		50000,
		100000,
		300000,
		1000000,
		3000000,
		10000000,
		30000000,
		100000000,
		500000000,
		1500000000
	};
	bool IsRightBotLevel(int BotLevel)
	{
		return BotLevel>0 && BotLevel<=10;
	}
	INT64 GetBotLevelMoney(int nLevel)
	{
		if ( nLevel>=0 && nLevel<=10 )
		{
			return s_BotLevelMoney[nLevel];
		}
		return s_BotLevelMoney[0];
	}

	const int c_PlayerRightUseRank[] = 
	{
		0,
		6,
		3,
		0,
		1,
		2,
		3,
		4,
		5
	};

	CPlayerRightInfo::CPlayerRightInfo()
	{
		m_RightLevel = PLAYER_RIGHT_NONE;
		memset(m_Right,0,RIGHTCOUNT);
		UpdateLevelTimes();
	}
	void CPlayerRightInfo::InitRight(long long nRight)
	{
		m_RightLevel = PLAYER_RIGHT_NONE;
		memset(m_Right,0,RIGHTCOUNT);
		memcpy(m_Right+1,&nRight,8);
		UpdateLevelTimes();
	}
	long long CPlayerRightInfo::GetRight()
	{
		long long nRet;
		m_Right[PLAYER_RIGHT_COMMON]=0;
		memcpy(&nRet,m_Right+1,8);
		return nRet;
	}
	void CPlayerRightInfo::UpdateLevelTimes()
	{
		m_RightLevel = PLAYER_RIGHT_NONE;

		int MaxRightRank = 0;
		for ( int nLevel=0;nLevel<RIGHTCOUNT;nLevel++ )
		{
			if ( m_Right[nLevel]>0 && c_PlayerRightUseRank[nLevel]>MaxRightRank )
			{
				m_RightLevel = nLevel;
			}
		}

		if ( m_RightLevel == PLAYER_RIGHT_NONE )
		{
			m_RightLevel = PLAYER_RIGHT_COMMON;
			m_Right[m_RightLevel] = 255;
		}
	}
	void CPlayerRightInfo::AddRight(int nLevel,int nTimes)
	{
		if ( nLevel>PLAYER_RIGHT_NONE && nLevel<=PLAYER_RIGHT_KING )
		{
			int TempTimes = m_Right[nLevel] + nTimes;
			TempTimes = max(TempTimes,0);
			TempTimes = min(TempTimes,255);
			m_Right[nLevel] = TempTimes;
			UpdateLevelTimes();
		}
	}
	void CPlayerRightInfo::ReduceRightTime()
	{
		if ( m_RightLevel == PLAYER_RIGHT_NONE )
		{
			UpdateLevelTimes();
		}
		else if ( m_RightLevel>PLAYER_RIGHT_NONE && m_RightLevel<=PLAYER_RIGHT_KING )
		{
			if ( m_Right[m_RightLevel] > 0 )
			{
				m_Right[m_RightLevel]--;
			}

			if ( m_Right[m_RightLevel] == 0 )
			{
				UpdateLevelTimes();
			}
		}
	}
	int CPlayerRightInfo::GetRightLevel()
	{
		if ( m_RightLevel>PLAYER_RIGHT_NONE && m_RightLevel<=PLAYER_RIGHT_KING )
		{
			return m_RightLevel;
		}
		return PLAYER_RIGHT_COMMON;
	}
	int CPlayerRightInfo::GetRightTimes()
	{
		if ( m_RightLevel>PLAYER_RIGHT_NONE && m_RightLevel<=PLAYER_RIGHT_KING )
		{
			return m_Right[m_RightLevel];
		}
		return 0;
	}

	const int s_JuBaoPengBeiShu[PAI_TYPE_MAX] = 
	{
		0,
		0,       //PAI_TYPE_ONE
		0,
		0,
		30,
		40,
		50,
		60,
		600,
		4000,
		40000
	};
	bool IsValidJuBaoPaiType(int nType)
	{
		return IsValidPaiType(nType) && s_JuBaoPengBeiShu[nType]>0;
	}
	int GetJuBaoPengBeiShu(int nType)
	{
		if (nType>=PAI_TYPE_ONE && nType<=PAI_TYPE_KING)
		{
			return s_JuBaoPengBeiShu[nType];
		}
		return 0;
	}
	bool IsValidPaiType(int nType)
	{
		return nType>=PAI_TYPE_ONE && nType<=PAI_TYPE_KING;
	}

	enum HonorType
	{
		HonorType_First    = 1,
		HonorType_Second   = 2,
		HonorType_Third    = 3,
		HonorType_Fouth    = 4,
	};
	const int Honor_Full                       = 0x40000000;

	bool IsValidHonorID(int HonorID)
	{
		return HonorID>=1 && HonorID<200;
	}
	bool IsValidType(int nType)
	{
		return nType>=HonorType_First && nType<=HonorType_Fouth;
	}
	bool IsValidIdx(int nIdx)
	{
		return nIdx>0 && nIdx<=31;
	}
	int GetType(int HonorID)
	{
		return HonorID/50+1;
	}
	int GetIdx(int HonorID)
	{
		return HonorID%50;
	}
	bool IsAllFinish(int nAchieve)
	{
		return (nAchieve&Honor_Full) > 0;
	}
	int  GetHonorID(int nType,int Idx)
	{
		if ( IsValidType(nType) && IsValidIdx(Idx) )
		{
			return (nType-1)*50 + Idx;
		}
		return 0;
	}

	bool  IsValidPayIncomeFlag(int nFlag)
	{
		return IsPayFlag(nFlag) || IsIncomeFlag(nFlag);
	}
	bool  IsPayFlag(int nFlag)
	{
		return nFlag>2000 && nFlag<4000;
	}
	bool IsIncomeFlag(int nFlag)
	{
		return nFlag>0 && nFlag<2000;
	}
};