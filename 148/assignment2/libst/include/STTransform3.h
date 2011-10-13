// STColor3f.h
#ifndef __STTRANSFORM3_H__
#define __STTRANSFORM3_H__

// Forward-declare libst types.
#include "stForward.h"

#include <vector>

class Matrix {
public:
    inline Matrix(int aWidth, int aHeight);
    
    int width;
    int height;

    // I'm not using multi-dimensional arrays for matrices based on this article: http://cplusplus.com/forum/articles/17108/
    std::vector< std::vector<float> > elements;  // row-major
    void clear();
};

/**
*  Class representing a 3x3 matrix that can be used to
*  apply transforms to 2D points and vectors
*/
class STTransform3
{
	// Fill in your class definition here. STTransform3 should implement a 3x3 matrix that
	// you can use to modify 2D vertices with homogenous coordinates (hence the 3x3 size
	// rather than 2x2). This class doesn't need to be complicated. Other than basic 
	// accessor functions, you'll probably want to implement multiplication between transforms.
	// You have a lot of flexibility as to how you implement this class, but you will need to
	// follow the conventions used in the rest of libst.
public:
    STTransform3();
    STTransform3(Matrix m);
    Matrix matrix;
    void loadIdentity();
    void print() const;
};

STVector3 operator*(const STTransform3 &left, const STVector3 &right);
STTransform3 operator*(const STTransform3 &left, const STTransform3 &right);

#endif