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

ImagePlane::ImagePlane() : outputImage(NULL), threadPool(NULL)
{
}

ImagePlane::~ImagePlane()
{
    delete outputImage;
    delete threadPool;
}

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

void ImagePlane::saveOutputImage()
{
    printf("Saving %s\n", outputFilename.c_str());
    outputImage->Save(outputFilename);
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
    
    numXYPairs = outputImage->GetWidth() * outputImage->GetHeight();
    xyPairs = new XYPair[numXYPairs];
    int i = 0;
    for (int x = 0; x < outputImage->GetWidth(); x++)
    {
        for (int y = 0; y < outputImage->GetHeight(); y++)
        {
            xyPairs[i].x = x;
            xyPairs[i].y = y;
            pthread_mutex_init(&xyPairs[i].mutex, NULL);
            i++;
        }
    }
    
    RayThreadArgs *rta = new RayThreadArgs();
    rta->imagePlane = this;
    rta->camera = &camera;
    rta->scene = scene;
    threadPool = new JankyRayThreadPool(rta);

    delete [] xyPairs;
}

static void *generateRaysFromCamera(void *args)
{
    RayThreadArgs *rta = (RayThreadArgs *)args;
    
    int i = 0;
    while (i < numXYPairs)
    {
        if (!pthread_mutex_trylock(&xyPairs[i].mutex))
        {
            int x = xyPairs[i].x;
            int y = xyPairs[i].y;
            float s = (x + 0.5) / rta->imagePlane->outputImage->GetWidth();
            float t = (y + 0.5) / rta->imagePlane->outputImage->GetHeight();
            STPoint3 p = rta->imagePlane->bilinearInterpolate(s, t);
            STVector3 d = p - rta->camera->position;
            
            Ray r(rta->camera->position, d, d.Length(), MAXFLOAT);
            STColor3f color;
            rta->scene->traceRay(r, &color);
            rta->imagePlane->outputImage->SetPixel(x, y, STImage::Pixel(color));
        }
        i++;
    }
    
    pthread_exit(NULL);
}

JankyRayThreadPool::JankyRayThreadPool(RayThreadArgs *args)
{
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int rc;
    void *status;
    
    for (int i = 0; i < MAX_NUM_THREADS; i++)
    {
        rc = pthread_create(&threads[i], &attr, generateRaysFromCamera, args); 
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }        
    }
    
    for (int i = 0; i < MAX_NUM_THREADS; i++)
    {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }
    
    delete args;
}