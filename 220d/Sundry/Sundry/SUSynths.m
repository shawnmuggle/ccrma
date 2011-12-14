//
//  SUSynths.m
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUSynths.h"
#import "fast_funcs.h"

static float TWO_PI = 2 * M_PI;

inline float pulse(float phase, float width)
{
    width = MAX(0.1, MIN(0.9, width));
    return phase < width * TWO_PI ? 1.0f : 0.0f;
}

inline float saw(float phase, float width)
{
    width = MAX(0.1, MIN(0.9, width));
    return phase < width * TWO_PI ? phase / width : 1.0f - ((phase - width) / (1.0f - width));
}

@interface SUFMSynth ()
{
    float phase;
    float modPhase;
    float modPhaseIncrement;
    float lfoPhase;
    float lfoPhaseIncrement;

    float gain;
    float gainFollowRate;
}
@end

@implementation SUFMSynth
@synthesize carrierFreq;
@synthesize modFreq, modDepth;
@synthesize lfoFreq, lfoDepth;

- (id)init {
    self = [super init];
    if (self) {
        gainFollowRate = 0.001;
        self.carrierFreq = 1.0f;
        self.modFreq = 1.0f;
        self.lfoFreq = 1.0f;
    }
    return self;
}

- (void)setModFreq:(float)inModFreq
{
    modFreq = inModFreq;
    modPhaseIncrement = TWO_PI * (modFreq / SAMPLE_RATE);
}

- (void)setLfoFreq:(float)inLfoFreq
{
    lfoFreq = inLfoFreq;
    lfoPhaseIncrement = TWO_PI * (lfoFreq / SAMPLE_RATE);
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain
{
    float freq;
    float phaseIncrement;
    float sample, modSample, lfoSample;
    for (int i = 0; i < buffer.numFrames; i++)
    {
        // track gain towards inGain (the desired playback gain) to alleviate zippering
        gain = gain + gainFollowRate * (inGain - gain);
        sample = gain * boosted_sin_lookup(phase);
        for (int j = 0; j < buffer.numChannels; j++)
        {
            buffer.buffer[i * buffer.numChannels + j] += sample;
        }
        
        lfoSample = sin_lookup(lfoPhase);

        modSample = sin_lookup(modPhase);
        freq = carrierFreq + modDepth * modSample * lfoSample;
        phaseIncrement = TWO_PI * (freq / SAMPLE_RATE);
        
//        lfoPhase = fmodf(lfoPhase + lfoPhaseIncrement, TWO_PI);
//        modPhase = fmodf(modPhase + modPhaseIncrement, TWO_PI);
//        phase = fmodf(phase + phaseIncrement, TWO_PI);
        
        // OPTIMIZATIONS
        lfoPhase += lfoPhaseIncrement;
        if (lfoPhase > TWO_PI)
            lfoPhase -= TWO_PI;
        modPhase += modPhaseIncrement;
        if (modPhase > TWO_PI)
            modPhase -= TWO_PI;
        phase += phaseIncrement;
        if (phase > TWO_PI)
            phase -= TWO_PI;
    }
}

@end

@interface SUPercSynth ()
{
    int lengthSamples;
    int playedSamples;

    float startAmplitude;
    
    float phase;
    
    float gain;
    float gainFollowRate;
}
@end

@implementation SUPercSynth
@synthesize length;
@synthesize startFreq, endFreq;

- (id)init {
    self = [super init];
    if (self) {
        gainFollowRate = 0.001;
        self.startFreq = 1.0f;
        self.endFreq = 1.0f;
        self.length = 0.0f;
    }
    return self;
}

- (void)setLength:(float)inLength
{
    length = inLength;
    lengthSamples = length * SAMPLE_RATE;
    playedSamples = lengthSamples + 1;  // This ensures that the synths don't all play when they first start processing samples
}

- (void)play:(float)inAmplitude
{
    startAmplitude = inAmplitude;
    playedSamples = 0;
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain
{
    float amplitude;
    float freq;
    float phaseIncrement;
    float sample;
        
    if (playedSamples > lengthSamples)
        return;
    
    for (int i = 0; i < buffer.numFrames; i++)
    {
        // track gain towards inGain (the desired playback gain) to alleviate zippering
        gain = gain + gainFollowRate * (inGain - gain);
        
        amplitude = 0.0f;
        freq = 1.0f;
        if (playedSamples <= lengthSamples)
        {
            amplitude = startAmplitude * expf(-playedSamples/(0.1f * lengthSamples));
            freq = startFreq + (endFreq - startFreq) * expf(-playedSamples/(0.1f * lengthSamples));
        }

        phaseIncrement = TWO_PI * (freq / SAMPLE_RATE);
        
        playedSamples++;
        
        sample = gain * amplitude * pulse(phase, 0.5);
        for (int j = 0; j < buffer.numChannels; j++)
        {
            buffer.buffer[i * buffer.numChannels + j] += sample;
        }
        
        phase += phaseIncrement;
        if (phase > TWO_PI)
            phase -= TWO_PI;
    }
}

@end