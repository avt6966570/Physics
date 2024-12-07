#include "StdAfx.h"
#include "D3DCamera.h"

D3DCamera::D3DCamera(void)
{
}


D3DCamera::D3DCamera(FLOAT fEyeX, FLOAT fEyeY, FLOAT fEyeZ,
					 FLOAT fAtX,  FLOAT fAtY,  FLOAT fAtZ,
					 FLOAT fUpX,  FLOAT fUpY,  FLOAT fUpZ,
					 const LPD3DXMATRIX pmatWorld,
					 FLOAT fSpeed, FLOAT fMouseSensitivity,
					 FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
					 FLOAT fWidth, FLOAT fHeight,
					 FLOAT fLeft, FLOAT fRight, FLOAT fBottom, FLOAT fTop)
	: m_eType(fFOV != 0.0f ? PERSPECTIVE : ORTHO)
	, m_ptEye(fEyeX, fEyeY, fEyeZ)
	, m_ptAt(fAtX, fAtY, fAtZ)
	, m_vUp(fUpX, fUpY, fUpZ)
	, m_fSpeed(fSpeed)
	, m_fFOV(fFOV)
	, m_fAspect(fAspect)
	, m_fNearPlane(fNearPlane)
	, m_fFarPlane(fFarPlane)
	, m_fWidth(fWidth)
	, m_fHeight(fHeight)
	, m_fLeft(fLeft)
	, m_fRight(fRight)
	, m_fBottom(fBottom)
	, m_fTop(fTop)
	, m_fMouseSensitivity(fMouseSensitivity)
	, m_bMouseLocked(false)
{
	SetProjectionMatrices(fFOV, fAspect, fNearPlane, fFarPlane,
						  fWidth, fHeight, fLeft, fRight, fBottom, fTop);
	NULL != pmatWorld ? Update(*pmatWorld) : Update();
}


D3DCamera::D3DCamera(const D3DXVECTOR3& ptEye, const D3DXVECTOR3& ptAt, const D3DXVECTOR3& vUp,
					 const LPD3DXMATRIX pmatWorld,
					 FLOAT fSpeed, FLOAT fMouseSensitivity,
					 FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
					 FLOAT fWidth, FLOAT fHeight,
					 FLOAT fLeft, FLOAT fRight, FLOAT fBottom, FLOAT fTop)
	: m_eType(fFOV != 0.0f ? PERSPECTIVE : ORTHO)
	, m_ptEye(ptEye)
	, m_ptAt(ptAt)
	, m_vUp(vUp)
	, m_fSpeed(fSpeed)
	, m_fFOV(fFOV)
	, m_fAspect(fAspect)
	, m_fNearPlane(fNearPlane)
	, m_fFarPlane(fFarPlane)
	, m_fWidth(fWidth)
	, m_fHeight(fHeight)
	, m_fLeft(fLeft)
	, m_fRight(fRight)
	, m_fBottom(fBottom)
	, m_fTop(fTop)
	, m_fMouseSensitivity(fMouseSensitivity)
	, m_bMouseLocked(false)
{
	SetProjectionMatrices(fFOV, fAspect, fNearPlane, fFarPlane,
						  fWidth, fHeight, fLeft, fRight, fBottom, fTop);
	NULL != pmatWorld ? Update(*pmatWorld) : Update();
}


