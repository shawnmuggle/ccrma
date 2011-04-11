//
//  MikeMesh.cpp
//  CymbalSounds
//
//  Created by Michael Rotondo on 3/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "MikeMesh.h"

MikeMesh::MikeMesh()
{
    rightGoingWaveLeft.setDelay(100);
    rightGoingWaveRight.setDelay(100);
    leftGoingWaveLeft.setDelay(100);
    leftGoingWaveRight.setDelay(100);
    
    leftEdge.setGain(0.99);
    leftEdge.setPole(0.5);
    rightEdge.setGain(0.99);
    rightEdge.setPole(0.5);
    
    input = 0.0;
    
    falloff = 0.9;
}

void MikeMesh::noteOn()
{
    input = 1.0;
}

stk::StkFloat MikeMesh::tick()
{
    stk::StkFloat leftOutput = rightGoingWaveLeft.tick(-leftEdge.tick(leftGoingWaveLeft.lastOut()));
    stk::StkFloat rightOutput = leftGoingWaveRight.tick(-rightEdge.tick(rightGoingWaveRight.lastOut()));
    
    stk::StkFloat output = leftOutput + rightOutput;
    
    leftGoingWaveLeft.tick(rightOutput + input / 2.0);
    rightGoingWaveRight.tick(leftOutput + input / 2.0);
    
    input *= falloff;
    
    return output;
}

void MikeMesh::setSize(float sizeSamps)
{
    float halfLength = sizeSamps / 2.0;
    
    rightGoingWaveLeft.setDelay(halfLength * 1.3);
    rightGoingWaveRight.setDelay(halfLength * 0.7);
    leftGoingWaveLeft.setDelay(halfLength * 1.3);
    leftGoingWaveRight.setDelay(halfLength * 0.7);
}

void MikeMesh::setAttackFalloff(float newFalloff)
{
    falloff = newFalloff;
}

void MikeMesh::setLeftPole(float pole)
{
    leftEdge.setPole(pole);
}
void MikeMesh::setLeftDamping(float damping)
{
    leftEdge.setGain(1 - damping);
}
void MikeMesh::setLeftA1(float coefficient)
{
    leftEdge.setA1(coefficient);
}
void MikeMesh::setLeftA2(float coefficient)
{
    leftEdge.setA2(coefficient);
}

void MikeMesh::setRightPole(float pole)
{
    rightEdge.setPole(pole);
}
void MikeMesh::setRightDamping(float damping)
{
    rightEdge.setGain(1 - damping);
}
void MikeMesh::setRightA1(float coefficient)
{
    rightEdge.setA1(coefficient);
}
void MikeMesh::setRightA2(float coefficient)
{
    rightEdge.setA2(coefficient);
}