// Physics.cpp : Defines base physic collision objects.
//

#include "StdAfx.h"
#include "Physics.h"
#include "GJK.h"


#define RESTITUTION_COEFFICIENT		0.5f
#define COLLISION_THRESHOLD			0.05f
#define MINIMUM_CONTACT_VELOCITY	0.01f


static __forceinline D3DXVECTOR3 Cross(CONST D3DXVECTOR3& V1, CONST D3DXVECTOR3& V2)
{
	D3DXVECTOR3 vRes;
	D3DXVec3Cross(&vRes, &V1, &V2);
	return vRes;
}

static __forceinline D3DXVECTOR3 Transform(CONST D3DXVECTOR3& V, CONST D3DXMATRIX& M,
										   CONST D3DXVECTOR3 *pVTrans = NULL)
{
	// Perform transformation
	D3DXVECTOR3 vRes;
	D3DXVec3TransformCoord(&vRes, &V, &M);

	// Translate if vector passed
	return pVTrans ? vRes + *pVTrans : vRes;
}


AnimationBox::AnimationBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, D3DCOLOR dwColor,
						   FLOAT fRotationVelocity, CONST D3DXVECTOR3& vRotationAxis,
						   FLOAT fInitialAngle)
	: m_vHalfSize(fWidth / 2.0f, fHeight / 2.0f, fDepth / 2.0f)
	, m_dwColor(dwColor)
	, m_fRotationVelocity(fRotationVelocity)
	, m_vRotationAxis(vRotationAxis)
	, m_vAngularVelocity(vRotationAxis * fRotationVelocity)
{
	SetTransformation(0, NULL, &fInitialAngle);
}

void AnimationBox::SetTransformation(DWORD nCfg, CONST D3DXVECTOR3 *pAxis, CONST FLOAT *pAngle)
{
	if (pAxis)
		m_vRotationAxis = *pAxis;

	if (pAngle)
		m_cfg[nCfg].m_fRotationAngle = *pAngle;

	if (pAxis || pAngle)
	{
		m_vAngularVelocity = m_vRotationAxis * m_fRotationVelocity;
		D3DXMatrixRotationAxis(&m_cfg[nCfg].m_matOrientation, &m_vRotationAxis, m_cfg[nCfg].m_fRotationAngle);
		UpdateGeometry(nCfg);
	}
}

D3DXVECTOR3	AnimationBox::GetContactVelocity(CONST D3DXVECTOR3& ptContactPoint) const
{
	return Cross(m_vAngularVelocity, ptContactPoint);
}

void AnimationBox::Integrate(FLOAT fDeltaTime, DWORD nSourceCfg, DWORD nTargetCfg)
{
	// Integrate rotation angle
	m_cfg[nTargetCfg].m_fRotationAngle =
		m_cfg[nSourceCfg].m_fRotationAngle + m_fRotationVelocity * fDeltaTime;
}

void AnimationBox::UpdateGeometry(DWORD nCfg)
{
	static const D3DXPLANE	s_plnCUQSides[6] = {
		D3DXPLANE(+0.0f, +0.0f, +1.0f, +1.0f),	// front
		D3DXPLANE(-1.0f, +0.0f, +0.0f, +1.0f),	// right
		D3DXPLANE(+0.0f, +0.0f, -1.0f, +1.0f),	// back
		D3DXPLANE(+1.0f, +0.0f, +0.0f, +1.0f),	// left
		D3DXPLANE(+0.0f, -1.0f, +0.0f, +1.0f),	// top
		D3DXPLANE(+0.0f, +1.0f, +0.0f, +1.0f)	// bottom
	};

	// Get orientation matrix from axis & angle
	D3DXMatrixRotationAxis(&m_cfg[nCfg].m_matOrientation, &m_vRotationAxis, m_cfg[nCfg].m_fRotationAngle);

	// Set CUQ size scalers
	D3DXMATRIX matTransformation;
	D3DXMatrixScaling(&matTransformation, m_vHalfSize.x, m_vHalfSize.y, m_vHalfSize.z);

	// Concatenate with box orientation
	D3DXMatrixMultiply(&matTransformation, &matTransformation, &m_cfg[nCfg].m_matOrientation);

	// Transform collision planes
	D3DXMATRIX matITTransformation;
	D3DXMatrixInverse(&matITTransformation, NULL, &matTransformation);
	D3DXMatrixTranspose(&matITTransformation, &matITTransformation);
	for (DWORD p = 0; p < 6; ++p)
	{
		D3DXPlaneTransform(&m_plnSides[p], &s_plnCUQSides[p], &matITTransformation);
		D3DXPlaneNormalize(&m_plnSides[p], &m_plnSides[p]);
	}
}

