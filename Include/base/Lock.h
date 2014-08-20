#pragma once

namespace AGBase
{
	// ���İ�װ�࣬
	// ע�⣺
	//		windowsʹ��CRITICAL_SECTION����һ����������
	//		linuxʹ��pthread_mutex_t��Ĭ�ϲ�����һ������������
	class CLock
	{
		friend class CCondition;
	public:
		explicit CLock( void );
		~CLock( void );

		// ����
		void Lock( void );
		// ����
		void Unlock( void );

	private:
	#if defined _WINDOWS_ || defined WIN32
		CRITICAL_SECTION	m_crit;
	#else
		pthread_mutex_t		m_crit;
	#endif

		// No copies do not implement
		CLock(const CLock &l);
		CLock &operator=(const CLock &l);
	};

	// �򻯵�CLock��װ�࣬ʹ��ʱֱ�Ӷ��弴��
	class CSelfLock
	{
	public:
		explicit CSelfLock(CLock &lock);
		~CSelfLock( void );

	private :
		CLock &m_lock;

		// No copies do not implement
		CSelfLock(const CSelfLock &l);
		CSelfLock &operator=(const CSelfLock &l);
	};
}

//#endif // __AGBASE_LOCK_H__

