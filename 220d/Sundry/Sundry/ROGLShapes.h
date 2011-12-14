//
//  ROGLShapes.h
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ROGLShapes : NSObject

+ (ROGLShapes *)sharedInstance;

- (void)prepareToDrawCubes;
- (void)drawCube;
- (void)prepareToDrawTriangles;
- (void)prepareToDrawSpringThings;
@end
