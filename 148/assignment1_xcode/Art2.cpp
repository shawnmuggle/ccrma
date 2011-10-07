//
//  Art2.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/6/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

#include "Art2.h"

using namespace std;

ConnectedVertex::ConnectedVertex(Vector3<float> aPosition, ConnectedVertex *connection)
{
    position = aPosition;
    velocity = (connection->position - position) * 0.1;
    connections.push_back(connection);
}

ConnectedVertex::ConnectedVertex(Vector3<float> aPosition)
{
    position = aPosition;
}

void ConnectedVertex::draw()
{
    for (vector<ConnectedVertex *>::iterator i = connections.begin(); i != connections.end(); i++) 
    {
        ConnectedVertex *otherVertex = *i;
        glColor4f(0.85, 0.85, 0.85 - (velocity).magnitude() * 0.5, 0.65);
        glVertex2f(position.x, position.y);
        glVertex2f(otherVertex->position.x, otherVertex->position.y);
    }
}

void ConnectedVertex::update()
{
//    printf("UPDATING with energy %f\n", energy);
    PhysicsEntity::update();
}

Art2::Art2(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;
    
    targetPoint.setAll(-1000, -1000, 0.0);
    prevTargetPoint.setAll(-1000, -1000, 0.0);
    
    ConnectedVertex *firstVertex = new ConnectedVertex(Vector3<float>(targetPoint.x, targetPoint.y, 0.0));
    vertices.push_back(firstVertex);
}

Art2::~Art2()
{
#warning I need to actually delete the vertices here, as they are not deleted by vector::clear
    vertices.clear();
}

void Art2::setTargetPoint(float x, float y)
{
    prevTargetPoint = targetPoint;
    targetPoint.setAll(x, y, 0.0);
}

void Art2::update()
{
    if ( prevTargetPoint.x >= 0 && prevTargetPoint.y >= 0)
    {
        vector<ConnectedVertex *> newPrevVertices;
        for (int i = 0; i < 5; i++) 
        {
            ConnectedVertex *vertex = new ConnectedVertex(Vector3<float>(targetPoint.x - 10 + 20 * (rand() / (float)RAND_MAX), 
                                                                         targetPoint.y - 10 + 20 * (rand() / (float)RAND_MAX)
                                                                         , 0.0), 
                                                          prevVertices[prevVertices.size() * (rand() / (float)RAND_MAX)]);
            vertices.push_back(vertex);
            newPrevVertices.push_back(vertex);
        }
        prevVertices = newPrevVertices;
    }
    else
    {
        vector<ConnectedVertex *> newPrevVertices;
        for (int i = 0; i < 5; i++) 
        {
            ConnectedVertex *vertex = new ConnectedVertex(Vector3<float>(targetPoint.x - 10 + 20 * (rand() / (float)RAND_MAX), 
                                                                         targetPoint.y - 10 + 20 * (rand() / (float)RAND_MAX)
                                                                         , 0.0));
            vertices.push_back(vertex);
            newPrevVertices.push_back(vertex);
        }
        prevVertices = newPrevVertices;        
    }
    
    for (vector<ConnectedVertex *>::iterator i = vertices.begin(); i != vertices.end(); i++)
    {
        ConnectedVertex *vertex = *i;
        vertex->update();
    }

}

void Art2::draw()
{
    glPointSize(10.0);
    glBegin(GL_LINES);
    for (vector<ConnectedVertex *>::iterator i = vertices.begin(); i != vertices.end(); i++)
    {
        ConnectedVertex *vertex = *i;        
        vertex->draw();
    }
    glEnd();
}