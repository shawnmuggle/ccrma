//
//  MikeMesh.h
//  CymbalSounds
//
//  Created by Michael Rotondo on 3/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MIKEMESH_H
#define MIKEMESH_H

#include "DelayA.h"
#include "NonLinearAllPass.h"

class MikeMesh
{
public:
    MikeMesh();
    ~MikeMesh();
  
    void noteOn();
    stk::StkFloat tick();
    
    void setSize(float sizeSamps);
    void setAttackFalloff(float falloff);
    
    void setLeftDamping(float damping);
    void setLeftPole(float pole);
    void setLeftA1(float coefficient);
    void setLeftA2(float coefficient);

    void setRightDamping(float damping);
    void setRightPole(float pole);
    void setRightA1(float coefficient);
    void setRightA2(float coefficient);
    

protected:
    stk::DelayA rightGoingWaveRight;
    stk::DelayA rightGoingWaveLeft;
    stk::DelayA leftGoingWaveRight;
    stk::DelayA leftGoingWaveLeft;

    stk::StkFloat falloff;
    
    stk::NonLinearAllPass leftEdge;
    stk::NonLinearAllPass rightEdge;
    
    stk::StkFloat input;
};

#endif