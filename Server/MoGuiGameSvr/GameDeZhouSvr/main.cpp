#include "server.h"

int main(int argc, char** argv)
{
	CServer gs;
	CGameDBSocket* DBSocket = new CGameDBSocket(&gs);
	CDBConnectThread ThreadCT;
	ThreadCT.m_pDBSocket = DBSocket;
	ThreadCT.Start();

	char temp[1024] = {0};
	do
	{
		memset( temp, 0, sizeof(temp) );
		gets_s( temp );
	} while( strncmp(temp, "quit",sizeof("quit"))!=0 );

	return 0;
}