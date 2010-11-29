/*	Quaternion class

	Last Modified	20/4/00
*/

#ifndef __QUATERNION_CPP_
#define	__QUATERNION_CPP_

#include <math.h>
#include <memory.h>

#include <assert.h>

#include "MathsStruct.h"

#include "Quaternion.h"

/////////////////////////////////

Quaternion::Quaternion()
{
	Identity();

	return;
}	// end constructor

////////

Quaternion::Quaternion(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;

	this->w = w;

	assert(!FLOAT_EQ(0.0f, Norm()));		// norm should never be close to 0

	Normalize();

	return;
}	// end constructor

////////

Quaternion::Quaternion(const float x, const float y, const float z)
{
	EulerToQuat(x, y, z);

	return;
}	// end constructor

////////

Quaternion::Quaternion(const Vector3D &v, const float angle)
{
	AxisToQuat(v, angle);

	return;
}	// end constructor

////////

inline double Quaternion::Norm(void) const
{
	return(sqrt( SQR(x) + SQR(y) + SQR(z) + SQR(w)));
}	// end float Norm()

////////

void Quaternion::Identity(void)
{
	x	= 0.0f;
	y	= 0.0f;
	z	= 0.0f;
	w	= 1.0f;

}	// end void Identity()

///////

void Quaternion::Zero_Clamp(void)
{
	x = ZERO_CLAMP(x);
	y = ZERO_CLAMP(y);
	z = ZERO_CLAMP(z);

	w = ZERO_CLAMP(w);

	return;
}	// end void Zero_Clamp()

////////

void Quaternion::Normalize(void)
{
	double norm = Norm();

	assert(!FLOAT_EQ(0.0f, norm));		// norm should never be close to 0

	x = float(x / norm);
	y = float(y / norm);
	z = float(z / norm);
	w = float(w / norm);

	assert(FLOAT_EQ(1.0f, Norm()));		// must be normalized, safe

	LIMIT_RANGE(-1.0f, w, 1.0f);

	LIMIT_RANGE(-1.0f, x, 1.0f);
	LIMIT_RANGE(-1.0f, y, 1.0f);
	LIMIT_RANGE(-1.0f, z, 1.0f);

	return;
}	// end void Normalize()

////////

void Quaternion::GetValues(float &x, float &y, float &z, float &w) const
{
	x = this->x;
	y = this->y;
	z = this->z;

	w = this->w;

	return;
}	// end void GetValues

////////

void Quaternion::AxisToQuat(const Vector3D &v, const float angle)
{
  //float x,y,z;			// temp vars of vector
	double rad, scale;		// temp vars

	if (v.IsZero())			// if axis is zero, then return quaternion (1,0,0,0)
	{
		w	= 1.0f;
		x	= 0.0f;
		y	= 0.0f;
		z	= 0.0f;

		return;
	}

	assert(v.IsUnit());		// make sure the axis is a unit vector

	rad		= angle / 2;

	w		= (float)cos(rad);

	scale	= sin(rad);

	//v.GetValues(x, y, z);

	this->x = float(v.x * scale);
	this->y = float(v.y * scale);
	this->z = float(v.z * scale);

	Normalize();		// make sure a unit quaternion turns up

	return;
}	// end void AxisToQuat(..)

/////////

void Quaternion::EulerToQuat(const float x, const float y, const float z)
{
	double	ex, ey, ez;		// temp half euler angles
	double	cr, cp, cy, sr, sp, sy, cpcy, spsy;		// temp vars in roll,pitch yaw

	ex = DEGTORAD(x) / 2.0;	// convert to rads and half them
	ey = DEGTORAD(y) / 2.0;
	ez = DEGTORAD(z) / 2.0;

	cr = cos(ex);
	cp = cos(ey);
	cy = cos(ez);

	sr = sin(ex);
	sp = sin(ey);
	sy = sin(ez);

	cpcy = cp * cy;
	spsy = sp * sy;

	this->w = float(cr * cpcy + sr * spsy);

	this->x = float(sr * cpcy - cr * spsy);
	this->y = float(cr * sp * cy + sr * cp * sy);
	this->z = float(cr * cp * sy - sr * sp * cy);

	Normalize();
	
	return;
}	// end void EulerToQuat(..)

////////

void Quaternion::GetAxisAngle(Vector3D &v, float &angle) const
{
	double	temp_angle;		// temp angle
	double	scale;			// temp vars

	temp_angle = acos(w);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Another version where scale is sqrt (x2 + y2 + z2)
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	scale = (float)sqrt(SQR(x) + SQR(y) + SQR(z));
//	scale = (float)sin(temp_angle);

	assert(0 <= temp_angle);		// make sure angle is 0 - PI
	assert(PI >= temp_angle);

	if (FLOAT_EQ(0.0f, scale))		// angle is 0 or 360 so just simply set axis to 0,0,1 with angle 0
	{
		angle = 0.0f;

		//v.SetValues(0.0f, 0.0f, 1.0f);		// any axis will do
		v.x = 0.0f;
		v.y = 0.0f;
		v.z = 0.0f;
	}
	else
	{
		angle = (float)(temp_angle * 2.0);		// angle in radians

		//v.SetValues(float(x / scale), float(y / scale), float(z / scale));
		v.x = float(x / scale);
		v.y = float(y / scale);
		v.z = float(z / scale);
		v.normalize();

		assert(0.0f <= angle);			// make sure rotation around axis is 0 - 360
		assert(2*PI >= angle);
		assert(v.IsUnit());				// make sure a unit axis comes up
	}

	return;
}	// end void GetAxisAngle(..)

