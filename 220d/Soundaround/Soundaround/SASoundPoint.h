//
//  SASoundPoint.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/31/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SASoundPoint : NSObject

@property (nonatomic) CGPoint position;
@property (nonatomic) float intensity;
@property (nonatomic) float angle;

- (id)initWithPosition:(CGPoint)inPosition andIntensity:(float)inIntensity andAngle:(float)inAngle;

@end
