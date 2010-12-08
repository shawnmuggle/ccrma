/*
 *  Environment.cpp
 *  FlyingDream
 *
 *  Created by Michael Rotondo on 11/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


// SOME CODE TAKEN & MODIFIED FROM http://www.lighthouse3d.com/opengl/terrain/index.php3?introduction

#include "Environment.h"


Environment::Environment(void)
{
    for (int i = 0; i < 50; i++) {
        clouds.push_back(new Cuboid(new Vector3D(-2000 + 4000 * (rand() / (double)RAND_MAX),
                                                 -1000 + 3000 * (rand() / (double)RAND_MAX),
                                                 -2000 + 4000 * (rand() / (double)RAND_MAX)),
                                    new Vector3D(1.0, 1.0, 1.0),
                                    0.7,
                                    20 + 80 * (rand() / (double)RAND_MAX)));
    }
    
    x_dim = y_dim = z_dim = 4000.0;
    x_resolution = 500;
    z_resolution = 500;
    
    terrain_heights = (float *)malloc(sizeof(float) * x_dim * z_dim);
    memset(terrain_heights, 0, sizeof(float) * x_dim * z_dim);
    
    /*
    std::vector<float> sine_coefficients;
    sine_coefficients.push_back(0.13);
    sine_coefficients.push_back(0.15);
    sine_coefficients.push_back(0.12);
    sine_coefficients.push_back(0);
    sine_coefficients.push_back(0.18);
    sine_coefficients.push_back(0.14);
    sine_coefficients.push_back(0.16);
    sine_coefficients.push_back(0.19);
    sine_coefficients.push_back(0.11);
    sine_coefficients.push_back(0.1);
    
    
    int freq = 0;
    float y_value = 0;
    std::vector<float>::iterator itr;
    for (int x = 0; x < x_resolution; x++) {
        y_value = 0;
        freq = 0;
        itr = sine_coefficients.begin();
        while(itr != sine_coefficients.end()) {
            float coefficient = *itr;
            y_value += sin(coefficient * ++freq * x * 0.5) * 10;
            ++itr;
        }
        for (int z = 0; z < z_resolution; z++) {
            terrain_heights[z * x_resolution + x] = y_value;
        }
    }
    */
    

    // CIRCLES ALGORITHM
    int num_iterations = 100; // 1000 looks good
    int point_x, point_z;
    float height = 100, radius = 100, point_dist; // to be randomized
    for (int i = 0; i < num_iterations; i++) {
        point_x = (rand() / (float)RAND_MAX) * x_resolution;
        point_z = (rand() / (float)RAND_MAX) * z_resolution;
        //printf("Generated a lump at %d, %d\n", point_x, point_z);
        
        for (int x = 0; x < x_resolution; x++) {
            for (int z = 0; z < z_resolution; z++) {
                point_dist = sqrt(pow(z - point_z, 2) + pow(x - point_x, 2)) * 2 / radius;
                if (fabs(point_dist) < 1.0) {
                    //printf("Within radius at point: %d, %d - dist is %f, height is %f\n", x, z, point_dist, height / 2 + cos(point_dist * M_PI) * (height/2));
                    
                    terrain_heights[x * x_resolution + z] += height / 2 + cos(point_dist * M_PI) * (height/2);
                }
            }
        }
        height *= 0.99;
    }

    terrain_normals = (float *)malloc(sizeof(float) * 3 * x_dim * z_dim);
    ComputeTerrainNormals();
    
    terrain_dl = glGenLists(1);
    glNewList(terrain_dl,GL_COMPILE);
    
    
    // DRAW GROUND
    
    float half_x_dim = x_dim / 2;
    float half_y_dim = y_dim / 2;
    float half_z_dim = z_dim / 2;
    
    
    GLfloat mat_amb_diff[] = { 0.0, 1.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
    
    float segment_x_dim = x_dim / (float)x_resolution;
    float segment_z_dim = z_dim / (float)z_resolution;
    
    //printf("segment dimensions: %fx%f\n", segment_x_dim, segment_z_dim);
    
    std::vector<float>::iterator itr;
    
    for (int z = 0; z < z_resolution; z++) {
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0, 1, 0);
        for (int x = 0; x < x_resolution; x++) {
            //printf("tile origin: %f, %f\n", x * segment_x_dim, z * segment_z_dim);
            
            glNormal3f(terrain_normals[3 * (x * x_resolution + z)],
                       terrain_normals[3 * (x * x_resolution + z) + 1],
                       terrain_normals[3 * (x * x_resolution + z) + 2]);
            
            glVertex3f(x * segment_x_dim - half_x_dim, 
                       terrain_heights[z * x_resolution + x] - half_y_dim, 
                       z * segment_z_dim - half_z_dim);
            
            glNormal3f(terrain_normals[3 * ((x + 1) * x_resolution + z)],
                       terrain_normals[3 * ((x + 1) * x_resolution + z) + 1],
                       terrain_normals[3 * ((x + 1) * x_resolution + z) + 2]);	
            
            glVertex3f(x * segment_x_dim - half_x_dim, 
                       terrain_heights[(z + 1) * x_resolution + x] - half_y_dim, 
                       (z + 1) * segment_z_dim - half_z_dim);
        }
        glEnd();
    }
    glEndList();
    
}

