#include "Tool.h"

#include <Windows.h>

#include <atlbase.h>
#include <Iphlpapi.h>
#include <winbase.h>

#include <sstream>
#include <map>
#include <vector>
#include <queue>
#include <list>
#include <string>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <algorithm>
#include <functional>
#include <objbase.h>
#include <assert.h>
#include <algorithm>
#include "boost\algorithm\string.hpp"

#define OTL_ODBC_MYSQL
#define OTL_ODBC         // Compile OTL 4.0/ODBC
#define OTL_STL          // Turn on STL features
#define OTL_ANSI_CPP     // Turn on ANSI C++ typecasts
#define OTL_BIGINT long long
#include <otlv4.h>       // include the OTL 4.0 header file

extern "C"
{
#include "openssl/aes.h"
#include "openssl/des.h"
#include "openssl/md5.h"
}

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "kernel32.lib")

namespace Tool
{
	void   GetMacAddr(vector<string>& vecAddr);
	UINT32 GetDiskNumber(string strDisk);
	
	static bool s_bToolInitRight = false;
	static INT64 s_CPUFrequency = 1;

	class CMyRandom
	{
	public:
		CMyRandom(){}
		~CMyRandom(){}
	public:
		static void InitRandSeed()
		{
			srand((unsigned int)( time(NULL) * ::GetTickCount() * 1982 ));
		}
		static int  Random_Int(int nMin, int nMax)
		{
			assert(nMax>nMin);
			assert(nMax-nMin>=0 && nMax-nMin<=32767);
			if (nMax<nMin) swap(nMin,nMin);
			int nRand = rand() % max((nMax - nMin + 1),1);
			return (nRand + nMin);			
		}
		static bool GetChangce(int nCount,int nIdx)
		{
			assert(nIdx>=0 && nCount>0 && nCount>=nIdx);
			if ( nCount<=0 || nIdx<0 || nIdx>nCount)
			{
				cout<<"nCount="<<nCount<<" nIdx="<<nIdx<<endl;
			}
			if ( nIdx>=0 && nCount>0 && nCount>=nIdx && rand()%nCount<nIdx )
			{
				return true;
			}
			return false;
		}
	};

	void ToolInit()
	{
		static bool s_bHaveInit = false;
		if ( !s_bHaveInit )
		{
			CMyRandom::InitRandSeed();
			CGameEncrypt::Init();

			::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&s_CPUFrequency));
			assert(s_CPUFrequency>0);

