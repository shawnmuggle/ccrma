//
//  MRObjects.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/14/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "MRObjects.h"

using namespace std;

MRBlob::MRBlob(float aX, float aY)
{
    size = 100 + 100 * (rand() / (float)RAND_MAX);
    
    position = Vector3<float>(aX, aY, 0.0f);
    numSlices = 20;
    phases = vector<float>(numSlices, 0);
    
    SplinePoint *prevPoint = new SplinePoint(Vector3<float>(position.x, position.y, 0.0f), Vector3<float>(position.x + size / 2.0f, position.y, 0.0f));
    SplinePoint *nextPoint;
    float theta;
    for (int i = 1; i <= numSlices; i++)
    {
        theta = 2 * M_PI * ((float)i / numSlices);
        float amplitude = size / 2;
        
        nextPoint = new SplinePoint(Vector3<float>(position.x, position.y, 0.0f), 
                                    Vector3<float>(position.x + cosf(theta) * amplitude, position.y + sinf(theta) * amplitude, 0.0f));
        prevPoint->setTangentWithControlPoint(nextPoint);
        spline.addPoint(prevPoint);
        prevPoint = nextPoint;
    }
    spline.lastPoint->nextPoint = spline.firstPoint;
    spline.firstPoint->prevPoint = spline.lastPoint;
    spline.lastPoint->setTangentWithControlPoint(spline.firstPoint);
}

void MRBlob::draw()
{
    spline.draw();
}

void MRBlob::update()
{
    PhysicsEntity::update();
    
    // TODO: Push spline points in and out with randomly incremented sine waves using phases vector
    SplinePoint *point = spline.firstPoint;
    while (point != NULL)
    {
        point->addForce((point->position - position) * 0.02 * (1 - 2 * (rand() / (float)RAND_MAX)));
        point = point->nextPoint;
        
        // Protect against loops! (This is definitely going to happen because we set up a loop
        if (point == spline.firstPoint)
            break;
    }
    
    spline.update();
}