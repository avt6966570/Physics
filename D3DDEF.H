#pragma once

#include <d3dx9.h>


/*
 *
 *	File:		d3ddef.h
 *	Desc:		more d3d custom utils
 *	Relise:		11/09/2005 11:01
 *	Author:		Max A. Berezhnoy
 *
 */


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = NULL; } }
#endif // SAFE_DELETE
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete [] (p);  (p) = NULL; } }
#endif // SAFE_DELETE_ARRAY
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = NULL; } }
#endif // SAFE_RELEASE


#define SIGN(x)	(((x) > 0) - ((x) < 0))



//*****************************************************************************
//								STRING UTILS
//*****************************************************************************

//	Analog strdup() but allocation via C++ ::new() operator
inline LPSTR AllocName (LPCSTR cpName)
{
	if (cpName)
	{	// Get name length
		UINT cbLength = ::lstrlen(cpName) + 1;
		LPSTR pNewName = new CHAR [ cbLength ];
		if (pNewName)
		{	// copy name
			::memcpy(pNewName, cpName, cbLength * sizeof(CHAR));
			return pNewName;
		}
	}

	return NULL;
}




//*****************************************************************************
//								MATH UTILS
//*****************************************************************************

//-----------------------------------------------------------------------------
//	FUNCTION:	clamp()
//	PURPOSE:	Returns value clamped between low and high so that
//				(low <= value <= high)
//	INPUT:		Value, min & max
//	OUTPUT:		Clamped value
//-----------------------------------------------------------------------------
template <class T> inline T clamp(T value, T low, T high)
{
	return value < low ? low : value > high ? high : value;
}


//-----------------------------------------------------------------------------
//	FUNCTION:	Min3()
//	PURPOSE:	Returns minimum value index
//	INPUT:		Three values
//	OUTPUT:		Index for minimum value in order
//-----------------------------------------------------------------------------
template<typename T> inline int Min3(T v0, T v1, T v2)
{
	return v0 < v1 ? v0 < v2 ? 0 : 2 : v1 < v2 ? 1 : 2;
}


//-----------------------------------------------------------------------------
//	FUNCTION:	Max3()
//	PURPOSE:	Returns maximum value index
//	INPUT:		Three values
//	OUTPUT:		Index for maximum value in order
//-----------------------------------------------------------------------------
template<typename T> inline int Max3(T v0, T v1, T v2)
{
	return v0 > v1 ? v0 > v2 ? 0 : 2 : v1 > v2 ? 1 : 2;
}


//-----------------------------------------------------------------------------
//	FUNCTION:	clampPositive()
//	PURPOSE:	Returns value so that (value >= 0)
//	INPUT:		Value
//	OUTPUT:		Positiwe clamped value
//-----------------------------------------------------------------------------
template <class T> inline T clampPositive(T value)
{
	return value < 0 ? 0 : value;
}

template <> inline float clampPositive(float fVal)
{
	// if the value is negative, set it to zero
	long iVal		= *reinterpret_cast<const long*>(&fVal);
	long iSignMask	= ~(iVal >> 31);
	iVal &= iSignMask;
	return *reinterpret_cast<const float*>(&iVal);
}


//-----------------------------------------------------------------------------
//	FUNCTION:	clampPositive()
//	PURPOSE:	Returns value so that (value <= 0)
//	INPUT:		Value
//	OUTPUT:		Positiwe clamped value
//-----------------------------------------------------------------------------
template <class T> inline T clampNegative(T value)
{
	return value > 0 ? 0 : value;
}

template <> inline float clampNegative(float fVal)
{
    // if the value is positive, set it to zero
	long iVal		= *reinterpret_cast<const long*>(&fVal);
	long iSignMask	= iVal >> 31;
	iVal &= iSignMask;
	return *reinterpret_cast<const float*>(&iVal);
}




//*****************************************************************************
//								BIT UTILS
//*****************************************************************************

namespace Bit
{
	//-------------------------------------------------------------------------
	//	FUNCTION:	bool NthTest()
	//	PURPOSE:	Test Nth bit from iVal
	//	RETURN:		Boolean result
	//-------------------------------------------------------------------------
	inline bool NthTest(unsigned long iVal, unsigned long nthBit)
	{
		return iVal & (1 << nthBit) ? true : false;
	}

	//-------------------------------------------------------------------------
	//	FUNCTION:	bool TestAll()
	//	PURPOSE:	Strong test all flags
	//	RETURN:		Boolean result
	//-------------------------------------------------------------------------
	inline bool TestAll(unsigned long iVal, unsigned long iFlags)
	{
		return (iVal & iFlags) == iFlags ? true : false;
	}

