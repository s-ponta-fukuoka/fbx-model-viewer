//=============================================================================
//
// vector.h
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _VECTOR_H_
#define _VECTOR_H_

//*****************************************************************************
// ÉNÉâÉXíËã`
//*****************************************************************************
class VECTOR2
{
public:
	float x;
	float y;

	VECTOR2() {}
	VECTOR2(const float nx, const float ny);

	//â¡éZ
	VECTOR2 operator+(const VECTOR2 &Vector) const;
	VECTOR2 &operator+=(const VECTOR2 &Vector);

	//å∏éZ
	VECTOR2 operator-(const VECTOR2 &Vector) const;
	VECTOR2 &operator-=(const VECTOR2 &Vector);

	//èÊéZ
	VECTOR2 operator*(const float f) const;
	VECTOR2 &operator*=(const float f);

	//èúéZ
	VECTOR2 operator/(const float f) const;
	VECTOR2 &operator/=(const float f);

};

class VECTOR3
{
public:
	float x;
	float y;
	float z;

	VECTOR3(){}
	VECTOR3(const float nx, const float ny, const float nz);

	//â¡éZ
	VECTOR3 operator+(const VECTOR3 &Vector) const;
	VECTOR3 &operator+=(const VECTOR3 &Vector);

	//å∏éZ
	VECTOR3 operator-(const VECTOR3 &Vector) const;
	VECTOR3 &operator-=(const VECTOR3 &Vector);

	//èÊéZ
	VECTOR3 operator*(const float f) const;
	VECTOR3 &operator*=(const float f);

	//èúéZ
	VECTOR3 operator/(const float f) const;
	VECTOR3 &operator/=(const float f);

	static void Cross(VECTOR3 *Vector, VECTOR3 *Vector1, VECTOR3 *Vector2);

	static void Normalize(VECTOR3 *Vector, VECTOR3 *Vector1);

	static float Length(VECTOR3 *Vector);

};

class VECTOR4
{
public:
	float x;
	float y;
	float z;
	float w;

	VECTOR4() {}
	VECTOR4(const float nx, const float ny, const float nz, const float nw);

	//â¡éZ
	VECTOR4 operator+(const VECTOR4 &Vector) const;
	VECTOR4 &operator+=(const VECTOR4 &Vector);

	//å∏éZ
	VECTOR4 operator-(const VECTOR4 &Vector) const;
	VECTOR4 &operator-=(const VECTOR4 &Vector);

	//èÊéZ
	VECTOR4 operator*(const float f) const;
	VECTOR4 &operator*=(const float f);

	//èúéZ
	VECTOR4 operator/(const float f) const;
	VECTOR4 &operator/=(const float f);

};

#endif