////////

void Quaternion::GetEulerAngles(float &x, float &y, float &z) const
{
	Matrix matrix;			// temp matrix

	GetMatrix(matrix);		// get matrix of this quaternion

	matrix.GetEulerAngles(x, y, z);

	return;
}	// end void GetEulerAngles(.,)

////////

void Quaternion::GetMatrix(Matrix &m) const
{
	double x2, y2, z2, w2, xy, xz, yz, wx, wy, wz;

	float matrix[16];		// temp matrix

	memset(matrix, 0, sizeof(float)*16);		// clear out matrix first
	matrix[15] = 1;								// last entry is 1

	x2	= SQR(x);	y2	= SQR(y);	z2	= SQR(z);	w2	= SQR(w);

	xy	= x * y;
	xz	= x * z;
	yz	= y * z;
	wx	= w * x;
	wy	= w * y;
	wz	= w * z;

/*	This is the old version
	// this gives the same matrix as uu^T + cos(angle)(I - uu^T) + sin(angle)S
	matrix[0] =	w2 + x2 -y2 - z2;
	matrix[1] =	2 * (xy + wz);
	matrix[2] =	2 * (xz - wy);

	matrix[4] =	2 * (xy - wz);
	matrix[5] =	w2 - x2 + y2 - z2;
	matrix[6] =	2 * (yz + wx);

	matrix[8] =	2 * (xz + wy);
	matrix[9] =	2 * (yz - wx);
	matrix[10] =	w2 - x2 - y2 + z2;
*/
//
	// should be slightly more accurate than above due to rearranging for unit quaternions only
	matrix[0] =	float(1 - 2*(y2 + z2));
	matrix[1] =	float(2 * (xy + wz));
	matrix[2] =	float(2 * (xz - wy));

	matrix[4] =	float(2 * (xy - wz));
	matrix[5] =	float(1 - 2*(x2 + z2));
	matrix[6] =	float(2 * (yz + wx));

	matrix[8] =	float(2 * (xz + wy));
	matrix[9] =	float(2 * (yz - wx));
	matrix[10] =	float(1 - 2*(x2 + y2));
//

	m.SetValues(matrix);

	return;
}	// end void GetMatrix(..)

////////

Quaternion Quaternion::operator*(const Quaternion &q) const
{
	double rx, ry, rz, rw;		// temp result

	rw	= q.w*w - q.x*x - q.y*y - q.z*z;

	rx	= q.w*x + q.x*w + q.y*z - q.z*y;
	ry	= q.w*y + q.y*w + q.z*x - q.x*z;
	rz	= q.w*z + q.z*w + q.x*y - q.y*x;

	return(Quaternion((float)rx, (float)ry, (float)rz, (float)rw));
}	// end operator *

/* Screw this method
Quaternion Quaternion::operator*(const Quaternion &q) const
{
	float x,y,z,w;			// temp new values

	Vector3D	v1((*this).x,(*this).y,(*this).z),		// temp vectors from quaternion
				v2(q.x, q.y, q.z);

	float		dot;		// temp dot result

	if (v1.IsZero() || v2.IsZero())		// if either vector components are 0, dot and cross product not defined
	{
		dot = 0;

		v1.Scale(q.w);
		v2.Scale((*this).w);

		v1 += v2;
	}
	else
	{
		Vector3D	nv1, nv2;

		nv1 = v1;
		nv1.Normalize();		// get normalized versions of v1 and v2 for dot product

		nv2 = v2;
		nv2.Normalize();

		dot	= nv1.DotProduct(nv2);	// get dot product

		v1.Scale(q.w);
		v2.Scale((*this).w);

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// this must also be normalized
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		v1 += v2 + v1*v2;				// s1v2 + s2v1 + v1*v2
	}

	w = (*this).w * q.w - dot;		// s1s2 - v1.v2
	v1.GetValues(x,y,z);			// get the x,y,z values of resultant vector

	return(Quaternion(x,y,z,w));
}	// end operator *
*/
////////

Quaternion& Quaternion::operator=(const Quaternion &q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;

	return(*this);
}	// end operator =

/////////

Quaternion& Quaternion::operator*=(const Quaternion &q)
{
	assert(false);		// make sure nobody calls this yet
	// same as *
	//%%%%%%%%%%%%%%%%%%
	// Unimplemented
	//%%%%%%%%%%%%%%%%%%

	return(*this);
}	// end operator *=

// -------------------------- debug
#include <iostream>
using namespace std;
void Quaternion::Print()
{
	cout << "Quaternion is " << x << " " << y << " " << z << " w is " << w << endl;
}

////////////////////////////////

#endif	// __QUATERNION_CPP_