	//-------------------------------------------------------------------------
	//	FUNCTION:	bool TestAll()
	//	PURPOSE:	True if any flags is set
	//	RETURN:		Boolean result
	//-------------------------------------------------------------------------
	inline bool TestAny(unsigned long iVal, unsigned long iFlags)
	{
		return iVal & iFlags ? true : false;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	void SetNth()
	//	PURPOSE:	Set Nth bit from pVal
	//	RETURN:		Bit string with setted nth bit
	//-------------------------------------------------------------------------
	inline void SetNth(unsigned long *pVal, unsigned long nthBit)
	{
		*pVal |= 1 << nthBit;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	void ClearNth()
	//	PURPOSE:	Clear Nth bit from pVal
	//	RETURN:		Bit string with cleared nth bit
	//-------------------------------------------------------------------------
	inline void ClearNth(unsigned long *pVal, unsigned long nthBit)
	{
		*pVal &= ~(1 << nthBit);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	void NegateNth()
	//	PURPOSE:	Negate Nth bit from pVal
	//	RETURN:		Bit string with setted bit
	//-------------------------------------------------------------------------
	inline void NegateNth(unsigned long *pVal, unsigned long nthBit)
	{
		*pVal ^= 1 << nthBit;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long Swap()
	//	PURPOSE:	Swap byte (0xAABBCCDD => 0xDDCCBBAA)
	//	RETURN:		Swapped bit string
	//-------------------------------------------------------------------------
	inline unsigned long Swap(unsigned long x)
	{
		_asm {
			mov		eax, x
			bswap	eax
		};
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long ZeroRight()
	//	PURPOSE:	Обнулить крайний справа еденичный бит
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long ZeroRight(unsigned long x)
	{
		return x & (x - 1);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long Is2n1()
	//	PURPOSE:	Проверить беззнаковое число на соответствие виду 2n - 1
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long Is2n1(unsigned long x)
	{
		return x & (x + 1);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	long SelectRightIdentity()
	//	PURPOSE:	Выделить крайний справа единичный бит
	//				(например 01011000 => 00001000, если такого нет - возвращается ноль)
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline long SelectRightIdentity(long x)
	{
		return x & (-x);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	long SelectRightZero()
	//	PURPOSE:	Выделить крайний справа нулевой бит
	//				(например 10100111 => 00001000, если такого нет - возвращается ноль)
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline long SelectRightZero(long x)
	{
		return -x & (x + 1);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	long MaskZeroes()
	//	PURPOSE:	Маска идентифицирующая завершающие нулевые биты
	//				(например 01011000 => 00000111, если исходное слово ноль
	//				все биты маски равны 1)
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline long MaskZeroes(long x)
	{
		return -x & (x - 1);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	long SpreadRightIdentity()
	//	PURPOSE:	Распространяет вправо крайне правый еденичный бит
	//				(например 01011000 => 01011111, если исходное слово ноль
	//				все биты результата равны 1)
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long SpreadRightIdentity(unsigned long x)
	{
		return x | (x - 1);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long ZeroRightSubstring()
	//	PURPOSE:	Обнуление крайней справа непрерывной подстроки еденичных битов
	//				(01011000 => 01000000)
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long ZeroRightSubstring(unsigned long x)
	{
		return ((x | (x - 1)) + 1) & x;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long PowerOfTwoFloor()
	//	PURPOSE:	Наибольшая степень 2 не превосходящая значение х
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long PowerOfTwoFloor(unsigned long x)
	{
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		return x - (x >> 1);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long PowerOfTwoCeil()
	//	PURPOSE:	Наименьшая степень 2 не уступающая значению х
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long PowerOfTwoCeil(unsigned long x)
	{
		--x;
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		return x + 1;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long NumIdentity()
	//	PURPOSE:	Подсчет количества единичных битов
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long NumIdentity(unsigned long x)
	{
		x -= ((x >> 1) & 0x55555555);
		x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
		x = (x + (x >> 4)) & 0x0F0F0F0F;
		x += (x >> 8);
		x += (x >> 16);
		return x & 0x0000003F;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long NumLeadingZeroes()
	//	PURPOSE:	Подсчет ведущих нулевых битов
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long NumLeadingZeroes(unsigned long x)
	{
		if (x == 0) return 32;
		unsigned long n = 0;
		if ((x & 0xFFFF0000) == 0) { n +=16; x <<=16; };
		if ((x & 0xFF000000) == 0) { n += 8; x <<= 8; };
		if ((x & 0xF0000000) == 0) { n += 4; x <<= 4; };
		if ((x & 0xC0000000) == 0) { n += 2; x <<= 2; };
		if ((x & 0x80000000) == 0) { ++n; };
		return n;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long NumFinishZeroes()
	//	PURPOSE:	Подсчет завершающих нулевых битов бинарным поиском
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long NumFinishZeroes(unsigned long x)
	{
		if (x == 0) return 32;
		unsigned long n = 1;
		if ((x & 0x0000FFFF) == 0) { n +=16; x >>=16; };
		if ((x & 0x000000FF) == 0) { n += 8; x >>= 8; };
		if ((x & 0x0000000F) == 0) { n += 4; x >>= 4; };
		if ((x & 0x00000003) == 0) { n += 2; x >>= 2; };
		return n - (x & 1);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long Reverse()
	//	PURPOSE:	Bits reverse
	//	RETURN:		Reversed bit string
	//-------------------------------------------------------------------------
	inline unsigned long Reverse(unsigned long x)
	{
		x = (x & 0x55555555) << 1 | (x >> 1) & 0x55555555;
		x = (x & 0x33333333) << 2 | (x >> 2) & 0x33333333;
		x = (x & 0x0F0F0F0F) << 4 | (x >> 4) & 0x0F0F0F0F;
		x = (x << 24) | ((x & 0xFF00) << 8) |
			((x >> 8) & 0xFF00) | (x >> 24);
		return x;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long SwapWord()
	//	PURPOSE:	Swap word in register
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long SwapWord(unsigned long x)
	{
		return (x >> 16) | (x << 16);
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long ExternalShuffle()
	//	PURPOSE:	Внешнее идеальное перемешивание (external perfect shuffle)
	//				(abcd efgh ijkl mnop ABCD EFGH IJKL MNOP =>
	//				aAbB cCdD eEfF gGhH iIjJ kKlL mMnN oOpP)
	//	RETURN:		Shuffled bit string
	//-------------------------------------------------------------------------
	inline unsigned long ExternalShuffle(unsigned long x)
	{
		unsigned long t;
		t = (x ^ (x >> 8)) & 0x0000FF00; x = x ^ t ^ (t << 8);
		t = (x ^ (x >> 4)) & 0x00F000F0; x = x ^ t ^ (t << 4);
		t = (x ^ (x >> 2)) & 0x0C0C0C0C; x = x ^ t ^ (t << 2);
		t = (x ^ (x >> 1)) & 0x22222222; x = x ^ t ^ (t << 1);
		return x;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long ExternalUnshuffle()
	//	PURPOSE:	Внешнее идеальное обратное перемешивание
	//				(external perfect unshuffle)
	//	RETURN:		Unshuffled bit string
	//-------------------------------------------------------------------------
	inline unsigned long ExternalUnshuffle(unsigned long x)
	{
		unsigned long t;
		t = (x ^ (x >> 1)) & 0x22222222; x = x ^ t ^ (t << 1);
		t = (x ^ (x >> 2)) & 0x0C0C0C0C; x = x ^ t ^ (t << 2);
		t = (x ^ (x >> 4)) & 0x00F000F0; x = x ^ t ^ (t << 4);
		t = (x ^ (x >> 8)) & 0x0000FF00; x = x ^ t ^ (t << 8);
		return x;
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long InternalShuffle()
	//	PURPOSE:	Внутреннее идеальное перемешивание (internal perfect shuffle)
	//				(abcd efgh ijkl mnop ABCD EFGH IJKL MNOP =>
	//				 AaBb CcDd EeFf GgHh IiJj KkLl MmNn OoPp)
	//	RETURN:		Shuffled bit string
	//-------------------------------------------------------------------------
	inline unsigned long InternalShuffle(unsigned long x)
	{
		return ExternalShuffle(SwapWord(x));
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long InternalUnshuffle()
	//	PURPOSE:	Внутреннее идеальное обратное перемешивание
	//				(internal perfect unshuffle)
	//	RETURN:		Unshuffled bit string
	//-------------------------------------------------------------------------
	inline unsigned long InternalUnshuffle(unsigned long x)
	{
		return SwapWord(ExternalUnshuffle(x));
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long Transpose32x32()
	//	PURPOSE:	Транспонирование битовой матрицы 32х32
	//	RETURN:		Reversed bit string
	//-------------------------------------------------------------------------
	inline void Transpose32x32(unsigned long A[32])
	{
		for(unsigned int j = 16, m = 0x0000FFFF, t; j; j >>= 1, m ^= (m << j))
		{
			for(unsigned int k = 0; k < 32; k = (k + j + 1) & ~j)
			{
				t = (A[k] ^ (A[k+j] >> j)) & m;
				A[k] ^= t;
				A[k+j] ^= (t << j);
			}
		}
	}


	//-------------------------------------------------------------------------
	//	FUNCTION:	unsigned long Compress()
	//	PURPOSE:	Сжатие (или обобщенное извлечение) на основе параллельного префикса
	//	RETURN:		Bit string
	//-------------------------------------------------------------------------
	inline unsigned long Compress(unsigned long x, unsigned long m)
	{
		unsigned long r = 0, b, s = 0;	// result, shift, mask bit
		do {
			b = m & 1;
			r |= ((x & b) << s);
			s += b;
			x >>= 1;
			m >>= 1;
		} while (m != 0);
		return r;
	}

} // END namespace Bit




//*****************************************************************************
//								INTEGER UTILS
//*****************************************************************************

namespace Integer
{
	__declspec(selectany) long iRandomSeed;

	inline void SetRandomSeed(long iSeed)
	{
		iRandomSeed = iSeed;
	}

	inline long Random()
	{
		iRandomSeed = 1664525L * iRandomSeed + 1013904223L;
		return iRandomSeed;
	}

	inline long Random(long iMin, long iMax)
	{
		long iRandom = Random();
		if (iRandom < 0)
			iRandom = -iRandom;
		return iMin + iRandom % (iMax - iMin);
	}

} // END namespace Integer




//*****************************************************************************
//								REAL UTILS
//*****************************************************************************

namespace Real
{
	const float E						= 2.71828182845904523536f;
	const float INVE					= 0.36787944117144232159f;		// 1/E
	const float ESQ						= 7.38905609893065022723f;		// E * E
	const float LOG2E					= 1.44269504088896340736f;
	const float LOG10E					= 0.43429448190325182765f;
	const float LN2						= 0.69314718055994530941f;
	const float LN3						= 1.09861228866810969139f;
	const float LN10					= 2.30258509299404568402f;
	const float LNPI					= 1.14472988584940017414f;

	const float PI						= 3.14159265358979323846f;
	const float TWOPI					= 6.28318530717958647692f;		// PI * 2
	const float HALFPI					= 1.57079632679489661923f;		// PI / 2
	const float QUARTERPI				= 0.78539816339744830961f;		// PI / 4
	const float INVPI					= 0.31830988618379067153f;		// 1 / PI
	const float PIDIV2					= 0.63661977236758134307f;		// 2 / PI
	const float PIOVER180				= (PI / 180.0f);
	const float PIDIV180				= (180.0f / PI);
	const float SQRT2					= 1.41421356237309504880f;		// sqrt(2)
	const float INVSQRT2				= 0.70710678118654752440f;		// 1/sqrt(2)
	const float SQRT3					= 1.73205080756887729352f;
	const float SQRT5					= 2.23606797749978969640f;
	const float CUBERT2					= 1.25992104989487316476f;
	const float CUBERT3					= 1.44224957030740838232f;

	const float GRAVITY					= 9.81f;

	const float	SMALLEPSILON			= 1e-2f;						// 0,01
	const float	EPSILON					= 1e-4f;						// 0,0001
	const float	BIGEPSILON				= 1e-6f;						// 0,000001
	const float	CLIPEPSILON				= 1e-4f;						// 0,0001
	const float	CSGEPSILON				= 1e-4f;						// 0,0001


	#define R_ToRadian(degree)			(degree * Real::PIOVER180)
	#define R_ToDegree(radian)			(radian * Real::PIDIV180)


#if 1
	#define	R_IsEqual(f1, f2)			(Real::Abs((f1) - (f2)) < Real::EPSILON)
#else
	#define	R_IsEqual(f1, f2)			((f2) - Real::EPSILON < (f1) && (f1) < (f2) + Real::EPSILON) // NOTE: undesirable macro "repeat" effect!
#endif // END R_IsEqual()

#if 1
	#define	R_IsEqualEps(f1, f2, fEps)	(Real::Abs((f1) - (f2)) < (fEps))
#else
	#define	R_IsEqualEps(f1, f2, fEps)	((f2) - (fEps) < (f1) && (f1) < (f2) + (fEps))	// NOTE: undesirable macro "repeat" effect!
#endif // END R_IsEqualEps()

#if 1
	#define	R_IsZero(f)					(Real::Abs(f) < Real::EPSILON)
#else
	#define	R_IsZero(f)					(-Real::EPSILON < (f) && (f) < Real::EPSILON)	// NOTE: undesirable macro "repeat" effect!
#endif // END R_IsZero()

#if 1
	#define	R_IsZeroEps(f, fEps)		(Real::Abs(f) < (fEps))
#else
	#define	R_IsZeroEps(f, fEps)		(-(fEps) < (f) && (f) < (fEps))		// NOTE: undesirable macro "repeat" effect!
#endif // END R_IsZeroEps()


	// reinterpret float bits as int
	#define FloatAsInt32(f)			(*reinterpret_cast<const long*>(&(f)))
	// reinterpret int bits as float
	#define Int32AsFloat(i)			(*reinterpret_cast<const float*>(&(i)))
	// reinterpret double bits as int64
	#define DoubleAsInt64(f)		(*reinterpret_cast<const long long*>(&(f)))
	// reinterpret int64 bits as double
	#define Int64AsDouble(i)		(*reinterpret_cast<const double*>(&(i)))

	// return 0 if positive or 1 if negative float value (based on the sign of the float)
	#define FloatExtractSign(f)		(FloatAsInt32(f) >> 31)
	#define DoubleExtractSign(f)	(DoubleAsInt64(f) >> 63)
	// return 0 if positive or 1 if negative float value (get float sign bit)
	#define FloatIsNegative(f)		FloatExtractSign(f)
	#define DoubleIsNegative(f)		DoubleExtractSign(f)
	// return 1 if positive or 0 if negative float value (inverse float sign bit)
	#define FloatIsPositive(f)		(!FloatExtractSign(f))
	#define DoubleIsPositive(f)		(!DoubleExtractSign(f))
	// extract the 8 bits of exponent as a signed integer by masking out this bits,
	// shifting down by 23, and subtracting the bias value of 127
	#define FloatGetExponent(f)		(((FloatAsInt32(f) & 0x7fffffff) >> 23) - 127)
	// return 0 or 1 based on the sign of the exponent
	#define FloatGetExponentSign(f)	(FloatGetExponent(f) >> 31)
	// get the 23 bits of mantissa without the implied bit
	#define FloatGetPureMantissa(f)	(FloatAsInt32(f) & 0x7fffff)
	// get the 23 bits of mantissa with the implied bit replaced
	#define FloatGetMantissa(f)		(FloatGetPureMantissa(f) | (1 << 23))
	// flipSign is a helper macro to invert the sign of i if flip = -1, if flip = 0, it does nothing
	#define RealFlipSign(i, flip)	((i ^ flip) - flip)
	#define FloatOneBits			0x3F800000


	//-----------------------------------------------------------------------------
	//	FUNCTION:	IsEqual()
	//	PURPOSE:	Equal real values with epsilon
	//	INPUT:		Two real value
	//	OUTPUT:		Boolean result
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline bool IsEqual(_Real fVal1, _Real fVal2)
		{
			return Abs(fVal1 - fVal2) < EPSILON;
			//return fVal2 - EPSILON < fVal1 && fVal1 < fVal2 + EPSILON;
		}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	IsEqual()
	//	PURPOSE:	Equal real values with user defined epsilon
	//	INPUT:		Two real value & user defined epsilon
	//	OUTPUT:		Boolean result
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline bool IsEqual(_Real fVal1, _Real fVal2, _Real fEpsilon)
		{
			return Abs((fVal1) - (fVal2)) < fEpsilon;
			//return fVal2 - fEpsilon < fVal1 && fVal1 < fVal2 + fEpsilon;
		}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	IsZero()
	//	PURPOSE:	Check zero real values with epsilon
	//	INPUT:		Real value
	//	OUTPUT:		Boolean result
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline bool IsZero(_Real fVal)
		{
			return Abs(fVal) < EPSILON;
			// return -EPSILON < fVal && fVal < EPSILON;
		}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	IsZero()
	//	PURPOSE:	Check zero real values with user defined epsilon
	//	INPUT:		Real value
	//	OUTPUT:		Boolean result
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline bool IsZero(_Real fVal, _Real fEpsilon)
		{
			return Abs(fVal) < fEpsilon;
			// return -fEpsilon < fVal && fVal < fEpsilon;
		}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	ToRadian()
	//	PURPOSE:	Convert degree value to radian
	//	INPUT:		Degree real value
	//	OUTPUT:		Radian
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline _Real ToRadian(_Real fDegree)
		{
			return fDegree * PIOVER180;
		}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	ToDegree()
	//	PURPOSE:	Convert radian value to degree
	//	INPUT:		Radian real value
	//	OUTPUT:		Degree
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline _Real ToDegree(_Real fRadian)
	{
		return fRadian * PIDIV180;
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Abs()
	//	PURPOSE:	Fast analogue fabs()
	//	INPUT:		Real value
	//	OUTPUT:		Absolute real value
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline _Real Abs(_Real fVal)
		{
			return fVal < 0 ? -fVal : fVal;
		}

	template<>
		inline float Abs(float fVal)
		{
			long iVal = FloatAsInt32(fVal) & 0x7fffffff;
			return Int32AsFloat(iVal);
		}

	template<>
		inline double Abs(double fVal)
		{
		#if 0
			long long iVal = DoubleAsInt64(fVal) & 0x7fffffff00000000i64;
			return Int64AsDouble(iVal);
		#else
			union {
				double	asDouble;
				long	asLong[sizeof(double) / sizeof(long)];
			};

			asDouble = fVal;
			asLong[sizeof(double) / sizeof(long) - 1] &= 0x7fffffff;	// clear 31-th bit
			return asDouble;
		#endif
		}

	//-----------------------------------------------------------------------------
	//	FUNCTION:	IsSameSign()
	//	PURPOSE:	Check that both real values has a same sign
	//				(SIGN(f1) == SIGN(f2))
	//	INPUT:		Two real values
	//	OUTPUT:		Boolean result
	//-----------------------------------------------------------------------------
	template<typename _Real>
		inline bool IsSameSign(_Real fVal1, _Real fVal2)
		{
			return (fVal1 > 0) - (fVal1 < 0) == (fVal2 > 0) - (fVal2 < 0);
		}

	template<>
		inline bool IsSameSign(float fVal1, float fVal2)
		{
			return FloatExtractSign(fVal1) == FloatExtractSign(fVal2);
		}

	template<>
		inline bool IsSameSign(double fVal1, double fVal2)
		{
		#if 0
			return DoubleExtractSign(fVal1) == DoubleExtractSign(fVal2);
		#else
			union {
				double	asDouble[2];
				long	asLong[2][sizeof(double) / sizeof(long)];
			};

			asDouble[0] = fVal1;
			asDouble[1] = fVal2;
			return	asLong[0][sizeof(double) / sizeof(long) - 1] >> 31 ==
					asLong[1][sizeof(double) / sizeof(long) - 1] >> 31;
		#endif
		}

	//-----------------------------------------------------------------------------
	//	FUNCTION:	ClampPositive()
	//	PURPOSE:	Clamp value that if the value is positive, set it to zero
	//				(fVal < 0 ? fVal : 0)
	//	INPUT:		Real value
	//	OUTPUT:		Clamped real value
	//-----------------------------------------------------------------------------
	template <typename _Real>
		inline _Real ClampPositive(_Real fVal)
		{
			return fVal < 0 ? fVal : 0;
		}

	template<>
		inline float ClampPositive(float fVal)
		{
			long iVal		= FloatAsInt32(fVal);
			long iSignMask	= ~FloatExtractSign(fVal);
			iVal &= iSignMask;
			return Int32AsFloat(iVal);
		}

	template<>
		inline double ClampPositive(double fVal)
		{
		#if 0
			long long iVal		= DoubleAsInt64(fVal);
			long long iSignMask	= ~DoubleExtractSign(fVal);
			iVal &= iSignMask;
			return Int64AsDouble(iVal);
		#else
			union {
				double	asDouble;
				long	asLong[sizeof(double) / sizeof(long)];
			};

			asDouble = fVal;
			long iSignMask	= ~(asLong[sizeof(double) / sizeof(long) - 1] >> 31);
			asLong[sizeof(double) / sizeof(long) - 1] &= iSignMask;
			return asDouble;
		#endif
		}

	//-----------------------------------------------------------------------------
	//	FUNCTION:	ClampNegative()
	//	PURPOSE:	Clamp value that if the value is negative, set it to zero
	//				(fVal > 0 ? fVal : 0)
	//	INPUT:		Real value
	//	OUTPUT:		Clamped real value
	//-----------------------------------------------------------------------------
	template <typename _Real>
		inline _Real ClampNegative(_Real fVal)
		{
			return fVal > 0 ? fVal : 0;
		}

	template<>
		inline float ClampNegative(float fVal)
		{
			long iVal		= FloatAsInt32(fVal);
			long iSignMask	= FloatExtractSign(fVal);
			iVal &= iSignMask;
			return Int32AsFloat(iVal);
		}

	template<>
		inline double ClampNegative(double fVal)
		{
		#if 0
			long long iVal		= DoubleAsInt64(fVal);
			long long iSignMask	= DoubleExtractSign(fVal);
			iVal &= iSignMask;
			return Int64AsDouble(iVal);
		#else
			union {
				double	asDouble;
				long	asLong[sizeof(double) / sizeof(long)];
			};

			asDouble = fVal;
			asLong[sizeof(double) / sizeof(long) - 1] &= asLong[sizeof(double) / sizeof(long) - 1] >> 31;
			return asDouble;
		#endif
		}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	ClampUnitSize()
	//	PURPOSE:	Clamp value in range [-1.0 - 1.0]
	//				(fVal < -1 ? -1 : fVal > 1 ? 1 : fVal)
	//	INPUT:		Real value
	//	OUTPUT:		Clamped real value
	//-----------------------------------------------------------------------------
	inline float ClampUnitSize(float fVal)
	{
		long iVal = FloatAsInt32(fVal);
		long iAbs = iVal & 0x7fffffff;
		iAbs -= (127<<23);
		iAbs >>= 31;
		long iOne = (127 << 23) & ~iAbs;
		iVal = iOne + (iVal & iAbs);
		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	ClampBelowOne()
	//	PURPOSE:	Clamp value in range [... - 1.0]
	//				(fVal > 1 ? 1 : fVal)
	//	INPUT:		Real value
	//	OUTPUT:		Clamped real value
	//-----------------------------------------------------------------------------
	inline float ClampBelowOne(float fVal)
	{
		long iVal	= FloatAsInt32(fVal);
		long iMask	= (~FloatExtractSign(fVal)) & 0x7fffffff;
		long iNew	= iVal & iMask;
		iNew -= (127<<23);
		iNew >>= 31;
		long iOne = (127<<23) & ~iNew;
		iVal = iOne + (iVal & iNew);
		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Trim()
	//	PURPOSE:	Rounds the input value to the desired precision.
	//				Precision is stated as the number of bits used to store the
	//				fractional value. Fractional bits are negative exponents,
	//				1 bit equals a precision of half a unit (0.5f),
	//				2 bits equals one quarter (0.25f), etc.
	//	INPUT:		Real value and precision
	//	OUTPUT:		Trimmed real value
	//-----------------------------------------------------------------------------
	inline float Trim(float fVal, long iPrecision)
	{
		long  iBias	= 23 - FloatGetExponent(fVal) - iPrecision;

		if (iBias < 1)
			return fVal;
		else if (iBias > 24)
			return 0.0f;

		long iVal = FloatAsInt32(fVal);
		if (iBias == 24)
		{
			iVal &= 1 << 31;
			iVal += (127 - iPrecision) << 23;
		}
		else
		{
			_asm {
				clc
				mov ecx, iBias
				mov eax, iVal
				shr eax, cl
				adc eax, 0
				shl eax, cl
				mov iVal, eax
			};
		}

		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	IsPowerOfTwo()
	//	PURPOSE:	Returns true if the input value is a power-of-teo (1,2,4,8,16, etc..)
	//	INPUT:		Real value
	//	OUTPUT:		Boolean result
	//-----------------------------------------------------------------------------
	inline bool IsPowerOfTwo(float fVal)
	{
		// For floating-point values, we know the value is a power-of-two if
		// the mantissa is zero (not including the implied bit)
		return !FloatGetMantissa(fVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	NearestPowerOfTwo()
	//	PURPOSE:	Rounds the input value to the nearest power-of-two. 
	//	INPUT:		Real value
	//	OUTPUT:		Real rounded result
	//	REMARKS:	All values below 1.0f generate a result of 1.0f
	//-----------------------------------------------------------------------------
	inline float NearestPowerOfTwo(float fVal)
	{
		// Convert the value to an integer
		long iVal = FloatAsInt32(fVal);
		// If the value is negative, or less than 1.0f, return 1.0f
		// this mask test for the sign bit and the exponents sign in one step
		if (iVal & 0xc0000000)
			return 1.0f;
		// If anything is in the high bit of the mantissa, 
		// use it to add one to the exponent
		iVal += (iVal & 0x800000) << 1;
		// Trim away the mantissa
		iVal &= ~((1 << 23) - 1);
		// Convert back to floating-point as we return
		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	FloorPowerOfTwo()
	//	PURPOSE:	Rounds the next-least power-of-two value. 
	//	INPUT:		Real value
	//	OUTPUT:		Real rounded result
	//	REMARKS:	All values below 1.0f generate a result of 1.0f
	//-----------------------------------------------------------------------------
	inline float FloorPowerOfTwo(float fVal)
	{
		// Convert the value to an integer
		long iVal = FloatAsInt32(fVal);
		// If the value is negative, or less than 1.0f, return 1.0f
		// this mask test for the sign bit and the exponents sign in one step
		if (iVal & 0xc0000000)
			return 1.0f;
		// Trim away the mantissa
		iVal &= ~((1 << 23) - 1);
		// Convert back to floating-point as we return
		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	CeilPowerOfTwo()
	//	PURPOSE:	Rounds the next-highest power-of-two value.
	//	INPUT:		Real value
	//	OUTPUT:		Real rounded result
	//	REMARKS:	All values below 1.0f generate a result of 1.0f
	//-----------------------------------------------------------------------------
	inline float CeilPowerOfTwo(float fVal)
	{
		// Convert the value to an integer
		long iVal = FloatAsInt32(fVal);
		// If the value is negative, or less than 1.0f, return 1.0f
		// this mask test for the sign bit and the exponents sign in one step
		if (iVal & 0xc0000000)
			return 1.0f;
		// If anything is in the mantissa, round up
		iVal += 0x7fffff;
		// Trim away the mantissa
		iVal &= ~((1 << 23) - 1);
		// Convert back to floating-point as we return
		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	TruncateToPowerOfTwo()
	//	PURPOSE:	Truncate to nearest pow-of-two value
	//	INPUT:		Real value
	//	OUTPUT:		Real result
	//-----------------------------------------------------------------------------
	inline float TruncateToPowerOfTwo(float fVal)
	{
		long iVal = FloatAsInt32(fVal);
		iVal &= ~((1 << 23) - 1);
		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	ToInteger()
	//	PURPOSE:	Convert a float value to integer, using the default rounding method set on the FPU.
	//	INPUT:		Real value
	//	OUTPUT:		Integer result
	//-----------------------------------------------------------------------------
	inline long ToInteger(float fVal)
	{
		long iVal;
		__asm fld		DWORD PTR fVal
		__asm fistp		DWORD PTR iVal
		return iVal;
	}

	inline long ToInteger(double fVal)
	{
		long iVal;
		__asm fld		QWORD PTR fVal
		__asm fistp		DWORD PTR iVal
		return iVal;
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Chop()
	//	PURPOSE:	Convert a real value to integer, all fractional values are truncated.
	//				(Chop(2.35) = 2; Chop(-2.35) = -2)
	//	INPUT:		Real value
	//	OUTPUT:		Integer result
	//-----------------------------------------------------------------------------
	inline long Chop(float fVal)
	{
		// Read the exponent and decide how much we need to shift the mantissa down
		long iShift = 23 - FloatGetExponent(fVal);
		// Read the mantissa and shift it down to remove all fractional values
		long iVal = FloatGetMantissa(fVal) >> iShift;
		// Set the sign of the new result
		iVal = RealFlipSign(iVal, FloatExtractSign(fVal));
		// If the exponent was negative, (-1<fVal<1) we must return zero
		iVal &= ~FloatGetExponentSign(fVal);
		// Return the result
		return iVal;                  
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Floor()
	//	PURPOSE:	Convert a real value to the next-lowest integer value.
	//				(Floor(2.35) = 2; Floor(-2.35) = -3)
	//	INPUT:		Real value
	//	OUTPUT:		Integer result
	//-----------------------------------------------------------------------------
	inline long Floor(float fVal)
	{
		// Read the exponent and decide how much we need to shift the mantissa down
		long iShift = 23 - FloatGetExponent(fVal);
		// Read the mantissa and shift it down to remove all fractional values
		long iVal = FloatGetMantissa(fVal) >> iShift;
		// Set the sign of the new result
		iVal = RealFlipSign(iVal, FloatExtractSign(fVal));
		// If the exponent was negative, (-1<fVal<1) we must return zero
		iVal &= ~FloatGetExponentSign(fVal);
		// If the original value is negative,
		// and any fractional values are present -- decrement the result by one
		iVal -= FloatExtractSign(fVal) &&
				(FloatGetExponentSign(fVal) || (FloatGetPureMantissa(fVal) & ((1 << iShift) - 1)));
		return iVal;       
	}


	//-------------------------------------------------------------------
	//	FUNCTION:	Floor()
	//	PURPOSE:	Floor real value to near val
	//	INPUT:		Real value & bound
	//-------------------------------------------------------------------
	inline long Floor(float fVal, long iBound)
	{
		return Floor(fVal / iBound + 0.5f) * iBound;
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Ceil()
	//	PURPOSE:	Convert a float value to the next-lowest integer value.
	//				Ceil(2.35) = 3; Ceil(-2.35) = -2
	//	INPUT:		Real value
	//	OUTPUT:		Integer result
	//-----------------------------------------------------------------------------
	inline long Ceil(float fVal)
	{
		// Read the exponent and decide how much we need to shift the mantissa down
		long iShift = 23 - FloatGetExponent(fVal);
		// Read the mantissa and shift it down to remove all fractional values
		long iVal = FloatGetMantissa(fVal) >> iShift;
		// Set the sign of the new result
		iVal = RealFlipSign(iVal, FloatExtractSign(fVal));
		// If the exponent was negative, (-1<fVal<1) we must return zero
		iVal &= ~FloatGetExponentSign(fVal);
		// If the original value is positive and not zero,
		// and any fractional values are present -- increment the result by one
		iVal += FloatIsPositive(fVal) && FloatAsInt32(fVal) &&
				(FloatGetExponentSign(fVal) || (FloatGetPureMantissa(fVal) & ((1 << iShift) - 1)));
		return iVal;       
	}


	//-------------------------------------------------------------------
	//	FUNCTION:	Ceil()
	//	PURPOSE:	Ceil real value to near val
	//	INPUT:		Real value & bound
	//-------------------------------------------------------------------
	inline long Ceil(float fVal, long iBound)
	{
		return Ceil(fVal / iBound - 0.5f) * iBound;
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Inverse()
	//	PURPOSE:	Calculates the inverse of a given value (1/value).
	//				approximatly 3 times faster than doing the divide yourself
	//	INPUT:		Real value
	//	OUTPUT:		Inverse real result
	//	REMARKS:	This approximation gets innacurate with small values.
	//				For the test case of 10000 values iterated between -5.0f and 5.0f,
	//				the worst deviation was the source value -0.000000126 creating a result of
	//				-7916990.5 instead of -7910778.5 (an error of 6212). But for reasonably small values
	//				like 0.0001, the approximate error is less than 10.
	//-----------------------------------------------------------------------------
	inline float Inverse(float fVal)
	{
		long iVal = 2 * FloatOneBits - FloatAsInt32(fVal);                                 
		float r = Int32AsFloat(iVal);                                                       
		return FloatAsInt32(fVal) ? (r * (2.0f - (fVal) * r)) : 0;  
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Sqrt()
	//	PURPOSE:	Fast square root operation
	//	INPUT:		Real value
	//	OUTPUT:		Square root result
	//	REMARKS:	Uses Jim Blinn's IEEE sqaure root method.
	//				Approximatly 10 times faster than sqrt().
	//				For the test case of 10000 values iterated between -5.0f and 5.0f,
	//				the worst deviation was 0.08 for the source value 2.0
	//-----------------------------------------------------------------------------
	inline float Sqrt(float fVal)
	{
		long iVal = (FloatAsInt32(fVal) >> 1) + (FloatOneBits >> 1);
		return Int32AsFloat(iVal);
	}


	//-----------------------------------------------------------------------------
	//	FUNCTION:	Weight()
	//	PURPOSE:	Compute squared ilinnearizated weigth with distance effect,
	//				invert and clamp to positive unit range
	//	INPUT:		Real value, min and max bounds
	//	OUTPUT:		Squared ilinnearizated weigth
	//-----------------------------------------------------------------------------
	template<typename _Real>
		_Real Weight(_Real fValue, _Real fMin, _Real fMax)
		{
			_Real fWeight = 0;

			if (fValue >= fMin && fValue <= fMax)
			{
				// Convert result in range {0 - 1} with distance effect
				fWeight = ((fValue - fMin) / (fMax - fMin) - 0.5f) * 2.0f;
				// Square for ilinnearization
				fWeight *= fWeight;
				// Invert result and clamp to positive unit range
				fWeight = ::clamp<_Real>(1.0f - ::abs(fWeight), 0.001f, 1.0f);
			}

			return fWeight;
		}


	//-------------------------------------------------------------------
	//	FUNCTION:	SinCos()
	//	PURPOSE:	Getting sin & cos via FPU cmd "sincos"
	//	INPUT:		Angle in radians, two destination storage
	//	OUTPUT:		sin & cos
	//-------------------------------------------------------------------
	inline void SinCos(float fRadian, float *pfSin, float *pfCos)
	{
		__asm {
			mov		ecx, DWORD PTR pfSin
			mov		edx, DWORD PTR pfCos
			fld			 DWORD PTR fRadian
			fsincos
			fstp		 DWORD PTR [edx]
			fstp		 DWORD PTR [ecx]
		};
	}

	inline void SinCos(double fRadian, double *pfSin, double *pfCos)
	{
		__asm {
			mov		ecx, DWORD PTR pfSin
			mov		edx, DWORD PTR pfCos
			fld			 QWORD PTR fRadian
			fsincos
			fstp		 QWORD PTR [edx]
			fstp		 QWORD PTR [ecx]
		};
	}


	//-------------------------------------------------------------------
	//	FUNCTION:	FFastCos()
	//	PURPOSE:	Fast cosine / sine utility
	//	REMARKS:
	//				assert:  0 <= fRadian <= PI/2
	//				maximum absolute error = 1.1880e-03
	//				speedup = 2.14
	//-------------------------------------------------------------------
	template <typename _Real> inline
		_Real FFastCos(_Real fRadian)
		{
			_Real fRadianSq = fRadian * fRadian;
			_Real fVal = 3.705e-02f;
			fVal *= fRadianSq;
			fVal -= 4.967e-01f;
			fVal *= fRadianSq;
			fVal += 1;
			return fVal;
		}

	template <typename _Real> inline
		_Real FFastSin(_Real fRadian)
		{	// FFastSin(a) = FFastCos(a - PI/2)
			return FFastCos<_Real>(fRadian - Real::HALFPI);
		}


	//-------------------------------------------------------------------
	//	FUNCTION:	FastCos()
	//	PURPOSE:	Fast cosine / sine utility
	//	REMARKS:
	//				assert:  0 <= fRadian <= PI/2
	//				maximum absolute error = 2.3082e-09
	//				speedup = 1.47
	//-------------------------------------------------------------------
	template <typename _Real> inline
		_Real FastCos(_Real fRadian)
		{
			_Real fRadianSq = fRadian * fRadian;
			_Real fVal = -2.605e-07f;
			fVal *= fRadianSq;
			fVal += 2.47609e-05f;
			fVal *= fRadianSq;
			fVal -= 1.3888397e-03f;
			fVal *= fRadianSq;
			fVal += 4.16666418e-02f;
			fVal *= fRadianSq;
			fVal -= 4.999999963e-01f;
			fVal *= fRadianSq;
			fVal += 1;
			return fVal;
		}


	template <typename _Real> inline
		_Real FastSin(_Real fRadian)
		{	// FastSin(a) = FastCos(a - PI/2)
			return FastCos(fRadian - Real::HALFPI);
		}



	//-------------------------------------------------------------------
	//	FUNCTION:	RandomUnit()
	//	PURPOSE:	Random float generator
	//	RETURN:		float value in range [0.0f ... 1.0f]
	//-------------------------------------------------------------------
	template <typename _Real> inline
		_Real RandomUnit();

	template <> inline
		float RandomUnit()
		{
			long temp = 0x3f800000 | (0x007fffff & Integer::Random());
			return reinterpret_cast<float&>(temp) - 1.0f;
			// return Integer::Random() * 2.3283064365386963e-10f; // 1/2^32
		}

	template <> inline
		double RandomUnit()
		{
		#if 0
			static long long iran;
			iran = 1664525 * iran + 1013904223;
			long long temp = 0x3ff0000000000000i64 | (0x007fffff00000000i64 & iran);
			return reinterpret_cast<double&>(temp) - 1.0;
		#else
			long temp[2] = { 0x3ff00000 | 0x007fffff, 0x3ff00000 | (0x007fffff & Integer::Random()) };
			return reinterpret_cast<double&>(temp) - 1.0;
			// return Integer::Random() * 2.3283064365386963e-10; // 1/2^32
		#endif
		}



	//-------------------------------------------------------------------
	//	FUNCTION:	RandomNormalized()
	//	PURPOSE:	Random normalized float generator
	//	RETURN:		normalized float value in range [-1.0f ... 1.0f]
	//-------------------------------------------------------------------
	template <typename _Real> inline
		_Real RandomNormalized()
		{
			return RandomUnit<_Real>() * 2.0f - 1.0f;
			//return (Integer::Random() * 2.3283064365386963e-10) * 2.0f - 1.0f;
		}


	//-------------------------------------------------------------------
	//	FUNCTION:	Random()
	//	PURPOSE:	Random float generator
	//	RETURN:		float value in range [-FLT_MAX ... FLT_MAX]
	//-------------------------------------------------------------------
	template <typename _Real> inline
		_Real Random()
		{
			return RandomNormalized<_Real>() * FLT_MAX;
		}


	//-------------------------------------------------------------------
	//	FUNCTION:	Random()
	//	PURPOSE:	Random float rang generator
	//	RETURN:		float value in range [fMin ... fMax]
	//-------------------------------------------------------------------
	template <typename _Real> inline
		_Real Random(_Real fMin, _Real fMax)
		{
			return fMin + (fMax - fMin) * RandomUnit<_Real>();
		}


	//-------------------------------------------------------------------
	//	FUNCTION:	GaussPair()
	//	PURPOSE:	Generatres a gaussian random number using
	//				the polar form of a Box-Muller transformation
	//	RETURN:		gaussian random pair
	//	REMARKS:	The polar form of the Box-Muller transformation
	// 				is capable of generating two random gauss values
	// 				in one pass.
	//-------------------------------------------------------------------
	template <typename _Real> inline
		void GaussPair(_Real *pfA, _Real *pfB, _Real fMean, _Real fStdDeviation)
		{
			_Real x1, x2, w, y1, y2;

			do {
				x1 = RandomNormalized<_Real>();
				x2 = RandomNormalized<_Real>();
				w = x1 * x1 + x2 * x2;
			} while (w >= 1.0f);

			w = ::sqrt((-2.0f * ::log(w)) / w);
			y1 = x1 * w;
			y2 = x2 * w;

			*pfA = fMean + y1 * fStdDeviation;
			*pfB = fMean + y2 * fStdDeviation;
		}

	template <typename _Real> inline
		void GaussPair(_Real *pfA, _Real *pfB)
		{
			_Real x1, x2, w;

			do {
				x1 = RandomNormalized<_Real>();
				x2 = RandomNormalized<_Real>();
				w = x1 * x1 + x2 * x2;
			} while (w >= 1.0f);

			w = ::sqrt((-2.0f * ::log(w)) / w);
			*pfA = x1 * w;
			*pfB = x2 * w;
		}


	//-------------------------------------------------------------------
	//	FUNCTION:	RandomVector()
	//	PURPOSE:	Getting random vector with needed length
	//	RETURN:		Random vector
	//-------------------------------------------------------------------
	inline D3DXVECTOR3 RandomVector(FLOAT fNeedLen = 1.0F)
	{
		for (; ;)
		{
			D3DXVECTOR3	v(RandomNormalized<FLOAT>(), RandomNormalized<FLOAT>(), RandomNormalized<FLOAT>());
			if (::D3DXVec3LengthSq(&v) < 0.0001F)
				continue;

			v *= fNeedLen / ::D3DXVec3Length(&v);
			return v;
		}
	}

	inline D3DXVECTOR3 RandomVector(CONST D3DXVECTOR3& vSpotDir, FLOAT fSpotCutoff,
											FLOAT fNeedLen = 1.0F)
	{	// ??? ??? ???
		float fSpotCos = Random(fSpotCutoff, 1.0F);
		float A = RandomNormalized<FLOAT>();
		float B = RandomNormalized<FLOAT>();
		float C = 1.0F - A - B;

		D3DXVECTOR3 v(fSpotCos * A / vSpotDir.x, fSpotCos * B / vSpotDir.y, fSpotCos * C / vSpotDir.z);
		v *= fNeedLen / ::D3DXVec3Length(&v);
		return v;
	}


	//-------------------------------------------------------------------
	//	FUNCTION:	SignBit()
	//	PURPOSE:	Getting real sign bit (31-th bit if float)
	//	RETURN:		Sign bit (0 - fVal is positive, 1 - fVal is negative)
	//-------------------------------------------------------------------
	inline unsigned int SignBit(float fVal)
	{
		_asm {
			mov		eax, DWORD PTR fVal
			shr		eax, 31					; leave sign bit only
		};
	}

	inline unsigned int SignBit(double fVal)
	{
		_asm {
			lea		esi, fVal
			mov		eax, DWORD PTR [esi+4]
			shr		eax, 31					; leave sign bit only
		};
	}


	//-------------------------------------------------------------------
	//	FUNCTION:	SignBits()
	//	PURPOSE:	Getting real sign bit mask for vector
	//	INPUT:		3D vector
	//	RETURN:		Set 0th, 1th, 2th byte if NEGATIVE x, y, z
	//				(return SignBit(x) | SignBit(y)<<1 | SignBit(z)<<2;)
	//	REMARKS:
	//			Calculate NEGATIVE sign bits!
	//			Use xor 0x7 for invert sign bytes
	//-------------------------------------------------------------------
	inline unsigned int SignBits(const float fVec[3])
	{
	//	return
	//		(*(long *)&x & 0x80000000 ? 1 : 0) |
	//		(*(long *)&y & 0x80000000 ? 1 : 0) << 1 |
	//		(*(long *)&z & 0x80000000 ? 1 : 0) << 2;
		_asm {
			mov		esi, DWORD PTR fVec
			mov		eax, DWORD PTR [esi+8]
			mov		ecx, DWORD PTR [esi+4]
			mov		edx, DWORD PTR [esi]
			shr		eax, 31					; leave sign bit only
			shl		eax, 1
			shr		ecx, 31					; leave sign bit only
			or		eax, ecx
			shl		eax, 1
			shr		edx, 31					; leave sign bit only
			or		eax, edx
		};
	}

	inline unsigned int SignBits(const double fVec[3])
	{
		_asm {
			mov		esi, DWORD PTR fVec
			mov		eax, DWORD PTR [esi+20]
			mov		ecx, DWORD PTR [esi+12]
			mov		edx, DWORD PTR [esi+4]
			shr		eax, 31					; leave sign bit only
			shl		eax, 1
			shr		ecx, 31					; leave sign bit only
			or		eax, ecx
			shl		eax, 1
			shr		edx, 31					; leave sign bit only
			or		eax, edx
		};
	}


	//-------------------------------------------------------------------
	//	FUNCTION:	ShiftMod()
	//	INPUT:		val - value which will be clampfed off
	//				b	- bounds
	//	OUTPUT:		Clamped real value in range [-b, b]
	//	REMARKS:	Typically use:
	//					clampf texture shift in texture boundaries (-width ... width, -height ... height)
	//					(used for shifting normalize)
	//				NOTE: You may control roundoff through _control87(_MCW_RC_, <mask>).
	//-------------------------------------------------------------------
	inline float ShiftMod(float fVal, unsigned long iBound)
	{
	#if 0
		while ( fVal > iBound )
			fVal -= iBound;
		while ( fVal < 0 )
			fVal += iBound;
		return fVal;
	#else
		_asm {
			fild	DWORD PTR iBound
			fld		DWORD PTR fVal
		$cycl:
			fprem
			fnstsw	ax			; save fpu environiment
			bt		ax, 2		; check C2 bit
			jc		$cycl		; not yet finite residual
			ffree	st(1)
		};
	#endif
	}

	inline float ShiftMod(double fVal, unsigned long iBound)
	{
	#if 0
		while ( fVal > iBound )
			fVal -= iBound;
		while ( fVal < 0 )
			fVal += iBound;
		return val;
	#else
		_asm {
			fild	DWORD PTR iBound
			fld		QWORD PTR fVal
		$cycl:
			fprem
			fnstsw	ax			; save fpu environiment
			bt		ax, 2		; check C2 bit
			jc		$cycl		; not yet finite residual
			ffree	st(1)
		};
	#endif
	}


	//-------------------------------------------------------------------
	//	FUNCTION:	IsNaN()
	//	PURPOSE:	Check that value has right IEEE format
	//	INPUT:		Tested real value
	//-------------------------------------------------------------------
	inline bool IsNaN(float fVal)
	{
		return ((*(long *)&fVal) & 0x7f800000L) == 0x7f800000L;
	}

} // END namespace Real





//*****************************************************************************
//							BBOX INTERSECTION UTILS
//*****************************************************************************

namespace MinMax
{
	// AB intersect CD?
	template<class _Ty> inline
		bool IsCross(_Ty _A, _Ty _B, _Ty _C, _Ty _D) 
		{
			return !(_A > _D || _B < _C));
		}

	// AB intersect CD(+ check delta correction)?
	template<class _Ty> inline
		bool IsCross(_Ty _A, _Ty _B, _Ty _C, _Ty _D, _Ty _Delta)
		{
			return !(_A - _Delta > _D || _B + _Delta < _C);
		}

	// AB strong (contact not considered as intersection!) intersect CD?
	template<class _Ty> inline
		bool IsStrongCross(_Ty _A, _Ty _B, _Ty _C, _Ty _D)
		{
			return !(_A >= _D || _B <= _C));
		}

	// AB strong (contact not considered as intersection!) intersect CD(+ check delta correction)?
	template<class _Ty> inline
		bool IsStrongCross(_Ty _A, _Ty _B, _Ty _C, _Ty _D, _Ty _Delta)
		{
			return !(_A - _Delta >= _D || _B + _Delta <= _C);
		}

	// full version
	template<class _Ty> inline
		bool IsCross(_Ty _A, _Ty _B, _Ty _C, _Ty _D, _Ty _Delta, bool _Strong)
		{
			return _Strong ?
					_Delta ?
						!(_A - _Delta >= _D || _B + _Delta <= _C) :
						!(_A >= _D || _B <= _C)) :
					_Delta ?
						!(_A - _Delta > _D || _B + _Delta < _C) :
						!(_A > _D || _B < _C));
		}


	// AB inside CD?
	template<class _Ty> inline
		bool IsInside(_Ty _A, _Ty _B, _Ty _C, _Ty _D)
		{
			return _A >= _C && _B <= _D;
		}

	// AB inside CD(+ check _Delta correction)?
	template<class _Ty> inline
		bool IsInside(_Ty _A, _Ty _B, _Ty _C, _Ty _D, _Ty _Delta)
		{
			return _A - _Delta >= _C && _B + _Delta <= _D;
		}

	// AB strong (does not concern from within!) inside CD?
	template<class _Ty> inline
		bool IsStrongInside(_Ty _A, _Ty _B, _Ty _C, _Ty _D)
		{
			return _A > _C && _B < _D;
		}

	// AB strong (does not concern from within!) inside CD(+ check delta correction)?
	template<class _Ty> inline
		bool IsStrongInside(_Ty _A, _Ty _B, _Ty _C, _Ty _D, _Ty _Delta)
		{
			return _A - _Delta > _C && _B + _Delta < _D;
		}

	// full version
	template<class _Ty> inline
		bool IsInside(_Ty _A, _Ty _B, _Ty _C, _Ty _D, _Ty _Delta, bool _Strong)
		{
			return _Strong ?
					_Delta ?
						_A - _Delta > _C && _B + _Delta < _D :
						_A > _C && _B < _D :
					_Delta ?
						_A - _Delta >= _C && _B + _Delta <= _D : 
						_A >= _C && _B <= _D;
		}

	// X inside AB?
	template<class _Ty> inline
		bool IsInside(_Ty _X, _Ty _A, _Ty _B)
		{
			return _X >= _A && _X <= _B;
		}

	// X strong (does not concern from within!) inside AB?
	template<class _Ty> inline
		bool IsStrongInside(_Ty _X, _Ty _A, _Ty _B)
		{
			return _X > _A && _X < _B;
		}

	template<class _Ty> inline
		bool IsInside(_Ty _X, _Ty _A, _Ty _B, bool _Strong)
		{
			return _Strong ? _X > _A && _X < _B : _X >= _A && _X <= _B;
		}

} // END namespace MinMax





//---------------------------------------------------------
//	Helper function to stuff a FLOAT into a DWORD argument
//---------------------------------------------------------

inline DWORD FtoDW(FLOAT f) { return *(DWORD*)&f; }



//*****************************************************************************
//								3D MATH  UTILS
//*****************************************************************************


//-----------------------------------------------------------------------------
//	Name:	BaryCentricFromTriangle
//	Desc:	Getting barycentric coordinates (f, g) for known point by triangle
//	Arg:	pV1		-- [in] Pointer to a source D3DXVECTOR3 structure
//							that is the n-th vertex of trinagle
//			pV2		-- [in] Pointer to a source D3DXVECTOR3 structure
//							that is the n-th vertex of trinagle
//			pV3		-- [in] Pointer to a source D3DXVECTOR3 structure
//							that is the n-th vertex of trinagle
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
	FLOAT *pF, FLOAT *pG
);



//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
D3DXMATRIX * MatrixPairPointTransformation(
	D3DXMATRIX *pOut,
	CONST D3DXVECTOR3 *pO1, CONST D3DXVECTOR3 *pV1,
	CONST D3DXVECTOR3 *pO2, CONST D3DXVECTOR3 *pV2
);


//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
D3DXMATRIX * MatrixPairPointTransformation(
	D3DXMATRIX *pOut,
	CONST D3DXVECTOR3 *pO1, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pW1,
	CONST D3DXVECTOR3 *pO2, CONST D3DXVECTOR3 *pV2, CONST D3DXVECTOR3 *pW2
);

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
D3DXMATRIX * MatrixPairPointTransformation(
	D3DXMATRIX *pOut,
	CONST D3DXVECTOR3 *pO1, CONST D3DXVECTOR3 *pV1,
	CONST D3DXVECTOR3 *pW1, CONST D3DXVECTOR3 *pU1,
	CONST D3DXVECTOR3 *pO2, CONST D3DXVECTOR3 *pV2,
	CONST D3DXVECTOR3 *pW2, CONST D3DXVECTOR3 *pU2
);


//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
D3DXMATRIX * ClipProjectionLH(
	D3DXMATRIX			*pOut,
	CONST D3DXMATRIX	*pmatProjection,
	CONST D3DXVECTOR4	*pClipPlane
);



//-----------------------------------------------------------------------------
//	Name:	DrawQube
//	Desc:	Draw simple solid qube (analog glutSolidQube()...)
//	Arg:	pd3dDevce		-- [in] current rendering device
//			Size			-- [in] side size
//			pSideTextures	-- [in] six texture for each qube side
//			Color			-- [in] DWORD color
//
//	Remarks:
//-----------------------------------------------------------------------------
void DrawQube(LPDIRECT3DDEVICE9 pd3dDevce, FLOAT Size,
			  LPDIRECT3DTEXTURE9 pSideTextures[6]);
void DrawQube(LPDIRECT3DDEVICE9 pd3dDevce, FLOAT Size,
			  D3DCOLOR Color);



//-----------------------------------------------------------------------------
//	Name:	DrawParallelepiped
//	Desc:	Draw simple solid parallelepiped
//	Arg:	pd3dDevce		-- [in] current rendering device
//			pSize			-- [in] NOTE: HALF size of parallelepiped (WxHxD)
//			pSideTextures	-- [in] six texture for each parallelepiped side
//			Color			-- [in] DWORD color
//
//	Remarks:
//-----------------------------------------------------------------------------
void DrawParallelepiped(LPDIRECT3DDEVICE9 pD3DDevice, CONST D3DXVECTOR3& vSize,
						D3DCOLOR dwColor);


HRESULT DrawCSAxis(LPDIRECT3DDEVICE9 pd3dDevce, FLOAT Length);


//-----------------------------------------------------------------------------
//	Name:	CreateSingleStripGridIB
//	Desc:	Create triangle strip index buffer for addressing vertexes in mesh knoth
//	Arg:	pd3dDevce		-- [in] current rendering device
//			nXVerts			-- [in] width of mesh
//			nYVerts			-- [in] height of mesh
//			XStep			-- [in] width step size
//			YStep			-- [in] height step size
//			Stride			-- [in] 
//			Flags			-- [in] index buffer creation flags
//			mpIndexBufffer	-- [out] pointer to index buffer
//
//	Remarks:
//-----------------------------------------------------------------------------
HRESULT CreateSingleStripGridIB(
	LPDIRECT3DDEVICE9 pD3DDevice,
	WORD nXVerts, WORD nYVerts,
	WORD XStep, WORD YStep, WORD Stride,
	LPDIRECT3DINDEXBUFFER9 *ppIndexBuffer
);




/*
 *	D3DXMESHCONTAINEREX (D3DXMESHCONTAINER extended version)
 *
 *	Added:
 *		-- construction / destruction
 *		-- textures pointers array
 *		-- skinned mesh animation data
 *		-- bone matrices array
 */
typedef struct D3DXMESHCONTAINEREX : public D3DXMESHCONTAINER
{
	// Construct

	D3DXMESHCONTAINEREX ()
		: ppTextures(0)
		, ppFrameMatrices(0)
		, pBoneMatrices(0)
		, pSkinMesh(0)
	{
		Name				= 0;
		MeshData.pMesh		= 0;
		pMaterials			= 0;
		pEffects			= 0;
		NumMaterials		= 0;
		pAdjacency			= 0;
		pSkinInfo			= 0;
		pNextMeshContainer	= 0;
	}

	~D3DXMESHCONTAINEREX ()
	{	// release textures
		if (NumMaterials && ppTextures)
			for (unsigned int i = 0; i < NumMaterials; ++i)
				SAFE_RELEASE(ppTextures[i]);
		delete [] ppTextures;
		delete [] ppFrameMatrices;
		delete [] pBoneMatrices;
		// release animated 'target' mesh
		SAFE_RELEASE(pSkinMesh);

		// ..DELETE BASE OBJECT..
		delete [] Name;
		// release original 'first' mesh
		SAFE_RELEASE(MeshData.pMesh);
		delete [] pMaterials;
		delete pEffects;
		delete [] pAdjacency;
		SAFE_RELEASE(pSkinInfo);
		delete pNextMeshContainer;
	}


	// Find mesh container by name in hierarchy
	D3DXMESHCONTAINEREX * Find (LPCSTR cpMeshName)
	{	
		if (cpMeshName)
		{	// check this
			if (Name && !::strcmp(Name, cpMeshName))
				return this;
			// check next node
			if (pNextMeshContainer)
				return static_cast<LPD3DXMESHCONTAINEREX>(pNextMeshContainer)->Find(cpMeshName);
		}

		return 0;
	}

	// Update mesh
	HRESULT Update();

	// Draw mesh
	HRESULT Draw();
	// Draw all meshes in linked list
	HRESULT DrawList();
	// Draw mesh with shader
	HRESULT Draw(LPDIRECT3DVERTEXSHADER9 pShader, LPDIRECT3DVERTEXDECLARATION9 pDecl);
	// Draw all meshes in linked list with shader
	HRESULT DrawList(LPDIRECT3DVERTEXSHADER9 pShader, LPDIRECT3DVERTEXDECLARATION9 pDecl);

	// Implementation

	LPDIRECT3DTEXTURE9	* ppTextures;		// object-texture, used for skin vizualization
	LPD3DXMATRIX		* ppFrameMatrices;
	LPD3DXMATRIX		  pBoneMatrices;
	LPD3DXMESH			  pSkinMesh;		// 'second' animated skinned mesh ('target')

} D3DXMESHCONTAINEREX, *LPD3DXMESHCONTAINEREX;



/*
 *	D3DXFRAMEEX (D3DXFRAME extended version)
 *
 *	Added:
 *		-- construction / destruction
 *		-- initital frame transformation matrix
 *		-- parent`s frames combined transformation
 */
typedef struct D3DXFRAMEEX : public D3DXFRAME
{
	// Construct

	D3DXFRAMEEX ()
	{
		Name = 0;
		pMeshContainer = 0;
		pFrameSibling = pFrameFirstChild = 0;
		// reset all matrices to identity
		::D3DXMatrixIdentity(&TransformationMatrix);
		::D3DXMatrixIdentity(&CombinedMatrix);
		::D3DXMatrixIdentity(&OriginalMatrix);
	}

	~D3DXFRAMEEX ()
	{
		delete [] Name;
		delete pFrameSibling;
		delete pFrameFirstChild;
	}

	// Find frame in frame hierarhy by name
	D3DXFRAMEEX * Find (LPCSTR cpFrameName)
	{
		if (cpFrameName)
		{	// check this name
			if (Name && !::strcmp(Name, cpFrameName))
				return this;
			// find in siblings
			if (pFrameSibling)
				if (LPD3DXFRAMEEX pFrame = static_cast<LPD3DXFRAMEEX>(pFrameSibling)->Find(cpFrameName))
					return pFrame;
			// find in childs
			if (pFrameFirstChild)
				if (LPD3DXFRAMEEX pFrame = static_cast<LPD3DXFRAMEEX>(pFrameFirstChild)->Find(cpFrameName))
					return pFrame;
		}

		return 0;
	}

	// Set transform matrix to original states
	void Reset ()
	{	// copy original matrix
		TransformationMatrix = OriginalMatrix;
		// reset sibling frames
		if (pFrameSibling)
			static_cast<LPD3DXFRAMEEX>(pFrameSibling)->Reset();
		//  reset child frames
		if (pFrameFirstChild)
			static_cast<LPD3DXFRAMEEX>(pFrameFirstChild)->Reset();
	}

	// Update: combined matrix in frame hierarhy
	void UpdateHierarchy (CONST D3DXMATRIX * pNewTransformation = NULL)
	{	// apply hierarchy combined transformation
		CombinedMatrix = pNewTransformation ? TransformationMatrix * *pNewTransformation : TransformationMatrix;
		// combine with sibling frames
		if (pFrameSibling)
			static_cast<LPD3DXFRAMEEX>(pFrameSibling)->UpdateHierarchy(pNewTransformation);
		// combine with child frames
		if (pFrameFirstChild)
			static_cast<LPD3DXFRAMEEX>(pFrameFirstChild)->UpdateHierarchy(&CombinedMatrix);
	}

	// Get num frames in hierarchy
	unsigned int Count () const
	{
		return	(pFrameSibling	   ? static_cast<const LPD3DXFRAMEEX>(pFrameSibling)->Count() : 0) +
				(pFrameFirstChild  ? static_cast<const LPD3DXFRAMEEX>(pFrameFirstChild)->Count() : 0) +
				1;
	}

	// Implementation

	D3DXMATRIX	CombinedMatrix;
	D3DXMATRIX	OriginalMatrix;

} D3DXFRAMEEX, *LPD3DXFRAMEEX;



/*
 *	D3D initialization
 */

extern HRESULT InitializeDirect3D(
	LPDIRECT3D9 *ppD3D,
	LPDIRECT3DDEVICE9 *ppD3DDevice,
	HWND hWnd,
	UINT ScreenWidth = 800, UINT ScreenHeight = 600,	// window width/height
	BOOL bForceWindowed = FALSE,						// don`t ask user windowed or fullscreen
	BOOL bPureDevice = FALSE,							// don`t provide any emulation services for vertex processing
	BOOL bMultiThreaded = FALSE							// application requests Direct3D to be multithread safe ?
);


/*
 *	D3D load vertex shader
 */

extern HRESULT LoadVertexShader(
	LPDIRECT3DVERTEXSHADER9 *ppShader, 
    LPDIRECT3DDEVICE9 pDevice,
    LPCTSTR pFileName,
    D3DVERTEXELEMENT9 *pElements = NULL,
    LPDIRECT3DVERTEXDECLARATION9 *ppDecl = NULL
);


/*
 *	D3D Font
 */

extern HRESULT CreateMyFont(
	LPDIRECT3DDEVICE9 pD3DDevice,		// D3D device
	INT nHeight,						// height of font
	INT nWidth,							// average character width
	LPD3DXFONT *ppD3DFont				// [out] D3D font interface
);

extern void DrawMyFont (
	LPD3DXFONT pD3DFont,								// D3D font interface
	LPCSTR pString,										// Pointer to a string to draw
	LONG Left, LONG Top,								// Rectangle, in logical coordinates, 
	LONG Right, LONG Bottom,							// in which the text is to be formatted
	D3DCOLOR Color = D3DCOLOR_ARGB(250, 250, 250, 50)	// Color of the text
);



/*
 *	Drawing morphed mesh from FRP / use morph shader
 */

extern void DrawMorphMesh(
	LPD3DXMESHCONTAINEREX pSourceMesh,
    LPD3DXMESHCONTAINEREX pTargetMesh,
    LPD3DXMESHCONTAINEREX pResultMesh,
    FLOAT fMorphScalar,
	long NormalOffset,
	DWORD FVFSize
);

extern void DrawMorphMesh(
	LPDIRECT3DVERTEXSHADER9		  pVShader,
	LPDIRECT3DVERTEXDECLARATION9  pVDecl,
	D3DXMESHCONTAINEREX			* pSourceMesh,
    D3DXMESHCONTAINEREX			* pTargetMesh,
    FLOAT fMorphScalar,
	CONST D3DXVECTOR3 			* pLightDir
);




/*
 *	Drawing blended morph mesh from FRP / use blend morph shader
 */

extern void DrawMorphBlendedMesh (
	LPD3DXMESHCONTAINEREX pBaseMesh,
    LPD3DXMESHCONTAINEREX pMesh1, FLOAT fBlend1,
    LPD3DXMESHCONTAINEREX pMesh2, FLOAT fBlend2,
    LPD3DXMESHCONTAINEREX pMesh3, FLOAT fBlend3,
    LPD3DXMESHCONTAINEREX pMesh4, FLOAT fBlend4,
	long NormalOffset,
	DWORD FVFSize,
	LPD3DXMESHCONTAINEREX pBlendedMesh
);


extern void DrawMorphBlendedMesh (
	LPDIRECT3DVERTEXSHADER9 pMorphShader,
	LPDIRECT3DVERTEXDECLARATION9 pMorphDecl,
	LPD3DXMESHCONTAINEREX pBaseMesh,
    LPD3DXMESHCONTAINEREX pMesh1, FLOAT fBlend1,
    LPD3DXMESHCONTAINEREX pMesh2, FLOAT fBlend2,
    LPD3DXMESHCONTAINEREX pMesh3, FLOAT fBlend3,
    LPD3DXMESHCONTAINEREX pMesh4, FLOAT fBlend4,
	CONST D3DXVECTOR3 *pLightDir
);






//*****************************************************************************
//								TEXTURE  UTILS
//*****************************************************************************

//-----------------------------------------------------------------------------
//	FUNCTION:	RedMaskTexture()
//	PURPOSE:	Add (masking) height terrain map with terrain mask.
//				Each pixels from height terrain map adding with red
//				components of mask for blending topography or creating islands.
//	INPUT:		Height map & terrain mask
//	OUTPUT:		HRESULT
//-----------------------------------------------------------------------------
extern HRESULT RedMaskTexture(
	LPDIRECT3DTEXTURE9 pTexture,
	LPDIRECT3DTEXTURE9 pMask
);


//-----------------------------------------------------------------------------
//	FUNCTION:	BuildNormalHeightMapFromFile()
//	PURPOSE:	Read height info, compute normal map from height map as RGB
//				save height info as ALPHA and save united to file
//	OUTPUT:		HRESULT
//-----------------------------------------------------------------------------
extern HRESULT BuildNormalHeightMapFromFile(
	LPDIRECT3DDEVICE9 g_pD3DDevice,
	LPCTSTR InFileName,
	LPCTSTR OutFileName
);

