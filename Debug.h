#pragma once

# if defined _ENABLE_DEBUGGING

#ifdef ASSERT
#undef ASSERT
#endif // ASSERT

#ifdef SYSERROR
#undef SYSERROR
#endif // SYSERROR


#define ASSERT(exp, comment)	{\
			static bool s_bIgnoreAssert = false;\
			if (!(exp) && !s_bIgnoreAssert) \
			{ \
				Debug::ERROR_RESULT eErrorResult = Debug::NotifyAssertion(_T(#exp), _T(comment), __FILE__, __LINE__);\
				if (Debug::ER_IGNORE == eErrorResult)\
					s_bIgnoreAssert = true;\
				else if (Debug::ER_BREAKPOINT == eErrorResult)\
				{\
					__asm int 3 \
				}\
			}}


#define SYSERROR(code) {\
			static bool s_bIgnoreError = false;\
			if (!s_bIgnoreError) \
			{\
				Debug::ERROR_RESULT eErrorResult = Debug::NotifyError(code, __FILE__, __LINE__);\
				if (eErrorResult == Debug::ER_IGNORE)\
					s_bIgnoreError = true;\
				else if (eErrorResult == Debug::ER_BREAKPOINT)\
				{\
					__asm int 3 \
				}\
			}}
# else
#define ASSERT(exp, comment)
#define SYSERROR(code)
# endif // _ENABLE_DEBUGGING



namespace Debug {

	enum ERROR_RESULT
	{
		ER_IGNORE,
		ER_CONTINUE,
		ER_BREAKPOINT,
		ER_ABORT
	};

	extern ERROR_RESULT DisplayError(
		LPCTSTR pszErrorTitle,
		LPCTSTR pszErrorText,
		LPCTSTR pszErrorDescription,
		LPCTSTR pszFileName, 
		int nLineNumber
	);

	extern ERROR_RESULT NotifyAssertion(
		LPCTSTR pszCondition,
		LPCTSTR pszDescription,
		LPCTSTR pszFileName,
		int nLineNumber
	);

	extern ERROR_RESULT NotifyError(
		unsigned int nErrorCode,
		LPCTSTR pszFileName,
		int nLineNumber
	);

} // END namespace Debug



#define TRACE_MSG(str,hr)    DXTrace(__FILE__, (DWORD)__LINE__, hr, str, TRUE)
