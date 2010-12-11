/*
 *  Entity.cpp
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Entity.h"

#include <iostream>

void Entity::InitColor(Vector3D *color)
{
    red = new Vector3D(color->x, color->x, 0.1);
    green = new Vector3D(color->y, color->y, 0.1);
    blue = new Vector3D(color->z, color->z, 0.1);
    delete color;
}
void Entity::UpdateColor() 
{
    red->interp();
    green->interp();
    blue->interp();
}
void Entity::Flash()
{
    red->value = 1.0;
    green->value = 1.0;
    blue->value = 1.0;
}

void Cuboid::Render()
{
    /*
    printf("Rendering cuboid with position %f, %f, %f, color %f, %f, %f, and scale %f\n", 
           position->x, position->y, position->z,
           red->value, green->value, blue->value, 
           scale);
     */
    
    UpdateColor();
    
    glPushMatrix();
    
    glTranslatef(position->x, position->y, position->z);

    GLfloat mat_amb_diff[] = { red->value, green->value, blue->value, alpha };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
    
    glBegin(GL_QUADS);

    //top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f( scale, scale,-scale);			// Top Right Of The Quad (Top)
    glVertex3f(-scale, scale,-scale);			// Top Left Of The Quad (Top)
    glVertex3f(-scale, scale, scale);			// Bottom Left Of The Quad (Top)
    glVertex3f( scale, scale, scale);			// Bottom Right Of The Quad (Top)
    
    
    //bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f( scale,-scale, scale);			// Top Right Of The Quad (Bottom)
    glVertex3f(-scale,-scale, scale);			// Top Left Of The Quad (Bottom)
    glVertex3f(-scale,-scale,-scale);			// Bottom Left Of The Quad (Bottom)
    glVertex3f( scale,-scale,-scale);			// Bottom Right Of The Quad (Bottom)
    
    //front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f( scale, scale, scale);			// Top Right Of The Quad (Front)
    glVertex3f(-scale, scale, scale);			// Top Left Of The Quad (Front)
    glVertex3f(-scale,-scale, scale);			// Bottom Left Of The Quad (Front)
    glVertex3f( scale,-scale, scale);			// Bottom Right Of The Quad (Front)
    
    //back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( scale,-scale,-scale);			// Bottom Left Of The Quad (Back)
    glVertex3f(-scale,-scale,-scale);			// Bottom Right Of The Quad (Back)
    glVertex3f(-scale, scale,-scale);			// Top Right Of The Quad (Back)
    glVertex3f( scale, scale,-scale);			// Top Left Of The Quad (Back)
    
    //left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-scale, scale, scale);			// Top Right Of The Quad (Left)
    glVertex3f(-scale, scale,-scale);			// Top Left Of The Quad (Left)
    glVertex3f(-scale,-scale,-scale);			// Bottom Left Of The Quad (Left)
    glVertex3f(-scale,-scale, scale);			// Bottom Right Of The Quad (Left)
    
    //right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f( scale, scale,-scale);			// Top Right Of The Quad (Right)
    glVertex3f( scale, scale, scale);			// Top Left Of The Quad (Right)
    glVertex3f( scale,-scale, scale);			// Bottom Left Of The Quad (Right)
    glVertex3f( scale,-scale,-scale);			// Bottom Right Of The Quad (Right)

	glEnd();						// Done Drawing The Quad
    
    glPopMatrix();
}