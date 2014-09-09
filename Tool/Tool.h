#pragma once

#include <Windows.h>

#include <string>
#include <map>
#include <vector>

#include "boost/utility.hpp"

class otl_datetime;

namespace Tool
{


	#define safe_delete(p) { if(p) { delete p; p = NULL; } }
	#define safe_delete_arr(p) { if(p) { delete[] p; p = NULL; } }

	class CRandom    //�������
	{
	public:
		CRandom(){}
		~CRandom(){}
	public:
		static void InitRandSeed();//��ʼ�����������
		static int  Random_Int(int nMin, int nMax);
		static bool GetChangce(int nCount,int nIdx);
	};


	//time_t -> tm ���̰߳�ȫ
	//time_t *time_input:�����time_t���͵�ʱ��
	//struct tm* tm_result:�����tm�ṹ��ʱ��
	extern int			time_to_tm(time_t *time_input,struct tm* tm_result);
	//tm -> time_t ���̰߳�ȫ
	extern int			tm_to_time(struct tm* tm_input, time_t *time_result);

	extern int          time_to_otltime(time_t* time_input,otl_datetime* otl_result);
	extern int          otltime_to_time(otl_datetime* otl_input,time_t* time_result);
	extern int          otltime_to_tm(otl_datetime* otl_input,struct tm* tm_result);
	extern int          tm_to_otltime(struct tm* tm_input,otl_datetime* otl_result);

	extern time_t       GetTimeFromOTLTime(const otl_datetime& otl_input);
	extern otl_datetime GetOTLTimeFromCurTime(time_t CurTime);

	extern bool         IsSameDay(time_t TimeL,time_t TimeR);
	extern bool         IsNearDay(time_t TimeL,time_t TimeR);
	extern time_t       GetNewDayTime(time_t curTime);

	extern std::string  GetDateTimeString(time_t CurTime = 0);
	extern std::string  GetDateString(time_t CurTime = 0);
	extern std::string  GetTimeString(time_t CurTime = 0);
	extern std::string  GetDateString_XXYYZZ(time_t CurTime = 0);
	extern std::string  GetMonthString_XXXXYY(time_t CurTime = 0);

	extern unsigned long long MakeUInt64( unsigned int high ,unsigned int low );

	template<class T>
	  inline std::string N2S(T nNumber,int nWidth=0)
	{
		//stringstream ssIn;
		//if ( nWidth )
		//{
		//	ssIn.width(nWidth);
		//}
		//if ( nNumber<=255 && nNumber>=-128 )
		//{
		//	ssIn<<int(nNumber);
		//}
		//else
		//{
		//	ssIn<<nNumber;
		//}
		//return ssIn.str();

		static char retBuf[32];
		static char digits[19] =  
		{ '9', '8', '7', '6', '5', '4', '3', '2', '1', 
		'0', 
		'1', '2', '3', '4', '5', '6', '7', '8', '9' };
		static const char* zero = digits + 9;

		T nShang = nNumber;
		T nYuSu  = 0;
		char* p = retBuf;
		do {
			nYuSu = nShang % 10;
			nShang /= 10;
			*p++ = zero[nYuSu];
		} while (nShang != 0);
		if (nNumber < 0)
			*p++ = '-';

		int nLen = p - retBuf;
		while( nLen < nWidth )
		{
			*p++ = 32;
			nLen++;
		}

		*p = 0;
		std::reverse(retBuf, p);
		return string(retBuf);
	}

	template<class T>
		inline void S2N(std::string& strData,T& Number)
	{
		stringstream ssIn;
		ssIn<<strData;
		ssIn>>Number;
	};

