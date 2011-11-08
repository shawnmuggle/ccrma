//
//  SAPhysicsObject.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/23/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

class b2Body;
class b2Contact;

@interface SAPhysicsObject : NSObject

@property (nonatomic, assign) b2Body *physicsBody;
@property (nonatomic, readonly) CGPoint position;
@property (nonatomic, readonly) float speed;

- (void)processContact:(b2Contact *)contact withOtherObject:(id)otherObject;
- (BOOL)shouldDisableContact:(b2Contact *)contact;
- (void)update;

@end
