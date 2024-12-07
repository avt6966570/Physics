// Game.cpp : Defines base callbacks for window message proc.
//

#include "StdAfx.h"
#include "Windowsx.h"		// GET_X_LPARAM(), GET_Y_LPARAM()
#include "Game.h"
#include "Physics.h"



HWND g_hWnd;


// Global
static D3DXMATRIX				g_matIdentity;
static D3DXMATRIX				g_matWorld;
static D3DXMATRIX				g_matProjection;

// Direct3D objects
static LPDIRECT3D9				g_pD3D			= NULL;
static LPDIRECT3DDEVICE9		g_pD3DDevice	= NULL;
static LPD3DXFONT				g_pD3DFont		= NULL;
static D3DADAPTER_IDENTIFIER9	AdapterInfo;

CONST D3DXVECTOR3				vZero(0.0f, 0.0f, 0.0f);
CONST D3DXMATRIX				matYZRotation(0.5f,		0.5f,	-0.707f, 0.0f,
											  -0.707f,	0.707f, 0.0f,	 0.0f,
											  0.5f,		0.5f,	0.707f,  0.0f,
											  0.0f,		0.0f,	0.0f,	 1.0f);

// Camera
static D3DCamera				gCamera;

// Rigid infos
//static AnimationBox				g_AnimationBox(6.0f, 6.0f, 6.0f, D3DCOLOR_XRGB(0, 128, 255));
//static RigidBox					g_RigidBox(1.0f, 1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(200, 50, 200), vZero, matYZRotation);
static Simulator				g_Simulator;


void DrawQubeInd(LPDIRECT3DDEVICE9 pd3dDevce, FLOAT Size, D3DCOLOR Color)
{
	struct QUBEVERTEX
	{
		FLOAT		Position[3];
		D3DCOLOR	Diffuse;
	};

	static const DWORD QUBEVERTEX_FVF =  D3DFVF_XYZ|D3DFVF_DIFFUSE;

	const QUBEVERTEX vVertexBuffer[8] = {
			{ -Size, -Size, -Size,  Color },
			{ -Size, +Size, -Size,  Color },
			{ +Size, +Size, -Size,  Color },
			{ +Size, -Size, -Size,  Color },

			{ -Size, -Size, +Size,  Color },
			{ -Size, +Size, +Size,  Color },
			{ +Size, +Size, +Size,  Color },
			{ +Size, -Size, +Size,  Color }
	};

	static const WORD wIndexBuffer[6][6] = {
		{ 0, 1, 2, 0, 2, 3 },
		{ 3, 2, 6, 3, 6, 7 },
		{ 7, 6, 5, 7, 5, 4 },
		{ 4, 5, 1, 4, 1, 0 },
		{ 1, 5, 6, 1, 6, 2 },
		{ 4, 0, 3, 4, 3, 7 }
	};

	// Draw qube
	pd3dDevce->SetFVF(QUBEVERTEX_FVF);
	pd3dDevce->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 2 * 6,
									 (LPVOID) wIndexBuffer, D3DFMT_INDEX16,
									 (LPVOID) vVertexBuffer, sizeof(QUBEVERTEX));
}


static void Test()
{
/*
	CONST D3DXVECTOR3 vIn[12] = {
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f)
	};

	D3DXVECTOR3 vOut[12];

	D3DXMATRIX mat;
	D3DXMatrixScaling(&mat, 2.0f, 2.0f, 2.0f);

	D3DXVec3TransformCoordArray(vOut, sizeof(D3DXVECTOR3), vIn, sizeof(D3DXVECTOR3), &mat, 11);

	FLOAT f, g, _1fg;

	CONST D3DXVECTOR3 vTri[3] = {
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(1.0f, 1.0f, 0.0f),
		D3DXVECTOR3(2.0f, 0.0f, 0.0f)
	};

	D3DXVECTOR3 v3v1(1.0f, 0.01f, 0.0f), v1v2(0.5f, 0.45f, 0.0f), v2v3(1.5f, 0.45f, 0.0f);

	return;
*/
}


