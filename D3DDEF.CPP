#include "StdAfx.h"
#include "d3ddef.h"

/*
 *	3D Math utils
 */

//-----------------------------------------------------------------------------
//	Name:	BaryCentricFromTriangle
//	Desc:	Getting barycentric coordinates (f, g) for known point by triangle
//	Arg:	pV1		-- [in] Pointer to a source D3DXVECTOR3 structure
//							that is the first vertex of trinagle
//			pV2		-- [in] Pointer to a source D3DXVECTOR3 structure
//							that is the second vertex of trinagle
//			pV3		-- [in] Pointer to a source D3DXVECTOR3 structure
//							that is the thrid vertex of trinagle
//			pP		-- [in] Pointer to a source D3DXVECTOR3 structure
//							that is knowing point
//			pF		-- [out] Weighting factor, that is the result of the operation.
//			pG		-- [out] Weighting factor, that is the result of the operation.
//
//	Remarks:
//-----------------------------------------------------------------------------
BOOL BaryCentricFromTriangle(
	CONST D3DXVECTOR3 *pV1,
	CONST D3DXVECTOR3 *pV2,
	CONST D3DXVECTOR3 *pV3,
	CONST D3DXVECTOR3 *pP,
	FLOAT *pF, FLOAT *pG)
{
	D3DXVECTOR3 v21 = *pV2 - *pV1;
	D3DXVECTOR3 v31 = *pV3 - *pV1;
	D3DXVECTOR3 vP	= *pP  - *pV1;

	FLOAT kA[4];
	kA[0] = D3DXVec3Dot(&v21, &v21);
	kA[1] = kA[2] = D3DXVec3Dot(&v21, &v31);
	kA[3] = D3DXVec3Dot(&v31, &v31);

	if (FLOAT kADet = kA[0] * kA[3] - kA[2] * kA[1])
	{
		FLOAT f = D3DXVec3Dot(&vP, &v21);
		FLOAT g = D3DXVec3Dot(&vP, &v31);

		kADet = 1.0f / kADet;
		FLOAT kAInv[4] = { kA[3] * kADet, -kA[1] * kADet, -kA[2] * kADet, kA[0] * kADet };
		*pF = f * kAInv[0] + g * kAInv[2];
		*pG = f * kAInv[1] + g * kAInv[3];
		return TRUE;
	}

	return FALSE;
}



//
//	Name:	MatrixPairPointTransformation
//	Desc:	Calculation affine transformation matrix by two-pair point.
//			Return affine transformation object matrix from state { pO1, pV1 } to
//			state { pO2, pV2 }.
//	Arg:	pOut	-- [out] result affine transformation object matrix
//			pO1		-- [in]  input point  initial state
//			pV1		-- [in]  input vector initial state
//			pO2		-- [in]  input point  end state
//			pV2		-- [in]  input vector end state
//
//	Remarks:
//		По методу парных точек строим две ортогональные системы векторов, связанные
//		с исходным и конечным состоянием объекта:
//			
//			1. вычислим нормаль N = v1 x v2, (если v1||v2 то при v1 # z0 полагаем
//				N = v1 x z0, а при v1 || z0 -- v1 x y0) и бинормаль W = v1 x N,
//				получим исходную систему векторов на четырех точках:
//							{ P1, P1 + v1, P1 + N, P1 + w1 }
//
//			2. вычислив коэффициент подобия k = |v2|/|v1| и бинормаль w2 = v2 x N,
//				получим конечную систему на четырех точках:
//							{ P2, P2 + v2, P2 + kN, P2 + w2 }
//			3. итоговая матрица кинематического перехода {P1, v1}->{P2, v2}
//				явится результатом конкатенации двух матриц ортогональных систем
//				векторов:
//
//									 -1
//						|	P1		1|	|	P2		1|
//						|P1	+ v1	1|	|P2	+ v2	1|
//						|P1	+ N		1|	|P2	+ kN	1|
//						|P1 + V1xN	1|	|P2 + V2xN	1|
//
//
D3DXMATRIX * MatrixPairPointTransformation(D3DXMATRIX *pOut,
										   CONST D3DXVECTOR3 *pO1, CONST D3DXVECTOR3 *pV1,
										   CONST D3DXVECTOR3 *pO2, CONST D3DXVECTOR3 *pV2)
{	// Get shared CS normal
	D3DXVECTOR3 N, V1Normalized, V2Normalized;
	D3DXVec3Normalize(&V1Normalized, pV1);
	D3DXVec3Normalize(&V2Normalized, pV2);
	if (::abs(D3DXVec3Dot(&V1Normalized, &V2Normalized)) < 1.0f)
		D3DXVec3Cross(&N, pV1, pV2);
	else if (::abs(pV1->z) < D3DXVec3Length(pV1))
	{
		CONST D3DXVECTOR3 oZ(0.0f, 0.0f, 1.0f);
		D3DXVec3Cross(&N, pV1, &oZ);
	}
	else
	{
		CONST D3DXVECTOR3 oY(0.0f, 1.0f, 0.0f);
		D3DXVec3Cross(&N, pV1, &oY);
	}

	// Set first CS matrix
	D3DXVECTOR3 vO1V1, vV1xN, vO1N, vO1V1xN;
	D3DXVec3Add(&vO1V1,	pO1, pV1);
	D3DXVec3Add(&vO1N,	pO1, &N);
	D3DXVec3Cross(&vV1xN, pV1, &N);
	D3DXVec3Add(&vO1V1xN, pO1, &vV1xN);

	D3DXMATRIX matCS1(pO1->x,		pO1->y,		pO1->z,		1.0f,
					   vO1V1.x,		vO1V1.y,	vO1V1.z,	1.0f,
					   vO1N.x,		vO1N.y,		vO1N.z,		1.0f,
					   vO1V1xN.x,	vO1V1xN.y,	vO1V1xN.z,	1.0f);

	// Check inverse
	FLOAT MatCS1Det;
	D3DXMatrixInverse(&matCS1, &MatCS1Det, &matCS1);
	if (MatCS1Det == 0.0f)
		return NULL;

	// Set second CS matrix
	D3DXVECTOR3 vO2V2, vV2xN, vkN, vO2kN, vO2V2xN;
	D3DXVec3Add(&vO2V2,	pO2, pV2);
	D3DXVec3Scale(&vkN,	&N, D3DXVec3Length(pV2) / D3DXVec3Length(pV1));
	D3DXVec3Add(&vO2kN,	pO2, &vkN);
	D3DXVec3Cross(&vV2xN, pV2, &N);
	D3DXVec3Add(&vO2V2xN, pO2, &vV2xN);

	D3DXMATRIX matCS2(pO2->x,		pO2->y,		pO2->z,		1.0f,
					   vO2V2.x,		vO2V2.y,	vO2V2.z,	1.0f,
					   vO2kN.x,		vO2kN.y,	vO2kN.z,	1.0f,
					   vO2V2xN.x,	vO2V2xN.y,	vO2V2xN.z,	1.0f);

	return D3DXMatrixMultiply(pOut, &matCS1, &matCS2);
}


