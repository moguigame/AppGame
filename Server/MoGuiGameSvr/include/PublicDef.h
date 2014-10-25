#pragma once

#include <Windows.h>
//#pragma warning( disable : 4067 )

namespace MoGui
{
	namespace N_AREAID
	{
		const short AREAID_CESHI               = 1;                    //�������
		const short AREAID_BOT                 = 2;                    //���������
		const short AREAID_VISITOR             = 3;                    //�ο����
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

		//�����ǳ���������ּ���ʱ�䣬����֧����Ϸ�������е����ݵ�
		const short t_FlashConnect              = 5;           //���FLASH��ȫ���Ժ����ӵ�ʱ��
		const short t_PlayerActive              = 45;          //��������������ʱ��
		const short t_SocketActive              = 180;
		const short t_PlayerOffLine             = 180;
		const short t_SocketStopTime            = 10;

		const short t_CheckRoom                 = 1;           //��ⷿ���ʱ��
		const short t_KeepPlayerData            = 600;         //����������Ҷ����ʱ��ֵ
		const short t_CheckAddFriend            = 300;
		const short t_KeepAddFriend             = 3600;
		const short T_CheckCityPlayer           = 10;
		const short T_CheckFriendPlayer         = 20;

		const short T_MatchAddBlind             = 300;
	}

	enum ClientClass   //ֻ��¼�������𷽵�����
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
		const BYTE None      = 0;     //û�й�ע
		const BYTE Follow    = 1;     //�ҹ�ע����
		const BYTE BeFollow  = 2;     //��ע�ҵ���
		const BYTE Both      = 3;     //�໥��ע����

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
		PLAYER_TYPE_PLAYER,                         //��ͨ���
		PLAYER_TYPE_BOT,             				//������
		PLAYER_TYPE_VISITOR,                        //�ο��ʺ�
		PLAYER_TYPE_ADMIN,                          //����Ա
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

		SOCKET_MSG_ERROR_STREAM   = 10001,               //Э��������
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

	enum eMoneyType                      //���ҵ�����
	{
		MONEYTYPE_MONEY,
		MONEYTYPE_JIFEN,
	};

	enum ePayMode
	{
		PM_OneTime = 1,                 //ֻ��һ�������ɾ��
		PM_EveryDay = 2                 //ÿ��һ�� 
	};

	
	//��¼�����ʶ
	const int GameError_PlayerClientError                 = 1;
	const int GameError_AddMoney                          = 101;
	const int GameError_AddMoguiMoney                     = 201;
	const int GameError_ChangeUserInfo                    = 301;

	namespace N_IncomeAndPay
	{
		//1000����Ϊ�������� 3000����Ϊ������ 1000��3000֮��Ϊ��������Ҳ���Լ�������1000��2000��Ϊ���� 2000��3000��Ϊ���� ���ұ���Ҫ��һһ��Ӧ��ϵ�����1000�Ĳ�ֵ
		//������ֵ������ 2000����Ϊ�� 2000����Ϊ��
		const short   JoinAward       = 10;                       //ע�ά��
		const short   LandAward       = 11;                       //��½����
		
		const short   FenHongAward    = 13;                       //�ֺ콱��
		const short   InviteAward     = 14;                       //������ҽ���

		const short   UpdateZiLiao    = 21;                       //�������Ͻ���

		const short   FinishHonor     = 31;                       //��ɳɾͽ���
		const short   FinishTask      = 33;                       //���������
		const short   GameLevel       = 35;                       //�����������

		const short   TimePromote     = 41;                       //������Ϸ�ϵͳ���������Ϸ��
		const short   JiuMingQian     = 42;                       //����ҷ��ľ���Ǯ

		const short   PayMoney        = 101;                      //��ֵ
		const short   PayMoneyAward   = 102;                      //��ֵ����

		const short   Mogui_PayMoney  = 201;                      //��ϵͳ��ֵ��ɽ��

		const short   SysAdd          = 1001;                     //ϵͳ���ӻ���٣��˹���ӻ������ӵ�
		const short   SysDec          = 2001;

		const short   BotAdd          = 1002;                     //�����˵���Ϸ�����ӻ��߼���
		const short   BotDec          = 2002;
	
		const short   Win             = 1021;                     //�����������Ӯ
		const short   Loss            = 2021;

		const short   MatchAward      = 1026;                     //��������
		const short   MatchTicket     = 2026;                     //������Ʊ		

		const short   RecvMoney       = 1031;                     //��Ҽ�����������������Ϸ��
		const short   SendMoney       = 2031;

		const short   SoldGift        = 1036;                     //��������
		const short   SendGift        = 2036;

		const short   BankToGame      = 1041;                     //���е���Ϸ
		const short   GameToBank      = 2041;                     //��Ϸ������

		const short   JuBaoPengAward  = 1051;                     //�۱����
		const short   JuBaoPengChip   = 2051;                     //�۱�����ע

		const short   ShowFace        = 3035;                     //������
		const short   CreateTable     = 3041;                     //�������Ӹ��ķ���
		const short   AddTableTime    = 3042;                     //�ӳ�����ʱ��
	}

	namespace N_Gift
	{
		enum
		{
			GiftTime_Cur=1,   //����
			GiftTime_Out=2,   //����
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