	template<class T>
	inline std::string ZN2S(T nNumber, int nWidth = 0)
	{
		static char retBuf[32];
		static char digits[19] =  
		{ '9', '8', '7', '6', '5', '4', '3', '2', '1', 
		'0', 
		'1', '2', '3', '4', '5', '6', '7', '8', '9' };
		static const char* zero = digits + 9;

		T nShang = nNumber;
		T nYuSu = 0;
		char* p = retBuf;
		do {
			nYuSu = nShang % 10;
			nShang /= 10;
			*p++ = zero[nYuSu];
		} while (nShang != 0);
		if (nNumber < 0)
			*p++ = '-';

		int nLen = p - retBuf;
		while( nLen < nWidth )
		{
			*p++ = 32;
			nLen++;
		}
		
		*p = 0;
		std::reverse(retBuf, p);
		return string(retBuf);
	};

	template<typename T>
	std::string ArrayToString(const T* pData, int nLen)
	{
		std::string retString("");
		for ( int nCount=0;nCount<nLen;nCount++ )
		{
			retString += (N2S(*(pData+nCount))+" ");
		}
		return retString;
	}

	template<class T1,class T2>
	UINT64 MakeUINT64( T1 high ,T2 low )
	{
		assert(high>=0);
		assert(low>=0);

		UINT64 Ret = high;
		Ret = Ret<<32;
		Ret += low;
		return Ret;
	}

	extern std::vector<std::string> SplitString(const std::string& src, const std::string& trim);
	extern std::map<std::string, long long> GetRuleMapInData(const std::string& src, const std::string& strFirst, const std::string& strSecond);
	extern std::map<std::string, std::string> GetRuleMapInString(const std::string& src, const std::string& strFirst, const std::string& strSecond);
	extern bool GetKeyValue(const std::string& strSource, const std::string& strKey, long long& nValue);
	extern std::string GetKeyString(const std::string& strSource, const std::string& strKey);

	extern bool   GetGUID(std::string& strGUID);
	extern bool   GetGUID(unsigned char* pGUID);
	extern int    GetGUIDNumber(int nInit=0);
	extern std::string MemoryToString(const void *memory, int size);
	extern void   StringToMemory(std::string& strSrc,const void *memory);
	extern std::string GetMD5(const void *memory, int nSize);
	extern void   GetMD5(const void *memory,int nSize,void* outMemory);
	extern std::string EncryptTo64(const void* pBuf, int nSize);

	class CGameEncrypt : public boost::noncopyable
	{
	public:
		CGameEncrypt(void);
		~CGameEncrypt(void);

	public:
		static void Init();
		static bool encrypt(BYTE* in, BYTE* out, int size);
		static bool decrypt(BYTE* in, BYTE* out, int size);
	};

	class CMoGuiEncrypt : public boost::noncopyable
	{
	public:
		CMoGuiEncrypt(void);
		~CMoGuiEncrypt(void);
		enum{ KeyLen_16=16,KeyLen_24=24,KeyLen_32=32 };
	public:
		void reset();	//��������

		bool setAesKey(const BYTE* key, short size);	//sizeֻ��ȡ16��24��32
		bool encrypt(BYTE* in, BYTE* out, int size, const BYTE* iv = _gAesIV, int num = 0);	//in,out����ѡ����ͬһ��ָ��
		bool decrypt(BYTE* in, BYTE* out, int size, const BYTE* iv = _gAesIV, int num = 0);	//in,out����ѡ����ͬһ��ָ��

		static const BYTE _gAesIV[16];
		static const BYTE _gKey[32];
	private:
		BYTE  m_AesKey[32];
		short m_KeySize;
		bool  m_bSetKey;                    //�Ƿ��ǳ�ʼ����
	};

	extern INT64 GetMilliSecond();         //����
	extern INT64 GetMicroSecond();         //΢��
	extern void  SleepMilliSecond(int nMilliSecond);  //��CPUռ�����ʺܸ�

	//Windows 2000  Windows XP  Windows Vista  Windows Server 2008
	//Windows 7   Windows Server 2008
	extern std::string GetWinVersion();
	extern int         GetCPUNumber();
}