//
//	Name:	MatrixPairPointTransformation
//	Desc:	Calculation affine transformation matrix by three-pair point.
//			Return affine transformation object matrix from state { pO1, pV1, pW1 }
//			to state { pO2, pV2, pW2 }.
//	Arg:	pOut	-- [out] result affine transformation object matrix
//			pO1		-- [in]  input point  initial state
//			pV1		-- [in]  input vector initial state
//			pW1		-- [in]  input vector initial state
//			pO2		-- [in]  input point  end state
//			pV2		-- [in]  input vector end state
//			pW2		-- [in]  input vector end state
//
//	Remarks:
//
//
D3DXMATRIX * MatrixPairPointTransformation(D3DXMATRIX *pOut,
										   CONST D3DXVECTOR3 *pO1, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pW1,
										   CONST D3DXVECTOR3 *pO2, CONST D3DXVECTOR3 *pV2, CONST D3DXVECTOR3 *pW2)
{	// Set first CS matrix
	D3DXVECTOR3 vO1V1, vO1W1, vV1xW1, vO1V1xW1;
	D3DXVec3Add(&vO1V1,	 pO1, pV1);
	D3DXVec3Add(&vO1W1,	 pO1, pW1);
	D3DXVec3Cross(&vV1xW1, pV1, pW1);
	D3DXVec3Add(&vO1V1xW1, pO1, &vV1xW1);

	D3DXMATRIX	matCS1(pO1->x,		pO1->y,		pO1->z,		1.0f,
						vO1V1.x,	vO1V1.y,	vO1V1.z,	1.0f,
						vO1W1.x,	vO1W1.y,	vO1W1.z,	1.0f,
						vO1V1xW1.x,	vO1V1xW1.y,	vO1V1xW1.z,	1.0f);
	// Check first CS matrix inverse
	FLOAT MatCS1Det;
	D3DXMatrixInverse(&matCS1, &MatCS1Det, &matCS1);
	if (MatCS1Det == 0.0f)
		return NULL;

	// Set second CS matrix
	D3DXVECTOR3 vO2V2, vO2W2, vV2xW2, vO2V2xW2;
	D3DXVec3Add(&vO2V2,	 pO2, pV2);
	D3DXVec3Add(&vO2W2,	 pO2, pW2);
	D3DXVec3Cross(&vV2xW2, pV2, pW2);
	D3DXVec3Add(&vO2V2xW2, pO2, &vV2xW2);

	D3DXMATRIX	matCS2(pO2->x,		pO2->y,		pO2->z,		1.0f,
						vO2V2.x,	vO2V2.y,	vO2V2.z,	1.0f,
						vO2W2.x,	vO2W2.y,	vO2W2.z,	1.0f,
						vO2V2xW2.x,	vO2V2xW2.y,	vO2V2xW2.z,	1.0f);
	return D3DXMatrixMultiply(pOut, &matCS1, &matCS2);
}


//
//	Name:	MatrixPairPointTransformation
//	Desc:	Calculation affine transformation matrix by four-pair point.
//			Return affine transformation object matrix from state { pO1, pV1, pW1, pU1 }
//			to state { pO2, pV2, pW2, pU2 }.
//	Arg:	pOut	-- [out] result affine transformation object matrix
//			pO1		-- [in]  input point  initial state
//			pV1		-- [in]  input vector initial state
//			pW1		-- [in]  input vector initial state
//			pU1		-- [in]  input vector initial state
//			pO2		-- [in]  input point  end state
//			pV2		-- [in]  input vector end state
//			pW2		-- [in]  input vector end state
//			pU2		-- [in]  input vector end state
//
//	Remarks:
//
//
D3DXMATRIX * MatrixPairPointTransformation(D3DXMATRIX *pOut,
										   CONST D3DXVECTOR3 *pO1, CONST D3DXVECTOR3 *pV1,
										   CONST D3DXVECTOR3 *pW1, CONST D3DXVECTOR3 *pU1,
										   CONST D3DXVECTOR3 *pO2, CONST D3DXVECTOR3 *pV2,
										   CONST D3DXVECTOR3 *pW2, CONST D3DXVECTOR3 *pU2)
{	// Set first CS matrix
	D3DXVECTOR3 vO1V1, vO1W1, vO1U1;
	D3DXVec3Add(&vO1V1, pO1, pV1);
	D3DXVec3Add(&vO1W1, pO1, pW1);
	D3DXVec3Add(&vO1U1, pO1, pU1);
	D3DXMATRIX	matCS1(pO1->x,		pO1->y,		pO1->z,		1.0f,
						vO1V1.x,	vO1V1.y,	vO1V1.z,	1.0f,
						vO1W1.x,	vO1W1.y,	vO1W1.z,	1.0f,
						vO1U1.x,	vO1U1.y,	vO1U1.z,	1.0f);
	// Check first CS matrix inverse
	FLOAT MatCS1Det;
	D3DXMatrixInverse(&matCS1, &MatCS1Det, &matCS1);
	if (MatCS1Det == 0.0f)
		return NULL;

	// Set second CS matrix
	D3DXVECTOR3 vO2V2, vO2W2, vO2U2;
	D3DXVec3Add(&vO2V2, pO2, pV2);
	D3DXVec3Add(&vO2W2, pO2, pW2);
	D3DXVec3Add(&vO2U2, pO2, pU2);
	D3DXMATRIX	matCS2(pO2->x,		pO2->y,		pO2->z,		1.0f,
						vO2V2.x,	vO2V2.y,	vO2V2.z,	1.0f,
						vO2W2.x,	vO2W2.y,	vO2W2.z,	1.0f,
						vO2U2.x,	vO2U2.y,	vO2U2.z,	1.0f);
	return D3DXMatrixMultiply(pOut, &matCS1, &matCS2);
}



//
//	Name:	ClipProjectionLH
//	Desc:	Clip FOV of the projection matrix by custom clip-plane
//			(change third column of the projection matrix on scaled plane equation)
//	Arg:	pOut			-- [out] result clipping projection matrix
//			pmatProjection	-- [in]  input original projection matrix
//			pClipPlane		-- [in]  clipping plane equation in implicit form { A, B, C, D }
//
//	Remarks:
//		Use this instead setting user defined custom plane (e.g. d3d->SetClipPlane(...))
//
//
D3DXMATRIX * ClipProjectionLH(D3DXMATRIX *pOut, CONST D3DXMATRIX *pmatProjection, CONST D3DXVECTOR4 *pClipPlane)
{	// Calculate the clip-space corner point opposite the clipping plane
	// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
	// transform it into camera space by multiplying it
	// by the inverse of the projection matrix
	D3DXVECTOR4 ptClipCorner((SIGN(pClipPlane->x) + pmatProjection->_31) / pmatProjection->_11,
							  (SIGN(pClipPlane->y) + pmatProjection->_32) / pmatProjection->_22,
							  1.0F,
							  (1.0F + pmatProjection->_33) / pmatProjection->_43);

	// Calculate the scaled plane equation
	D3DXVECTOR4 plnScaled = *pClipPlane * D3DXVec4Dot(pClipPlane, &ptClipCorner);

	// Replace the third col of the projection matrix
	pOut->_13 = plnScaled.x;
	pOut->_23 = plnScaled.y;
	pOut->_33 = plnScaled.z;
	pOut->_43 = plnScaled.w;

	if (pOut != pmatProjection)
	{
		pOut->_11 = pmatProjection->_11;
		pOut->_12 = pmatProjection->_12;
		pOut->_14 = pmatProjection->_14;
		pOut->_21 = pmatProjection->_21;
		pOut->_22 = pmatProjection->_22;
		pOut->_24 = pmatProjection->_24;
		pOut->_31 = pmatProjection->_31;
		pOut->_32 = pmatProjection->_32;
		pOut->_34 = pmatProjection->_34;
		pOut->_41 = pmatProjection->_41;
		pOut->_42 = pmatProjection->_42;
		pOut->_44 = pmatProjection->_44;
	}

	return pOut;
}


