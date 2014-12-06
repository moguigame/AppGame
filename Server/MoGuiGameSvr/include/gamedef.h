#pragma once

#include <Windows.h>

namespace MoGui
{
	namespace Game
	{
		namespace N_CeShiLog
		{
			//DB专用
			const int   c_NoCheckLogin        = 1;               //不检测用户登陆认证
			const int   c_RWDBLog             = 1;               //记录RWDB运行效率速度等情况
			const int   c_DBOnTimeDiff        = 600;             //多少间隔打印一次运行时间,单位秒
			const int   c_DBMaxOnTime         = 100;             //当运行时间大于多少的时候打印一次

			//GS专用
			const int   c_BotPlayerLeave      = 0;               //机器人随机离开
			const int   c_PlayerAction        = 1;               //玩家的进出房间桌子以及坐下
			const int   c_CityPlayer          = 1;               //同城相关的记录
			const int   c_GSOnTimeDiff        = 600;              //多少间隔打印一次运行时间,单位秒
			const int   c_GSMaxOnTime         = 500;             //当运行时间大于多少的时候打印一次

			//通用
			const int   c_RightLog            = 1;               //测试玩家RIGHT变化的地方
			const int   c_FuncTimeLog         = 1;               //记录函数运行时间的统计
			const int   c_MoneyLog            = 1;               //记录和moneylog相关的内容
			const int   c_XieYiLog            = 1;               //对游戏的协议进行分类统计
			const int   c_NewMomory           = 1;               //测试动态内存,有动态分配的类统计
		}

		namespace DeZhou
		{
			const short    MAX_PALYER_ON_TABLE      = 9;                   //本桌最多一起游戏人数
			const short    MIN_PLAYER_FOR_GAME      = 2;                   //开始游戏的最少玩家
			const short    MAX_PLAYER_IN_TABLE      = 250;                 //本桌旁观加游戏人数不能超过255
			const short    MAX_TABLE_IN_ROOM        = 1000;                //房间内的最大桌子数量
			const short    MAX_PLAYER_IN_ROOM       = 10000;               //房间内的最大人数
			const short    MAX_TALBE_ID             = 9999;
			const short    MAX_PaiTypeNum           = 10;

			const int      Max_PlayerData_In_DBS    = 200000;

			const int      Max_Player_Level         = 9;
			const int      Min_Player_Level         = 1;

			const int      Max_RecommendFriend      = 50;                  //系统自动推荐朋友总数

			const short    HAND_CARDS               = 2;
			const short    TABLE_CARDS              = 5;
			const short    RESULT_CARDS             = 5;
			const short    SELECT_CARDS             = HAND_CARDS+TABLE_CARDS;

			const short    XIPAI_CARDS              = 52;                  //每一轮的牌数
			const short    XIPAI_POOLS              = 200;                 //所有牌的池子数

			const short    T_ChipTime_Long          = 20;
			const short    T_ChipTime_Mid           = 15;
			const short    T_ChipTime_Quick         = 10;
			const short    T_WaitStartTime          = 2;
			const short    T_ReqMoney               = 60;
			const short    T_MaxSocketOutTimes      = 3;

			const short    Max_City_Player          = 50;
			const short    T_MinCityTime            = 300;
			const short    T_MaxCityTime            = 600;

			const short    TIME_CHECK_SERVERACTIVE  = 10;
			const short    TIME_SERVER_ONLINE       = 30;

			const int      MAX_Blind                = 1000000;

			enum PAIVALUE
			{
				PAI_VALUE_NONE = 0,
				PAI_VALUE_BACK = 55,
			};

			enum PAYTYPE
			{
				Pay_Type_TableMoney,
				Pay_Type_GameMoney,
			};

			enum MatchType
			{
				Match_Type_TaoJin   = 1,                    //淘汰赛
				Match_Type_JingBiao = 2,                    //锦标赛
				Match_Type_GuanJun  = 3,                    //冠军赛
			};

			enum ROOMTYPE
			{
				ROOM_TYPE_Common             = 1,          //桌间模式是普通的还是排队性质的
				ROOM_TYPE_TableMatch         = 2,         //单桌比赛,
				ROOM_TYPE_RoomMatch          = 3,         //整个房间的比赛，主要是组织大型比赛				
			};
			
