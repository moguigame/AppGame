#pragma once

#include "PublicDef.h"

#include "DBServerXY.h"

using namespace MoGui;
using namespace MoGui::MoGuiXY::DBServerXY;

namespace MoGui
{
namespace Game
{
namespace DeZhou
{
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

	struct stCreatePlayerGameInfo
	{
		INT16                                 m_AID;
		UINT32                                m_PID;

		INT64                                 m_nUpperLimite;
		INT64                                 m_nLowerLimite;

		INT64                                 m_OrigenMoney;
		INT64                                 m_LandMoney;

		int                                   m_Right[4];

		stCreatePlayerGameInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stUserInfo
	{
		INT16                                 m_AID;
		UINT32                                m_PID;

		int                                   m_Sex;
		int                                   m_ChangeName;

		std::string                           m_NickName;
		std::string                           m_HeadPicURL;
		std::string                           m_HomePageURL;
		std::string                           m_City;

		stUserInfo(){ Init(); }
		void Init()
		{
			m_PID = 0;
			m_AID = 0;

			m_Sex = 0;
			m_ChangeName = 0;

			m_NickName = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";
		}
	};

	struct stInviteJoin
	{
		UINT32                               m_InvitePID;
		UINT32                               m_JoinTime;

		stInviteJoin(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	
	struct stDBMoguiInfo
	{
		short                                 m_AID;
		UINT32                                m_PID;
		int                                   m_MoGuiMoney;

		stDBMoguiInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stDBGameMoney
	{
		short                                 m_AID;
		UINT32                                m_PID;
		INT64                                 m_GameMoney;

		stDBGameMoney(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stDBGameMoneyError
	{
		short                       m_AID;
		UINT32                      m_PID;
		INT64                       m_nAddMoney;
		INT64                       m_nCurGameMoney;
		std::string                 m_strLogMsg;

		stDBGameMoneyError(){ ReSet();}
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_nAddMoney = 0;
			m_nCurGameMoney = 0;
			m_strLogMsg = "";
		}
	};

	struct stDBMoGuiMoneyError
	{
		short                                 m_AID;
		UINT32                                m_PID;
		int                                   m_AddMoney;
		int                                   m_CurMoGuiMoney;
		std::string                           m_strLog;

		stDBMoGuiMoneyError(){ ReSet();}
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;
			m_AddMoney = 0;
			m_CurMoGuiMoney = 0;
			m_strLog = "";
		}
	};

	struct stDBUpdateGiftInfo
	{
		unsigned int              m_PID;
		int                       m_GiftIdx;
		short                     m_Flag;

		stDBUpdateGiftInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}
	};

	struct stDBUserAward
	{
		int                         m_Idx;
		UINT32                      m_PID;
		INT16                       m_AID;

		INT64                       m_nMoney;
		INT16                       m_MoneyFlag;
		INT16                       m_PayMode;
		UINT32                      m_EndTime;

		stDBUserAward(){ Init(); }
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef std::vector<stDBUserAward>   VectorAward;

	struct stDBGameRight 
	{
		int                         m_Idx;
		UINT32                      m_PID;
		INT16                       m_AID;

		int                         m_Right;
		short                       m_Flag;
		UINT32                      m_EndTime;

		stDBGameRight(){ Init(); }
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef std::vector<stDBGameRight>  VectorRight;

	struct stUserGameInfo
	{
		INT16                                   m_AID;
		UINT32                                  m_PID;

		INT64                                   m_nGameMoney;
		INT32                                   m_nMoguiMoney;
		INT64                                   m_nMatchJF;

		INT64                                   m_JF;
		INT64                                   m_EP;
		INT32                                   m_nWinTimes;
		INT32                                   m_nLossTimes;
		INT32                                   m_nGameTime;

		INT64                                   m_nUpperLimite;
		INT64                                   m_nLowerLimite;

		INT32                                   m_Right;

		stUserGameInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stUpdateUserGameInfo
	{
		INT16                                   m_AID;
		UINT32                                  m_PID;

		INT64                                   m_nGameMoney;
		INT64                                   m_nMatchJF;

		INT64                                   m_JF;
		INT64                                   m_EP;
		INT32                                   m_nWinTimes;
		INT32                                   m_nLossTimes;
		INT32                                   m_nGameTime;

		INT64                                   m_nUpperLimite;
		INT64                                   m_nLowerLimite;

		stUpdateUserGameInfo(){Init();}
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
	typedef std::vector<stRoomInfo> VectorRoomInfo;

	struct stTableInfo
	{
		INT16                		m_ServerID;
		INT16                		m_RoomID;
		INT16                		m_TableID;
		UINT32               		m_FoundByWho;
		
		std::string                 m_TableName;
		std::string                 m_Password;
		std::string                 m_TableRule;
		std::string                 m_TableRuleEX;
		std::string                 m_MatchRule;

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
			m_TableRuleEX = "";
			m_MatchRule = "";

			m_TableEndTime = 0;
		}
	};
	typedef std::vector<stTableInfo>     VectorTableInfo;

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
	typedef std::vector<stFaceInfo>             VectorFaceInfo;
	typedef std::map<INT16,stFaceInfo>          MapFaceInfo;

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
	typedef std::list<stGiftInfo>           ListGiftInfo;
	typedef std::vector<stGiftInfo>         VectorGiftInfo;
	typedef std::map<INT16, stGiftInfo>      MapGiftInfo;

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
		UINT32               m_RecvPID;
		INT16                m_GiftID;
		INT32                m_Price;
		UINT32               m_SendPID;

		UINT32               m_ActionTime;
		UINT32               m_CurEndTime;
		UINT32               m_LastEndTime;

		stDBUserGiftInfo(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};
	typedef vector<stDBUserGiftInfo>     VectorDBUserGiftInfo;
	typedef list<int>                    ListGiftIdx;

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
	typedef list<stUserFriendFlag>      ListFriendFlag;

	struct stAddUserMoney
	{
		INT64               m_nWinMoney;
		INT64               m_nLossMoney;
		INT64               m_nServiceMoney;
		INT64               m_nGivenMoney;
		INT64               m_nIncomeMoney;
		INT64               m_nExpendMoney;

		stAddUserMoney(){Init();}
		void Init(){ memset(this,0,sizeof(*this));}
	};

	struct stPlayerData
	{
		UINT32                      m_PID;
		INT16                       m_AID;

		int                         m_ChangeName;
		std::string                 m_NickName;
		std::string                 m_HeadPicURL;
		std::string                 m_HomePageURL;
		std::string                 m_City;

		int                         m_Sex;

		UINT32                      m_JoinTime;                 //加入游戏的时间
		UINT32                      m_InvitePID;                //被哪个玩家邀请进来，0为系统

		UINT32                      m_nMoGuiMoney;
		INT64    					m_nGameMoney;
		INT64                       m_nMatchJF;

		INT64                       m_nJF;
		INT64                       m_nEP;
		INT32                       m_nWinTimes;
		INT32                       m_nLossTimes;
		INT32                       m_nGameTime;

		INT64                       m_nUpperLimite;
		INT64                       m_nLowerLimite;

		INT32                       m_Right;

		stPlayerData(){Init();}
		~stPlayerData(){}

		void Init()
		{
			m_PID  = 0;
			m_AID  = 0;

			m_ChangeName = 0;
			m_NickName = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";

			m_Sex = 0;
			m_JoinTime = 0;
			m_InvitePID = 0;

			m_nGameMoney = 0;
			m_nMoGuiMoney = 0;
			m_nMatchJF = 0;

			m_nJF = 0;
			m_nEP = 0;
			m_nWinTimes = 0;
			m_nLossTimes = 0;
			m_nGameTime = 0;

			m_nUpperLimite = 0;
			m_nLowerLimite = 0;

			m_Right = 0;
		}
	};

	struct stPlayerDataEx
	{
		enum { WinTypeNum = 10 };

		UINT32                      m_PID;
		INT16                       m_AID;

		INT64                       m_MaxPai;
		UINT32                      m_MaxPaiTime;
		INT64                       m_MaxMoney;
		UINT32                      m_MaxMoneyTime;
		INT64                       m_MaxWin;
		UINT32                      m_MaxWinTime;

		int                         m_WinRecord[WinTypeNum];

		int                         m_Achieve1;
		int                         m_Achieve2;
		int                         m_Achieve3;
		int                         m_Achieve4;

		stPlayerDataEx(){Init();}
		~stPlayerDataEx(){}

		void Init()
		{
			m_PID  = 0;
			m_AID  = 0;

			m_MaxPai = 0;
			m_MaxPaiTime = 0;
			m_MaxMoney = 0;
			m_MaxMoneyTime = 0;
			m_MaxWin = 0;
			m_MaxWinTime = 0;

			memset(m_WinRecord,0,sizeof(m_WinRecord));

			m_Achieve1 = 0;
			m_Achieve2 = 0;
			m_Achieve3 = 0;
			m_Achieve4 = 0;
		}
	};

	struct stBotPlayerData 
	{
		enum { WinTypeNum = 10 };

		UINT32                      m_PID;
		INT16                       m_AID;

		std::string                 m_NickName;
		std::string                 m_HeadPicURL;
		std::string                 m_HomePageURL;
		std::string                 m_City;

		int                         m_Sex;

		UINT32                      m_JoinTime;                 //加入游戏的时间
		UINT32                      m_InvitePID;                //被哪个玩家邀请进来，0为系统

		UINT8                       m_BotLevel;                 //机器人的级别用于控制机器人的游戏币总量

		INT64    					m_nGameMoney;
		UINT32                      m_nMoGuiMoney;
		INT64                       m_nMatchJF;

		INT64                       m_nJF;
		INT64                       m_nEP;
		INT32                       m_nWinTimes;
		INT32                       m_nLossTimes;
		INT32                       m_nGameTime;

		INT64                       m_MaxPai;
		UINT32                      m_MaxPaiTime;
		INT64                       m_MaxMoney;
		UINT32                      m_MaxMoneyTime;
		INT64                       m_MaxWin;
		UINT32                      m_MaxWinTime;

		int                         m_WinRecord[WinTypeNum];

		int                         m_Achieve1;
		int                         m_Achieve2;
		int                         m_Achieve3;
		int                         m_Achieve4;

		stBotPlayerData(){Init();}
		~stBotPlayerData(){}

		void Init()
		{
			m_PID  = 0;
			m_AID  = 0;

			m_NickName = "";
			m_HeadPicURL = "";
			m_HomePageURL = "";
			m_City = "";

			m_Sex = 0;
			m_JoinTime = 0;
			m_InvitePID = 0;

			m_BotLevel = 0;

			m_nGameMoney = 0;
			m_nMoGuiMoney = 0;
			m_nMatchJF = 0;

			m_nJF = 0;
			m_nEP = 0;
			m_nWinTimes = 0;
			m_nLossTimes = 0;
			m_nGameTime = 0;

			m_MaxPai = 0;
			m_MaxPaiTime = 0;
			m_MaxMoney = 0;
			m_MaxMoneyTime = 0;
			m_MaxWin = 0;
			m_MaxWinTime = 0;

			memset(m_WinRecord,0,sizeof(m_WinRecord));

			m_Achieve1 = 0;
			m_Achieve2 = 0;
			m_Achieve3 = 0;
			m_Achieve4 = 0;		
		}
	};
	typedef std::vector<stBotPlayerData>     VectorBotPlayerData;

	class DBServerPlayerInfo : public CMemoryPool_Public<DBServerPlayerInfo, 100>
	{
	public:
		unsigned int                             m_PID;
		INT16                                    m_AID;
		INT16                                    m_ServerID;

		BYTE                                     m_PlayerType;
		bool                                     m_bOnLine;
		UINT32                                   m_ActiveTime;                      //指玩家活跃的最后时间，如果超过X小时那么将被删除
		BYTE                                     m_BotLevel;
		
		ListFriendFlag                           m_listFriend;                      //所有好友的数字ID和FLAG

		//礼品信息
		list<int>                                m_listGiftIdx;

		DBServerPlayerInfo(){ Init(); }
		~DBServerPlayerInfo()
		{
			m_listFriend.clear();
			m_listGiftIdx.clear();
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

			m_listFriend.clear();
			m_listGiftIdx.clear();
		}

		bool IsBotPlayer()
		{
			return m_PlayerType == PLAYER_TYPE_BOT;
		}
		bool IsVisitorPlayer()
		{
			return m_PlayerType == PLAYER_TYPE_VISITOR;
		}

		bool IsOnLine()const 
		{
			return m_bOnLine;
		}
		bool IsDead() const
		{
			return !m_bOnLine;
		}
		void SetActiveTime(UINT32 curTime)
		{
			m_ActiveTime = curTime;
		}
		UINT32 GetActiveTime()const
		{
			return m_ActiveTime;
		}

		void DeleteFriend(UINT32 PID)
		{
			for ( ListFriendFlag::iterator itorFF=m_listFriend.begin();itorFF!=m_listFriend.end();++itorFF)
			{
				if ( itorFF->m_PID == PID )
				{
					m_listFriend.erase(itorFF);
					break;
				}
			}
		}
		bool IsFollow(UINT32 PID)
		{
			if ( PID > 0 )
			{
				for ( ListFriendFlag::iterator itorFF=m_listFriend.begin();itorFF!=m_listFriend.end();++itorFF)
				{
					if ( itorFF->m_PID == PID && (itorFF->m_Flag==N_Friend::Follow|| itorFF->m_Flag==N_Friend::Both) )
					{
						return true;
					}
				}
			}
			return false;
		}
		bool IsBeFollow(UINT32 PID)
		{
			if ( PID )
			{
				for ( ListFriendFlag::iterator itorFF=m_listFriend.begin();itorFF!=m_listFriend.end();++itorFF)
				{
					if ( itorFF->m_PID == PID && (itorFF->m_Flag==N_Friend::BeFollow || itorFF->m_Flag==N_Friend::Both) )
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
			ListFriendFlag::iterator itorFF;
			for ( itorFF=m_listFriend.begin();itorFF!=m_listFriend.end();++itorFF)
			{
				if ( itorFF->m_PID == PID )
				{
					bFind = true;
					break;;
				}
			}
			if ( bFind )
			{
				if ( Flag == N_Friend::None )
				{
					m_listFriend.erase(itorFF);
				}
				else
				{
					itorFF->m_Flag = INT8(Flag);
				}				
			}
			else
			{
				if( Flag != N_Friend::None )
				{
					stUserFriendFlag stFF;
					stFF.m_PID = PID;
					stFF.m_Flag = INT8(Flag);
					m_listFriend.push_back(stFF);
				}
			}
		}

		void DeleteGift(int GiftIdx)
		{
			ListGiftIdx::iterator itorGI = find(m_listGiftIdx.begin(),m_listGiftIdx.end(),GiftIdx);
			if (itorGI != m_listGiftIdx.end() )
			{
				m_listGiftIdx.erase(itorGI);
			}
		}
		void AddGift(int GiftIdx)
		{
			ListGiftIdx::iterator itorGI = find(m_listGiftIdx.begin(),m_listGiftIdx.end(),GiftIdx);
			if (itorGI == m_listGiftIdx.end() )
			{
				m_listGiftIdx.push_back(GiftIdx);
			}		
		}
	};
}
}
}