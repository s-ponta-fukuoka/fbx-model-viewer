//=============================================================================
//
// vector.h
// Author : SHOTA FUKUOKA
//
//=============================================================================

//*****************************************************************************
// ÉCÉìÉNÉãÅ[Éh
//*****************************************************************************
#include <math.h>
#include "vector.h"

///////////////////////////////////////////////////////////////////////////////
//ÉRÉìÉXÉgÉâÉNÉ^
///////////////////////////////////////////////////////////////////////////////
VECTOR2::VECTOR2(const float nx, const float ny)
	:x(nx)
	, y(ny)
{
	;
}

VECTOR3::VECTOR3(const float nx, const float ny, const float nz)
	:x(nx)
	,y(ny)
	,z(nz)
{
	;
}

VECTOR4::VECTOR4(const float nx, const float ny, const float nz, const float nw)
	:x(nx)
	, y(ny)
	, z(nz)
	, w(nw)
{
	;
}


///////////////////////////////////////////////////////////////////////////////
//â¡éZ
///////////////////////////////////////////////////////////////////////////////
VECTOR2 VECTOR2::operator+(const VECTOR2 &Vector) const
{
	VECTOR2 vector;

	vector.x = x + Vector.x;
	vector.y = y + Vector.y;

	return vector;
}

VECTOR3 VECTOR3::operator+(const VECTOR3 &Vector) const
{
	VECTOR3 vector;

	vector.x = x + Vector.x;
	vector.y = y + Vector.y;
	vector.z = z + Vector.z;

	return vector;
}

VECTOR4 VECTOR4::operator+(const VECTOR4 &Vector) const
{
	VECTOR4 vector;

	vector.x = x + Vector.x;
	vector.y = y + Vector.y;
	vector.z = z + Vector.z;
	vector.w = w + Vector.w;

	return vector;
}

///////////////////////////////////////////////////////////////////////////////
//â¡éZÅÅ
///////////////////////////////////////////////////////////////////////////////
VECTOR2& VECTOR2::operator+=(const VECTOR2 &Vector)
{
	x += Vector.x;
	y += Vector.y;

	return *this;
}

VECTOR3& VECTOR3::operator+=(const VECTOR3 &Vector)
{
	x += Vector.x;
	y += Vector.y;
	z += Vector.z;

	return *this;
}

VECTOR4& VECTOR4::operator+=(const VECTOR4 &Vector)
{
	x += Vector.x;
	y += Vector.y;
	z += Vector.z;
	w += Vector.w;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//å∏éZ
///////////////////////////////////////////////////////////////////////////////
VECTOR2 VECTOR2::operator-(const VECTOR2 &Vector) const
{
	VECTOR2 vector;

	vector.x = x - Vector.x;
	vector.y = y - Vector.y;

	return vector;
}

VECTOR3 VECTOR3::operator-(const VECTOR3 &Vector) const
{
	VECTOR3 vector;

	vector.x = x - Vector.x;
	vector.y = y - Vector.y;
	vector.z = z - Vector.z;

	return vector;
}

VECTOR4 VECTOR4::operator-(const VECTOR4 &Vector) const
{
	VECTOR4 vector;

	vector.x = x - Vector.x;
	vector.y = y - Vector.y;
	vector.z = z - Vector.z;
	vector.w = w - Vector.w;

	return vector;
}

///////////////////////////////////////////////////////////////////////////////
//å∏éZÅÅ
///////////////////////////////////////////////////////////////////////////////
VECTOR2& VECTOR2::operator-=(const VECTOR2 &Vector)
{
	x -= Vector.x;
	y -= Vector.y;

	return *this;
}

VECTOR3& VECTOR3::operator-=(const VECTOR3 &Vector)
{
	x -= Vector.x;
	y -= Vector.y;
	z -= Vector.z;

	return *this;
}

VECTOR4& VECTOR4::operator-=(const VECTOR4 &Vector)
{

	x -= Vector.x;
	y -= Vector.y;
	z -= Vector.z;
	w -= Vector.w;

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
//èÊéZ
///////////////////////////////////////////////////////////////////////////////
VECTOR2 VECTOR2::operator*(const float f) const
{
	VECTOR2 vector;

	vector.x = x * f;
	vector.y = y * f;

	return vector;
}

VECTOR3 VECTOR3::operator*(const float f) const
{
	VECTOR3 vector;

	vector.x = x * f;
	vector.y = y * f;
	vector.z = z * f;

	return vector;
}

VECTOR4 VECTOR4::operator*(const float f) const
{
	VECTOR4 vector;

	vector.x = x * f;
	vector.y = y * f;
	vector.z = z * f;
	vector.w = w * f;

	return vector;
}

///////////////////////////////////////////////////////////////////////////////
//èÊéZÅÅ
///////////////////////////////////////////////////////////////////////////////
VECTOR2& VECTOR2::operator*=(const float f)
{
	x *= f;
	y *= f;

	return *this;
}

VECTOR3& VECTOR3::operator*=(const float f)
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

VECTOR4& VECTOR4::operator*=(const float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//èúéZ
///////////////////////////////////////////////////////////////////////////////
VECTOR2 VECTOR2::operator/(const float f) const
{
	VECTOR2 vector;

	vector.x = x / f;
	vector.y = y / f;

	return vector;
}

VECTOR3 VECTOR3::operator/(const float f) const
{
	VECTOR3 vector;

	vector.x = x / f;
	vector.y = y / f;
	vector.z = z / f;

	return vector;
}

VECTOR4 VECTOR4::operator/(const float f) const
{
	VECTOR4 vector;

	vector.x = x / f;
	vector.y = y / f;
	vector.z = z / f;
	vector.w = w / f;

	return vector;
}

///////////////////////////////////////////////////////////////////////////////
//èúéZÅÅ
///////////////////////////////////////////////////////////////////////////////
VECTOR2& VECTOR2::operator/=(const float f)
{
	x /= f;
	y /= f;

	return *this;
}

VECTOR3& VECTOR3::operator/=(const float f)
{
	x /= f;
	y /= f;
	z /= f;

	return *this;
}

VECTOR4& VECTOR4::operator/=(const float f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;

	return *this;
}

// äOêœ ////////////////////////////////////////////////////////////////
void VECTOR3::Cross(VECTOR3 *Vector, VECTOR3 *Vector1, VECTOR3 *Vector2)
{

	Vector->x = Vector1->y * Vector2->z - Vector1->z * Vector2->y;
	Vector->y = Vector1->z * Vector2->x - Vector1->x * Vector2->z;
	Vector->z = Vector1->x * Vector2->y - Vector1->y * Vector2->x;

}

// ê≥ãKâª ////////////////////////////////////////////////////////////////
void VECTOR3::Normalize(VECTOR3 *Vector, VECTOR3 *Vector1)
{

	*Vector = *Vector1 / Length(Vector1);

}

// í∑Ç≥ ////////////////////////////////////////////////////////////////
float VECTOR3::Length(VECTOR3 *Vector)
{

	float len = sqrt(Vector->x * Vector->x +
		Vector->y * Vector->y +
		Vector->z * Vector->z);

	return len;

}