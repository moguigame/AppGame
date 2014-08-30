#pragma once

//#ifndef __AGBASE_DISPATCHER_H__
//#define __AGBASE_DISPATCHER_H__

namespace AGBase
{

	class CConnect;
	class CConnectPool;

	class CDispatcher : public CThread
	{
	public:
		CDispatcher( void );
		virtual ~CDispatcher( void );

		bool Init( CConnectPool* cpool );
		void Fini( void );

		void OnRecvPacket( CPacketQueue& packets );
		void OnRecvPacket( CPacket* packet );

		void AddForbidIP( const char* ip );
		void DelForbidIP( const char* ip );
		bool IsForbidIP( const std::string& ip );

	protected:
		virtual int Run( void );

		void DispatchPacket( void );
		bool OnPriorityEvent( void );
		void CheckTimer( void );
		void OnPacket( CPacket* packet );

	private:
		CConnectPool*	m_cpool;

		CPacketQueue	m_packets;
		volatile unsigned long		m_lasttime;

		int             m_StartTime;
		int             m_MaxWaitTime;
		long long       m_TotalFinishPacket;
		long long       m_TotalWaitTime;

		CLock			m_packetlock;

		CLock			m_iplock;
		std::set<std::string>	m_forbidips;
	};

}

//#endif //__AGBASE_DISPATCHER_H__