// taken from website and modified (mostly variable renaming so i make sure i read the whole thing)
void Environment::ComputeTerrainNormals() {
    
	float *norm1,*norm2,*norm3,*norm4; 
	int z,x,k;
	
	if (terrain_normals == NULL)
		return;
    
    
	for(z = 0; z < z_resolution; z++)
		for(x = 0; x < x_resolution; x++) {
			norm1 = NULL;
			norm2 = NULL;
			norm3 = NULL;
			norm4 = NULL;
            
			/* normals for the four corners */
			if (z == 0 && x == 0) {
				norm1 = TerrainCrossProduct(0,0, 0,1, 1,0);	
				TerrainNormalize(norm1);				
			}
			else if (x == x_resolution-1 && z == z_resolution-1) {
				norm1 = TerrainCrossProduct(x,z, x,z-1, x-1,z);	
				TerrainNormalize(norm1);				
			}
			else if (x == 0 && z == z_resolution-1) {
				norm1 = TerrainCrossProduct(x,z, x,z-1, x+1,z);	
				TerrainNormalize(norm1);				
			}
			else if (x == x_resolution-1 && z == 0) {
				norm1 = TerrainCrossProduct(x,z, x,z+1, x-1,z);	
				TerrainNormalize(norm1);				
			}
            
			/* normals for the borders */
			else if (z == 0) {
				norm1 = TerrainCrossProduct(x,0, x-1,0, x,1);
				TerrainNormalize(norm1);
				norm2 = TerrainCrossProduct(x,0,x,1,x+1,0);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
			}
			else if (x == 0) {
				norm1 = TerrainCrossProduct(0,z, 1,z, 0,z-1);
				TerrainNormalize(norm1);
				norm2 = TerrainCrossProduct(0,z, 0,z+1, 1,z);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
			}
			else if (z == z_resolution-1) {
				norm1 = TerrainCrossProduct(x,z, x,z-1, x+1,z);
				TerrainNormalize(norm1);
				norm2 = TerrainCrossProduct(x,z, x+1,z, x,z-1);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
			}
			else if (x == x_resolution-1) {
				norm1 = TerrainCrossProduct(x,z, x,z-1, x-1,z);
				TerrainNormalize(norm1);
				norm2 = TerrainCrossProduct(x,z, x-1,z, x,z+1);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
			}
            
			/* normals for the inner vertices using 8 neighbours */
			else {
				norm1 = TerrainCrossProduct(x,z, x-1,z, x-1,z+1);
				TerrainNormalize(norm1);
				norm2 = TerrainCrossProduct(x,z, x-1,z+1, x,z+1);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
				norm2 = TerrainCrossProduct(x,z, x,z+1, x+1,z+1);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
				norm2 = TerrainCrossProduct(x,z, x+1,z+1, x+1,z);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
				norm2 = TerrainCrossProduct(x,z, x+1,z, x+1,z-1);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
				norm2 = TerrainCrossProduct(x,z, x+1,z-1, x,z-1);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
				norm2 = TerrainCrossProduct(x,z, x,z-1, x-1,z-1);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
				norm2 = TerrainCrossProduct(x,z, x-1,z-1, x-1,z);
				TerrainNormalize(norm2);
				TerrainAddVector(norm1,norm2);
				free(norm2);
			}
            
			TerrainNormalize(norm1);
			norm1[2] = - norm1[2];
			for (k = 0; k< 3; k++) {
				terrain_normals[3*(x*x_resolution + z) + k] = norm1[k]; // MIXED UP INDICES????
            }
			free(norm1);
			
            
		}
}

float *Environment::TerrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) {
    
	float *auxNormal,v1[3],v2[3];
    
    float segment_x_dim = x_dim / (float)x_resolution;
    float segment_z_dim = z_dim / (float)z_resolution;
    
	v1[0] = (x2-x1) * segment_x_dim; 
	v1[1] = -terrain_heights[z1 * x_resolution + x1] 
    + terrain_heights[z2 * x_resolution + x2];
	v1[2] = (z2-z1) * segment_z_dim; 
    
    
	v2[0] = (x3-x1) * segment_x_dim; 
	v2[1] = -terrain_heights[z1 * x_resolution + x1] 
    + terrain_heights[z3 * x_resolution + x3];
	v2[2] = (z3-z1) * segment_z_dim; 
    
	auxNormal = (float *)malloc(sizeof(float)*3);
    
	auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
	auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];
    
	return(auxNormal);
}

