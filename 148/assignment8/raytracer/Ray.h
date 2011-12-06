//
//  Ray.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Ray_h
#define raytracer_Ray_h

#include "stForward.h"

class Ray {
        
    float minValidT, maxValidT;
    
public:
    STPoint3 e;
    STVector3 d;
    
    Ray(STPoint3 e, STVector3 d, float minValidT, float maxValidT) :
    e(e), d(d), minValidT(minValidT), maxValidT(maxValidT) {}
    
    inline STPoint3 pointOnRayAtT(float t) { return e + t * d; }
    inline bool isValidT(float t) { return t >= minValidT && t <= maxValidT; }
};

#endif
