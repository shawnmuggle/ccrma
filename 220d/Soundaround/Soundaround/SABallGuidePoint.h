//
//  SABallGuidePoint.h
//  Soundaround
//
//  Created by Michael Rotondo on 11/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SABall;

@interface SABallGuidePoint : NSObject

@property CGPoint position;
@property NSTimeInterval timeToNextPoint;

- (id)initWithPosition:(CGPoint)inPosition;
- (BOOL)isBallClose:(SABall *)ball;

@end
