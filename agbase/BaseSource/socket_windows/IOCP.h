#pragma once

namespace AGBase
{

class CConnect;
class CDispatcher;

class CIOCP
{
public:
	CIOCP( void );
	~CIOCP( void );

	bool Init( CDispatcher* dispatcher, int clientcnt, int connectcnt );
	void Fini( void );
	CConnect* Connect( const char* ip, unsigned short port, unsigned int recvsize, unsigned int sendsize);
	bool Listen( unsigned short port, unsigned int recvsize, unsigned int sendsize );

	void Close( CConnect* socket, bool bactive );

	static void CALLBACK IOCP_IO( DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred, LPOVERLAPPED lpOverlapped );

private:
	CDispatcher* m_dispatcher;
	int		m_max_clientcnt;
	int		m_max_connectcnt;

	CConnect* m_listener;
	std::deque<CConnect*> m_accepts;

	CLock				m_lock;
	CCondition			m_condition;
	int					m_connects;

private:
	CIOCP( const CIOCP& );
	CIOCP& operator= ( const CIOCP& );
};


}


