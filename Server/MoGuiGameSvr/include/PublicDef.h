#pragma once

#include <Windows.h>
//#pragma warning( disable : 4067 )

namespace MoGui
{
	namespace N_AREAID
	{
		const short AREAID_CESHI               = 1;                    //测试玩家
		const short AREAID_BOT                 = 2;                    //机器人玩家
		const short AREAID_VISITOR             = 3;                    //游客玩家
	}

	namespace N_Time
	{
		const int Minute_1                      = 60;
		const int Hour_1                        = 3600;
		const int Day_1                         = 86400;
		const int DAY_2                         = Day_1*2;
		const int Week_1                        = 604800;
		const int Month_1                       = 2592000;
		const int Month_2                       = 5184000;
		const int Year_1                        = Day_1*365;

		const int Second_MoGuiBegin             = 1340000000;

		const unsigned int Max_Time             = UINT32(-1);

		//下面是程序里面各种检测的时间，用来支持游戏正常进行的内容的
		const short t_FlashConnect              = 5;           //检测FLASH安全策略后连接的时间
		const short t_PlayerActive              = 45;          //检测玩家心跳包的时间
		const short t_SocketActive              = 180;
		const short t_PlayerOffLine             = 180;
		const short t_SocketStopTime            = 10;

		const short t_CheckRoom                 = 1;           //检测房间的时间
		const short t_KeepPlayerData            = 600;         //保留下线玩家对象的时间值
		const short t_CheckAddFriend            = 300;
		const short t_KeepAddFriend             = 3600;
		const short T_CheckCityPlayer           = 10;
		const short T_CheckFriendPlayer         = 20;

		const short T_MatchAddBlind             = 300;
	}

	enum ClientClass   //只记录主动发起方的类型
	{
		CLIENT_CLASS_NONE,
		CLIENT_CLASS_GAMECLIENT = 10,
		CLIENT_CLASS_GAMESERVER = 20,
		CLIENT_CLASS_LOGINSERVER = 30
	};

	enum Sex
	{
		Sex_None     = 0,
		Sex_MAIL     = 1,
		Sex_FEMAIL   = 2,
	};

	namespace N_Friend
	{
		const BYTE None      = 0;     //没有关注
		const BYTE Follow    = 1;     //我关注的人
		const BYTE BeFollow  = 2;     //关注我的人
		const BYTE Both      = 3;     //相互关注的人

		const short     MAX_FOLLOW      = 300;
		const short     MAX_BEFOLLOW    = 2000;

		extern inline  BYTE GetAnti(BYTE flag)
		{
			if ( flag>Both || flag<= None)
			{
				return None;
			}
			else if ( flag == Follow)
			{
				return BeFollow;
			}
			else if( flag == BeFollow )
			{
				return Follow;
			}
			else
			{
				return Both;
			}
		}
	};
	
	enum ClientType
	{
		CLIENT_TYPE_PC = 11,
		CLIENT_TYPE_WEB = 21,
		CLIENT_TYPE_FLASH = 51,
		CLIENT_TYPE_MOBILE = 71,
		CLIENT_TYPE_PAD = 91,
		CLIENT_TYPE_TV = 111,
	};

	enum LoginType
	{
		Login_Type_None = 0,
		Login_Type_Password = 11,
		Login_Type_Session = 21,
		Login_Type_Relink = 31,
		Login_Type_CeShi = 101,
	};

	enum PlayerType
	{
		PLAYER_TYPE_NONE,
		PLAYER_TYPE_PLAYER,                         //普通玩家
		PLAYER_TYPE_BOT,             				//机器人
		PLAYER_TYPE_VISITOR,                        //游客帐号
		PLAYER_TYPE_ADMIN,                          //管理员
		PLAYER_TYPE_ENDS
	};

	

	enum ChangeInfo
	{
		ChangeInfo_NickName = 1,
		//ChangeInfo_Year     = 2,
		ChangeInfo_Sex      = 4,
		ChangeInfo_HeadPic  = 8,
		ChangeInfo_City     = 16,
	};

	enum MsgError
	{
		SUCCESS = 0,

