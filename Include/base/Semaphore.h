#pragma once

namespace AGBase
{
	class CSemaphore
	{
#if defined _WINDOWS_ || defined WIN32
		typedef HANDLE		semaid;
#else
		typedef sem_t		semaid;
#endif
	public:
		CSemaphore( unsigned int initcount=0 );
		~CSemaphore( void );

		bool Post( void );

		bool Wait( void );
		bool Wait( unsigned int ms );

	private:
		semaid	m_sema;

	private:
		CSemaphore (const CSemaphore&);
		CSemaphore& operator= (const CSemaphore&);
	};
};