			enum PLACE
			{
				Place_TaoJin  = 101,
				Place_GuanJun = 111,
				Place_JinBiao = 121,
			};

			enum MATCHROOMSTATE
			{
				MatchRoom_ST_None = 0,
				MatchRoom_ST_WaitSign = 1,
				MatchRoom_ST_HaveSign,
				MatchRoom_ST_WaitEnter,
				MatchRoom_ST_MatchStart,
				MatchRoom_ST_MatchOver,
			};

			enum MATCHTABLESTATE
			{
				MatchTable_ST_None = 0,
				MatchTable_ST_WaitSign = 1,
				MatchTable_ST_Matching,
				MatchTable_ST_MatchOver,
			};

			enum TABLELIMITE
			{
				TABLE_LIMITE_NO = 1,                       //无限桌
				TABLE_LIMITE_TEXAS = 2,                    //有限下注桌 即是限注桌
				TABLE_LIMITE_POT = 3,                      //限池桌
			};

			enum TABLEPOT
			{
				TABLE_POT_NO = 1,
				TABLE_POT_YES
			};

			enum TABLETYPE
			{
				TABLE_TYPE_COMMON = 1,     				   //普通桌
				TABLE_TYPE_TableMatch,
				TABLE_TYPE_RoomMatch,
				TABLE_TYPE_END,
			};

			enum TABLEBOT
			{
				Table_Bot_None = 0,
				Table_Bot_Keep = 1,
				Table_Bot_Flow = 2,
			};

			enum MTACHTYPE
			{
				MatchType_None = 0,
				MatchType_TaoJin = 1,
				MatchType_JinBiao = 2,
				MatchType_GuanJun = 3,
			};

			enum TBALESTATE
			{
				TABLE_ST_NONE,             					//没有人坐着，旁观的并不算，
				TABLE_ST_WAIT,             					//等着开始，有可能是人不足，也有可能是两局中间的短暂等待期
				TABLE_ST_PLAYING,          					//游戏中
				TABLE_ST_END,                               //桌子游戏结束，针对比赛场以及自建房间场而言
			};

			enum GAMESTATE
			{
				GAME_ST_NONE,                               //人数为0时的状态
				GAME_ST_WAIT,                               //人数不足或两局中间的等待
				GAME_ST_HAND,                               //发过HAND牌后的状态，余下依次类推
				GAME_ST_FLOP,
				GAME_ST_TURN,
				GAME_ST_RIVER,
				GAME_ST_DIVIDEMONEY,                        //分派筹码的过程
				GAME_ST_END
			};

			enum GAMEPLAYERSTATE                           //玩家在游戏逻辑中的状态，和桌子上有区别
			{
				GAME_PLAYER_ST_NULL,
				GAME_PLAYER_ST_WAITING,                    //等待游戏，钱不足没有加入
				GAME_PLAYER_ST_GIVEUP,                     //游戏中弃牌
				GAME_PLAYER_ST_ALLIN,                      //游戏中ALL IN
				GAME_PLAYER_ST_PLAYING,		               //正在游戏中
				GAME_PLAYER_ST_WASHOUT,                    //比赛被淘汰出去
			};

			enum PLAYERSTATE
			{
				PLAYER_ST_NONE,             				//表示玩家刚创建或者无效
				PLAYER_ST_LOBBY,                            //表示玩家在大厅里
				PLAYER_ST_ROOM,            				    //在房间里站着,
				PLAYER_ST_SEEING,           				//旁观
				PLAYER_ST_WAIT,             				//在桌上等待其它玩家的加入或者待开始
				PLAYER_ST_PALYING,           				//正在游戏中，此时桌上玩害大于或者2个
				PLAYER_ST_END
			};

			enum MaxChip
			{
				MAXCHIP_HAND,               				//起手牌下注
				MAXCHIP_FLOP,               				//翻牌下注
				MAXCHIP_TURN,               				//转牌下注
				MAXCHIP_RIVER,              				//河牌下注，也是最后的下注机会
				MAXCHIP_END
			};

