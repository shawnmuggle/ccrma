//
//  Math.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/2/11.
//  Copyright 2011 Stanford. All rights reserved.
//

#ifndef Assignment1_Math_h
#define Assignment1_Math_h

#include <math.h>

template <class T>
class Vector3 {
public: 
    // setting and getting (should maybe be through setters & getters?)
    void setAll(T newX, T newY, T newZ) { x = newX; y = newY; z = newZ; }
    T x;
    T y;
    T z;
    
    // operators
    Vector3<T> & operator += (const Vector3<T> & otherVector)
    {
        x += otherVector.x;
        y += otherVector.y;
        z += otherVector.z;
        
        return *this;
    }
    
    Vector3<T> & operator -= (const Vector3<T> & otherVector)
    {
        x -= otherVector.x;
        y -= otherVector.y;
        z -= otherVector.z;
        
        return *this;
    }
    
    Vector3<T> & operator *= (const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        
        return *this;
    }
    
    Vector3<T> operator- (const Vector3<T> & otherVector)
    {
        Vector3<T> result = *this;
        result -= otherVector;
        return result;
    }
    
    Vector3<T> operator* (const float scalar)
    {
        Vector3<T> result = *this;
        result *= scalar;
        return result;
    }

    float magnitude()
    {
        return sqrt(x * x + y * y + z * z);
    }
};

#endif