/*
 *  Environment.h
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 11/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Entity.h"
#include "vector3d.h"

#include <vector>

class Environment
{
public:
    Environment(void);
    void Render();
    void RenderClouds();
    void BoundPosition(Vector3D *position, Vector3D *velocity, Vector3D *gravity);
private:
    std::vector<Cuboid *> clouds;
    float *terrain_heights;
    float *terrain_normals;
    float x_dim, y_dim, z_dim;
    int x_resolution, z_resolution;
    GLuint terrain_dl;
    
    void ComputeTerrainNormals();
    float *TerrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3);
    void TerrainNormalize(float *v);
    void TerrainAddVector(float *a, float *b);
};