/*	Point3D class

	Last Modified	6/4/00
*/

#ifndef __POINT3D_CPP_
#define __POINT3D_CPP_

#include <math.h>
#include <assert.h>

#ifdef __MAC__
  // note: for mac only
  #include <OpenGL/OpenGL.h>
#else
  #include <GL/gl.h>
#endif


#include "MathsStruct.h"

#include "Vector3D.h"
#include "Point3D.h"

//////////////////////////////

Point3D::Point3D()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	return;
}	// end constructor

////////

Point3D::Point3D(const float x, const float y, const float z)
{
	SetValues(x, y, z);

	return;
}	// end constructor

////////

void Point3D::Round()
{
	x = ROUND(x, FLOAT_DECIMAL_TOLERANCE);
	y = ROUND(y, FLOAT_DECIMAL_TOLERANCE);
	z = ROUND(z, FLOAT_DECIMAL_TOLERANCE);

	return;
}	// end void Round();

//////

void Point3D::GetValues(float &x, float &y, float &z) const
{
	x = this->x;
	y = this->y;
	z = this->z;

	return;
}	// end void Get_Values(..)

////////

void Point3D::SetValues(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;

	return;
}	// end void Set_Values(..)

/////////

void Point3D::Negate()
{
	x	= -x;
	y	= -y;
	z	= -z;

	return;
}	// end void Negate()

/////////

double Point3D::Distance(const Point3D &p) const
{
	return(sqrt(DistanceSquared(p)));
}	// end double Distance(..)

/////////

double Point3D::DistanceSquared(const Point3D &p) const
{
	float x, y, z;		// temp vars

	p.GetValues(x, y, z);		// get values from point

	return(	SQR(this->x - x) +
			SQR(this->y - y) + 
			SQR(this->z - z) );

}	// end double DistanceSquared(..)

/////////

void Point3D::Translate(const Vector3D &v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;

	return;
}	// end void Translate(..)

////////

void Point3D::glTranslate(void) const
{
	glTranslatef(x, y, z);

	return;
}	// end void glTranslate()

/////////

void Point3D::glVertex(void) const
{
	glVertex3f(x, y, z);

	return;
}	// end void glVertex()

/////////

Point3D Point3D::operator+(const Point3D &p) const
{
	float x, y, z;		// temp values

	x = this->x + p.x;
	y = this->y + p.y;
	z = this->z + p.z;

	return(Point3D(x, y, z));
}	// end operator +

/////////

Point3D Point3D::operator-(const Point3D &p) const
{
	float x, y, z;		// temp values

	x = this->x - p.x;
	y = this->y - p.y;
	z = this->z - p.z;

	return(Point3D(x, y, z));
}	// end operator -

////////

Point3D Point3D::operator*(const float s) const
{
	float x, y, z;

	x = this->x * s;
	y = this->y * s;
	z = this->z * s;

	return(Point3D(x, y, z));
}	// end operator *

///////

Point3D Point3D::operator-(void) const
{
	return(Point3D(-x, -y, -z));
}	// end operator -

////////

Point3D Point3D::operator+(const Vector3D &v) const
{
	float x, y, z;		// temp final values

	x = this->x + v.x;	// translate
	y = this->y + v.y;
	z = this->z + v.z;

	return(Point3D(x, y, z));
}	// end operator +

////////

Point3D& Point3D::operator+=(const Vector3D &v)
{
	Translate(v);

	return(*this);
}	// end operator +=

////////

Point3D& Point3D::operator=(const Point3D &p)
{
	x = p.x;
	y = p.y;
	z = p.z;

	return(*this);
}	// end operator =

////////

Point3D& Point3D::operator+=(const Point3D &p)
{
	x += p.x;
	y += p.y;
	z += p.z;

	return(*this);
}	// end operator +=

////////

Point3D& Point3D::operator-=(const Point3D &p)
{
	x -= p.x;
	y -= p.y;
	z -= p.z;

	return(*this);
}	// end operator -=

////////

Point3D& Point3D::operator*=(const float s)
{
	x *= s;
	y *= s;
	z *= s;

	return(*this);
}	// end operator *=

////////

int Point3D::operator==(const Point3D &p) const
{
	return( FLOAT_EQ(x, p.x) && FLOAT_EQ(y, p.y) && FLOAT_EQ(z, p.z) );
}	// end operator ==

///////

int Point3D::operator!=(const Point3D &p) const
{
	return( !(*this == p) );
}	// end operator !=

////////

// ------------------------ debug
#include <iostream>
using namespace std;

void Point3D::Print(void)
{
	cout << "Point is " << x << " " << y << " " << z << endl;

	return;
}	// end void Print()

//////////////////////////////

#endif	// __POINT3D_CPP_
