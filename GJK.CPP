#include "StdAfx.h"
#include "GJK.h"

// Even this algorithm has an epsilon (fudge) factor.  It basically indicates
// how far apart two points have to be to declared different, expressed 
// loosely as a proportion of the `average distance' between the point sets.
#define GJK_TOLERANCE			1.0e-6f

// GJK_TINY is used in one place, to indicate when a positive number is getting
// so small that we loose confidence in being able to divide a positive
// number smaller than it into it, and still believing the result.
#define GJK_TINY				1.0e-20f



CONST INT Simplex::s_iCardinality[TWICE_TWO_TO_DIM] = {
	 0,  1,  1,  2,  1,  2,  2,  3,  1,  2,  2,  3,  2,  3,  3,  4
};

CONST INT Simplex::s_iMaxElement[TWICE_TWO_TO_DIM] = {
	-1,  0,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3
};

CONST INT Simplex::s_iElements[TWICE_TWO_TO_DIM][DIM_PLUS_ONE] = {
	{  0,  0,  0,  0 },
	{  0,  0,  0,  0 },
	{  1,  0,  0,  0 },
	{  0,  1,  0,  0 },
	{  2,  0,  0,  0 },
	{  0,  2,  0,  0 },
	{  1,  2,  0,  0 },
	{  0,  1,  2,  0 },
	{  3,  0,  0,  0 },
	{  0,  3,  0,  0 },
	{  1,  3,  0,  0 },
	{  0,  1,  3,  0 },
	{  2,  3,  0,  0 },
	{  0,  2,  3,  0 },
	{  1,  2,  3,  0 },
	{  0,  1,  2,  3 }
};

CONST INT Simplex::s_iNonElements[TWICE_TWO_TO_DIM][DIM_PLUS_ONE] = {
	{  0,  1,  2,  3 },
	{  1,  2,  3,  0 },
	{  0,  2,  3,  0 },
	{  2,  3,  0,  0 },
	{  0,  1,  3,  0 },
	{  1,  3,  0,  0 },
	{  0,  3,  0,  0 },
	{  3,  0,  0,  0 },
	{  0,  1,  2,  0 },
	{  1,  2,  0,  0 },
	{  0,  2,  0,  0 },
	{  2,  0,  0,  0 },
	{  0,  1,  0,  0 },
	{  1,  0,  0,  0 },
	{  0,  0,  0,  0 },
	{  0,  0,  0,  0 }
};

CONST INT Simplex::s_iPredecessor[TWICE_TWO_TO_DIM][DIM_PLUS_ONE] = {
	{  0,  0,  0,  0 },
	{  0,  0,  0,  0 },
	{  0,  0,  0,  0 },
	{  2,  1,  0,  0 },
	{  0,  0,  0,  0 },
	{  4,  1,  0,  0 },
	{  4,  2,  0,  0 },
	{  6,  5,  3,  0 },
	{  0,  0,  0,  0 },
	{  8,  1,  0,  0 },
	{  8,  2,  0,  0 },
	{ 10,  9,  3,  0 },
	{  8,  4,  0,  0 },
	{ 12,  9,  5,  0 },
	{ 12, 10,  6,  0 },
	{ 14, 13, 11,  7 }
};

CONST INT Simplex::s_iSuccessor[TWICE_TWO_TO_DIM][DIM_PLUS_ONE] = {
	{  1,  2,  4,  8 },
	{  3,  5,  9,  0 },
	{  3,  6, 10,  0 },
	{  7, 11,  0,  0 },
	{  5,  6, 12,  0 },
	{  7, 13,  0,  0 },
	{  7, 14,  0,  0 },
	{ 15,  0,  0,  0 },
	{  9, 10, 12,  0 },
	{ 11, 13,  0,  0 },
	{ 11, 14,  0,  0 },
	{ 15,  0,  0,  0 },
	{ 13, 14,  0,  0 },
	{ 15,  0,  0,  0 },
	{ 15,  0,  0,  0 },
	{  0,  0,  0,  0 }
};

FLOAT Simplex::s_fDeltas[TWICE_TWO_TO_DIM][DIM_PLUS_ONE];
FLOAT Simplex::s_fDots[DIM_PLUS_ONE][DIM_PLUS_ONE];


