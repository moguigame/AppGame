

#ifndef __AGBASE_INSIDEBASE_H__
#define __AGBASE_INSIDEBASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <stdarg.h>

//windows support
#if defined _WINDOWS_ || defined WIN32

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif	// WIN32_LEAN_AND_MEAN

	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0600
	#endif	// _WIN32_WINNT

	#include <windows.h>
	#include <assert.h>
	#include <winbase.h>
	#include <winsock2.h>
	#include <mswsock.h>
	#include <process.h>	

	#pragma warning(disable : 4786)
	#pragma warning(disable : 4244)
	#pragma warning(disable : 4267)

	#include <shlwapi.h>
	#pragma comment(lib, "shlwapi.lib")
	#pragma comment(lib, "ws2_32.lib")

	#ifndef snprintf
		#define snprintf _snprintf
	#endif
	
	#define _WINDOW_MSG_LOG
	#define WM_MSG_LOG WM_USER + 1

#else
	#include <cassert>
	#include <cerrno>
	#include <pthread.h>
	#include <stddef.h>
	#include <netdb.h>
	#include <signal.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/time.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/select.h>
	#include <sys/socket.h>
	#include <sys/epoll.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <semaphore.h>
	#include <string.h>
	#include <strings.h>

	#define INVALID_SOCKET			-1

#endif	//OS_LINUX


#endif // __AGBASE_INSIDEBASE_H__