void Environment::TerrainNormalize(float *v) {
    
	double d;
	
	d = sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
    
	v[0] = v[0] / d;
	v[1] = v[1] / d;
	v[2] = v[2] / d;
}

void Environment::TerrainAddVector(float *a, float *b) {
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void Environment::Render(void)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    
    glPushMatrix();
    
    glColor3f(127/255.0, 178/255.0, 230/255.0);
    
    glBegin(GL_QUADS);
    
    //top of the sky
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f( 2000.0f, 2000.0f, -2000.0f);			// Top Right Of The Quad (Top)
    glVertex3f(-2000.0f, 2000.0f, -2000.0f);			// Top Left Of The Quad (Top)
    glVertex3f(-2000.0f, 2000.0f, 2000.0f);			// Bottom Left Of The Quad (Top)
    glVertex3f( 2000.0f, 2000.0f, 2000.0f);			// Bottom Right Of The Quad (Top)
    
    //front of the sky
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( 2000.0f, 2000.0f, 2000.0f);			// Top Right Of The Quad (Front)
    glVertex3f(-2000.0f, 2000.0f, 2000.0f);			// Top Left Of The Quad (Front)
    glVertex3f(-2000.0f,-2000.0f, 2000.0f);			// Bottom Left Of The Quad (Front)
    glVertex3f( 2000.0f,-2000.0f, 2000.0f);			// Bottom Right Of The Quad (Front)
    
    //back
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 2000.0f,-2000.0f,-2000.0f);			// Bottom Left Of The Quad (Back)
    glVertex3f(-2000.0f,-2000.0f,-2000.0f);			// Bottom Right Of The Quad (Back)
    glVertex3f(-2000.0f, 2000.0f,-2000.0f);			// Top Right Of The Quad (Back)
    glVertex3f( 2000.0f, 2000.0f,-2000.0f);			// Top Left Of The Quad (Back)
    
    //left
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-2000.0f, 2000.0f, 2000.0f);			// Top Right Of The Quad (Left)
    glVertex3f(-2000.0f, 2000.0f,-2000.0f);			// Top Left Of The Quad (Left)
    glVertex3f(-2000.0f,-2000.0f,-2000.0f);			// Bottom Left Of The Quad (Left)
    glVertex3f(-2000.0f,-2000.0f, 2000.0f);			// Bottom Right Of The Quad (Left)
    
    //right
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f( 2000.0f, 2000.0f,-2000.0f);			// Top Right Of The Quad (Right)
    glVertex3f( 2000.0f, 2000.0f, 2000.0f);			// Top Left Of The Quad (Right)
    glVertex3f( 2000.0f,-2000.0f, 2000.0f);			// Bottom Left Of The Quad (Right)
    glVertex3f( 2000.0f,-2000.0f,-2000.0f);			// Bottom Right Of The Quad (Right)

    /*
    glColor4f(53/255.0, 71/255.0, 140/255.0, 0.8);
    
    //top of the ocean
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 2000.0f, -2000.0f, 2000.0f);			// Top Right Of The Quad (Bottom)
    glVertex3f(-2000.0f, -2000.0f, 2000.0f);			// Top Left Of The Quad (Bottom)
    glVertex3f(-2000.0f, -2000.0f, -2000.0f);			// Bottom Left Of The Quad (Bottom)
    glVertex3f( 2000.0f, -2000.0f, -2000.0f);			// Bottom Right Of The Quad (Bottom)
    
     */
     
     glEnd();						// Done Drawing The Quad
    
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    
    
    glCallList(terrain_dl);

}

void Environment::RenderClouds()
{
    std::vector<Cuboid *>::iterator itr=clouds.begin();
    while(itr != clouds.end()) {
        Cuboid *cuboid = *itr;
        cuboid->Render();
        ++itr;
    }
}

void Environment::BoundPosition(Vector3D *position, Vector3D *velocity, Vector3D *gravity)
{
    if (position->x < -x_dim / 2) {
        position->x = -x_dim / 2 + 3;
        velocity->x = 0;
    }
    else if (position->x > x_dim / 2) {
        position->x = x_dim / 2 - 3;
        velocity->x = 0;
    }

    if (position->z < -z_dim / 2) {
        position->z = -z_dim / 2 + 3;
        velocity->z = 0;
    }
    else if (position->z > z_dim / 2) {
        position->z = z_dim / 2 - 3;
        velocity->z = 0;
    }
    
    int x_height_index = (-position->x + x_dim / 2) / (float)x_dim * x_resolution;
    int z_height_index = (-position->z + z_dim / 2) / (float)x_dim * z_resolution;
    float y_value = terrain_heights[z_height_index * x_resolution + x_height_index] - y_dim / 2;
    
    float dist_from_ground = -y_value - position->y; // positive values mean above ground
    
    if (dist_from_ground < 25) {
        position->y += (dist_from_ground - 25) * 0.7;
        *velocity -= *gravity;
    }
}