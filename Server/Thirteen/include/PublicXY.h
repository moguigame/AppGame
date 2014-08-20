#pragma once

#include <string>
#include <vector>

#include "biostream.h"
#include "publicxydef.h"

namespace MoGui
{
namespace MoGuiXY
{
	using namespace std;
	using namespace MoGui::MoGuiXY;

	class CRecvMsgPacket
	{
	public:
		enum { MaxMsgDataLen = MAX_TOTAL_XY_LEN - 4 };

		unsigned short m_XYID;
		unsigned short m_nLen;
		char           m_DataBuf[MaxMsgDataLen];

		CRecvMsgPacket()
		{
			m_XYID = 0;
			m_nLen = 0;
			memset(m_DataBuf,0,sizeof(m_DataBuf));
		}
		~CRecvMsgPacket(void) {}

		int GetDataFromBuf( char* recvbuf,int len )
		{
			if( len < SHORT_HEAD_SIZE + XYID_HEAD_SIZE )
			{
				return 0;
			}

			bistream     bis;
			bis.attach(recvbuf,len);

			unsigned char charlen;
			unsigned short xylen;

			bis >> charlen;
			if( charlen < 255 )
			{
				xylen = charlen;
			}
			else
			{
				if( bis.avail() >= LONG_HEAD_SIZE )
				{
					bis >> xylen;
				}
				else
				{
					return 0;
				}
			}
			m_nLen = xylen;

			if( bis.avail() >= XYID_HEAD_SIZE )
			{
				bis >> m_XYID;
			}
			else
			{
				return 0;
			}

			if( bis.avail() >= m_nLen )
			{
				bis.read(m_DataBuf,m_nLen);
				return len - bis.avail();
			}

			return 0;
		}

		int GetMsgTransLen()
		{
			if ( m_nLen >= 255 ) return m_nLen + 5;
			else return m_nLen + 3;
		}
	};

	template<class Txieyi>
		inline int ExplainMsg(CRecvMsgPacket& msgPack,Txieyi& xieyi)   //用于解释直接发送的协议
	{
		int ret = 0;

		bistream bis;
		bis.attach(msgPack.m_DataBuf,msgPack.m_nLen);

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

		return ret;
	}

#define TransplainMsg(msgPack,xieyi)    \
	if ( ExplainMsg(msgPack,xieyi) )    \
	{	                                \
		return SOCKET_MSG_ERROR_STREAM; \
	}

	inline void InString(bostream& bos,const std::string& strData,size_t nMaxLen)
	{
		if ( nMaxLen >= 0xff || nMaxLen <= 0 )
		{
			throw agproexception(agproexception::rangeerror);
		}
		if ( strData.size() > nMaxLen )
		{
			throw agproexception(agproexception::rangeerror);
		}

		if ( int(strData.length()) <= nMaxLen )
		{
			bos << strData.c_str();
		}
		else
		{
			std::string TempStr = strData.substr(0,nMaxLen);
			bos << TempStr.c_str();
		}
	}
	inline void OutString( bistream& bis,std::string& strData,size_t nMaxLen )
	{
		if ( nMaxLen >= 0xff || nMaxLen <= 0 )
		{
			throw agproexception(agproexception::rangeerror);
		}

		static char StrBuffer[256];
		memset(StrBuffer,0,256);

		unsigned char nLen;
		bis >> nLen;
		if ( nLen < 0xff && nLen <= nMaxLen )
		{
			bis.seekg(-1);
			bis >> StrBuffer;
			strData = std::string(StrBuffer);
		}
		else
		{
			throw agproexception(agproexception::rangeerror);
		}
	}

	template<class T>
		inline void InVector(bostream& bos,const std::vector<T>& vecData,size_t MaxSize)
	{
		if ( MaxSize >= 0xff || MaxSize <= 0 )
		{
			throw agproexception(agproexception::rangeerror);
		}
		if ( vecData.size() > MaxSize )
		{
			throw agproexception(agproexception::rangeerror);
		}

		unsigned char nSize = unsigned char(min(int(vecData.size()),MaxSize));		
		bos << nSize;
		for (unsigned int i=0;i<nSize;++i)
		{
			bos << vecData.at(i);
		}
	}