void D3DCamera::Create(FLOAT fEyeX, FLOAT fEyeY, FLOAT fEyeZ,
					   FLOAT fAtX,  FLOAT fAtY,  FLOAT fAtZ,
					   FLOAT fUpX,  FLOAT fUpY,  FLOAT fUpZ,
					   const LPD3DXMATRIX pmatWorld,
					   FLOAT fSpeed, FLOAT fMouseSensitivity,
					   FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
					   FLOAT fWidth, FLOAT fHeight,
					   FLOAT fLeft, FLOAT fRight, FLOAT fBottom, FLOAT fTop)
{
	m_ptEye.x	= fEyeX;
	m_ptEye.y	= fEyeY;
	m_ptEye.z	= fEyeZ;
	m_ptAt.x	= fAtX;
	m_ptAt.y	= fAtY;
	m_ptAt.z	= fAtZ;
	m_vUp.x		= fUpX;
	m_vUp.y		= fUpY;
	m_vUp.z		= fUpZ;

	m_fSpeed	= fSpeed;

	m_fFOV		= fFOV;
	m_fAspect	= fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane	= fFarPlane;
	m_fWidth	= fWidth;
	m_fHeight	= fHeight;
	m_fLeft		= fLeft;
	m_fRight	= fRight;
	m_fBottom	= fBottom;
	m_fTop		= fTop;

	m_fMouseSensitivity = fMouseSensitivity;
	m_bMouseLocked		= false;

	SetProjectionMatrices(fFOV, fAspect, fNearPlane, fFarPlane,
						  fWidth, fHeight, fLeft, fRight, fBottom, fTop);
	NULL != pmatWorld ? Update(*pmatWorld) : Update();
}


void D3DCamera::Create(const D3DXVECTOR3& ptEye, const D3DXVECTOR3& ptAt, const D3DXVECTOR3& vUp,
					   const LPD3DXMATRIX pmatWorld,
					   FLOAT fSpeed, FLOAT fMouseSensitivity,
					   FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
					   FLOAT fWidth, FLOAT fHeight,
					   FLOAT fLeft, FLOAT fRight, FLOAT fBottom, FLOAT fTop)
{
	m_ptEye = ptEye;
	m_ptAt	= ptAt;
	m_vUp	= vUp;

	m_fSpeed	= fSpeed;

	m_fFOV		= fFOV;
	m_fAspect	= fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane	= fFarPlane;
	m_fWidth	= fWidth;
	m_fHeight	= fHeight;
	m_fLeft		= fLeft;
	m_fRight	= fRight;
	m_fBottom	= fBottom;
	m_fTop		= fTop;

	m_fMouseSensitivity = fMouseSensitivity;
	m_bMouseLocked		= false;

	SetProjectionMatrices(fFOV, fAspect, fNearPlane, fFarPlane,
						  fWidth, fHeight, fLeft, fRight, fBottom, fTop);
	NULL != pmatWorld ? Update(*pmatWorld) : Update();
}


void D3DCamera::SetProjectionMatrices(FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
									  FLOAT fWidth, FLOAT fHeight,
									  FLOAT fLeft, FLOAT fRight, FLOAT fBottom, FLOAT fTop)
{
	if (fFOV != 0.0f)
	{
		// Create regular projection matrix
		D3DXMatrixPerspectiveFovLH(&m_matProjection, fFOV, fAspect, fNearPlane, fFarPlane);
		// Save inverse projection matrix for fast frustum-AABB calculation
		D3DXMatrixInverse(&m_matInverseProjection, NULL, &m_matProjection);
		// Create a unit-space matrix for sky box geometry.
		// This ensures that the near and far plane enclose the unit space around the camera
		D3DXMatrixPerspectiveFovLH(&m_matUnitProjection, fFOV, fAspect, 0.01f, 2.0f);
	}
	else if (fLeft != 0.0f || fRight != 0.0f || fBottom != 0.0f || fTop != 0.0f)
	{
		// Create regular projection matrix
		D3DXMatrixOrthoOffCenterLH(&m_matProjection, fLeft, fRight, fBottom, fTop, fNearPlane, fFarPlane);
		// Save inverse projection matrix for fast frustum-AABB calculation
		D3DXMatrixInverse(&m_matInverseProjection, NULL, &m_matProjection);
		// Create a unit-space matrix for sky box geometry.
		// This ensures that the near and far plane enclose the unit space around the camera
		D3DXMatrixOrthoOffCenterLH(&m_matUnitProjection, fLeft, fRight, fBottom, fTop, 0.01f, 2.0f);
	}
	else
	{
		// Create regular projection matrix
		D3DXMatrixOrthoLH(&m_matProjection, fWidth, fHeight, fNearPlane, fFarPlane);
		// Save inverse projection matrix for fast frustum-AABB calculation
		D3DXMatrixInverse(&m_matInverseProjection, NULL, &m_matProjection);
		// Create a unit-space matrix for sky box geometry.
		// This ensures that the near and far plane enclose the unit space around the camera
		D3DXMatrixOrthoLH(&m_matUnitProjection, fWidth, fHeight, 0.01f, 2.0f);
	}
}


