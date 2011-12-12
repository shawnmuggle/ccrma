//
//  SUPlayer.h
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@interface SUPlayer : NSObject

@property (nonatomic) GLKVector3 position;
@property (nonatomic) GLKQuaternion orientation;

- (id)initWithPosition:(GLKVector3)inPosition orientation:(GLKQuaternion)inOrientation;

@end
