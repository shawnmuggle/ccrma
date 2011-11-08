/*
 *  InstrumentEntity.h
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#import <OpenGL/glu.h>

#include "Entity.h"

class InstrumentEntity : public Entity
{
public:
    InstrumentEntity(Vector3D *position, Vector3D *color, float alpha, float scale, int instrument_id);
    void Render();
    
    int instrument_id;
private:
    GLUquadricObj *quadratic;
};