void DrawQube(LPDIRECT3DDEVICE9 pd3dDevce, FLOAT Size, LPDIRECT3DTEXTURE9 pSideTextures[6])
{
	struct QUBEVERTEX
	{
		FLOAT	Position[3];
		FLOAT	Normal[3];
		FLOAT	TU, TV;
	};

	static const DWORD QUBEVERTEX_FVF =  D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0);

	const QUBEVERTEX cpSides[6][4] = {
		// FRONT SIDE
		{
			{ -Size, -Size, -Size,  +0.0f, +0.0f, -1.0f,  0.0f, 1.0f },
			{ -Size, +Size, -Size,  +0.0f, +0.0f, -1.0f,  0.0f, 0.0f },
			{ +Size, -Size, -Size,  +0.0f, +0.0f, -1.0f,  1.0f, 1.0f },
			{ +Size, +Size, -Size,  +0.0f, +0.0f, -1.0f,  1.0f, 0.0f }
		},
		// RIGHT SIDE
		{
			{ +Size, -Size, -Size,  +1.0f, +0.0f, +0.0f,  0.0f, 1.0f },
			{ +Size, +Size, -Size,  +1.0f, +0.0f, +0.0f,  0.0f, 0.0f },
			{ +Size, -Size, +Size,  +1.0f, +0.0f, +0.0f,  1.0f, 1.0f },
			{ +Size, +Size, +Size,  +1.0f, +0.0f, +0.0f,  1.0f, 0.0f }
		},
		// BACK SIDE
		{
			{ +Size, -Size, +Size,  +0.0f, +0.0f, +1.0f,  0.0f, 1.0f },
			{ +Size, +Size, +Size,  +0.0f, +0.0f, +1.0f,  0.0f, 0.0f },
			{ -Size, -Size, +Size,  +0.0f, +0.0f, +1.0f,  1.0f, 1.0f },
			{ -Size, +Size, +Size,  +0.0f, +0.0f, +1.0f,  1.0f, 0.0f }
		},
		// LEFT SIDE
		{
			{ -Size, -Size, +Size,  -1.0f, +0.0f, +0.0f,  0.0f, 1.0f },
			{ -Size, +Size, +Size,  -1.0f, +0.0f, +0.0f,  0.0f, 0.0f },
			{ -Size, -Size, -Size,  -1.0f, +0.0f, +0.0f,  1.0f, 1.0f },
			{ -Size, +Size, -Size,  -1.0f, +0.0f, +0.0f,  1.0f, 0.0f }
		},
		// TOP SIDE
		{
			{ -Size, +Size, -Size,  +0.0f, +1.0f, +0.0f,  0.0f, 1.0f },
			{ -Size, +Size, +Size,  +0.0f, +1.0f, +0.0f,  0.0f, 0.0f },
			{ +Size, +Size, -Size,  +0.0f, +1.0f, +0.0f,  1.0f, 1.0f },
			{ +Size, +Size, +Size,  +0.0f, +1.0f, +0.0f,  1.0f, 0.0f }
		},
		// BOTTOM SIDE
		{
			{ -Size, -Size, +Size,  +0.0f, -1.0f, +0.0f,  0.0f, 1.0f },
			{ -Size, -Size, -Size,  +0.0f, -1.0f, +0.0f,  0.0f, 0.0f },
			{ +Size, -Size, +Size,  +0.0f, -1.0f, +0.0f,  1.0f, 1.0f },
			{ +Size, -Size, -Size,  +0.0f, -1.0f, +0.0f,  1.0f, 0.0f }
		}
	};

	// Set qube FVF
	pd3dDevce->SetFVF(QUBEVERTEX_FVF);

	// Draw qube
	for (DWORD i = 0; i < 6; ++i)
	{
		pd3dDevce->SetTexture(0, pSideTextures[i]);
		pd3dDevce->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID) cpSides[i], sizeof(QUBEVERTEX));
	}
}



void DrawQube(LPDIRECT3DDEVICE9 pd3dDevce, FLOAT Size, D3DCOLOR Color)
{
	struct QUBEVERTEX
	{
		FLOAT		Position[3];
		FLOAT		Normal[3];
		D3DCOLOR	Diffuse;
	};

	static const DWORD QUBEVERTEX_FVF =  D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE;

	const QUBEVERTEX cpSides[6][4] = {
		// FRONT SIDE
		{
			{ -Size, -Size, -Size,  +0.0f, +0.0f, -1.0f,  Color },
			{ -Size, +Size, -Size,  +0.0f, +0.0f, -1.0f,  Color },
			{ +Size, -Size, -Size,  +0.0f, +0.0f, -1.0f,  Color },
			{ +Size, +Size, -Size,  +0.0f, +0.0f, -1.0f,  Color }
		},
		// RIGHT SIDE
		{
			{ +Size, -Size, -Size,  +1.0f, +0.0f, +0.0f,  Color },
			{ +Size, +Size, -Size,  +1.0f, +0.0f, +0.0f,  Color },
			{ +Size, -Size, +Size,  +1.0f, +0.0f, +0.0f,  Color },
			{ +Size, +Size, +Size,  +1.0f, +0.0f, +0.0f,  Color }
		},
		// BACK SIDE
		{
			{ +Size, -Size, +Size,  +0.0f, +0.0f, +1.0f,  Color },
			{ +Size, +Size, +Size,  +0.0f, +0.0f, +1.0f,  Color },
			{ -Size, -Size, +Size,  +0.0f, +0.0f, +1.0f,  Color },
			{ -Size, +Size, +Size,  +0.0f, +0.0f, +1.0f,  Color }
		},
		// LEFT SIDE
		{
			{ -Size, -Size, +Size,  -1.0f, +0.0f, +0.0f,  Color },
			{ -Size, +Size, +Size,  -1.0f, +0.0f, +0.0f,  Color },
			{ -Size, -Size, -Size,  -1.0f, +0.0f, +0.0f,  Color },
			{ -Size, +Size, -Size,  -1.0f, +0.0f, +0.0f,  Color }
		},
		// TOP SIDE
		{
			{ -Size, +Size, -Size,  +0.0f, +1.0f, +0.0f,  Color },
			{ -Size, +Size, +Size,  +0.0f, +1.0f, +0.0f,  Color },
			{ +Size, +Size, -Size,  +0.0f, +1.0f, +0.0f,  Color },
			{ +Size, +Size, +Size,  +0.0f, +1.0f, +0.0f,  Color }
		},
		// BOTTOM SIDE
		{
			{ -Size, -Size, +Size,  +0.0f, -1.0f, +0.0f,  Color },
			{ -Size, -Size, -Size,  +0.0f, -1.0f, +0.0f,  Color },
			{ +Size, -Size, +Size,  +0.0f, -1.0f, +0.0f,  Color },
			{ +Size, -Size, -Size,  +0.0f, -1.0f, +0.0f,  Color }
		}
	};

	// Set qube FVF
	pd3dDevce->SetFVF(QUBEVERTEX_FVF);
//	pd3dDevce->SetTexture(0, NULL);

	// Draw qube
	for (DWORD i = 0; i < 6; ++i)
		pd3dDevce->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID) cpSides[i], sizeof(QUBEVERTEX));
}




void DrawParallelepiped(LPDIRECT3DDEVICE9 pD3DDevice, CONST D3DXVECTOR3& vSize, D3DCOLOR dwColor)
{
	struct CUSTOMVERTEX
	{
		FLOAT		Position[3];
		FLOAT		Normal[3];
		D3DCOLOR	Diffuse;
	};

	const DWORD CUSTOMVERTEX_FVF =  D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE;

	const CUSTOMVERTEX cpSides[6][4] = {
		// FRONT SIDE
		{
			{ -vSize.x, -vSize.y, -vSize.z,  +0.0f, +0.0f, -1.0f,  dwColor },
			{ -vSize.x, +vSize.y, -vSize.z,  +0.0f, +0.0f, -1.0f,  dwColor },
			{ +vSize.x, -vSize.y, -vSize.z,  +0.0f, +0.0f, -1.0f,  dwColor },
			{ +vSize.x, +vSize.y, -vSize.z,  +0.0f, +0.0f, -1.0f,  dwColor }
		},
		// RIGHT SIDE
		{
			{ +vSize.x, -vSize.y, -vSize.z,  +1.0f, +0.0f, +0.0f,  dwColor },
			{ +vSize.x, +vSize.y, -vSize.z,  +1.0f, +0.0f, +0.0f,  dwColor },
			{ +vSize.x, -vSize.y, +vSize.z,  +1.0f, +0.0f, +0.0f,  dwColor },
			{ +vSize.x, +vSize.y, +vSize.z,  +1.0f, +0.0f, +0.0f,  dwColor }
		},
		// BACK SIDE
		{
			{ +vSize.x, -vSize.y, +vSize.z,  +0.0f, +0.0f, +1.0f,  dwColor },
			{ +vSize.x, +vSize.y, +vSize.z,  +0.0f, +0.0f, +1.0f,  dwColor },
			{ -vSize.x, -vSize.y, +vSize.z,  +0.0f, +0.0f, +1.0f,  dwColor },
			{ -vSize.x, +vSize.y, +vSize.z,  +0.0f, +0.0f, +1.0f,  dwColor }
		},
		// LEFT SIDE
		{
			{ -vSize.x, -vSize.y, +vSize.z,  -1.0f, +0.0f, +0.0f,  dwColor },
			{ -vSize.x, +vSize.y, +vSize.z,  -1.0f, +0.0f, +0.0f,  dwColor },
			{ -vSize.x, -vSize.y, -vSize.z,  -1.0f, +0.0f, +0.0f,  dwColor },
			{ -vSize.x, +vSize.y, -vSize.z,  -1.0f, +0.0f, +0.0f,  dwColor }
		},
		// TOP SIDE
		{
			{ -vSize.x, +vSize.y, -vSize.z,  +0.0f, +1.0f, +0.0f,  dwColor },
			{ -vSize.x, +vSize.y, +vSize.z,  +0.0f, +1.0f, +0.0f,  dwColor },
			{ +vSize.x, +vSize.y, -vSize.z,  +0.0f, +1.0f, +0.0f,  dwColor },
			{ +vSize.x, +vSize.y, +vSize.z,  +0.0f, +1.0f, +0.0f,  dwColor }
		},
		// BOTTOM SIDE
		{
			{ -vSize.x, -vSize.y, +vSize.z,  +0.0f, -1.0f, +0.0f,  dwColor },
			{ -vSize.x, -vSize.y, -vSize.z,  +0.0f, -1.0f, +0.0f,  dwColor },
			{ +vSize.x, -vSize.y, +vSize.z,  +0.0f, -1.0f, +0.0f,  dwColor },
			{ +vSize.x, -vSize.y, -vSize.z,  +0.0f, -1.0f, +0.0f,  dwColor }
		}
	};

	// Set qube FVF
	pD3DDevice->SetFVF(CUSTOMVERTEX_FVF);

	// Draw qube
	for (DWORD i = 0; i < 6; ++i)
		pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID) cpSides[i], sizeof(CUSTOMVERTEX));
}




