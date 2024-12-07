// Physics.h : Defines base physic collision objects.
//

#pragma once


enum CollisionState
{
	CS_CLEAR,
	CS_COLLIDING,
	CS_PENETRATING
};


class AnimationBox
{
	// Internal structs
public:
private:
	struct Configuration
	{
		FLOAT		m_fRotationAngle;
		D3DXMATRIX	m_matOrientation;
	};

	// Construct
public:
	AnimationBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, D3DCOLOR dwColor,
				 FLOAT fRotationVelocity = D3DX_PI / 6.0f,
				 CONST D3DXVECTOR3& vRotationAxis = D3DXVECTOR3(0.0f, 0.0f, 1.0f),
				 FLOAT fInitialAngle = D3DX_PI / 4.0f);

	// Utils
public:
	void SetTransformation(DWORD nCfg, CONST D3DXVECTOR3 *pAxis, CONST FLOAT *pAngle);
	D3DXVECTOR3	GetContactVelocity(CONST D3DXVECTOR3& ptContactPoint) const;

	// Physic
public:
	void Integrate(FLOAT fDeltaTime,
				   DWORD nSourceCfg, DWORD nTargetCfg);
	CollisionState CheckCollisions(CONST D3DXVECTOR3& vPoint, D3DXVECTOR3 *pContactNormal) const;
	void UpdateGeometry(DWORD nCfg);
private:

	// Drawing
public:
	void Draw(LPDIRECT3DDEVICE9 pD3DDevice, DWORD nCfg) const;

	// Implementation
public:
private:
	// Base info
	D3DXVECTOR3		m_vHalfSize;
	D3DCOLOR		m_dwColor;
	FLOAT			m_fRotationVelocity;
	D3DXVECTOR3		m_vRotationAxis;
	D3DXVECTOR3		m_vAngularVelocity;

	// Two "source-target" configurations
	Configuration	m_cfg[2];

	// Geometry
	D3DXPLANE		m_plnSides[6];

	friend class Simulator;
};


class RigidBox
{
	// Internal structs
public:
private:
	struct Configuration
	{
		// Base quantities
		D3DXVECTOR3		m_vPosition;
		D3DXMATRIX		m_matOrientation;
		D3DXVECTOR3		m_vLinearVelocity;
		D3DXVECTOR3		m_vAngularMomentum;

		// Auxilary quantities
		D3DXMATRIX		m_matInvWorldInertiaTensor;
		D3DXVECTOR3		m_vAngularVelocity;
	};

	// Construct
public:
	RigidBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fMass, D3DCOLOR dwColor,
			 RigidBox *pNext = NULL,
			 CONST D3DXVECTOR3& vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
			 CONST D3DXMATRIX& matOrientation = D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f,
															0.0f, 1.0f, 0.0f, 0.0f,
															0.0f, 0.0f, 1.0f, 0.0f,
															0.0f, 0.0f, 0.0f, 1.0f),
			 CONST D3DXVECTOR3& vLinearVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
			 CONST D3DXVECTOR3& vAngularMomentum = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	~RigidBox();

	// Utils
public:
	void		SetTransformation(DWORD nCfg,
								  CONST D3DXVECTOR3 *pPosition, CONST D3DXMATRIX *pOrientation);
	D3DXMATRIX	GetTransformation(DWORD nCfg) const;

	// Physic
public:
private:
	void ComputeForces(
		DWORD nCfg,
		CONST D3DXVECTOR3& vGravity,
		FLOAT fLinearDamping, FLOAT fAngularDamping);

	void Integrate(FLOAT fDeltaTime, DWORD nSourceCfg, DWORD nTargetCfg);

	void UpdateGeometry(DWORD nCfg);

	D3DXVECTOR3 GetContactVelocity(DWORD nCfg, DWORD nContactPoint) const;
	D3DXVECTOR3 GetContactVelocity(DWORD nCfg, CONST D3DXVECTOR3 *pContactPoint) const;

	CollisionState CheckCollisions(
		DWORD nCfg,
		const AnimationBox *pBox,
		DWORD *pContactPoint, D3DXVECTOR3 *pContactNormal) const;
	void ResolveCollisions(
		DWORD nCfg,
		const AnimationBox *pBox,
		DWORD nContactPoint, CONST D3DXVECTOR3 *pContactNormal);

	CollisionState CheckCollisions(
		DWORD nCfg,
		const RigidBox *pBox,
		D3DXVECTOR3 *pContactPoint, D3DXVECTOR3 *pContactNormal) const;
	void ResolveCollisions(
		DWORD nCfg,
		RigidBox *pBox,
		CONST D3DXVECTOR3 *pContactPoint, CONST D3DXVECTOR3 *pContactNormal);

	// Drawing
