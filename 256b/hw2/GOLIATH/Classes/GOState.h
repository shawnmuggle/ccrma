//
//  GoliathState.h
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "mo_gfx.h"

@class GOBall;

@interface GOState : NSObject {

    NSMutableDictionary* touches;
    
    NSMutableArray* line_points;
    NSMutableArray* line_offsets;
    
    NSMutableArray* balls;
    int num_wall_impacts;
    
    NSMutableArray* collisions;
    int num_ball_impacts;
    
    long tickCount;
    float stringMultiplier;
    
    float stringFrequency;
    NSMutableSet* touchesPluckable;
    
    float damping;
    float reverb;
    BOOL shake;
    BOOL gravity;
}
- (void) addTouch:(UITouch*) touch;
- (void) removeTouch:(UITouch*) touch;
- (void) moveTouch:(UITouch*) touch;
- (void) tick;
- (Vector3D) getShotVelocity:(UITouch*) touch;
- (BOOL) slingshotPresent;
- (void) clear;

@property (nonatomic, assign, readonly) NSMutableDictionary* touches;
@property (nonatomic, retain, readonly) NSMutableArray* line_points;
@property (nonatomic, retain, readonly) NSMutableArray* line_offsets;
@property (nonatomic, retain, readonly) NSMutableArray* balls;
@property (nonatomic, retain, readonly) NSMutableArray* collisions;
@property int num_wall_impacts;
@property int num_ball_impacts;
@property float stringFrequency;
@property float damping;
@property float reverb;
@property BOOL shake;
@property BOOL gravity;

@end

#define BALL_LIFETIME 1000

@interface GOBall : NSObject {

    Vector3D* position;
    Vector3D* velocity;
    
    int age;
    int lifetime;
    
    float radius;
    
}
- (id) initWithPosition:(Vector3D)aPosition andVelocity:(Vector3D)aVelocity;


@property Vector3D* position;
@property Vector3D* velocity;
@property int age;
@property int lifetime;
@property float radius;

@end

#define COLLISION_LIFETIME 50

@interface GOCollision : NSObject {

    Vector3D* position;
    
    int age;
    int lifetime;
}

@property Vector3D* position;
@property int age;
@property int lifetime;

- (id)initWithPosition:(Vector3D)aPosition;

@end