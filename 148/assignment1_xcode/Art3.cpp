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

SplinePoint::SplinePoint(Vector3<float> aPosition)
{
    position = aPosition;
    nextPoint = prevPoint = NULL;
}

void SplinePoint::setTangentWithControlPoint(Vector3<float> controlPoint)
{
    tangentVector = controlPoint - position;
}

float SplinePoint::slope()
{
    return (tangentVector.y / tangentVector.x);
}

//void SplinePoint::drawHermiteCurveToNextPoint(int numInterpolatedPoints)
//{
//    SplinePoint *p1 = this;
//    SplinePoint *p2 = nextPoint;
//    
//    if (p1->position.x > p2->position.x)
//    {
//        SplinePoint *temp = p1;
//        p1 = p2;
//        p2 = temp;
//    }
//    
//    float s;
//    Vector3<float> prevPoint = p1->position; // NOTE: This might result in a redundant line from position to position.
//    for (int i = 0; i < numInterpolatedPoints; i++)
//    {
//        s = (float)i / (numInterpolatedPoints - 1);
//        float h1 =  2*pow(s,3) - 3*pow(s,2) + 1;          // calculate basis function 1
//        float h2 = -2*pow(s,3) + 3*pow(s,2);              // calculate basis function 2
//        float h3 =    pow(s,3) - 2*pow(s,2) + s;          // calculate basis function 3
//        float h4 =    pow(s,3) -  pow(s,2);               // calculate basis function 4
//        
//        Vector3<float> p;
//        p.x = p1->position.x + s * (p2->position.x - p1->position.x);
//        p.y = (p1->position.y * h1 +
//               p2->position.y * h2 +
//               p1->slope() * (p2->position.x - p1->position.x) * h3 +
//               p2->slope() * (p2->position.x - p1->position.x) * h4);
//        
//        glVertex3f(prevPoint.x, prevPoint.y, 0.0f);
//        glVertex3f(p.x, p.y, 0.0);
//        prevPoint = p;
//    }
//}


void SplinePoint::drawHermiteCurveToNextPoint(int numInterpolatedPoints)
{
    SplinePoint *p1 = this;
    SplinePoint *p2 = nextPoint;
    
//    bool reverse = false;
//    if (p1->position.x > p2->position.x)
//    {
//        SplinePoint *temp = p1;
//        p1 = p2;
//        p2 = temp;
//        reverse = true;
//    }
    
    Vector3<float> p;
    Vector3<float> prevP = p1->position;
    float t;
    for (int i = 0; i < numInterpolatedPoints; ++i)
    {
        t = static_cast<float>(i) / (numInterpolatedPoints - 1);
        Vector3<float> a = p1->position;
        Vector3<float> b = p1->position + p1->tangentVector / 3.0;
        Vector3<float> c = p2->position - p2->tangentVector / 3.0;
//        if (reverse)
//        {
//            b = p1->position + p1->tangentVector / 3.0;
//            c = p2->position - p2->tangentVector / 3.0;
//        }

        Vector3<float> d = p2->position;
        bezier(p, a, b, c, d, t);
        glVertex2f(prevP.x, prevP.y);
        glVertex2f(p.x, p.y);
        prevP = p;
    }
}


void SplinePoint::drawBezierCurveToNextPoint(int numInterpolatedPoints)
{
    SplinePoint *p1 = this;
    SplinePoint *p2 = nextPoint;
    
    if (p1->position.x > p2->position.x)
    {
        SplinePoint *temp = p1;
        p1 = p2;
        p2 = temp;
    }
    
    Vector3<float> p;
    Vector3<float> prevP = p1->position;
    float t;
    for (int i = 0; i < numInterpolatedPoints; ++i)
    {
        t = static_cast<float>(i) / (numInterpolatedPoints - 1);
        bezier(p, p1->position, p1->position + p1->tangentVector, p2->position + p2->tangentVector, p2->position, t);
        glVertex2f(prevP.x, prevP.y);
        glVertex2f(p.x, p.y);
        prevP = p;
    }
}

void SplinePoint::draw()
{
    glColor4f(0.2f, 0.9f, 0.1f, 1.0f);
    glVertex2f(position.x, position.y);
    glVertex2f((position + tangentVector).x, (position + tangentVector).y);
    
    if (nextPoint)
    {
        glColor4f(0.1f, 0.3f, 0.8f, 1.0f);
        glVertex2f(position.x, position.y);
        glVertex2f(nextPoint->position.x, nextPoint->position.y);

        glColor4f(0.9f, 0.3f, 0.01f, 1.0f);
        drawHermiteCurveToNextPoint(100);
        
//        glColor4f(0.1f, 0.9f, 0.8f, 1.0f);
//        drawBezierCurveToNextPoint(100);
    }
}

