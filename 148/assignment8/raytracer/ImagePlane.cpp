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

ImagePlane::ImagePlane() : outputImage(NULL), threadPool(new JankyRayThreadPool()) 
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
    
    for (int x = 0; x < outputImage->GetWidth(); x++)
    {
        for (int y = 0; y < outputImage->GetHeight(); y++)
        {
            RayThreadArgs *rta = new RayThreadArgs();
            rta->imagePlane = this;
            rta->camera = &camera;
            rta->scene = scene;
            rta->x = x;
            rta->y = y;
            threadPool->callFunctionWithArgs(generateRayFromCameraAtPoint, rta);
        }
    }
}

static void *generateRayFromCameraAtPoint(void *args)
{
    RayThreadArgs *rta = (RayThreadArgs *)args;
    
//    float s = (rta->x + 0.5) / rta->imagePlane->outputImage->GetWidth();
//    float t = (rta->y + 0.5) / rta->imagePlane->outputImage->GetHeight();
//    STPoint3 p = rta->imagePlane->bilinearInterpolate(s, t);
//    STVector3 d = p - rta->camera->position;
//    
//    Ray r(rta->camera->position, d, d.Length(), MAXFLOAT);
//    STColor3f color;
//    rta->scene->traceRay(r, &color);
//    rta->imagePlane->outputImage->SetPixel(rta->x, rta->y, STImage::Pixel(color));
//    
//    // This pointer was created just for this thread, so we can delete it here.
//    // OR MAYBE NOT?!?
//    delete rta;
    pthread_exit(NULL);
}

JankyRayThreadPool::JankyRayThreadPool()
{
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    currentThread = 0;
}

void JankyRayThreadPool::callFunctionWithArgs(void *(*function)(void*), void *args)
{
    void *status;
    pthread_join(threads[currentThread], &status);
    int rc = pthread_create(&threads[currentThread], &attr, function, args); 
    if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    currentThread = (currentThread + 1) % MAX_NUM_THREADS;
    printf("Current thread: %d\n", currentThread);
}