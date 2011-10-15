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
#include "MRSplines.h"
#include "MRObjects.h"

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
    void setForcePoint(Vector3<float> point);
    int size();
protected:
    void extendSplinesWithPointAndAngle(Vector3<float> pointToAdd, float angle);
    void applyTaperForces(Spline *spline);
    void applyPointForce(Spline *spline);
    
    Spline *topSpline;
    Spline *bottomSpline;
    
    std::vector< Vector3<float> > linePoints;
    float width;
    
    Vector3<float> forcePoint;
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
    
    std::vector<MRBlob *> blobs;
};


#endif
