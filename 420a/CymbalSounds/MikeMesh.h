//
//  MikeMesh.h
//  CymbalSounds
//
//  Created by Michael Rotondo on 3/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MIKEMESH_H
#define MIKEMESH_H

#include "DelayL.h"
#include "OnePole.h"

class MikeMesh
{
public:
    MikeMesh();
    ~MikeMesh();
  
    void noteOn();
    stk::StkFloat tick();
    
    void setSize(float newSizeSamps);
    void setPole(float newPole);
    void setDamping(float newDamping);
    void setAttackFalloff(float newFalloff);
protected:
    stk::DelayL rightGoingWaveRight;
    stk::DelayL rightGoingWaveLeft;
    stk::DelayL leftGoingWaveRight;
    stk::DelayL leftGoingWaveLeft;

    stk::StkFloat falloff;
    
    stk::OnePole leftEdge;
    stk::OnePole rightEdge;
    
    stk::StkFloat input;
};

#endif