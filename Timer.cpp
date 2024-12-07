#include "StdAfx.h"
#include "Timer.h"

DWORD Timer::s_nSecondsFrequency		= 0;
DWORD Timer::s_nMillisecondsFrequency	= 0;
float Timer::s_fInvSecFrequency			= 0.0f;

CodeTimer *	CodeTimer::s_pRoot	= NULL;
AutoTimer	CodeTimer::s_profiler;