HRESULT DrawCSAxis(LPDIRECT3DDEVICE9 pd3dDevce, FLOAT Length)
{
	struct AXISVERTEX
	{
		FLOAT		Position[3];
		D3DCOLOR	Diffuse;
	};

	static const DWORD AXISVERTEX_FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;

	const AXISVERTEX Axis[3][2] =
	{
		{
			{ 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0) },
			{ Length, 0.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0) }
		},

		{
			{ 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0) },
			{ 0.0f, Length, 0.0f, D3DCOLOR_XRGB(0, 255, 0) }
		},

		{
			{ 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255) },
			{ 0.0f, 0.0f, Length, D3DCOLOR_XRGB(0, 0, 255) }
		}
	};

	// Set qube FVF
	pd3dDevce->SetFVF(AXISVERTEX_FVF);

	// Draw axises
	return pd3dDevce->DrawPrimitiveUP(D3DPT_LINELIST, 3, (LPVOID) Axis, sizeof(AXISVERTEX));
}



//
//	Name:	CreateSingleStripGridIB
//	Desc:	Create triangle strip index buffer for addressing vertexes in mesh knoth
//	Arg:	pd3dDevce		-- [in] current rendering device
//			nXVerts			-- [in] width of mesh
//			nYVerts			-- [in] height of mesh
//			nXStep			-- [in] width step size
//			nYStep			-- [in] height step size
//			Stride			-- [in] 
//			Flags			-- [in] index buffer creation flags
//			mpIndexBufffer	-- [out] pointer to index buffer
//
//	Remarks:
//
HRESULT CreateSingleStripGridIB(LPDIRECT3DDEVICE9 pD3DDevice,
								WORD nXVerts, WORD nYVerts, WORD XStep, WORD YStep, WORD Stride,
								LPDIRECT3DINDEXBUFFER9 *ppIndexBuffer)
{
	WORD nStrips		  = nYVerts - 1;
	WORD nIndexesPerStrip = nXVerts * 2;
	WORD nIndexes		  = nStrips * nIndexesPerStrip + nStrips * 2 - 2;

	WORD *pIndexes		  = new WORD [nIndexes];
	if (NULL == pIndexes)
		return E_OUTOFMEMORY;

	WORD *pIndex = pIndexes;
	WORD StartVert = 0;
	WORD LineStep = Stride * YStep;

	for (WORD j = 0; j < nStrips; ++j)
	{
		WORD v = StartVert;

		// Create strip for this row
		for (WORD k = 0; k < nXVerts; ++k)
		{
			*pIndex++ = v;
			*pIndex++ = v + LineStep;
			v += XStep;
		}
		StartVert += LineStep;

		if (j+1 < nStrips)
		{
			// Add degenerate triangle to attach to next row
			*pIndex++ = (v - XStep) + LineStep;
			*pIndex++ = StartVert;
		}
	}

	// Create shared index buffer
	HRESULT hr;
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer = NULL;
	if (FAILED(hr = pD3DDevice ->
		CreateIndexBuffer(nIndexes * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIndexBuffer, NULL)))
		goto $error;

	// Fill indices
	LPVOID pIB;
	if (FAILED(hr = pIndexBuffer->Lock(0, 0, &pIB, 0)))
		goto $error;
	::memcpy(pIB, pIndexes, nIndexes * sizeof(WORD));
	pIndexBuffer->Unlock();

	*ppIndexBuffer = pIndexBuffer;

$error:
	if (FAILED(hr))
		SAFE_RELEASE(pIndexBuffer);
	delete [] pIndexes;
	return hr;
}




HRESULT D3DXMESHCONTAINEREX::Update ()
{	// Error checking
	if (!MeshData.pMesh || !pSkinMesh || !pSkinInfo || !pBoneMatrices || !ppFrameMatrices)
		return E_FAIL;
	// Copy the bone matrices over (must have been combined before call DrawMesh)
	for (DWORD i = 0; i < pSkinInfo->GetNumBones(); ++i)
	{	// start with bone offset matrix
		pBoneMatrices[i] = *pSkinInfo->GetBoneOffsetMatrix(i);
		// apply frame transformation
		if (ppFrameMatrices[i])
			pBoneMatrices[i] *= *ppFrameMatrices[i];
	}
	// lock the meshes' vertex buffers
	void *pSrc, *pDest;
	MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pSrc);
	pSkinMesh	  ->LockVertexBuffer(0, &pDest);
	// update the skinned mesh using provided transformations
	pSkinInfo	  ->UpdateSkinnedMesh(pBoneMatrices, NULL, pSrc, pDest);
	// unlock the meshes vertex buffers
	pSkinMesh	  ->UnlockVertexBuffer();
	MeshData.pMesh->UnlockVertexBuffer();
	return S_OK;
}

HRESULT D3DXMESHCONTAINEREX::Draw ()
{
	LPDIRECT3DDEVICE9 pD3DDevice;
	DWORD LastState, OldAlphaState, OldSrcBlend, OldDestBlend;

	// Error checking
	if (!MeshData.pMesh || !NumMaterials || !pMaterials)
		return E_FAIL;
	// Get the device interface
	MeshData.pMesh->GetDevice(&pD3DDevice);
	// Release vertex shader if being used
	pD3DDevice->SetVertexShader(NULL);
	pD3DDevice->SetVertexDeclaration(NULL);
	// Save render states
	pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &OldAlphaState);
	pD3DDevice->GetRenderState(D3DRS_SRCBLEND, &OldSrcBlend);
	pD3DDevice->GetRenderState(D3DRS_DESTBLEND, &OldDestBlend);
	LastState = OldAlphaState;
	// Setup pointer for mesh to draw, either regular or skinned
	ID3DXMesh *pDrawMesh = (!pSkinMesh) ? MeshData.pMesh : pSkinMesh;
	// Look through all subsets
	for (DWORD i = 0; i < NumMaterials; ++i)
	{	// Set material and texture
		pD3DDevice->SetMaterial(&pMaterials[i].MatD3D);
		pD3DDevice->SetTexture(0, ppTextures[i]);
		// Enable or disable alpha blending per material
		if (pMaterials[i].MatD3D.Diffuse.a != 1.0f)
		{
			if (LastState != TRUE)
			{
				LastState = TRUE;
				pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);			// SRCCOLOR);
				pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);
			}
		}
		else if (LastState != FALSE)
		{
			LastState = FALSE;
			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		// Draw the mesh subset
		pDrawMesh->DrawSubset(i);
	}
	// Restore alpha blending states
	if (LastState != OldAlphaState)
	{
		pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, OldAlphaState);
		pD3DDevice->SetRenderState(D3DRS_SRCBLEND, OldSrcBlend);
		pD3DDevice->SetRenderState(D3DRS_DESTBLEND, OldDestBlend);
	}
	// Make sure to release the device object!
	pD3DDevice->Release();
	return S_OK;
}

