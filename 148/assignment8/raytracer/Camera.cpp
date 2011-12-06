//
//  Camera.cpp
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>
#include "Camera.h"

void Camera::createOrthonormalBasis(const STPoint3& eye, const STVector3& up, const STPoint3& lookAt)
{
    position = eye;
    
    // Create orthonormal basis
    STVector3 a = lookAt - eye;
    STVector3 b = up;
    
    a.Normalize();
    w = a;
    
    u = STVector3::Cross(b, w);
    u.Normalize();
    
    v = STVector3::Cross(w, u);
}
