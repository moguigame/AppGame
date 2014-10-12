#pragma once

#include<queue>
#include "boost/shared_ptr.hpp"

#include "Base.h"
#include "MemoryPool.h"
#include "MoGuiGame.h"
#include "gamedef.h"

namespace MoGui
{
namespace MoGuiXY
{
namespace RWDB_XY
{
	class ReadWriteDBMessage : public AGBase::CMemoryPool_Public<ReadWriteDBMessage>
	{
	public:
		enum { MaxMsgLen = MAX_TOTAL_XY_LEN - 16 };

		short                  m_MsgID;
		short                  m_MsgLen;
		INT64                  m_nTimeCount;
		char                   m_Message[MaxMsgLen];

		ReadWriteDBMessage() { ReSet(); }
		virtual ~ReadWriteDBMessage(){}

		void ReSet()
		{
			m_MsgID = 0;
			m_MsgLen = 0;
			m_nTimeCount = 0;
			memset(m_Message,0,MaxMsgLen);
		}
	};
	typedef boost::shared_ptr<ReadWriteDBMessage> PRWDBMsg;
	typedef std::queue<PRWDBMsg>                  QueueRWDBMsg;

	template<class Txieyi>
		void MakeRWDBMsg(Txieyi& xieyi,ReadWriteDBMessage& src)
	{
		src.ReSet();
		src.m_MsgID = xieyi.XY_ID;

		bostream bos;
		bos.attach(src.m_Message,src.MaxMsgLen);
		bos<<xieyi;

		src.m_MsgLen = static_cast<short>(bos.length());
	}

	template<class Txieyi>
		int ExplainRWDBMsg(ReadWriteDBMessage& src,Txieyi& xieyi)
	{
		int ret = 0;

		bistream bis;
		bis.attach(src.m_Message,src.m_MsgLen);

		try
		{
			bis >> xieyi;
		}
		catch ( agproexception* e )
		{
			ret = 10000+e->m_cause;
		}
		catch ( biosexception* e )
		{
			ret = 20000+e->m_cause;
		}
		catch(...)
		{
			ret = 30000;
		}

		if ( ret )
		{
			fprintf_s(stderr,"ExplainRWDBMsg Error Ret=%d Id=%d Len=%d \n",ret,src.m_MsgID,src.m_MsgLen);
		}

		return ret;
	}

	const short RWDB_XYID_WinLossLog                               = 10;
	const short RWDB_XYID_WinLossInfo                              = 11;
	const short RWDB_XYID_MaxPai                                   = 12;
	const short RWDB_XYID_MaxWin                                   = 13;
	const short RWDB_XYID_MaxMoney                                 = 14;
	const short RWDB_XYID_UpdateUserGameMoneyType                  = 15;            //玩家收支细项明细表
	const short RWDB_XYID_MatchResultLog                           = 16;
	const short RWDB_XYID_MatchWinLoss                             = 17;
	const short RWDB_XYID_AddLimite                                = 18;
	const short RWDB_XYID_MatchInfo                                = 19;
	const short RWDB_XYID_GameRight                                = 20;
	const short RWDB_XYID_AddGameMoney                             = 21;
	const short RWDB_XYID_WriteUserGift                            = 22;

	const short RWDB_XYID_AddPlayerAward                           = 23;
	const short RWDB_XYID_ChangeBankLog                            = 24;
	const short RWDB_XYID_UpdateHuiYuan                            = 25;
	const short RWDB_XYID_DeleteCreateInfo                         = 26;
	const short RWDB_XYID_UpdateWinType                            = 27;
	const short RWDB_XYID_AddPlayerInfo                            = 28;            //增加玩家的信息的值用FLAG和数值来控制
	const short RWDB_XYID_HonorLog                                 = 29;

	const short RWDB_XYID_MakeFriend                               = 31;
	const short RWDB_XYID_SendMoney                                = 32;
	const short RWDB_XYID_IncomeAndPay                             = 33;

	const short RWDB_XYID_PlayerActionLog                          = 41;
	const short RWDB_XYID_GameError                                = 42;
	const short RWDB_XYID_ChatLog                                  = 43;

	const short RWDB_XYID_MoGuiMoneyLog                            = 51;

	const short RWDB_XYID_TableInfo                                = 71;
	const short RWDB_XYID_UpdateGiftInfo                           = 72;

