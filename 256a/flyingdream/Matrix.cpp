/*	4x4 Matrix

	Last Modified	14/4/00
*/

#ifndef __MATRIX_CPP_
#define __MATRIX_CPP_

#include <math.h>
#include <assert.h>
#include <memory.h>

#include "MathsStruct.h"

#include "Point3D.h"
#include "Vector3D.h"
#include "Matrix.h"


///////////////////////////////////////

Matrix::Matrix()
{
	LoadIdentity();

	return;
}	// end constructor

///////////

Matrix::Matrix(const float m[16])
{
	SetValues(m);

	return;
}	// end constructor

//////////

Matrix::Matrix(const Vector3D &v)
{
	SetValues(v);

	return;
}	// end constructor

//////////

Matrix::Matrix(const float angle, const Vector3D &v)
{
	SetValues(angle, v);

	return;
}	// end constructor

//////////

inline void Matrix::Verify_Matrix()
{
	m[3]	= 0.0f;
	m[7]	= 0.0f;
	m[11]	= 0.0f;
	m[15]	= 1.0f;

	return;
}	// end void Verify_Matrix()

/////////

void Matrix::Zero_Clamp(void)
{
	int index;		// looping var

	for (index = 0; index < 16; index++)
		m[index] = ZERO_CLAMP(m[index]);

	return;
}	// end void Zero_Clamp()

/////////

void Matrix::GetValues(float m[16]) const
{
	memcpy(m, this->m, 16*sizeof(float));	// copy the values to variable

	return;
}	// end void GetValues(..)

////////

void Matrix::GetTranslation(Point3D &p) const
{
	p.SetValues(m[12], m[13], m[14]);		// copy the translation portion of the 4x4 matrix

	return;
}	// end void GetTranslation(..)

////////

void Matrix::GetEulerAngles(float &x, float &y, float &z) const
{
	//%%%%%%%%%%%%%
	// NOT WORKING
	//%%%%%%%%%%%%%

	double angle_x, angle_y, angle_z;		// temp angles
	double cy, tx, ty, sy;
/*
	angle_y = -asin(m[2]);		// calculate y axis angle
	cy		= cos(angle_y);
*/

	sy	= -m[2];
	cy	= sqrt(1 - SQR(sy));
	angle_y	= atan2(sy, cy);


	if (EPSILON < fabs(cy))		// no gimbal lock?
	{
		tx	= m[10] / cy;		// get x axis angle
		ty	= m[6] / cy;

		angle_x	= atan2(ty, tx);

		tx	= m[0] / cy;		// get z axis angle
		ty	= m[1] / cy;

		angle_z	= atan2(ty, tx);
	}
	else		// gimbal lock
	{
		angle_x	= 0.0f;			// set x axis angle to 0

		tx	= m[9];				// get z axis angle
		ty	= m[5];

		angle_z	= atan2(ty, tx);
	}

	x	= (float)RADTODEG(angle_x);		// convert all to deg
	y	= (float)RADTODEG(angle_y);
	z	= (float)RADTODEG(angle_z);
/*
	LIMIT_RANGE(-180.0f, x, 180.0f);	// and limit them
	LIMIT_RANGE(-180.0f, y, 180.0f);
	LIMIT_RANGE(-180.0f, z, 180.0f);
*/
	return;
}	// end void GetEulerAngles(..)

//////

void Matrix::SetValues(const float m[16])
{
	memcpy(this->m, m, 16*sizeof(float));	// copy the values to this matrix

	Verify_Matrix();

	return;
}	// end void SetValues(..)

////////

void Matrix::SetValues(const Point3D &p)
{
	LoadIdentity();

	float x, y, z;		// temp vars

	p.GetValues(x, y, z);		// get values from points since no friends

	m[12]	= x;
	m[13]	= y;
	m[14]	= z;

	return;
}	// end void SetValues(..)

////////

void Matrix::SetValues(const Vector3D &v)
{
	LoadIdentity();

	float x, y, z;		// temp vars

	//v.GetValues(x, y, z);	// get values from vector since no friends

	m[12]	= v.x;
	m[13]	= v.y;
	m[14]	= v.z;

	return;
}	// end void SetValues(..)

///////

