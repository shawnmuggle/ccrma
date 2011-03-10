/*
 *  at_mglMaterial.h
 *  AngryTruck
 *
 *  Created by njb on 2/28/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __MGLMATERIAL_H__
#define __MGLMATERIAL_H__

#include "at_mglTypes.h"
#include <stdio.h>

struct MGLMaterial
{
    MGLMaterial()
    {
        specular[0] = 0.0; specular[1] = 0.0; specular[2] = 0.0;
        diffuse[0] = 0.8; diffuse[1] = 0.8; diffuse[2] = 0.8;
        ambient[0] = .2;  ambient[1] = .2; ambient[2] = .2;
        shininess = 0.0;
    }
    MGLfloat specular[3];
    MGLfloat diffuse[3];
    MGLfloat ambient[3];
    MGLfloat shininess;
    
    void print()
    {
        printf("ambient r:%f g:%f b:%f\n", ambient[0], ambient[1], ambient[2]);
        printf("diffuse r:%f g:%f b:%f\n", diffuse[0], diffuse[1], diffuse[2]);
        printf("specular r:%f g:%f b:%f\n", specular[0], specular[1], specular[2]);
        printf("shininess:%f\n\n", shininess);
        
    }
};


#endif