/*
 *  InstrumentEntity.h
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Entity.h"

class InstrumentEntity : public Entity
{
public:
    InstrumentEntity(Vector3D *position, Vector3D *color, float alpha, float scale, int instrument_id) : 
    Entity(position, color, alpha, scale),
    instrument_id(instrument_id)
    {
        quadratic = gluNewQuadric();
        gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
        gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords ( NEW )
    }
    void Render();
    
    int instrument_id;
private:
    GLUquadricObj *quadratic;
};