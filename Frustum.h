#pragma once


struct Frustum
{
	enum { LEFTPLANE,
		   RIGHTPLANE,
		   TOPPLANE,
		   BOTTOMPLANE,
		   NEARPLANE,
		   FARPLANE,
		   NUMPLANES };

	Frustum(void)
	{}

	Frustum(const LPD3DXMATRIX pProjection, bool bNormalize = false);

	void Extract(const LPD3DXMATRIX pProjection, bool bNormalize = false);

	bool TestPoint(const LPD3DXVECTOR3 pPoint) const;
	bool TestAABB(const LPD3DXVECTOR3 pMin, const LPD3DXVECTOR3 pMax) const;
	bool TestAABB(const D3DXVECTOR3 vMinMax[2]) const;
	bool TestSphere(const LPD3DXVECTOR3 pCenter, FLOAT Radius) const;


	D3DXPLANE m_planes[NUMPLANES];
};

inline Frustum::Frustum(const LPD3DXMATRIX pProjection, bool bNormalize)
{
	Extract(pProjection, bNormalize);
}

inline void Frustum::Extract(const LPD3DXMATRIX pProjectionMatrix, bool bNormalize)
{
	// Left clipping plane 
	m_planes[LEFTPLANE].a = pProjectionMatrix->_14 + pProjectionMatrix->_11; 
	m_planes[LEFTPLANE].b = pProjectionMatrix->_24 + pProjectionMatrix->_21; 
	m_planes[LEFTPLANE].c = pProjectionMatrix->_34 + pProjectionMatrix->_31; 
	m_planes[LEFTPLANE].d = pProjectionMatrix->_44 + pProjectionMatrix->_41;

	// Right clipping plane 
	m_planes[RIGHTPLANE].a = pProjectionMatrix->_14 - pProjectionMatrix->_11; 
	m_planes[RIGHTPLANE].b = pProjectionMatrix->_24 - pProjectionMatrix->_21; 
	m_planes[RIGHTPLANE].c = pProjectionMatrix->_34 - pProjectionMatrix->_31; 
	m_planes[RIGHTPLANE].d = pProjectionMatrix->_44 - pProjectionMatrix->_41;

	// Top clipping plane 
	m_planes[TOPPLANE].a = pProjectionMatrix->_14 - pProjectionMatrix->_12; 
	m_planes[TOPPLANE].b = pProjectionMatrix->_24 - pProjectionMatrix->_22; 
	m_planes[TOPPLANE].c = pProjectionMatrix->_34 - pProjectionMatrix->_32; 
	m_planes[TOPPLANE].d = pProjectionMatrix->_44 - pProjectionMatrix->_42;

	// Bottom clipping plane 
	m_planes[BOTTOMPLANE].a = pProjectionMatrix->_14 + pProjectionMatrix->_12; 
	m_planes[BOTTOMPLANE].b = pProjectionMatrix->_24 + pProjectionMatrix->_22; 
	m_planes[BOTTOMPLANE].c = pProjectionMatrix->_34 + pProjectionMatrix->_32; 
	m_planes[BOTTOMPLANE].d = pProjectionMatrix->_44 + pProjectionMatrix->_42;

	// Near clipping plane 
	m_planes[NEARPLANE].a = pProjectionMatrix->_13; 
	m_planes[NEARPLANE].b = pProjectionMatrix->_23; 
	m_planes[NEARPLANE].c = pProjectionMatrix->_33; 
	m_planes[NEARPLANE].d = pProjectionMatrix->_43;

	// Far clipping plane 
	m_planes[FARPLANE].a = pProjectionMatrix->_14 - pProjectionMatrix->_13; 
	m_planes[FARPLANE].b = pProjectionMatrix->_24 - pProjectionMatrix->_23; 
	m_planes[FARPLANE].c = pProjectionMatrix->_34 - pProjectionMatrix->_33; 
	m_planes[FARPLANE].d = pProjectionMatrix->_44 - pProjectionMatrix->_43; 

	// It is not always nessesary to normalize the planes of the frustum.
	// Non-normalized planes can still be used for basic intersection tests.
	if (bNormalize)
	{
		::D3DXPlaneNormalize(&m_planes[LEFTPLANE],	&m_planes[LEFTPLANE]);
		::D3DXPlaneNormalize(&m_planes[RIGHTPLANE],	&m_planes[RIGHTPLANE]);
		::D3DXPlaneNormalize(&m_planes[TOPPLANE],	&m_planes[TOPPLANE]);
		::D3DXPlaneNormalize(&m_planes[BOTTOMPLANE],&m_planes[BOTTOMPLANE]);
		::D3DXPlaneNormalize(&m_planes[NEARPLANE],	&m_planes[NEARPLANE]);
		::D3DXPlaneNormalize(&m_planes[FARPLANE],	&m_planes[FARPLANE]);
	}
}

