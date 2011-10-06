//
//  Art1.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/2/11.
//  Copyright 2011 Stanford. All rights reserved.
//

#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Art1.h"

TriangleThing::TriangleThing(float x, float y, float z)
{
    position.setAll(x + 500, y, z);
    home.setAll(x, y, z);

    velocity.setAll(0.0, 0.0, 0.0);
}

void TriangleThing::update()
{
    PhysicsEntity::update();
}

void TriangleThing::draw()
{
    glColor4f(1.0f, 0.25f + (position - home).magnitude() / 10, 0.25f, 0.25f);
    glVertex2d(position.x, position.y - 10);
    glVertex2d(position.x + 10, position.y + 10);
    glVertex2d(position.x - 10, position.y + 10);
    
//    glColor4f(0.1f, 0.01f, 0.9f, 0.25f);
//    glVertex2d(home.x, home.y - 5);
//    glVertex2d(home.x + 5, home.y + 5);
//    glVertex2d(home.x - 5, home.y + 5);

}

Art1::Art1(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;
    
    physicsEngine = new PhysicsEngine();
    
    for (int i = 0; i < 5000; i++)
    {
        float x = width * (rand() / (float)RAND_MAX);
        float y = height * (rand() / (float)RAND_MAX);
        
        TriangleThing *t = new TriangleThing(x, y, 0.0);
//        TriangleThing *t = new TriangleThing(0.5, 0.5, 0.0);
        physicsEngine->addEntity(t);
        triangleThings.push_back(t);
    }
    
    targetPoint.setAll(width / 2.0, height / 2.0, 0.0);
}

Art1::~Art1()
{
    delete physicsEngine;
    triangleThings.clear();
}

void Art1::update()
{    
    for (vector<TriangleThing *>::iterator i = triangleThings.begin(); i != triangleThings.end(); i++)
    {
        TriangleThing * triangle = *i;

        Vector3<float> mouseVector = triangle->position - targetPoint;
        float mouseDistance = mouseVector.magnitude();
        
        Vector3<float> mouseDirection = mouseVector * (1.0 / mouseDistance);
        float pushMagnitude = 12000.0 / pow(mouseDistance, 2);
        
        triangle->addForce(mouseDirection * pushMagnitude);
       
        Vector3<float> homeForce = (triangle->home - triangle->position) * 0.1;
        triangle->addForce(homeForce);
    }
    
    physicsEngine->update();
}

void Art1::draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBegin(GL_TRIANGLE_STRIP);
    for (vector<TriangleThing *>::iterator i = triangleThings.begin(); i != triangleThings.end(); i++)
    {
        TriangleThing * triangle = *i;
        triangle->draw();
    }
    glEnd();
    glDisable(GL_BLEND);
}

void Art1::setTargetPoint(float x, float y)
{
//    printf("New target point: %f, %f\n", x, y);
    targetPoint.setAll(x, y, 0.0);
}
