// Game.cpp : Defines base callbacks for window message proc.
//

#pragma once

//
//	FUNCTION:	BOOL GameInit(HWND)
//
//  PURPOSE:	Initialize Direct3D device, adapter info,
//				DX font, custom DX app structures (textures, lights, materials...).
//				Setup some DX render states.
//
//	RETURN:		Boolean result of initialization
//
extern BOOL GameInit(HWND hWnd);


//
//	FUNCTION:	void GameDraw(void)
//
//  PURPOSE:	Rendering new frame.
//				(Set time, set matrices, textures lights, clear frame, drawing, flip buffers...)
//
//	RETURN:		None
//
extern void GameDraw(void);



//
//	FUNCTION:	void GameShutdown(void)
//
//  PURPOSE:	Free all game resources include COM interfaces (textures, fonts, Direct3D device...)
//
//	RETURN:		None
//
extern void GameShutdown(void);


//
//	FUNCTION:	GameInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//
//  PURPOSE:	Process key, mouse input.
//				Callback proc for main window`s WndProc().
//
//	RETURN:		None
//
extern void GameInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

