#pragma once

#include <Windows.h>

namespace MoGui
{
	namespace Game
	{
		namespace N_CeShiLog
		{
			//DBר��
			const int   c_NoCheckLogin        = 1;               //������û���½��֤
			const int   c_RWDBLog             = 1;               //��¼RWDB����Ч���ٶȵ����
			const int   c_DBOnTimeDiff        = 600;             //���ټ����ӡһ������ʱ��,��λ��
			const int   c_DBMaxOnTime         = 100;             //������ʱ����ڶ��ٵ�ʱ���ӡһ��

			//GSר��
			const int   c_BotPlayerLeave      = 0;               //����������뿪
			const int   c_PlayerAction        = 1;               //��ҵĽ������������Լ�����
			const int   c_CityPlayer          = 1;               //ͬ����صļ�¼
			const int   c_GSOnTimeDiff        = 600;              //���ټ����ӡһ������ʱ��,��λ��
			const int   c_GSMaxOnTime         = 500;             //������ʱ����ڶ��ٵ�ʱ���ӡһ��

			//ͨ��
			const int   c_RightLog            = 1;               //�������RIGHT�仯�ĵط�
			const int   c_FuncTimeLog         = 1;               //��¼��������ʱ���ͳ��
			const int   c_MoneyLog            = 1;               //��¼��moneylog��ص�����
			const int   c_XieYiLog            = 1;               //����Ϸ��Э����з���ͳ��
			const int   c_NewMomory           = 1;               //���Զ�̬�ڴ�,�ж�̬�������ͳ��
		}

		namespace DeZhou
		{
			const short    MAX_PALYER_ON_TABLE      = 9;                   //�������һ����Ϸ����
			const short    MIN_PLAYER_FOR_GAME      = 2;                   //��ʼ��Ϸ���������
			const short    MAX_PLAYER_IN_TABLE      = 250;                 //�����Թۼ���Ϸ�������ܳ���255
			const short    MAX_TABLE_IN_ROOM        = 1000;                //�����ڵ������������
			const short    MAX_PLAYER_IN_ROOM       = 10000;               //�����ڵ��������
			const short    MAX_TALBE_ID             = 9999;
			const short    MAX_PaiTypeNum           = 10;

			const int      Max_PlayerData_In_DBS    = 200000;

			const int      Max_Player_Level         = 9;
			const int      Min_Player_Level         = 1;

			const int      Max_RecommendFriend      = 50;                  //ϵͳ�Զ��Ƽ���������

			const short    HAND_CARDS               = 2;
			const short    TABLE_CARDS              = 5;
			const short    RESULT_CARDS             = 5;
			const short    SELECT_CARDS             = HAND_CARDS+TABLE_CARDS;

			const short    XIPAI_CARDS              = 52;                  //ÿһ�ֵ�����
			const short    XIPAI_POOLS              = 200;                 //�����Ƶĳ�����

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
				Match_Type_TaoJin   = 1,                    //��̭��
				Match_Type_JingBiao = 2,                    //������
				Match_Type_GuanJun  = 3,                    //�ھ���
			};

			enum ROOMTYPE
			{
				ROOM_TYPE_Common             = 1,          //����ģʽ����ͨ�Ļ����Ŷ����ʵ�
				ROOM_TYPE_TableMatch         = 2,         //��������,
				ROOM_TYPE_RoomMatch          = 3,         //��������ı�������Ҫ����֯���ͱ���				
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
				TABLE_LIMITE_NO = 1,                       //������
				TABLE_LIMITE_TEXAS = 2,                    //������ע�� ������ע��
				TABLE_LIMITE_POT = 3,                      //�޳���
			};

			enum TABLEPOT
			{
				TABLE_POT_NO = 1,
				TABLE_POT_YES
			};

			enum TABLETYPE
			{
				TABLE_TYPE_COMMON = 1,     				   //��ͨ��
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
				TABLE_ST_NONE,             					//û�������ţ��Թ۵Ĳ����㣬
				TABLE_ST_WAIT,             					//���ſ�ʼ���п������˲��㣬Ҳ�п����������м�Ķ��ݵȴ���
				TABLE_ST_PLAYING,          					//��Ϸ��
				TABLE_ST_END,                               //������Ϸ��������Ա������Լ��Խ����䳡����
			};