	template<class T>
		inline void OutVector(bistream& bis,std::vector<T>& vecData,size_t MaxSize)
	{
		if ( MaxSize >= 0xff || MaxSize <= 0 )
		{
			throw agproexception(agproexception::rangeerror);
		}

		unsigned char nSize;
		T TempData;

		bis >> nSize;
		if ( nSize > MaxSize )
		{
			throw agproexception(agproexception::rangeerror);
		}
		for (unsigned int i=0;i<nSize;++i)
		{
			bis >> TempData;
			vecData.push_back(TempData);
		}
	}

namespace PublicXY
{
	const short MOGUI_ENCRYPTVER			= MOGUI_FIRST_PUBLIC+1;     //加密版本沟通协议，作为所有通讯协议的前置协议
	const short MOGUI_CHECKACT				= MOGUI_FIRST_PUBLIC+2;     //激活协议，心跳包，间隔时间40秒。删除不活动socket的间隔时间为5分钟。
	const short MOGUI_REQHEART              = MOGUI_FIRST_PUBLIC+3;     //新版心跳包
	const short MOGUI_RESQHEART             = MOGUI_FIRST_PUBLIC+4;
	const short MOGUI_REQKEY                = MOGUI_FIRST_PUBLIC+5;
	const short MOGUI_RESPKEY               = MOGUI_FIRST_PUBLIC+6;
	const short MOGUI_DATETIME				= MOGUI_FIRST_PUBLIC+105;	//报告时间
	const short MOGUI_BATCHPROTOCOL			= MOGUI_FIRST_PUBLIC+106;	//协议批量发送通知协议

	const short MOGUI_XieYiList             = MOGUI_FIRST_PUBLIC+111;	//批量协议List
	const short MOGUI_TestXieYi             = MOGUI_FIRST_PUBLIC+201;

	struct EncryptVer
	{
		enum { XY_ID = MOGUI_ENCRYPTVER };

		unsigned short m_dwVer;

		EncryptVer() { reset(); }
		void reset() { memset(this,0,sizeof(*this)); }
		friend bostream& operator<<(bostream& bos, const EncryptVer& ev)
		{
			bos << ev.m_dwVer;
			return bos;
		}
		friend bistream& operator>>(bistream& bis, EncryptVer& ev)
		{
			ev.reset();
			bis >> ev.m_dwVer;
			return bis;
		}
	};

	struct CheckAct
	{
		enum { XY_ID = MOGUI_CHECKACT };

		unsigned short m_nActive;

		CheckAct():m_nActive(0) {}
		friend bostream& operator<<(bostream& bos, const CheckAct& ca)
		{
			bos << ca.m_nActive;
			return bos;
		}
		friend bistream& operator>>(bistream& bis, CheckAct& ca)
		{
			bis >> ca.m_nActive;
			return bis;
		}
	};

	struct Req_Heart
	{
		enum { XY_ID = MOGUI_REQHEART };

		unsigned long long m_HeatID;

		Req_Heart():m_HeatID(0) {}
		friend bostream& operator<<(bostream& bos, const Req_Heart& rh)
		{
			bos << rh.m_HeatID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis, Req_Heart& rh)
		{
			bis >> rh.m_HeatID;
			return bis;
		}
	};
	struct Resp_Heart
	{
		enum { XY_ID = MOGUI_RESQHEART };

		unsigned long long m_HeatID;

		Resp_Heart():m_HeatID(0) {}
		friend bostream& operator<<(bostream& bos, const Resp_Heart& rh)
		{
			bos << rh.m_HeatID;
			return bos;
		}
		friend bistream& operator>>(bistream& bis, Resp_Heart& rh)
		{
			bis >> rh.m_HeatID;
			return bis;
		}
	};

	struct ReqKey
	{
		enum { XY_ID = MOGUI_REQKEY };
		enum { KeyEnable=100,KeyDisable=100,};

		unsigned char m_Flag;

		ReqKey(){ Reset(); }
		void Reset(){ m_Flag = 0; }

		friend bostream& operator<<(bostream& bos, const ReqKey& src)
		{
			bos << src.m_Flag;

			return bos;
		}
		friend bistream& operator>>(bistream& bis, ReqKey& src)
		{
			src.Reset();

			bis >> src.m_Flag;
			
			return bis;
		}
	};

	struct RespKey
	{
		enum { XY_ID = MOGUI_RESPKEY };
		enum { KEYLEN16=16,KEYLEN24=24,KEYLEN32=32 };

		unsigned char    m_Len;
		unsigned char    m_Key[KEYLEN32];

		RespKey(){ Reset(); }
		void Reset(){ memset(m_Key,0,sizeof(m_Key)); }