void Matrix::SetValues(const float angle, const Vector3D &v)
{
	int index;		// looping var

	double sin_rad, cos_rad;		// temp angle in radians

	float sx, sy, sz;				// temp vars

	float xx, xy, xz, yy, yz, zz;	// temp values forming matrix uut

	Vector3D u;						// temp vector

	float uut[9];					// temp 3x3 matrix UU^T

	float matrix[9];				// temp 3x3 rotation matrix

	u = v;
	u.normalize();		// safe

	xx	= SQR(u.x);
	xy	= u.x * u.y;
	xz	= u.x * u.z;
	yy	= SQR(u.y);
	yz	= u.y * u.z;
	zz	= SQR(u.z);

	uut[0] = xx;		// create matrix uuT
	uut[1] = xy;
	uut[2] = xz;
	uut[3] = xy;
	uut[4] = yy;
	uut[5] = yz;
	uut[6] = xz;
	uut[7] = yz;
	uut[8] = zz;

	memset(matrix, 0, sizeof(float) * 9);		// clear out matrix first

	sin_rad = sin(angle);			// cache result

	sx = (float)(sin_rad * u.x);
	sy = (float)(sin_rad * u.y);
	sz = (float)(sin_rad * u.z);

	matrix[1] = sz;		matrix[3] = -sz;	matrix[6] = sy;		// form sin(angle)S
	matrix[2] = -sy;	matrix[5] = sx;		matrix[7] = -sx;

	for (index = 0; index < 9; index++)				// add UU^T to form sin(angle)S + UU^T
		matrix[index] += uut[index];

	for (index = 0; index < 9; index++)				// negate UU^T
		uut[index] = -uut[index];

	uut[0] += 1;		// add one to form (I - UU^T)
	uut[4] += 1;
	uut[8] += 1;

	cos_rad = cos(angle);		// cache result

	for (index = 0; index < 9; index++)		// form cos(angle)(I - UU^T)
		uut[index] *= (float)cos_rad;

	for (index = 0; index < 9; index++)		// add cos(angle)(I - UU^T) to matrix to form UU^T + cos(angle)(I - UU^T) + sin(angle)S
		matrix[index] += uut[index];

	// copy to matrix
	LoadIdentity();

	for (index = 0; index < 3; index++)		// copy over to main matrix
		this->m[index] = matrix[index];

	for (index = 3; index < 6; index++)
		this->m[index+1] = matrix[index];

	for (index = 6; index < 9; index++)
		this->m[index+2] = matrix[index];

	return;
}	// end void SetValues(..)

/*	Another way of doing it
void Matrix::SetValues(const float angle, const Vector3D &v)
{
	Matrix final, rx, ry, rz,irx, iry;		// temp matrices

	Vector3D	u;

	float	m[16];			// temp matrix

	float	x, y, z, d;		// temp vars

	u = v;
	u.Normalize();				// normalize it first. safe

	u.GetValues(x, y, z);		// get values of axis

	d = (float)sqrt( SQR(y) + SQR(z));

	rx.LoadIdentity();
	rx.GetValues(m);

	m[5]	= z / d;
	m[6]	= y / d;
	m[9]	= -y / d;
	m[10]	= z / d;

	rx.SetValues(m);			// create rot x

	ry.LoadIdentity();
	ry.GetValues(m);

	m[0]	= d;
	m[2]	= x;
	m[8]	= -x;
	m[10]	= d;

	ry.SetValues(m);			// create rot y

	rz.SetValues_RotZ((float)RADTODEG(angle));

	irx = rx;
	irx.Transpose();	// get inverse

	iry = ry;
	iry.Transpose();	// get inverse

	final = irx * iry * rz * ry * rx;		// align axis to y axis, rotate and then unalign

	*this =final;		// copy the final matrix over

	Verify_Matrix();

	return;
}	// end void SetValues(..)
*/
/////////

void Matrix::SetValues_RotX(const float angle)
{
	double rad;		// temp angle
	float cos_angle, sin_angle;		// temp vars

	LoadIdentity();

	rad = DEGTORAD(angle);		// convert to radians

	cos_angle = (float)cos(rad);
	sin_angle = (float)sin(rad);

	m[5]	= cos_angle;
	m[6]	= sin_angle;
	m[9]	= -sin_angle;
	m[10]	= cos_angle;

	return;
}	// end void SetValues_RotX(..)

/////////

void Matrix::SetValues_RotY(const float angle)
{
	double rad;		// temp angle
	float cos_angle, sin_angle;		// temp vars

	LoadIdentity();

	rad = DEGTORAD(angle);		// convert to radians

	cos_angle = (float)cos(rad);
	sin_angle = (float)sin(rad);

	m[0]	= cos_angle;
	m[2]	= -sin_angle;
	m[8]	= sin_angle;
	m[10]	= cos_angle;

	return;
}	// end void SetValues_RotY(..)