			enum ChipAction
			{
				ACTION_NONE,
				ACTION_Chip = 1,                            //下注，指一轮的第一个玩家
				ACTION_Follow = 2,                          //跟注
				ACTION_AddChip = 4,                         //加注
				ACTION_KanPai = 8,                          //看牌
				ACTION_GiveUp = 16,                         //放弃
				ACTION_AllIn = 32,                          //全下，也可能全下后仍然不足基本额
				ACTION_POT = 64,                            //底注
				ACTION_SMALLBLIND = 128,                    //小盲注
				ACTION_BIGBLIND = 256,                      //大盲注
				ACTION_NewBlind = 512,                      //新手盲注

				ACTION_WaitChip = 8912,                     //等待下注
			};

			enum eTallyFlag
			{
				TALLY_NONE,
				TALLY_WIN,
				TALLY_LOSS,
				TALLY_GIVEUP,             //弃牌
				TALLY_ESCAPE,             //直接逃走
				TALLY_GIVEUP_ESCAPE,      //先放弃后逃走
				TALLY_SYS,                //系统回收
			};

			enum ActionFlag
			{
				Action_Flag_Player = 0,        //玩家自己请求的信息
				Action_Flag_SysLogic,          //系统根据逻辑让玩家的动作，比如重连，加机器人,输光站起等     
				Action_Flag_Match,             //比赛中的分配，此时不能导制玩家比赛退出等
			};

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

			namespace N_JuBaoPeng
			{
				static const int cs_LowZhuChip         = 1;
				static const int cs_TopZhuChip         = 1000000;
				static const int cs_JuBaoPengFlagMoney = 10000000;
			};			

			enum
			{
				HandPai_Level_1 = 1,
				HandPai_Level_2 = 2,
				HandPai_Level_3 = 3,
				HandPai_Level_4 = 4
			};

			enum PlayerClientError
			{
				PlayerClient_Error = 1,
				PlayerClient_LogicError           = 100,
				PlayerClient_MsgCountOver         = 101,
				PlayerClient_MsgStreamError       = 102,
				PlayerClient_StepError            = 103,
			};

			static const UINT32 S_Uint32_Number[32] = 
			{
				1,
				2,
				4,
				8,
				16,
				32,
				64,
				128,
				256,
				512,
				1024,
				2048,
				4096,
				8192,
				16384,
				32768,
				65536,
				131072,
				262144,
				524288,
				1048576,
				2097152,
				4194304,
				8388608,
				16777216,
				33554432,
				67108864,
				134217728,
				268435456,
				536870912,
				1073741824,
				2147483648
			};
			static const UINT64 S_Uint64_Number[64] =
			{
				1,
				2,
				4,
				8,
				16,
				32,
				64,
				128,
				256,
				512,
				1024,
				2048,
				4096,
				8192,
				16384,
				32768,
				65536,
				131072,
				262144,
				524288,
				1048576,
				2097152,
				4194304,
				8388608,
				16777216,
				33554432,
				67108864,
				134217728,
				268435456,
				536870912,
				1073741824,
				2147483648,
				4294967296,
				8589934592,
				17179869184,
				34359738368,
				68719476736,
				137438953472,
				274877906944,
				549755813888,
				1099511627776,
				2199023255552,
				4398046511104,
				8796093022208,
				17592186044416,
				35184372088832,
				70368744177664,
				140737488355328,
				281474976710656,
				562949953421312,
				1125899906842624,
				2251799813685248,
				4503599627370496,
				9007199254740992,
				18014398509481984,
				36028797018963968,
				72057594037927936,
				144115188075855872,
				288230376151711744,
				576460752303423488,
				1152921504606846976,
				2305843009213693952,
				4611686018427387904,
				9223372036854775808
			};

			namespace N_Honor
			{
				enum HonorResult
				{
					HonorResult_False = 0,
					HonorResult_True  = 1,
					HonorResult_Error = 2,
				};

				enum HonorSort
				{
					HonorSort_Tong = 0,
					HonorSort_Ying = 1,
					HonorSort_Jing = 2,
					HonorSort_Zuan = 3,
				};

				const int Honor_Full                       = 0x40000000;