void AnimationBox::Draw(LPDIRECT3DDEVICE9 pD3DDevice, DWORD nCfg) const
{
	// Set world transformation
	pD3DDevice->SetTransform(D3DTS_WORLD, &m_cfg[nCfg].m_matOrientation);

	// Draw inverted rescaled box
	::DrawParallelepiped(pD3DDevice, -m_vHalfSize, m_dwColor);
}

CollisionState AnimationBox::CheckCollisions(CONST D3DXVECTOR3& vPoint, D3DXVECTOR3 *pContactNormal) const
{
	CollisionState cs = CS_CLEAR;
	for (DWORD p = 0; p < 6; ++p)
	{
		FLOAT fDist = D3DXPlaneDotCoord(&m_plnSides[p], &vPoint);
		if (fDist < -COLLISION_THRESHOLD)
			return CS_PENETRATING;
		else if (fDist < 0.0f)
		{
			pContactNormal->x = m_plnSides[p].a;
			pContactNormal->y = m_plnSides[p].b;
			pContactNormal->z = m_plnSides[p].c;
			cs = CS_COLLIDING;
		}
	}

	return cs;
}




RigidBox::RigidBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fMass, D3DCOLOR dwColor,
				   RigidBox *pNext,
				   CONST D3DXVECTOR3& vPosition, CONST D3DXMATRIX& matOrientation,
				   CONST D3DXVECTOR3& vLinearVelocity, CONST D3DXVECTOR3& vAngularMomentum)
	: m_vHalfSize(fWidth / 2.0f, fHeight / 2.0f, fDepth / 2.0f)
	, m_fInvMass(1.0f / (fMass = fMass ? fMass : fWidth * fHeight * fDepth))
	, m_dwColor(dwColor)
	, m_pNext(pNext)
{
	// Calculate inverse object space inertia tensor
	FLOAT fIxx = fWidth  * fWidth;
	FLOAT fIyy = fHeight * fHeight;
	FLOAT fIzz = fDepth  * fDepth;
	D3DXMatrixScaling(&m_matInvObjectInertiaTensor,
					  1.0f / (fMass * (fIyy + fIzz)),
					  1.0f / (fMass * (fIxx + fIzz)),
					  1.0f / (fMass * (fIxx + fIyy)));

	// Set initial velocities
	m_cfg[0].m_vLinearVelocity  = vLinearVelocity;
	m_cfg[0].m_vAngularMomentum = vAngularMomentum;

	// Update geometry
	SetTransformation(0, &vPosition, &matOrientation);
}

RigidBox::~RigidBox()
{
	delete m_pNext;
}

void RigidBox::SetTransformation(DWORD nCfg,
								 CONST D3DXVECTOR3 *pPosition, CONST D3DXMATRIX *pOrientation)
{
	RigidBox::Configuration *pCFG = &m_cfg[nCfg];

	if (pPosition)
		pCFG->m_vPosition = *pPosition;

	if (pOrientation)
	{
		pCFG->m_matOrientation = *pOrientation;

		// Calculate inverse world space inertia tensor
		D3DXMATRIX matTransposedOrientation;
		D3DXMatrixTranspose(&matTransposedOrientation, pOrientation);
		pCFG->m_matInvWorldInertiaTensor = pCFG->m_matOrientation * m_matInvObjectInertiaTensor * matTransposedOrientation;

		// Calculate angular velocity
		D3DXVec3TransformCoord(&pCFG->m_vAngularVelocity, &pCFG->m_vAngularMomentum, &pCFG->m_matInvWorldInertiaTensor);
	}

	// Final geometry update
	if (pPosition || pOrientation)
		UpdateGeometry(nCfg);
}