void SplinePoint::update()
{
    
}

Spline::Spline()
{
    firstPoint = lastPoint = NULL;
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

void Spline::addPoint(Vector3<float> position)
{
    SplinePoint *newPoint = new SplinePoint(position);

    if (!firstPoint)
    {
        firstPoint = newPoint;
        lastPoint = newPoint;
    }
    else
    {
        lastPoint->nextPoint = newPoint;
        newPoint->prevPoint = newPoint;
        lastPoint = newPoint;
    }
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

Blob2D::Blob2D(Vector3<float> aPosition)
{
    position = aPosition;
    size = 100 + 100 * (rand() / (float)RAND_MAX);
}

float getSlopeBetweenPoints(Vector3<float> p0, Vector3<float> p1, Vector3<float> p2)
{
    // We assume that p1 is to the left of p2 already, and we don't have to sort the points here

    // slope calculation for obtaining a Catmull-Rom Spline
    return (p2.y - p0.y) / (p2.x - p0.x);
    
    // finite difference
//    return (p2.y - p1.y) / (2 * (p2.x - p1.x)) + (p1.y - p0.y) / (2 * (p1.x - p0.x));

}

void Blob2D::addPoint(Vector3<float> point)
{
    initialPoints.push_back(point);
}


void Blob2D::generatePoints()
{
//    int numPoints = 8;
//    for ( int i = 0; i < numPoints; i++ )
//    {
//        float rads = 0.4 + 2 * M_PI * (i / (float)numPoints);
//        float distance = 0.8 + 0.2 * (rand() / (float)RAND_MAX);
//        
//        Vector3<float> point( distance * cos(rads), distance * sin(rads), 0);
//        initialPoints.push_back(point);
//        
//        
////        Vector3<float> point( -2.5 + 5 * (i / ((float) numPoints - 1)), -2.5 + 5 * (rand() / (float)RAND_MAX), 0);
////        initialPoints.push_back(point);
//        
//    }
    
    
    
//    points = initialPoints;
//    return;
    
    if ( initialPoints.size() <= 2 )
        return;
    
    // Cubic hermite spline interpolation, GO
    int numInterpolationPoints = 30;
    for (int i = 1; i < initialPoints.size() - 2; i++)
    {
        vector< Vector3<float> > interpolationPoints;

        int prevPointIndex = i - 1;
        if (prevPointIndex < 0)
            prevPointIndex += initialPoints.size();
        int nextPointIndex = (i + 1) % initialPoints.size();
        int nextNextPointIndex = (i + 2) % initialPoints.size();
        Vector3<float> prevPoint = initialPoints[prevPointIndex];
        Vector3<float> point = initialPoints[i];
        Vector3<float> nextPoint = initialPoints[nextPointIndex];
        Vector3<float> nextNextPoint = initialPoints[nextNextPointIndex];
        
        bool reverse = false;
        
        
//        printf("Prevpoint is %f, %f\n", prevPoint.x, prevPoint.y);
//        printf("Point is %f, %f\n", point.x, point.y);
//        printf("Nextpoint is %f, %f\n", nextPoint.x, nextPoint.y);
//        printf("NextNextpoint is %f, %f\n", nextNextPoint.x, nextNextPoint.y);
//        printf("---\n");
//        float m0 = getSlopeBetweenPoints(prevPoint, point, nextPoint); // Slope at point
//        float m1 = getSlopeBetweenPoints(point, nextPoint, nextNextPoint); // Slope at nextPoint        
        
//        if ((point.x > prevPoint.x && point.x > nextPoint.x) ||
//            (point.x < prevPoint.x && point.x < nextPoint.x))
//        {
//            m0 = -m0;
//            printf("YUP\n");
//        }
//
//        if ((nextPoint.x > point.x && nextPoint.x > nextNextPoint.x) ||
//            (nextPoint.x < nextNextPoint.x && nextPoint.x < nextNextPoint.x))
//            m1 = -m1;

        
//        printf("m0 is %f\n", m0);
//        printf("m1 is %f\n", m1);
//        printf("---\n");
        
        Vector3<float> p0 = point;
        Vector3<float> p1 = nextPoint;
//        if ( p0.x > p1.x)
//        {
//            p0 = nextPoint;
//            p1 = point;
//            
//            float temp = m0;
//            m0 = m1;
//            m1 = temp;
//            
//            printf("HEEEYYY\n");
//            
//            reverse = true;
//        }
        
        float xRange = p1.x - p0.x;
        for (int j = 0; j < numInterpolationPoints; j++) {
            Vector3<float> interpolationPoint;
            float t = (j / (float)numInterpolationPoints);
//            float p_t = ((2.0 * pow(t, 3) - 3.0 * pow(t, 2) + 1.0) * p0.y + 
//                         (pow(t, 3) - 2 * pow(t, 2) + t) * m0 + 
//                         (-2.0 * pow(t, 3) + 3.0 * pow(t, 2)) * p1.y + 
//                         (pow(t, 3) - pow(t, 2)) * m1);
//            printf("j: %d, t: %f, p: %f\n", j, t, p_t);
//            
//            interpolationPoints.push_back(Vector3<float>(p0.x + t * xRange, p_t, 0.0));
            
            /*
             q(t) = 0.5 *(  	(2 * P1) +
             (-P0 + P2) * t +
             (2*P0 - 5*P1 + 4*P2 - P3) * t2 +
             (-P0 + 3*P1- 3*P2 + P3) * t3)
             */

            Vector3<float> newPoint = ((point * 2.0) + 
                                       (prevPoint * -1.0 + nextPoint) * t +
                                       (prevPoint * 2.0 - point * 5.0 + nextPoint * 4.0 - nextNextPoint) * pow(t, 2) +
                                       (prevPoint * -1.0 + point * 3.0 - nextPoint * 3.0 + nextNextPoint) * pow(t, 3));
            
            newPoint *= 0.5;
            newPoint.x = p0.x + t * xRange;
            interpolationPoints.push_back(newPoint);                                       
        }
        
        if (!reverse)
        {
            for ( vector< Vector3<float> >::iterator itr = interpolationPoints.begin(); itr != interpolationPoints.end(); itr++)
            {
                points.push_back(*itr);
            }
        }
//        else
//        {
//            for ( vector< Vector3<float> >::reverse_iterator itr = interpolationPoints.rbegin(); itr != interpolationPoints.rend(); itr++)
//            {
//                points.push_back(*itr);
//            }
//        }
    }
}

void Blob2D::draw()
{
    points.clear();
    generatePoints();

    glPushMatrix();

//    glTranslatef(position.x, position.y, position.z);
//    glScalef(size, size, 0);
    glPointSize(3.0);
    glBegin(GL_LINE_STRIP);
    int counter = 0;
    for ( vector< Vector3<float> >::iterator i = points.begin(); i != points.end(); i++)
    {
        glColor4f(counter++ / (float)points.size(), 0.0, 0.0, 1.0);
        Vector3<float> point = *i;
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
    
    
//    glPointSize(10.0);
//    glBegin(GL_POINTS);
//    counter = 0;
//    for ( vector< Vector3<float> >::iterator i = initialPoints.begin(); i != initialPoints.end(); i++)
//    {
//        glColor4f(counter++ / (float)initialPoints.size(), 0.0, 0.0, 0.3);
//        Vector3<float> point = *i;
//        glVertex3f(point.x, point.y, point.z);
//    }
//    glEnd();
    

    glPopMatrix();
}

void Blob2D::update()
{
    
}

Art3::Art3(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;
    mouseDown = false;
    
    int numBlobs = 1;
    for (int i = 0; i < numBlobs; i++)
    {
        //Blob2D *blob = new Blob2D( Vector3<float>( width * (rand() / (float)RAND_MAX), height * (rand() / (float)RAND_MAX), 0.0 ) );
        Blob2D *blob = new Blob2D( Vector3<float>( width / 2.0, height / 2.0, 0.0 ) );
        //blob->generatePoints();
        blobs.push_back(blob);
    }
}

Art3::~Art3()
{
#warning I need to actually delete the blobs here, as they are not deleted by vector::clear
    blobs.clear();
}

void Art3::mouseButton(int button, int state, int x, int y)
{
    if (state == 0)
    {
        //blobs[0]->addPoint(Vector3<float>(x, y, 0.0));
        spline.addPoint(Vector3<float>(x, y, 0.0));
        mouseDown = true;
    }
    else if (state == 1)
    {
        mouseDown = false;
    }
}

void Art3::setTargetPoint(float x, float y)
{
    targetPoint.setAll(x, y, 0.0);
    if (mouseDown)
    {
        Vector3<float> mousePosition(x, y, 0.0);
        spline.lastPoint->setTangentWithControlPoint(mousePosition);
    }
}

void Art3::draw()
{
    for (vector<Blob2D *>::iterator i = blobs.begin(); i != blobs.end(); i++)
    {
        Blob2D *blob = *i;
        blob->draw();
    }
    
    spline.draw();
}

void Art3::update()
{
    
}