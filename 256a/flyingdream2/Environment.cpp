/*
 *  Environment.cpp
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 11/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Environment.h"


Environment::Environment(void)
{
    for (int i = 0; i < 50; i++) {
        clouds.push_back(new Cuboid(new Vector3D(-2000 + 4000 * (rand() / (double)RAND_MAX),
                                                 1000 + 1000 * (rand() / (double)RAND_MAX),
                                                 -2000 + 4000 * (rand() / (double)RAND_MAX)),
                                    new Vector3D(1.0, 1.0, 1.0),
                                    0.7,
                                    20 + 80 * (rand() / (double)RAND_MAX)));
    }
}

void Environment::Render(void)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    
    glPushMatrix();
    
    glColor3f(127/255.0, 178/255.0, 230/255.0);
    
    glBegin(GL_QUADS);
    
    //top of the sky
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f( 2000.0f, 2000.0f, -2000.0f);			// Top Right Of The Quad (Top)
    glVertex3f(-2000.0f, 2000.0f, -2000.0f);			// Top Left Of The Quad (Top)
    glVertex3f(-2000.0f, 2000.0f, 2000.0f);			// Bottom Left Of The Quad (Top)
    glVertex3f( 2000.0f, 2000.0f, 2000.0f);			// Bottom Right Of The Quad (Top)
    
    //front of the sky
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( 2000.0f, 2000.0f, 2000.0f);			// Top Right Of The Quad (Front)
    glVertex3f(-2000.0f, 2000.0f, 2000.0f);			// Top Left Of The Quad (Front)
    glVertex3f(-2000.0f,-2000.0f, 2000.0f);			// Bottom Left Of The Quad (Front)
    glVertex3f( 2000.0f,-2000.0f, 2000.0f);			// Bottom Right Of The Quad (Front)
    
    //back
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 2000.0f,-2000.0f,-2000.0f);			// Bottom Left Of The Quad (Back)
    glVertex3f(-2000.0f,-2000.0f,-2000.0f);			// Bottom Right Of The Quad (Back)
    glVertex3f(-2000.0f, 2000.0f,-2000.0f);			// Top Right Of The Quad (Back)
    glVertex3f( 2000.0f, 2000.0f,-2000.0f);			// Top Left Of The Quad (Back)
    
    //left
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-2000.0f, 2000.0f, 2000.0f);			// Top Right Of The Quad (Left)
    glVertex3f(-2000.0f, 2000.0f,-2000.0f);			// Top Left Of The Quad (Left)
    glVertex3f(-2000.0f,-2000.0f,-2000.0f);			// Bottom Left Of The Quad (Left)
    glVertex3f(-2000.0f,-2000.0f, 2000.0f);			// Bottom Right Of The Quad (Left)
    
    //right
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f( 2000.0f, 2000.0f,-2000.0f);			// Top Right Of The Quad (Right)
    glVertex3f( 2000.0f, 2000.0f, 2000.0f);			// Top Left Of The Quad (Right)
    glVertex3f( 2000.0f,-2000.0f, 2000.0f);			// Bottom Left Of The Quad (Right)
    glVertex3f( 2000.0f,-2000.0f,-2000.0f);			// Bottom Right Of The Quad (Right)

    
    glColor4f(53/255.0, 71/255.0, 140/255.0, 0.8);
    
    //top of the ocean
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 2000.0f, -2000.0f, 2000.0f);			// Top Right Of The Quad (Bottom)
    glVertex3f(-2000.0f, -2000.0f, 2000.0f);			// Top Left Of The Quad (Bottom)
    glVertex3f(-2000.0f, -2000.0f, -2000.0f);			// Bottom Left Of The Quad (Bottom)
    glVertex3f( 2000.0f, -2000.0f, -2000.0f);			// Bottom Right Of The Quad (Bottom)
    
	glEnd();						// Done Drawing The Quad
    
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);

}

void Environment::RenderClouds()
{
    std::vector<Cuboid *>::iterator itr=clouds.begin();
    while(itr != clouds.end()) {
        printf("FOOP\n");
        Cuboid *cuboid = *itr;
        cuboid->Render();
        ++itr;
    }
}

void Environment::BoundPosition(Vector3D *position)
{

}