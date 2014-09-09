#pragma once

#include "MoGuiGame.h"

namespace MoGui
{
namespace MoGuiXY
{
namespace GameXY
{
namespace DeZhou
{
namespace DeZhouLogic
{
	const short DeZhouLogic_XYID_JuBaoPeng                            = 10;
	const short DeZhouLogic_XYID_AddMoney                             = 11;
	const short DeZhouLogic_XYID_WashOutPlayer                        = 12;
	const short DeZhouLogic_XYID_ShowFace                             = 13;
	const short DeZhouLogic_XYID_SendGift                             = 14;

	struct DeZhouLogic_JuBaoPeng
	{
		enum { XY_ID = DeZhouLogic_XYID_JuBaoPeng };

		BYTE                      m_SitID;
		UINT32                    m_PID;

		int                       m_nZhuChip;
		int                       m_nCount;

		DeZhouLogic_JuBaoPeng() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const DeZhouLogic_JuBaoPeng& src )
		{
			bos << src.m_SitID;
			bos << src.m_PID;
			bos << src.m_nZhuChip;
			bos << src.m_nCount;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DeZhouLogic_JuBaoPeng& src )
		{
			src.ReSet();

			bis >> src.m_SitID;
			bis >> src.m_PID;
			bis >> src.m_nZhuChip;
			bis >> src.m_nCount;

			return bis;
		}
	};

	struct DeZhouLogic_AddMoney
	{
		enum { XY_ID = DeZhouLogic_XYID_AddMoney };

		BYTE                      m_SitID;
		UINT32                    m_PID;

		INT64                     m_nAddMoney;

		DeZhouLogic_AddMoney() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const DeZhouLogic_AddMoney& src )
		{
			bos << src.m_SitID;
			bos << src.m_PID;

			bos << src.m_nAddMoney;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DeZhouLogic_AddMoney& src )
		{
			src.ReSet();

			bis >> src.m_SitID;
			bis >> src.m_PID;

			bis >> src.m_nAddMoney;

			return bis;
		}
	};

	struct DeZhouLogic_WashOutPlayer
	{
		enum { XY_ID = DeZhouLogic_XYID_WashOutPlayer };

		std::vector<int>     m_listSit;

		DeZhouLogic_WashOutPlayer() { ReSet(); }
		void ReSet(){ if(m_listSit.size())m_listSit.clear();}

		friend bostream& operator<<( bostream& bos, const DeZhouLogic_WashOutPlayer& src )
		{
			InVector(bos,src.m_listSit,10);
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DeZhouLogic_WashOutPlayer& src )
		{
			src.ReSet();
			OutVector(bis,src.m_listSit,10);
			return bis;
		}
	};


	struct DeZhouLogic_ShowFace
	{
		enum { XY_ID = DeZhouLogic_XYID_ShowFace };

		BYTE                      m_SitID;
		UINT32                    m_PID;
		INT32                     m_FaceID;		
		UINT32                    m_ActionTime;

		DeZhouLogic_ShowFace() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const DeZhouLogic_ShowFace& src )
		{
			bos << src.m_SitID;
			bos << src.m_PID;
			bos << src.m_FaceID;

			bos << src.m_ActionTime;

			return bos;
		}
		friend bistream& operator>>( bistream& bis, DeZhouLogic_ShowFace& src )
		{
			src.ReSet();

			bis >> src.m_SitID;
			bis >> src.m_PID;
			bis >> src.m_FaceID;

			bis >> src.m_ActionTime;

			return bis;
		}
	};

	struct DeZhouLogic_SendGift
	{
		enum { XY_ID = DeZhouLogic_XYID_SendGift };

		BYTE                      m_SendSitID;
		UINT32                    m_SendPID;
		UINT32                    m_RecvPID;
		INT32                     m_GiftID;
		UINT32                    m_ActionTime;

		DeZhouLogic_SendGift() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const DeZhouLogic_SendGift& src )
		{
			bos << src.m_SendSitID;
			bos << src.m_SendPID;			
			bos << src.m_RecvPID;
			bos << src.m_GiftID;
			bos << src.m_ActionTime;
			return bos;
		}
		friend bistream& operator>>( bistream& bis, DeZhouLogic_SendGift& src )
		{
			src.ReSet();
			bis >> src.m_SendSitID;
			bis >> src.m_SendPID;			
			bis >> src.m_RecvPID;
			bis >> src.m_GiftID;
			bis >> src.m_ActionTime;
			return bis;
		}
	};
}
}
}
}
}