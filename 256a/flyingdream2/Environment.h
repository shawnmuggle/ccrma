/*
 *  Environment.h
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 11/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Entity.h"
#include "vector3d.h"

#include <vector>

class Environment
{
public:
    Environment(void);
    void Render();
    void BoundPosition(Vector3D *position);
private:
    std::vector<Cuboid *> clouds;
};