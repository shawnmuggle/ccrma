//
//  ImagePlane.cpp
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>
#include "ImagePlane.h"
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"

void ImagePlane::createBoundaryPoints(Camera camera, float fovy, float aspect)
{
    
    STPoint3 C = camera.position + camera.w;
    float x = tanf(fovy / 2.0f);
    float y = tanf(aspect * fovy / 2.0f);
    
    LL = C + (x * camera.u) - (y * camera.v);
    UL = C + (x * camera.u) + (y * camera.v);
    LR = C - (x * camera.u) - (y * camera.v);
    UR = C - (x * camera.u) + (y * camera.v);
    
    printf("LL is %f, %f, %f\n", LL.x, LL.y, LL.z);
    printf("UL is %f, %f, %f\n", UL.x, UL.y, UL.z);
    printf("LR is %f, %f, %f\n", LR.x, LR.y, LR.z);
    printf("UR is %f, %f, %f\n", UR.x, UR.y, UR.z);
}

void ImagePlane::createOutputImage(const int &width, const int &height, const std::string &filename)
{
    outputImage = new STImage(width, height);
    outputFilename = filename;
}

STPoint3 ImagePlane::bilinearInterpolate(float s, float t)
{
    STVector3 LL_UL = UL - LL;
    STVector3 LR_UR = UR - LR;
    STPoint3 a = LL + t * LL_UL;
    STPoint3 b = LR + t * LR_UR;
    return a + s * (b - a);
}

void ImagePlane::generateRaysFromCamera(Camera camera, Scene const* scene)
{
    if (!outputImage)
        return;
    
    float s, t;
    for (int x = 0; x < outputImage->GetWidth(); x++)
    {
        s = (x + 0.5) / outputImage->GetWidth();
        for (int y = 0; y < outputImage->GetHeight(); y++)
        {
            t = (y + 0.5) / outputImage->GetHeight();
            STPoint3 p = bilinearInterpolate(s, t);
            STVector3 d = p - camera.position;

#warning Is there a more sensible maximum t I should use here?
            Ray r(camera.position, d, d.Length(), 10000);
            
            if (x == 128 && y == 256)
                printf("HI\n");
            
            outputImage->SetPixel(x, y, STImage::Pixel(scene->traceRay(r)));
        }
    }
}

void ImagePlane::saveOutputImage()
{
    printf("Saving %s\n", outputFilename.c_str());
    outputImage->Save(outputFilename);
}