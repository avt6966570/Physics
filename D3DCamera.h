#pragma once
#include "Frustum.h"

class D3DCamera
{
	// Construct
public:
	D3DCamera(void);

	D3DCamera(FLOAT fEyeX, FLOAT fEyeY, FLOAT fEyeZ,
			  FLOAT fAtX,  FLOAT fAtY,  FLOAT fAtZ,
			  FLOAT fUpX,  FLOAT fUpY,  FLOAT fUpZ,
			  const LPD3DXMATRIX pmatWorld,
			  FLOAT fSpeed, FLOAT fMouseSensitivity,
			  FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
			  FLOAT fWidth = 0.0f, FLOAT fHeight = 0.0f,
			  FLOAT fLeft = 0.0f, FLOAT fRight = 0.0f, FLOAT fBottom = 0.0f, FLOAT fTop = 0.0f);

	D3DCamera(const D3DXVECTOR3& ptEye, const D3DXVECTOR3& ptAt, const D3DXVECTOR3& vUp,
			  const LPD3DXMATRIX pmatWorld,
			  FLOAT fSpeed, FLOAT fMouseSensitivity,
			  FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
			  FLOAT fWidth = 0.0f, FLOAT fHeight = 0.0f,
			  FLOAT fLeft = 0.0f, FLOAT fRight = 0.0f, FLOAT fBottom = 0.0f, FLOAT fTop = 0.0f);

	void Create(FLOAT fEyeX, FLOAT fEyeY, FLOAT fEyeZ,
				FLOAT fAtX,  FLOAT fAtY,  FLOAT fAtZ,
				FLOAT fUpX,  FLOAT fUpY,  FLOAT fUpZ,
				const LPD3DXMATRIX pmatWorld,
				FLOAT fSpeed, FLOAT fMouseSensitivity,
				FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
				FLOAT fWidth = 0.0f, FLOAT fHeight = 0.0f,
				FLOAT fLeft = 0.0f, FLOAT fRight = 0.0f, FLOAT fBottom = 0.0f, FLOAT fTop = 0.0f);

	void Create(const D3DXVECTOR3& ptEye, const D3DXVECTOR3& ptAt, const D3DXVECTOR3& vUp,
				const LPD3DXMATRIX pmatWorld,
				FLOAT fSpeed, FLOAT fMouseSensitivity,
				FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
				FLOAT fWidth = 0.0f, FLOAT fHeight = 0.0f,
				FLOAT fLeft = 0.0f, FLOAT fRight = 0.0f, FLOAT fBottom = 0.0f, FLOAT fTop = 0.0f);


	void SetProjectionMatrices(FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane, 
							   FLOAT fWidth = 0.0f, FLOAT fHeight = 0.0f,
							   FLOAT fLeft = 0.0f, FLOAT fRight = 0.0f,
							   FLOAT fBottom = 0.0f, FLOAT fTop = 0.0f);

	
	void Place(const D3DXVECTOR3& ptEye, const D3DXVECTOR3& ptAt, const D3DXVECTOR3& vUp);
	void Place(FLOAT EyeX, FLOAT EyeY, FLOAT EyeZ,
				FLOAT AtX, FLOAT AtY, FLOAT AtZ,
				FLOAT UpX, FLOAT UpY, FLOAT UpZ);

	// Movement
	void Move(FLOAT fSpeed);
	void Strafe(FLOAT fSpeed);
	void Elevate(FLOAT fSpeed);
	void Rotate(FLOAT fAngle, const D3DXVECTOR3& vAxis);

	// Mouse
	void LockMouse();
	void UnlockMouse();
	void SetMousePosition(int iMouseX, int iMouseY);

	// Update
	void Update();								// Update without updating billbord matrix and AABBox
	void Update(const D3DXMATRIX& matWorld);	// Update all
	void SetViewByMouse();
	FLOAT GetTimeSpeed(FLOAT fFrameTime) const;

	// Utils
protected:
	void UpdateForwardDirection();
	void UpdateStrafeDirection();
	void UpdateViewMatrix();
	void UpdateSkyBoxMatrix();
	void UpdateBillboardMatrix(const D3DXMATRIX& matWorld);
	void UpdateViewProjectionMatrix();
	void UpdateFrustumAABB(const D3DXMATRIX& matWorld);
	void UpdateFrustum();

	// Constants
public:
	enum Type
	{
		PERSPECTIVE,
		ORTHO
	};


	// Implementation
	Type			m_eType;				// ortho or perspective

	D3DXVECTOR3		m_ptEye;				// camera eye point
	D3DXVECTOR3		m_ptAt;					// look-at camera target
	D3DXVECTOR3		m_vUp;					// current world's up, usually [0, 1, 0]

	D3DXVECTOR3		m_vForward;				// camera forward direction
	D3DXVECTOR3		m_vStrafe;				// camera strafe direction

	FLOAT			m_fSpeed;				// camera speed

	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProjection;
	D3DXMATRIX		m_matInverseProjection;	// for fast frustum-aabb calculation
	D3DXMATRIX		m_matUnitProjection;	// for fast sky-box matrix calculation
	D3DXMATRIX		m_matViewProjection;
	D3DXMATRIX		m_matSkyBox;
	D3DXMATRIX		m_matBillboard;

	D3DXVECTOR3		m_vAABBox[2];			// camera frustum AABB

	// Projection matrix attributes
	FLOAT			m_fFOV;
	FLOAT			m_fAspect;
	FLOAT			m_fNearPlane;
	FLOAT			m_fFarPlane;
	FLOAT			m_fWidth;
	FLOAT			m_fHeight;
	FLOAT			m_fLeft;
	FLOAT			m_fRight;
	FLOAT			m_fBottom;
	FLOAT			m_fTop;

	Frustum			m_frustum;				// camera`s frustum

	// Mouse
	D3DXVECTOR2		m_vMousePos;			// mouse screen coord position
	D3DXVECTOR2		m_vMouseLockedPos;		// mouse position in locked mode
	FLOAT			m_fMouseSensitivity;	// mouse sensitivity [0 - 1]
	bool			m_bMouseLocked;			// is mouse pressed?
};
