//
//  ROGLProgram.h
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@interface ROGLProgram : NSObject

- (id)initWithVertexShader:(NSString *)vertexShaderFilename fragmentShader:(NSString *)fragmentShaderFilename;
- (void)use;

@end

@interface ROGLGouraudProgram : ROGLProgram

+ (ROGLGouraudProgram *)sharedInstance;
- (void)setModelViewProjectionMatrix:(GLKMatrix4)modelViewProjectionMatrix normalMatrix:(GLKMatrix3)normalMatrix;
- (void)setColor:(GLKVector4)color;

@end