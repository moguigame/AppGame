#pragma once

#include <string>
#include <sstream>

// Log是线程安全
//

namespace AGBase
{

	enum LogLevel
	{
		// log debug 日志
		LOGLEVEL_DEBUG	= 0,
		// log info 日志
		LOGLEVEL_INFO	= 1,
		// log warning 日志
		LOGLEVEL_WARN	= 2,
		// log error 日志
		LOGLEVEL_ERROR	= 3,
	};

	const int LOGLEVEL_ALL = LOGLEVEL_DEBUG;
	const int LOGLEVEL_NULL= 255;
	typedef void (*logfun)( int level, const char* str );

	// 初始化Log，一个程序只能InitLogger一次
	void InitLogger( const char* prefix, bool log2file, int level = LOGLEVEL_ALL );
	// 销毁Log，一个程序只能FiniLogger一次
	void FiniLogger( void );
	// 更改日志等级
	void SetLogLevel( int level );
	// 添加日志回调函数
	void SetLogHandle( logfun loghandle );
	// 写字符日志
	void Log_Text( int level, const char* str );
	// 写字符日志
	void Log_Text_Format( int level, char* szstr, ... );
	// 写16进制日志
	void Log_Hex( int level, const char * hex, size_t len );

// 用流写日志
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

