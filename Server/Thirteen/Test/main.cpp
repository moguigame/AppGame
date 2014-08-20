#include <iostream>
#include <queue>

#include "Base.h"

const unsigned int _MAX_BUFFER_LENGTH		= 1024;
#include "..//..//..//agbase/BaseSource/socket_windows/Packet.h"

using namespace std;
using namespace AGBase;

CLock      g_lock;
queue<int> g_queue;
CCondition g_cond;

class CMPThread : public CThread
{
public:
	CMPThread():CThread("MemoryPool"){}
	virtual ~CMPThread(){}

	int Run()
	{
		CPacket* pPakcet = NULL;
		while (IsRunning())
		{
			pPakcet = new CPacket();
			delete pPakcet;
		}
		return 0;
	}
};

class CReadThread : public CThread
{
public:
	CReadThread():CThread("Read"){}
	virtual ~CReadThread(){}

	int Run()
	{
		while (IsRunning())
		{
			CSelfLock lock(g_lock);
			if ( g_queue.size() == 0 )
			{
				cout<<"wait"<<endl;
				g_cond.Wait(g_lock,0xffffffff);
			}

			int nRet = g_queue.front();
			g_queue.pop();
		}
		return 0;
	}
};

class CWriteThread : public CThread
{
public:
	CWriteThread():CThread("Write"){}
	virtual ~CWriteThread(){}

	int Run()
	{
		while (IsRunning())
		{
			CSelfLock lock(g_lock);
			g_queue.push(0);
			if ( g_queue.size() == 1 )
			{
				cout<<"broad"<<endl;
				g_cond.Broadcast();
			}
		}
		return 0;
	}
};

int main(void)
{
	int nRet = 0;

	CMPThread testTD;
	testTD.Start();

	/*
	CWriteThread WT[10];
	CReadThread  RT[10];

	for (int i=0;i<1;i++)
	{
		RT[i].Start();
	}

	for (int i=0;i<1;i++)
	{
		WT[i].Start();
	}
	*/

	cin>>nRet;
	return nRet;
}