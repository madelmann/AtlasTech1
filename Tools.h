
#pragma once

// Library includes
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <xutility>

// Project includes
#include "Tuple2.h"


//#define PI 3.14159265f

const float fPi = 3.14159265f;
/** Pi constant in double format.
 */
const double dPi = 3.1415926535897932384626433832795;


#define SAFE_DELETE( p )       { if( p ) { delete ( p );     ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );   ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if( p ) { ( p )->Release(); ( p ) = NULL; } }

#define SQR(a)  ( a * a)


typedef	signed		__int8		sint8;
typedef	unsigned	__int8		uint8;
typedef	signed		__int16		sint16;
typedef	unsigned	__int16		uint16;
typedef	signed		__int32		sint32;
typedef	unsigned	__int32		uint32;
typedef	signed		__int64		sint64;
typedef	unsigned	__int64		uint64;

typedef				int			sint;			// at least 32bits (depend of processor)
typedef	unsigned	int			uint;			// at least 32bits (depend of processor)


extern double round(double r);


class CScreen
{
public:

	CScreen();
	~CScreen();

	int Depth;
	int Height;
	Tuple2<int32_t> Middle;
	int Width;
};

// This is our basic 3D point/vector class
class CVector3
{
public:

	CVector3();
	~CVector3(void);

    CVector3(float x_, float y_, float z_);
    void set(float x_, float y_, float z_);
    float length(void);
    void normalize(void);

    // Operators...
    CVector3 operator + (const CVector3 &other);
    CVector3 operator - (const CVector3 &other);
    CVector3 operator * (const CVector3 &other);
	CVector3 operator / (const CVector3 &other);

    CVector3 operator * (const float scalar);
    friend CVector3 operator * (const float scalar, const CVector3 &other);

	CVector3 operator / (const float scalar);
    
	CVector3& operator = (const CVector3 &other);
    CVector3& operator += (const CVector3 &other);
    CVector3& operator -= (const CVector3 &other);

	CVector3 operator + (void) const;
	CVector3 operator - (void) const;

	bool operator == (const CVector3 &other);
	bool operator != (const CVector3 &other);

	inline float normalize() const { return (float)sqrt(x * x + y * y + z * z); }

	// Static utility methods
    static float distance(const CVector3 &v1, const CVector3 &v2);
    static float dotProduct(const CVector3 &v1,  const CVector3 &v2 );
    static CVector3 crossProduct(const CVector3 &v1, const CVector3 &v2);


	float x;
	float y;
	float z;						
};


class CRGBA
{
public:
	/// Default constructor. do nothing
	CRGBA() {}

