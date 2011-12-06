//
//  Light.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Light_h
#define raytracer_Light_h

#include "STColor3f.h"
#include "STPoint3.h"
#include "STVector3.h"

class AmbientLight {
public:
    STColor3f color;
    AmbientLight(STColor3f const& color) : color(color) {}
};

class PointLight {
public:
    STPoint3 position;
    STColor3f color;
    PointLight(STPoint3 const& position, STColor3f const& color) : position(position), color(color) {}
};

class DirectionalLight {
public:
    STVector3 direction;
    STColor3f color;    
    DirectionalLight(STVector3 const& direction, STColor3f const& color) : direction(direction), color(color) {}    
};


#endif
