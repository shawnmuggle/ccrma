//
//  SUWorld.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUWorld.h"
#import "ROGLProgram.h"
#import "ROGLShapes.h"

@interface SUWorld ()
{
    ROGLGouraudProgram *program;
    
    float angle;
    float angleIncrement;
    GLKVector3 axis;
    
    float scale;
}
@end

@implementation SUWorld
@synthesize position;

- (id)init {
    self = [super init];
    if (self) {
        program = [ROGLGouraudProgram sharedInstance];
        
        position = GLKVector3Make(0.0f, 0.0f, 0.0f);
        position = GLKVector3Make(100 * (arc4random() / (float)0x100000000) - 50, 
                                  100 * (arc4random() / (float)0x100000000) - 50, 
                                  100 * (arc4random() / (float)0x100000000) - 50);
        angle = 0.0f;
        angleIncrement = 0.1 * (arc4random() / (float)0x100000000);
        axis = GLKVector3Make((arc4random() / (float)0x100000000), 
                              (arc4random() / (float)0x100000000), 
                              (arc4random() / (float)0x100000000));
        scale = (arc4random() / (float)0x100000000);
    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
{
    [program use];
    
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(position.x, position.y, position.z);
    modelViewMatrix = GLKMatrix4Scale(modelViewMatrix, scale, scale, scale);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, angle, axis.x, axis.y, axis.z);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    angle += angleIncrement;
    
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
    [program setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    
    [[ROGLShapes sharedInstance] drawCube];
}

@end
