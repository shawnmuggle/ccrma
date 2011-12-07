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

class Camera;
class Scene;

class ImagePlane {
    STPoint3 LL, UL, LR, UR;
    
    std::string outputFilename;
    STImage *outputImage;
    
    STPoint3 bilinearInterpolate(float const& s, float const& t);
    
public:
    ImagePlane() : outputImage(NULL) {}
    void createBoundaryPoints(Camera const& camera, float const& fovy, float const& aspect);
    void createOutputImage(int const& width, int const& height, std::string const& filename);
    void generateRaysFromCamera(Camera const& camera, Scene const* const scene);
    void saveOutputImage();
};

#endif
