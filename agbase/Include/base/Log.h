#pragma once

#include <string>
#include <sstream>

// Log���̰߳�ȫ
//

namespace AGBase
{

	enum LogLevel
	{
		// log debug ��־
		LOGLEVEL_DEBUG	= 0,
		// log info ��־
		LOGLEVEL_INFO	= 1,
		// log warning ��־
		LOGLEVEL_WARN	= 2,
		// log error ��־
		LOGLEVEL_ERROR	= 3,
	};

	const int LOGLEVEL_ALL = LOGLEVEL_DEBUG;
	const int LOGLEVEL_NULL= 255;
	typedef void (*logfun)( int level, const char* str );

	// ��ʼ��Log��һ������ֻ��InitLoggerһ��
	void InitLogger( const char* prefix, bool log2file, int level = LOGLEVEL_ALL );
	// ����Log��һ������ֻ��FiniLoggerһ��
	void FiniLogger( void );
	// ������־�ȼ�
	void SetLogLevel( int level );
	// �����־�ص�����
	void SetLogHandle( logfun loghandle );
	// д�ַ���־
	void Log_Text( int level, const char* str );
	// д�ַ���־
	void Log_Text_Format( int level, char* szstr, ... );
	// д16������־
	void Log_Hex( int level, const char * hex, size_t len );

// ����д��־
#define LOG_STREAM( level, os ) \
{\
	std::ostringstream ss;\
	ss<<os;\
	std::string str( ss.str() ); \
	AGBase::Log_Text( level, (const char *)str.c_str() );\
}

#define LOGI( os ) LOG_STREAM( AGBase::LOGLEVEL_INFO, os )
#define LOGD( os ) LOG_STREAM( AGBase::LOGLEVEL_DEBUG, os )
#define LOGW( os ) LOG_STREAM( AGBase::LOGLEVEL_WARN, os )
#define LOGE( os ) LOG_STREAM( AGBase::LOGLEVEL_ERROR, os )

};

//#endif // __AGBASE_LOG_H__