void D3DCamera::Update()
{	// Update without updating billbord matrix and AABBox

	// GET CAMERA FORWARD DIRECTION
	m_vForward = m_ptAt - m_ptEye;
	D3DXVec3Normalize(&m_vForward, &m_vForward);

	// GET CAMERA STRAFE DIRECTION
	D3DXVec3Cross(&m_vStrafe, &m_vUp, &m_vForward);
	D3DXVec3Normalize(&m_vStrafe, &m_vStrafe);

	// CALCULATE VIEW MATRIX
//	D3DXVECTOR3 vRealUp;
//	D3DXVec3Cross(&vRealUp, &m_vForward, &m_vStrafe);
//	m_matView._11 = m_vStrafe.x;
//	m_matView._12 =   vRealUp.x;
//	m_matView._13 = m_vForward.x;
//	m_matView._14 = 0.0f;
//	m_matView._21 = m_vStrafe.y;
//	m_matView._22 =   vRealUp.y;
//	m_matView._23 = m_vForward.y;
//	m_matView._24 = 0.0f;
//	m_matView._31 = m_vStrafe.z;
//	m_matView._32 =   vRealUp.z;
//	m_matView._33 = m_vForward.z;
//	m_matView._34 = 0.0f;
//	m_matView._41 = -D3DXVec3Dot(&m_vStrafe, &m_ptEye);
//	m_matView._42 = -D3DXVec3Dot(&vRealUp, &m_ptEye);
//	m_matView._43 = -D3DXVec3Dot(&m_vForward, &m_ptEye);
//	m_matView._44 = 1.0f;
	D3DXMatrixLookAtLH(&m_matView, &m_ptEye, &m_ptAt, &m_vUp);

	// CALCULATE SKYBOX MATRIX
	// SkyBox use camera view matrix without any translation information
	// This is combined with the unit space projection matrix to form
	// the sky box viewing matrix
	m_matSkyBox = m_matView;
	m_matSkyBox._41 = 0.0f;
	m_matSkyBox._42 = 0.0f;
	m_matSkyBox._43 = 0.0f;
	D3DXMatrixMultiply(&m_matSkyBox, &m_matSkyBox, &m_matUnitProjection);

	// CALCULATE VIEW-PROJECTION MATRIX
	D3DXMatrixMultiply(&m_matViewProjection, &m_matView, &m_matProjection);

	// UPDATE FRUSTUM
	m_frustum.Extract(&m_matViewProjection);
}


void D3DCamera::Update(const D3DXMATRIX& matWorld)
{
	Update();

	// CALCULATE BILLBOARD MATRIX
	// Billboard objects use our world matrix without translation
	m_matBillboard = matWorld;
	m_matBillboard._41 = 0.0f;
	m_matBillboard._42 = 0.0f;
	m_matBillboard._43 = 0.0f;

	// CALCULATE AABB
	D3DXMATRIX matInverseView;
	D3DXMatrixInverse(&matInverseView, NULL, &m_matView);
	D3DXMatrixMultiply(&matInverseView, &m_matInverseProjection, &matInverseView);

	static const D3DVECTOR ptUnitBounds[] = {
		{ -1.0f,+1.0f,+1.0f },
		{ -1.0f,-1.0f,+1.0f },
		{ +1.0f,-1.0f,+1.0f },
		{ +1.0f,+1.0f,+1.0f },
		{ -1.0f,+1.0f, 0.0f },
		{ -1.0f,-1.0f, 0.0f },
		{ +1.0f,-1.0f, 0.0f },
		{ +1.0f,+1.0f, 0.0f }
	};

	D3DXVECTOR3 ptFrustumBounds[8];
	D3DXVec3TransformCoordArray(ptFrustumBounds, sizeof(D3DXVECTOR3),
								(const D3DXVECTOR3*)ptUnitBounds, sizeof(D3DVECTOR),
								&matInverseView, sizeof(ptUnitBounds)/sizeof(ptUnitBounds[0]));
	D3DXComputeBoundingBox(ptFrustumBounds, sizeof(ptFrustumBounds)/sizeof(ptFrustumBounds[0]), sizeof(D3DXVECTOR3),
						   &m_vAABBox[0], &m_vAABBox[1]);
}


