#pragma once

#include<vector>
#include<list>
#include <set>
#include <string>

#include "MemoryPool.h"
#include "Tool.h"

#include "PublicDef.h"
#include "gamedef.h"

#include "Dezhoulib.h"

namespace MoGui
{
namespace Game
{
namespace DeZhou
{
	using namespace std;
	using namespace AGBase;
	using namespace MoGui;

	typedef  list<int>                     ListPID;
	typedef  set<int>                      SetPID;
	typedef  vector<int>                   VectorPID;

	struct stXieYiLog
	{
		INT32              m_XieYiID;
		INT64              m_TotalByte;
		INT32              m_Percent;

		stXieYiLog(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef vector<stXieYiLog>              VectorXieYiLog;
	class CCompareXieYiTotalByte
	{
	public:
		bool operator()(const stXieYiLog& lSrc,const stXieYiLog& rSrc)
		{
			return rSrc.m_TotalByte < lSrc.m_TotalByte;
		}
	};
	
namespace DBS
{
	struct stDBGameMoney
	{
		int                                   m_AID;
		int                                   m_PID;
		INT64                                 m_GameMoney;
		INT64                                 m_HongBao;

		stDBGameMoney(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stDBAreaInfo
	{
		int                 m_AID;
		string              m_AreaName;
		string              m_JoinRule;
		string              m_LandRule;
		stDBAreaInfo(){Init();}
		void Init()
		{
			m_AID = 0;
			m_AreaName = "";
			m_JoinRule = "";
			m_LandRule = "";
		}
	};
	typedef vector<stDBAreaInfo>         VectorDBAreaInfo;

	struct stAreaInfo
	{
		int                 m_AID;
		string              m_AreaName;
		int                 m_JoinMoney[Max_Player_Level];
		int                 m_LandMoney[Max_Player_Level];

		stAreaInfo(){Init();}
		void Init()
		{
			m_AID = 0;
			m_AreaName = "";
			for ( int i=0;i<Max_Player_Level;++i)
			{
				m_JoinMoney[i] = m_LandMoney[i] = 0;
			}
		}

		void InitLandMoney(const string& strLang)
		{
			vector<string> vecStr = Tool::SplitString(strLang,",");
			for (size_t i=0;i<vecStr.size()&&i<Max_Player_Level;i++)
			{
				Tool::S2N(vecStr.at(i),m_LandMoney[i]);
				m_LandMoney[i] = max(m_LandMoney[i],0);
			}
		}
		void InitJoinMoney(const string& strJoin)
		{
			vector<string> vecStr = Tool::SplitString(strJoin,",");
			for (size_t i=0;i<vecStr.size()&&i<Max_Player_Level;i++)
			{
				Tool::S2N(vecStr.at(i),m_JoinMoney[i]);
				m_JoinMoney[i] = max(m_JoinMoney[i],0);
			}
		}
	};	
	typedef map<int,stAreaInfo>        MapAreaInfo;

	struct stDBUserAward
	{
		int                         m_Idx;
		int                         m_PID;
		int                         m_AID;

		INT64                       m_nMoney;
		INT16                       m_MoneyFlag;
		INT16                       m_PayMode;
		UINT32                      m_EndTime;

		stDBUserAward(){ Init(); }
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef vector<stDBUserAward>   VectorAward;

	struct stWinLossMoney //专用于一局结束后更新money表
	{
		INT16                       m_AID;
		UINT32                      m_PID;

		INT64                       m_nGameMoney;
		INT64                       m_nHongBao;

		stWinLossMoney(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stRoomInfo
	{
		INT16                m_ServerID;
		INT16                m_RoomID;

		std::string          m_RoomName;
		std::string          m_Password;
		std::string          m_AreaRule;
		std::string          m_RoomRule;
		std::string          m_RoomRuleEX;
		std::string          m_MatchRule;

		UINT32               m_EndTime;

		stRoomInfo(){Init();}
		void Init()
		{
			m_ServerID = 0;
			m_RoomID = 0;

			m_RoomName = "";
			m_Password = "";
			m_AreaRule = "";
			m_RoomRule = "";
			m_RoomRuleEX = "";
			m_MatchRule = "";

			m_EndTime = 0;
		}
	};
	typedef vector<stRoomInfo> VectorRoomInfo;

	struct stTableInfo
	{
		INT16                		m_ServerID;
		INT16                		m_RoomID;
		INT16                		m_TableID;
		UINT32               		m_FoundByWho;

		std::string                 m_TableName;
		std::string                 m_Password;
		std::string                 m_TableRule;

		UINT32               		m_TableEndTime;

		stTableInfo(){Init();}
		void Init()
		{
			m_ServerID = 0;
			m_RoomID = 0;
			m_TableID = 0;
			m_FoundByWho = 0;

			m_TableName = "";
			m_Password = "";
			m_TableRule = "";

			m_TableEndTime = 0;
		}
	};
	typedef vector<stTableInfo>     VectorTableInfo;

	struct stFaceInfo
	{
		INT16                m_FaceID;
		INT16                m_Type;
		INT16                m_PriceFlag;
		INT16                m_Price;
		INT32                m_MinPrice;
		INT32                m_MaxPrice;

		stFaceInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef vector<stFaceInfo>             VectorFaceInfo;
	typedef map<INT16,stFaceInfo>          MapFaceInfo;

	struct stGiftInfo
	{
		INT16                m_GiftID;
		INT16                m_Type;
		INT16                m_PriceFlag;
		INT32                m_Price;
		INT32                m_MinPrice;
		INT32                m_MaxPrice;
		INT16                m_Rebate;
		UINT32               m_CurLastTime;
		UINT32               m_TotalLastTime;

		stGiftInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef list<stGiftInfo>           ListGiftInfo;
	typedef vector<stGiftInfo>         VectorGiftInfo;
	typedef map<INT16,stGiftInfo>      MapGiftInfo;


	struct ProductInfo
	{
		int                m_ProductID;
		int                m_Price;
		short              m_ProductType;
		string             m_ProductRule;

		ProductInfo(){ Init(); }
		void Init()
		{
			m_ProductID = 0;
			m_Price = 0;
			m_ProductType = 0;
			m_ProductRule = "";
		}
	};
	typedef vector<ProductInfo>   VectorProductInfo;
	typedef map<int,ProductInfo>  MapProductInfo;

	struct stHuiYuanInfo
	{
		int                m_ProductID;

		BYTE               m_VipLevel;
		INT64              m_SendOneceMoney;
		INT32              m_GivenDaysMoney;
		BYTE               m_BankBox;
		BYTE               m_FreeFace;
		UINT32             m_DuringTime;

		stHuiYuanInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef map<int,stHuiYuanInfo>                      MapHuiYuanInfo;	

	struct stMoguiExchangeInfo
	{
		int                m_ProductID;
		int                m_MoGuiMoney;

		stMoguiExchangeInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef map<int,stMoguiExchangeInfo>                MapMoguiExchangeInfo;	

	struct stUserProduct
	{
		INT16              m_AID;
		UINT32             m_PID;

		int                m_Idx;
		int                m_ProductID;
		int                m_SellMoney;
		INT16              m_UseFlag;
		UINT32             m_ActionTime;

		stUserProduct(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef vector<stUserProduct> VectorUserProduct;

	//struct stHonorInfo
	//{
	//	INT16              m_ID;
	//	INT16              m_Idx;
	//	INT16              m_Type;
	//	INT32              m_HonorMoney;

	//	stHonorInfo(){Init();}
	//	void Init(){ memset(this,0,sizeof(*this));}
	//};
	//typedef vector<stHonorInfo>     VectorHonorInfo;
	//typedef map<INT16,stHonorInfo>  MapHonorInfo;

	//struct stPlayTimesInfo
	//{
	//	INT16              m_PTLevel;
	//	INT32              m_TotalPT;
	//	INT32              m_AddPT;
	//	INT32              m_AwardMoney;

	//	stPlayTimesInfo(){Init();}
	//	void Init(){ memset(this,0,sizeof(*this));}
	//};
	//typedef vector<stPlayTimesInfo>     VectorPTLevelInfo;
	//typedef map<INT16,stPlayTimesInfo>  MapPTLevelInfo;

	struct stDBMatchInfo
	{
		int                       m_MatchID;
		int                       m_MatchType;

		int                       m_Ticket;
		int                       m_nTakeMoney;
		int                       m_StartMoney;

		std::string               m_strBlind;
		std::string               m_strAward;	
		std::string               m_StrRule;

		UINT32                    m_StartTime;
		int                       m_StartInterval;

		stDBMatchInfo() { Init(); }
		void Init()
		{
			m_MatchID = 0;
			m_MatchType = 0;
			m_Ticket = 0;
			m_nTakeMoney = 0;
			m_StartMoney = 0;

			m_strBlind = "";
			m_strAward = "";
			m_StrRule = "";

			m_StartTime = 0;
			m_StartInterval = 0;
		}
	};
	typedef vector<stDBMatchInfo>      VectorDBMatchInfo;
	typedef map<int,stDBMatchInfo>     MapDBMatchInfo;

	struct stDBUserGiftInfo
	{
		int                  m_GiftIdx;
		INT16                m_GiftID;
		UINT32               m_RecvPID;
		INT32                m_Price;
		UINT32               m_SendPID;
		string               m_NickName;
		UINT32               m_ActionTime;	

		stDBUserGiftInfo(){Init();}
		void Init()
		{
			m_GiftIdx = 0;
			m_RecvPID = 0;
			m_GiftID  = 0;
			m_Price   = 0;
			m_SendPID = 0;
			m_NickName= "";
			m_ActionTime = 0;
		}
	};
	typedef vector<stDBUserGiftInfo>     VectorDBUserGiftInfo;
	class CCompareUGI
	{
	public:
		CCompareUGI(){};
		~CCompareUGI(){};

		bool operator()(stDBUserGiftInfo& lSrc,stDBUserGiftInfo& rSrc)
		{
			return lSrc.m_GiftIdx < rSrc.m_GiftIdx;
		}
	};

	struct stUserGiftInfo
	{
		int                  m_GiftIdx;
		INT16                m_GiftID;
		INT32                m_Price;
		UINT32               m_ActionTime;
		string               m_NickName;

		stUserGiftInfo(){ Init(); }
		void Init()
		{
			m_GiftIdx = 0;
			m_GiftID  = 0;
			m_Price   = 0;
			m_ActionTime = 0;
			m_NickName= "";
		}
	};
	typedef vector<stUserGiftInfo>     VectorUserGiftInfo;

	struct stUserFriendInfo
	{
		//enum { FOLLOW = 1,BEFOLLOW = 2 };
		//enum { NONE=0,SINGLEFOLLOW,SINGLEBEFOLLOW,MUTUALFOLLOW };

		UINT32                      m_LeftPID;
		UINT32                      m_RightPID;
		INT16                       m_Flag;

		stUserFriendInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef vector<stUserFriendInfo>                         VectorFriendInfo;

	struct stUserFriendFlag
	{
		UINT32                      m_PID;
		INT8                        m_Flag;

		stUserFriendFlag(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef vector<stUserFriendFlag>    VectorFriendFlag;

	struct stGameInfo
	{		
		INT64    					m_nGameMoney;
		INT32                       m_nMedal;
		INT32                       m_Diamond;
		INT32                       m_GoldenPig;
		INT32                       m_PigFood;

		INT32                       m_Forbid;
		INT64                       m_nJF;
		INT64                       m_nEP;
		INT32                       m_nWinTimes;
		INT32                       m_nLossTimes;
		INT32                       m_DrawnTimes;
		INT32                       m_EscapeTimes;
		INT32                       m_nGameTime;

		string                      m_MaxPai;
		UINT32                      m_MaxPaiTime;
		INT64                       m_MaxMoney;
		UINT32                      m_MaxMoneyTime;
		INT64                       m_MaxWin;
		UINT32                      m_MaxWinTime;

		UINT32                      m_JoinTime;

		stGameInfo(){ Init(); }
		void Init()
		{
			m_nGameMoney = 0;
			m_nMedal = 0;
			m_Diamond = 0;
			m_GoldenPig = 0;
			m_PigFood = 0;

			m_Forbid = 0;
			m_nJF = 0;
			m_nEP = 0;
			m_nWinTimes = 0;
			m_nLossTimes = 0;
			m_DrawnTimes = 0;
			m_EscapeTimes = 0;
			m_nGameTime = 0;

			m_MaxPai = "";
			m_MaxPaiTime = 0;
			m_MaxMoney = 0;
			m_MaxMoneyTime = 0;
			m_MaxWin = 0;
			m_MaxWinTime = 0;

			m_JoinTime = 0;
		}
	};

	struct stDBGameInfo
	{
		INT16                                   m_AID;
		UINT32                                  m_PID;

		stGameInfo                            m_stGameInfo;

		stDBGameInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stLoginInfo
	{
		UINT32                      m_FirstTodayTime;
		BYTE                        m_ContinueLogin;
		BYTE                        m_ContinuePlay;

		stLoginInfo(){ Init(); }
		void Init(){ memset(this,0,sizeof(*this)); }
	};

	struct stUserDataInfo
	{
		int                                     m_Sex;
		int                                     m_HeadID;
		int                                     m_InvitePID;

		std::string                             m_UserName;
		std::string                             m_Password;
		std::string                             m_NickName;
		std::string                             m_HardCode;
		std::string                             m_HeadPicURL;
		std::string                             m_HomePageURL;
		std::string                             m_City;

		stUserDataInfo(){ Init(); }
		void Init()
		{
			m_Sex = 0;
			m_HeadID = 0;
			m_InvitePID = 0;

			m_UserName = "";
			m_Password = "";
			m_NickName = "";
			m_HardCode = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";
		}
	};

	struct stDBUserDataInfo
	{
		int                 m_AID;
		int                 m_PID;
		stUserDataInfo      m_stUDI;

		stDBUserDataInfo(){ Init(); }
		void Init()
		{
			m_AID = 0;
			m_PID = 0;
			m_stUDI.Init();
		}
	};

	struct stRegisterUser
	{
		enum { Flag_UserName=1,Flag_HardCode,};

		int                m_nFlag;
		int                m_AID;
		string             m_UserName;
		string             m_Password;
		string             m_HardCode;
		string             m_NickName;

		stRegisterUser(){Init();}
		void Init()
		{
			m_nFlag = 0;
			m_AID   = 0;
			m_UserName = "";
			m_Password = "";
			m_HardCode = "";
			m_NickName = "";
		}
	};
	struct stCreateGameInfo
	{
		int               m_AID;
		int               m_PID;

		INT64             m_OrigenMoney;
		INT64             m_LandMoney;

		stCreateGameInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stBotPlayerData 
	{
		enum { WinTypeNum = 10 };

		int                         m_PID;
		int                         m_AID;

		int                         m_BotLevel;

		stUserDataInfo              m_stUserDataInfo;
		stGameInfo                  m_stGameInfo;

		stBotPlayerData(){ Init(); }
		void Init()
		{
			m_PID  = 0;
			m_AID  = 0;

			m_BotLevel = 0;

			m_stUserDataInfo.Init();
			m_stGameInfo.Init();
		}
	};
	typedef vector<stBotPlayerData>     VectorBotPlayerData;

	class DBServerPlayerInfo : public CMemoryPool_Public<DBServerPlayerInfo, 100>
	{
	public:
		static long                              s_nMoneyLogCount;
	public:
		int                                      m_PID;
		int                                      m_AID;

		INT16                                    m_ServerID;
		BYTE                                     m_PlayerType;
		bool                                     m_bOnLine;		
		int                                      m_BotLevel;
		UINT32                                   m_ActiveTime;             //指玩家活跃的最后时间，如果超过X小时那么将被删除
		UINT32                                   m_UpdateMemCachTime;

		stLoginInfo                              m_stLoginInfo;
		stUserDataInfo                           m_stUserDataInfo;
		stGameInfo                               m_stGameInfo;

		INT16                                    m_FriendCount;
		VectorFriendFlag                         m_vectorFriend;            //所有好友的数字ID和FLAG

		VectorUserGiftInfo                       m_vectorUserGiftInfo;    //礼品信息
		vector<string>                           m_vectorMoneyLog;

		VectorUserProduct                        m_vectorUserProduct;

		DBServerPlayerInfo(){ Init(); }
		virtual ~DBServerPlayerInfo()
		{
			if (m_vectorFriend.size()) m_vectorFriend.clear();
			if (m_vectorUserGiftInfo.size()) m_vectorUserGiftInfo.clear();
			if ( m_vectorMoneyLog.size()) ClearMoneyLog();
			if ( m_vectorUserProduct.size()) m_vectorUserProduct.clear();
		}
		void Init()
		{
			m_PID = 0;
			m_AID = 0;
			m_ServerID = 0;

			m_PlayerType = 0;
			m_bOnLine = false;
			m_ActiveTime = 0;
			m_BotLevel = 0;

			m_stLoginInfo.Init();
			m_stUserDataInfo.Init();
			m_stGameInfo.Init();

			m_FriendCount = 0;
			m_vectorFriend.clear();
			m_vectorUserGiftInfo.clear();
			ClearMoneyLog();
			m_vectorUserProduct.clear();
		}

		bool IsBotPlayer(){	return m_PlayerType == PLAYER_TYPE_BOT;	}
		bool IsOnLine()const { return m_bOnLine; }
		bool IsDead() const	{ return !m_bOnLine; }
		void SetActiveTime(UINT32 curTime){	m_ActiveTime = curTime;	}
		UINT32 GetActiveTime()const	{ return m_ActiveTime; }

		void AddUserProduct(const stUserProduct& stUP)
		{
			if ( m_vectorUserProduct.size() == 0 )
			{
				m_vectorUserProduct.push_back(stUP);
			}
			else
			{
				bool bSuccess = false;
				for ( size_t nPos=0;nPos<m_vectorUserProduct.size();++nPos)
				{
					if ( m_vectorUserProduct[nPos].m_Idx == 0 )
					{
						bSuccess = true;
						m_vectorUserProduct[nPos] = stUP;
						break;
					}
				}
				if ( bSuccess == false )
				{
					m_vectorUserProduct.push_back(stUP);
				}
			}
		}
		void AddMoneyLog(INT64 nAddMoney,string strLog="")
		{
			if( N_CeShiLog::c_MoneyLog )
			{
				InterlockedIncrement(&s_nMoneyLogCount);
				m_vectorMoneyLog.push_back("GameMoney="+Tool::N2S(m_stGameInfo.m_nGameMoney,10)+" nAddMoney="+Tool::N2S(nAddMoney,10)+" "+strLog);
			}
		}
		void ClearMoneyLog()
		{
			if ( m_vectorMoneyLog.size() )
			{
				InterlockedExchangeAdd(&s_nMoneyLogCount,0-int(m_vectorMoneyLog.size()) );
				m_vectorMoneyLog.clear();
			}
		}

		void DeleteFriend(UINT32 PID)
		{
			for ( size_t nPos=0;nPos<m_vectorFriend.size();++nPos)
			{
				if ( m_vectorFriend[nPos].m_PID == PID )
				{
					m_vectorFriend[nPos].Init();
					break;
				}
			}
		}
		bool IsFollow(UINT32 PID)
		{
			if ( PID > 0 )
			{
				for ( size_t nPos=0;nPos<m_vectorFriend.size();++nPos)
				{
					const stUserFriendFlag& rstUFF = m_vectorFriend[nPos];
					if ( rstUFF.m_PID==PID && (rstUFF.m_Flag==N_Friend::Follow || rstUFF.m_Flag==N_Friend::Both) )
					{
						return true;
					}
				}
			}
			return false;
		}
		bool IsBeFollow(UINT32 PID)
		{
			if ( PID > 0 )
			{
				for ( size_t nPos=0;nPos<m_vectorFriend.size();++nPos)
				{
					const stUserFriendFlag& rstUFF = m_vectorFriend[nPos];
					if ( rstUFF.m_PID==PID && (rstUFF.m_Flag==N_Friend::BeFollow || rstUFF.m_Flag==N_Friend::Both) )
					{
						return true;
					}
				}
			}
			return false;
		}
		void UpdateFriendFlag(UINT32 PID,int Flag)
		{
			if ( Flag < N_Friend::None || Flag > N_Friend::Both )
			{
				return ;
			}

			bool bFind = false;
			int  FindPos = 0;
			for ( size_t nPos=0;nPos<m_vectorFriend.size();++nPos)
			{
				if ( m_vectorFriend[nPos].m_PID == PID )
				{
					bFind = true;
					FindPos = nPos;
					break;
				}
			}
			if ( bFind )
			{
				if ( Flag == N_Friend::None )
				{
					m_vectorFriend[FindPos].Init();
				}
				else
				{
					m_vectorFriend[FindPos].m_Flag = INT8(Flag);
				}				
			}
			else
			{
				if( Flag != N_Friend::None )
				{
					stUserFriendFlag stFF;
					stFF.m_PID = PID;
					stFF.m_Flag = INT8(Flag);

					bool bSuccess = false;
					for ( size_t nPos=0;nPos<m_vectorFriend.size();++nPos)
					{
						if ( m_vectorFriend[nPos].m_PID == 0 )
						{
							m_vectorFriend[nPos] = stFF;
							bSuccess = true;
							break;
						}
					}
					if ( bSuccess == false )
					{
						m_vectorFriend.push_back(stFF);
					}
				}
			}
		}
		void DeleteGift(int GiftIdx)
		{
			for (size_t nPos=0;nPos<m_vectorUserGiftInfo.size();++nPos)
			{
				if ( m_vectorUserGiftInfo[nPos].m_GiftIdx == GiftIdx )
				{
					m_vectorUserGiftInfo[nPos].Init();
					break;
				}
			}
		}
		void AddGift(stUserGiftInfo& stGI)
		{
			bool bSuccess = false;
			for (size_t nPos=0;nPos<m_vectorUserGiftInfo.size();++nPos)
			{
				if ( m_vectorUserGiftInfo[nPos].m_GiftIdx == 0 )
				{
					bSuccess = true;
					m_vectorUserGiftInfo[nPos] = stGI;
					break;
				}
			}
			if ( bSuccess == false )
			{
				m_vectorUserGiftInfo.push_back(stGI);
			}
		}
	};
	typedef DBServerPlayerInfo*                          PDBServerPlayerInfo;
	typedef map<unsigned int,DBServerPlayerInfo*>     	 MapDBPlayerInfo;
	//boost::shared_ptr<DBServerPlayerInfo>                PDBPlayerInfo;
};
namespace GS
{
	struct stHandPai
	{
		BYTE            m_Pai[2];

		stHandPai()
		{
			Init();
		}
		stHandPai(BYTE Lpai,BYTE Rpai)
		{
			m_Pai[0] = Lpai;
			m_Pai[1] = Rpai;
		}
		void Init()
		{
			m_Pai[0] = m_Pai[1] = 0;
		}
		void SetPai(BYTE Lpai,BYTE Rpai)
		{
			m_Pai[0] = Lpai;
			m_Pai[1] = Rpai;
		}
		bool IsEmptyPai() const
		{
			return (m_Pai[0] == m_Pai[1]) && (m_Pai[0] == 0);
		}
		bool IsValidPai() const
		{
			return m_Pai[0]>PAI_VALUE_NONE && m_Pai[0]<PAI_VALUE_BACK
				&& m_Pai[1]>PAI_VALUE_NONE && m_Pai[1]<PAI_VALUE_BACK ;
		}

		stHandPai(const stHandPai&hp)
		{
			m_Pai[0] = hp.m_Pai[0];
			m_Pai[1] = hp.m_Pai[1];
		}

		stHandPai& operator=(const stHandPai&hp)
		{
			if( this != &hp )
			{
				m_Pai[0] = hp.m_Pai[0];
				m_Pai[1] = hp.m_Pai[1];
			}
			return *this;
		}
		friend bool operator==(const stHandPai lhs,const stHandPai rhs)
		{
			return lhs.m_Pai[0] == rhs.m_Pai[0] && lhs.m_Pai[1] == rhs.m_Pai[1];
		}
	};
	struct stTablePai
	{
		BYTE            m_TablePai[5];

		stTablePai(){Init();}

		void Init()
		{
			memset(this,0,sizeof(*this));
		}

		stTablePai(const stTablePai& tp)
		{
			Copy(tp);
		}

		stTablePai& operator=(const stTablePai& tp)
		{
			if( this != &tp )
			{
				Copy(tp);
			}
			return *this;
		}
		void Copy(const stTablePai& tp)
		{
			for (int i=0;i<5;i++)
			{
				m_TablePai[i] = tp.m_TablePai[i];
			}
		}
	};

	struct stPlayerOnLine
	{
		short                     m_Total;
		short                     m_Lobby;
		short                     m_Table;
		short                     m_Game;
		short                     m_Match;

		stPlayerOnLine(){ Init(); }
		void Init(){ memset(this,0,sizeof(*this)); }
	};
	typedef map<short,stPlayerOnLine> MapPlayerOnLine;

	struct TablePoolMoney
	{
		INT64            m_nPoolMoney;
		BYTE             m_ChipSit[MAX_PALYER_ON_TABLE];         //下注玩家的数字帐号
		BYTE             m_WinSitID[MAX_PALYER_ON_TABLE];

		TablePoolMoney(){Init();}

		void Init()
		{
			memset(this,0,sizeof(*this));
		}

		TablePoolMoney(const TablePoolMoney& tpm)
		{
			Copy(tpm);
		}

		TablePoolMoney& operator=(const TablePoolMoney& tpm)
		{
			if( this != &tpm )
			{
				Copy(tpm);
			}
			return *this;
		}
		void Copy(const TablePoolMoney& tpm)
		{
			m_nPoolMoney          = tpm.m_nPoolMoney;
			memcpy(m_ChipSit,tpm.m_ChipSit,sizeof(BYTE)*MAX_PALYER_ON_TABLE);
			memcpy(m_WinSitID,tpm.m_WinSitID,sizeof(BYTE)*MAX_PALYER_ON_TABLE);					
		}

		int GetChipCount() const
		{
			int nCount = 0;
			for( int i=0;i<MAX_PALYER_ON_TABLE;i++ )
			{
				if ( m_ChipSit[i] == 1)
				{
					nCount++;
				}				
			}

			return nCount;
		}
		int GetWinCount() const
		{
			int nCount = 0;
			for( int i=0;i<MAX_PALYER_ON_TABLE;i++ )
			{
				if ( m_WinSitID[i] > 0 )
				{
					nCount++;
				}				
			}
			return nCount;
		}				
	};
	static bool DoTogether( TablePoolMoney& ltpm,const TablePoolMoney& rtpm)
	{
		if ( ltpm.m_nPoolMoney == 0 )
		{
			ltpm = rtpm;
			return true;
		}
		if ( rtpm.m_nPoolMoney == 0 )
		{
			return true;
		}

		bool bCanTogether = true;
		for(int i=0;i<MAX_PALYER_ON_TABLE;i++)
		{
			if( ltpm.m_ChipSit[i] != rtpm.m_ChipSit[i] )
			{
				bCanTogether = false;
				break;
			}
		}
		if ( bCanTogether )
		{
			ltpm.m_nPoolMoney += rtpm.m_nPoolMoney;
			return true;
		}

		return false;
	}

	struct stTableMoney
	{
		BYTE                     m_SitID;
		UINT32                   m_PID;                //谁下的注，注码有多少
		INT64                    m_nCurChipMoney;

		stTableMoney(){Init();}

		void Init()
		{
			memset(this,0,sizeof(*this));
		}

		stTableMoney(const stTableMoney& tm)
		{
			Copy(tm);
		}

		stTableMoney& operator=(const stTableMoney& tm)
		{
			if( this != &tm )
			{
				Copy(tm);
			}
			return *this;
		}
		void Copy(const stTableMoney& tm)
		{
			m_SitID          = tm.m_SitID;
			m_PID            = tm.m_PID;
			m_nCurChipMoney  = tm.m_nCurChipMoney;
		}
	};			

	struct CreateRoomInfo
	{
		UINT16           				 m_RoomID;

		string              			 m_RoomName;
		string              			 m_Password;                                    //房间密码
		string                           m_RoomRule;
		string                           m_RoomRuleEX;
		string                           m_MatchRule;
		string                           m_AreaRule;

		CreateRoomInfo(){ ReSet();}
		void ReSet()
		{
			m_RoomID = 0;

			m_RoomName = "";
			m_Password = "";

			m_RoomRule = "";
			m_RoomRuleEX = "";
			m_MatchRule = "";
		}			
	};

	struct CreateTableInfo
	{
		UINT16				    m_TableID;
		string					m_TableName;
		string                  m_Password;
		string                  m_TableRule;
		string                  m_TableRuleEX;
		string                  m_MatchRule;
		UINT32                  m_FoundByWho;
		UINT32                  m_EndTime;

		CreateTableInfo(){ ReSet();}
		void ReSet()
		{
			m_TableID                = 0;
			m_TableName              = "";
			m_Password               = "";
			m_TableRule              = "";
			m_TableRuleEX            = "";
			m_MatchRule              = "";
			m_FoundByWho             = 0;
			m_EndTime                = 0;
		}
	};

	struct stWinLossTally
	{
		int                       m_AID;
		int                       m_PID;                                //玩家帐号
		INT32                     m_nServiceMoney;                      //服务费
		INT64                     m_nWinLossMoney;                      //输赢数,不包括服务费
		BYTE                      m_EndGameFlag;                        //标识符
		BYTE                      m_PaiType;

		INT64                     m_Money;
		INT64                     m_JF;
		INT64                     m_EP;

		stWinLossTally() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
	};

	struct MatchWinLossTally
	{
		short                     m_AID;
		UINT32                    m_PID;                                //玩家帐号
		INT64                     m_nWinLossMoney;
		BYTE                      m_EndGameFlag;                        //标识符
	};

	struct stMatchInfo
	{
		int                    m_MatchID;
		int                    m_MatchType;
		int                    m_Ticket;
		int                    m_TakeMoney;
		int                    m_StartMoney;
		vector<int>            m_vecBlind;
		vector<int>            m_vecTime;
		vector<int>            m_vecAwardMoney;
		vector<int>            m_vecAwardJF;
		int                    m_FirstAwoke;           //第一次提醒距离游戏开始的间隔
		int                    m_SecondAwoke;          //第二次提醒距离游戏开始的间隔
		int                    m_StartSit;
		UINT32                 m_StartTime;
		int                    m_StartInterval;

		stMatchInfo() { ReSet(); }
		void ReSet()
		{
			m_MatchID = 0;
			m_MatchType = 0;
			m_Ticket = 0;
			m_TakeMoney = 0;
			m_StartMoney = 0;
			m_vecBlind.clear();
			m_vecTime.clear();
			m_vecAwardMoney.clear();
			m_vecAwardJF.clear();
			m_FirstAwoke = 0;
			m_SecondAwoke = 0;
			m_StartSit = 0;
			m_StartTime = 0;
			m_StartInterval = 0;
		}
	};
	typedef map<int,stMatchInfo>             MapMatchInfo;

	struct stForbidenPlayer
	{
		UINT32                    m_PID;
		UINT32                    m_ForbidenEndTime;         //禁止失效的时间点

		stForbidenPlayer() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
	};

	struct stAddFriendInfo
	{
		UINT32       m_ReqPID;
		UINT32       m_ReqTime;

		stAddFriendInfo() { ReSet(); }
		void ReSet()
		{
			m_ReqPID = 0;
			m_ReqTime = 0;
		}
	};
	typedef list<stAddFriendInfo>       ListReqFriend;

	//主要是指机器人的要求
	struct stBogPlayer
	{
		INT64             m_nMinTake;
		INT64             m_nMaxTake;

		stBogPlayer() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
	};

	struct stPlayerInfo
	{
		UINT32        m_PID;
		INT64         m_OriginMoney;
		INT64         m_LeftMoney;
		stHandPai     m_HandPai;
		INT64         m_PaiValue;

		stPlayerInfo() { Init(); }
		void Init()
		{
			m_PID = 0;
			m_OriginMoney = 0;
			m_LeftMoney = 0;
			m_HandPai.Init();
			m_PaiValue = 0;
		}
	};
	typedef  vector<stPlayerInfo>   VecPlayerInfo;

	struct PlayerChipStep
	{
		int     m_Step;
		int     m_SitID;
		INT64   m_nMoney;
		INT64   m_LeftMoney;

		PlayerChipStep() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }

		string GetString()
		{
			return  "CS gs="  + Tool::N2S(m_Step) + 
				" SitID=" + Tool::N2S(m_SitID) +
				" Chip="  + Tool::N2S(m_nMoney,8) + 
				" Left="  + Tool::N2S(m_LeftMoney,8);
		}
	};
	typedef  list<PlayerChipStep>    ListChipStep;
	typedef  vector<PlayerChipStep>  VecChipStep;			

	struct stMoneyPool
	{
		int              m_Flag;
		vector<INT64>    m_vecMoney;

		stMoneyPool() { Init(); }
		void Init()
		{
			m_Flag = 0;
			m_vecMoney.clear();
		}

		string GetString()
		{
			string strRet = "MP  " + Tool::N2S(m_Flag);
			for (vector<INT64>::iterator itorMoney=m_vecMoney.begin();itorMoney!=m_vecMoney.end();++itorMoney)
			{
				strRet += " " + Tool::N2S(*itorMoney,8);
			}
			return strRet;
		}
	};
	typedef vector<stMoneyPool>     VecMoneyPool;

	struct stXieYiTime
	{
		int     m_XieID;
		int     m_nCount;
		INT64   m_TotalMicroSecond;

		stXieYiTime() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
	};
	typedef map<int,stXieYiTime>         MapXieYiTime;

	struct stCityTime
	{
		UINT32           m_UpdateTime;       //上次更新的时间
		UINT32           m_InernalTime;      //更新的间隔时间

		stCityTime() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
	};

	struct stMatchPlayerRank
	{
		unsigned short   m_AID;
		UINT32           m_PID;
		int              m_TableMoney;
		short            m_Position;

		stMatchPlayerRank() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }

		stMatchPlayerRank( const stMatchPlayerRank& src)
		{
			Copy(src);
		}
		stMatchPlayerRank& operator=( const stMatchPlayerRank& src)
		{
			if(this == &src)
			{
				return *this;
			}
			Copy(src);
			return *this;
		}
		void Copy( const stMatchPlayerRank& src )
		{
			this->m_AID         = src.m_AID;
			this->m_PID         = src.m_PID;
			this->m_TableMoney  = src.m_TableMoney;
			this->m_Position    = src.m_Position;
		}
	};
	typedef  vector<stMatchPlayerRank>     VectorMatchPlayerRank;
	class CompareMatchPlayer
	{
	public:
		CompareMatchPlayer(){};
		~CompareMatchPlayer(){};

		bool operator()(stMatchPlayerRank& lSrc,stMatchPlayerRank& rSrc)
		{
			return lSrc.m_TableMoney >= rSrc.m_TableMoney ;
		}
	};

	struct stWinLossPlayerInfo
	{
		UINT32           m_PID;
		INT64            m_WinLossMoney;

		stWinLossPlayerInfo() { ReSet(); }
		void ReSet() { memset( this, 0, sizeof( *this ) ); }
	};
	class CompareWinLossPlayer
	{
	public:
		CompareWinLossPlayer(){};
		~CompareWinLossPlayer(){};

		bool operator()(stWinLossPlayerInfo& lSrc,stWinLossPlayerInfo& rSrc)
		{
			return lSrc.m_WinLossMoney >= rSrc.m_WinLossMoney;
		}
	};
	typedef vector<stWinLossPlayerInfo> VectorWinLossPlayerInfo;
};
};
};
};