/////////

void Matrix::SetValues_RotZ(const float angle)
{
	double rad;		// temp angle
	float cos_angle, sin_angle;		// temp vars

	LoadIdentity();

	rad = DEGTORAD(angle);		// convert to radians

	cos_angle = (float)cos(rad);
	sin_angle = (float)sin(rad);

	m[0]	= cos_angle;
	m[1]	= sin_angle;
	m[4]	= -sin_angle;
	m[5]	= cos_angle;

	return;
}	// end void SetValues_RotZ(..)

/////////

void Matrix::SetValues_Rot(const float x, const float y, const float z)
{
	Matrix rotx, roty, rotz;		// temp transformation matrixes

	rotx.SetValues_RotX(x);
	roty.SetValues_RotY(y);
	rotz.SetValues_RotZ(z);

	*this = rotz * roty * rotx;		// multiply all together

	Verify_Matrix();

	return;
}	// end void SetValues_Rot(..)

/////////

void Matrix::Inverse()
{
	//%%%%%%%%%%%%
	assert(false);		// make sure this function is not called
	//%%%%%%%%%%%%

	assert(FLOAT_EQ(m[3], 0.0f));	// last row all zeros
	assert(FLOAT_EQ(m[7], 0.0f));
	assert(FLOAT_EQ(m[11], 0.0f));
	assert(FLOAT_EQ(m[15], 1.0f));	// last value is 1

	double det, det_inv;		// temp var

	det =	m[0] *(m[5]*m[10]	- m[9]*m[6]) -
			m[4] * (m[1]*m[10]	- m[9]*m[2]) +
			m[8] * (m[1]*m[6]	- m[5]*m[2]);

	assert(!FLOAT_EQ(det, 0.0f));		// matrix not singular

	det_inv = 1 / det;		// cache value

	//%%%%%%%%%%
	// NOT DONE
	//%%%%%%%%%%
/*
	m[3]	= 0.0f;
	m[7]	= 0.0f;
	m[11]	= 0.0f;
	m[15]	= 1.0f;		// last row is 0 0 0 1
*/
	return;
}	// end void Inverse()

////////

void Matrix::LoadIdentity(void)
{
	memset(m, 0, 16*sizeof(float));		// set all to 0

	m[0]	= 1;		// set diagonal to 1
	m[5]	= 1;
	m[10]	= 1;
	m[15]	= 1;

	return;
}	// end void LoadIdentity()

///////////

void Matrix::Negate(void)
{
	int index;		// looping var

	for (index = 0; index < 16; index++)	// negate each value
		m[index] = -m[index];

	return;
}	// end void Negate()

///////////

void Matrix::Transpose(void)
{
	SWAP(m[1], m[4]);		// swap the values
	SWAP(m[2], m[8]);
	SWAP(m[3], m[12]);
	SWAP(m[6], m[9]);
	SWAP(m[7], m[13]);
	SWAP(m[11],m[14]);

	return;
}	// end void Transpose()

//////////

Matrix Matrix::operator*(const Matrix &m) const
{
	float	result[16];		// temp vars
	double	sum;

	int	index, alpha, beta;		// loop vars

	for (index = 0; index < 4; index++)			// perform multiplcation the slow and safe way
	{
		for (alpha = 0; alpha < 4; alpha++)
		{
			sum = 0.0f;

			for (beta = 0; beta < 4; beta++)
				sum += this->m[index + beta*4] * m.m[alpha*4 + beta];

			result[index + alpha*4] = (float)sum;
		}	// end for alpha
	}	// end for index

	return(Matrix(result));
}	// end operator *

///////////

Matrix Matrix::operator*(const float s) const
{
	float result[16];	// temp result

	int index;			// looping var

	for (index = 0; index < 16; index++)		// multiply each element by s
		result[index]	= m[index] * s;

	return(Matrix(result));
}	// end operator *

///////////
/*
Matrix Matrix::operator+(const Matrix &m) const
{
	float result[16];	// temp result

	int index;			// looping var

	for (index = 0; index < 16; index++)		// add each element in matrix
		result[index]	= this->m[index] + m.m[index];

	return(Matrix(result));
}	// end operator +
*/
////////////
/*
Matrix Matrix::operator-(const Matrix &m) const
{
	float result[16];	// temp result

	int index;			// looping var

	for (index = 0; index < 16; index++)		// delete each element in matrix
		result[index]	= this->m[index] - m.m[index];

	return(Matrix(result));
}	// end operator -
*/
//////////