D3DXMATRIX	RigidBox::GetTransformation(DWORD nCfg) const
{
	const RigidBox::Configuration *pCFG = &m_cfg[nCfg];

	D3DXMATRIX matTransformation(pCFG->m_matOrientation);
	matTransformation._41 = pCFG->m_vPosition.x;
	matTransformation._42 = pCFG->m_vPosition.y;
	matTransformation._43 = pCFG->m_vPosition.z;
	return matTransformation;
}

void RigidBox::ComputeForces(DWORD nCfg,
							 CONST D3DXVECTOR3& vGravity, FLOAT fLinearDamping, FLOAT fAngularDamping)
{
	const RigidBox::Configuration *pCFG = &m_cfg[nCfg];

	m_vForce  = pCFG->m_vLinearVelocity  * fLinearDamping + vGravity / m_fInvMass;
	m_vTorque =	pCFG->m_vAngularVelocity * fAngularDamping;
}

void RigidBox::Integrate(FLOAT fDeltaTime,
						 DWORD nSourceCfg, DWORD nTargetCfg)
{
	RigidBox::Configuration *pSrc = &m_cfg[nSourceCfg];
	RigidBox::Configuration *pTrg = &m_cfg[nTargetCfg];

	// Integrate position
	pTrg->m_vPosition = pSrc->m_vPosition + pSrc->m_vLinearVelocity * fDeltaTime;

	// Integrate orientation (two methods ;) )
#if 1
	D3DXMATRIX matDerivativeOrientation;
	D3DXMatrixRotationAxis(&matDerivativeOrientation, &pSrc->m_vAngularVelocity,
						   D3DXVec3Length(&pSrc->m_vAngularVelocity) * fDeltaTime);
	pTrg->m_matOrientation = pSrc->m_matOrientation * matDerivativeOrientation;
#else
	// Special matrix for cross-product representation:
	//	R(t)	= [R1(t)   R2(t)   R3(t)]
	//	dR(t)	= [w(t) x R1(t)   w(t) x R2(t)   w(t) x R3(t)]
	D3DXMATRIX matAVCross(0.0f, 						pSrc->m_vAngularVelocity.z,		-pSrc->m_vAngularVelocity.y,0.0f,
						  -pSrc->m_vAngularVelocity.z,	0.0f, 							pSrc->m_vAngularVelocity.x,	0.0f,
						  pSrc->m_vAngularVelocity.y,	-pSrc->m_vAngularVelocity.x,	0.0f,						0.0f,
						  0.0f,							0.0f,							0.0f,						0.0f);
	pTrg->m_matOrientation = pSrc->m_matOrientation + pSrc->m_matOrientation * matAVCross * fDeltaTime;
	OrthonormalizeOrientation(&pTrg->m_matOrientation);
#endif

	// Integrate linear velocity
	pTrg->m_vLinearVelocity  = pSrc->m_vLinearVelocity + m_vForce * fDeltaTime * m_fInvMass;

	// Integrate momentum
	pTrg->m_vAngularMomentum = pSrc->m_vAngularMomentum + m_vTorque * fDeltaTime;

	// Calculate world space inverse inertia tensor
	D3DXMATRIX matTransposedOrientation;
	D3DXMatrixTranspose(&matTransposedOrientation, &pTrg->m_matOrientation);
	pTrg->m_matInvWorldInertiaTensor = pTrg->m_matOrientation * m_matInvObjectInertiaTensor * matTransposedOrientation;

	// Calculate new angular velocity 
	::D3DXVec3TransformCoord(&pTrg->m_vAngularVelocity, &pTrg->m_vAngularMomentum, &pTrg->m_matInvWorldInertiaTensor);
}

