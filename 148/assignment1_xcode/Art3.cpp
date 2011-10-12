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

SplinePoint::SplinePoint(Vector3<float> aPosition, Vector3<float> aHome)
{
    startPosition = position = aPosition;
    home = aHome;
    nextPoint = prevPoint = controlPoint = NULL;
}

void SplinePoint::setTangentWithControlPoint(SplinePoint * aControlPoint)
{
    controlPoint = aControlPoint;
}

// These two functions should probably be const
float SplinePoint::slope()
{
    return (tangent().y / tangent().x);
}

Vector3<float> SplinePoint::tangent()
{
    if (controlPoint)
        return controlPoint->position - position;
    else
        return Vector3<float>(0.0f, 0.0f, 0.0f);
}

Vector3<float> SplinePoint::vectorToHome()
{
    return home - position;
}

Vector3<float> SplinePoint::vectorToStartPoint()
{
    return startPosition - position;
}

void SplinePoint::drawHermiteCurveToNextPoint(int numInterpolatedPoints)
{
    SplinePoint *p1 = this;
    SplinePoint *p2 = nextPoint;

    Vector3<float> p;
    Vector3<float> prevP = p1->position;
    float t;
    for (int i = 1; i < numInterpolatedPoints; ++i)
    {
        t = static_cast<float>(i) / (numInterpolatedPoints - 1);
        Vector3<float> a = p1->position;
        Vector3<float> b = p1->position + p1->tangent() / 3.0;
        Vector3<float> c = p2->position - p2->tangent() / 3.0;

        Vector3<float> d = p2->position;
        bezier(p, a, b, c, d, t);
        glVertex2f(prevP.x, prevP.y);
        glVertex2f(p.x, p.y);
        prevP = p;
    }
}

void SplinePoint::draw()
{
    if (nextPoint)
    {
        glColor4f(0.9f, 0.3f, 0.01f, 1.0f);
        drawHermiteCurveToNextPoint(20);
    }
}

void SplinePoint::update()
{
    PhysicsEntity::update();
}

Spline::Spline()
{
    firstPoint = lastPoint = NULL;
    numPoints = 0;
    totalLength = 0;
}

Spline::~Spline()
{
    SplinePoint *point = firstPoint;
    SplinePoint *nextPoint;
    while (point != NULL)
    {
        nextPoint = point->nextPoint;
        delete point;
        point = nextPoint;
    }
}

void Spline::addPoint(SplinePoint *newPoint)
{
    if (!firstPoint)
    {
        firstPoint = newPoint;
        lastPoint = newPoint;
    }
    else
    {
        lastPoint->nextPoint = newPoint;
        newPoint->prevPoint = lastPoint;
        lastPoint = newPoint;
        totalLength += (lastPoint->position - lastPoint->prevPoint->position).magnitude();
    }
    
    numPoints++;
}

SplinePoint *Spline::end()
{
    return lastPoint;
}

int Spline::size()
{
    return numPoints;
}

float Spline::length()
{
    return totalLength;
}

void Spline::draw()
{
    glBegin(GL_LINES);
    SplinePoint *point = firstPoint;
    while (point != NULL)
    {
        point->draw();
        point = point->nextPoint;
    }
    glEnd();
}

void Spline::update()
{
    SplinePoint *point = firstPoint;
    while (point != NULL)
    {
        Vector3<float> homeForce = point->vectorToHome() * 0.1;
        point->addForce(homeForce);
        
        point->update();
        point = point->nextPoint;
    }
}

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
    applyTaperForces(topSpline);
    applyTaperForces(bottomSpline);
    
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
        if (lengthSoFar < 0.3 * spline->length())
        {
            taperMagnitude = 1.0 - (lengthSoFar / spline->length()) / 0.3;
        }
        else if (lengthSoFar > 0.7 * spline->length())
        {
            taperMagnitude = 1.0 - (1.0 - (lengthSoFar / spline->length())) / 0.3;
        }
        
        Vector3<float> taperForce = point->vectorToStartPoint() * 0.3 * taperMagnitude;
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
    if (state == 0)
    {
        mouseDown = true;
        currentPath = new CurvyTaperedPath2D(30.0f);
        paths.push_back(currentPath);
    }
    else if (state == 1)
    {
        mouseDown = false;
        currentPath->finishSplines();
    }
}

void Art3::setTargetPoint(float x, float y)
{
    targetPoint.setAll(x, y, 0.0);
    if (mouseDown)
    {
        Vector3<float> mousePosition(x, y, 0.0);        
        currentPath->addPoint(mousePosition);
    }
}

void Art3::draw()
{
    for (vector<CurvyTaperedPath2D *>::iterator i = paths.begin(); i != paths.end(); i++)
    {
        CurvyTaperedPath2D *path = *i;
        path->draw();
    }
}

void Art3::update()
{
    for (vector<CurvyTaperedPath2D *>::iterator i = paths.begin(); i != paths.end(); i++)
    {
        CurvyTaperedPath2D *path = *i;
        path->update();
    }
}