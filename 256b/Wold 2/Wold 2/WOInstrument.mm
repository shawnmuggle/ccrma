//
//  WOInstrument.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOInstrument.h"
#import "FileRead.h"

@implementation WOInstrument

- (id) init
{
    self = [super init];
    if (self) {
        NSString *filePath = [[NSBundle mainBundle] pathForResource:@"whistle" ofType:@"wav"];
        std::string fileName = [filePath UTF8String];
        bool typeRaw = false;
        
        // Attempt to load the soundfile data.
        stk::FileRead file( fileName, typeRaw );
        data.resize( file.fileSize(), file.channels() );
        NSLog(@"File size: %lu", file.fileSize());
        file.read( data );
        
        //grain = [[SoundGrain alloc] initWithRandomParamsAndFrames:&data];

        int numGrains = 100;
        grainIndex = 0;
        grainsDict = [[NSMutableDictionary alloc] init];
        grainsArray = [[NSMutableArray alloc] initWithCapacity:numGrains];
        for (int i = 0; i < numGrains; i++) {
            SoundGrain* grain = [[SoundGrain alloc] initWithRandomParamsAndFrames:&data];
            grain.masterGain = 0;
            [grainsArray addObject:grain];
        }
    }
    return self;
}

- (void) setGrainWithId:(int)pointId ParamsX:(float)x Y:(float)y
{
    NSNumber* idNum = [NSNumber numberWithInt:pointId];
    SoundGrain* grain = [grainsDict objectForKey:idNum];
    if (grain == nil && grainIndex < [grainsArray count]) {
        grain = [grainsArray objectAtIndex:grainIndex++];
        [grainsDict setObject:grain forKey:idNum];
    }
    
    // Map x to grain start position
    // NOTE: the y coordinate of this point actually corresponds to x in visible space, because the trees start out going sideways to the right before we rotate them into view at the "top" of the planet
    // NOTE: these values are raw and meaningless until we normalize them when we get the lSystem's bounding box
    grain.newBaseStart = y;
    //NSLog(@"Point for id %d : %f, %f", pointId, x, y);

    grain.newDensity = x;
}

- (void) updateParamsWithBoundingBox:(CGRect)boundingBox
{
    for (id key in grainsDict) {
        SoundGrain* grain = [grainsDict objectForKey:key];
        [grain updateValsInBoundingBox:boundingBox];
        grain.masterGain = 0.05;
    }
}

- (void) tickAudio:(stk::StkFrames*)frames;
{
    for (SoundGrain* grain in grainsArray) {
        if (grain.masterGain == 0) {
            // All the grains after this one haven't been used yet
            break;
        }
        [grain tickAudio:frames];
    }
}

@end
