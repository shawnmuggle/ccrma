//
//  MRObjects.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/14/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef Assignment1_MRObjects_h
#define Assignment1_MRObjects_h

#include <vector>

#include "MRPhysics.h"
#include "MRGraphics.h"
#include "MRMath.h"
#include "MRSplines.h"

class MRBlob : public PhysicsEntity, public DrawableEntity
{
    float size;
    float angle;
    float angleIncrement;
    int numSlices;
    std::vector<float> phases;
    Spline spline;
    
public:
    MRBlob(float aX, float aY);
    virtual void draw();
    virtual void update();
};


#endif
