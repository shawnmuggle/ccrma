/*
 *  at_mglLight.h
 *  AngryTruck
 *
 *  Created by njb on 2/28/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __MGLLIGHT_H__
#define __MGLLIGHT_H__

#include "at_mglTypes.h"
#include <stdio.h>

// Structure to model a light
struct MGLLight 
{
    MGLLight(bool isFirstLight)
    {
        position[0] = 0.0; position[1] = 0; position[2] = 1; position[3] = 1;
        ambient[0] = 0.0; ambient[1] = 0.0; ambient[2] = 0.0;
        
        diffuse[0] = 0.0; diffuse[1] = 0.0; diffuse[2] = 0.0;
        specular[0] = 0.0; specular[1] = 0.0; specular[2] = 0.0;
        
        if(isFirstLight)
        {
            specular[0] = 1.0; specular[1] = 1.0; specular[2] = 1.0;
            diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 1.0;
        }
        
        isOn = false;
    }
    
    void print()
    {
		printf("position x:%f y:%f z:%f w:%f\n", position[0], position[1], position[2], position[3]);
		printf("ambient r:%f g:%f b:%f\n", ambient[0], ambient[1], ambient[2]);
		printf("diffuse r:%f g:%f b:%f\n", diffuse[0], diffuse[1], diffuse[2]);
		printf("specular r:%f g:%f b:%f\n", specular[0], specular[1], specular[2]);
		if(isOn)
			printf("isOn\n\n");
		else {
			printf("isOff\n\n");
		}
		
    }
    
    MGLfloat position[4];
    MGLfloat ambient[3];
    MGLfloat diffuse[3];
    MGLfloat specular[3];
    bool isOn;
};





#endif