//
//  MRSplines.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/12/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#ifndef Assignment1_MRSplines_h
#define Assignment1_MRSplines_h

#include <vector>

#include "MRMath.h"
#include "MRPhysics.h"
#include "MRGraphics.h"

class SplinePoint : public DrawableEntity, public PhysicsEntity
{
public:
    SplinePoint(Vector3<float> aPosition, Vector3<float> aHome);
    virtual void draw();
    virtual void update();
    void setTangentWithControlPoint(SplinePoint *aControlPoint);
    float slope();
    Vector3<float> tangent();
    Vector3<float> vectorToHome();
    Vector3<float> vectorToStartPoint();
    void drawHermiteCurveToNextPoint(int numInterpolatedPoints);
    void drawBezierCurveToNextPoint(int numInterpolatedPoints);
    
    SplinePoint *nextPoint;
    SplinePoint *prevPoint;
protected:
    Vector3<float> tangentVector;
    SplinePoint *controlPoint;
    Vector3<float> home;
    Vector3<float> startPosition;
};

class Spline : public DrawableEntity, public PhysicsEntity
{
public:
    Spline();
    ~Spline();
    void addPoint(SplinePoint *newPoint);
    SplinePoint *end();
    int size();
    float length();
    virtual void draw();
    virtual void update();
    //private:
    SplinePoint *firstPoint;
    SplinePoint *lastPoint;
protected:
    int numPoints;
    float totalLength;
};


#endif
