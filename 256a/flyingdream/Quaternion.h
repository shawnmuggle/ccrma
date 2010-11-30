/////////// Taken from http://www.gamedev.net/reference/programming/features/qpowers/page7.asp
/////////// NOT MY CODE (though I think I understand how it all works at this point, after lotsa reading)


/*	Quaternion Library

	Last Modified	20/4/00
*/

#ifndef __QUATERNION_H_
#define __QUATERNION_H_

#include "vector3d.h"
#include "Matrix.h"

/////////////////////////////////

class	Quaternion		// a always normalized Quaternion class
{
public:

	float	x,y,z,w;			// quaternion representation (w, xi, yj, zk)

	inline double	Norm(void)					const;		// get the norm N^2
	
public:

	Quaternion();												// constructor a quaternion with zero rotation
	Quaternion(float x, float y, float z, float w);				// constructs a quaternion from given values. w is in rad, and quat will be normalized. Will perform a valid check
	Quaternion(const float x, const float y, const float z);	// construct a quaternion from euler angles in degrees
	Quaternion(const Vector3D &v, const float angle);			// construct a quaternion from axis angle representation, angle in radians

//	~Quaternion();

////////

	void	Identity(void);														// set this quaternion to the identity quaternion

	void	Zero_Clamp(void);													// clamp any values very close to 0

	void	Normalize(void);													// normalize this quaternion

	void	GetValues(float &x, float &y, float &z, float &w)			const;	// gets the value of this quaternion
	
////////

	void	AxisToQuat(const Vector3D &v, const float angle);					// convert an axis angle to quaternion, angle is in radians
	void	EulerToQuat(const float x, const float y, const float z);			// convert euler angles to quaternion, euler in degrees

	void	GetAxisAngle(Vector3D &v, float &angle)						const;	// get an axis angle from this quaternion, angle returned in radians

	void	GetEulerAngles(float &x, float &y, float &z)				const;	// get the Euler angles(deg) representing this quaternion

	void	GetMatrix(Matrix &m)										const;	// get the 4x4 rotation matrix representation of this quaternion

////////

	// operators
	Quaternion	operator*(const Quaternion &q)				const;			// multiplication

	Quaternion&	operator=(const Quaternion &q);								// copy constructor

	//%%%%%%%%%%%%%%%
	// UnImplemented
	//%%%%%%%%%%%%%%%
	Quaternion&	operator*=(const Quaternion &q);

//////////

	// debug
	void	Print(void);

};	// end class Quaternion

/////////////////////////////////

#endif	// __QUATERNION_H_