	const short RWDB_XYID_UpdateAwardInfo                          = 81;
	const short RWDB_XYID_UpdateRightInfo                          = 82;
	const short RWDB_XYID_WriteRightInfo                           = 83;
	const short RWDB_XYID_UpdateAchieve                            = 84;
 
	const short RWDB_XYID_ChangeUserInfo                           = 91;
	const short RWDB_XYID_Login                                    = 92;
	const short RWDB_XYID_CreateGameInfo                           = 93;

	const short RWDB_XYID_ReportPlayerOnline                       = 101;	

	struct RWDB_WinLossLog
	{
		enum { XY_ID = RWDB_XYID_WinLossLog };

		long long                 m_GameGUID;
		short                     m_AID;
		unsigned int              m_PID;
		short                     m_RoomID;
		short                     m_TableID;
		short                     m_SitID;
		int                       m_nServiceMoney;
		long long                 m_nWinLossMoney;
		short                     m_EndGameFlag;
		short                     m_LeftPai;
		short                     m_RightPai;
		short                     m_PaiType;

		RWDB_WinLossLog() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_WinLossLog& src )
		{
			bos << src.m_GameGUID;
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_SitID;
			bos << src.m_nServiceMoney;
			bos << src.m_nWinLossMoney;
			bos << src.m_EndGameFlag;
			bos << src.m_LeftPai;
			bos << src.m_RightPai;
			bos << src.m_PaiType;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_WinLossLog& src )
		{
			src.ReSet();

			bis >> src.m_GameGUID;
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_SitID;
			bis >> src.m_nServiceMoney;
			bis >> src.m_nWinLossMoney;
			bis >> src.m_EndGameFlag;
			bis >> src.m_LeftPai;
			bis >> src.m_RightPai;
			bis >> src.m_PaiType;

			return bis;
		}
	};

	struct RWDB_WinLossInfo
	{
		enum { XY_ID = RWDB_XYID_WinLossInfo };
		
		short                     m_AID;
		unsigned int              m_PID;

		long long                 m_JF;
		long long                 m_EP;
		int                       m_WinTimes;
		int                       m_LossTimes;
		INT64                     m_nUpLimite;
		INT64                     m_nLowLimite;

		RWDB_WinLossInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_WinLossInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_JF;
			bos << src.m_EP;
			bos << src.m_WinTimes;
			bos << src.m_LossTimes;
			bos << src.m_nUpLimite;
			bos << src.m_nLowLimite;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_WinLossInfo& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_JF;
			bis >> src.m_EP;
			bis >> src.m_WinTimes;
			bis >> src.m_LossTimes;
			bis >> src.m_nUpLimite;
			bis >> src.m_nLowLimite;

			return bis;
		}
	};

	struct RWDB_MaxPai
	{
		enum { XY_ID = RWDB_XYID_MaxPai };
		
		short                     m_AID;
		unsigned int              m_PID;
		unsigned int              m_MaxPaiTime;
		long long                 m_MaxPai;

		RWDB_MaxPai() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_MaxPai& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_MaxPaiTime;
			bos << src.m_MaxPai;			

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MaxPai& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_MaxPaiTime;
			bis >> src.m_MaxPai;

