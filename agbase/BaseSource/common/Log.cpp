#if _MSC_VER>=1400
#define _CRT_SECURE_NO_DEPRECATE
#endif

#if defined _WINDOWS_ || defined WIN32
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif

#if defined _WINDOWS_ || defined WIN32
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

#include <iostream>
#include <fstream>
#include <queue>

#include "InsideBase.h"
#include <Include/base/Lock.h>
#include <Include/base/MemoryPool.h>
#include <Include/base/Utils.h>
#include <Include/base/Thread.h>
#include <Include/base/Log.h>

namespace AGBase
{

static const int LOGLEVEL_NULL_IN= LOGLEVEL_ERROR+1;
static const char* g_logDesc[LOGLEVEL_NULL_IN]={" [DEBUG] ", " [INFO ] ", " [WARN ] ", " [ERROR] "};

#define MAX_LOG_LEN		1024
#define MAX_LOG_HEX_LEN 4104
#define MAX_LOG_COUNT   1024

class CLogThread : public CThread
{
public:
	class CLogPacket : public CMemoryPool_Public<CLogPacket, 5>
	{
	public:
		unsigned char	level;
		unsigned char	type;
		int		len;
		char	log[MAX_LOG_LEN+1];
	};

	CLogThread( void ) : CThread( "logthread" )
	{
		m_init			= false;
		m_bDirCreated	= false;
		m_nLogLevel		= LOGLEVEL_NULL_IN;
		m_bLog2File		= false;
		m_nDate			= 0;
		m_nMonth		= 0;

		m_logex			= false;
		m_loghandle		= 0;
	}

	void Init( const char* prefix, bool log2file, int level )
	{
		CSelfLock l(m_packetlock);

		if ( m_init ) return;

		m_init = true;
		m_sLogRootDir = AGBase::GetModulePath()+"log/";
		m_nLogLevel = level;

		if ( m_nLogLevel<LOGLEVEL_ALL )			m_nLogLevel=LOGLEVEL_ALL;
		else if( m_nLogLevel>=LOGLEVEL_NULL )	m_nLogLevel=LOGLEVEL_NULL;

		m_bLog2File = log2file;
		if ( prefix )	m_sFilePrefix = prefix;

		CreateMonthDir();

		Start( );
	}

	void Fini( void )
	{
		CSelfLock l(m_packetlock);

		Terminate( );

		if( m_fp.is_open() )
		{
			m_fp.close();
		}

		CLogPacket* packet = 0;
		while ( !m_packets.empty() )
		{
			packet = m_packets.front( );
			m_packets.pop_front();

			delete packet;
		}

		m_init = false;
	}

	void SetLogLevel( int level )
	{
		CSelfLock l(m_packetlock);
		m_nLogLevel = level;

		if ( m_nLogLevel<LOGLEVEL_ALL )			m_nLogLevel=LOGLEVEL_ALL;
		else if( m_nLogLevel>=LOGLEVEL_NULL )	m_nLogLevel=LOGLEVEL_NULL;
	}

	void SetLogHandle( logfun loghandle )
	{
		CSelfLock l( m_logexlock );

		m_loghandle = loghandle;
		if ( m_loghandle )
		{
			m_logex = true;
		}
		else
		{
			m_logex = false;
		}
	}

	void LogText( int level, const char* str )
	{
		if( !m_init ) return;
		if( level<m_nLogLevel )				return;
		if( m_nLogLevel==LOGLEVEL_NULL )	return;

		int pos = 0;
		int leftlen = strlen(str);
		do 
		{
			CLogThread::CLogPacket* packet = new CLogThread::CLogPacket( );

			packet->level = level;
			packet->type  = 0;

			if ( leftlen>MAX_LOG_LEN )
			{
				memcpy( packet->log+pos, str, MAX_LOG_LEN );
				packet->len = MAX_LOG_LEN;
			}
			else
			{
				memcpy( packet->log+pos, str, leftlen );
				packet->len = leftlen;
			}
			packet->log[ packet->len ] = 0;
			leftlen -= packet->len;
			pos		+= packet->len;

			PostLog( packet );

		} while ( leftlen>0 );
	}

	void LogHex( int level, const char * hex, size_t len )
	{
		if( !m_init ) return;
		if( level<m_nLogLevel )				return;
		if( m_nLogLevel==LOGLEVEL_NULL )	return;

		int pos = 0;
		int leftlen = len;

		do 
		{
			CLogThread::CLogPacket* packet = new CLogThread::CLogPacket( );

			packet->level = level;
			packet->type  = 1;

			if ( leftlen>MAX_LOG_LEN )
			{
				memcpy( packet->log+pos, hex, MAX_LOG_LEN );
				packet->len = MAX_LOG_LEN;
			}
			else
			{
				memcpy( packet->log+pos, hex, leftlen );
				packet->len = leftlen;
			}
			packet->log[ packet->len ] = 0;
			leftlen -= packet->len;
			pos		+= packet->len;

			PostLog( packet );

		} while ( leftlen>0 );
	}

