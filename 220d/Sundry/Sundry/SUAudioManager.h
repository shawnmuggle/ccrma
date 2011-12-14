//
//  SUAudio.h
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SUPlayer;
@class SUSpace;
@class SUWorld;

void AudioCallback( Float32 * buffer, UInt32 numFrames, void * userData );

@interface SUAudioManager : NSObject

@property (nonatomic) BOOL editMode;
@property (nonatomic, weak) SUWorld *editingWorld;

+ (SUAudioManager *)sharedAudioManager;
+ (void)initAudioWithPlayer:(SUPlayer *)player space:(SUSpace *)space;
- (void)audioCallback:(SUAudioBuffer)audioBuffer;

@end
