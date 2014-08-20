//=============================================================================
/*
*
*  @author guyongliang<guyongliang@gameabc.com>, ʢ������ 
*  @version 1.0.0.1
*  @date 2010-06-06
*
*  Copyright (C) 2010-2012 - All Rights Reserved
*/
//=============================================================================

#include "InsideBase.h"

#include <Include/base/Utils.h>
#include <Include/base/Semaphore.h>

namespace AGBase
{

CSemaphore::CSemaphore( unsigned int initcount )
{
	if (initcount > 2147483647) initcount = 2147483647;
#if defined _WINDOWS_ || defined WIN32
	m_sema = ::CreateSemaphore(NULL, initcount, 2147483647, NULL);
	assert( m_sema!=INVALID_HANDLE_VALUE );
#else
	int ret = ::sem_init(&m_sema, 0, initcount);
	assert(ret==0);
#endif
}

CSemaphore::~CSemaphore( void )
{
#if defined _WINDOWS_ || defined WIN32
	::CloseHandle (m_sema);
#else
	::sem_destroy(&m_sema);
#endif
}

bool CSemaphore::Post( void )
{
#if defined _WINDOWS_ || defined WIN32
	return ::ReleaseSemaphore (m_sema, 1, NULL) ? true : false;
#else
	return ::sem_post(&m_sema)==0;
#endif
}

bool CSemaphore::Wait( void )
{
#if defined _WINDOWS_ || defined WIN32
	return (::WaitForSingleObject (m_sema, INFINITE)==WAIT_OBJECT_0) ? true : false;
#else
	return ::sem_wait(&m_sema)==0;
#endif
}

bool CSemaphore::Wait( unsigned int ms )
{
#if defined _WINDOWS_ || defined WIN32
	return (::WaitForSingleObject (m_sema, ms)==WAIT_OBJECT_0) ? true : false;
#else

	unsigned long long expires64 = GetTimeMs() + ms;
	timespec expiresTS;
	expiresTS.tv_sec = expires64 / 1000;
	expiresTS.tv_nsec = (expires64 % 1000) * 1000000L;

	return ::sem_timedwait(&m_sema, &expiresTS)==0;
#endif
}

}
