#include "StdAfx.h"
#include <Dbghelp.h>
#pragma comment (lib, "Dbghelp.lib")
#include <vector>

#include "StackTrace.h"

// The current process handle
static HANDLE g_hCurrentProcess = 0;


static DWORD WINAPI GetModBase_(HANDLE hProcess, DWORD dwAddress)
{
	// Get module information of specified process
	IMAGEHLP_MODULE ihm;
	ihm.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
	if (::SymGetModuleInfo(g_hCurrentProcess, dwAddress, &ihm))
		return ihm.BaseOfImage;

	// Get information about a range of pages within the virtual address space of a specified process
	MEMORY_BASIC_INFORMATION mbi;
	if (::VirtualQueryEx(hProcess, (LPCVOID)dwAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
	{
		TCHAR szFile[MAX_PATH];
		HANDLE hFile = NULL;
		DWORD dwNameLen = ::GetModuleFileName((HINSTANCE)mbi.AllocationBase, szFile, MAX_PATH);
		if (dwNameLen)
			hFile = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);

		::SymLoadModule(g_hCurrentProcess, hFile, dwNameLen ? szFile : NULL, NULL, (DWORD)mbi.AllocationBase, 0);
		return (DWORD)mbi.AllocationBase;
	}

	return 0;
}


static DWORD ConvertAddress_(DWORD dwAddress, LPTSTR szOutBuff)
{
	IMAGEHLP_MODULE ihm;
	ZeroMemory(&ihm, sizeof(IMAGEHLP_MODULE));
	ihm.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

	// Always stick the address in first
	LPTSTR pCurrPos = szOutBuff;
	pCurrPos += ::wsprintf(pCurrPos, _T("0x%08X "), dwAddress);

	// Get the module name
	if (::SymGetModuleInfo(g_hCurrentProcess, dwAddress, &ihm))
	{
		// Strip off the path
		LPTSTR szName = ::_tcsrchr(ihm.ImageName, _T('\\'));
		if (szName)	szName++;
		else		szName = ihm.ImageName;
		pCurrPos += ::wsprintf(pCurrPos, _T("%s: "), szName);
	}
	else
		pCurrPos += ::wsprintf(pCurrPos, _T("<unknown module>: "));

	// Get the function
	BYTE szTemp[MAX_PATH + sizeof(IMAGEHLP_SYMBOL)];
	PIMAGEHLP_SYMBOL pIHSymbol	= (PIMAGEHLP_SYMBOL)&szTemp;
	ZeroMemory(pIHSymbol, MAX_PATH + sizeof(IMAGEHLP_SYMBOL));
	pIHSymbol->SizeOfStruct		= sizeof(IMAGEHLP_SYMBOL);
	pIHSymbol->Address			= dwAddress;
	pIHSymbol->MaxNameLength	= MAX_PATH;
	DWORD dwDisp;
	if (::SymGetSymFromAddr(g_hCurrentProcess, dwAddress, &dwDisp, pIHSymbol))
	{
		pCurrPos += dwDisp ?
			::wsprintf(pCurrPos, _T("%s() + %d bytes"), pIHSymbol->Name, dwDisp) :
			::wsprintf(pCurrPos, _T("%s()"), pIHSymbol->Name);

		// If I got a symbol, give the source and line a whirl
		IMAGEHLP_LINE ihl;
		ZeroMemory(&ihl, sizeof(IMAGEHLP_LINE));
		ihl.SizeOfStruct = sizeof(IMAGEHLP_LINE);
		if (SymGetLineFromAddr(g_hCurrentProcess, dwAddress, &dwDisp, &ihl))
		{
			// Put this on the next line and indented a bit
			pCurrPos += ::wsprintf(pCurrPos, _T("\n\t%s, Line %d"), ihl.FileName, ihl.LineNumber);
			if (dwDisp)
				pCurrPos += ::wsprintf(pCurrPos, _T(" + %d bytes"), dwDisp);
		}
	}
	else
		pCurrPos += ::wsprintf(pCurrPos, _T("<unknown symbol>"));

	// Tack on a CRLF
	pCurrPos += ::wsprintf(pCurrPos, _T("\n"));
	return pCurrPos - szOutBuff;
}


void StackTracer::BuildTrace(LPTSTR szString, DWORD dwSize, DWORD dwNumSkip)
{
	// If the symbol handler is not initialized, set it up now
	HANDLE hProcess = ::GetCurrentProcess();
	if (!g_hCurrentProcess && ::SymInitialize(hProcess, NULL, FALSE))
	{
		// Turn on load lines
		::SymSetOptions(::SymGetOptions() | SYMOPT_LOAD_LINES);
		g_hCurrentProcess = hProcess;
	}

	typedef std::vector<DWORD> ADDRESS_VECTOR;
	ADDRESS_VECTOR vAddresses;
	CONTEXT tc;
	tc.ContextFlags = CONTEXT_FULL;
	if (::GetThreadContext(GetCurrentThread(), &tc))
	{
		STACKFRAME sf;
		ZeroMemory(&sf, sizeof(STACKFRAME));
		sf.AddrPC.Mode		= AddrModeFlat;
		sf.AddrPC.Offset    = tc.Eip;
		sf.AddrStack.Offset = tc.Esp;
		sf.AddrStack.Mode   = AddrModeFlat;
		sf.AddrFrame.Offset = tc.Ebp;
		sf.AddrFrame.Mode   = AddrModeFlat;

		// Loop for the first 512 stack elements.
		for (DWORD i = 0; i < 512; i++)
		{
			if (FALSE == ::StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hProcess, &sf,
								      &tc, NULL, SymFunctionTableAccess, ::GetModBase_, NULL))
				break;
			if (i > dwNumSkip && sf.AddrPC.Offset != 0)
			{	// Also check that the address is not zero.
				// Sometimes StackWalk returns TRUE with a frame of zero.
				vAddresses.push_back(sf.AddrPC.Offset);
			}
		}

		// Now start converting the addresses
		DWORD dwSizeLeft = dwSize;
		TCHAR szSym[MAX_PATH * 2];
		LPTSTR szCurrPos = szString;
		for (ADDRESS_VECTOR::const_iterator It = vAddresses.begin(); It != vAddresses.end(); ++It)
		{
			DWORD dwSymSize = ::ConvertAddress_(*It, szSym);
			if (dwSizeLeft < dwSymSize)
				break;
			::_tcscpy(szCurrPos, szSym);
			szCurrPos += dwSymSize;
			dwSizeLeft -= dwSymSize;
		}
	}
}
