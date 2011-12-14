//
//  SUSynth.h
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@interface SUFMSynth : NSObject

@property (nonatomic) float modFreq;
@property (nonatomic) float modDepth;
@property (nonatomic) float carrierFreq;
@property (nonatomic) float lfoFreq;
@property (nonatomic) float lfoDepth;

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain;

@end
