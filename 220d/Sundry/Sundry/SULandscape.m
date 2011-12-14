//
//  SULandscape.m
//  Sundry
//
//  Created by Michael Rotondo on 12/14/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SULandscape.h"
#import "ROGLProgram.h"

// square grid
#define GRID_DIMENSION 10

@interface SULandscape ()
{
    float grid[GRID_DIMENSION][GRID_DIMENSION];
    GLKVector3 cornerPosition;
    float gridCellSize;
    GLKVector4 color;
}

@end

@implementation SULandscape

- (id)init {
    self = [super init];
    if (self) {
        for (int i = 0; i < GRID_DIMENSION; i++)
        {
            for (int j = 0; j < GRID_DIMENSION; j++)
            {
                grid[i][j] = 400.0f * (arc4random() / (float)0x100000000);
            }
        }
        cornerPosition = GLKVector3Make(0.0f, 1000.0f, 0.0f);
        gridCellSize = 400.0f;
        color = GLKVector4Make(0.2f, 0.9f, 0.35f, 1.0f);
    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
{
    [[ROGLGouraudProgram sharedInstance] use];

    GLKMatrix4 modelViewMatrix = GLKMatrix4Translate(baseModelViewMatrix, -cornerPosition.x, -cornerPosition.y, -cornerPosition.z);
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    [[ROGLGouraudProgram sharedInstance] setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    [[ROGLGouraudProgram sharedInstance] setColor:color];

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    for (int i = 0; i < GRID_DIMENSION - 1; i++)
    {
        int numVerticesPerIteration = 3 * 2 * 2;
        GLKVector3 vertices[numVerticesPerIteration * (GRID_DIMENSION - 1)];  // individual triangles of positions + normals
        for (int j = 0; j < GRID_DIMENSION - 1; j++)
        {
            GLKVector3 p1 = GLKVector3Make(i * gridCellSize, grid[i][j], j * gridCellSize);
            GLKVector3 p2 = GLKVector3Make((i + 1) * gridCellSize, grid[i+1][j], j * gridCellSize);
            GLKVector3 p3 = GLKVector3Make(i * gridCellSize, grid[i][j+1], (j + 1) * gridCellSize);
            GLKVector3 p4 = GLKVector3Make((i + 1) * gridCellSize, grid[i+1][j+1], (j + 1) * gridCellSize);

            
            GLKVector3 t1Normal = GLKVector3Normalize(GLKVector3CrossProduct(GLKVector3Subtract(p3, p1), GLKVector3Subtract(p2, p1)));
            vertices[j * numVerticesPerIteration + 0] = p1;
            vertices[j * numVerticesPerIteration + 1] = p2;
            vertices[j * numVerticesPerIteration + 2] = p3;
            vertices[j * numVerticesPerIteration + 3] = t1Normal;            
            vertices[j * numVerticesPerIteration + 4] = t1Normal;
            vertices[j * numVerticesPerIteration + 5] = t1Normal;
            
            GLKVector3 t2Normal = GLKVector3Normalize(GLKVector3CrossProduct(GLKVector3Subtract(p1, p2), GLKVector3Subtract(p4, p2)));
            vertices[j * numVerticesPerIteration + 6] = p2;
            vertices[j * numVerticesPerIteration + 7] = p4;
            vertices[j * numVerticesPerIteration + 8] = p3;
            vertices[j * numVerticesPerIteration + 9] = t2Normal;
            vertices[j * numVerticesPerIteration + 10] = t2Normal;
            vertices[j * numVerticesPerIteration + 11] = t2Normal;
        }
        glEnableVertexAttribArray(GLKVertexAttribPosition);
        glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices);
        glEnableVertexAttribArray(GLKVertexAttribNormal);
        glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices + 3);
        glDrawArrays(GL_TRIANGLES, 0, numVerticesPerIteration * (GRID_DIMENSION - 2));
    }
    
}


@end
