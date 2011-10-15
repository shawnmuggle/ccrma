//
//  Art3.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/6/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#include <iostream>

#include "Art3.h"

using namespace std;

CurvyTaperedPath2D::CurvyTaperedPath2D(float aWidth)
{
    width = aWidth;
    
    topSpline = new Spline();
    bottomSpline = new Spline();
}

CurvyTaperedPath2D::~CurvyTaperedPath2D()
{
    linePoints.clear();
}

void CurvyTaperedPath2D::draw()
{
    topSpline->draw();
    bottomSpline->draw();
    
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for ( vector< Vector3<float> >::iterator i = linePoints.begin(); i != linePoints.end(); i++)
    {
        Vector3<float> p = *i;
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void CurvyTaperedPath2D::extendSplinesWithPointAndAngle(Vector3<float> pointToAdd, float angle)
{
    Vector3<float> topPoint = Vector3<float>(pointToAdd.x - (width / 2.0) * -sin(angle), pointToAdd.y - (width / 2.0) * cos(angle), 0.0);
    SplinePoint *newTopSplinePoint = createNewSplinePoint(pointToAdd, topPoint);
    if (topSpline->size())
        topSpline->end()->setTangentWithControlPoint(newTopSplinePoint);
    topSpline->addPoint(newTopSplinePoint);
    
    Vector3<float> bottomPoint = Vector3<float>(pointToAdd.x + (width / 2.0) * -sin(angle), pointToAdd.y + (width / 2.0) * cos(angle), 0.0);
    SplinePoint *newBottomSplinePoint = createNewSplinePoint(pointToAdd, bottomPoint);
    if (bottomSpline->size())
        bottomSpline->end()->setTangentWithControlPoint(newBottomSplinePoint);
    bottomSpline->addPoint(newBottomSplinePoint);
}

void CurvyTaperedPath2D::addPoint(Vector3<float> newPoint)
{
    linePoints.push_back(newPoint);

    // We always add the previous point to the splines, so only continue if this is not the first point
    if (linePoints.size() < 2)
        return;
    
    // Compute the angle at the previous point, so that we can figure out how to tilt the top and bottom splines
    Vector3<float> pointToAdd;
    float angle = 0;
    if (linePoints.size() == 2)
    {
        vector< Vector3<float> >::iterator i = linePoints.end();
        Vector3<float> lastPoint = *(--i);
        pointToAdd = *(--i);
        angle = (lastPoint - pointToAdd).angle(); // Angle between the previous point and this one
    }
    else if (linePoints.size() > 2)
    {
        vector< Vector3<float> >::iterator i = linePoints.end();
        Vector3<float> lastPoint = *(--i);
        pointToAdd = *(--i);
        Vector3<float> prevPoint = *(--i);
        angle = (lastPoint - prevPoint).angle(); // Angle between the point before the previous one and this one.
    }
    
    extendSplinesWithPointAndAngle(pointToAdd, angle);
}

void CurvyTaperedPath2D::finishSplines()
{
    Vector3<float> pointToAdd;
    float angle = 0;
    vector< Vector3<float> >::iterator i = linePoints.end();
    pointToAdd = *(--i);
    Vector3<float> prevPoint = *(--i);
    angle = (pointToAdd - prevPoint).angle(); // Angle between the previous point and this one

    extendSplinesWithPointAndAngle(pointToAdd, angle);
}

void CurvyTaperedPath2D::update()
{
//    applyTaperForces(topSpline);
//    applyTaperForces(bottomSpline);
    
//    applyPointForce(topSpline);
//    applyPointForce(bottomSpline);
    
    topSpline->update();
    bottomSpline->update();
}

void CurvyTaperedPath2D::applyTaperForces(Spline *spline)
{
    float lengthSoFar = 0;
    SplinePoint *point = spline->firstPoint;
    while (point != NULL)
    {
        float taperMagnitude = 0;
        if (lengthSoFar < 0.1 * spline->length())
        {
            taperMagnitude = 1.0 - (lengthSoFar / spline->length()) / 0.1;
        }
        else if (lengthSoFar > 0.9 * spline->length())
        {
            taperMagnitude = 1.0 - (1.0 - (lengthSoFar / spline->length())) / 0.1;
        }
        
        Vector3<float> taperForce = point->vectorToStartPoint() * 0.3 * taperMagnitude;
        
        if (taperForce.magnitude() > 100)
            printf("WHOA\n");
        
        point->addForce(taperForce);
        
        point = point->nextPoint;
        if (point)
            lengthSoFar += (point->position - point->prevPoint->position).magnitude();
    }

}

SplinePoint *CurvyTaperedPath2D::createNewSplinePoint(Vector3<float> linePoint, Vector3<float> splinePointPosition)
{
    return new SplinePoint(linePoint, splinePointPosition);
}

void CurvyTaperedPath2D::setForcePoint(Vector3<float> point)
{
    forcePoint = point;
}

void CurvyTaperedPath2D::applyPointForce(Spline *spline)
{
    SplinePoint *point = spline->firstPoint;
    while (point != NULL)
    {
        Vector3<float> mouseVector = point->position - forcePoint;
        float mouseDistance = mouseVector.magnitude();
        
        Vector3<float> mouseDirection = mouseVector * (1.0 / mouseDistance);
        float pushMagnitude = 200.0 / pow(mouseDistance, 2);
        
        point->addForce(mouseDirection * pushMagnitude);

        point = point->nextPoint;
    }
}

int CurvyTaperedPath2D::size()
{
    return linePoints.size();
}

Art3::Art3(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;
    mouseDown = false;
}

Art3::~Art3()
{
}

void Art3::mouseButton(int button, int state, int x, int y)
{
    printf("BUTTON: %d\n", button);
    
//    if (button == 0)
//    {
//        if (state == 0)
//        {
//            mouseDown = true;
//            currentPath = new CurvyTaperedPath2D(30.0f);
//            paths.push_back(currentPath);
//        }
//        else if (state == 1)
//        {
//            mouseDown = false;
//            if (currentPath->size() == 0)
//            {
//                paths.pop_back();
//                currentPath = NULL;
//            }
//            else
//            {
//                currentPath->finishSplines();
//            }
//        }
//    }
    if (button == 0)
    {
        if (state == 0)
        {
            blobs.push_back(new MRBlob(x, y));
        }
    }
}

void Art3::setTargetPoint(float x, float y)
{
    targetPoint.setAll(x, y, 0.0);
    Vector3<float> mousePosition(x, y, 0.0);        
    if (mouseDown)
    {
        currentPath->addPoint(mousePosition);
    }
    for (vector<CurvyTaperedPath2D *>::iterator i = paths.begin(); i != paths.end(); i++)
    {
        CurvyTaperedPath2D *path = *i;
        path->setForcePoint(mousePosition);
    }    
}

void Art3::draw()
{
    for (vector<CurvyTaperedPath2D *>::iterator i = paths.begin(); i != paths.end(); i++)
    {
        CurvyTaperedPath2D *path = *i;
        path->draw();
    }
    for (vector<MRBlob *>::iterator i = blobs.begin(); i != blobs.end(); i++)
    {
        MRBlob *blob = *i;
        blob->draw();
    }
}

void Art3::update()
{
    for (vector<CurvyTaperedPath2D *>::iterator i = paths.begin(); i != paths.end(); i++)
    {
        CurvyTaperedPath2D *path = *i;
        path->update();
    }
    for (vector<MRBlob *>::iterator i = blobs.begin(); i != blobs.end(); i++)
    {
        MRBlob *blob = *i;
        blob->update();
    }
}