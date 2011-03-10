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

void MikeMesh::setSize(float newSizeSamps)
{
    float halfLength = newSizeSamps / 2.0;
    
    rightGoingWaveLeft.setDelay(halfLength);
    rightGoingWaveRight.setDelay(halfLength);
    leftGoingWaveLeft.setDelay(halfLength);
    leftGoingWaveRight.setDelay(halfLength);
}

void MikeMesh::setAttackFalloff(float newFalloff)
{
    falloff = newFalloff;
}

void MikeMesh::setPole(float newPole)
{
    leftEdge.setPole(newPole);
    rightEdge.setPole(newPole);
}

void MikeMesh::setDamping(float newDamping)
{
    leftEdge.setGain(1 - newDamping);
    rightEdge.setGain(1 - newDamping);
}