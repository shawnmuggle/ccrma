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
    
    STPoint3 bilinearInterpolate(float s, float t);
    
public:
    ImagePlane() : outputImage(NULL) {}
    void createBoundaryPoints(Camera camera, float fovy, float aspect);
    void createOutputImage(const int &width, const int &height, const std::string &filename);
    void generateRaysFromCamera(Camera camera, Scene const* scene);
    void saveOutputImage();
};

#endif