		SOCKET_MSG_ERROR_STREAM   = 10001,               //协议流出错
		SOCKET_MSG_ERROR_NOSOCKET = 10002,
		MSG_ERROR_NOSERVER        = 10003,
		MSG_ERROR_NOPLAYER        = 10004,
		MSG_ERROR_STEPERROR       = 10005,
		MSG_ERROR_DBNOTUSED       = 10006,
		MSG_ERROR_LOGIC           = 10100,
		SOCKET_MSG_ERROR_CLOSE    = 11000,
	};

	enum ShowMsgFlag
	{
		ShowMsg_None,
		ShowMsg_Text,
		ShowMsg_Show,
	};

	enum DBOperateError
	{
		DB_RESULT_SUCCESS,
		DB_RESULT_DBERROR,
		DB_RESULT_NOPLAYER,
		DB_RESULT_NOSERVER,
		DB_RESULT_NOGIFTINFO,
		DB_RESULT_NOFACEINFO,
		DB_RESULT_PlayerForbid,
		DB_RESULT_NumberError,
	};

	enum eMoneyType                      //货币的种类
	{
		MONEYTYPE_MONEY,
		MONEYTYPE_JIFEN,
	};

	enum ePayMode
	{
		PM_OneTime = 1,                 //只用一次用完就删掉
		PM_EveryDay = 2                 //每天一次 
	};

	
	//记录错误标识
	const int GameError_PlayerClientError                 = 1;
	const int GameError_AddMoney                          = 101;
	const int GameError_AddMoguiMoney                     = 201;
	const int GameError_ChangeUserInfo                    = 301;

	namespace N_IncomeAndPay
	{
		//1000以下为单独增加 3000以上为单独减 1000到3000之间为即可以增也可以减，其中1000到2000间为增加 2000到3000间为减少 而且必须要有一一对应关系且相差1000的差值
		//以正负值来计算 2000以下为正 2000以上为负
		const short   JoinAward       = 10;                       //注册奖励
		const short   LandAward       = 11;                       //登陆奖励
		
		const short   FenHongAward    = 13;                       //分红奖励
		const short   InviteAward     = 14;                       //邀请玩家奖励

		const short   UpdateZiLiao    = 21;                       //完善资料奖励

		const short   FinishHonor     = 31;                       //完成成就奖励
		const short   FinishTask      = 33;                       //完成任务奖励
		const short   GameLevel       = 35;                       //玩家升级奖励

		const short   TimePromote     = 41;                       //在线游戏活动系统向玩家送游戏币
		const short   JiuMingQian     = 42;                       //向玩家发的救命钱

		const short   PayMoney        = 101;                      //充值
		const short   PayMoneyAward   = 102;                      //充值奖励

		const short   Mogui_PayMoney  = 201;                      //向系统充值充成金币

		const short   SysAdd          = 1001;                     //系统增加或减少，人工添加或程序添加等
		const short   SysDec          = 2001;

		const short   BotAdd          = 1002;                     //机器人的游戏币增加或者减少
		const short   BotDec          = 2002;
	
		const short   Win             = 1021;                     //比赛结果的输赢
		const short   Loss            = 2021;

		const short   MatchAward      = 1026;                     //比赛奖金
		const short   MatchTicket     = 2026;                     //比赛门票		

		const short   RecvMoney       = 1031;                     //玩家间相送赠送少量的游戏币
		const short   SendMoney       = 2031;

		const short   SoldGift        = 1036;                     //礼物买卖
		const short   SendGift        = 2036;

		const short   BankToGame      = 1041;                     //银行到游戏
		const short   GameToBank      = 2041;                     //游戏到银行

		const short   JuBaoPengAward  = 1051;                     //聚宝盆获奖
		const short   JuBaoPengChip   = 2051;                     //聚宝盆下注

		const short   ShowFace        = 3035;                     //发表情
		const short   CreateTable     = 3041;                     //创建桌子付的费用
		const short   AddTableTime    = 3042;                     //延长桌子时间
	}

	namespace N_Gift
	{
		enum
		{
			GiftTime_Cur=1,   //当期
			GiftTime_Out=2,   //过期
		};

		enum
		{
			MaxCurGift = 60,
			MaxLastGift = 240,
			MaxGiftCount = 300,
		};

		enum 
		{
			GiftST_None,
			GiftST_OnSale,
			GiftST_Solded,
			GiftST_OutTime,
		};

		enum
		{
			PriceFlag_Chang = 1,
			PriceFlag_Fixed = 2,
			PriceFlag_MoGui = 3,
		};

		const int M2G_Rate = 10000;
	}
}