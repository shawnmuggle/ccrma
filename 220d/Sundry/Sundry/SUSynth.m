//
//  SUSynth.m
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUSynth.h"

inline float pulse(float phase, float width)
{
    width = MAX(0.1, MIN(0.9, width));
    return phase < width * 2 * M_PI ? 1.0f : 0.0f;
}

inline float saw(float phase, float width)
{
    width = MAX(0.1, MIN(0.9, width));
    return phase < width * 2 * M_PI ? phase / width : 1.0f - ((phase - width) / (1.0f - width));
}

@interface SUSynth ()
{
    float phase;
    float lfoPhase;
    float lfoPhaseIncrement;

    float gain;
    float gainFollowRate;
    
    GLKVector3 waveBalance;
}
@end

@implementation SUSynth
@synthesize lfoFreq;
@synthesize lfoDepth;
@synthesize centerFreq;
@synthesize waveAmounts;

- (id)init {
    self = [super init];
    if (self) {
        gainFollowRate = 0.001;
        centerFreq = 1.0f;
        lfoFreq = 1.0f;
        
        waveBalance = GLKVector3Make(1.0f, 0.2f, 0.1f);
    }
    return self;
}

- (void)setLfoFreq:(float)inLfoFreq
{
    lfoFreq = inLfoFreq;
    lfoPhaseIncrement = (2 * M_PI) * (lfoFreq / SAMPLE_RATE);
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain
{
    float freq;
    float phaseIncrement;
    float sample, lfoSample;
    for (int i = 0; i < buffer.numFrames; i++)
    {
        // track gain towards inGain (the desired playback gain) to alleviate zippering
        gain = gain + gainFollowRate * (inGain - gain);
        sample = gain * (waveAmounts.x * waveBalance.x * sin(phase * 2) + 
                         waveAmounts.y * waveBalance.y * pulse(phase, cosf(lfoPhase) / 2.0f + 1.0f) +
                         waveAmounts.z * waveBalance.z * saw(phase, sinf(lfoPhase) / 4.0f + 0.5f));
        for (int j = 0; j < buffer.numChannels; j++)
        {
            buffer.buffer[i * buffer.numChannels + j] += sample;
        }

        lfoSample = sinf(lfoPhase);
        freq = centerFreq + lfoDepth * lfoSample;
        phaseIncrement = (2 * M_PI) * (freq / SAMPLE_RATE);
        
        lfoPhase = fmodf(lfoPhase + lfoPhaseIncrement, 2 * M_PI);
        phase = fmodf(phase + phaseIncrement, 2 * M_PI);
    }
}

@end
