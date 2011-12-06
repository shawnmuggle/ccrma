//
//  SceneObject.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_SceneObject_h
#define raytracer_SceneObject_h

#include "STTransform4.h"
#include "Shape.h"
#include "Material.h"

class SceneObject {
    
public:

    Shape *shape;
    Material material;
    STTransform4 transform;
    
    SceneObject(Shape * const inShape, Material const& inMaterial, STTransform4 const& inTransform) : 
    shape(inShape),
    material(inMaterial),
    transform(inTransform)
    { }
};

#endif