			return bis;
		}
	};

	struct RWDB_MaxWin
	{
		enum { XY_ID = RWDB_XYID_MaxWin };

		short                    m_AID;
		unsigned int             m_PID;
		unsigned int             m_MaxWinTime;
		long long                m_MaxWin;

		RWDB_MaxWin() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_MaxWin& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_MaxWinTime;
			bos << src.m_MaxWin;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MaxWin& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_MaxWinTime;
			bis >> src.m_MaxWin;

			return bis;
		}
	};
	struct RWDB_MaxMoney
	{
		enum { XY_ID = RWDB_XYID_MaxMoney };

		short                    m_AID;
		unsigned int             m_PID;
		unsigned int             m_MaxMoneyTime;
		long long                m_MaxMoney;

		RWDB_MaxMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_MaxMoney& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_MaxMoneyTime;
			bos << src.m_MaxMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MaxMoney& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_MaxMoneyTime;
			bis >> src.m_MaxMoney;

			return bis;
		}
	};

	struct RWDB_AddLimite
	{
		enum { XY_ID = RWDB_XYID_AddLimite };

		short                    m_AID;
		unsigned int             m_PID;
		long long                m_nUpperLimite;
		long long                m_nLowerLimite;
		
		RWDB_AddLimite() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_AddLimite& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nUpperLimite;
			bos << src.m_nLowerLimite;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_AddLimite& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nUpperLimite;
			bis >> src.m_nLowerLimite;

			return bis;
		}
	};

	struct RWDB_AddPlayerAward
	{
		enum { XY_ID = RWDB_XYID_AddPlayerAward };

		short                    m_AID;
		unsigned int             m_PID;

		INT64                    m_Money;
		short                    m_nFlag;
		short                    m_PayMode;
		short                    m_CanUse;
		unsigned int             m_EndTime;
		std::string              m_strLog;

		RWDB_AddPlayerAward() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;

			m_Money = 0;
			m_nFlag = 0;
			m_PayMode = 0;
			m_CanUse = 0;
			m_EndTime = 0;
			m_strLog = "";
		}
		friend bostream& operator<<( bostream& bos, const RWDB_AddPlayerAward& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_Money;
			bos << src.m_nFlag;
			bos << src.m_PayMode;
			bos << src.m_CanUse;
			bos << src.m_EndTime;
			InString(bos,src.m_strLog,50);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_AddPlayerAward& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_Money;
			bis >> src.m_nFlag;
			bis >> src.m_PayMode;
			bis >> src.m_CanUse;
			bis >> src.m_EndTime;
			OutString(bis,src.m_strLog,50);

			return bis;
		}
	};

	struct RWDB_UpdateHuiYuan
	{
		enum { XY_ID = RWDB_XYID_UpdateHuiYuan };

		short                    m_AID;
		unsigned int             m_PID;

		UINT32                   m_FreeFaceTime;
		short                    m_VipLevel;
		UINT32                   m_VipEndTime;

		RWDB_UpdateHuiYuan() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_UpdateHuiYuan& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_FreeFaceTime;
			bos << src.m_VipLevel;
			bos << src.m_VipEndTime;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_UpdateHuiYuan& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_FreeFaceTime;
			bis >> src.m_VipLevel;
			bis >> src.m_VipEndTime;
			return bis;
		}
	};	

	struct RWDB_ChangeBankLog
	{
		enum { XY_ID = RWDB_XYID_ChangeBankLog };

		short                    m_AID;
		unsigned int             m_PID;

		short                    m_Flag;
		INT64                    m_Money;

		RWDB_ChangeBankLog() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_ChangeBankLog& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Flag;
			bos << src.m_Money;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_ChangeBankLog& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Flag;
			bis >> src.m_Money;

			return bis;
		}
	};

	struct RWDB_GameRight
	{
		enum { XY_ID = RWDB_XYID_GameRight };

		short                    m_AID;
		unsigned int             m_PID;
		long long                m_Right;

		RWDB_GameRight() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_GameRight& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Right;			

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_GameRight& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Right;

			return bis;
		}
	};

	struct RWDB_AddGameMoney
	{
		enum { XY_ID = RWDB_XYID_AddGameMoney };

		short                    m_AID;
		unsigned int             m_PID;
		long long                m_GameMoney;
		long long                m_HongBao;


		RWDB_AddGameMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_AddGameMoney& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_GameMoney;
			bos << src.m_HongBao;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_AddGameMoney& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_GameMoney;
			bis >> src.m_HongBao;
			return bis;
		}
	};

	struct RWDB_WriteUserGift
	{		
		enum { XY_ID = RWDB_XYID_WriteUserGift };
		
		UINT32               m_RecvPID;
		int                  m_GiftIdx;
		INT16                m_GiftID;
		INT32                m_Price;
		UINT32               m_SendPID;
		std::string          m_NickName;
		UINT32               m_ActionTime;

		RWDB_WriteUserGift() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_WriteUserGift& src )
		{
			bos << src.m_RecvPID;
			bos << src.m_GiftIdx;
			bos << src.m_GiftID;
			bos << src.m_Price;
			bos << src.m_SendPID;
			InString(bos,src.m_NickName,50);
			bos << src.m_ActionTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_WriteUserGift& src )
		{
			src.ReSet();
			
			bis >> src.m_RecvPID;
			bis >> src.m_GiftIdx;
			bis >> src.m_GiftID;
			bis >> src.m_Price;
			bis >> src.m_SendPID;
			OutString(bis,src.m_NickName,50);
			bis >> src.m_ActionTime;

			return bis;
		}
	};

	struct RWDB_DeleteCreateInfo
	{
		enum { XY_ID = RWDB_XYID_DeleteCreateInfo };

		short                    m_AID;
		unsigned int             m_PID;

		RWDB_DeleteCreateInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_DeleteCreateInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_DeleteCreateInfo& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			return bis;
		}
	};

	struct RWDB_UpdateWinType
	{
		enum { XY_ID = RWDB_XYID_UpdateWinType };

		short                    m_AID;
		unsigned int             m_PID;
		int                      m_WinRecord[MoGui::Game::DeZhou::MAX_PaiTypeNum];

		RWDB_UpdateWinType() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_UpdateWinType& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			for (int i = 0; i<MoGui::Game::DeZhou::MAX_PaiTypeNum; ++i)
			{
				bos << src.m_WinRecord[i];
			}
			
			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_UpdateWinType& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			for (int i = 0; i<MoGui::Game::DeZhou::MAX_PaiTypeNum; ++i)
			{
				bis >> src.m_WinRecord[i];
			}
			return bis;
		}
	};

	struct RWDB_AddPlayerInfo
	{
		enum { XY_ID = RWDB_XYID_AddPlayerInfo };
		enum { ShowFace=1,WinTimes,LossTimes,GameTime,InvitePlayer,OpenBank,LandTime, };

		short                    m_AID;
		unsigned int             m_PID;

		unsigned char            m_Flag;
		INT64                    m_AddValue;

		RWDB_AddPlayerInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_AddPlayerInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Flag;
			bos << src.m_AddValue;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_AddPlayerInfo& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Flag;
			bis >> src.m_AddValue;
			return bis;
		}
	};

	struct RWDB_HonorLog
	{
		enum { XY_ID = RWDB_XYID_HonorLog };

		short                    m_AID;
		unsigned int             m_PID;
		
		int                      m_HonorID;
		INT64                    m_Money;

		RWDB_HonorLog() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_HonorLog& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_HonorID;
			bos << src.m_Money;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_HonorLog& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_HonorID;
			bis >> src.m_Money;
			return bis;
		}
	};

	struct RWDB_MatchInfo
	{
		enum { XY_ID = RWDB_XYID_MatchInfo };

		short                       m_AID;
		unsigned int                m_PID;

		INT32                       m_TaoJinTimes;
		INT64                       m_TaoJinWinMoney;
		INT32                       m_TaoJinBest;
		UINT32                      m_TaoJinBestTime;		

		INT32                       m_JingBiaoTimes;
		INT64                       m_JingBiaoWinMoney;
		INT32                       m_JingBiaoBest;
		UINT32                      m_JingBiaoBestTime;	

		INT32                       m_GuanJunTimes;
		INT64                       m_GuanJunWinMoney;
		INT32                       m_GuanJunBest;
		UINT32                      m_GuanJunBestTime;

		RWDB_MatchInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_MatchInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_TaoJinTimes;
			bos << src.m_TaoJinWinMoney;
			bos << src.m_TaoJinBest;
			bos << src.m_TaoJinBestTime;

			bos << src.m_JingBiaoTimes;
			bos << src.m_JingBiaoWinMoney;
			bos << src.m_JingBiaoBest;
			bos << src.m_JingBiaoBestTime;

			bos << src.m_GuanJunTimes;
			bos << src.m_GuanJunWinMoney;
			bos << src.m_GuanJunBest;
			bos << src.m_GuanJunBestTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MatchInfo& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_TaoJinTimes;
			bis >> src.m_TaoJinWinMoney;
			bis >> src.m_TaoJinBest;
			bis >> src.m_TaoJinBestTime;

			bis >> src.m_JingBiaoTimes;
			bis >> src.m_JingBiaoWinMoney;
			bis >> src.m_JingBiaoBest;
			bis >> src.m_JingBiaoBestTime;

			bis >> src.m_GuanJunTimes;
			bis >> src.m_GuanJunWinMoney;
			bis >> src.m_GuanJunBest;
			bis >> src.m_GuanJunBestTime;

			return bis;
		}
	};

	struct RWDB_MatchResultLog
	{
		enum { XY_ID = RWDB_XYID_MatchResultLog };

		long long                 m_MatchGUID;
		short                     m_AID;
		unsigned int              m_PID;
		short                     m_RoomID;
		short                     m_TableID;
		int                       m_MatchID;
		int                       m_Ticket;
		int                       m_TakeMoney;
		long long                 m_AwardMoney;
		long long                 m_AwardJF;
		int                       m_Position;

		RWDB_MatchResultLog() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_MatchResultLog& src )
		{
			bos << src.m_MatchGUID;
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_MatchID;
			bos << src.m_Ticket;
			bos << src.m_TakeMoney;
			bos << src.m_AwardMoney;
			bos << src.m_AwardJF;
			bos << src.m_Position;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MatchResultLog& src )
		{
			src.ReSet();

			bis >> src.m_MatchGUID;
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_MatchID;
			bis >> src.m_Ticket;
			bis >> src.m_TakeMoney;
			bis >> src.m_AwardMoney;
			bis >> src.m_AwardJF;
			bis >> src.m_Position;

			return bis;
		}
	};

	struct RWDB_MatchWinLoss
	{
		enum { XY_ID = RWDB_XYID_MatchWinLoss };

		long long                 m_MatchGUID;
		long long                 m_GameGUID;
		short                     m_AID;
		unsigned int              m_PID;
		short                     m_RoomID;
		short                     m_TableID;
		unsigned char             m_SitID;
		int                       m_nWinLossMoney;
		short                     m_PaiType;
		short                     m_EndGameFlag;

		RWDB_MatchWinLoss() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_MatchWinLoss& src )
		{
			bos << src.m_MatchGUID;
			bos << src.m_GameGUID;
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_SitID;
			bos << src.m_nWinLossMoney;
			bos << src.m_PaiType;
			bos << src.m_EndGameFlag;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MatchWinLoss& src )
		{
			src.ReSet();

			bis >> src.m_MatchGUID;
			bis >> src.m_GameGUID;
			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_SitID;
			bis >> src.m_nWinLossMoney;
			bis >> src.m_PaiType;
			bis >> src.m_EndGameFlag;

			return bis;
		}
	};

	struct RWDB_UpdateUserGameMoneyType
	{
		enum { XY_ID = RWDB_XYID_UpdateUserGameMoneyType };

		short                     m_AID;
		unsigned int              m_PID;

		long long                 m_nWinMoney;
		long long                 m_nLossMoney;
		long long                 m_nServiceMoney;
		long long                 m_nGivenMoney;
		long long                 m_nIncomeMoney;
		long long                 m_nExpendMoney;

		RWDB_UpdateUserGameMoneyType() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_UpdateUserGameMoneyType& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_nWinMoney;
			bos << src.m_nLossMoney;
			bos << src.m_nServiceMoney;
			bos << src.m_nGivenMoney;
			bos << src.m_nIncomeMoney;
			bos << src.m_nExpendMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_UpdateUserGameMoneyType& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_nWinMoney;
			bis >> src.m_nLossMoney;
			bis >> src.m_nServiceMoney;
			bis >> src.m_nGivenMoney;
			bis >> src.m_nIncomeMoney;
			bis >> src.m_nExpendMoney;

			return bis;
		}
	};

	struct RWDB_MakeFriend
	{
		enum { XY_ID = RWDB_XYID_MakeFriend };

		UINT32                      m_LeftPID;
		UINT32                      m_RightPID;
		INT16                       m_Flag;

		RWDB_MakeFriend() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_MakeFriend& src )
		{			
			bos << src.m_LeftPID;
			bos << src.m_RightPID;
			bos << src.m_Flag;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MakeFriend& src )
		{
			src.ReSet();

			bis >> src.m_LeftPID;
			bis >> src.m_RightPID;
			bis >> src.m_Flag;

			return bis;
		}
	};

	struct RWDB_SendMoney
	{
		enum { XY_ID = RWDB_XYID_SendMoney };

		short              m_SendAID;
		UINT32             m_SendPID;
		short              m_RecvAID;
		UINT32             m_RecvPID;
		int                m_nMoney;
		int                m_Service;

		RWDB_SendMoney(){ReSet();}
		void ReSet(){ memset(this,0,sizeof(*this));}		

		friend bostream& operator<<( bostream& bos, const RWDB_SendMoney& src )
		{
			bos << src.m_SendAID;
			bos << src.m_SendPID;
			bos << src.m_RecvAID;
			bos << src.m_RecvPID;
			bos << src.m_nMoney;
			bos << src.m_Service;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_SendMoney& src )
		{
			src.ReSet();

			bis >> src.m_SendAID;
			bis >> src.m_SendPID;
			bis >> src.m_RecvAID;
			bis >> src.m_RecvPID;
			bis >> src.m_nMoney;
			bis >> src.m_Service;

			return bis;
		}
	};	

	struct RWDB_IncomeAndPay
	{
		enum { XY_ID = RWDB_XYID_IncomeAndPay };

		INT16              m_AID;
		UINT32             m_PID;
		INT64              m_nMoney;		
		INT16              m_Flag;
		std::string        m_strReMark;

		RWDB_IncomeAndPay(){ReSet();}
		void ReSet()
		{ 
			m_AID = 0;
			m_PID = 0;
			m_nMoney = 0;
			m_Flag = 0;
			m_strReMark = "";
		}

		friend bostream& operator<<( bostream& bos, const RWDB_IncomeAndPay& src )
		{			
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_nMoney;
			bos << src.m_Flag;
			InString(bos,src.m_strReMark,MAX_MSG_LEN);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_IncomeAndPay& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_nMoney;
			bis >> src.m_Flag;
			OutString(bis,src.m_strReMark,MAX_MSG_LEN);

			return bis;
		}
	};

	struct RWDB_PlayerActionLog
	{
		enum { XY_ID = RWDB_XYID_PlayerActionLog };

		short                    m_AID;
		UINT32                   m_PID;

		int                      m_OneLifeTotalTime;
		int                      m_OneLifeGameTime;
		int                      m_OneLifeMatchTime;
		short                    m_EnterRoomTimes;
		short                    m_EnterTableTimes;
		short                    m_SitDownTimes;
		short                    m_PlayTimes;

		RWDB_PlayerActionLog(){ ReSet();}
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_PlayerActionLog& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_OneLifeTotalTime;
			bos << src.m_OneLifeGameTime;		
			bos << src.m_OneLifeMatchTime;
			bos << src.m_EnterRoomTimes;
			bos << src.m_EnterTableTimes;
			bos << src.m_SitDownTimes;
			bos << src.m_PlayTimes;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_PlayerActionLog& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_OneLifeTotalTime;
			bis >> src.m_OneLifeGameTime;
			bis >> src.m_OneLifeMatchTime;
			bis >> src.m_EnterRoomTimes;
			bis >> src.m_EnterTableTimes;
			bis >> src.m_SitDownTimes;
			bis >> src.m_PlayTimes;

			return bis;
		}
	};

	struct RWDB_GameError
	{
		enum { XY_ID = RWDB_XYID_GameError };

		short                    m_AID;
		UINT32                   m_PID;
		int                      m_Flag;
		std::string              m_Key;
		std::string              m_Des;

		RWDB_GameError(){ ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<(bostream& bos, const RWDB_GameError& src)
		{
			bos << src.m_AID;
			bos << src.m_PID;			
			bos << src.m_Flag;
			InString(bos, src.m_Key, 50);
			InString(bos, src.m_Des, 255);

			return bos;
		}
		friend bistream& operator>>(bistream& bis, RWDB_GameError& src)
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;			
			bis >> src.m_Flag;
			OutString(bis, src.m_Key, 50);
			OutString(bis, src.m_Des, 255);

			return bis;
		}
	};

	struct RWDB_ChatLog
	{
		enum { XY_ID = RWDB_XYID_ChatLog };

		unsigned int             m_PID;
		unsigned int             m_ToPID;

		short                    m_ServerID;
		short                    m_RoomID;
		short                    m_TableID;
		BYTE                     m_SitID;

		std::string              m_ChatMsg;

		RWDB_ChatLog(){ ReSet();}
		void ReSet()
		{
			m_PID = 0;
			m_ToPID = 0;

			m_ServerID = 0;
			m_RoomID = 0;
			m_TableID = 0;
			m_SitID = 0;

			m_ChatMsg = "";
		}

		friend bostream& operator<<( bostream& bos, const RWDB_ChatLog& src )
		{
			bos << src.m_PID;
			bos << src.m_ToPID;

			bos << src.m_ServerID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_SitID;

			InString(bos,src.m_ChatMsg,MAX_TABLECHAT_SIZE);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_ChatLog& src )
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_ToPID;

			bis >> src.m_ServerID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_SitID;

			OutString(bis,src.m_ChatMsg,MAX_TABLECHAT_SIZE);

			return bis;
		}
	};

	struct RWDB_TableInfo
	{
		enum { XY_ID = RWDB_XYID_TableInfo };

		INT16                		m_ServerID;
		INT16                		m_RoomID;
		INT16                		m_TableID;
		UINT32               		m_FoundByWho;

		UINT32               		m_EndTime;

		std::string                 m_TableName;
		std::string                 m_Password;
		std::string                 m_TableRule;
		std::string                 m_TableRuleEX;
		std::string                 m_TableMatchRule;
		
		RWDB_TableInfo(){ReSet();}
		void ReSet()
		{
			m_ServerID = 0;
			m_RoomID = 0;
			m_TableID = 0;
			m_FoundByWho = 0;

			m_TableName = "";
			m_Password = "";
			m_TableRule = "";
			m_TableRuleEX = "";
			m_TableMatchRule = "";

			m_EndTime = 0;
		}
		
		friend bostream& operator<<( bostream& bos, const RWDB_TableInfo& src )
		{
			bos << src.m_ServerID;
			bos << src.m_RoomID;
			bos << src.m_TableID;
			bos << src.m_FoundByWho;
			bos << src.m_EndTime;

			InString(bos,src.m_TableName,MAX_TABLENAME_SIZE);
			InString(bos,src.m_Password,TABLE_PASSWORD_SIZE);
			InString(bos,src.m_TableRule,MAX_RULE_SIZE);
			InString(bos,src.m_TableRuleEX,MAX_RULE_SIZE);
			InString(bos,src.m_TableMatchRule,MAX_RULE_SIZE);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_TableInfo& src )
		{
			src.ReSet();

			bis >> src.m_ServerID;
			bis >> src.m_RoomID;
			bis >> src.m_TableID;
			bis >> src.m_FoundByWho;
			bis >> src.m_EndTime;

			OutString(bis,src.m_TableName,MAX_TABLENAME_SIZE);
			OutString(bis,src.m_Password,TABLE_PASSWORD_SIZE);
			OutString(bis,src.m_TableRule,MAX_RULE_SIZE);
			OutString(bis,src.m_TableRuleEX,MAX_RULE_SIZE);
			OutString(bis,src.m_TableMatchRule,MAX_RULE_SIZE);

			return bis;
		}
	};

	struct RWDB_UpdateGiftInfo
	{
		enum { XY_ID = RWDB_XYID_UpdateGiftInfo };

		unsigned int              m_PID;
		int                       m_GiftIdx;
		short                     m_Flag;

		RWDB_UpdateGiftInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_UpdateGiftInfo& src )
		{
			bos << src.m_PID;
			bos << src.m_GiftIdx;
			bos << src.m_Flag;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_UpdateGiftInfo& src )
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_GiftIdx;
			bis >> src.m_Flag;

			return bis;
		}
	};

	struct RWDB_UpdateAwardInfo
	{
		enum { XY_ID = RWDB_XYID_UpdateAwardInfo };

		int                       m_AwardIdx;
		short                     m_Flag;

		RWDB_UpdateAwardInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_UpdateAwardInfo& src )
		{			
			bos << src.m_AwardIdx;
			bos << src.m_Flag;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_UpdateAwardInfo& src )
		{
			src.ReSet();

			bis >> src.m_AwardIdx;
			bis >> src.m_Flag;

			return bis;
		}
	};

	struct RWDB_UpdateRightInfo
	{
		enum { XY_ID = RWDB_XYID_UpdateRightInfo };

		int                       m_RightIdx;
		short                     m_CanUse;

		RWDB_UpdateRightInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_UpdateRightInfo& src )
		{			
			bos << src.m_RightIdx;
			bos << src.m_CanUse;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_UpdateRightInfo& src )
		{
			src.ReSet();

			bis >> src.m_RightIdx;
			bis >> src.m_CanUse;

			return bis;
		}
	};

	struct RWDB_WriteRightInfo
	{
		enum { XY_ID = RWDB_XYID_WriteRightInfo };

		short                     m_AID;
		unsigned int              m_PID;

		int                       m_Right;
		short                     m_Times;
		short                     m_CanUse;
		UINT32                    m_EndTime;

		RWDB_WriteRightInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_WriteRightInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_Right;
			bos << src.m_Times;
			bos << src.m_CanUse;
			bos << src.m_EndTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_WriteRightInfo& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_Right;
			bis >> src.m_Times;
			bis >> src.m_CanUse;
			bis >> src.m_EndTime;

			return bis;
		}
	};

	struct RWDB_UpdateAchieve
	{
		enum { XY_ID = RWDB_XYID_UpdateAchieve };

		short                     m_AID;
		unsigned int              m_PID;

		BYTE                      m_HonorType;
		int                       m_HonorValue;

		RWDB_UpdateAchieve() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_UpdateAchieve& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			bos << src.m_HonorType;
			bos << src.m_HonorValue;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_UpdateAchieve& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			bis >> src.m_HonorType;
			bis >> src.m_HonorValue;		

			return bis;
		}
	};

	struct RWDB_ChangeUserInfo
	{
		enum { XY_ID = RWDB_XYID_ChangeUserInfo };

		short                     m_AID;
		unsigned int              m_PID;

		int                       m_Sex;
		short                     m_Year;
		std::string               m_NickName;
		std::string               m_HeadPicUrl;
		std::string               m_City;

		RWDB_ChangeUserInfo() { ReSet(); }
		void ReSet()
		{
			m_AID = 0;
			m_PID = 0;

			m_Sex = 0;
			m_Year = 0;

			m_NickName = "";
			m_HeadPicUrl = "";
			m_City = "";
		}

		friend bostream& operator<<( bostream& bos, const RWDB_ChangeUserInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;

			bos << src.m_Sex;
			bos << src.m_Year;

			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HeadPicUrl,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_ChangeUserInfo& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;

			bis >> src.m_Sex;
			bis >> src.m_Year;

			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HeadPicUrl,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			return bis;
		}
	};

	struct RWDB_PlayerLogin
	{
		enum { XY_ID = RWDB_XYID_Login };

		short                     m_AID;
		unsigned int              m_PID;
		std::string               m_IP;

		RWDB_PlayerLogin() { ReSet(); }
		void ReSet(){ m_AID=0;m_PID=0;m_IP="";}

		friend bostream& operator<<( bostream& bos, const RWDB_PlayerLogin& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			InString(bos,src.m_IP,20);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_PlayerLogin& src )
		{
			src.ReSet();

			bis >> src.m_AID;
			bis >> src.m_PID;
			OutString(bis,src.m_IP,20);

			return bis;
		}
	};

	struct RWDB_CreateGameInfo
	{
		enum { XY_ID = RWDB_XYID_CreateGameInfo };

		short                     m_AID;
		unsigned int              m_PID;

		RWDB_CreateGameInfo() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_CreateGameInfo& src )
		{
			bos << src.m_AID;
			bos << src.m_PID;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_CreateGameInfo& src )
		{
			src.ReSet();
			bis >> src.m_AID;
			bis >> src.m_PID;
			return bis;
		}
	};

	struct RWDB_ReportPlayerOnline
	{
		enum { XY_ID = RWDB_XYID_ReportPlayerOnline };

		short                     m_ServerID;
		short                     m_AID;
		short                     m_Total;
		short                     m_Lobby;
		short                     m_Table;
		short                     m_Game;
		short                     m_Match;

		RWDB_ReportPlayerOnline() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const RWDB_ReportPlayerOnline& src )
		{
			bos << src.m_ServerID;
			bos << src.m_AID;
			bos << src.m_Total;
			bos << src.m_Lobby;
			bos << src.m_Table;
			bos << src.m_Game;
			bos << src.m_Match;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_ReportPlayerOnline& src )
		{
			src.ReSet();

			bis >> src.m_ServerID;
			bis >> src.m_AID;
			bis >> src.m_Total;
			bis >> src.m_Lobby;
			bis >> src.m_Table;
			bis >> src.m_Game;
			bis >> src.m_Match;

			return bis;
		}
	};

	struct RWDB_MoGuiMoneyLog
	{
		enum { XY_ID = RWDB_XYID_MoGuiMoneyLog };

		UINT32                                m_PID;
		INT16                                 m_AID;
		int                                   m_Money;
		INT16                                 m_Flag;
		std::string                           m_strLog;

		RWDB_MoGuiMoneyLog(){ReSet();}
		void ReSet()
		{
			m_PID = 0;
			m_AID = 0;
			m_Money = 0;
			m_Flag = 0;
			m_strLog = "";
		}

		friend bostream& operator<<( bostream& bos, const RWDB_MoGuiMoneyLog& src )
		{			
			bos << src.m_PID;
			bos << src.m_AID;
			bos << src.m_Money;
			bos << src.m_Flag;
			InString(bos,src.m_strLog,MAX_MSG_LEN);

			return bos;
		}
		friend bistream& operator>>( bistream& bis, RWDB_MoGuiMoneyLog& src )
		{
			src.ReSet();

			bis >> src.m_PID;
			bis >> src.m_AID;
			bis >> src.m_Money;
			bis >> src.m_Flag;
			OutString(bis,src.m_strLog,MAX_MSG_LEN);

			return bis;
		}
	};
};
}
}