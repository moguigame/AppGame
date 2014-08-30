#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <stdarg.h>

//不包括MFC的内容
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>

#include "Atomic.h"
#include "MemoryPool.h"
#include "RefPtr.h"
#include "CircleQueue.h"

#include "Utils.h"
#include "Lock.h"
#include "Condition.h"
#include "Semaphore.h"
#include "Thread.h"
#include "Log.h"

#include "SocketInterface.h"

