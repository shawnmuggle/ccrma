//
//  SUSpace.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUSpace.h"
#import "SUWorld.h"

@implementation SUSpace
@synthesize worlds;

- (id)init {
    self = [super init];
    if (self) {
        int numWorlds = 1000;
        worlds = [NSMutableSet setWithCapacity:numWorlds];
        for (int i = 0; i < numWorlds; i++)
        {
            [worlds addObject:[[SUWorld alloc] init]];
        }

    }
    return self;
}

@end