HRESULT D3DXMESHCONTAINEREX::DrawList ()
{
	HRESULT hr;

	// draw this
	if (FAILED(hr = Draw()))
		return hr;
	// draw next
	for (LPD3DXMESHCONTAINEREX pM = static_cast<LPD3DXMESHCONTAINEREX>(pNextMeshContainer);
		 pM;
		 pM = static_cast<LPD3DXMESHCONTAINEREX >(pM->pNextMeshContainer))
		if (FAILED(hr = pM->Draw()))
			return hr;
	return S_OK;
}

HRESULT D3DXMESHCONTAINEREX::Draw(LPDIRECT3DVERTEXSHADER9 pShader,
								  LPDIRECT3DVERTEXDECLARATION9 pDecl)
{	// Error checking
	if (!pShader || !pDecl)				return E_FAIL;
	if (!MeshData.pMesh)				return E_FAIL;
	if (!NumMaterials || !pMaterials)	return E_FAIL;
	// Get the device interface
	LPDIRECT3DDEVICE9 pD3DDevice;
	MeshData.pMesh->GetDevice(&pD3DDevice);
	// Save render states
	DWORD LastState, OldAlphaState, OldSrcBlend, OldDestBlend;
	pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &OldAlphaState);
	pD3DDevice->GetRenderState(D3DRS_SRCBLEND, &OldSrcBlend);
	pD3DDevice->GetRenderState(D3DRS_DESTBLEND, &OldDestBlend);
	LastState = OldAlphaState;
	// Get mesh buffer pointers
	IDirect3DVertexBuffer9 *pVB = NULL;
	IDirect3DIndexBuffer9  *pIB = NULL;
	MeshData.pMesh->GetVertexBuffer(&pVB);
	MeshData.pMesh->GetIndexBuffer(&pIB);
	// Get attribute table
	DWORD NumAttributes;
	MeshData.pMesh->GetAttributeTable(NULL, &NumAttributes);
	D3DXATTRIBUTERANGE *pAttributes = new D3DXATTRIBUTERANGE[NumAttributes];
	MeshData.pMesh->GetAttributeTable(pAttributes, &NumAttributes);
	// Use the vertex shader interface passed
	pD3DDevice->SetFVF(0);
	pD3DDevice->SetVertexShader(pShader);
	pD3DDevice->SetVertexDeclaration(pDecl);
	// Set stream sources
	pD3DDevice->SetStreamSource(0, pVB, 0, D3DXGetFVFVertexSize(MeshData.pMesh->GetFVF()));
	pD3DDevice->SetIndices(pIB);

	// Go through each attribute group and render
	for (DWORD i = 0; i < NumAttributes; i++)
	{
		if (pAttributes[i].FaceCount)
		{	// Set texture
			pD3DDevice->SetTexture(0, ppTextures[ pAttributes[i].AttribId ]);
			// Enable or disable alpha blending per material
			if (pMaterials[i].MatD3D.Diffuse.a != 1.0F)
			{
				if (LastState != TRUE)
				{	// Enable alpha blending
					LastState = TRUE;
					pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);//SRCCOLOR);
					pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);
				}
			}
			else if (LastState != FALSE)
			{	// Disable alpha blending
				LastState = FALSE;
				pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}
			// Draw the mesh subset
			pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
											 pAttributes[i].VertexStart,
											 pAttributes[i].VertexCount,
											 pAttributes[i].FaceStart * 3,
											 pAttributes[i].FaceCount);
		}
	}

	// Clear stream uses
	pD3DDevice->SetStreamSource(0, NULL, 0, 0);
	pD3DDevice->SetIndices(NULL);
	// Free resources
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
	delete [] pAttributes;
	// Restore alpha blending states
	if (LastState != OldAlphaState)
	{
		pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, OldAlphaState);
		pD3DDevice->SetRenderState(D3DRS_SRCBLEND, OldSrcBlend);
		pD3DDevice->SetRenderState(D3DRS_DESTBLEND, OldDestBlend);
	}
	// Make sure to release the device object!
	pD3DDevice->Release();
	// Release vertex shader and declaration mapping
	pD3DDevice->SetVertexShader(NULL);
	pD3DDevice->SetVertexDeclaration(NULL);
	return S_OK;
}

HRESULT D3DXMESHCONTAINEREX::DrawList(LPDIRECT3DVERTEXSHADER9 pShader,
									  LPDIRECT3DVERTEXDECLARATION9 pDecl)
{
	HRESULT hr;

	// draw this
	if (FAILED(hr = Draw(pShader, pDecl)))
		return hr;
	// draw next
	for (LPD3DXMESHCONTAINEREX  pM = static_cast<LPD3DXMESHCONTAINEREX >(pNextMeshContainer);
		 pM;
		 pM = static_cast<LPD3DXMESHCONTAINEREX >(pM->pNextMeshContainer))
		if (FAILED(hr = pM->Draw(pShader, pDecl)))
			return hr;
	return S_OK;
}




/*
 *	D3D initialization
 */

HRESULT InitializeDirect3D(LPDIRECT3D9 *ppD3D, LPDIRECT3DDEVICE9 *ppD3DDevice,
						   HWND hWnd, UINT ScreenWidth, UINT ScreenHeight,
						   BOOL bForceWindowed, BOOL bPureDevice, BOOL bMultiThreaded)
{
	HRESULT           hr;
	IDirect3D9       *pD3D       = NULL;
	LPDIRECT3DDEVICE9 pD3DDevice = NULL;

	// Error checking
	if (!ppD3D || !ppD3DDevice || !hWnd)
		return E_FAIL;
	// Initialize Direct3D interface
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;
	// Store the Direct3D interface
	*ppD3D = pD3D;
	// Set the video (depending on windowed mode or fullscreen)
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	// Ask if user wants to run windowed or fullscreen or force windowed if flagged to do such
	int Mode = bForceWindowed ?
		IDNO :
		::MessageBox(hWnd, "Use fullscreen mode?", "Initialize D3D", MB_YESNO|MB_ICONQUESTION);
	// Setup video settings based on choice of fullscreen or not
	if (Mode == IDYES)
	{	// SETUP FULLSCREEN FORMAT (set to your own if you prefer)
		d3dpp.BackBufferWidth			= ScreenWidth;
		d3dpp.BackBufferHeight			= ScreenHeight;
		d3dpp.BackBufferFormat			= D3DFMT_A8R8G8B8;		// 640x480x16 bit bpp
		d3dpp.SwapEffect				= D3DSWAPEFFECT_FLIP;
		d3dpp.Windowed					= FALSE;
		d3dpp.EnableAutoDepthStencil	= TRUE;
		d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else
	{	// SETUP WINDOWED FORMAT (set to your own dimensions below)
		// Get the client and window dimensions
		RECT ClientRect, WndRect;
		GetClientRect(hWnd, &ClientRect);
		GetWindowRect(hWnd, &WndRect);
		// Set the width and height (set your dimensions here)
		const DWORD DesiredWidth  = ScreenWidth;
		const DWORD DesiredHeight = ScreenHeight;
		const DWORD Width  = (WndRect.right - WndRect.left) + (DesiredWidth  - ClientRect.right);
		const DWORD Height = (WndRect.bottom - WndRect.top) + (DesiredHeight - ClientRect.bottom);
		// Set the window's dimensions
		MoveWindow(hWnd, WndRect.left, WndRect.top, Width, Height, TRUE);
		// Set the presentation parameters (use windowed)
		d3dpp.BackBufferWidth			= DesiredWidth;
		d3dpp.BackBufferHeight			= DesiredHeight;
		d3dpp.BackBufferFormat			= D3DFMT_A8R8G8B8;
		d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
		d3dpp.Windowed					= TRUE;
		d3dpp.EnableAutoDepthStencil	= TRUE;
		d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	// Get Direct3D device caps
	D3DCAPS9 d3ddc;
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3ddc);
	// Check for hardware T&L
	DWORD dwBehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	if (d3ddc.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		// May set hardware vertex processing!
		dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		// Check for pure device?
		if (bPureDevice && d3ddc.DevCaps & D3DDEVCAPS_PUREDEVICE)
			dwBehaviorFlags |= D3DCREATE_PUREDEVICE;
	}
	if (bMultiThreaded)
		dwBehaviorFlags |= D3DCREATE_MULTITHREADED;
	// Create Direct3D device
	if (FAILED(hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwBehaviorFlags, &d3dpp, &pD3DDevice)))
		return hr;
	// Store the 3-D device object pointer
	*ppD3DDevice = pD3DDevice;
	// Set the perspective projection
	FLOAT Aspect = (FLOAT)d3dpp.BackBufferWidth / (FLOAT)d3dpp.BackBufferHeight;
	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI/4, Aspect, 1.0f, 10000.0f);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection);
	// Set the default render states
	pD3DDevice->SetRenderState(D3DRS_LIGHTING,         FALSE);
	pD3DDevice->SetRenderState(D3DRS_ZENABLE,          D3DZB_TRUE);
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,  FALSE);
	// Set the default texture stage states
	pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	// Set the default texture filters
	pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	return S_OK;
}