		friend bostream& operator<<(bostream& bos, const RespKey& src)
		{
			bos << src.m_Len;
			for (int i=0;i<src.m_Len&&i<src.KEYLEN32;i++)
			{
				bos << src.m_Key[i];
			}
			return bos;
		}
		friend bistream& operator>>(bistream& bis, RespKey& src)
		{
			bis >> src.m_Len;
			for(int i=0;i<src.m_Len&&i<src.KEYLEN32;i++)
			{
				bis >> src.m_Key[i];
			}
			return bis;
		}
	};

	struct DateTime
	{
		enum { XY_ID = MOGUI_DATETIME };
		enum FLAG
		{
			ONLY_REPORT = 0,    //仅仅告诉一下当前时间
			NEED_UPDATE = 1     //要求收到后将本机时间更新成该协议内的时间
		};

		unsigned char		    m_Flag;
		unsigned int    	    m_nDateTime;

		DateTime() { reset(); }
		void reset() { memset(this,0,sizeof(*this)); }
		friend bostream& operator<<(bostream& bos, const DateTime& dt)
		{
			bos << dt.m_Flag;
			bos << dt.m_nDateTime;
			return bos;
		}
		friend bistream& operator>>(bistream& bis, DateTime& dt)
		{
			dt.reset();
			bis >> dt.m_Flag;
			bis >> dt.m_nDateTime;
			return bis;
		}
	};

	struct BatchProtocol
	{
		enum { XY_ID = MOGUI_BATCHPROTOCOL };
		enum FLAG
		{
			NONE = 0,
			START = 1,		//批量发送开始
			END = 2			//批量发送结束
		};

		unsigned char	m_Flag;
		unsigned short	m_XYID;		//协议号
		unsigned short	m_nCount;	//数量

		BatchProtocol() { reset(); }
		void reset() { memset(this,0,sizeof(*this)); }
		friend bostream& operator<<(bostream& bos, const BatchProtocol& bp)
		{
			bos << bp.m_Flag;
			bos << bp.m_XYID;
			bos << bp.m_nCount;
			return bos;
		}
		friend bistream& operator>>(bistream& bis, BatchProtocol& bp)
		{
			bp.reset();
			bis >> bp.m_Flag;
			bis >> bp.m_XYID;
			bis >> bp.m_nCount;
			return bis;
		}
	};

	template<class Txieyi,size_t nMaxCount>
	struct XieYiList
	{
	public:
		enum { XY_ID = MOGUI_XieYiList };

		XieYiList()
		{
			m_XieYiID = Txieyi::XY_ID;
			Reset();
		}
		void Reset()
		{
			m_XieYiList.clear();
		}
		bool IsMustSendOut()const
		{
			return m_XieYiList.size() >= nMaxCount;
		}
		bool IsHaveData()const
		{
			return m_XieYiList.size() > 0;
		}

		short              m_XieYiID;
		vector<Txieyi>     m_XieYiList;

		friend bostream& operator<<(bostream& bos,const XieYiList& src)
		{
			bos << src.m_XieYiID;
			InVector(bos,src.m_XieYiList,nMaxCount);
			return bos;
		}
		//friend bistream& operator>>(bistream& bis,XieYiList& src)
		//{
			//src.ReSet();
			//bis >> src.m_XieYiID;
			//OutVector(bis,src.m_XieYiList,nMaxCount);
			//return bis;
		//}
	};

	struct TestXieYi
	{
		enum { XY_ID    = MOGUI_TestXieYi };
		enum { MAX_DATA = 4090 };

		unsigned short	m_msgLen;	          //协议长度
		char        	m_msgData[MAX_DATA];  //协议数据

		TestXieYi() { reset(); }
		void reset() { memset(this,0,sizeof(*this)); }
		friend bostream& operator<<(bostream& bos, const TestXieYi& src)
		{
			short msgLen = min(max(src.m_msgLen,0),src.MAX_DATA);
			bos << msgLen;
			bos.write(src.m_msgData,msgLen);

			return bos;
		}
		friend bistream& operator>>(bistream& bis, TestXieYi& src)
		{
			bis >> src.m_msgLen;
			if (  src.m_msgLen > src.MAX_DATA || src.m_msgLen < 0 )
			{
				throw agproexception(agproexception::rangeerror);
			}
			bis.read(src.m_msgData,src.m_msgLen);

			return bis;
		}
	};
}
}
}