void D3DCamera::UpdateForwardDirection()
{
	m_vForward = m_ptAt - m_ptEye;
	D3DXVec3Normalize(&m_vForward, &m_vForward);
}


void D3DCamera::UpdateStrafeDirection()
{
	D3DXVec3Cross(&m_vStrafe, &m_vUp, &m_vForward);
	D3DXVec3Normalize(&m_vStrafe, &m_vStrafe);
}


void D3DCamera::UpdateViewMatrix()
{
//	D3DXVECTOR3 vRealUp;
//	D3DXVec3Cross(&vRealUp, &m_vForward, &m_vStrafe);
//	m_matView._11 = m_vStrafe.x;
//	m_matView._12 =   vRealUp.x;
//	m_matView._13 = m_vForward.x;
//	m_matView._14 = 0.0f;
//	m_matView._21 = m_vStrafe.y;
//	m_matView._22 =   vRealUp.y;
//	m_matView._23 = m_vForward.y;
//	m_matView._24 = 0.0f;
//	m_matView._31 = m_vStrafe.z;
//	m_matView._32 =   vRealUp.z;
//	m_matView._33 = m_vForward.z;
//	m_matView._34 = 0.0f;
//	m_matView._41 = -D3DXVec3Dot(&m_vStrafe, &m_ptEye);
//	m_matView._42 = -D3DXVec3Dot(&vRealUp, &m_ptEye);
//	m_matView._43 = -D3DXVec3Dot(&m_vForward, &m_ptEye);
//	m_matView._44 = 1.0f;

	D3DXMatrixLookAtLH(&m_matView, &m_ptEye, &m_ptAt, &m_vUp);
}


void D3DCamera::UpdateSkyBoxMatrix()
{
	// SkyBox use camera view matrix without any translation information
	// This is combined with the unit space projection matrix to form
	// the sky box viewing matrix
	m_matSkyBox = m_matView;
	m_matSkyBox._41 = 0.0f;
	m_matSkyBox._42 = 0.0f;
	m_matSkyBox._43 = 0.0f;
	D3DXMatrixMultiply(&m_matSkyBox, &m_matSkyBox, &m_matUnitProjection);
}


void D3DCamera::UpdateBillboardMatrix(const D3DXMATRIX& matWorld)
{
	// Billboard objects use our world matrix without translation
	m_matBillboard = matWorld;
	m_matBillboard._41 = 0.0f;
	m_matBillboard._42 = 0.0f;
	m_matBillboard._43 = 0.0f;
}

void D3DCamera::UpdateViewProjectionMatrix()
{
	D3DXMatrixMultiply(&m_matViewProjection, &m_matView, &m_matProjection);
}


void D3DCamera::UpdateFrustumAABB(const D3DXMATRIX& matWorld)
{
	Update();

	// CALCULATE BILLBOARD MATRIX
	// Billboard objects use our world matrix without translation
	m_matBillboard = matWorld;
	m_matBillboard._41 = 0.0f;
	m_matBillboard._42 = 0.0f;
	m_matBillboard._43 = 0.0f;

	// CALCULATE AABB
	D3DXMATRIX matInverseView;
	D3DXMatrixInverse(&matInverseView, NULL, &m_matView);
	D3DXMatrixMultiply(&matInverseView, &m_matInverseProjection, &matInverseView);

	static const D3DVECTOR ptUnitBounds[] = {
		{ -1.0f,+1.0f,+1.0f },
		{ -1.0f,-1.0f,+1.0f },
		{ +1.0f,-1.0f,+1.0f },
		{ +1.0f,+1.0f,+1.0f },
		{ -1.0f,+1.0f, 0.0f },
		{ -1.0f,-1.0f, 0.0f },
		{ +1.0f,-1.0f, 0.0f },
		{ +1.0f,+1.0f, 0.0f }
	};

	D3DXVECTOR3 ptBounds[8];
	D3DXVec3TransformCoordArray(ptBounds, sizeof(D3DXVECTOR3), (const D3DXVECTOR3*)ptUnitBounds, sizeof(D3DVECTOR),
								&matInverseView, sizeof(ptUnitBounds)/sizeof(ptUnitBounds[0]));
	D3DXComputeBoundingBox(ptBounds, sizeof(ptBounds)/sizeof(ptBounds[0]), sizeof(D3DXVECTOR3),
						   &m_vAABBox[0], &m_vAABBox[1]);
}