public:
	void Draw(LPDIRECT3DDEVICE9 pD3DDevice, DWORD nCfg) const;

	// Implementation
public:
private:
	// Body constants
	D3DXVECTOR3		m_vHalfSize;
	FLOAT			m_fInvMass;
	D3DCOLOR		m_dwColor;
	D3DXMATRIX		m_matInvObjectInertiaTensor;

	// Two "source-target" configurations
	Configuration	m_cfg[2];

	// Forces
	D3DXVECTOR3		m_vForce;
	D3DXVECTOR3		m_vTorque;

	// Geometry data
	D3DXVECTOR3		m_ptVertices[8];

	// Support single-linked list
	RigidBox*		m_pNext;

	friend class Simulator;
};



class Simulator
{
	// Internal structs
public:
private:
	struct ContactInfo
	{
		BOOL				bMutual;	// FALSE: regid-animation, TRUE: regid-regid
		RigidBox *			pRigidA;
		union {
			RigidBox *		pRigidB;
			AnimationBox *	pAnimationB;
		};

		union {
			struct { D3DXVECTOR3		ptPoint1; };	// contact point #1
			DWORD			nPoint1;					// # in vertices array
		};

		D3DXVECTOR3			vNormal;					// normal by contact
	};

	// Construct
public:
	Simulator(CONST D3DXVECTOR3& vGravity = D3DXVECTOR3(0.0f, -9.810f, 0.0f),
			  FLOAT fLinearDamping  = -0.04f,
			  FLOAT fAngularDamping = -0.02f);

	~Simulator();

	void AddAnimationBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, D3DCOLOR dwColor,
						 FLOAT fRotationVelocity = D3DX_PI / 6.0f,
						 CONST D3DXVECTOR3& vRotationAxis = D3DXVECTOR3(0.0f, 0.0f, 1.0f),
						 FLOAT fInitialAngle = 0.0f);

	void AddRigidBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fMass, D3DCOLOR dwColor,
					 CONST D3DXVECTOR3& vPosition	   = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					 CONST D3DXMATRIX& matOrientation  = D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f,
																	0.0f, 1.0f, 0.0f, 0.0f,
																	0.0f, 0.0f, 1.0f, 0.0f,
																	0.0f, 0.0f, 0.0f, 1.0f),
					 CONST D3DXVECTOR3& vLinearVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					 CONST D3DXVECTOR3& vAngularMomentum = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// Utils
public:
	void SetGravity(CONST D3DXVECTOR3& vGravity)	{ m_vGravity = vGravity;				}
	void SetLinearDamping(FLOAT fLinearDamping)		{ m_fLinearDamping = fLinearDamping;	}
	void SetAngularDamping(FLOAT fAngularDamping)	{ m_fAngularDamping = fAngularDamping;	}

	// Simulation
public:
	void Simulate(FLOAT fDeltaTime);
private:
	void ComputeForces(DWORD nCfg);
	void Integrate(FLOAT fDeltaTime, DWORD nSourceCfg, DWORD nTargetCfg);
	void UpdateGeometry(DWORD nCfg);
	CollisionState CheckCollisions(DWORD nCfg, ContactInfo *pContactInfo) const;
	void ResolveCollisions(DWORD nCfg, ContactInfo *pContactInfo);

	// Drawing
public:
	void DrawWorld(LPDIRECT3DDEVICE9 pD3DDevice) const;


	// Implementation
public:
private:
	AnimationBox	* m_pAnimationBox;
	RigidBox		* m_pRigidBoxes;

	D3DXVECTOR3		  m_vGravity;
	FLOAT			  m_fLinearDamping;
	FLOAT			  m_fAngularDamping;

	DWORD			  m_nSourceConfiguration;
	DWORD			  m_nTargetConfiguration;
};
