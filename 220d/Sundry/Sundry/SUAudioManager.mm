//
//  SUAudio.m
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUAudioManager.h"
#import "SUViewController.h"
#import "SUPlayer.h"
#import "SUSpace.h"

#import "mo_audio.h"

static int numChannels = 2;

void AudioCallback( Float32 * buffer, UInt32 numFrames, void * userData )
{
    @autoreleasepool {        
        SUAudioManager *audioManager = (__bridge SUAudioManager *)userData;
        SUAudioBuffer audioBuffer;
        
        memset(buffer, 0, numFrames * numChannels * sizeof(Float32));
        
        audioBuffer.buffer = buffer;
        audioBuffer.numFrames = numFrames;
        audioBuffer.numChannels = numChannels;
        [audioManager audioCallback:audioBuffer];
    }
}

@interface SUAudioManager ()

@property (nonatomic, weak) SUPlayer *player;
@property (nonatomic, weak) SUSpace *space;

+ (SUAudioManager *)sharedAudioManager;

@end

@implementation SUAudioManager
@synthesize player;
@synthesize space;

+ (void)initAudioWithPlayer:(SUPlayer *)player space:(SUSpace *)space
{
    SUAudioManager *audioManager = [SUAudioManager sharedAudioManager];
    audioManager.player = player;
    audioManager.space = space;
    MoAudio::init(SAMPLE_RATE, 4096, numChannels);
    MoAudio::start(AudioCallback, (__bridge void *) audioManager);

}

+ (SUAudioManager *)sharedAudioManager
{
    static SUAudioManager * audioManager;
    if (audioManager == nil)
    {
        audioManager = [[SUAudioManager alloc] init];
    }
    return audioManager;
}

- (id)init {
    self = [super init];
    if (self) {
    }
    return self;
}

- (void)dealloc {
    MoAudio::stop();
}

- (void)audioCallback:(SUAudioBuffer)audioBuffer
{
    [self.space renderAudioIntoBuffer:audioBuffer forPlayer:self.player];
}

@end
