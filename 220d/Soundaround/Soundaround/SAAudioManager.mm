//
//  SAAudioManager.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/23/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include "RtAudio.h"

#import "SAAudioManager.h"
#import "SAWorld.h"

@interface SAAudioManager ()

@property (nonatomic, assign) RtAudio *audio;
@property (nonatomic, assign) SAWorld *world;

- (void)initAudio;
- (void)audioCallbackWithOutput:(void*)outputBuffer input:(void*)input numBufferFrames:(int)numBufferFrames;

@end

int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                  double streamTime, RtAudioStreamStatus status, void *data);

@implementation SAAudioManager
@synthesize audio;
@synthesize world;

+ (id)audioManagerWithWorld:(SAWorld *)aWorld
{
    SAAudioManager *audioManager = [[[SAAudioManager alloc] init] autorelease];

    audioManager.world = aWorld;
    audioManager.audio = new RtAudio();
    [audioManager initAudio];
    
    return audioManager;
}

- (void)initAudio
{
    // Let RtAudio print messages to stderr.
    self.audio->showWarnings( true );
    
    // Set our stream parameters for output only.
    unsigned int bufferFrames = 256;
    RtAudio::StreamParameters oParams;
    oParams.deviceId = self.audio->getDefaultOutputDevice();
    oParams.nChannels = 2;
    oParams.firstChannel = 0;
    
    RtAudio::StreamOptions options;
    options.flags |= RTAUDIO_HOG_DEVICE;
    options.flags |= RTAUDIO_SCHEDULE_REALTIME;
    
    unsigned int fs = 44100;
    
    try {
        self.audio->openStream( &oParams, NULL, RTAUDIO_FLOAT32, fs, &bufferFrames, &audioCallback, (void *)self, &options );
        self.audio->startStream();
    }
    catch ( RtError& e ) {
        e.printMessage();
        return;
    }
}

- (void)dealloc
{
    if ( audio->isStreamOpen() ) audio->closeStream();
    delete audio;
    [super dealloc];
}

- (void)audioCallbackWithOutput:(void *)outputBuffer input:(void *)inputBuffer numBufferFrames:(int)numBufferFrames
{
    float *output = (float *)outputBuffer;
    int numChannels = 2;
    
    for (int i = 0; i < numBufferFrames; i++)
    {
        for (int j = 0; j < numChannels; j++)
        {
            output[i * numChannels + j] = 0;
        }
    }
    [self.world audioProducer:self.world fillBuffer:output withNumFrames:numBufferFrames];
}

@end

int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                  double streamTime, RtAudioStreamStatus status, void *data)
{
    SAAudioManager * audioManager = (SAAudioManager *)data;
    [audioManager audioCallbackWithOutput:outputBuffer input:inputBuffer numBufferFrames:nBufferFrames];
    return 0;
}