				const int First_Play100                    = 1;    //牌场新人，打满100场游戏
				const int First_Win10                      = 2;    //小试牛刀，取得10场胜利
				const int First_Send20                     = 3;    //社交新人，送出20份礼物
				const int First_Recv20                     = 4;    //广交朋友，收到20份礼物
				const int First_ShowFace100                = 5;    //有说有笑，发出100次表情
				const int First_Friend1                    = 6;    //至少有你，有一个牌友
				const int Frist_WinJuBaoPeng               = 8;    //铜聚宝盆，以三条及以上牌型获得一次聚宝盆奖励

				const int First_ThreeWin1                  = 11;    //三条，以三条获得一场胜利
				const int First_StrWin1                    = 12;    //顺子，以顺子获得一场胜利
				const int First_HuaWin1                    = 13;    //同花，以同花获得一场胜利
				const int First_ThreeTwoWin1               = 14;    //葫芦，以葫芦获得一场胜利
				const int First_FourWin1                   = 15;    //四条，以四条获得一场胜利
				const int First_HuaStrWin1                 = 16;    //同花顺，以同花顺获得一场胜利
				const int First_WinMoney5K                 = 17;    //赢5000，一把赢得5000游戏币
				const int First_WinMoney25K                = 18;    //赢25000，一把赢得25000游戏币
				const int First_MoneyUp50W                 = 19;    //白领精英，游戏币数量到达50W
				const int First_MoneyUp100W                = 20;    //百万富翁，游戏币数量到达100W

				const int First_AllInWin1                  = 21;    //孤注一掷，翻牌前ALLIN取得胜利
				const int First_GoOnWin3                   = 22;    //连胜三局，在同一桌连续三局获胜
				const int First_Land5                      = 23;    //连战五日，连续5天游戏不低于半小时
				const int First_27Win                      = 24;    //绝处逢生，以27手牌取得胜利
				const int First_AAWin                      = 25;    //机会在手，以AA手牌获胜
				const int First_OpenBank                   = 26;    //保险柜，开通保险箱功能





				const int Second_Play1000                  = 51;    //中级牌手，打满1000局
				const int Second_Win300                    = 52;    //初露头角，取得第300场胜利
				const int Second_Send200                   = 53;    //送礼达人，送出200份礼物
				const int Second_Recv200                   = 54;    //魅力四射，收到200份礼物
				const int Second_ShowFace500               = 55;    //笑口常开，发送500次表情
				const int Second_Friend30                  = 56;    //广交朋友，有30名牌友
				const int Second_WinJuBaoPengFour          = 58;    //银聚宝盆，以四条获得一次聚宝盆奖励

				const int Second_ThreeWin20                = 61;    //三条高手，以三条获得20场胜利
				const int Second_StrWin20                  = 62;    //顺子高手，以顺子获得20场胜利
				const int Second_HuaWin20                  = 63;    //同花高手，以同花获得20场胜利
				const int Second_ThreeTwoWin20             = 64;    //葫芦高手，以葫芦获得20场胜利
				const int Second_FourWin5                  = 65;    //四条高手，以四条获得5场胜利
				const int Second_HuaStrWin5                = 66;    //同花顺高手，以同花顺获得5场胜利
				const int Second_King1                     = 67;    //赌侠，以皇家同花顺获得一场胜利
				const int Seoncd_Win5W                     = 68;    //赢5万，一把赢得5万游戏币
				const int Seoncd_Win25W                    = 69;    //赢25万，一把赢得25万游戏币
				const int Second_MoneyUp1000W              = 70;    //千万富翁，游戏币数量到达1000W
				
				const int Seoncd_GoOnWin6                  = 72;    //连胜六局，连续6局获得胜利
				const int Second_Land10                    = 73;    //连战十日，连续10天每天游戏不低于半小时





				const int Third_Play5000                   = 101;    //高级牌手，打满5000局
				const int Third_Win1000                    = 102;    //小有成就，取得第1000次胜利
				const int Third_Send1000                   = 103;    //广结善缘，送出1000份礼物
				const int Third_Recv1000                   = 104;    //魅力之星，收到1000份礼物
				const int Third_ShowFace2500               = 105;    //表情达人，发送2500次表情
				const int Third_Friend100                  = 106;    //高朋满坐，拥有100名牌友
				const int Third_WinJuBaoPengHuaStr         = 108;    //金聚宝盆，以同花顺获得一次聚宝盆奖励