void RigidBox::UpdateGeometry(DWORD nCfg)
{
	static CONST D3DXVECTOR3 s_ptCUQVertices[8] = {
		D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(-1.0f, +1.0f, -1.0f),
		D3DXVECTOR3(+1.0f, +1.0f, -1.0f),
		D3DXVECTOR3(+1.0f, -1.0f, -1.0f),

		D3DXVECTOR3(-1.0f, -1.0f, +1.0f),
		D3DXVECTOR3(-1.0f, +1.0f, +1.0f),
		D3DXVECTOR3(+1.0f, +1.0f, +1.0f),
		D3DXVECTOR3(+1.0f, -1.0f, +1.0f)
	};

	// Get summary transformation
	D3DXMATRIX matCUQScale;
	D3DXMatrixScaling(&matCUQScale, m_vHalfSize.x, m_vHalfSize.y, m_vHalfSize.z);
	const D3DXMATRIX matTransformation = matCUQScale * GetTransformation(nCfg);

	// Transform CUQ-vertices to world
	D3DXVec3TransformCoordArray(&m_ptVertices[0], sizeof(D3DXVECTOR3),
								&s_ptCUQVertices[0], sizeof(D3DXVECTOR3),
								&matTransformation, 8);
}

D3DXVECTOR3 RigidBox::GetContactVelocity(DWORD nCfg, DWORD nContactPoint) const
{
	const RigidBox::Configuration *pCFG = &m_cfg[nCfg];

	// Compute relative arm contact
	D3DXVECTOR3 vRA	= m_ptVertices[nContactPoint] - pCFG->m_vPosition;
	// Compute velocity of contact point
	return pCFG->m_vLinearVelocity + Cross(pCFG->m_vAngularVelocity, vRA);
}

D3DXVECTOR3 RigidBox::GetContactVelocity(DWORD nCfg, CONST D3DXVECTOR3 *pContactPoint) const
{
	const RigidBox::Configuration *pCFG = &m_cfg[nCfg];

	// Compute relative arm contact
	D3DXVECTOR3 vRA	= *pContactPoint - pCFG->m_vPosition;
	// Compute velocity of contact point
	return pCFG->m_vLinearVelocity + Cross(pCFG->m_vAngularVelocity, vRA);
}

CollisionState RigidBox::CheckCollisions(DWORD nCfg,
										 const AnimationBox *pBox,
										 DWORD *pContactPoint, D3DXVECTOR3 *pContactNormal) const
{
	CollisionState	cs = CS_CLEAR;
	D3DXVECTOR3		vContactNormal;

	for (DWORD v = 0; v < 8; ++v)
		switch (pBox->CheckCollisions(m_ptVertices[v], &vContactNormal))
		{
		case CS_COLLIDING:
			if (D3DXVec3Dot(&vContactNormal,
							&(GetContactVelocity(nCfg, v) -
							  pBox->GetContactVelocity(m_ptVertices[v]))) < 0.0f)
			{
				*pContactPoint	= v;
				*pContactNormal = vContactNormal;
				cs				= CS_COLLIDING;
			}
			break;
		case CS_PENETRATING:
			return CS_PENETRATING;
		}

	return cs;
}

void RigidBox::ResolveCollisions(DWORD nCfg,
								 const AnimationBox *pBox,
								 DWORD nContactPoint, CONST D3DXVECTOR3 *pContactNormal)
{
	RigidBox::Configuration *pCFG = &m_cfg[nCfg];

	// Compute relative arm contact
	D3DXVECTOR3 vRA	= m_ptVertices[nContactPoint] - pCFG->m_vPosition;
	// Compute velocity of contact point A (rigid body)
	D3DXVECTOR3 vVA = pCFG->m_vLinearVelocity + Cross(pCFG->m_vAngularVelocity, vRA);
	// Compute velocity of contact point B (kinematic body)
	D3DXVECTOR3 vVB = pBox->GetContactVelocity(m_ptVertices[nContactPoint]);
	// Get relative velocity of two contact
	FLOAT fVAB		= D3DXVec3Dot(&(vVA - vVB), pContactNormal);
	// Compute impulse
	FLOAT fIForce   = MINIMUM_CONTACT_VELOCITY - (1.0f + RESTITUTION_COEFFICIENT) * fVAB;
	FLOAT fIDamping = m_fInvMass +
		D3DXVec3Dot(&Cross(Transform(Cross(vRA, *pContactNormal), pCFG->m_matInvWorldInertiaTensor), vRA),
					pContactNormal);
	D3DXVECTOR3 vImpulse = *pContactNormal * (fIForce / fIDamping);

	// Apply impulse to primary quantities
	pCFG->m_vLinearVelocity  += vImpulse * m_fInvMass;
	pCFG->m_vAngularMomentum += Cross(vRA, vImpulse);

	// Compute affected auxiliary quantities
	D3DXVec3TransformCoord(&pCFG->m_vAngularVelocity, &pCFG->m_vAngularMomentum, &pCFG->m_matInvWorldInertiaTensor);
}