			enum GAMESTATE
			{
				GAME_ST_NONE,                               //����Ϊ0ʱ��״̬
				GAME_ST_WAIT,                               //��������������м�ĵȴ�
				GAME_ST_HAND,                               //����HAND�ƺ��״̬��������������
				GAME_ST_FLOP,
				GAME_ST_TURN,
				GAME_ST_RIVER,
				GAME_ST_DIVIDEMONEY,                        //���ɳ���Ĺ���
				GAME_ST_END
			};

			enum GAMEPLAYERSTATE                           //�������Ϸ�߼��е�״̬����������������
			{
				GAME_PLAYER_ST_NULL,
				GAME_PLAYER_ST_WAITING,                    //�ȴ���Ϸ��Ǯ����û�м���
				GAME_PLAYER_ST_GIVEUP,                     //��Ϸ������
				GAME_PLAYER_ST_ALLIN,                      //��Ϸ��ALL IN
				GAME_PLAYER_ST_PLAYING,		               //������Ϸ��
				GAME_PLAYER_ST_WASHOUT,                    //��������̭��ȥ
			};

			enum PLAYERSTATE
			{
				PLAYER_ST_NONE,             				//��ʾ��Ҹմ���������Ч
				PLAYER_ST_LOBBY,                            //��ʾ����ڴ�����
				PLAYER_ST_ROOM,            				    //�ڷ�����վ��,
				PLAYER_ST_SEEING,           				//�Թ�
				PLAYER_ST_WAIT,             				//�����ϵȴ�������ҵļ�����ߴ���ʼ
				PLAYER_ST_PALYING,           				//������Ϸ�У���ʱ�����溦���ڻ���2��
				PLAYER_ST_END
			};

			enum MaxChip
			{
				MAXCHIP_HAND,               				//��������ע
				MAXCHIP_FLOP,               				//������ע
				MAXCHIP_TURN,               				//ת����ע
				MAXCHIP_RIVER,              				//������ע��Ҳ��������ע����
				MAXCHIP_END
			};

			enum ChipAction
			{
				ACTION_NONE,
				ACTION_Chip = 1,                            //��ע��ָһ�ֵĵ�һ�����
				ACTION_Follow = 2,                          //��ע
				ACTION_AddChip = 4,                         //��ע
				ACTION_KanPai = 8,                          //����
				ACTION_GiveUp = 16,                         //����
				ACTION_AllIn = 32,                          //ȫ�£�Ҳ����ȫ�º���Ȼ���������
				ACTION_POT = 64,                            //��ע
				ACTION_SMALLBLIND = 128,                    //Сäע
				ACTION_BIGBLIND = 256,                      //��äע
				ACTION_NewBlind = 512,                      //����äע

				ACTION_WaitChip = 8912,                     //�ȴ���ע
			};

			enum eTallyFlag
			{
				TALLY_NONE,
				TALLY_WIN,
				TALLY_LOSS,
				TALLY_GIVEUP,             //����
				TALLY_ESCAPE,             //ֱ������
				TALLY_GIVEUP_ESCAPE,      //�ȷ���������
				TALLY_SYS,                //ϵͳ����
			};

			enum ActionFlag
			{
				Action_Flag_Player = 0,        //����Լ��������Ϣ
				Action_Flag_SysLogic,          //ϵͳ�����߼�����ҵĶ����������������ӻ�����,���վ���     
				Action_Flag_Match,             //�����еķ��䣬��ʱ���ܵ�����ұ����˳���
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
				PAI_TYPE_ONE,                               //����
				PAI_TYPE_TWO,                               //����
				PAI_TYPE_TWOTWO,                            //����
				PAI_TYPE_THREE,                             //����
				PAI_TYPE_STR,                               //˳��
				PAI_TYPE_HUA,                               //ͬ��
				PAI_TYPE_THREETWO,                          //��«
				PAI_TYPE_FOUR,                              //�ĸ�
				PAI_TYPE_HUASTR,                            //ͬ��˳
				PAI_TYPE_KING,                              //�ʼ�ͬ��˳
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

