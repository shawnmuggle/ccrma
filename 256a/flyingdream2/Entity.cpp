/*
 *  Entity.cpp
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Entity.h"


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
    UpdateColor();
    
    glPushMatrix();
    
    glTranslatef(position->x, position->y, position->z);
    glScalef(scale, scale, scale);
    //glColor4f(red->value, green->value, blue->value, 1.0);

    GLfloat mat_amb_diff[] = { red->value, green->value, blue->value, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
    
    glBegin(GL_QUADS);

    //top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);			// Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 1.0f);			// Bottom Right Of The Quad (Top)
    

    //bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);			// Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);			// Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Bottom)
    
    //front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Front)
    
    //back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Back)
    
    //left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Left)

    //right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Right)

	glEnd();						// Done Drawing The Quad
    
    glPopMatrix();
}

void Skybox::Render()
{
    UpdateColor();
    
    glPushMatrix();
    
    glTranslatef(position->x, position->y, position->z);
    glScalef(scale, scale, scale);

    //glColor3f(red->value, green->value, blue->value);
    
    GLfloat mat_amb_diff[] = { red->value, green->value, blue->value, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
    
    
    glBegin(GL_QUADS);
    
    //top
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);			// Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 1.0f);			// Bottom Right Of The Quad (Top)
    
    
    //bottom
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);			// Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);			// Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Bottom)
    
    //front
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Front)
    
    //back
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Back)
    
    //left
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Left)
    
    //right
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Right)
    
	glEnd();						// Done Drawing The Quad
    
    glPopMatrix();
}