//
//  Art1.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/2/11.
//  Copyright 2011 Stanford. All rights reserved.
//

#include <iostream>

#include "Art1.h"

using namespace std;

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
    glVertex2f(position.x, position.y - 10);
    glVertex2f(position.x + 10, position.y + 10);
    glVertex2f(position.x - 10, position.y + 10);
}

//void TriangleThing::drawVBO(GLvoid *data)
//{
//    GLubyte r = 255 * 1.0f;
//    GLubyte g = 255 * (0.25f + (position - home).magnitude() / 10);
//    GLubyte b = 255 * 0.25f;
//    GLubyte a = 255 * 0.25f;
//
//    ((GLubyte *)data)[0] = r;
//    ((GLubyte *)data)[1] = g;
//    ((GLubyte *)data)[2] = b;
//    ((GLubyte *)data)[3] = a;
//    ((float *)data)[1] = position.x;
//    ((float *)data)[2] = position.y - 10.0;
//
//    ((GLubyte *)data)[12 + 0] = r;
//    ((GLubyte *)data)[12 + 1] = g;
//    ((GLubyte *)data)[12 + 2] = b;
//    ((GLubyte *)data)[12 + 3] = a;
//    ((float *)data)[4] = position.x + 10.0;
//    ((float *)data)[5] = position.y + 10.0;
//    
//    ((GLubyte *)data)[24 + 0] = r;
//    ((GLubyte *)data)[24 + 1] = g;
//    ((GLubyte *)data)[24 + 2] = b;
//    ((GLubyte *)data)[24 + 3] = a;
//    ((float *)data)[7] = position.x - 10.0;
//    ((float *)data)[8] = position.y + 10.0;
//}

Art1::Art1(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;
    
    numTriangles = 100000;
//    glGenBuffers(1, &trianglesVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, trianglesVBO);
//
//    // each triangle has 3 vertices, each of which have 2 floats & 4 unsigned bytes associated with them (GL_C4UB_V2F)
//    glBufferData(GL_ARRAY_BUFFER, numTriangles * 3 * 2 * sizeof(float) + numTriangles * 3 * 4 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
//    
//    glInterleavedArrays(GL_V2F, 0, NULL);
//    glEnableClientState(GL_VERTEX_ARRAY);
    
    physicsEngine = new PhysicsEngine();
    
    for (int i = 0; i < numTriangles; i++)
    {
        float x = width * (rand() / (float)RAND_MAX);
        float y = height * (rand() / (float)RAND_MAX);
        
        TriangleThing *t = new TriangleThing(x, y, 0.0);
        physicsEngine->addEntity(t);
        triangleThings.push_back(t);
    }
    
    targetPoint.setAll(width / 2.0, height / 2.0, 0.0);
}

Art1::~Art1()
{
    delete physicsEngine;
    triangleThings.clear();
//    glDeleteBuffers(1, &trianglesVBO);
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

//    GLvoid *data;
//    data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
//    int i = 0;
//    for (vector<TriangleThing *>::iterator itr = triangleThings.begin(); itr != triangleThings.end(); itr++)
//    {
//        TriangleThing * triangle = *itr;
//        triangle->drawVBO(&((GLubyte *) data)[i * 3 * (2 * sizeof(float) + 4 * sizeof(GLubyte))]);
//        i++;
//    }
//    glUnmapBuffer(GL_ARRAY_BUFFER);
//    
//    glDrawArrays(GL_TRIANGLES, 0, numTriangles);
    
    glBegin(GL_TRIANGLES);
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
