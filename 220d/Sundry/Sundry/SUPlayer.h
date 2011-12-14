//
//  SUPlayer.h
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@class SUNoiseSynth;

@interface SUPlayer : NSObject

@property (nonatomic) GLKVector3 position;
@property (nonatomic) GLKQuaternion orientation;
@property (nonatomic, strong) SUNoiseSynth *synth;

- (id)initWithPosition:(GLKVector3)inPosition orientation:(GLKQuaternion)inOrientation;
- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain;
@end
