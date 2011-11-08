//
//  SABallGuide.h
//  Soundaround
//
//  Created by Michael Rotondo on 11/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SAAudioProducer.h"
#import "SADrawableObject.h"

@class SABallGuidePoint;
@class SAWorld;
@class SABall;

@interface SABallGuide : NSObject <SAAudioProducer, SADrawableObject>

- (void)addPoint:(SABallGuidePoint *)newPoint;
- (BOOL)isValid;
- (BOOL)shouldActivateInWorld:(SAWorld *)world;
- (void)activateWithBall:(SABall *)ball;
- (void)deactivate;
- (void)updateInWorld:(SAWorld *)world withTimeInterval:(NSTimeInterval)timeInterval;

@end