void Simplex::ComputeSubterms() const
{
	// Compute the coordinates of the simplex as C-space obstacle points
	D3DXVECTOR3 vCSOSet[4];
	for (INT i = 0; i < m_nPoints; ++i)
		vCSOSet[i] = m_vASet[i] - m_vBSet[i];

	// Compute the dot product terms
	for (INT i = 0; i < m_nPoints; ++i)
		for (INT j = i; j < m_nPoints; ++j)
			s_fDots[i][j] = s_fDots[j][i] = D3DXVec3Dot(&vCSOSet[i], &vCSOSet[j]);

	// Now, compute all the delta terms
	for (INT s = 1; s < TWICE_TWO_TO_DIM && s_iMaxElement[s] < m_nPoints; ++s)
		switch (s_iCardinality[s])
		{
		case 0:
		case 1:
			s_fDeltas[s][s_iElements[s][0]] = 1.0f;
			break;
		case 2:
         s_fDeltas[s][s_iElements[s][0]] =
			 s_fDots[s_iElements[s][1]][s_iElements[s][1]] - s_fDots[s_iElements[s][1]][s_iElements[s][0]];
         s_fDeltas[s][s_iElements[s][1]] =
			 s_fDots[s_iElements[s][0]][s_iElements[s][0]] - s_fDots[s_iElements[s][0]][s_iElements[s][1]];
			break;
		default:
			for (INT j = 0; j < s_iCardinality[s]; ++j)
			{
				INT   nJElt		= s_iElements[s][j];
				INT   nJSubset	= s_iPredecessor[s][j];
				FLOAT fSum		= 0.0f;
				for (INT i = 0; i < s_iCardinality[nJSubset]; ++i)
				{
					INT nIElt = s_iElements[nJSubset][i];
					fSum += s_fDeltas[nJSubset][nIElt] *
						(s_fDots[nIElt][s_iElements[nJSubset][0]] - s_fDots[nIElt][nJElt]);
				}

				s_fDeltas[s][nJElt] = fSum;
			}
			break;
		}
}

static FLOAT delta[Simplex::TWICE_TWO_TO_DIM];

BOOL Simplex::DefaultDistance()
{
	BOOL bOK = FALSE;
	INT s;

	// For every subset s of the given set of points ...
	for (s = 1; s < TWICE_TWO_TO_DIM && s_iMaxElement[s] < m_nPoints; ++s)
	{
		// delta[s] will accumulate the sum of the delta expressions for
		// this subset, and ok will remain TRUE whilst this subset can
		// still be thought to be a candidate simplex for the shortest
		// distance.
		delta[s] = 0.0f; bOK = TRUE;

		// Now the first check is whether the simplex formed by this
		// subset holds the foot of the perpendicular from the origin
		// to the point/line/plane passing through the simplex. This will
		// be the case if all the delta terms for each s_iPredecessor subset
		// are (strictly) positive.
		for (INT j = 0; bOK && j < s_iCardinality[s]; ++j)
			if (s_fDeltas[s][s_iElements[s][j]] > 0.0f)
				delta[s] += s_fDeltas[s][s_iElements[s][j]];
			else
				bOK = FALSE;

		// If the subset survives the previous test, we still need to check
		// whether the true minimum distance is to a larger piece of geometry,
		// or indeed to another piece of geometry of the same dimensionality.
		// A necessary and sufficient condition for it to fail at this stage
		// is if the delta term for any s_iSuccessor subset is positive, as this
		// indicates a direction on the appropriate higher dimensional simplex
		// in which the distance gets shorter.
		for (INT k = 0; bOK && k < m_nPoints - s_iCardinality[s]; ++k)
			if (s_fDeltas[s_iSuccessor[s][k]][s_iNonElements[s][k]] > 0)
				bOK = FALSE;

		// If we've found a viable subset -- stopping!
		if (bOK && delta[s] >= GJK_TINY)
			break;
	}

	if (bOK)
		Reset(s);
	return bOK;
}

