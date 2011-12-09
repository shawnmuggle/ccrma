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
#include "STTransform4.h"

class Light {
public:
    STTransform4 transform;
    STColor3f color;
    Light(STTransform4 transform, STColor3f const& color) : 
    transform(transform), color(color) {}
};

class AmbientLight : public Light {
public:
    AmbientLight(STTransform4 transform, STColor3f const& color) : 
    Light(transform, color) {}
};

class PointLight : public Light {
public:
    STPoint3 position;
    PointLight(STTransform4 transform, STColor3f const& color, STPoint3 const& position) : 
    Light(transform, color), position(position) {}
};

class DirectionalLight : public Light {
public:
    STVector3 direction;
    DirectionalLight(STTransform4 transform, STColor3f const& color, STVector3 const& direction) : 
    Light(transform, color), direction(direction) {}    
};


#endif
