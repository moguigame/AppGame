#include "MoGui.h"
#include "MoGuiGame.h"
#include "gamedezhou.h"

#include "clientpool.h"
#include "ClientPool.h"
#include "testplayer.h"

#include <process.h>

unsigned int __stdcall TestThread(void* pvoid)
{
	CTestPlayer* pTestPlayer = (CTestPlayer*)pvoid;
	for (;;)
	{
		if ( (!pTestPlayer->m_clientsocket.IsConnected()) && pTestPlayer->m_clientsocket.Connect("192.168.138.160",5536) )
		{
			//cout<<"DBSocket Connect Succeed."<<endl;
		}
		Sleep(1000);
	}

	return 0;
}

int main(int argc, char** argv)
{
	int StartPID,EndPID;
	string strPath,strTemp;
	char CurPath[256];

	::GetCurrentDirectory(255,CurPath);
	strPath = string(CurPath)+"\\"+"client.ini";

	StartPID = ::GetPrivateProfileInt("config","start",0,strPath.c_str());
	cout<<"StartPID="<<StartPID<<endl;

	EndPID = ::GetPrivateProfileInt("config","end",0,strPath.c_str());
	cout<<"EndPID="<<EndPID<<endl;

	if ( StartPID>1000 && EndPID>StartPID )
	{
		CTestPlayer* pTestPlayer;
		for ( int PID=StartPID;PID<=EndPID;PID++)
		{
			pTestPlayer = new CTestPlayer(2,PID);
			::_beginthreadex(NULL,0,TestThread,pTestPlayer,0,NULL);
			//Sleep(2000);
		}
	}
	
	char temp[1024] = {0};
	do
	{
		memset( temp, 0, sizeof(temp) );
		gets( temp );
	} while( strncmp(temp, "quit",sizeof("quit"))!=0 );

	return 0;
}