void Simplex::BackupDistance()
{
	INT nBestS = 0;
	FLOAT fDistSq_Num[TWICE_TWO_TO_DIM], fDistSq_Den[TWICE_TWO_TO_DIM];

	// For every subset s of the given set of points ...
	for (INT s = 1; s < TWICE_TWO_TO_DIM && s_iMaxElement[s] < m_nPoints; ++s)
	{
		if (delta[s] <= 0.0f)
			continue;

		INT i;
		for (i = 0; i < s_iCardinality[s]; ++i)
			if (s_fDeltas[s][s_iElements[s][i]] <= 0.0f)
				break;
		if (i < s_iCardinality[s])
			continue;

		// otherwise we've found a viable subset
      fDistSq_Num[s] = 0.0f;
      for (INT j = 0; j < s_iCardinality[s]; ++j)
         for (INT k = 0; k < s_iCardinality[s]; ++k)
            fDistSq_Num[s] += s_fDeltas[s][s_iElements[s][j]] *
							  s_fDeltas[s][s_iElements[s][k]] *
							  s_fDots[s_iElements[s][j]][s_iElements[s][k]];
      fDistSq_Den[s] = delta[s] * delta[s];

      if (nBestS < 1 ||
		  fDistSq_Num[s] * fDistSq_Den[nBestS] < fDistSq_Num[nBestS] * fDistSq_Den[s])
         nBestS = s;
	}

	Reset(nBestS);
}

void Simplex::Reset(INT nSubset)
{
	// Compute the lambda values that indicate exactly where the
	// witness points lie. We also fold back the values stored for the
	// indices into the original point arrays, and the transformed
	// coordinates, so that these are ready for subsequent calls.
	for (INT j = 0; j < s_iCardinality[nSubset]; ++j)
	{	// rely on s_iElements[nSubset][j]] >= j, which is true as they are
		// stored in ascending order.
		INT nOldPos = s_iElements[nSubset][j];
		if (nOldPos != j)
		{
			m_nASet[j] = m_nASet[nOldPos];
			m_vASet[j] = m_vASet[nOldPos];
			m_nBSet[j] = m_nBSet[nOldPos];
			m_vBSet[j] = m_vBSet[nOldPos];
		}

		m_fLambdas[j] = s_fDeltas[nSubset][s_iElements[nSubset][j]] / delta[nSubset];
	}

	m_nPoints = s_iCardinality[nSubset];
}


static UINT GetSupportPoint(CONST D3DXVECTOR3 P[], UINT nP, CONST D3DXVECTOR3& vDirection,
							FLOAT *pSupportVal)
{	// No information for hill-climbing. Use brute-force instead.
	FLOAT fMaxDot = -FLT_MAX;
	UINT  nBestP  = 0;
	for (UINT p = 0; p < nP; ++p)
	{
		FLOAT fDot = D3DXVec3Dot(&P[p], &vDirection);
		if (fDot > fMaxDot)
			fMaxDot = fDot, nBestP = p;
	}

	*pSupportVal = fMaxDot;
	return nBestP;
}



