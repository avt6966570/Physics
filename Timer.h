#pragma once


//////////////////////////////////////////////////////////////////////////
//							  T I M E R
//////////////////////////////////////////////////////////////////////////
//	Name:			Timer
//	Description:	Code Timers are used to time fuctions and
//					sections of code
//	Members:		m_eState		-- current timer state (on, off, or hold)
//					m_nStartTime	-- beginning time when timer started
//					m_nStopTime		-- fixed stopping time when timer stopped
//					m_nTimeDelta	-- elapsed time when timer running session
//					m_nElapsedCount	-- elapsed time when stopped
//	Remarks:
//					A counter is a general term used in programming to refer to
//					an incrementing variable. Some systems include a high-resolution
//					performance counter that provides high-resolution elapsed times. 
//					If a high-resolution performance counter exists on the system,
//					the QueryPerformanceFrequency function can be used to express
//					the frequency, in counts per second. The value of the count is
//					processor dependent. On some processors, for example, the count
//					might be the cycle rate of the processor clock.
//					This timer used high-resolution performance counter that provides
//					high-resolution elapsed times via QueryPerformanceCounter().
//////////////////////////////////////////////////////////////////////////

class Timer
{
	//				 CONSTANTS					//
public:
	enum State
	{
		OFF, ON, HOLD
	};


	//		 CONSTRUCTION / DESTRUCTION			//
public:
	Timer(void)
		: m_eState(OFF)
		, m_nStartTime(0)
		, m_nStopTime(0)
		, m_nTimeDelta(0)
		, m_nElapsedCount(0)
	{}


	//				   UTILS					//
public:
	void		Start(void);
	void		Stop(void);
	void		Suspend(void);
	void		Resume(void);

	float		GetElapsedTime(void);
	DWORD		GetElapsedSeconds(void);
	DWORD		GetElapsedMilliseconds(void);
	DWORD		GetElapsedCount(void);

protected:
	static void  SetupTimerFrequency(void);
	static DWORD SamplePerformanceCounter(void);


	//				STATIC DATA					//
public:
protected:
	static DWORD s_nSecondsFrequency;
	static DWORD s_nMillisecondsFrequency;
	static float s_fInvSecFrequency;


	//				IMPLEMENTATION				//
public:
protected:
	State	m_eState;
	DWORD	m_nStartTime;
	DWORD	m_nStopTime;
	DWORD	m_nTimeDelta;
	DWORD	m_nElapsedCount;
};




//////////////////////////////////////////////////////////////////////////
//							A U T O   T I M E R
//////////////////////////////////////////////////////////////////////////
//	Name:			AutoTimer
//	Base class:		Timer
//	Description:	Derived from Timer. AutoTimer started in constructor and
//					stopped in destructor so that he might work immediately after
//					initialized.
//	Members:		<none>
//	Remarks:
//					AutoTimer used for exactly measure timing by some thread
//					when it must be started in "static" run-time before any
//					stack variables initialization.
//					It must be good way -- use auto-timer for global application
//					timer or high-resolution counter for code profiling.
//////////////////////////////////////////////////////////////////////////

class AutoTimer : public Timer
{
public:
	AutoTimer(void)
	{
		Start();
	}

	~AutoTimer(void)
	{
		Stop();
	}
};




//////////////////////////////////////////////////////////////////////////
//							C O D E   T I M E R
//////////////////////////////////////////////////////////////////////////
//	Name:			CodeTimer
//	Base class:		<none>
//	Description:	Code timer specially developed for code profile timing.
//					He use high-resolution performance counter that provides
//					high-resolution elapsed times via QueryPerformanceCounter().
//					CodeTimer has static pointer "pRoot" init in static time --
//					so that it organized in "static reversed single-linked list"
//					and it free for adding new nodes. Has possible to hold timer,
//					find some timer-node by name, counting nodes, and reset in
//					initial state.
//	Members:		m_pName				-- name
//					m_fStartTime		-- time after start profiling session
//					m_fMinimumTimeSample-- minimum sample form multitude more launch
//					m_fMaximumTimeSample-- maximum sample form multitude more launch
//					m_fTotalTime		-- total elapsed profile time
//					m_nTotalCalls		-- total profile calls measurement proc
//					m_pNext				-- next static timer in linked list
//	Remarks:
//					CodeTimer used for exactly measure timing by some thread
//					when it must be started in "static" run-time before any
//					stack variables initialization.
//					It must be good way -- use auto-timer for global application
//					timer or high-resolution counter for code profiling.
//////////////////////////////////////////////////////////////////////////