	/**
	 * Constructor.
	 * \param r Red componant.
	 * \param g Green componant.
	 * \param b Blue componant.
	 * \param a Alpha componant.
	 */
	CRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 255)
		: R(r), G(g), B(b), A(a) {}

	CRGBA(float r, float g, float b, float a = 1.f)
		: R((uint8)(r * 255)), G((uint8)(g * 255)), B((uint8)(b * 255)), A((uint8)(a * 255)) {}

	/**
	 * setup as a packed pixel
	 */
	void setPacked(uint packed)
	{
		R = (packed >> 24) & 255;
		G = (packed >> 16) & 255;
		B = (packed >> 8) & 255;
		A = packed & 255;
	}

	/**
	 * Return a packed pixel
	 */
	uint getPacked() const
	{
		return ((uint)R<<24) + ((uint)G<<16) + ((uint)B<<8) + A;
	}

	/**
	 * Comparison operator.
	 */
	bool operator < (CRGBA c) const
	{
		return getPacked() < c.getPacked();
	}
	/**
	 * Comparison operator.
	 */
	bool operator != (CRGBA c) const
	{
		return !(*this == c);
	}

	/**
	 * Equality operator.
	 */
	bool operator == (CRGBA c) const
	{
		return R == c.R && G == c.G && B == c.B && A == c.A;
	}

	/**
	 * Blend two colors.
	 * \param c0 Color 0.
	 * \param c1 Color 1.
	 * \param coef Blend factor. 0~256. 0 return c0 and 256 return c1.
	 */
	void blend(CRGBA c0, CRGBA c1, uint coef) // coef must be in [0,256]
	{
		uint a1 = coef;
		uint a2 = 256-a1;
		R = uint8((c0.R*a2 + c1.R*a1) >>8);
		G = uint8((c0.G*a2 + c1.G*a1) >>8);
		B = uint8((c0.B*a2 + c1.B*a1) >>8);
		A = uint8((c0.A*a2 + c1.A*a1) >>8);
	}

	/**
	 * Modulate colors with a constant.
	 * \param c0 Color 0.
	 * \param a E [0,256]. c0*a returned into this.
	 */
	void modulateFromui(CRGBA c0, uint a)
	{
		R = uint8((c0.R*a) >> 8);
		G = uint8((c0.G*a) >> 8);
		B = uint8((c0.B*a) >> 8);
		A = uint8((c0.A*a) >> 8);
	}

	/**
	 * Modulate colors with another color.
	 * \param c0 Color 0.
	 * \param c1 Color 1. c0*c1 returned into this.
	 */
	void modulateFromColor(CRGBA c0, CRGBA c1)
	{
		R = (c0.R*c1.R) >> 8;
		G = (c0.G*c1.G) >> 8;
		B = (c0.B*c1.B) >> 8;
		A = (c0.A*c1.A) >> 8;
	}

	/**
	 * Set colors.
	 * \param r Red componant.
	 * \param g Green componant.
	 * \param b Blue componant.
	 * \param a Alpha componant.
	 */
	void set(uint8 r, uint8 g, uint8 b, uint8 a = 255);

	/**
	 * Convert to gray value
	 */
	uint8 toGray() const;

	/**
	 * Compute in this the average of 2 RGBA.
	 */
	void avg2(CRGBA a, CRGBA b)
	{
		R = ((uint)a.R + (uint)b.R) >> 1;
		G = ((uint)a.G + (uint)b.G) >> 1;
		B = ((uint)a.B + (uint)b.B) >> 1;
		A = ((uint)a.A + (uint)b.A) >> 1;
	}

	/**
	 * Compute in this the average of 4 RGBA.
	 * The average is "correct": +1 is added to the four color, to make a "round" like average.
	 */
	void avg4(CRGBA a, CRGBA b, CRGBA c, CRGBA d)
	{
		R= ((uint)a.R + (uint)b.R + (uint)c.R + (uint)d.R + 1) >> 2;
		G= ((uint)a.G + (uint)b.G + (uint)c.G + (uint)d.G + 1) >> 2;
		B= ((uint)a.B + (uint)b.B + (uint)c.B + (uint)d.B + 1) >> 2;
		A= ((uint)a.A + (uint)b.A + (uint)c.A + (uint)d.A + 1) >> 2;
	}

	/**
	 *	Do the sum of 2 rgba, clamp, and store in this
	 */
	void add(CRGBA c0, CRGBA c1)
	{
		uint r, g, b, a;
		r = c0.R + c1.R;	r = (std::min)(r, 255U);	R = (uint8)r;
		g = c0.G + c1.G;	g = (std::min)(g, 255U);	G = (uint8)g;
		b = c0.B + c1.B;	b = (std::min)(b, 255U);	B = (uint8)b;
		a = c0.A + c1.A;	a = (std::min)(a, 255U);	A = (uint8)a;
	}

	/**
	 *	Compute c0 - c1, and store in this
	 */
	void sub(CRGBA c0, CRGBA c1)
	{
		sint r, g, b, a;
		r = c0.R - c1.R;	r = (std::max)(r, 0);	R = (uint8)r;
		g = c0.G - c1.G;	g = (std::max)(g, 0);	G = (uint8)g;
		b = c0.B - c1.B;	b = (std::max)(b, 0);	B = (uint8)b;
		a = c0.A - c1.A;	a = (std::max)(a, 0);	A = (uint8)a;
	}


	/// \name RGBOnly methods. Same f() as their homonym, but don't modify A component.
	// @{

	/// see blend()
	void blendRGBOnly(CRGBA c0, CRGBA c1, uint coef) // coef must be in [0,256]
	{
		uint	a1 = coef;
		uint	a2 = 256 - a1;
		R = uint8((c0.R * a2 + c1.R * a1) >> 8);
		G = uint8((c0.G * a2 + c1.G * a1) >> 8);
		B = uint8((c0.B * a2 + c1.B * a1) >> 8);
	}
	/// see modulateFromui()
	void modulateFromuiRGBOnly(CRGBA c0, uint a)
	{
		R = uint8((c0.R * a) >> 8);
		G = uint8((c0.G * a) >> 8);
		B = uint8((c0.B * a) >> 8);
	}
	/// see modulateFromColor()
	void modulateFromColorRGBOnly(CRGBA c0, CRGBA c1)
	{
		R = (c0.R * c1.R) >> 8;
		G = (c0.G * c1.G) >> 8;
		B = (c0.B * c1.B) >> 8;
	}
	/// see avg2()
	void avg2RGBOnly(CRGBA a, CRGBA b)
	{
		R = ((uint)a.R + (uint)b.R) >> 1;
		G = ((uint)a.G + (uint)b.G) >> 1;
		B = ((uint)a.B + (uint)b.B) >> 1;
	}
	/// see avg4()
	void avg4RGBOnly(CRGBA a, CRGBA b, CRGBA c, CRGBA d)
	{
		R = ((uint)a.R + (uint)b.R + (uint)c.R + (uint)d.R + 1) >> 2;
		G = ((uint)a.G + (uint)b.G + (uint)c.G + (uint)d.G + 1) >> 2;
		B = ((uint)a.B + (uint)b.B + (uint)c.B + (uint)d.B + 1) >> 2;
	}
	/// see add()
	void addRGBOnly(CRGBA c0, CRGBA c1)
	{
		uint r, g, b;
		r = c0.R + c1.R;	r = (std::min)(r, 255U);	R = (uint8)r;
		g = c0.G + c1.G;	g = (std::min)(g, 255U);	G = (uint8)g;
		b = c0.B + c1.B;	b = (std::min)(b, 255U);	B = (uint8)b;
	}
	/// see sub()
	void subRGBOnly(CRGBA c0, CRGBA c1)
	{
		sint r, g, b;
		r = c0.R - c1.R;	r = (std::max)(r, 0);	R = (uint8)r;
		g = c0.G - c1.G;	g = (std::max)(g, 0);	G = (uint8)g;
		b = c0.B - c1.B;	b = (std::max)(b, 0);	B = (uint8)b;
	}
	// @}

	///\name Color group manipulation
	//@{
		/** Add a group of colors with saturation, using mmx instructions when present.
		  * \param dest The destination color buffer, encoded as CRGBA's.
		  * \param src1 The first source color buffer, encoded as CRGBA's.
		  * \param src2 The second source color buffer, encoded as CRGBA's.
		  * \param numColors The number of colors to compute
		  * \param Stride between each source color.
		  * \param Stride between each destination color.
		  * \param Dup the number of time the result must be duplicated in the destination.
		  */
		static void addColors(CRGBA *dest, const CRGBA *src1, const CRGBA *src2, uint numColors, uint srcStride = sizeof(CRGBA), uint destStride = sizeof(CRGBA), uint dup = 1);

		/** Modulate a group of colors with saturation, using mmx instructions when present.
		  * \param dest The destination color buffer, encoded as CRGBA's.
		  * \param src1 The first source color buffer, encoded as CRGBA's.
		  * \param src2 The second source color buffer, encoded as CRGBA's.
		  * \param numColors The number of colors to compute
		  * \param Stride between each color.  It is the same for sources and destination.
		  */
		static void modulateColors(CRGBA *dest, const CRGBA *src1, const CRGBA *src2, uint numColors, uint srcStride = sizeof(CRGBA), uint destStride = sizeof(CRGBA), uint dup = 1);

		/** Subtract a group of colors with saturation (src1 - src2), using mmx instructions when present.
		  * \param dest The destination color buffer, encoded as CRGBA's.
		  * \param src1 The first source color buffer, encoded as CRGBA's.
		  * \param src2 The second source color buffer, encoded as CRGBA's.
		  * \param numColors The number of colors to compute
		  * \param Stride between each color.  It is the same for sources and destination.
		  */
		static void subtractColors(CRGBA *dest, const CRGBA *src1, const CRGBA *src2, uint numColors, uint srcStride = sizeof(CRGBA), uint destStride = sizeof(CRGBA), uint dup = 1);
	//@}

	/// \name Color space conversions RGB only
	//@{
			/** Convert to HLS color space.
			  * Lightness and satuation ranges from 0 to 1
			  * There's no range for hue, (all hues colors range from 0 to 360, from 360 to 720 and so on)
			  * \return true if the color is achromatic
			  */
			bool convertToHLS(float &h, float &l, float &S) const;

			/** Build from HLS valued
			  *	Each component ranges from 0 to 1.f
			  */
			void buildFromHLS(float h, float l, float s);
	//@}


	/// Swap the B and R components, to simulate a CBRGA
	void swapBR()
	{
		std::swap(R,B);
	}


	/// Red componant.
	uint8	R;
	/// Green componant.
	uint8	G;
	/// Blue componant.
	uint8	B;
	/// Alpha componant.
	uint8	A;

	/// some colors
	static const CRGBA Black ;
	static const CRGBA Red ;
	static const CRGBA Green ;
	static const CRGBA Yellow ;
	static const CRGBA Blue ;
	static const CRGBA Magenta ;
	static const CRGBA Cyan ;
	static const CRGBA White ;
};


char *ExtractFileExt(char *Filename);
char *ExtractFilename(char *Filename);
void readline(FILE *f, char *string);
void readstr(FILE *f, char *string);
char *RemoveFileExt(char Filename[255]);

// Strings
bool StringCompare(std::string s1, std::string s2);
bool StringCompareI(const std::string & s1, const std::string& s2);

