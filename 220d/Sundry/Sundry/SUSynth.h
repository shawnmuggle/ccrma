//
//  SUSynth.h
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@interface SUSynth : NSObject

@property (nonatomic) float lfoFreq;
@property (nonatomic) float lfoDepth;
@property (nonatomic) float centerFreq;
@property (nonatomic) GLKVector3 waveAmounts;

// others: different types of basic waveforms, different type of lfo waveforms, lfo depth, num harmonics, harmonic weights, noise component, detune/thickness

// put rhythmic stuff elsewhere

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)gain;

@end
