//
//  SUPlayer.m
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUPlayer.h"
#import "SUSynths.h"

@interface SUPlayer ()
{
    float lastDistance;
}
@end

@implementation SUPlayer
@synthesize position;
@synthesize orientation;
@synthesize synth;

- (id)initWithPosition:(GLKVector3)inPosition orientation:(GLKQuaternion)inOrientation
{
    self = [super init];
    if (self) {
        self.position = inPosition;
        self.orientation = inOrientation;
        self.synth = [[SUNoiseSynth alloc] init];
    }
    return self;
}

- (void)setPosition:(GLKVector3)inPosition
{
    lastDistance = GLKVector3Length(GLKVector3Subtract(position, inPosition));
    position = inPosition;
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain
{    
    float masterGain = MIN(1.0f, 0.0004f * lastDistance + 0.0001f * lastDistance * (arc4random() / (float)0x100000000));
    float gain = inGain * masterGain;
    [self.synth renderAudioIntoBuffer:buffer gain:gain];
}

@end
