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

Blob2D::Blob2D(Vector3<float> aPosition)
{
    position = aPosition;
    size = 100 + 100 * (rand() / (float)RAND_MAX);
}

float getSlopeBetweenPoints(Vector3<float> p1, Vector3<float> p2)
{
    // We assume that p1 is to the left of p2 already, and we don't have to sort the points here

    // slope calculation for obtaining a Catmull-Rom Spline
    return (p2.y - p1.y) / (p2.x - p1.x);
    
    // finite difference

}

void Blob2D::generatePoints()
{
    int numPoints = 8;
    for ( int i = 0; i < numPoints; i++ )
    {
        float rads = 2 * M_PI * (i / (float)numPoints);
        float distance = 0.8 + 0.2 * (rand() / (float)RAND_MAX);
        
        Vector3<float> point( distance * cos(rads), distance * sin(rads), 0);
        initialPoints.push_back(point);
        
        
//        Vector3<float> point( 2.5 - 5 * (i / ((float) numPoints - 1)), rand() / (float)RAND_MAX, 0);
//        initialPoints.push_back(point);
        
    }
    
    
    
//    points = initialPoints;
//    return;
    

    // Cubic hermite spline interpolation, GO
    int numInterpolationPoints = 30;
    for (int i = 0; i < initialPoints.size() - 1; i++)
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
        
        
        printf("Prevpoint is %f, %f\n", prevPoint.x, prevPoint.y);
        printf("Point is %f, %f\n", point.x, point.y);
        printf("Nextpoint is %f, %f\n", nextPoint.x, nextPoint.y);
        printf("NextNextpoint is %f, %f\n", nextNextPoint.x, nextNextPoint.y);
        float m0 = getSlopeBetweenPoints(prevPoint, nextPoint); // Slope at point
        float m1 = getSlopeBetweenPoints(point, nextNextPoint); // Slope at nextPoint        
        
//        if ((point.x > prevPoint.x && point.x > nextPoint.x) ||
//            (point.x < prevPoint.x && point.x < nextPoint.x))
//            m0 = -m0;

//        if ((nextPoint.x > point.x && nextPoint.x > nextNextPoint.x) ||
//            (nextPoint.x < nextNextPoint.x && nextPoint.x < nextNextPoint.x))
//            m1 = -m1;

        
        printf("m0 is %f\n", m0);
        printf("m1 is %f\n", m1);
        printf("---\n");
        
        Vector3<float> p0 = point;
        Vector3<float> p1 = nextPoint;
        if ( p0.x > p1.x)
        {
            p0 = nextPoint;
            p1 = point;
            
            float temp = m0;
            m0 = m1;
            m1 = temp;
            
            reverse = true;
        }
        
        float xRange = p1.x - p0.x;
        for (int j = 0; j < numInterpolationPoints; j++) {
            Vector3<float> interpolationPoint;
            float t = (j / (float)numInterpolationPoints);
            float p_t = ((2 * pow(t, 3) - 3 * pow(t, 2) + 1) * p0.y + 
                         (pow(t, 3) - 2 * pow(t, 2) + t) * m0 + 
                         (-2 * pow(t, 3) + 3 * pow(t, 2)) * p1.y + 
                         (pow(t, 3) - pow(t, 2)) * m1);
            interpolationPoints.push_back(Vector3<float>(p0.x + t * xRange, p_t, 0.0));
        }
        
        if (!reverse)
        {
            for ( vector< Vector3<float> >::iterator itr = interpolationPoints.begin(); itr != interpolationPoints.end(); itr++)
            {
                points.push_back(*itr);
            }
        }
        else
        {
            for ( vector< Vector3<float> >::reverse_iterator itr = interpolationPoints.rbegin(); itr != interpolationPoints.rend(); itr++)
            {
                points.push_back(*itr);
            }
        }
    }
}

void Blob2D::draw()
{
    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);
    glScalef(size, size, 0);
    glPointSize(3.0);
    glBegin(GL_POINTS);
    int counter = 0;
    for ( vector< Vector3<float> >::iterator i = points.begin(); i != points.end(); i++)
    {
        glColor4f(counter++ / (float)points.size(), 0.0, 0.0, 1.0);
        Vector3<float> point = *i;
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
    
    
    glPointSize(10.0);
    glBegin(GL_POINTS);
    counter = 0;
    for ( vector< Vector3<float> >::iterator i = initialPoints.begin(); i != initialPoints.end(); i++)
    {
        glColor4f(counter++ / (float)initialPoints.size(), 0.0, 0.0, 0.3);
        Vector3<float> point = *i;
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
    

    glPopMatrix();
}

void Blob2D::update()
{
    
}

Art3::Art3(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;
    
    targetPoint = Vector3<float>(width / 2.0, height / 2.0, 0.0);
    prevTargetPoint = targetPoint;
    
    int numBlobs = 1;
    for (int i = 0; i < numBlobs; i++)
    {
        //Blob2D *blob = new Blob2D( Vector3<float>( width * (rand() / (float)RAND_MAX), height * (rand() / (float)RAND_MAX), 0.0 ) );
        Blob2D *blob = new Blob2D( Vector3<float>( width / 2.0, height / 2.0, 0.0 ) );
        blob->generatePoints();
        blobs.push_back(blob);
    }
}

Art3::~Art3()
{
#warning I need to actually delete the blobs here, as they are not deleted by vector::clear
    blobs.clear();
}

void Art3::setTargetPoint(float x, float y)
{
    prevTargetPoint = targetPoint;
    targetPoint.setAll(x, y, 0.0);
}

void Art3::draw()
{
    for (vector<Blob2D *>::iterator i = blobs.begin(); i != blobs.end(); i++)
    {
        Blob2D *blob = *i;
        blob->draw();
    }
}

void Art3::update()
{
    
}