#include "InsideBase.h"

#include <Include/base/Lock.h>
#include <Include/base/Utils.h>

#include <Include/base/Condition.h>

namespace AGBase
{

#if defined _WINDOWS_ || defined _WINDOWS_

CCondition::CCondition( void )
{
	m_condid = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	assert(m_condid != INVALID_HANDLE_VALUE);
}

CCondition::~CCondition( void )
{
	if ( m_condid!=INVALID_HANDLE_VALUE )
		::CloseHandle( m_condid );
}

bool CCondition::Wait (CLock& locker)
{
	bool ret = false;
	locker.Unlock();
	if( ::WaitForSingleObject( m_condid, INFINITE )==WAIT_OBJECT_0 )
		ret = true;
	locker.Lock();

	::ResetEvent( m_condid );

	return ret;
}

bool CCondition::Wait (CLock& locker, unsigned int ms)
{
	bool ret = false;
	locker.Unlock();
	if( ::WaitForSingleObject( m_condid, ms )==WAIT_OBJECT_0 )
		ret = true;
	locker.Lock();

	::ResetEvent( m_condid );

	return ret;
}

void CCondition::Signal( void )
{
	::SetEvent( m_condid );
}

void CCondition::Broadcast( void )
{
	::SetEvent( m_condid );
}

#else

#endif

}
