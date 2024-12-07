#pragma once

namespace StackTracer {

	// ATTENTION:: StackTracer not supported newest 64bit AMD & Intel platforms!
	// (Need add auto-detect options before call StackWalk(),
	//  currently supported IMAGE_FILE_MACHINE_I386 only ;( )

	extern void BuildTrace(
		LPTSTR szString,		// Null terminated string with stack text info
		DWORD dwSize,			// Size of string buffer for stack text info
		DWORD dwNumSkip			// Skip nth first level
	);

} // END namespace StackTracer