/*
 *  InstrumentEntity.cpp
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "InstrumentEntity.h"

InstrumentEntity::InstrumentEntity(Vector3D *position, Vector3D *color, float alpha, float scale, int instrument_id) : 
Entity(position, color, alpha, scale),
instrument_id(instrument_id)
{
    quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
    gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords ( NEW )
}


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