FLOAT GjkDistanceSq(CONST D3DXVECTOR3 A[], UINT nA, CONST D3DXMATRIX *pATransformation,
					CONST D3DXVECTOR3 B[], UINT nB, CONST D3DXMATRIX *pBTransformation,
					Simplex *pSimplex, BOOL bUseSeed,
					D3DXVECTOR3 *pAClosestPoint,
					D3DXVECTOR3 *pBClosestPoint)
{
	BOOL bUseDefault = TRUE, bFirstIteration = TRUE;
	FLOAT fOldSqD = -FLT_MAX;

	// If we have custom transformation we need save transposed displacement
	// vector as local vector
	D3DXVECTOR3 vDisplacement, vRevDisplacement,
				vLocalDisplacement, vLocalRevDisplacement;
	D3DXVECTOR3 *pFDisp = pATransformation ? &vLocalDisplacement    : &vDisplacement,
				*pRDisp = pBTransformation ? &vLocalRevDisplacement : &vRevDisplacement;

	// If have cached best simplex from previous iteration -- use it!
	// (We need only update vertices in both sets neccesary!)
	Simplex simplex;
	if (!pSimplex)
	{
		bUseSeed = FALSE;
		pSimplex = &simplex;
	}

	if (bUseSeed)
		pSimplex->UpdateVertices(A, pATransformation, B, pBTransformation);
	else
		pSimplex->AddVertex(0, A, 0, pATransformation, B, 0, pBTransformation, 1.0f);

	// Counting the iterations in this way should not be necessary;
    // a for(;;) should do just as well.
	for (UINT i = 0; i < nA*nB; ++i)
	{
		// Now the main loop. We first compute the distance between the
		// current simplicies, the check whether this gives the globally
		// correct answer, and if not construct new simplices and try again.
		if (pSimplex->m_nPoints == 1)		// simple case
			pSimplex->m_fLambdas[0] = 1.0f;
		else								// normal case
		{
			pSimplex->ComputeSubterms();
			if (bUseDefault)
				bUseDefault = pSimplex->DefaultDistance();
			if (!bUseDefault)
				pSimplex->BackupDistance();
		}

		// Compute at least the displacement vectors given by the
		// simplex_point structure. If we are to provide both witness
		// points, it's slightly faster to compute those first.
		if (pAClosestPoint || pBClosestPoint)
		{
			*pAClosestPoint = pSimplex->ExtractPoint(0);
			*pBClosestPoint = pSimplex->ExtractPoint(1);
			D3DXVec3Subtract(&vDisplacement, pBClosestPoint, pAClosestPoint);
		}
		else
			vDisplacement = pSimplex->ComputeDisplacement();
		vRevDisplacement = -vDisplacement;

		// If we are using a c-space simplex with D+1
		// points, this is interior to the simplex, and indicates
		// that the original hulls overlap, as does the distance 
		// between them being too small.
		FLOAT SqD = D3DXVec3LengthSq(&vDisplacement);
		if (SqD < GJK_TOLERANCE)
			break; //return SqD;

		// If we have transformation -- apply inverse rotation to displacement vectors
		if (pATransformation)
		{
			D3DXMATRIX mAITT;
			D3DXMatrixTranspose(&mAITT, pATransformation);
			D3DXVec3TransformNormal(pFDisp, &vDisplacement, &mAITT);
		}
		if (pBTransformation)
		{
			D3DXMATRIX mBITT;
			D3DXMatrixTranspose(&mBITT, pBTransformation);
			D3DXVec3TransformNormal(pRDisp, &vRevDisplacement, &mBITT);
		}

		// Find the point in A that is maximal in the
		// direction displacement, and the point in B that
		// is minimal in direction displacement.
		FLOAT fMaxV;
		UINT  nMaxP = GetSupportPoint(A, nA, *pFDisp, &fMaxV);
		FLOAT fMinusMinV;
		UINT  nMinP = GetSupportPoint(B, nB, *pRDisp, &fMinusMinV);

		// Now apply the G-test on this pair of points.
		FLOAT fG = SqD + fMaxV + fMinusMinV;
		if (pATransformation)
		{
			D3DXVECTOR3 vATranslation(pATransformation->_41, pATransformation->_42, pATransformation->_43);
			fG += D3DXVec3Dot(&vDisplacement, &vATranslation);
		}
		if (pBTransformation)
		{
			D3DXVECTOR3 vBTranslation(pBTransformation->_41, pBTransformation->_42, pBTransformation->_43);
			fG += D3DXVec3Dot(&vRevDisplacement, &vBTranslation);
		}

		if (fG < GJK_TOLERANCE)	// then no better points - finish
			return SqD;

		// Check for good calculation above
		if ((bFirstIteration || SqD < fOldSqD) && pSimplex->m_nPoints < Simplex::DIM_PLUS_ONE)
		{	// Normal case: add the new c-space points to the current
			// simplex, and call simplex_distance()
			pSimplex->AddVertex(pSimplex->m_nPoints, A, nMaxP, pATransformation, B, nMinP, pBTransformation, 0.0f);
			fOldSqD = SqD;
			bFirstIteration = FALSE;
			bUseDefault = TRUE;
		}
		else if (bUseDefault)	// Abnormal cases!
			bUseDefault = FALSE;
		else
			return SqD;
	}

	return 0.0f;	// original hulls overlapped -- return ZERO!
}