#if defined _ENABLE_PROFILING
# define BEGIN_PROFILE(name)	static CodeTimer s_ct##name(_TEXT(#name)); \
								s_ct##name.BeginProfiling();
# define END_PROFILE(name)		s_ct##name.EndProfiling();
# define PROFILE_SCOPE(name) 	static CodeTimer s_ct##name(_TEXT(#name)); \
								ScopeCodeTimer SCT_(&s_ct##name);
#else
# define BEGIN_PROFILE(name) 
# define END_PROFILE(name)
# define PROFILE_SCOPE(name)
#endif // _ENABLE_PROFILING

class CodeTimer
{
	//		 CONSTRUCTION / DESTRUCTION			//
public:
	CodeTimer(const TCHAR* pName);


	//					UTILS					//
public:
	void BeginProfiling();
	void EndProfiling();
	void Reset();
	void ResetAllTimers();

	static const CodeTimer* Find(const TCHAR* pName);
	static unsigned int		Count();
	static void				ForEach(void (*PFNPROC)(const CodeTimer*));

    
	//					INFO					//
public:
	const TCHAR* GetName()				const { return m_pName;				 }
	float		 GetAverageTime()		const { return m_nTotalCalls ? m_fTotalTime/m_nTotalCalls : 0.0f; }
	float		 GetTotalTime()			const { return m_fTotalTime;		 }
	unsigned int GetTotalCalls()		const { return m_nTotalCalls;		 }
	float		 GetMinimumTimeSample() const { return m_fMinimumTimeSample; }
	float		 GetMaximumTimeSample() const { return m_fMaximumTimeSample; }


	//				STATIC DATA					//
public:
protected:
	static CodeTimer *	s_pRoot;
	static AutoTimer	s_profiler;


	//				IMPLEMENTATION				//
public:
protected:
	const TCHAR*		m_pName;
	float				m_fStartTime;
	float				m_fMinimumTimeSample;
	float				m_fMaximumTimeSample;
	float				m_fTotalTime;
	unsigned int		m_nTotalCalls;
	CodeTimer *			m_pNext;
};



class ScopeCodeTimer
{
	//		 CONSTRUCTION / DESTRUCTION			//
public:
	ScopeCodeTimer(CodeTimer *pCodeTimer)
		: m_pCodeTimer(pCodeTimer)
	{
		m_pCodeTimer->BeginProfiling();
	}

	~ScopeCodeTimer()
	{
		m_pCodeTimer->EndProfiling();
	}

private:
	CodeTimer *		m_pCodeTimer;
};



inline void Timer::Start(void)
{
	m_nStartTime	= SamplePerformanceCounter();
	m_nElapsedCount = 0;
	m_eState		= ON;
}

inline void Timer::Stop(void)
{
	m_nElapsedCount = GetElapsedCount();
	m_eState		= OFF;
}

inline void Timer::Suspend(void)
{
	if (m_eState == ON)
	{
		m_nElapsedCount = GetElapsedCount();
		m_eState		= HOLD;
	}
}

inline void Timer::Resume(void)
{
	if (m_eState == HOLD)
	{
		// roll the start time back by our previous delta
		m_nStartTime	= SamplePerformanceCounter() - m_nTimeDelta;
		m_nElapsedCount = 0;
		m_eState		= ON;
	}
}

inline float Timer::GetElapsedTime(void)
{
	if (m_eState == ON)
	{
		m_nStopTime	 = SamplePerformanceCounter();
		m_nTimeDelta = m_nStopTime - m_nStartTime;
		return m_nTimeDelta * s_fInvSecFrequency;
	}
	else
		return m_nElapsedCount * s_fInvSecFrequency;
}

inline DWORD Timer::GetElapsedSeconds(void)
{
	if (m_eState == ON)
	{
		m_nStopTime  = SamplePerformanceCounter();
		m_nTimeDelta = m_nStopTime - m_nStartTime;
		return m_nTimeDelta / s_nSecondsFrequency;
	}
	else
		return m_nElapsedCount / s_nSecondsFrequency;
}

inline DWORD Timer::GetElapsedMilliseconds(void)
{
	if (m_eState == ON)
	{
		m_nStopTime  = SamplePerformanceCounter();
		m_nTimeDelta = m_nStopTime - m_nStartTime;
		return m_nTimeDelta / s_nMillisecondsFrequency;
	}
	else
		return m_nElapsedCount / s_nMillisecondsFrequency;
}

inline DWORD Timer::GetElapsedCount(void)
{
	if (m_eState == ON)
	{
		m_nStopTime  = SamplePerformanceCounter();
		m_nTimeDelta = m_nStopTime - m_nStartTime;
		return m_nTimeDelta;
	}
	else
		return m_nElapsedCount;
}

inline void Timer::SetupTimerFrequency(void)
{
	if (0 == s_nSecondsFrequency)
	{
		LARGE_INTEGER liFrequency;
		::QueryPerformanceFrequency(&liFrequency);
		s_nSecondsFrequency		 = liFrequency.LowPart;
		s_nMillisecondsFrequency = liFrequency.LowPart/1000;
		s_fInvSecFrequency		 = 1.0f / s_nSecondsFrequency;
	}
}

inline DWORD Timer::SamplePerformanceCounter(void)
{
	LARGE_INTEGER liSample;
	::QueryPerformanceCounter(&liSample);
	return liSample.LowPart;
}



inline CodeTimer::CodeTimer(const TCHAR* pName)
	: m_pName(pName)
	, m_fStartTime(0.0f)
	, m_fMinimumTimeSample(FLT_MAX)
	, m_fMaximumTimeSample(0.0f)
	, m_fTotalTime(0.0f)
	, m_nTotalCalls(0)
	, m_pNext(s_pRoot)
{
	s_pRoot = this;
}

inline void CodeTimer::BeginProfiling()
{
	++m_nTotalCalls;
	if (!m_fStartTime)
		m_fStartTime = s_profiler.GetElapsedTime();
}

inline void CodeTimer::EndProfiling()
{
	if (m_fStartTime)
	{
		float fSample = s_profiler.GetElapsedTime() - m_fStartTime;
		m_fTotalTime += fSample;
		m_fMinimumTimeSample = min(m_fMinimumTimeSample, fSample);
		m_fMaximumTimeSample = max(m_fMaximumTimeSample, fSample);
		m_fStartTime = 0.0f;
	}
}

inline void CodeTimer::Reset()
{
	m_fStartTime		 = 0.0f;
	m_fMinimumTimeSample = FLT_MAX;
	m_fMaximumTimeSample = 0.0f;
	m_fTotalTime		 = 0.0f;
	m_nTotalCalls		 = 0;
}

inline void CodeTimer::ResetAllTimers()
{
	for (CodeTimer* pCT = s_pRoot; pCT; pCT = pCT->m_pNext)
		pCT->Reset();
}

inline const CodeTimer* CodeTimer::Find(const TCHAR* pName)
{
	for (const CodeTimer* pCT = s_pRoot; pCT; pCT = pCT->m_pNext)
		if (0 == _tcscmp(pCT->m_pName, pName))
			return pCT;
	return NULL;
}

inline unsigned int CodeTimer::Count()
{
	unsigned int nCounter = 0;
	for (const CodeTimer* pCT = s_pRoot; pCT; pCT = pCT->m_pNext)
		++nCounter;
	return nCounter;
}

inline void	CodeTimer::ForEach(void (*PFNPROC)(const CodeTimer *))
{
	for (const CodeTimer* pCT = s_pRoot; pCT; pCT = pCT->m_pNext)
		PFNPROC(pCT);
}
