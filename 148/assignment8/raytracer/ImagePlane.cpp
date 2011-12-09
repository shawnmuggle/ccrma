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

void ImagePlane::createBoundaryPoints(Camera const& camera, float const& fovy, float const& aspect)
{
    STPoint3 C = camera.position + camera.w;
    float x = tanf(fovy / 2.0f);
    float y = tanf(aspect * fovy / 2.0f);
    
    LL = C + (x * camera.u) - (y * camera.v);
    UL = C + (x * camera.u) + (y * camera.v);
    LR = C - (x * camera.u) - (y * camera.v);
    UR = C - (x * camera.u) + (y * camera.v);
}

void ImagePlane::createOutputImage(int const& width, int const& height, std::string const& filename)
{
    outputImage = new STImage(width, height);
    outputFilename = filename;
}

STPoint3 ImagePlane::bilinearInterpolate(float const& s, float const& t)
{
    STVector3 LL_UL = UL - LL;
    STVector3 LR_UR = UR - LR;
    STPoint3 a = LL + t * LL_UL;
    STPoint3 b = LR + t * LR_UR;
    return a + s * (b - a);
}

void ImagePlane::generateRaysFromCamera(Camera const& camera, Scene const* const scene)
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
            
            if (x == 256 && y == 10)
                printf("HI\n");

            Ray r(camera.position, d, d.Length(), MAXFLOAT);
            STColor3f color;
            scene->traceRay(r, &color);
            outputImage->SetPixel(x, y, STImage::Pixel(color));
        }
    }
}

void ImagePlane::saveOutputImage()
{
    printf("Saving %s\n", outputFilename.c_str());
    outputImage->Save(outputFilename);
}