inline bool Frustum::TestPoint(const LPD3DXVECTOR3 pPoint) const
{
	if (::D3DXPlaneDotCoord(&m_planes[LEFTPLANE],	pPoint) < 0.0f ||
		::D3DXPlaneDotCoord(&m_planes[RIGHTPLANE],	pPoint) < 0.0f ||
		::D3DXPlaneDotCoord(&m_planes[TOPPLANE],	pPoint) < 0.0f ||
		::D3DXPlaneDotCoord(&m_planes[BOTTOMPLANE], pPoint) < 0.0f ||
		::D3DXPlaneDotCoord(&m_planes[NEARPLANE],	pPoint) < 0.0f ||
		::D3DXPlaneDotCoord(&m_planes[FARPLANE],	pPoint) < 0.0f)
		return false;
	return true;
}

inline bool Frustum::TestAABB(const LPD3DXVECTOR3 pMin, const LPD3DXVECTOR3 pMax) const
{
	for (UINT i = 0; i < NUMPLANES; ++i)
	{
		D3DXVECTOR3 ptFarToPlane;
		ptFarToPlane.x = FloatIsNegative(m_planes[i].a) ? pMin->x : pMax->x;
		ptFarToPlane.y = FloatIsNegative(m_planes[i].b) ? pMin->y : pMax->y;
		ptFarToPlane.z = FloatIsNegative(m_planes[i].c) ? pMin->z : pMax->z;

		if (::D3DXPlaneDotCoord(&m_planes[i], &ptFarToPlane) < 0.0f)
			return false;
	}

	return true;
}

inline bool Frustum::TestAABB(const D3DXVECTOR3 vMinMax[2]) const
{
	for (UINT i = 0; i < NUMPLANES; ++i)
	{
		D3DXVECTOR3 ptFarToPlane;
		ptFarToPlane.x = vMinMax[FloatExtractSign(m_planes[i].a) ^ 1].x;
		ptFarToPlane.y = vMinMax[FloatExtractSign(m_planes[i].b) ^ 1].y;
		ptFarToPlane.z = vMinMax[FloatExtractSign(m_planes[i].c) ^ 1].z;

		if (::D3DXPlaneDotCoord(&m_planes[i], &ptFarToPlane) < 0.0f)
			return false;
	}

	return true;
}

inline bool Frustum::TestSphere(const LPD3DXVECTOR3 pCenter, FLOAT Radius) const
{	// Note: planes must be normalzed !
	if (::D3DXPlaneDotCoord(&m_planes[LEFTPLANE],	pCenter) + Radius < -Radius ||
		::D3DXPlaneDotCoord(&m_planes[RIGHTPLANE],	pCenter) + Radius < -Radius ||
		::D3DXPlaneDotCoord(&m_planes[TOPPLANE],	pCenter) + Radius < -Radius ||
		::D3DXPlaneDotCoord(&m_planes[BOTTOMPLANE],	pCenter) + Radius < -Radius ||
		::D3DXPlaneDotCoord(&m_planes[NEARPLANE],	pCenter) + Radius < -Radius ||
		::D3DXPlaneDotCoord(&m_planes[FARPLANE],	pCenter) + Radius < -Radius)
		return false;
	return true;
}