				const int First_Play100                    = 1;    //�Ƴ����ˣ�����100����Ϸ
				const int First_Win10                      = 2;    //С��ţ����ȡ��10��ʤ��
				const int First_Send20                     = 3;    //�罻���ˣ��ͳ�20������
				const int First_Recv20                     = 4;    //�㽻���ѣ��յ�20������
				const int First_ShowFace100                = 5;    //��˵��Ц������100�α���
				const int First_Friend1                    = 6;    //�������㣬��һ������
				const int Frist_WinJuBaoPeng               = 8;    //ͭ�۱��裬���������������ͻ��һ�ξ۱��轱��

				const int First_ThreeWin1                  = 11;    //���������������һ��ʤ��
				const int First_StrWin1                    = 12;    //˳�ӣ���˳�ӻ��һ��ʤ��
				const int First_HuaWin1                    = 13;    //ͬ������ͬ�����һ��ʤ��
				const int First_ThreeTwoWin1               = 14;    //��«���Ժ�«���һ��ʤ��
				const int First_FourWin1                   = 15;    //���������������һ��ʤ��
				const int First_HuaStrWin1                 = 16;    //ͬ��˳����ͬ��˳���һ��ʤ��
				const int First_WinMoney5K                 = 17;    //Ӯ5000��һ��Ӯ��5000��Ϸ��
				const int First_WinMoney25K                = 18;    //Ӯ25000��һ��Ӯ��25000��Ϸ��
				const int First_MoneyUp50W                 = 19;    //���쾫Ӣ����Ϸ����������50W
				const int First_MoneyUp100W                = 20;    //�����̣���Ϸ����������100W

				const int First_AllInWin1                  = 21;    //��עһ��������ǰALLINȡ��ʤ��
				const int First_GoOnWin3                   = 22;    //��ʤ���֣���ͬһ���������ֻ�ʤ
				const int First_Land5                      = 23;    //��ս���գ�����5����Ϸ�����ڰ�Сʱ
				const int First_27Win                      = 24;    //������������27����ȡ��ʤ��
				const int First_AAWin                      = 25;    //�������֣���AA���ƻ�ʤ
				const int First_OpenBank                   = 26;    //���չ񣬿�ͨ�����书��





				const int Second_Play1000                  = 51;    //�м����֣�����1000��
				const int Second_Win300                    = 52;    //��¶ͷ�ǣ�ȡ�õ�300��ʤ��
				const int Second_Send200                   = 53;    //������ˣ��ͳ�200������
				const int Second_Recv200                   = 54;    //�������䣬�յ�200������
				const int Second_ShowFace500               = 55;    //Ц�ڳ���������500�α���
				const int Second_Friend30                  = 56;    //�㽻���ѣ���30������
				const int Second_WinJuBaoPengFour          = 58;    //���۱��裬���������һ�ξ۱��轱��

				const int Second_ThreeWin20                = 61;    //�������֣����������20��ʤ��
				const int Second_StrWin20                  = 62;    //˳�Ӹ��֣���˳�ӻ��20��ʤ��
				const int Second_HuaWin20                  = 63;    //ͬ�����֣���ͬ�����20��ʤ��
				const int Second_ThreeTwoWin20             = 64;    //��«���֣��Ժ�«���20��ʤ��
				const int Second_FourWin5                  = 65;    //�������֣����������5��ʤ��
				const int Second_HuaStrWin5                = 66;    //ͬ��˳���֣���ͬ��˳���5��ʤ��
				const int Second_King1                     = 67;    //�������Իʼ�ͬ��˳���һ��ʤ��
				const int Seoncd_Win5W                     = 68;    //Ӯ5��һ��Ӯ��5����Ϸ��
				const int Seoncd_Win25W                    = 69;    //Ӯ25��һ��Ӯ��25����Ϸ��
				const int Second_MoneyUp1000W              = 70;    //ǧ���̣���Ϸ����������1000W
				
				const int Seoncd_GoOnWin6                  = 72;    //��ʤ���֣�����6�ֻ��ʤ��
				const int Second_Land10                    = 73;    //��սʮ�գ�����10��ÿ����Ϸ�����ڰ�Сʱ





