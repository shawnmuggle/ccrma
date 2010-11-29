/*
 *  Entity.h
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "vector3d.h"
#include <stdlib.h>

class Entity
{
protected:
    Entity(Vector3D *position, Vector3D *color, float scale) : 
    position(position),
    scale(scale)
    {
        id = rand();
        InitColor(color);
    }
    
    ~Entity() 
    {
        delete position; 
        delete red; delete green; delete blue; 
    }
    void InitColor(Vector3D *color);
    void UpdateColor();
    virtual void Render() = 0;
    
public:
    void Flash();
    
    Vector3D *red;
    Vector3D *green;
    Vector3D *blue;
    Vector3D *position;
    float scale;
    int id;    
};

class Cuboid : public Entity
{
public:
    Cuboid(Vector3D *position, Vector3D *color, float scale) : Entity(position, color, scale)
    { }
    virtual void Render();
};

// Just like cuboid, but with inward-facing normals
class Skybox : public Entity
{
public:
    Skybox(Vector3D *position, Vector3D *color, float scale) : Entity(position, color, scale)
    { }
    virtual void Render();
};