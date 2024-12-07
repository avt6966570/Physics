// WinMain.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"
#include "resource.h"
#include "GAME.h"


// Window class and caption text
LPCTSTR g_lpWindowClass	= "WaterQubeDemoClass";
LPCTSTR g_lpWindowName	= "Qube In Water Demo by Max Berezhnoy";


// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPTSTR    /*lpCmdLine*/,
                     int       nCmdShow)
{
	// Create window class
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_lpWindowClass;
	wcex.hIconSm		= LoadIcon(hInstance, IDI_APPLICATION);

	// Try register window class
	if (!RegisterClassEx(&wcex))
		return FALSE;

	// Create window
	HWND hWnd = CreateWindow(g_lpWindowClass, g_lpWindowName, WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
							 GetDesktopWindow(), NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		InvalidateRect(hWnd, NULL, FALSE);	// Redraw client area!
	}

	return 0;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- Process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- Post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// First procees user game input
	GameInput(hWnd, message, wParam, lParam);

	// Next process app specific message
	switch (message) 
	{
//	case WM_COMMAND:
//		int wmId, wmEvent;
//		wmId    = LOWORD(wParam); 
//		wmEvent = HIWORD(wParam); 
//		// Parse the menu selections:
//		switch (wmId)
//		{
//		case IDM_EXIT:
//			DestroyWindow(hWnd);
//			break;
//		default:
//			return DefWindowProc(hWnd, message, wParam, lParam);
//		}
//		break;
	case WM_CREATE:
		// Init game resources
		return GameInit(hWnd) ? 0L : -1L;
	case WM_PAINT:
		// Draw game frame
		GameDraw();
		// Validate client area
		ValidateRect(hWnd, NULL);
		break;
	case WM_DESTROY:
		// Free game resources
		GameShutdown();
		// Unregister window class
		::UnregisterClass(g_lpWindowClass, GetModuleHandle(NULL));
		// Request on terminate (quit)
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0L;
}
