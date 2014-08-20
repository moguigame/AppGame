#pragma once

#include "MoGuiGame.h"

namespace MoGui
{
namespace MemcatchXY
{
namespace DeZhou
{
	//struct memGSUserData                 //��Ϸ����Ϸ�һ

	//struct memUserLandRecord             //��¼��ҵ�½�����

	//struct memUserBaseInfo               //���ڿͻ���ȡ��ҵ�ͷ���NICKNAME��Ϣ
	//struct memUserInfo                   //���ڴ����ҵ���Ϣ

	//struct memUserDataInfo               //��¼��������Ϣ
	//struct memUserGameInfo
	//struct memUserGameInfoEx             //���ڴ�������Ϸ�����Ϣ�����ǲ��������µ�����
	//struct memUserGameWinRecord          //���ڴ������Ժ��ַ�ʽӮ�����ݣ��������£�����cachҲ��һ��
	//struct memUserMatchInfo              //���ڴ����ҵı�����Ϣ

	//struct memGiftInfo                   //���ڴ���������Ϣ��ͬʱ����ҳ�ӿڵ���

	//��Ϸ����Ϸ�һ
	struct memGSUserData
	{
		int       m_AID;
		int       m_PID;

		INT32     m_LastActTime;         //�ϴμ�¼��ʱ��
		INT8      m_CurStep;             //��ǰ�ڼ���
		INT16     m_PassTime;            //��ǰ����ȥ��ʱ��

		UINT32    m_FirstTimeToday;
		UINT16    m_TodayGameSecond;
		UINT16    m_TodayGameCount;
		INT8      m_HaveAwardInvite;

		memGSUserData(){ Reset();}
		void Reset(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<(bostream& bos, const memGSUserData& src)
		{
			bos << src.m_LastActTime;
			bos << src.m_CurStep;
			bos << src.m_PassTime;

			bos << src.m_FirstTimeToday;
			bos << src.m_TodayGameSecond;
			bos << src.m_TodayGameCount;
			bos << src.m_HaveAwardInvite;

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memGSUserData& src)
		{
			bis >> src.m_LastActTime;
			bis >> src.m_CurStep;
			bis >> src.m_PassTime;

			bis >> src.m_FirstTimeToday;
			bis >> src.m_TodayGameSecond;
			bis >> src.m_TodayGameCount;
			bis >> src.m_HaveAwardInvite;

			return bis;
		}
	};

	//��¼��ҵ�½�����
	struct memUserLandRecord
	{
		int              m_AID;
		int              m_PID;

		UINT32           m_FirstTodayTime;
		BYTE             m_ContinueLogin;
		BYTE             m_ContinuePlay;

		memUserLandRecord(){ Reset();}
		void Reset(){ memset(this, 0, sizeof(*this)); }

		friend bostream& operator<<(bostream& bos, const memUserLandRecord& src)
		{
			bos << src.m_FirstTodayTime;
			bos << src.m_ContinueLogin;
			bos << src.m_ContinuePlay;
			
			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserLandRecord& src)
		{
			bis >> src.m_FirstTodayTime;
			bis >> src.m_ContinueLogin;
			bis >> src.m_ContinuePlay;

			return bis;
		}
	};

	//��¼������������Ϣ�ģ��ʱ�䱣���
	struct memUserDataInfo
	{
		int                                   m_AID;
		int                                   m_PID;

		int                                   m_Sex;
		int                                   m_HeadID;
		int                                   m_InvitePID;
		
		std::string                           m_UserName;
		std::string                           m_Password;
		std::string                           m_NickName;
		std::string                           m_HardCode;
		std::string                           m_HeadPicURL;
		std::string                           m_HomePageURL;
		std::string                           m_City;

		memUserDataInfo(){ Reset(); }
		void Reset()
		{
			m_PID = 0;
			m_AID = 0;

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
		friend bostream& operator<<(bostream& bos, const memUserDataInfo& src)
		{			
			bos << src.m_Sex;
			bos << src.m_HeadID;
			bos << src.m_InvitePID;

			InString(bos,src.m_UserName,MAX_USERNAME_SIZE);
			InString(bos,src.m_Password,MAX_PASSWORD_SIZE);
			InString(bos,src.m_NickName,MAX_NICKNAME_SIZE);
			InString(bos,src.m_HardCode,MAX_HARDCODE_SIZE);
			InString(bos,src.m_HeadPicURL,MAX_URL_SIZE);
			InString(bos,src.m_HomePageURL,MAX_URL_SIZE);
			InString(bos,src.m_City,MAX_CITYNAME_SIZE);

			return bos;
		}
		friend bistream& operator>>(bistream& bis, memUserDataInfo& src)
		{			
			bis >> src.m_Sex;
			bis >> src.m_HeadID;
			bis >> src.m_InvitePID;

			OutString(bis,src.m_UserName,MAX_USERNAME_SIZE);
			OutString(bis,src.m_Password,MAX_PASSWORD_SIZE);
			OutString(bis,src.m_NickName,MAX_NICKNAME_SIZE);
			OutString(bis,src.m_HardCode,MAX_HARDCODE_SIZE);
			OutString(bis,src.m_HeadPicURL,MAX_URL_SIZE);
			OutString(bis,src.m_HomePageURL,MAX_URL_SIZE);
			OutString(bis,src.m_City,MAX_CITYNAME_SIZE);

			return bis;
		}
	};

}
}
}