//
//  MRSplines.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/12/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "MRSplines.h"

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
    if (nextPoint && prevPoint)
        return (nextPoint->position - prevPoint->position) / 2;
    else if (controlPoint)
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
    // In case this spline is a loop, disconnect the end from the beginning before destructing it.
    firstPoint->prevPoint->nextPoint = NULL;
    firstPoint->prevPoint = NULL;
    
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
        // We make sure that even if some other spline connected up to our last point, we keep that connection to the new last point.
        SplinePoint *temp = lastPoint->nextPoint;
        lastPoint->nextPoint = newPoint;
        newPoint->prevPoint = lastPoint;
        if (temp)
        {
            newPoint->nextPoint = temp;
            temp->prevPoint = newPoint;
        }
        
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
    // draw curves!
    glBegin(GL_LINES);
    SplinePoint *point = firstPoint;
    while (point != NULL)
    {
        point->draw();
        point = point->nextPoint;

        // Protect against looped splines
        if (point == firstPoint)
            break;
    }
    glEnd();

    // draw points!
//    glPointSize(5.0f);
//    glBegin(GL_POINTS);
//    point = firstPoint;
//    while (point != NULL)
//    {
//        glVertex2f(point->position.x, point->position.y);
//        point = point->nextPoint;
//
//        // Protect against looped splines
//        if (point == firstPoint)
//            break;
//    }
//    glEnd();
}

void Spline::update()
{
    SplinePoint *point = firstPoint;
    while (point != NULL)
    {
        Vector3<float> homeForce = point->vectorToHome() * 0.1;
        if (homeForce.magnitude() > 100)
            printf("WHAT\n");
        point->addForce(homeForce);
        
        point->update();
        point = point->nextPoint;

        // Protect against looped splines
        if (point == firstPoint)
            break;
    }
}
