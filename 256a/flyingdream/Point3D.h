
/*	Point class

	Last Modified	25/4/00
*/

#ifndef __POINT3D_H_
#define __POINT3D_H_

#include "MathsStruct.h"

class Vector3D;

////////////////////////////

class	Point3D
{
private:

	float	x,y,z;		// x, y, z

public:

	Point3D();
	Point3D(const float x, const float y, const float z);		// create a new point

//	~Point3D();

////////////

	void	Round();																// round values within tolerance

	void	GetValues(float &x, float &y, float &z)						const;		// get the points
	void	SetValues(const float x, const float y, const float z);					// set the point

////////////

	void	Negate();											// negate the point

	double	Distance(const Point3D &p)				const;		// get the distance between the points
	double	DistanceSquared(const Point3D &p)		const;		// get distance SQUARED between the points

	void	Translate(const Vector3D &v);						// translate this point by vector

////////////

	// opengl
	void	glTranslate(void)						const;		// translate current matrix to this point
	void	glVertex(void)							const;		// creates a opengl vertex of this point

////////////

	Point3D		operator+(const Point3D &p)			const;
	Point3D		operator-(const Point3D &p)			const;
	Point3D		operator*(const float s)			const;		// scale point
	Point3D		operator-(void)						const;		// negation

	Point3D		operator+(const Vector3D &v)		const;		// translate by vector
	Point3D&	operator+=(const Vector3D &v);					// translate by vector
	
	Point3D&	operator=(const Point3D &p);					// copy constructor
	Point3D&	operator+=(const Point3D &p);
	Point3D&	operator-=(const Point3D &p);
	Point3D&	operator*=(const float s);
	
	int			operator==(const Point3D &p)		const;		// equality operator
	int			operator!=(const Point3D &p)		const;

////////////

	// debug
	void	Print(void);

};	// end class Point3D

////////////////////////////

#endif	// __POINT3D_H_