CollisionState RigidBox::CheckCollisions(DWORD nCfg,
										 const RigidBox *pBox,
										 D3DXVECTOR3 *pContactPoint, D3DXVECTOR3 *pContactNormal) const
{
	D3DXVECTOR3 vAContactPoint, vBContactPoint;
	FLOAT fDistanceSq = GjkDistanceSq(m_ptVertices, 8, NULL, pBox->m_ptVertices, 8, NULL,
									  NULL, FALSE, &vAContactPoint, &vBContactPoint);
	if (fDistanceSq == 0.0f)
		return CS_PENETRATING;
	else if (fDistanceSq < COLLISION_THRESHOLD * COLLISION_THRESHOLD)
	{
		D3DXVECTOR3 vContactNormal = vAContactPoint - vBContactPoint;
		if (D3DXVec3Dot(&vContactNormal,
						&(GetContactVelocity(nCfg, &vAContactPoint) -
						pBox->GetContactVelocity(nCfg, &vAContactPoint))) < 0.0f)	// NOTE: Set A-point as shared contact point between bodies!
		{	// Save A contact point & NORMALIZED normal
			*pContactPoint = vAContactPoint;
			D3DXVec3Normalize(pContactNormal, &vContactNormal);
			return CS_COLLIDING;
		}
	}

	return CS_CLEAR;
}

void RigidBox::ResolveCollisions(DWORD nCfg,
								 RigidBox *pBox,
								 CONST D3DXVECTOR3 *pContactPoint, CONST D3DXVECTOR3 *pContactNormal)
{
	RigidBox::Configuration *pEff = &m_cfg[nCfg];
	RigidBox::Configuration *pTrg = &pBox->m_cfg[nCfg];

	// Compute relative arm contact (A-body)
	D3DXVECTOR3 vRA	= *pContactPoint - pEff->m_vPosition;
	// Compute relative arm contact (B-body)
	D3DXVECTOR3 vRB	= *pContactPoint - pTrg->m_vPosition;
	// Compute velocity of contact point A
	D3DXVECTOR3 vVA = pEff->m_vLinearVelocity + Cross(pEff->m_vAngularVelocity, vRA);
	// Compute velocity of contact point B
	D3DXVECTOR3 vVB = pTrg->m_vLinearVelocity + Cross(pTrg->m_vAngularVelocity, vRB);
	// Get relative velocity of two contact
	FLOAT fVAB		= D3DXVec3Dot(&(vVA - vVB), pContactNormal);
	// Compute impulse
	FLOAT fIForce   = MINIMUM_CONTACT_VELOCITY - (1.0f + RESTITUTION_COEFFICIENT) * fVAB;
	FLOAT fIDamping = m_fInvMass + pBox->m_fInvMass +
		D3DXVec3Dot(&(Cross(Transform(Cross(vRA, *pContactNormal), pEff->m_matInvWorldInertiaTensor), vRA) +
					  Cross(Transform(Cross(vRB, *pContactNormal), pTrg->m_matInvWorldInertiaTensor), vRB)),
						pContactNormal);
	D3DXVECTOR3 vImpulse = *pContactNormal * (fIForce / fIDamping);

	// Apply impulse to primary quantities
	pEff->m_vLinearVelocity  += vImpulse * m_fInvMass;
	pEff->m_vAngularMomentum += Cross(vRA, vImpulse);
	pTrg->m_vLinearVelocity  -= vImpulse * pBox->m_fInvMass;
	pTrg->m_vAngularMomentum -= Cross(vRB, vImpulse);

	// Compute affected auxiliary quantities
	D3DXVec3TransformCoord(&pEff->m_vAngularVelocity, &pEff->m_vAngularMomentum, &pEff->m_matInvWorldInertiaTensor);
	D3DXVec3TransformCoord(&pTrg->m_vAngularVelocity, &pTrg->m_vAngularMomentum, &pTrg->m_matInvWorldInertiaTensor);
}