//
//	FUNCTION:	BOOL GameInit(HWND)
//
//  PURPOSE:	Initialize Direct3D device, adapter info,
//				DX font, custom DX app structures (textures, lights, materials...).
//				Setup some DX render states.
//
//	RETURN:		Boolean result of initialization
//
BOOL GameInit(HWND hWnd)
{
	//     D 3 D   I N I T I A L I Z A T I O N     //

	HRESULT hr;
	if (FAILED(hr = ::InitializeDirect3D(&g_pD3D, &g_pD3DDevice, hWnd, 800, 600, TRUE, TRUE, FALSE)))
	{
		TRACE_MSG(_T("WinMain()->GameInit()->InitializeDirect3D()"), hr);
		return FALSE;
	}

	// Get adapter info
	g_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &AdapterInfo);


	//     R E S O U R C E S   I N I T I A L I Z A T I O N     //

//	// Initialize font
//	if (FAILED(hr = ::CreateMyFont(g_pD3DDevice, 25, 8, &g_pD3DFont)))
//	{
//		TRACE_MSG(_T("WinMain()->GameInit()->CreateMyFont()"), hr);
//		return FALSE;
//	}


	//     U S E R   R E S O U R C E S   I N I T I A L I Z A T I O N     //
	D3DXMATRIX mXRot, mZRot;
	D3DXMatrixRotationX(&mXRot, D3DX_PI / 4);
	D3DXMatrixRotationZ(&mZRot, D3DX_PI / 4);

	D3DXVECTOR3 vAxis(0.0f, 0.1f, 1.0f);
	D3DXVec3Normalize(&vAxis, &vAxis);
	g_Simulator.AddAnimationBox(8.0f, 8.0f, 8.0f, D3DCOLOR_XRGB(0, 128, 255), D3DX_PI / 6, vAxis);
	g_Simulator.AddRigidBox(1.0f, 1.0f, 2.0f, 0.0f, D3DCOLOR_XRGB(200, 50, 200),
							D3DXVECTOR3(0.0f, -2.1f, 0.0f));
	g_Simulator.AddRigidBox(1.0f, 2.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(200, 200, 50),
							D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	g_Simulator.AddRigidBox(1.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(50, 200, 200),
							D3DXVECTOR3(2.0f, -2.0f, 0.0f));
	g_Simulator.AddRigidBox(1.0f, 0.25f, 3.0f, 0.0f, D3DCOLOR_XRGB(50, 200, 50),
							D3DXVECTOR3(2.0f, 0.0f, 0.0f));

	//     S E T U P   R E N D E R   S T A T S     //

	// Set light for the scene
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));
	Light.Type		= D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r = Light.Diffuse.g = Light.Diffuse.b = Light.Diffuse.a = 1.0f;
	Light.Direction = D3DXVECTOR3(-0.578f, -0.578f, 0.578f);
	g_pD3DDevice->SetLight(0, &Light);
	g_pD3DDevice->LightEnable(0, TRUE);

	// Set material for the scene
	D3DMATERIAL9 Material;
	ZeroMemory(&Material, sizeof(D3DMATERIAL9));
    Material.Diffuse.r = Material.Ambient.r = 1.0F;
	Material.Diffuse.g = Material.Ambient.g = 1.0F;
	Material.Diffuse.b = Material.Ambient.b = 1.0F;
	Material.Diffuse.a = Material.Ambient.a = 1.0F;
    g_pD3DDevice->SetMaterial(&Material);

	// Enable lighting
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(25, 25, 5));
	// Enable depth test
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	// Disable alpha blending
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,  FALSE);
	// Enable CCW culling
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// Set solid fill mode
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	//     S E T U P   M A T R I X   T R A N S F O R M A T I O N S     //

	// Set identity matrix
	::D3DXMatrixIdentity(&g_matIdentity);

	// Calculate XZY world matrix
//	D3DXMATRIX matRotX, matYInv;
//	::D3DXMatrixRotationX(&matRotX, D3DX_PI/2);					// oY => oZ
//	::D3DXMatrixScaling(&matYInv, 1.0f, -1.0f, 1.0f);			// -oZ = -`oY => `oY
//	::D3DXMatrixMultiply(&g_matWorld, &matRotX, &matYInv);

	::D3DXMatrixIdentity(&g_matWorld);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &g_matWorld);	// NOTE: we set identity world matrix!

	// Create camera
	gCamera.Create(0.0f, 0.0f, -10.0f,
				   0.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f,
				   &g_matWorld,
				   300.0f, 0.25f,
				   D3DX_PI/4, 4.0f/3.0f, 1.0f, 128.0f);

	// Set camera`s view
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &gCamera.m_matView);

	// Set camera`s projection
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &gCamera.m_matProjection);

	// Save global window`s HWND
	g_hWnd = hWnd;

