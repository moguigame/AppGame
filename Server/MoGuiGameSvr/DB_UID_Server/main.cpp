#include "server.h"

using namespace AGBase;
using namespace std;

int main(int argc, char** argv)
{
	CServer s;

	char temp[1024] = {0};
	do
	{
		memset( temp, 0, sizeof(temp) );
		gets( temp );
	} while( strcmp(temp, "quit")!=0 );

	return 0;
}