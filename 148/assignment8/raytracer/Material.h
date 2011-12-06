//
//  Material.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Material_h
#define raytracer_Material_h

#include "STColor3f.h"

class Material {
public:
    STColor3f ambient, diffuse, specular, mirr;
    float shine;
    Material() {}
    Material(STColor3f const& ambient, STColor3f const& diffuse, STColor3f const& specular, STColor3f const& mirr, float const& shine) 
    : ambient(ambient), diffuse(diffuse), specular(specular), mirr(mirr), shine(shine) {}
};

#endif