/*
 *	D3D load vertex shader
 */

HRESULT LoadVertexShader(LPDIRECT3DVERTEXSHADER9 *ppShader, 
                         LPDIRECT3DDEVICE9 pDevice,
                         LPCTSTR pFileName,
                         D3DVERTEXELEMENT9 *pElements,
                         LPDIRECT3DVERTEXDECLARATION9 *ppDecl)
{	// Assemble the shader
	HRESULT hr;
	ID3DXBuffer *pCode;
	if (FAILED(hr = D3DXAssembleShaderFromFile(pFileName, NULL, NULL, 0, &pCode, NULL)))
		return hr;
	// Create vertex shader
	if (FAILED(hr = pDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), ppShader)))
	{
		pCode->Release();
		return hr;
	}
	pCode->Release();
	// Create the declaration interface if needed
	if (pElements && ppDecl)
		pDevice->CreateVertexDeclaration(pElements, ppDecl);
	return S_OK;
}




/*
 *	D3D Font
 */

HRESULT CreateMyFont(
	LPDIRECT3DDEVICE9 pD3DDevice,	// D3D device
	INT nHeight,					// height of font
	INT nWidth,						// average character width
	LPD3DXFONT *ppD3DFont)			// [out] D3D font interface
{
	HFONT hFont = ::CreateFont(nHeight, nWidth, 0, 0, FW_NORMAL, FALSE, FALSE,
		0, 1, 0, 0, 0, DEFAULT_PITCH|FF_MODERN, "Arial");
	return D3DXCreateFont(pD3DDevice, hFont, ppD3DFont);
}

void DrawMyFont (
	LPD3DXFONT pD3DFont,			// D3D font interface
	LPCSTR pString,					// Pointer to a string to draw
	LONG Left, LONG Top,			// Rectangle, in logical coordinates, 
	LONG Right, LONG Bottom,		// in which the text is to be formatted
	D3DCOLOR Color)					// Color of the text
{	// Init text rect
	RECT rcText;
	rcText.left   = Left;
	rcText.top	  = Top;
	rcText.right  = Right;
	rcText.bottom = Bottom;

	// Print text
	pD3DFont->DrawText(pString, -1, &rcText, DT_WORDBREAK, Color);
}




/*
 *	Drawing morphed mesh from FRP / VS
 */

void DrawMorphMesh (
	LPD3DXMESHCONTAINEREX pSourceMesh,
    LPD3DXMESHCONTAINEREX pTargetMesh,
    LPD3DXMESHCONTAINEREX pResultMesh,
    FLOAT fMorphScalar,
	long NormalOffset,
	DWORD FVFSize)
{	
	// Lock mesh vertex buffers
	LPBYTE pSourceVB, pTargetVB, pResultVB;
	pSourceMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pSourceVB);
	pTargetMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pTargetVB);
	pResultMesh->MeshData.pMesh->LockVertexBuffer(0, (void**)&pResultVB);
	// Go through each vertex and interpolate coordinates
	for (DWORD i = 0; i < pSourceMesh->MeshData.pMesh->GetNumVertices(); ++i)
	{	// Get pointers to vertex data
		D3DXVECTOR3 *pSourceVert = (D3DXVECTOR3 *)pSourceVB;
		D3DXVECTOR3 *pTargetVert = (D3DXVECTOR3 *)pTargetVB;
		D3DXVECTOR3 *pResultVert = (D3DXVECTOR3 *)pResultVB;
		// Store morphed position (lerp)
		D3DXVec3Lerp(pResultVert, pSourceVert, pTargetVert, fMorphScalar);
//		*pResultVert = *pSourceVert * (1.0F - fScalar) + *pTargetVert * fScalar;	// may use lerp ?
		// Handle interpolation of normals
		if (NormalOffset)
		{	// Offset on FVF normal size
			pSourceVert = (D3DXVECTOR3 *)&pSourceVB[NormalOffset];
			pTargetVert = (D3DXVECTOR3 *)&pTargetVB[NormalOffset];
			pResultVert = (D3DXVECTOR3 *)&pResultVB[NormalOffset];
			// Store morphed normal (lerp)
			D3DXVec3Lerp(pResultVert, pSourceVert, pTargetVert, fMorphScalar);
//			*pResultVert = *pSourceVert * (1.0F - fScalar) + *pTargetVert * fScalar;	// may use lerp ?
		}
		// Go to next vertex
		pSourceVB += FVFSize;
		pTargetVB += FVFSize;
		pResultVB += FVFSize;
	}
	// Unlock meshes buffers
	pSourceMesh->MeshData.pMesh->UnlockVertexBuffer();
	pTargetMesh->MeshData.pMesh->UnlockVertexBuffer();
	pResultMesh->MeshData.pMesh->UnlockVertexBuffer();
	// Draw result mesh
	pResultMesh->Draw();
}

void DrawMorphMesh (
	LPDIRECT3DVERTEXSHADER9		  pVShader,
	LPDIRECT3DVERTEXDECLARATION9  pVDecl,
	D3DXMESHCONTAINEREX			* pSourceMesh,
    D3DXMESHCONTAINEREX			* pTargetMesh,
    FLOAT fMorphScalar,
	CONST D3DXVECTOR3 *			  pLightDir)
{	
	// Get D3D device
	LPDIRECT3DDEVICE9 pD3DDevice;
	if (SUCCEEDED(pSourceMesh->MeshData.pMesh->GetDevice(&pD3DDevice)))
	{	// Get the world, view, and projection matrices
		D3DXMATRIX matWorld, matView, matProj;
		pD3DDevice->GetTransform(D3DTS_WORLD,	   &matWorld);
		pD3DDevice->GetTransform(D3DTS_VIEW,	   &matView);
		pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);
		// Get the world * view * proj matrix and set it
		D3DXMATRIX matWVP = matWorld * matView * matProj;
		D3DXMatrixTranspose(&matWVP, &matWVP);
		pD3DDevice->SetVertexShaderConstantF(0, (CONST float*)&matWVP, 4);
		// Set the scalar value to use
		D3DXVECTOR4 VSScalar(1.0F - fMorphScalar, fMorphScalar, 0.0F, 0.0F);
		pD3DDevice->SetVertexShaderConstantF(4, (CONST float*)&VSScalar, 1);
		// Set the light direction
		pD3DDevice->SetVertexShaderConstantF(5, (CONST float*)pLightDir, 1);
		// Set the 2nd stream source
		LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
		pTargetMesh->MeshData.pMesh->GetVertexBuffer(&pVB);
		pD3DDevice->SetStreamSource(1, pVB, 0, D3DXGetFVFVertexSize(pTargetMesh->MeshData.pMesh->GetFVF()));
		// Draw the mesh in the vertex shader
		pSourceMesh->Draw(pVShader, pVDecl);
		// Clear the 2nd stream source and free the vertex buffer interface
		pD3DDevice->SetStreamSource(1, NULL, 0, 0);
		SAFE_RELEASE(pVB);
		// Free D3D device
		pD3DDevice->Release();
	}
}



//
//	DRAW BLENDED MORPH MESH
//

