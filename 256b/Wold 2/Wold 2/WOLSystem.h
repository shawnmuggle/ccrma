//
//  WOLSystem.h
//  wold
//
//  Created by Michael Rotondo on 2/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import "WOInstrument.h"

@interface WOLSystemTransformState : NSObject {

    float width;
    float angle;
    float x;
    float y;

}
@property float width;
@property float angle;
@property float x;
@property float y;

@end


@interface WOLSystemTransformStack : NSObject {

    NSMutableArray* stack;
}

- (void) push;
- (void) pop;
- (void) translateBy:(float)length;
- (void) rotateBy:(float)angle;
- (CGPoint) getPoint;

@end


@interface WOLSystem : NSObject {

    NSMutableArray* nodes;
    
    int maxGeneration;
    int currentGeneration;
    int ticksPerGeneration;
    int generationTickCount;
    
    BOOL growing;
    CGMutablePathRef path;
    CAShapeLayer* layer;
    
    WOInstrument* instrument;
}

@property (nonatomic, retain) NSMutableArray* nodes;

@property int maxGeneration;
@property int currentGeneration;
@property int ticksPerGeneration;
@property int generationTickCount;
@property BOOL growing;

@property CGMutablePathRef path;
@property (nonatomic, retain) CAShapeLayer* layer;

@property (nonatomic, retain) WOInstrument* instrument;

- (id) initWithMaxGeneration:(int)maxGen andAngle:(float)newAngle andOrigin:(CGPoint)origin;
- (void) advanceGeneration;
- (void) tick;
- (void) tickAudio:(stk::StkFrames*)frames;
- (void) setAge:(float)age;

@end

@interface WONode : NSObject {

    float growthPercent;
    float randomOffset;
    float offset;
}

@property (setter=setNewGrowthPercent:,getter=getGrowthPercent) float growthPercent;
@property float offset;
@property float randomOffset;

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration;
- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system;
- (void) setNewGrowthPercent:(float)percent;
- (float) getGrowthPercent;

@end

@interface WOANode : WONode {}
@end

@interface WOLittleFNode : WONode
{
    float baseLength;
    float maxOffset;
    
    int pointId;
}

@property int pointId;

- (id) initWithBaseLength:(float)newBaseLength andMaxOffset:(float)newMaxOffset;

@end

@interface WOLNode : WONode {}
@end

@interface WOWiggleNode : WONode {}
@end

@interface WOAngleNode : WONode 
{
    float baseAngle;
    float maxOffset;
}

- (id) initWithBaseAngle:(float)newBaseAngle andMaxOffset:(float)newMaxOffset;

@end

@interface WOLeftBracketNode : WONode {}
@end

@interface WORightBracketNode : WONode {}
@end