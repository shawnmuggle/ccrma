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
#include "SceneObject.h"
#include "Light.h"

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

void ImagePlane::generateRaysFromCamera(Camera camera, 
                                        std::vector<SceneObject> const* objects, 
                                        std::vector<AmbientLight> const* ambientLights,
                                        std::vector<PointLight> const* pointLights,
                                        std::vector<DirectionalLight> const* directionalLights)
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
            
            for (std::vector<SceneObject>::const_iterator itr = objects->begin(); itr != objects->end(); itr++)
            {
                SceneObject object = *itr;
                Intersection intersection;
                bool intersects = object.shape->intersectionWithRay(r, &intersection);
                
                if (intersects)
                {
                    STColor3f ambientColor = STColor3f();
                    for (std::vector<AmbientLight>::const_iterator aItr = ambientLights->begin(); aItr != ambientLights->end(); aItr++)
                    {
                        AmbientLight aLight = *aItr;
                        ambientColor += aLight.color;
                    }
                    ambientColor *= object.material.ambient;                    

#warning Do something about potential color overflow??
                    STColor3f diffuseColor;
                    STColor3f specularColor;
                    for (std::vector<PointLight>::const_iterator pItr = pointLights->begin(); pItr != pointLights->end(); pItr++)
                    {
                        PointLight pLight = *pItr;

                        // Diffuse lighting
                        STVector3 L = pLight.position - intersection.position;                        
                        L.Normalize();
                        float diffuseAmount = fmax(0, STVector3::Dot(L, intersection.normal));
                        diffuseColor += object.material.diffuse * pLight.color * diffuseAmount;
                        
                        // Specular lighting
                        L *= -1.0f;
                        STVector3 R = L - 2 * STVector3::Dot(L, intersection.normal) * intersection.normal;
                        R.Normalize();
                        STVector3 V = camera.position - intersection.position;
                        V.Normalize();
                        float specularAmount = powf(fmaxf(0, STVector3::Dot(R, V)), object.material.shine);
                        specularColor += object.material.specular * pLight.color * specularAmount;
                    }
                    
                    for (std::vector<DirectionalLight>::const_iterator dItr = directionalLights->begin(); dItr != directionalLights->end(); dItr++)
                    {
                        DirectionalLight dLight = *dItr;
                        
                        // Diffuse lighting
                        STVector3 L = -dLight.direction;
                        L.Normalize();
                        float diffuseAmount = fmax(0, STVector3::Dot(L, intersection.normal));
                        diffuseColor += object.material.diffuse * dLight.color * diffuseAmount;
                        
                        // Specular lighting
                        L *= -1.0f;
                        STVector3 R = L - 2 * STVector3::Dot(L, intersection.normal) * intersection.normal;
                        R.Normalize();
                        STVector3 V = camera.position - intersection.position;
                        V.Normalize();
                        float specularAmount = powf(fmaxf(0, STVector3::Dot(R, V)), object.material.shine);
                        specularColor += object.material.specular * dLight.color * specularAmount;
                    }
                    
                    outputImage->SetPixel(x, y, STImage::Pixel(ambientColor + diffuseColor + specularColor));
                }
                else
                {
                    outputImage->SetPixel(x, y, STImage::Pixel(0, 0, 0, 255));
                }
            }
        }
    }
}

void ImagePlane::saveOutputImage()
{
    outputImage->Save(outputFilename);
}