void DrawMorphBlendedMesh (
	LPD3DXMESHCONTAINEREX pBaseMesh,
    LPD3DXMESHCONTAINEREX pMesh1, FLOAT fBlend1,
    LPD3DXMESHCONTAINEREX pMesh2, FLOAT fBlend2,
    LPD3DXMESHCONTAINEREX pMesh3, FLOAT fBlend3,
    LPD3DXMESHCONTAINEREX pMesh4, FLOAT fBlend4,
	long NormalOffset,
	DWORD FVFSize,
	LPD3DXMESHCONTAINEREX pBlendedMesh)
{	// Lock all meshes and get pointers
	unsigned char *pBaseVB, *pMesh1VB, *pMesh2VB,
				  *pMesh3VB, *pMesh4VB, *pBlendedVB;

	pBaseMesh	->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pBaseVB);
	pMesh1		->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pMesh1VB);
	pMesh2		->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pMesh2VB);
	pMesh3		->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pMesh3VB);
	pMesh4		->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pMesh4VB);
	pBlendedMesh->MeshData.pMesh->LockVertexBuffer(0, (void**)&pBlendedVB);
	// Iterate all vertices
	for (DWORD i = 0; i < pBaseMesh->MeshData.pMesh->GetNumVertices(); ++i)
	{	// Get blended position
		D3DXVECTOR3 *pPosBase	 = (D3DXVECTOR3 *) pBaseVB;
		D3DXVECTOR3 *pPos1		 = (D3DXVECTOR3 *) pMesh1VB;
		D3DXVECTOR3 *pPos2		 = (D3DXVECTOR3 *) pMesh2VB;
		D3DXVECTOR3 *pPos3		 = (D3DXVECTOR3 *) pMesh3VB;
		D3DXVECTOR3 *pPos4		 = (D3DXVECTOR3 *) pMesh4VB;
		D3DXVECTOR3 *pPosBlended = (D3DXVECTOR3 *) pBlendedVB;
		D3DXVECTOR3  vBlendedPos =	(*pPos1 - *pPosBase) * fBlend1 + 
									(*pPos2 - *pPosBase) * fBlend2 + 
									(*pPos3 - *pPosBase) * fBlend3 + 
									(*pPos4 - *pPosBase) * fBlend4;
		*pPosBlended = *pPosBase + vBlendedPos;
		// Get blended normal
		D3DXVECTOR3 *pNormBase	= (D3DXVECTOR3 *) &pBaseVB[NormalOffset];
		D3DXVECTOR3 *pNorm1	= (D3DXVECTOR3 *) &pMesh1VB[NormalOffset];
		D3DXVECTOR3 *pNorm2	= (D3DXVECTOR3 *) &pMesh2VB[NormalOffset];
		D3DXVECTOR3 *pNorm3	= (D3DXVECTOR3 *) &pMesh3VB[NormalOffset];
		D3DXVECTOR3 *pNorm4	= (D3DXVECTOR3 *) &pMesh4VB[NormalOffset];
		D3DXVECTOR3 *pNormBlended = (D3DXVECTOR3 *) &pMesh4VB[NormalOffset];
		D3DXVECTOR3  vBlendedNorm =	(*pNorm1 - *pNormBase) * fBlend1 + 
									(*pNorm2 - *pNormBase) * fBlend2 + 
									(*pNorm3 - *pNormBase) * fBlend3 + 
									(*pNorm4 - *pNormBase) * fBlend4;
		*pNormBlended = *pNormBase + vBlendedNorm;
		D3DXVec3Normalize(pNormBlended, pNormBlended);
		// Go to next vertices
		pBaseVB    += FVFSize;
		pMesh1VB   += FVFSize;
		pMesh2VB   += FVFSize;
		pMesh3VB   += FVFSize;
		pMesh4VB   += FVFSize;
		pBlendedVB += FVFSize;
	}
	// Unlock the vertex buffers
	pBaseMesh	->MeshData.pMesh->UnlockVertexBuffer();
	pMesh1		->MeshData.pMesh->UnlockVertexBuffer();
	pMesh2		->MeshData.pMesh->UnlockVertexBuffer();
	pMesh3		->MeshData.pMesh->UnlockVertexBuffer();
	pMesh4		->MeshData.pMesh->UnlockVertexBuffer();
	pBlendedMesh->MeshData.pMesh->UnlockVertexBuffer();
	// Get D3D device
	LPDIRECT3DDEVICE9 pD3DDevice;
	if (SUCCEEDED(pBaseMesh->MeshData.pMesh->GetDevice(&pD3DDevice)))
	{	// Enable lighting and zbuffering
		pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		// Render the blended mesh
		pBlendedMesh->Draw();
		// Disable lighting and zbuffering
		pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}
}



//
//	DRAW BLENDED MORPH MESH WITH VERTEX SHADER
//

void DrawMorphBlendedMesh (
	LPDIRECT3DVERTEXSHADER9 pMorphShader,
	LPDIRECT3DVERTEXDECLARATION9 pMorphDecl,
	LPD3DXMESHCONTAINEREX pBaseMesh,
    LPD3DXMESHCONTAINEREX pMesh1, FLOAT fBlend1,
    LPD3DXMESHCONTAINEREX pMesh2, FLOAT fBlend2,
    LPD3DXMESHCONTAINEREX pMesh3, FLOAT fBlend3,
    LPD3DXMESHCONTAINEREX pMesh4, FLOAT fBlend4,
	CONST D3DXVECTOR3 *pLightDir)
{	// Get D3D device
	LPDIRECT3DDEVICE9 pD3DDevice;
	if (SUCCEEDED(pBaseMesh->MeshData.pMesh->GetDevice(&pD3DDevice)))
	{	// Get the world, view, and projection matrices
		D3DXMATRIX matWorld, matView, matProj;
		pD3DDevice->GetTransform(D3DTS_WORLD, &matWorld);
		pD3DDevice->GetTransform(D3DTS_VIEW, &matView);
		pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);
		// Get the world*view*proj matrix and set it
		D3DXMATRIX matWVP = matWorld * matView * matProj;
		D3DXMatrixTranspose(&matWVP, &matWVP);
		pD3DDevice->SetVertexShaderConstantF(0, (CONST float*)&matWVP, 4);
		// Set the scalar blending values to use
		D3DXVECTOR4 VSLight(fBlend1, fBlend2, fBlend3, fBlend4);
		pD3DDevice->SetVertexShaderConstantF(4, (CONST float*)&VSLight, 1);
		// Set the light direction
		pD3DDevice->SetVertexShaderConstantF(5, (CONST float*)pLightDir, 1);
		// Get the meshes' vertex buffers
		LPDIRECT3DVERTEXBUFFER9 pVB1 = NULL;
		LPDIRECT3DVERTEXBUFFER9 pVB2 = NULL;
		LPDIRECT3DVERTEXBUFFER9 pVB3 = NULL;
		LPDIRECT3DVERTEXBUFFER9 pVB4 = NULL;
		pMesh1->MeshData.pMesh->GetVertexBuffer(&pVB1);
		pMesh2->MeshData.pMesh->GetVertexBuffer(&pVB2);
		pMesh3->MeshData.pMesh->GetVertexBuffer(&pVB3);
		pMesh4->MeshData.pMesh->GetVertexBuffer(&pVB4);
		// Set the stream sources
		pD3DDevice->SetStreamSource(1, pVB1, 0, D3DXGetFVFVertexSize(pMesh1->MeshData.pMesh->GetFVF()));
		pD3DDevice->SetStreamSource(2, pVB2, 0, D3DXGetFVFVertexSize(pMesh2->MeshData.pMesh->GetFVF()));
		pD3DDevice->SetStreamSource(3, pVB3, 0, D3DXGetFVFVertexSize(pMesh3->MeshData.pMesh->GetFVF()));
		pD3DDevice->SetStreamSource(4, pVB4, 0, D3DXGetFVFVertexSize(pMesh4->MeshData.pMesh->GetFVF()));
		// Enable zbuffering
		pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		// Draw the mesh in the vertex shader
		pBaseMesh ->Draw(pMorphShader, pMorphDecl);
		// Disable zbuffering
		pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		// Clear the stream sources
		pD3DDevice->SetStreamSource(1, NULL, 0, 0);
		pD3DDevice->SetStreamSource(2, NULL, 0, 0);
		pD3DDevice->SetStreamSource(3, NULL, 0, 0);
		pD3DDevice->SetStreamSource(4, NULL, 0, 0);
		// Free the vertex buffer interface
		SAFE_RELEASE(pVB1);
		SAFE_RELEASE(pVB2);
		SAFE_RELEASE(pVB3);
		SAFE_RELEASE(pVB4);
		// Free D3D device
		pD3DDevice->Release();
	}
}