/*	{
		D3DXMATRIX YRot, ZRot, Sum;
		D3DXMatrixRotationY(&YRot, D3DX_PI / 4);
		D3DXMatrixRotationZ(&ZRot, D3DX_PI / 4);
		Sum = YRot * ZRot;
		return TRUE;
	}*/

	Test();
	return TRUE;
}



//
//	FUNCTION:	void GameDraw(void)
//
//  PURPOSE:	Rendering new frame.
//				(Set time, set matrices, textures lights, clear frame, drawing, flip buffers...)
//
//	RETURN:		None
//
void GameDraw(void)
{
	//     T I M E     //

    // Keep track of the frame count
	static DWORD dwStartTime = ::timeGetTime();
    static DWORD dwLastTime = ::timeGetTime();
    static DWORD dwFrames = 0;
	static TCHAR szFPSInfoStr[256];

	// Start frame time
	DWORD dwTime = ::timeGetTime();
    ++dwFrames;
    // Update the scene stats once per second
    if (dwTime - dwLastTime > 1000)
    {
        FLOAT fFPS  = dwFrames / ((dwTime - dwLastTime) * 0.001f);
        dwLastTime	= dwTime;
        dwFrames	= 0;

		_stprintf(szFPSInfoStr, _T("\"Physics - Max Berezhnoy\" %s, FPS: %.2f, Time: %.2f"),
			AdapterInfo.Description, fFPS, (dwTime - dwStartTime) * 0.001f);
		::SetWindowText(g_hWnd, szFPSInfoStr);
	}

	//     D R A W I N G     //

	// Set camera`s view
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &gCamera.m_matView);

	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(160, 160, 160), 1.0f, 0);
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		// Rotate qube
//		g_pD3DDevice->SetTransform(D3DTS_WORLD, &qZRot);
//		::DrawQube(g_pD3DDevice, 1.0f, D3DCOLOR_XRGB(255, 0, 0));

		static DWORD dwOldTick;
		DWORD dwCurrentTick = ::timeGetTime();
		DWORD dwDeltaTick = dwCurrentTick - dwOldTick;
		if (dwDeltaTick)
		{
			// Elapsed time restriction
			if (dwDeltaTick > 10)
				dwDeltaTick = 10;
			// Resolve contacts
			g_Simulator.Simulate(dwDeltaTick * 0.001f);
			dwOldTick = dwCurrentTick;
		}

		g_Simulator.DrawWorld(g_pD3DDevice);
		
		// End the scene
		g_pD3DDevice->EndScene();
	}

	// Present the scene to the user
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}



//
//	FUNCTION:	void GameShutdown(void)
//
//  PURPOSE:	Free all game resources include COM interfaces (textures, fonts, Direct3D device...)
//
//	RETURN:		None
//
void GameShutdown(void)
{
	// Release font
	SAFE_RELEASE(g_pD3DFont);
	// Release D3D objects
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pD3D);
}



//
//	FUNCTION:	GameInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//
//  PURPOSE:	Process key, mouse input.
//				Callback proc for main window`s WndProc().
//
//	RETURN:		None
//
void GameInput(HWND /*hWnd*/, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
			gCamera.SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			gCamera.LockMouse();
		break;
	case WM_MOUSEMOVE:
			gCamera.SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			gCamera.SetViewByMouse();
			gCamera.Update(g_matWorld);
		break;
	case WM_LBUTTONUP:
			gCamera.UnlockMouse();
		break;
	case WM_KEYDOWN:
		{
			FLOAT TimeSpeed = gCamera.GetTimeSpeed(0.001F);
			switch(wParam)
			{
			case 'W': 		gCamera.Move(TimeSpeed);		gCamera.Update(g_matWorld); break;
			case 'S': 		gCamera.Move(-TimeSpeed);		gCamera.Update(g_matWorld); break;
			case 'A': 		gCamera.Strafe(-TimeSpeed);		gCamera.Update(g_matWorld); break;
			case 'D': 		gCamera.Strafe(TimeSpeed);		gCamera.Update(g_matWorld); break;
			case 'E': 		gCamera.Elevate(TimeSpeed);		gCamera.Update(g_matWorld); break;
			case 'Q': 		gCamera.Elevate(-TimeSpeed);	gCamera.Update(g_matWorld); break;
			case VK_ESCAPE: ::PostQuitMessage(0);										break;
			}
		}
		break;
	}
}