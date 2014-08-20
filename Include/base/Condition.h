#pragma once

// Windows�汾
//	�����̲߳����Եȴ�һ��handler�������Waitʧ�ܣ�Ҳ����Windows�²���ҪBroadcast
// Linux�¿���
//  


namespace AGBase
{

	class CLock;

	class CCondition
	{

#if defined _WINDOWS_ || defined WIN32
	typedef HANDLE			condid;
#else
	typedef pthread_cond_t	condid;
#endif

	public:
		explicit  CCondition( void );
		~CCondition( void );

		// �ȴ��źŵ���
		// msΪ��ʱʱ�䣬��λΪ����
		bool Wait (CLock& locker);
		bool Wait (CLock& locker, unsigned int ms);

		// �����ź������ı�
		// Signal-������һ���̣߳�Broadcast-�����������߳�
		void Signal( void );
		void Broadcast( void );

	private:
		condid	m_condid;

	private:
		CCondition (const CCondition&);
		CCondition& operator= (const CCondition&);

	};

};



//#endif // __AGBASE_CONDITION_H__