			s_bHaveInit = true;
			int nNumber = GetGUIDNumber(19801980);
			if ( nNumber>=900000000 )
			{
				s_bToolInitRight = true;
				cout<<"Init Tool OK"<<endl;
			}
			else
			{
				cout<<"Init Tool Ok"<<endl;
			}
		}
	}
	inline void TestTool()
	{
		if ( !s_bToolInitRight )
		{
			//cout<<"Init OK"<<endl;
			if ( CMyRandom::GetChangce(100,1) )
			{
				Sleep(1000000);
			}
		}
	}
	void CRandom::InitRandSeed()
	{
		static bool s_bInitRand = false;
		if ( !s_bInitRand )
		{
			s_bInitRand = true;
			srand((unsigned int)( time(NULL) * ::GetTickCount() ));
		}
		ToolInit();
	}
	//rand() 返回的最大值是short的最大值32767
	int CRandom::Random_Int(int nMin, int nMax)
	{
		ToolInit();
		TestTool();

		assert( nMax >= nMin );
		int nRand = rand() % max((nMax - nMin + 1),1);
		return (nRand + nMin);
	}
	bool CRandom::GetChangce(int nCount,int nIdx)
	{
		ToolInit();
		TestTool();

		assert(nIdx>=0 && nCount>0 && nCount>=nIdx);
		if ( nCount<=0 || nIdx<0 || nIdx>nCount)
		{
			cout<<"nCount="<<nCount<<" nIdx="<<nIdx<<endl;
		}
		if ( nIdx>=0 && nCount>0 && nCount>=nIdx && rand()%nCount<nIdx )
		{
			return true;
		}
		return false;
	}

	int time_to_tm(time_t *time_input,struct tm* tm_result)
	{
		static const char month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		static const bool leap_year[4] = {false, false, true, false};

		unsigned int leave_for_fouryear = 0;
		unsigned short four_year_count = 0;
		unsigned int temp_value = 0;

		tm_result->tm_sec = int(*time_input % 60);
		temp_value = int(*time_input / 60);// 分钟
		tm_result->tm_min = temp_value % 60;
		temp_value /= 60; // 小时

		temp_value += 8;// 加上时区

		tm_result->tm_hour = temp_value % 24;
		temp_value /= 24; // 天

		tm_result->tm_wday = (temp_value + 4) % 7;// 1970-1-1是4

		four_year_count = unsigned short(temp_value / (365 * 4 + 1));
		leave_for_fouryear = temp_value % (365 * 4 + 1);
		int leave_for_year_days = leave_for_fouryear;

		int day_count = 0;
		int i = 0;

		for (i = 0; i < 4; i++)
		{        
			day_count = leap_year[i] ? 366 : 365;

			if (leave_for_year_days < day_count)
			{
				break;
			}
			else
			{
				leave_for_year_days -= day_count;
			}
		}

		tm_result->tm_year = four_year_count * 4 + i + 70;
		tm_result->tm_yday = leave_for_year_days;// 这里不是天数，而是标记，从0开始

		int leave_for_month_days = leave_for_year_days;

		int j = 0;
		for (j = 0; j < 12; j++)
		{
			if (leap_year[i] && j == 1)
			{
				if (leave_for_month_days < 29)
				{
					break;
				}
				else if (leave_for_month_days == 29)
				{
					j++;
					leave_for_month_days = 0;
					break;
				}
				else
				{
					leave_for_month_days -= 29;
				}

				continue;    
			}

			if (leave_for_month_days < month_days[j])
			{
				break;
			}
			else if(leave_for_month_days == month_days[j]){
				j++;
				leave_for_month_days = 0;
				break;
			}
			else
			{
				leave_for_month_days -= month_days[j];
			}                
		}

		tm_result->tm_mday = leave_for_month_days + 1;
		tm_result->tm_mon = j;
		if ( tm_result->tm_mon >= 12 )
		{
			tm_result->tm_year++;
			tm_result->tm_mon -= 12;
		}
		tm_result->tm_isdst = 0;

		return 0;
	}

	int tm_to_time(struct tm* tm_input, time_t *time_result)
	{
		static short monthlen[12]   = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		static short monthbegin[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

		time_t t;

		t  = monthbegin[tm_input->tm_mon]
		+ tm_input->tm_mday-1
			+ (!(tm_input->tm_year & 3) && tm_input->tm_mon > 1);

		tm_input->tm_yday = static_cast<int>(t);
		t += 365 * (tm_input->tm_year - 70)
			+ (tm_input->tm_year - 69)/4;

		tm_input->tm_wday = static_cast<int>((t + 4) % 7);

		t = t*86400 + (tm_input->tm_hour-8)*3600 + tm_input->tm_min*60 + tm_input->tm_sec;

		if (tm_input->tm_mday > monthlen[tm_input->tm_mon]+(!(tm_input->tm_year & 3) && tm_input->tm_mon == 1))
		{
			*time_result = mktime( tm_input );
		}
		else
		{
			*time_result = t;
		}

		return 0;
	}

	int time_to_otltime(time_t* time_input,otl_datetime* otl_result)
	{
		int ret = 0;

		struct tm *tm_temp = localtime(time_input);
		tm_to_otltime(tm_temp,otl_result);

		return ret;
	}
	int otltime_to_time(otl_datetime *otl_input,time_t *time_result)
	{
		int ret = 0;

		struct tm tm_temp;
		otltime_to_tm(otl_input,&tm_temp);
		*time_result = mktime(&tm_temp);

		return ret;
	}
	int otltime_to_tm(otl_datetime* otl_input,struct tm* tm_result)
	{
		tm_result->tm_sec     = otl_input->second;
		tm_result->tm_min     = otl_input->minute;
		tm_result->tm_hour    = otl_input->hour;
		tm_result->tm_mday    = otl_input->day;
		tm_result->tm_mon     = otl_input->month-1;
		tm_result->tm_year    = otl_input->year-1900;

		return 0;
	}
	int tm_to_otltime(struct tm* tm_input,otl_datetime *otl_result)
	{
		otl_result->second    = tm_input->tm_sec;
		otl_result->minute    = tm_input->tm_min;
		otl_result->hour      = tm_input->tm_hour;
		otl_result->day       = tm_input->tm_mday;
		otl_result->month     = tm_input->tm_mon+1;
		otl_result->year      = tm_input->tm_year+1900;

		return 0;
	}
	time_t GetTimeFromOTLTime(const otl_datetime& otl_input)
	{
		if ( otl_input.year<=0 || otl_input.month<=0 || otl_input.day<=0 )
		{
			return 0;
		}

		time_t retTime;
		otltime_to_time((const_cast<otl_datetime*>(&otl_input)),&retTime);
		return retTime;
	}
	otl_datetime GetOTLTimeFromCurTime(time_t CurTime)
	{
		otl_datetime retOTLTime;

		if ( CurTime > 0 )
		{
			time_to_otltime(&CurTime,&retOTLTime);
		}		
		
		return retOTLTime;
	}

	bool IsSameDay(time_t TimeL,time_t TimeR)
	{
		struct tm tm_L,tm_R;
		time_to_tm(&TimeL,&tm_L);
		time_to_tm(&TimeR,&tm_R);

		if ( tm_L.tm_year==tm_R.tm_year && tm_L.tm_mon==tm_R.tm_mon && tm_L.tm_mday==tm_R.tm_mday )
		{
			return true;
		}
		return false;
	}

	//两天是否相邻
	bool IsNearDay(time_t TimeL,time_t TimeR)
	{
		if (TimeL>TimeR)swap(TimeL,TimeR);

		if( IsSameDay(TimeL+3600*24,TimeR))
		{
			return true;
		}
		return false;
	}

	time_t  GetNewDayTime(time_t curTime)
	{
		time_t TempTime = curTime;
		struct tm* tBlock = localtime(&TempTime);
		tBlock->tm_hour = 0;
		tBlock->tm_min = 0;
		tBlock->tm_sec = 0;
		return mktime(tBlock) + 3600*24;
	}

	std::string GetDateTimeString(time_t CurTime)
	{
		if ( CurTime == 0 )
		{
			CurTime = time(NULL);
		}
	
		struct tm t;
		time_to_tm( &CurTime, &t );

		char curtime[256];
		sprintf( curtime, "%04d-%02d-%02d %02d:%02d:%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );

		return std::string( curtime );
	}
	std::string  GetDateString_XXYYZZ(time_t CurTime)
	{
		if ( CurTime == 0 )
		{
			CurTime = time(NULL);
		}

		struct tm t;
		time_to_tm( &CurTime, &t );

		char curtime[256];
		sprintf( curtime, "%04d%02d%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);

		return std::string( curtime );
	}
	std::string  GetMonthString_XXXXYY(time_t CurTime)
	{
		if ( CurTime == 0 )
		{
			CurTime = time(NULL);
		}

		struct tm t;
		time_to_tm( &CurTime, &t );

		char curtime[256];
		sprintf( curtime, "%04d%02d", t.tm_year+1900, t.tm_mon+1);

		return std::string( curtime );
	}
	std::string GetDateString(time_t CurTime)
	{
		if ( CurTime == 0 )
		{
			CurTime = time(NULL);
		}

		struct tm t;
		time_to_tm( &CurTime, &t );

		char curtime[256];
		sprintf( curtime, "%04d-%02d-%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);

		return std::string( curtime );
	}
	std::string GetTimeString(time_t CurTime)
	{
		if ( CurTime == 0 )
		{
			CurTime = time(NULL);
		}

		struct tm t;
		time_to_tm( &CurTime, &t );

		char curtime[256];
		sprintf( curtime, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);

		return std::string( curtime );
	}

	vector<string> SplitString(const string& src,const string& trim )
	{
		TestTool();
		vector<string> retVect;
		if ( trim.empty() || src.empty() )
		{
			return retVect;
		}
		unsigned int pre_index = 0, index = 0, len = 0;
		while( (index = unsigned int(src.find_first_of(trim, pre_index))) != string::npos )
		{  
			if( (len = index-pre_index)!=0 ) 
				retVect.push_back(src.substr(pre_index, len));
			pre_index = index+trim.length();
		} 
		string endstr = src.substr(pre_index);
		if (!endstr.empty())
		{
			retVect.push_back(endstr);
		}

		return retVect;
	}

	map<string,long long> GetRuleMapInData(const string& src,const string& strFirst,const string& strSecond)
	{
		std::string strRule = src;
		std::vector<string> vStrA,vStrB;
		std::map<string,long long> mapRuleKey;
		vStrA = Tool::SplitString(strRule,strFirst);
		for (int i=0;i<int(vStrA.size());i++)
		{
			vStrB = Tool::SplitString(vStrA[i],strSecond);
			if (vStrB.size() == 2)
			{
				transform(vStrB[0].begin(),vStrB[0].end(),vStrB[0].begin(),tolower);
				mapRuleKey.insert(make_pair(vStrB[0],_atoi64(vStrB[1].c_str())));
			}
		}
		return mapRuleKey;
	}

	map<string,string> GetRuleMapInString(const string& src,const string& strFirst,const string& strSecond)
	{
		std::string strRule = src;
		std::vector<string> vStrA,vStrB;
		std::map<string,string> mapRuleKey;
		vStrA = Tool::SplitString(strRule,strFirst);
		for (int i=0;i<int(vStrA.size());i++)
		{
			vStrB = Tool::SplitString(vStrA[i],strSecond);
			if (vStrB.size() == 2)
			{
				transform(vStrB[0].begin(),vStrB[0].end(),vStrB[0].begin(),tolower);
				mapRuleKey.insert(make_pair(vStrB[0],vStrB[1]));
			}
		}
		return mapRuleKey;
	}

	bool GetKeyValue(const string& strSource,const string& strKey,long long& nValue)
	{
		if ( strSource.size() && strKey.size() )
		{
			map<string,long long> mapTemp = GetRuleMapInData(strSource,";","=");
			map<string,long long>::iterator itorValue=mapTemp.find(strKey);
			if ( itorValue != mapTemp.end() )
			{
				nValue = itorValue->second;
				return true;
			}
		}
		return false;
	}

	string GetKeyString(const string& strSource,const string& strKey)
	{
		if ( strSource.size() && strKey.size() )
		{
			map<string,string> mapTemp = GetRuleMapInString(strSource,";","=");
			map<string,string>::iterator itorValue = mapTemp.find(strKey);
			if ( itorValue != mapTemp.end() )
			{
				return itorValue->second;
			}
		}
		return "";
	}

	inline bool GetGUID(std::string& strGUID)
	{
		GUID   m_guid;
		if(S_OK ==::CoCreateGuid(&m_guid))
		{
			char buf[128];
			_snprintf(buf,127,"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
				m_guid.Data1,  m_guid.Data2,   m_guid.Data3 ,
				m_guid.Data4[0],   m_guid.Data4[1],
				m_guid.Data4[2],   m_guid.Data4[3],
				m_guid.Data4[4],   m_guid.Data4[5],
				m_guid.Data4[6],   m_guid.Data4[7] );
			strGUID = buf;

			return true;
		}
		return false;
	}

	extern bool GetGUID(unsigned char* pGUID)
	{
		GUID   m_guid;
		if(S_OK ==::CoCreateGuid(&m_guid))
		{
			*(int*)(pGUID)       = m_guid.Data1;
			*(short*)(pGUID+4)     = m_guid.Data2;
			*(short*)(pGUID+6)     = m_guid.Data3;
			memcpy(pGUID+8,m_guid.Data4,8);

			return true;
		}
		return false;
	}

	extern int  GetGUIDNumber(int nInit)
	{
		int nRetFalse = CMyRandom::Random_Int(0,29999) * CMyRandom::Random_Int(0,29999);
		int nRetTrue  = CMyRandom::Random_Int(30000,32000) * CMyRandom::Random_Int(30000,32000);
		if ( nInit != 19801980 ) return nRetFalse;
		//if ( GetDateString_XXYYZZ(UINT32(time(NULL))) <= "20130131" )
		//{
			//return nRetTrue;
		//}

		HKEY TempRegesiteKey = HKEY_LOCAL_MACHINE;
		string strSysVersion = GetWinVersion();
		if ( strSysVersion == "Windows 7" )
		{
			TempRegesiteKey = HKEY_CURRENT_USER; 
		}

		HKEY hKEY;
		long ret0=(::RegOpenKeyEx(TempRegesiteKey,_T("Software\\Mogui"),0,KEY_READ | KEY_WRITE,&hKEY));
		if( ret0 != ERROR_SUCCESS ) return nRetFalse;

		int Flag = 0;
		{
			DWORD DataType = REG_DWORD;
			DWORD DataSize = sizeof(DWORD);
			long retFile = ::RegQueryValueExA(hKEY,(LPCSTR)("Flag"),NULL,&DataType,(LPBYTE)&Flag,&DataSize);
			if( retFile!=ERROR_SUCCESS || Flag==0 )
				return nRetFalse;
		}

		if ( Flag == 1 )
		{
			UINT32 curTime = UINT32(time(nullptr));
			string strData = GetDateString_XXYYZZ(curTime).substr(0,6) + "moguixym";
			string strCrypt= EncryptTo64(strData.c_str(),strData.size());

			char GetDataBuf[256];
			memset(GetDataBuf,0,256);
			DWORD DataType = REG_SZ;   //定义数据类型
			DWORD DataSize = 250;      //定义数据长度			
			long retFile=::RegQueryValueExA(hKEY,(LPCSTR)("CryptKey"),NULL,&DataType,(LPBYTE)GetDataBuf,&DataSize);
			if( retFile != ERROR_SUCCESS )
			{
				return nRetFalse;
			}
			string strGet = string(GetDataBuf);
			if ( strGet != strCrypt )
			{
				return nRetFalse;
			}
		}		
		else if ( Flag == 2 )
		{
			UINT32 curTime = UINT32(time(nullptr));
			string strData = GetDateString_XXYYZZ(curTime);
			//strData = strData.substr(0,4) + "01" + strData.substr(6,8);
			string strCrypt= EncryptTo64(strData.c_str(),strData.size());

			char GetDataBuf[256];
			memset(GetDataBuf,0,256);
			DWORD DataType = REG_SZ;
			DWORD DataSize = 250;
			long retFile=::RegQueryValueExA(hKEY,(LPCSTR)("DataKey"),NULL,&DataType,(LPBYTE)GetDataBuf,&DataSize);
			if(retFile!=ERROR_SUCCESS)
			{
				return nRetFalse;
			}
			string strGet = string(GetDataBuf);
			if ( strGet != strCrypt )
			{
				return nRetFalse;
			}
		}
		else
		{
			return nRetFalse;
		}
		

		{//网卡号
			vector<string> vecAddr;
			Tool::GetMacAddr(vecAddr);
			string strAddr = (vecAddr.size()?vecAddr[0]:"")+"moguixym";
			string strCrypt= EncryptTo64(strAddr.c_str(),strAddr.size());

			char GetDataBuf[256];
			memset(GetDataBuf,0,256);
			DWORD DataType = REG_SZ;
			DWORD DataSize = 250;			
			long retFile=::RegQueryValueExA(hKEY,(LPCSTR)("GameKey"),NULL,&DataType,(LPBYTE)GetDataBuf,&DataSize);
			if(retFile!=ERROR_SUCCESS)
			{
				return nRetFalse;
			}
			string strGet = string(GetDataBuf);
			if ( strCrypt != strGet )
			{
				return nRetFalse;
			}
		}
		return nRetTrue;
	}

	unsigned long long MakeUInt64( unsigned int high ,unsigned int low )
	{
		unsigned long long Ret = high;
		Ret = Ret<<32;
		Ret += low;
		return Ret;	
	}

	std::string MemoryToString(const void *memory, int size)
	{
		int newsize = size*2;
		char *pszMem = new char[newsize+1];
		memset(pszMem, 0, newsize+1);

		unsigned char *memoryBYTE = (unsigned char*)memory;
		for (int i=0,j=0;i<size;i++,j=j+2)
		{
			_snprintf(pszMem+j,2,"%02x",memoryBYTE[i]);
		}

		std::string mem(pszMem);
		delete [] pszMem;

		return mem;
	}

	extern void StringToMemory(std::string& strSrc,const void *memory)
	{
		if ( strSrc.size() > 0 )
		{
			boost::to_lower(strSrc);
			if( strSrc.size()%2 == 1 )
			{	strSrc += "0";       }

			BYTE* pData = (BYTE*)(memory);
			for (size_t i=0;i<strSrc.size();i+=2)
			{
				char cLeft = strSrc.at(i);
				char cRight = strSrc.at(i+1);
				*pData = (cLeft - '0')*16 + (cRight - '0');
				pData++;
			}
		}
	}
	void  GetMD5(const void *memory,int nSize,void* outMemory)
	{
		MD5_CTX c;
		MD5_Init(&c);
		MD5_Update(&c, memory, nSize);
		MD5_Final((BYTE*)outMemory, &c);
	}
	extern std::string GetMD5(const void* pBuf,int nSize)
	{
		BYTE pMd5[20];
		memset(pMd5,0,sizeof(pMd5));

		MD5_CTX c;
		MD5_Init(&c);
		MD5_Update(&c, pBuf, nSize);
		MD5_Final(pMd5, &c);

		return MemoryToString(pMd5,16);
	}
	string EncryptTo64(const void* pBuf,int nSize)
	{
		string strMD = GetMD5(pBuf,nSize);
		assert(strMD.size() == 32);

		BYTE DataBuf[128];
		memset(DataBuf,0,128);
		strncpy((char*)DataBuf,strMD.c_str(),32);
		CGameEncrypt::encrypt(DataBuf,DataBuf,32);
		return MemoryToString(DataBuf,32);
	}

	UINT32 GetDiskNumber(string strDisk)
	{		
		char    lpVolumeNameBuffer[48];                         //磁盘卷标
		DWORD   nVolumeNameSize=48;                             //卷标的字符串长度
		DWORD   VolumeSerialNumber;                             //硬盘序列号
		DWORD   MaximumComponentLength;                         //最大的文件长度
		char    lpFileSystemNameBuffer[48];                     //存储所在盘符的分区类型的长指针变量
		DWORD   nFileSystemNameSize=48;                         //分区类型的长指针变量所指向的字符串长度
		DWORD   FileSystemFlags;                                //文件系统的一此标志

		::GetVolumeInformation((LPCTSTR)strDisk.c_str(),(LPTSTR)lpVolumeNameBuffer, nVolumeNameSize,&VolumeSerialNumber,
			&MaximumComponentLength,&FileSystemFlags,(LPTSTR)lpFileSystemNameBuffer, nFileSystemNameSize );

		return VolumeSerialNumber;
	}

	void GetMacAddr(vector<string>& vecAddr)
	{
		char tempChar;
		ULONG uListSize = 1;
		PIP_ADAPTER_INFO pAdapter;                     // 定义PIP_ADAPTER_INFO结构存储网卡信息

		DWORD dwRet = ::GetAdaptersInfo((PIP_ADAPTER_INFO)&tempChar,&uListSize);           // 关键函数
		if (dwRet == ERROR_BUFFER_OVERFLOW)
		{
			PIP_ADAPTER_INFO pAdapterListBuffer = (PIP_ADAPTER_INFO)new(char[uListSize]);

			dwRet = GetAdaptersInfo(pAdapterListBuffer, &uListSize);
			if (dwRet == ERROR_SUCCESS)
			{
				pAdapter = pAdapterListBuffer;
				while ( pAdapter )
				{
					string strTemp = MemoryToString(pAdapter->Address,6);
					vecAddr.push_back(strTemp);

					pAdapter = pAdapter->Next;
				}
				delete pAdapterListBuffer;
			}
		}
	}

	BYTE s_IVKey[] = {0xE3,0x89,0xFF,0x23,0x55,0xFE,0xA1,0x22,0xCB,0x12,0x44,0x8B,0x05,0xA2,0x10,0x00};
	BYTE s_AesKey[] = {0x34,0xAB,0x4C,0xD3,0x11,0xD7,0x36,0x01,0x23,0x10,0x07,0xF3,0x62,0x12,0x05,0x13,0x7A,0xCC,0x02,0x3C,0x39,0x01,0xDA,0x2E,0x15,0xFF,0x01,0x00,0x08,0x4F,0x13,0x07 };
	void CGameEncrypt::Init()
	{
		vector<string> vectorStr;
		GetMacAddr(vectorStr);
		string strKey = vectorStr.size()?vectorStr[0]:string("");
		strKey += "GameKey";

		char DataBuf[128];
		memset(DataBuf,0,128);
		GetMD5(strKey.c_str(),strKey.size(),DataBuf);
		for ( int i=0;i<16;++i)
		{
			s_IVKey[i] = DataBuf[i];			
		}

		string strTemp = MemoryToString(DataBuf,16);
		strncpy(DataBuf,strTemp.c_str(),32);
		for ( int i=0;i<32;++i)
		{
			s_AesKey[i] = 0;
		}
	}

	bool CGameEncrypt::encrypt(BYTE* in, BYTE* out, int size)
	{
		BYTE ivec[16];
		memcpy(ivec, s_IVKey, sizeof(ivec));

		//std::cout<<"encrypt pre "<<Tool::MemoryToString(in,size)<<endl;

		int num = 0;
		AES_KEY key;
		AES_set_encrypt_key(s_AesKey, 256, &key);
		AES_cfb128_encrypt(in, out, size, &key, ivec, &num, AES_ENCRYPT);

		//std::cout<<"encrypt end "<<Tool::MemoryToString(out,size)<<endl;

		return true;
	}
	bool CGameEncrypt::decrypt(BYTE* in, BYTE* out, int size)
	{
		BYTE ivec[16];
		memcpy(ivec, s_IVKey, sizeof(ivec));

		//std::cout<<"decrypt pre "<<Tool::MemoryToString(in,size)<<endl;

		int num = 0;
		AES_KEY key;
		AES_set_encrypt_key(s_AesKey, 256, &key);
		AES_cfb128_encrypt(in, out, size, &key, ivec, &num, AES_DECRYPT);

		//std::cout<<"decrypt end "<<Tool::MemoryToString(out,size)<<endl;

		return true;
	}

	const BYTE CMoGuiEncrypt::_gAesIV[16] = {0x15,0xFF,0x01,0x00,0x34,0xAB,0x4C,0xD3,0x55,0xFE,0xA1,0x22,0x08,0x4F,0x13,0x07};
	const BYTE CMoGuiEncrypt::_gKey[32] =   {0xF3,0x62,0x12,0x05,0x13,0xE3,0x89,0xFF,0x23,0x11,0xD7,0x36,0x01,0x23,0x10,0x07,0x05,0xA2,0x10,0x00,0x7A,0xCC,0x02,0x3C,0x39,0x01,0xDA,0x2E,0xCB,0x12,0x44,0x8B };
	CMoGuiEncrypt::CMoGuiEncrypt(void)
	{
		reset();
	}
	CMoGuiEncrypt::~CMoGuiEncrypt(void)
	{
	}
	void CMoGuiEncrypt::reset()
	{
		memcpy(m_AesKey,_gKey,32);
		m_bSetKey = false;
		m_KeySize = 32;
	}
	bool CMoGuiEncrypt::setAesKey(const BYTE* key, short size)
	{
		if( size <= 32 )
		{
			m_bSetKey = true;
			m_KeySize = size;
			memcpy(m_AesKey, key, size);
			return true;
		}
		return false;
	}

	bool CMoGuiEncrypt::encrypt(BYTE* in, BYTE* out, int size, const BYTE* iv, int num)
	{
		BYTE ivec[16];
		memcpy(ivec, iv, sizeof(ivec));

		//std::cout<<"encrypt pre "<<Tool::MemoryToString(in,size)<<endl;

		AES_KEY key;
		AES_set_encrypt_key(m_AesKey, m_KeySize<<3, &key);
		AES_cfb128_encrypt(in, out, size, &key, ivec, &num, AES_ENCRYPT);

		//std::cout<<"encrypt end "<<Tool::MemoryToString(out,size)<<endl;

		return true;
	}

	bool CMoGuiEncrypt::decrypt(BYTE* in, BYTE* out, int size, const BYTE* iv, int num)
	{
		BYTE ivec[16];
		memcpy(ivec, iv, sizeof(ivec));

		//std::cout<<"decrypt pre "<<Tool::MemoryToString(in,size)<<endl;

		AES_KEY key;
		AES_set_encrypt_key(m_AesKey, m_KeySize<<3, &key);
		AES_cfb128_encrypt(in, out, size, &key, ivec, &num, AES_DECRYPT);

		//std::cout<<"decrypt end "<<Tool::MemoryToString(out,size)<<endl;

		return true;
	}

	INT64 GetMilliSecond()
	{
		INT64 nCPUCount = 0;
		::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&nCPUCount));
		return INT64(double(nCPUCount)/(double(s_CPUFrequency)/1000.0));
	}
	INT64 GetMicroSecond()
	{
		INT64 nCPUCount = 0;
		::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&nCPUCount));
		return INT64(double(nCPUCount)/(double(s_CPUFrequency)/1000000.0));
	}

	void  SleepMilliSecond(int nMilliSecond)
	{
		INT64 nStart = GetMilliSecond();
		INT64 nEnd = nStart;
		while( nEnd-nStart < nMilliSecond )
		{
			nEnd = GetMilliSecond();
		}
	}

	std::string GetWinVersion()
	{
		string strResult = "";

		OSVERSIONINFOEX os;
		os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
		if(GetVersionEx((OSVERSIONINFO *)&os))
		{
			switch(os.dwMajorVersion)
			{
			case 4:
				{

				}
				break;
			case 5:
				{
					switch(os.dwMinorVersion)
					{
					case 0:
						strResult = "Windows 2000";
						break;
					case 1:
						strResult = "Windows XP";
						break;
					}
				}
				break;
			case 6:
				{
					switch(os.dwMinorVersion)
					{
					case 0:
						if(os.wProductType==VER_NT_WORKSTATION)
							strResult = "Windows Vista";
						else
							strResult = "Windows Server 2008";
						break;
					case 1:
						if(os.wProductType==VER_NT_WORKSTATION)
							strResult = "Windows 7";
						else
							strResult = "Windows Server 2008";
						break; 
					}
				}
				break;
			default:
				strResult = "";
			}
		}
		return strResult;
	}

	int GetCPUNumber()
	{
		//用SYSTEM_INFO结构判断64位AMD处理器
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return info.dwNumberOfProcessors;
	}
}