void RigidBox::Draw(LPDIRECT3DDEVICE9 pD3DDevice, DWORD nCfg) const
{
	CONST D3DXMATRIX matTransformation = GetTransformation(nCfg);
	pD3DDevice->SetTransform(D3DTS_WORLD, &matTransformation);
	::DrawParallelepiped(pD3DDevice, m_vHalfSize, m_dwColor);
}



Simulator::Simulator(CONST D3DXVECTOR3& vGravity,
					 FLOAT fLinearDamping, FLOAT fAngularDamping)
	: m_pAnimationBox(NULL)
	, m_pRigidBoxes(NULL)
	, m_vGravity(vGravity)
	, m_fLinearDamping(fLinearDamping)
	, m_fAngularDamping(fAngularDamping)
	, m_nSourceConfiguration(0)
	, m_nTargetConfiguration(1)
{
}

Simulator::~Simulator()
{
	delete m_pAnimationBox;
	delete m_pRigidBoxes;
}

void Simulator::AddAnimationBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, D3DCOLOR dwColor,
								FLOAT fRotationVelocity,
								CONST D3DXVECTOR3& vRotationAxis,
								FLOAT fInitialAngle)
{
	delete m_pAnimationBox;
	m_pAnimationBox = new AnimationBox(fWidth, fHeight, fDepth, dwColor,
									   fRotationVelocity, vRotationAxis, fInitialAngle);
}

void Simulator::AddRigidBox(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, FLOAT fMass, D3DCOLOR dwColor,
							CONST D3DXVECTOR3& vPosition,
							CONST D3DXMATRIX&  matOrientation,
							CONST D3DXVECTOR3& vLinearVelocity,
							CONST D3DXVECTOR3& vAngularMomentum)
{
	m_pRigidBoxes = new RigidBox(fWidth, fHeight, fDepth, fMass, dwColor,
								 m_pRigidBoxes,
								 vPosition, matOrientation,
								 vLinearVelocity, vAngularMomentum);
}

void Simulator::ComputeForces(DWORD nCfg)
{
	for (RigidBox *pBox = m_pRigidBoxes; pBox; pBox = pBox->m_pNext)
		pBox->ComputeForces(nCfg, m_vGravity, m_fLinearDamping, m_fAngularDamping);
}

void Simulator::Integrate(FLOAT fDeltaTime, DWORD nSourceCfg, DWORD nTargetCfg)
{
	// Integrate animation object
	m_pAnimationBox->Integrate(fDeltaTime, nSourceCfg, nTargetCfg);

	// Integrate rigid objects
	for (RigidBox *pBox = m_pRigidBoxes; pBox; pBox = pBox->m_pNext)
		pBox->Integrate(fDeltaTime, nSourceCfg, nTargetCfg);
}

void Simulator::UpdateGeometry(DWORD nCfg)
{
	// Update geometry for animation objects
	m_pAnimationBox->UpdateGeometry(nCfg);

	// Update geometry for rigid objects
	for (RigidBox *pBox = m_pRigidBoxes; pBox; pBox = pBox->m_pNext)
		pBox->UpdateGeometry(nCfg);
}