	void PostLog( CLogPacket* logpacket )
	{
		if( 0==logpacket )	return;

		CSelfLock l(m_packetlock);

		if ( m_packets.size()>=MAX_LOG_COUNT )
		{
			delete logpacket;
			return;
		}

		m_packets.push_back( logpacket );
	}

protected:
	int Run( void )
	{
		CLogPacket* packet = 0;
		while ( IsRunning() )
		{
			{
				CSelfLock l(m_packetlock);

				if ( !m_packets.empty() )
				{
					packet = m_packets.front( );
					m_packets.pop_front();
				}
			}

			if ( packet==0 )
			{
				SleepMillisecond( 200 );
			}
			else
			{
				if (packet->type==0)
				{
					Log_Text_2File( packet->level, packet->log );
				}
				else
				{
					Log_Hex_2File( packet->level, packet->log, packet->len );
				}

				delete packet;
				packet = 0;
			}
		}

		return 0;
	}

	void CreateMonthDir()
	{
		time_t now = time(NULL);
		struct tm t;
		time_to_tm( &now, &t);

		char subDir[255];

		m_bDirCreated = false;

		sprintf( subDir, "%s%d-%d/", m_sLogRootDir.c_str(), t.tm_year+1900, t.tm_mon+1 );

		if( !AGBase::CreateDir( m_sLogRootDir.c_str() ) )	return;
		m_sLogDir = subDir;

		if( !AGBase::CreateDir( m_sLogDir.c_str()) )		return;
		m_bDirCreated = true;
		m_nMonth = t.tm_mon;
	}

	void Log_Text_2File( int level, const char* str )
	{
		if ( m_logex )
		{
			CSelfLock l( m_logexlock ); 

			if ( m_loghandle )
			{
				(*m_loghandle)( level, str );
			}
		}

		if( level>=LOGLEVEL_NULL_IN || level<0 )	return;

		if( !m_bLog2File )
		{
			std::cout<<AGBase::GetDataTimeString()<<g_logDesc[level]<<str<<std::endl;
			return;
		}

		time_t now		= time(NULL);
		struct tm		t;
		bool			bMonthChange = false;

		time_to_tm( &now, &t);
		
		if( t.tm_mon!=m_nMonth )
		{
			CreateMonthDir();
			bMonthChange = true;
		}

		if( !m_bDirCreated ) return;

		if( !m_fp.is_open() || m_nDate!=t.tm_mday || bMonthChange )
		{
			if( m_fp.is_open() )
			{
				m_fp.close();
			}

			char file[255];
			sprintf( file, "%s%s_%02d-%02d-%02d.log", m_sLogDir.c_str(), m_sFilePrefix.c_str(), t.tm_year+1900, t.tm_mon+1, t.tm_mday );
			m_fp.open( file, std::ios::out|std::ios::app);
			if ( !m_fp.is_open() )	return;
			m_nDate = t.tm_mday;
		}

		m_fp<<AGBase::GetDataTimeString()<<g_logDesc[level]<<str<<std::endl;
	}

	void Log_Hex_2File( int level, const char * hex, size_t len )
	{
		char buffer[MAX_LOG_HEX_LEN];

		sprintf( buffer, "(0x)" );

		size_t head = strlen( buffer );
		size_t pos = 0;
		while ( pos < len )
		{
			if ( ((1+pos)<<2) + head >= MAX_LOG_HEX_LEN )
			{
				break;
			}
			sprintf( buffer + (pos<<2) + head, "  %02X", (unsigned char)(hex[pos]) );
			++pos;
		}

		Log_Text_2File(level, buffer);
	}

public:
	CLock					m_packetlock;
	std::deque<CLogPacket*>	m_packets;
	volatile bool			m_init;

	std::string			m_sLogRootDir;
	std::string			m_sLogDir;
	std::string			m_sFilePrefix;
	bool				m_bDirCreated;
	int					m_nLogLevel;
	bool				m_bLog2File;
	std::fstream		m_fp;
	int					m_nDate;
	int					m_nMonth;

	bool				m_logex;
	CLock				m_logexlock;
	logfun				m_loghandle;
private:
	CLogThread(const CLogThread &l);
	CLogThread &operator=(const CLogThread &l);
};

static bool       s_InitLog = false;
static CLogThread g_logthread;

void InitLogger( const char* prefix, bool log2file, int level )
{
	if( !s_InitLog )
	{
		s_InitLog = true;
		g_logthread.Init( prefix, log2file, level );
	}
}

void FiniLogger( void )
{
	if( s_InitLog )	g_logthread.Fini( );
}

void SetLogLevel( int level )
{
	g_logthread.SetLogLevel( level );
}

void SetLogHandle( logfun loghandle )
{
	g_logthread.SetLogHandle( loghandle );
}

void Log_Text( int level, const char * str )
{
	g_logthread.LogText( level, str ); 
}

void Log_Text_Format( int level, char* szstr, ... )
{
	if( !g_logthread.m_init ) return;
	if( level<g_logthread.m_nLogLevel )				return;
	if( g_logthread.m_nLogLevel==LOGLEVEL_NULL )	return;

	CLogThread::CLogPacket* packet = new CLogThread::CLogPacket( );

	packet->level = level;
	packet->type  = 0;

	va_list args;
	va_start(args, szstr);
	packet->len = vsnprintf(packet->log, sizeof(packet->log)-1, szstr, args);
	va_end(args);
	packet->log[packet->len] = 0;

	g_logthread.PostLog( packet );
}

void Log_Hex( int level, const char * hex, size_t len )
{
	g_logthread.LogHex( level, hex, len );
}

}

