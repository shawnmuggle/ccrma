//
//  SABall.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/21/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SAWorld;
class b2Body;

@interface SABall : NSObject

+ (id)ballInWorld:(SAWorld *)world;
- (void)draw;
- (void)throwTowards:(CGPoint)point;

@end
