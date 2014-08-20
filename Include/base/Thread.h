#pragma once

#include <string>
#include "Lock.h"
#include "Condition.h"

namespace AGBase
{
	class CThread
	{
#if defined _WINDOWS_ || defined _WINDOWS_
#define INVALID_THREAD_VALUE INVALID_HANDLE_VALUE
		typedef HANDLE				THREADID;
#else
#define INVALID_THREAD_VALUE 0
		typedef pthread_t			THREADID;
#endif
	public :
		CThread(const std::string &name="");
		virtual ~CThread( void );

		std::string	GetName() const;

		// �Ƿ��߳�ѭ������
		bool IsRunning( void );
		// �Ƿ�ֹͣ
		bool IsStop( void );

		// �����߳�ѭ��
		bool Start( void );
		// ��ֹ�߳�ѭ����ms����λ�Ǻ��룩�ǵȴ��߳��˳�ѭ��������ȴ���ʱ����ǿ�йر�
		void Terminate( unsigned int ms=500 );

	public :
		// ����̳е���ѭ��
		virtual int Run() = 0;
		// ֪ͨ�ر�
		void Broadcast( void );

	private:
		// No copies do not implement
		CThread(const CThread &l);
		CThread &operator=(const CThread &l);

	protected:
		THREADID			m_hThread;
		const std::string	m_name;
		volatile bool		m_brunning;
		CLock				m_lock;
		CCondition			m_condition;
	};

}

//#endif // __AGBASE_THREAD_H__
