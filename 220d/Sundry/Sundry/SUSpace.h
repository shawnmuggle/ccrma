//
//  SUSpace.h
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SUPlayer.h"

@interface SUSpace : NSObject

@property (nonatomic, strong) NSMutableSet *worlds;

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player;

@end
