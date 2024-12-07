#pragma once


struct Simplex
{
	//		C O N S T R U C T I O N		//

	Simplex()
		: m_nPoints(0)
	{}

	Simplex(CONST D3DXVECTOR3 A[], CONST D3DXMATRIX* pATransformation/* = NULL*/,
			CONST D3DXVECTOR3 B[], CONST D3DXMATRIX* pBTransformation/* = NULL*/)
		: m_nPoints(0)
	{
		AddVertex(0, A, 0, pATransformation, B, 0, pBTransformation, 1.0f);
	}


	//			U T I L S				//

	void ComputeSubterms() const;
	BOOL DefaultDistance();
	void BackupDistance();
	void Reset(INT nSubset);


	//		R E F I R E M E N T			//

	void AddVertex(UINT nPos,
				   CONST D3DXVECTOR3 A[], UINT IAV, CONST D3DXMATRIX* pATransformation/* = NULL*/,
				   CONST D3DXVECTOR3 B[], UINT IBV, CONST D3DXMATRIX* pBTransformation/* = NULL*/,
				   FLOAT fLambda = 1.0f)
	{
		m_nASet[nPos] = IAV;
		if (pATransformation)
			D3DXVec3TransformCoord(&m_vASet[nPos], &A[IAV], pATransformation);
		else
			m_vASet[nPos] = A[IAV];

		m_nBSet[nPos] = IBV;
		if (pBTransformation)
			D3DXVec3TransformCoord(&m_vBSet[nPos], &B[IBV], pBTransformation);
		else
			m_vBSet[nPos] = B[IBV];

		m_fLambdas[nPos] = fLambda;
		++m_nPoints;
	}

	void UpdateVertices(CONST D3DXVECTOR3 A[], CONST D3DXMATRIX* pATransformation/* = NULL*/,
						CONST D3DXVECTOR3 B[], CONST D3DXMATRIX* pBTransformation/* = NULL*/)
	{
		for (INT p = 0; p < m_nPoints; ++p)
		{
			if (pATransformation)
				D3DXVec3TransformCoord(&m_vASet[p], &A[m_nASet[p]], pATransformation);
			else
				m_vASet[p] = A[m_nASet[p]];

			if (pBTransformation)
				D3DXVec3TransformCoord(&m_vBSet[p], &B[m_nBSet[p]], pBTransformation);
			else
				m_vBSet[p] = B[m_nBSet[p]];
		}
	}

	D3DXVECTOR3 ExtractPoint(UINT nWhichSet) const
	{
		D3DXVECTOR3 v(0.0f, 0.0f, 0.0f);
		CONST D3DXVECTOR3 *pSet = nWhichSet ? m_vBSet : m_vASet;
		for (INT p = 0; p < m_nPoints; ++p)
			v += pSet[p] * m_fLambdas[p];
		return v;
	}

	D3DXVECTOR3 ComputeDisplacement() const
	{
		D3DXVECTOR3 v(0.0f, 0.0f, 0.0f);
		for (INT p = 0; p < m_nPoints; ++p)
			v += (m_vBSet[p] - m_vASet[p]) * m_fLambdas[p];
		return v;
	}


	//		S T A T I C   D A T A			//
	enum {
		DIM					= 3,
		DIM_PLUS_ONE		= DIM + 1,
		TWO_TO_DIM			= 1<<DIM,					// 2^DIM
		TWICE_TWO_TO_DIM	= TWO_TO_DIM + TWO_TO_DIM	// 2^DIM * 2
	};

	static CONST INT s_iCardinality[TWICE_TWO_TO_DIM];
	static CONST INT s_iMaxElement[TWICE_TWO_TO_DIM];
	static CONST INT s_iElements[TWICE_TWO_TO_DIM][DIM_PLUS_ONE];
	static CONST INT s_iNonElements[TWICE_TWO_TO_DIM][DIM_PLUS_ONE];
	static CONST INT s_iPredecessor[TWICE_TWO_TO_DIM][DIM_PLUS_ONE];
	static CONST INT s_iSuccessor[TWICE_TWO_TO_DIM][DIM_PLUS_ONE];

	static FLOAT	 s_fDeltas[TWICE_TWO_TO_DIM][DIM_PLUS_ONE];
	static FLOAT	 s_fDots[DIM_PLUS_ONE][DIM_PLUS_ONE];


	//		I M P L E M E N T A T I O N		//

	UINT		m_nASet[4];
	D3DXVECTOR3	m_vASet[4];
	UINT		m_nBSet[4];
	D3DXVECTOR3	m_vBSet[4];
	INT			m_nPoints;
	FLOAT		m_fLambdas[4];
};


FLOAT GjkDistanceSq(CONST D3DXVECTOR3 A[], UINT nA, CONST D3DXMATRIX *pATransformation/* = NULL*/,
					CONST D3DXVECTOR3 B[], UINT nB, CONST D3DXMATRIX *pBTransformation/* = NULL*/,
					Simplex *pSimplex = NULL, BOOL bUseSeed = FALSE,
					D3DXVECTOR3 *pAClosestPoint = NULL,
					D3DXVECTOR3 *pBClosestPoint = NULL);
