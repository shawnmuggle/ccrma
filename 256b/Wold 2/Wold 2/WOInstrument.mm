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
@synthesize grainsArray;


- (id) initWithFilename:(NSString*)filename
{
    self = [super init];
    if (self) {
        NSString *filePath = [[NSBundle mainBundle] pathForResource:filename ofType:@"wav"];
        std::string fileName = [filePath UTF8String];
        bool typeRaw = false;
        
        // Attempt to load the soundfile data.
        stk::FileRead file( fileName, typeRaw );
        data.resize( file.fileSize(), file.channels() );
        file.read( data );
        
        //grain = [[SoundGrain alloc] initWithRandomParamsAndFrames:&data];

        int numGrains = 100;
        grainIndex = 0;
        grainsDict = [[NSMutableDictionary alloc] init];
        grainsArray = [[NSMutableArray alloc] initWithCapacity:numGrains];
        for (int i = 0; i < numGrains; i++) {
            SoundGrain* grain = [[SoundGrain alloc] initWithRandomParamsAndFrames:&data];
            [grainsArray addObject:grain];
        }
        
        gain = 0.0;
        gainTarget = 0.0;
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
        grain.on = YES;
        gainTarget = 0.75;
    }
}

- (void) handleTouch:(CGPoint)loc withVelocity:(CGPoint)vel
{
    gainTarget = 0.25 + loc.y * 0.75;
    float densityOffset = -vel.y * 0.000005;
    float lengthOffset = vel.y * 0.0005;
        
//    if (densityOffset > 2.0) densityOffset = 2.0;
//    if (densityOffset < -0.5) densityOffset = -0.5;
    
    for (id key in grainsDict) {
        SoundGrain* grain = [grainsDict objectForKey:key];
        grain.on = YES;
        grain.densityOffset += densityOffset;
        grain.lengthOffset += lengthOffset;
    }
}

- (void) tick
{
    gain += (gainTarget - gain) * 0.5;
    gainTarget *= 0.95;
    if (gain < 0.00001)
    {
        for (SoundGrain* grain in grainsArray) {
            if (!grain.on) {
                // All the grains after this one haven't been used yet
                break;
            }
            grain.on = NO;
        }        
    }
}

- (void) tickAudio:(stk::StkFrames*)frames;
{
    for (SoundGrain* grain in grainsArray) {
        if (!grain.on) {
            // All the grains after this one haven't been used yet
            break;
        }
        grain.masterGain = gain;
        [grain tickAudio:frames];
    }
}

@end