void D3DCamera::UpdateFrustum()
{
	m_frustum.Extract(&m_matViewProjection);
}


void D3DCamera::Place(const D3DXVECTOR3& ptEye, const D3DXVECTOR3& ptAt, const D3DXVECTOR3& vUp)
{
	m_ptEye	= ptEye;
	m_ptAt	= ptAt;
	m_vUp	= vUp;
}


void D3DCamera::Place(FLOAT fEyeX, FLOAT fEyeY, FLOAT fEyeZ,
					  FLOAT fAtX,  FLOAT fAtY,  FLOAT fAtZ,
					  FLOAT fUpX,  FLOAT fUpY,  FLOAT fUpZ)
{
	m_ptEye.x = fEyeX;
	m_ptEye.y = fEyeY;
	m_ptEye.z = fEyeZ;
	m_ptAt.x  = fAtX;
	m_ptAt.y  = fAtY;
	m_ptAt.z  = fAtZ;
	m_vUp.x   = fUpX;
	m_vUp.y   = fUpY;
	m_vUp.z	  = fUpZ;
}


void D3DCamera::SetMousePosition(int iMouseX, int iMouseY)
{
	m_vMousePos.x = (FLOAT)iMouseX;
	m_vMousePos.y = (FLOAT)iMouseY;
}


void D3DCamera::SetViewByMouse()
{
	if (m_bMouseLocked && m_vMousePos != m_vMouseLockedPos)
	{
		D3DXVECTOR2 vMouseDelta = (m_vMouseLockedPos - m_vMousePos) * (D3DX_PI / 180.0f) * m_fMouseSensitivity;
		Rotate(-vMouseDelta.y, m_vStrafe);
		Rotate(-vMouseDelta.x, m_vUp);
		m_vMouseLockedPos = m_vMousePos;
	}
}


void D3DCamera::LockMouse()
{
	m_vMouseLockedPos	= m_vMousePos;
	m_bMouseLocked		= true;
}


void D3DCamera::UnlockMouse()
{
	m_bMouseLocked	= false;
}


FLOAT D3DCamera::GetTimeSpeed(FLOAT fFrameTime) const
{
	return m_fSpeed * fFrameTime;
}


void D3DCamera::Move(FLOAT fSpeed)
{
	D3DXVECTOR3 vDelta = m_vForward * fSpeed;
	m_ptEye += vDelta;
	m_ptAt  += vDelta;
}


void D3DCamera::Strafe(FLOAT fSpeed)
{
	m_ptEye.x += m_vStrafe.x * fSpeed;
	m_ptEye.z += m_vStrafe.z * fSpeed;
	m_ptAt.x  += m_vStrafe.x * fSpeed;
	m_ptAt.z  += m_vStrafe.z * fSpeed;
}


void D3DCamera::Elevate(FLOAT fSpeed)
{
	m_ptEye.y += fSpeed;
	m_ptAt.y  += fSpeed;
}


void D3DCamera::Rotate(FLOAT fAngle, const D3DXVECTOR3& vAxis)
{
	// Get rotation matrix
	D3DXMATRIX matRotation;
	::D3DXMatrixRotationAxis(&matRotation, &vAxis, fAngle);
	// Rotate view direction
	::D3DXVec3TransformNormal(&m_ptAt, &m_vForward, &matRotation);
	// Get focus view
	m_ptAt += m_ptEye;
	// Update forward
	UpdateForwardDirection();
}
