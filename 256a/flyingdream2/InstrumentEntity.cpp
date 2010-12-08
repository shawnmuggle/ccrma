/*
 *  InstrumentEntity.cpp
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "InstrumentEntity.h"

void InstrumentEntity::Render()
{
    UpdateColor();
    glPushMatrix();
    GLfloat mat_amb_diff[] = { red->value, green->value, blue->value, alpha };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
    glTranslatef(position->x, position->y, position->z);
    gluSphere(quadratic, scale, 16, 16);
    glPopMatrix();
}