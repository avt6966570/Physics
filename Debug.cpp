#include "StdAfx.h"
#include "Debug.h"
#include "StackTrace.h"

namespace Debug {

ERROR_RESULT DisplayError(
	LPCTSTR pszErrorTitle,
	LPCTSTR pszErrorText,
	LPCTSTR pszErrorDescription,
	LPCTSTR pszFileName, 
	int nLineNumber
)
{
	// Attempt to get the module name
	const int MODULE_NAME_SIZE = 255;
	TCHAR szModuleName[MODULE_NAME_SIZE];
	if (!::GetModuleFileName(NULL, szModuleName, MODULE_NAME_SIZE))
        ::_tcscpy(szModuleName, _T("<unknown application>"));

	// If stack tracing is enabled, build a string containing the unwound stack information
#if defined _ENABLE_STACKTRACE
		const int STACK_STRING_SIZE = 4096;
		TCHAR pszStackText[STACK_STRING_SIZE];
		StackTracer::BuildTrace(pszStackText, STACK_STRING_SIZE, 0);
#else
		TCHAR pszStackText[] = _T("<stack trace disabled>");
#endif

	// Build a collosal string containing the entire asster message
	const int MAX_BUFFER_SIZE = 1024;
	TCHAR szBuffer[MAX_BUFFER_SIZE];
	::_sntprintf(szBuffer, 
				MAX_BUFFER_SIZE, 
				_T(	"%s\n\n"
					"Program : %s\n"
					"File: %s\n"
					"Line: %d\n"
					"Error: %s\n"
					"Comment: %s\n"
					"\nStack:\n%s\n\n"
					"ABORT to exit (or debug), RETRY to continue,\n"
					"IGNORE to disregard all occurances of this error\n"),
				pszErrorTitle,
				szModuleName,
				pszFileName,
				nLineNumber,
				pszErrorText,
				pszErrorDescription,
				pszStackText
				);

	// Place a copy of the message into the clipboard
	if (::OpenClipboard(NULL))
	{
		size_t nBufferLength = ::_tcsclen(szBuffer);
		HGLOBAL hMem = ::GlobalAlloc(GHND|GMEM_DDESHARE, nBufferLength + 1);
		if (hMem)
		{
			::memcpy(::GlobalLock(hMem), szBuffer, nBufferLength);
			::GlobalUnlock(hMem);
			::EmptyClipboard();
			::SetClipboardData(CF_TEXT, hMem);
		}

		::CloseClipboard();
	}

	// Find the top most window of the current application
	HWND hWndParent = ::GetActiveWindow();
	if (NULL != hWndParent)
		hWndParent = ::GetLastActivePopup(hWndParent);

	// Put up a message box with the error
	switch (::MessageBox(hWndParent,
						 szBuffer,
						 _T("ERROR NOTIFICATION..."),
						 MB_TASKMODAL|MB_SETFOREGROUND|MB_ABORTRETRYIGNORE|MB_ICONERROR))
	{
	case IDRETRY:	return ER_CONTINUE;	// Ignore this error and continue
	case IDIGNORE:	return ER_IGNORE;	// Ignore this error and continue, plus
										// never stop on this error again (handled by the caller)
	default:
		if (IDYES == ::MessageBox(hWndParent,
								  _T("Do you wish to debug the last error?"),
								  _T("DEBUG OR EXIT?"),
								  MB_TASKMODAL|MB_SETFOREGROUND|MB_YESNO|MB_ICONQUESTION))
			return ER_BREAKPOINT;		// Inform the caller to break on the current line of execution

		::ExitProcess((UINT)-1);		// Must be a full-on termination of the app
//		return ER_ABORT;
	}
}


ERROR_RESULT NotifyAssertion(
	LPCTSTR pszCondition,
	LPCTSTR pszDescription,
	LPCTSTR pszFileName,
	int nLineNumber
)
{
	return DisplayError(_T("Assert Failed!"),
								pszCondition,
								pszDescription,
								pszFileName,
								nLineNumber);
}


ERROR_RESULT NotifyError(
		unsigned int nErrorCode,
		LPCTSTR pszFileName,
		int nLineNumber
	)
{
	// If no error code is provided, get the last known error
	if (0 == nErrorCode)
		nErrorCode = ::GetLastError();

	// Use DirectX to supply a string and description for our error.
	// This will handle all known DirectX error codes (HRESULTs)
	// as well as Win32 error codes normally found via FormatMessage
	LPCTSTR pszErrorString		= ::DXGetErrorString9(nErrorCode);
 	LPCTSTR pszErrorDescription = ::DXGetErrorDescription9(nErrorCode);

	// Put the incoming last error back.
	::SetLastError(nErrorCode);

	// Pass the data on to the message box
	return DisplayError(_T("Debug Error!"),
								pszErrorString,
								pszErrorDescription,
								pszFileName,
								nLineNumber);
}

} // END namespace Debug 