static inline BYTE GetSampleColor(float fU, float fV,
								  const D3DSURFACE_DESC *pSD, const D3DLOCKED_RECT *pLR,
								  unsigned int nBPP)
{
	// Get REAL pixel location
	float fX = fU * pSD->Width;
	float fY = fV * pSD->Height;
	// Chop to INTEGER
	long  nX0 = Real::Chop(fX);
	long  nY0 = Real::Chop(fY);
	// Get billinear interpolators
	float fXScale = fX - nX0;
	float fYScale = fY - nY0;
	// Read 4 samples
	nX0 %= pSD->Width;
	nY0 %= pSD->Height;
	long nX1 = (nX0 + 1) % pSD->Width;
	long nY1 = (nY0 + 1) % pSD->Height;
	BYTE *pBits = (BYTE*)pLR->pBits;
	float a = pBits[nY0*pLR->Pitch + nX0*nBPP];
	float b = pBits[nY0*pLR->Pitch + nX1*nBPP];
	float c = pBits[nY1*pLR->Pitch + nX0*nBPP];
	float d = pBits[nY1*pLR->Pitch + nX1*nBPP];
	// Billinear interpolation
	a = a + fXScale * (b - a);
	b = c + fXScale * (d - c);
	a = a + fYScale * (b - a);
	return (BYTE)::clamp(Real::Chop(a), 0L, 255L);
}

HRESULT RedMaskTexture(LPDIRECT3DTEXTURE9 pTexture, LPDIRECT3DTEXTURE9 pMask)
{
	HRESULT hr;

	// Check input arguments
	if (NULL == pTexture || NULL == pMask)
		return E_INVALIDARG;

	// Get base texture description
	D3DSURFACE_DESC BaseSD;
	if (FAILED(hr = pTexture->GetLevelDesc(0, &BaseSD)))
		return hr;

	// Get mask texture description
	D3DSURFACE_DESC MaskSD;
	if (FAILED(hr = pMask->GetLevelDesc(0, &MaskSD)))
		return hr;

	// Check right size
//	if (BaseSD.Width != MaskSD.Width || BaseSD.Height != MaskSD.Height)
//		return E_FAIL;

	// Get color byte offsets
	DWORD dwBaseByteOffset;
	if (BaseSD.Format == D3DFMT_A8R8G8B8 || BaseSD.Format == D3DFMT_X8R8G8B8)
		dwBaseByteOffset = 4;
	else if (BaseSD.Format == D3DFMT_R8G8B8)
		dwBaseByteOffset = 3;
	else if (BaseSD.Format == D3DFMT_A8 || BaseSD.Format == D3DFMT_P8 || BaseSD.Format == D3DFMT_L8)
		dwBaseByteOffset = 1;
	else
		return E_FAIL;	// unsupported image format
	
	DWORD dwMaskByteOffset;
	if (MaskSD.Format == D3DFMT_A8R8G8B8 || MaskSD.Format == D3DFMT_X8R8G8B8)
		dwMaskByteOffset = 4;
	else if (MaskSD.Format == D3DFMT_R8G8B8)
		dwMaskByteOffset = 3;
	else if (MaskSD.Format == D3DFMT_A8 || MaskSD.Format == D3DFMT_P8 || MaskSD.Format == D3DFMT_L8)
		dwMaskByteOffset = 1;
	else
		return E_FAIL;	// unsupported image format

	// Lock base texture
	D3DLOCKED_RECT BaseLR;
	if (FAILED(hr = pTexture->LockRect(0, &BaseLR, NULL, D3DLOCK_DISCARD)))
		goto $error;

	// Lock mask texture
	D3DLOCKED_RECT MaskLR;
	if (FAILED(hr = pMask->LockRect(0, &MaskLR, NULL, D3DLOCK_READONLY)))
		goto $error;

	// Process masking pixels
	BYTE *pBaseBits = (BYTE *)BaseLR.pBits;
	for (UINT h = 0; h < BaseSD.Height; ++h)
	{
		for (UINT w = 0; w < BaseSD.Width; ++w)
		{
			UINT nBaseOffset = h * BaseLR.Pitch + w * dwBaseByteOffset;
			BYTE btMaskColor = ::GetSampleColor((float)w/(BaseSD.Width-1), (float)h/(BaseSD.Height-1),
												&MaskSD, &MaskLR, dwMaskByteOffset);
			for (UINT i = 0; i < dwBaseByteOffset; ++i)
			{
				BYTE btBaseColor = pBaseBits[nBaseOffset+i];
				pBaseBits[nBaseOffset+i] = min(btBaseColor, btMaskColor);
			}
		}
	}

$error:
	pTexture->UnlockRect(0);
	pMask->UnlockRect(0);
	return hr;
}



HRESULT BuildNormalHeightMapFromFile(LPDIRECT3DDEVICE9 g_pD3DDevice, LPCTSTR InFileName, LPCTSTR OutFileName)
{
	HRESULT hr;
	LPDIRECT3DTEXTURE9 pHeightMap = NULL;
	LPDIRECT3DTEXTURE9 pNormalMap = NULL;


	//		O P E N   H E I G H T   M A P		//

	if (FAILED(hr = D3DXCreateTextureFromFile(g_pD3DDevice, InFileName, &pHeightMap)))
		goto error;

	// Get height map texture description
	D3DSURFACE_DESC ddsd;
	if (FAILED(hr = pHeightMap->GetLevelDesc(0, &ddsd)))
		goto error;

	// Get color data offset
	DWORD dwByteOffset;
	if (ddsd.Format == D3DFMT_A8R8G8B8 || ddsd.Format == D3DFMT_X8R8G8B8)
		dwByteOffset = 4;
	else if (ddsd.Format == D3DFMT_R8G8B8)
		dwByteOffset = 3;
	else if (ddsd.Format == D3DFMT_A8 || ddsd.Format == D3DFMT_P8 || ddsd.Format == D3DFMT_L8)
		dwByteOffset = 1;
	else
		goto error;

	// Get height-map image width/height
	UINT iTableWidth  = ddsd.Width;
	UINT iTableHeight = ddsd.Height;


	//		C R E A T E   N O R M A L   M A P   F R O M   H E I G H T   M A P		//

	// Create texture for normal map
	if (FAILED(hr = D3DXCreateTexture(g_pD3DDevice, iTableWidth, iTableHeight, 0, 0,
										D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pNormalMap)))
		goto error;

	// Compute normal map
	if (FAILED(hr = D3DXComputeNormalMap(pNormalMap, pHeightMap, NULL, 0, D3DX_CHANNEL_RED, (FLOAT)iTableWidth)))
		goto error;


	//		A D D   H E I G H T   I N F O   A S   A L P H A   T O   N O R M A L   M A P		//

	// Lock height texture
	D3DLOCKED_RECT HeightRect;
	if (FAILED(hr = pHeightMap->LockRect(0, &HeightRect, NULL, D3DLOCK_READONLY)))
		goto error;

	// Lock normal texture
	D3DLOCKED_RECT NormalRect;
	if (FAILED(hr = pNormalMap->LockRect(0, &NormalRect, NULL, 0)))
		goto error;

	BYTE *pHeightMapBits = (BYTE*)HeightRect.pBits;
	BYTE *pNormalMapBits = (BYTE*)NormalRect.pBits;
	for (UINT h = 0; h < iTableHeight; ++h)
	{
		for (UINT w = 0; w < iTableWidth; ++w)
		{
			BYTE iHeight = pHeightMapBits[h * HeightRect.Pitch + w * dwByteOffset];
			pNormalMapBits[h * NormalRect.Pitch + w * 4 + 3] = iHeight;
		}
	}

	// Unlock height map
	pNormalMap->UnlockRect(0);

	// Unlock height map
	pHeightMap->UnlockRect(0);


	//		S A V E   N O R M A L - D E P T H   M A P		//

	if (FAILED(hr = D3DXSaveTextureToFile(OutFileName, D3DXIFF_DDS, pNormalMap, NULL)))
		goto error;

error:
	SAFE_RELEASE(pHeightMap);
	SAFE_RELEASE(pNormalMap);
	return hr;
}

