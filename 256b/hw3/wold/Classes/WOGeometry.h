//
//  WOGeometry.h
//  wold
//
//  Created by Michael Rotondo on 2/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "mo_gfx.h"
#import "Vector.hpp"
#import <vector>

struct Vertex {
    vec3 Position;
    vec3 Normal;
};

@interface WOGeometry : NSObject {

}

+ (void)drawSphereWithRadius:(GLfloat)r andNumLats:(GLint)lats andNumLongs:(GLint)longs;

+ (void)generateFrustumVBO;
+ (void)startDrawingFrustums;
+ (void)stopDrawingFrustums;
+ (void)addFrustumToVerticesVector:(std::vector<Vertex>*)vertices 
                  andIndicesVector:(std::vector<GLushort>*) indices
                  withBottomRadius:(GLfloat)bottomRadius 
                         andHeight:(GLfloat)height;
+ (void)drawFrustumWithBottomRadius:(GLfloat)rBottom andTopRadius:(GLfloat)topRadius andHeight:(GLfloat)h andSections:(GLint)sections;

+ (void)generateDisk;
+ (void)drawDiskWithRadius:(GLfloat)r andSections:(GLint)sections;

@end
