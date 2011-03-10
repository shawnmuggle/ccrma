///*
// *  at_mglTexture.h
// *  AngryTruck
// *
// *  Created by njb on 2/28/11.
// *  Copyright 2011 __MyCompanyName__. All rights reserved.
// *
// */
//
//#ifndef __MGLTEXTURE_H__
//#define __MGLTEXTURE_H__
//
//#include "at_mglTypes.h"
//#include <stdio.h>
//#include <string>
//
//
//struct MGLtexture
//{
//    MGLtexture(MGLsize width_, MGLsize height_, MGLpixel * data)
//    {   
//        
//        textureIDGenerator++;
//        id = textureIDGenerator;
//        width = width_;
//        height = height_;
//        imageData = new MGLpixel[width*height];
//        //printf("[MGLtexture]: Loading texture, width:%d height%d id:%d\n", width, height, id);
//        memcpy(imageData, data, sizeof(MGLpixel)*width*height);
//        
//        printf("[MGLtexture]: Loading texture, width:%d height%d id:%d\n", width, height, id);
//    }
//    
//    ~MGLtexture()
//    {
//        printf("[MGLtexture]: Deleting texture, width:%d height%d id:%d\n", width, height, id);
//        if(imageData!=NULL)
//        {  
//            delete imageData;
//            imageData = NULL;
//        }
//    }
//    
//    MGLint id;
//    MGLsize width;
//    MGLsize height;
//    MGLpixel *imageData;
//    static int textureIDGenerator;
//    
//};
//
//
//#endif