Matrix Matrix::operator-() const
{
	Matrix m;

	memcpy(m.m, this->m, 16*sizeof(float));		// copy this matrix to result matrix

	m.Negate();

	return(m);
}	// end operator -

/////////

Vector3D Matrix::operator*(const Vector3D &v) const
{
	int		index, alpha;	// looping var

	float vect[4],		// temp 1x4 col vector
	  final[4];		// temp 1x4 col vector to hold the result

	double	sum;

	vect[0] = v.x;			// create a 1x4 column vector
	vect[1] = v.y;
	vect[2] = v.z;
	vect[3] = 1;

	for (index = 0; index < 4; index++)
	{
		sum= 0;

		for (alpha = 0; alpha < 4; alpha++)
			sum += m[index + alpha*4] * vect[alpha];

		final[index] = (float)sum;
	}	// end for index

	assert(FLOAT_EQ(1.0f, final[3]));		// make sure last entry in vector is 1

	return(Vector3D(final[0], final[1], final[2]));
}	// end operator *


////////

Point3D Matrix::operator*(const Point3D &p) const
{
	int		index, alpha;	// looping var

	float	x, y, z,		// temp vars
			point[4],		// temp 1x4 col vector
			final[4];		// temp 1x4 col vector to hold the result

	double	sum;

	p.GetValues(x, y, z);		// get values from vector since no friends

	point[0] = x;			// create a 1x4 column vector
	point[1] = y;
	point[2] = z;
	point[3] = 1;

	for (index = 0; index < 4; index++)
	{
		sum= 0;

		for (alpha = 0; alpha < 4; alpha++)
			sum += m[index + alpha*4] * point[alpha];

		final[index] = (float)sum;
	}	// end for index

	assert(FLOAT_EQ(1.0f, final[3]));		// make sure last entry in vector is 1

	return(Point3D(final[0], final[1], final[2]));
}	// end operator *

////////

Matrix& Matrix::operator=(const Matrix &m)
{
	memcpy(this->m, m.m, 16*sizeof(float));		// copy ths matrix over

	return(*this);
}	// end operator =

/////////
/*
Matrix& Matrix::operator+=(const Matrix &m)
{
	int index;		// looping var

	for (index = 0; index < 16; index++)		// add each element
		this->m[index] += m.m[index];

	Zero_Clamp();

	return(*this);
}	// end operator +=
*/
/////////
/*
Matrix& Matrix::operator-=(const Matrix &m)
{
	int index;		// looping var

	for (index = 0; index < 16; index++)		// minus each element
		this->m[index]	-= m.m[index];

	Zero_Clamp();

	return(*this);
}	// end operator -=
*/
////////////

Matrix& Matrix::operator*=(const Matrix &m)
{
	Matrix temp;	// temp matrix

	temp = (*this) * m;		// calculate cross product and store in temp to avoid overwriting with new values

	(*this) = temp;			// copy temp to this matrix

	Verify_Matrix();

	return(*this);
}	// end operator *=

/////////

Matrix& Matrix::operator*=(const float s)
{
	int index;		// looping var

	for (index = 0; index < 16; index++)		// multiply each element
		this->m[index]	*= s;

	Verify_Matrix();

	return(*this);
}

/////////

int Matrix::operator==(const Matrix &m) const
{
	int	index,		// looping var
		result;		// temp result

	result = 1;		// set flag to true

	for (index = 0; index < 16 && result; index++)		// loop through each element and test if they are equal
		result = FLOAT_EQ(this->m[index], m.m[index]);

	return(result);
}	// end operator ==

////////

int Matrix::operator!=(const Matrix &m) const
{
	return(!(*this == m));
}	// end operator !=

//----------- debug

#include <iostream>
using namespace std;

void Matrix::Print(void) const
{
	cout << "Matrix is "  << endl;
	cout << m[0] << " " << m[4] << " " << m[8]	<< " " << m[12] << endl;
	cout << m[1] << " " << m[5] << " " << m[9]	<< " " << m[13] << endl;
	cout << m[2] << " " << m[6] << " " << m[10] << " " << m[14] << endl;
	cout << m[3] << " " << m[7] << " " << m[11] << " " << m[15] << endl;
}	// end void Print

//////////////////////////////////////

#endif	// __MATRIX_CPP_
