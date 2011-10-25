//
//  SAGrassBlade.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/24/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SAPhysicsObject.h"
#import "SADrawableObject.h"

@class SAWorld;

@interface SAGrassBlade : SAPhysicsObject <SADrawableObject>

+ (id)grassbladeWithWorld:(SAWorld *)world atPosition:(CGPoint)position;

@end