				const int Third_Play5000                   = 101;    //�߼����֣�����5000��
				const int Third_Win1000                    = 102;    //С�гɾͣ�ȡ�õ�1000��ʤ��
				const int Third_Send1000                   = 103;    //�����Ե���ͳ�1000������
				const int Third_Recv1000                   = 104;    //����֮�ǣ��յ�1000������
				const int Third_ShowFace2500               = 105;    //������ˣ�����2500�α���
				const int Third_Friend100                  = 106;    //����������ӵ��100������
				const int Third_WinJuBaoPengHuaStr         = 108;    //��۱��裬��ͬ��˳���һ�ξ۱��轱��

				const int Third_ThreeWin50                 = 111;    //�������ӣ����������50��ʤ��
				const int Third_StrWin50                   = 112;    //˳�����ӣ���˳�ӻ��50��ʤ��
				const int Third_HuaWin50                   = 113;    //ͬ�����ӣ���ͬ�����50��ʤ��
				const int Third_ThreeTwoWin50              = 114;    //��«���ӣ��Ժ�«���50��ʤ��
				const int Third_FourWin10                  = 115;    //�������ӣ����������10��ʤ��
				const int Third_HuaStrWin10                = 116;    //ͬ��˳���ӣ���ͬ��˳���10��ʤ��
				const int Third_King5                      = 117;    //��ʥ���Իʼ�ͬ��˳���5��ʤ��
				const int Third_Win100W                    = 118;    //Ӯ100��һ��Ӯ��100����Ϸ��
				const int Third_Win500W                    = 119;    //Ӯ500��һ��Ӯ��500����Ϸ��
				const int Third_MoneyUp10000W              = 120;    //�����̣���Ϸ����������1��
				
				const int Third_GoOnWin10                  = 122;    //ԽսԽ�£�����10�ֻ��ʤ��
				const int Third_Land15                     = 123;    //��ս���£�����15��ÿ����Ϸ�����ڰ�Сʱ





				const int Fouth_Play20000                  = 151;    //�Ƴ����֣���Ϸ��20000��
				const int Fouth_Win10000                   = 152;    //����    ��ȡ�õ�10000��ʤ��
				const int Fouth_Send10000                  = 153;    //ʥ�����ˣ��ͳ�10000������
				const int Fouth_Recv10000                  = 154;    //�����ԣ�  �յ�10000������
				const int Fouth_ShowFace10000              = 155;    //�������ӣ�����10000�α���
				const int Fouth_Friend200                  = 156;    //���ѱ����£�ӵ��200������
				const int Fouth_WinJuBaoPengKing           = 158;    //��ʯ�۱��裬�����ͬ��˳���һ�ξ۱��轱��

				const int Fouth_ThreeWin100                = 161;    //����֮�����������100��ʤ��
				const int Fouth_StrWin100                  = 162;    //˳��֮����˳�ӻ��100��ʤ��
				const int Fouth_HuaWin100                  = 163;    //ͬ��֮����ͬ�����100��ʤ��
				const int Fouth_ThreeTwoWin100             = 164;    //��«֮���Ժ�«���100��ʤ��
				const int Fouth_FourWin30                  = 165;    //����֮�����������30��ʤ��
				const int Fouth_HuaStrWin30                = 166;    //ͬ��˳֮����ͬ��˳���30��ʤ��
				const int Fouth_King20                     = 167;    //�����Իʼ�ͬ��˳���20��ʤ��
				const int Fouth_Win2000W                   = 168;    //Ӯ2000��һ��Ӯ��2000����Ϸ��
				const int Fouth_Win20000W                  = 169;    //Ӯ2�ڣ�һ��Ӯ��2����Ϸ��
				const int Third_MoneyUp10E	               = 170;    //�󸻺�����Ϸ����������10��

				const int Fouth_GoOnWin15                  = 172;    //�Ʋ��ɵ�������15�ֻ��ʤ��
				const int Fouth_Land30                     = 173;    //ħ����ң�����30��ÿ����Ϸ�����ڰ�Сʱ

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
