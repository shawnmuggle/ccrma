//
//  SASoundPath.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/29/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SAPhysicsObject.h"
#import "SAAudioProducer.h"
#import "SADrawableObject.h"

@class SASoundPoint;

@interface SASoundPath : SAPhysicsObject <SAAudioProducer, SADrawableObject>

- (void)addPoint:(SASoundPoint *)newPoint;

@end