				const int Third_ThreeWin50                 = 111;    //三条王子，以三条获得50场胜利
				const int Third_StrWin50                   = 112;    //顺子王子，以顺子获得50场胜利
				const int Third_HuaWin50                   = 113;    //同花王子，以同花获得50场胜利
				const int Third_ThreeTwoWin50              = 114;    //葫芦王子，以葫芦获得50场胜利
				const int Third_FourWin10                  = 115;    //四条王子，以四条获得10场胜利
				const int Third_HuaStrWin10                = 116;    //同花顺王子，以同花顺获得10场胜利
				const int Third_King5                      = 117;    //赌圣，以皇家同花顺获得5场胜利
				const int Third_Win100W                    = 118;    //赢100万，一把赢得100万游戏币
				const int Third_Win500W                    = 119;    //赢500万，一把赢得500万游戏币
				const int Third_MoneyUp10000W              = 120;    //亿万富翁，游戏币数量到达1亿
				
				const int Third_GoOnWin10                  = 122;    //越战越勇，连续10局获得胜利
				const int Third_Land15                     = 123;    //连战半月，连续15天每天游戏不低于半小时





				const int Fouth_Play20000                  = 151;    //牌场老手，游戏满20000局
				const int Fouth_Win10000                   = 152;    //高手    ，取得第10000次胜利
				const int Fouth_Send10000                  = 153;    //圣诞老人，送出10000份礼物
				const int Fouth_Recv10000                  = 154;    //万人迷，  收到10000份礼物
				const int Fouth_ShowFace10000              = 155;    //表情王子，发送10000次表情
				const int Fouth_Friend200                  = 156;    //朋友遍天下，拥用200名牌友
				const int Fouth_WinJuBaoPengKing           = 158;    //钻石聚宝盆，以天皇同花顺获得一次聚宝盆奖励

				const int Fouth_ThreeWin100                = 161;    //三条之神，以三条获得100场胜利
				const int Fouth_StrWin100                  = 162;    //顺子之神，以顺子获得100场胜利
				const int Fouth_HuaWin100                  = 163;    //同花之神，以同花获得100场胜利
				const int Fouth_ThreeTwoWin100             = 164;    //葫芦之神，以葫芦获得100场胜利
				const int Fouth_FourWin30                  = 165;    //四条之神，以四条获得30场胜利
				const int Fouth_HuaStrWin30                = 166;    //同花顺之神，以同花顺获得30场胜利
				const int Fouth_King20                     = 167;    //赌神，以皇家同花顺获得20场胜利
				const int Fouth_Win2000W                   = 168;    //赢2000万，一把赢得2000万游戏币
				const int Fouth_Win20000W                  = 169;    //赢2亿，一把赢得2亿游戏币
				const int Third_MoneyUp10E	               = 170;    //大富豪，游戏币数量到达10亿

				const int Fouth_GoOnWin15                  = 172;    //势不可挡，连续15局获得胜利
				const int Fouth_Land30                     = 173;    //魔鬼玩家，连续30天每天游戏不低于半小时

			}

			enum ProductType
			{
				ProductType_HuiYuan = 1,
				ProductType_MoguiExchange,
			};

			namespace N_Face
			{
				const short FaceID_KaiXin          = 1;
				const short FaceID_WeiXiao         = 2;
				const short FaceID_JingKong        = 3;
				const short FaceID_XieXie          = 4;
				const short FaceID_MeiNv           = 5;
				const short FaceID_TiaoPi          = 6;
				const short FaceID_FanZao          = 7;
				const short FaceID_BiShi           = 8;
				const short FaceID_FengLu          = 9;
				const short FaceID_LiuHan          = 10;
				const short FaceID_HaiXiu          = 11;
				const short FaceID_FanKun          = 12;
				const short FaceID_KuQi            = 13;
				const short FaceID_DeYi            = 14;
				const short FaceID_YiHuo           = 15;
				const short FaceID_QiDao           = 16;
				const short FaceID_OTu             = 17;
				const short FaceID_KanRen          = 18;
				const short FaceID_Bang            = 19;
				const short FaceID_Yun             = 20;
				const short FaceID_JiaYou          = 21;
			}
		}
	}
}
