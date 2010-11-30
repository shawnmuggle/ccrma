/*	4x4 Matrix

	Last Modified	14/4/00
*/

#ifndef __MATRIX_H_
#define	__MATRIX_H_

class Vector3D;
class Point3D;

//////////////////////////////

class	Matrix
{
private:

	float	m[16];		// RIGHT HANDED column major matrix

	inline	void Verify_Matrix();				// force last row of matrix to be 0 0 0 1 to be a homogenous matrix

public:

	Matrix();											// creates an 4x4 identity matrix
	Matrix(const float m[16]);
	Matrix(const Vector3D &v);							// generate a transformation matrix of translation v
	Matrix(const float angle, const Vector3D &v);		// generate a rotation matrix of angle (rad) around an axis of rotation v

//	~Matrix();

///////////

	void	Zero_Clamp(void);		// clamp any values in the matrix to 0

	void	GetValues(float m[16])								const;			// get the values of this matrix
	void	GetTranslation(Point3D	&p)							const;			// get the translation portion of the 4x4 homogenous transformation matrix

	//%%%%%%%%%%%%%
	// NOT WORKING
	//%%%%%%%%%%%%%
	void	GetEulerAngles(float &x, float &y, float &z)		const;			// get euler values of rotation order x,y,z of matrix

	void	SetValues(const float m[16]);										// set the values of this matrix, last row always 0 0 0 1 regardless of what is passed in
	void	SetValues(const Point3D &p);										// create a transformation matrix that translates to the position p
	void	SetValues(const Vector3D &v);										// create a transformation matrix with the translation vector
	void	SetValues(const float angle, const Vector3D &v);					// create a rotation matrix with angle (rad) around axis of rotation vector

	void	SetValues_RotX(const float angle);									// generate a 4x4 rotation matrix with angle (degree) around respective axis
	void	SetValues_RotY(const float angle);
	void	SetValues_RotZ(const float angle);
	void	SetValues_Rot(const float x, const float y, const float z);			// generates a final 4x4 transformation matrix of rotation around x ,y, z axis (deg)

///////////

	//%%%%%%%%%%%%%%%%%
	// NOT IMPLEMENTED
	//%%%%%%%%%%%%%%%%%
	void	Inverse(void);			// inverse the matrix

	void	LoadIdentity(void);		// set this matrix to the identity matrix

	void	Negate(void);			// negate the matrix;

	void	Transpose(void);		// transpose the matrix

///////////

	Matrix		operator*(const Matrix &m)		const;		// multiplication with a matrix
	Matrix		operator*(const float s)		const;		// scale matrix by s
//	Matrix		operator+(const Matrix &m)		const;
//	Matrix		operator-(const Matrix &m)		const;
	Matrix		operator-()						const;		// negation

	Vector3D	operator*(const Vector3D &v)	const;		// pre-multiply by column vector
	Point3D		operator*(const Point3D &p)		const;		// pre-multiply by point

	Matrix&		operator=(const Matrix &m);					// copy constructor
//	Matrix&		operator+=(const Matrix &m);
//	Matrix&		operator-=(const Matrix &m);
	Matrix&		operator*=(const Matrix &m);
	Matrix&		operator*=(const float s);					// scale matrix by s
	
	int			operator==(const Matrix &m)		const;		// equality
	int			operator!=(const Matrix &m)		const;	

//------- debug

	void		Print() const;

};	// end class Matrix

//////////////////////////////

#endif	// __MATRIX_H_
