//
//  ImagePlane.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_ImagePlane_h
#define raytracer_ImagePlane_h

#include "STPoint3.h"
#include "STImage.h"
#include <vector>
#include <pthread.h>

class Camera;
class Scene;
class JankyRayThreadPool;

class ImagePlane {
private:
    JankyRayThreadPool *threadPool;
    
public:
    // Public just because I need to access them from a static method for threading :\ There must be a better way...
    STPoint3 LL, UL, LR, UR;
    std::string outputFilename;
    STImage *outputImage;
    STPoint3 bilinearInterpolate(float const& s, float const& t);
    
    // Actually should be public:
    ImagePlane();
    ~ImagePlane();
    void createBoundaryPoints(Camera const& camera, float const& fovy, float const& aspect);
    void createOutputImage(int const& width, int const& height, std::string const& filename);
    void generateRaysFromCamera(Camera const& camera, Scene const* const scene);
    void saveOutputImage();
};


// TIME FOR JANKY THREAD POOLS GO

struct RayThreadArgs {
    ImagePlane *imagePlane;
    Camera const* camera;
    Scene const* scene;
    int x;
    int y;
};

static void *generateRayFromCameraAtPoint(void *args);

#define MAX_NUM_THREADS 10
class JankyRayThreadPool {
    pthread_attr_t attr;
    int currentThread;
public:
    pthread_t threads[MAX_NUM_THREADS];
    JankyRayThreadPool();
    void callFunctionWithArgs(void *(*function)(void*), void *args);
};

#endif
