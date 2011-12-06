//
//  Camera.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Camera_h
#define raytracer_Camera_h

#include "STPoint3.h"
#include "STVector3.h"

class Camera {
public:
    STPoint3 position;
    STVector3 u, v, w;
    void createOrthonormalBasis(const STPoint3& eye, const STVector3& up, const STPoint3& lookAt);
};

#endif
