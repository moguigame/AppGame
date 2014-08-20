#pragma once

#include <map>

#include "Base.h"
#include "MoGuiGame.h"

using namespace std;
using namespace AGBase;
using namespace Tool;

typedef map<UINT16,INT64>                  MapXieYiIO;

class CMoGuiServerSocket : public IConnectCallback
{
public:
	CMoGuiServerSocket( IConnect* pConnect );
	~CMoGuiServerSocket(void);

	enum { SOCKET_ST_NONE,SOCKET_ST_CONNECTED,SOCKET_ST_STOP };

	static MapXieYiIO s_InXieYi;
	static MapXieYiIO s_OutXieYi;
	static INT64      s_TotalInByte;
	static INT64      s_TotalOutByte;
	static INT64      s_TotalInPacket;
	static INT64      s_TotalOutPacket;

	static INT64      s_LastTotalInByte;
	static INT64      s_LastTotalOutByte;
	static INT64      s_LastTotalInPacket;
	static INT64      s_LastTotalOutPacket;

	virtual void             OnConnect( void );
	virtual void             OnClose( bool bactive );
	virtual int              OnMsg( const char* buf, int len );

	unsigned int             GetPeerLongIp( void );
	string                   GetPeerStringIp( void );
	bool                     IsConnected() const { return m_SocketState == SOCKET_ST_CONNECTED; }

	int                      CheckFlash( const char* buf, int len );
	IConnect*                GetConnect(){ return m_pConnect; }
	void                     Close();
	void                     Stop(int curTime);
	unsigned int             GetStopTime();

	//���ܵ�����
	void                     SetCrypt(bool bCrypt);
	void                     SetKey(unsigned char* pKey,int keySize);

	int                      FinishMsg(UINT16 XYID,int TotalXYLen);

public:
	int				m_nRecvPackets;       //�հ�������
	int				m_nSendPackets;       //����������
	int				m_nRecvSize;          //�յ����ֽ���
	int				m_nSendSize;          //���͵��ֽ���

protected:
	IConnect*	    m_pConnect;
	int			    m_SocketState;        //�ǲ�����������
	unsigned int    m_nStopTime;

	bool            m_bCrypt;             //�Ƿ����
	CMoGuiEncrypt   m_Crypt;              //���ܶ���

public:
	template<class Txieyi>
		inline int SendMsg(Txieyi& xieyi)
	{
		char sendBuf[MAX_SEND_BUF_SIZE];
		bostream	bos;
		bos.attach(sendBuf,MAX_SEND_BUF_SIZE);

		XYIDHEAD xyid = xieyi.XY_ID;
		LONGHEAD xylen = 0;
		unsigned short sendlen = 0;

		bos.seekp( SHORT_HEAD_SIZE+LONG_HEAD_SIZE,bios::beg);
		bos<<xyid<<xieyi;		
		xylen = (LONGHEAD)bos.length() - SHORT_HEAD_SIZE-LONG_HEAD_SIZE-XYID_HEAD_SIZE;

		if (m_bCrypt)
		{
			m_Crypt.encrypt((BYTE*)sendBuf+SHORT_HEAD_SIZE+LONG_HEAD_SIZE+XYID_HEAD_SIZE,
				(BYTE*)sendBuf+SHORT_HEAD_SIZE+LONG_HEAD_SIZE+XYID_HEAD_SIZE,xylen);
		}

		if( xylen < 255 )
		{
			bos.seekp(LONG_HEAD_SIZE,bios::beg);
			bos<<SHORTHEAD(xylen);

			sendlen = xylen+SHORT_HEAD_SIZE+XYID_HEAD_SIZE;
			m_pConnect->Send(sendBuf+LONG_HEAD_SIZE,sendlen);

			m_nSendSize += sendlen;
		}
		else
		{
			bos.seekp(0,bios::beg);
			bos<<SHORTHEAD(255)<<xylen;

			sendlen = xylen+SHORT_HEAD_SIZE + LONG_HEAD_SIZE+XYID_HEAD_SIZE;

			m_pConnect->Send(sendBuf,sendlen);
			m_nSendSize += sendlen;
		}

		m_nSendPackets++;

		s_TotalOutPacket++;
		MapXieYiIO::iterator itorXieYi = s_OutXieYi.find(xyid);
		if ( itorXieYi == s_OutXieYi.end() )
		{
			s_OutXieYi.insert(make_pair(xyid,INT64(0)));
			itorXieYi = s_OutXieYi.find(xyid);
		}
		itorXieYi->second += sendlen;
		s_TotalOutByte += sendlen;

		if ( xyid == 11030 )
		{
			fprintf_s(stderr,"xylen=%d \n",xylen );
		}

		return sendlen;
	}

private:
	CMoGuiServerSocket(const CMoGuiServerSocket &l);
	CMoGuiServerSocket &operator=(const CMoGuiServerSocket &l);
};
