//
//  SAWorld.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/20/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

class b2World;
@class SABall;

@interface SAWorld : NSObject

@property (nonatomic, retain) NSMutableArray *terrainPoints;
@property (nonatomic, assign) b2World *physicsWorld;
@property (nonatomic, retain) SABall *ball;

+ (id)world;
- (void)update:(NSTimer *)timer;
- (void)draw;
- (void)setTargetPoint:(CGPoint)targetPoint;

@end
