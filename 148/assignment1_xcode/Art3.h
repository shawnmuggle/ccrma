//
//  Art3.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/6/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#ifndef Assignment1_Art3_h
#define Assignment1_Art3_h

#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

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

// class SplineLoop (keep lastPoint connected to firstPoint)

// class Blob2D (make a vaguely circular splineloop with wavy control points along it that animate in & out)

// A smoothly curving shape that follows a line strip and has tapered ends
class CurvyTaperedPath2D : public DrawableEntity, public PhysicsEntity
{
public:
    CurvyTaperedPath2D(float aWidth = 1.0f);
    ~CurvyTaperedPath2D();
    virtual void draw();
    virtual void update();
    void addPoint(Vector3<float> newPoint);
    void finishSplines();
    virtual SplinePoint *createNewSplinePoint(Vector3<float> linePoint, Vector3<float> splinePointPosition);
protected:
    void extendSplinesWithPointAndAngle(Vector3<float> pointToAdd, float angle);
    void applyTaperForces(Spline *spline);
    
    Spline *topSpline;
    Spline *bottomSpline;
    
    std::vector< Vector3<float> > linePoints;
    float width;
};

class Art3
{
public:
    Art3(int aWidth, int aHeight);
    ~Art3();
    void update();
    void draw();
    void mouseButton(int button, int state, int x, int y);
    void setTargetPoint(float x, float y);
private:
    int width;
    int height;
    bool mouseDown;
    Vector3<float> targetPoint;
    
    std::vector<CurvyTaperedPath2D *> paths;
    CurvyTaperedPath2D *currentPath;
};


#endif