CollisionState Simulator::CheckCollisions(DWORD nCfg, ContactInfo *pContactInfo) const
{
	CollisionState cs = CS_CLEAR;
	for (RigidBox *pBox = m_pRigidBoxes; pBox; pBox = pBox->m_pNext)
	{
		// #1. Test animation box
		switch (pBox->CheckCollisions(nCfg, m_pAnimationBox, &pContactInfo->nPoint1, &pContactInfo->vNormal))
		{
		case CS_COLLIDING:
			pContactInfo->bMutual	  = FALSE;
			pContactInfo->pRigidA	  = pBox;
			pContactInfo->pAnimationB = m_pAnimationBox;
			cs = CS_COLLIDING;
			break;
		case CS_PENETRATING:
			return CS_PENETRATING;
		}

		// #2. Test another rigids
		for (RigidBox *pTarget = m_pRigidBoxes; pTarget; pTarget = pTarget->m_pNext)
			if (pBox != pTarget)
				switch (pBox->CheckCollisions(nCfg, pTarget, &pContactInfo->ptPoint1, &pContactInfo->vNormal))
				{
				case CS_COLLIDING:
					pContactInfo->bMutual = TRUE;
					pContactInfo->pRigidA = pBox;
					pContactInfo->pRigidB = pTarget;
					cs = CS_COLLIDING;
					break;
				case CS_PENETRATING:
					return CS_PENETRATING;
				}
	}

	return cs;
}

void Simulator::ResolveCollisions(DWORD nCfg, ContactInfo *pContactInfo)
{
	if (pContactInfo->bMutual)
		pContactInfo->pRigidA->ResolveCollisions(nCfg, pContactInfo->pRigidB,
			&pContactInfo->ptPoint1, &pContactInfo->vNormal);
	else
		pContactInfo->pRigidA->ResolveCollisions(nCfg, pContactInfo->pAnimationB,
			pContactInfo->nPoint1, &pContactInfo->vNormal);
}

void Simulator::Simulate(FLOAT fDeltaTime)
{
	ContactInfo ci;
	FLOAT fCurrentTime = 0.0f;
	FLOAT fTargetTime  = fDeltaTime;

	while(fCurrentTime < fDeltaTime)
	{
		ComputeForces(m_nSourceConfiguration);
        Integrate(fTargetTime - fCurrentTime, m_nSourceConfiguration, m_nTargetConfiguration);
		UpdateGeometry(m_nTargetConfiguration);

		CollisionState cs = CheckCollisions(m_nTargetConfiguration, &ci);
		if (cs == CS_PENETRATING)
		{
			fTargetTime = (fCurrentTime + fTargetTime) / 2.0f;
			assert(fTargetTime - fCurrentTime > 0.000001f);
			if (fTargetTime - fCurrentTime < 0.000001f)	{
				::MessageBox(NULL, _T("Time threshold exceed!\n(fTargetTime - fCurrentTime < 0.000001f)"), _T("Physics"), MB_ICONERROR|MB_OK);
				::ExitProcess((UINT)-1);
			}
//			cs = CheckCollisions(m_nTargetConfiguration, &ci);
		}
		else
		{
			if (cs == CS_COLLIDING)
			{
				DWORD i = 0;
				do {
					ResolveCollisions(m_nTargetConfiguration, &ci);
				} while(CheckCollisions(m_nTargetConfiguration, &ci) == CS_COLLIDING && ++i < 1000);

				assert(i < 1000);
				assert(CheckCollisions(m_nTargetConfiguration, &ci) == CS_CLEAR);

				if (i >= 1000)	{
					::MessageBox(NULL, _T("Exceed repeat limit!\n(i == 1000)"), _T("Physics"), MB_ICONERROR|MB_OK);
					::ExitProcess((UINT)-1);
				}
//				CheckCollisions(m_nTargetConfiguration, &ci);
			}

            fCurrentTime			= fTargetTime;
            fTargetTime				= fDeltaTime;
			m_nSourceConfiguration	^= 1;
			m_nTargetConfiguration	^= 1;
		}
	}
}

void Simulator::DrawWorld(LPDIRECT3DDEVICE9 pD3DDevice) const
{
	m_pAnimationBox->Draw(pD3DDevice, m_nSourceConfiguration);
	
	for (const RigidBox *pBox = m_pRigidBoxes; pBox; pBox = pBox->m_pNext)
		pBox->Draw(pD3DDevice, m_nSourceConfiguration);
}
