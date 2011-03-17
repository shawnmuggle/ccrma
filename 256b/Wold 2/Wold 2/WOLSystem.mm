//
//  WOLSystem.m
//  wold
//
//  Created by Michael Rotondo on 2/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOLSystem.h"

@implementation WOLSystemTransformState
@synthesize width, angle, x, y;
@end

@implementation WOLSystemTransformStack

- (id) init
{
    self = [super init];
    if (self) {
        stack = [[NSMutableArray alloc] init];
        WOLSystemTransformState* firstState = [[[WOLSystemTransformState alloc] init] autorelease];
        firstState.width = 0;
        firstState.angle = 0;
        firstState.x = 0;
        firstState.y = 0;
        [stack addObject:firstState];
    }
    return self;
}

- (void) push
{
    WOLSystemTransformState* oldState = [stack lastObject];
    WOLSystemTransformState* newState = [[[WOLSystemTransformState alloc] init] autorelease];
    newState.width = oldState.width;
    newState.angle = oldState.angle;
    newState.x = oldState.x;
    newState.y = oldState.y;
    
    [stack addObject:newState];
}

- (void) pop
{
    [stack removeLastObject];
}

- (void) dealloc
{
    [stack release];
    
    [super dealloc];
}

- (void) rotateBy:(float)angle
{
    WOLSystemTransformState* currentState = [stack lastObject];
    currentState.angle += angle;
}

- (void) translateBy:(float)length
{
    WOLSystemTransformState* currentState = [stack lastObject];
    currentState.x = currentState.x + length * cos((M_PI / 180) * currentState.angle);
    currentState.y = currentState.y - length * sin((M_PI / 180) * currentState.angle); // Growing upwards towards negative Y
}

- (CGPoint) getPoint
{
    WOLSystemTransformState* currentState = [stack lastObject];
    return CGPointMake(currentState.x, currentState.y);
}

@end



@implementation WOLSystem

@synthesize nodes;
@synthesize maxGeneration;
@synthesize currentGeneration;
@synthesize ticksPerGeneration;
@synthesize generationTickCount;
@synthesize growing;
@synthesize path;
@synthesize layer;
@synthesize instrument;

- (id) initWithMaxGeneration:(int)maxGen andAngle:(float)newAngle andOrigin:(CGPoint)origin
{
    self = [super init];
    if (self) {
        self.nodes = [[NSMutableArray alloc] init];
        self.maxGeneration = maxGen;
        self.currentGeneration = 0;
        self.generationTickCount = 0;
        self.ticksPerGeneration = 20;
        
        self.growing = YES;
        
        self.layer = [CAShapeLayer layer];
        
        CGAffineTransform rotationTransform = CGAffineTransformMakeRotation(newAngle);
        CGAffineTransform translationTransform = CGAffineTransformMakeTranslation(origin.x, origin.y);
        CGAffineTransform transform = CGAffineTransformConcat(rotationTransform, translationTransform);
        [self.layer setAffineTransform:transform];
        
        instrument = [[WOInstrument alloc] init];
    }
    return self;
}

- (void) advanceGeneration
{
    self.currentGeneration++;
    self.generationTickCount = 0;

    NSMutableArray* newNodes = [[NSMutableArray alloc] init];
    for (WONode* node in self.nodes) {
        [newNodes addObjectsFromArray:[node expandInLSystem:self isLastGeneration:self.currentGeneration == self.maxGeneration]];
    }

    self.nodes = newNodes;

    if (self.currentGeneration == self.maxGeneration) {
        growing = NO;
    }
}

- (void) tick
{
    if (self.generationTickCount == self.ticksPerGeneration && self.currentGeneration < self.maxGeneration) {
        [self advanceGeneration];
    }
    self.generationTickCount++;
    float generationPercent = self.generationTickCount / (float)self.ticksPerGeneration;
    for (WONode* node in self.nodes) {
        node.growthPercent = generationPercent;
    }

    if (growing) {
        path = CGPathCreateMutable();
        CGPathMoveToPoint(path, NULL, 0, 0);
        WOLSystemTransformStack* stack = [[[WOLSystemTransformStack alloc] init] autorelease];
        for (WONode* node in self.nodes) {
            [node renderWithStack:stack inLSystem:self];
        }
        self.layer.path = path;
        
        CGRect b = CGPathGetBoundingBox(path);
        self.layer.bounds = CGRectMake(-b.size.height / 2.0, -b.size.width / 2.0, b.size.height, b.size.width);

        [self.instrument updateParamsWithBoundingBox:b];
    }
}

- (void) tickAudio:(stk::StkFrames*)frames;
{
    [self.instrument tickAudio:frames];
}

- (void) setAge:(float)age
{
    int generation = 1;
    while (generation++ <= age) {
        [self advanceGeneration];
    }
    
    float percent = age - floor(age);
    self.generationTickCount = percent * self.ticksPerGeneration;
}
@end

@implementation WONode
@synthesize offset, randomOffset;

- (id) init
{
    self = [super init];
    if (self) {
        self.growthPercent = 0;
        offset = 0;
        randomOffset = rand() / (float)RAND_MAX;
    }
    return self;
}

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* newNodes = [NSMutableArray array];
    WONode* newNode = [[[[self class] alloc] init] autorelease];
    newNode.growthPercent = self.growthPercent;
    newNode.offset = offset;
    newNode.randomOffset = randomOffset;
    [newNodes addObject:newNode];
    return newNodes;
}

- (void) setNewGrowthPercent:(float)percent
{
    if (self.growthPercent < 1.0) {
        growthPercent = percent;
    }
    if (self.growthPercent > 1.0) {
        growthPercent = 1.0;
    }
}

- (float) getGrowthPercent
{
    return growthPercent;
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{}

@end


@implementation WOANode

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* nodes = [NSMutableArray array];

    [nodes addObject:[[[WOLittleFNode alloc] initWithBaseLength:50 andMaxOffset:20] autorelease]];

    [nodes addObject:[[[WOLeftBracketNode alloc] init] autorelease]];
    [nodes addObject:[[[WOAngleNode alloc] initWithBaseAngle:15 andMaxOffset:10] autorelease]];
    [nodes addObject:[[[WOAngleNode alloc] initWithBaseAngle:15 andMaxOffset:10] autorelease]];
    [nodes addObject:[[[WOANode alloc] init] autorelease]];
    [nodes addObject:[[WOLNode alloc] init]];
    [nodes addObject:[[[WORightBracketNode alloc] init] autorelease]];
    
    [nodes addObject:[[[WOLeftBracketNode alloc] init] autorelease]];
    [nodes addObject:[[[WOAngleNode alloc] initWithBaseAngle:-15 andMaxOffset:-10] autorelease]];
    [nodes addObject:[[[WOAngleNode alloc] initWithBaseAngle:-15 andMaxOffset:-10] autorelease]];
    [nodes addObject:[[[WOANode alloc] init] autorelease]];
    [nodes addObject:[[WOLNode alloc] init]];
    [nodes addObject:[[[WORightBracketNode alloc] init] autorelease]];

    [nodes addObject:[[[WOANode alloc] init] autorelease]];

    return nodes;
}

@end

@implementation WOLittleFNode
@synthesize pointId;

- (id) initWithBaseLength:(float)newBaseLength andMaxOffset:(float)newMaxOffset
{
    self = [super init];
    if (self) {
        
        baseLength = newBaseLength;
        maxOffset = newMaxOffset;
        
        pointId = rand();
    }
    return self;
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    float length = (baseLength + maxOffset * randomOffset) * self.growthPercent;
    [stack translateBy:length];
    CGPoint newPoint = [stack getPoint];
    CGPathAddLineToPoint(system.path, NULL, newPoint.x, newPoint.y);
    
    [system.instrument setGrainWithId:self.pointId ParamsX:newPoint.x Y:newPoint.y];
}

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* newNodes = [NSMutableArray array];
    WOLittleFNode* newNode = [[[WOLittleFNode alloc] initWithBaseLength:baseLength andMaxOffset:maxOffset] autorelease];
    newNode.growthPercent = self.growthPercent;
    newNode.offset = offset;
    newNode.randomOffset = randomOffset;
    newNode.pointId = pointId;
    [newNodes addObject:newNode];
    return newNodes;
}

@end

@implementation WOLNode

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    float radius = 2;
    CGPoint newPoint = [stack getPoint];
    CGPathAddArc(system.path, NULL, newPoint.x, newPoint.y, radius, 0, 2 * M_PI, 0);
}

@end

@implementation WOWiggleNode

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* nodes = [NSMutableArray array];
    
    WOAngleNode* a = [[[WOAngleNode alloc] initWithBaseAngle:-10 andMaxOffset:20] autorelease];
    WOLittleFNode* f = [[[WOLittleFNode alloc] initWithBaseLength:10 andMaxOffset:5] autorelease];
    WOWiggleNode* w = [[[WOWiggleNode alloc] init] autorelease];
    
    [nodes addObject:a];
    [nodes addObject:f];
    [nodes addObject:w];
    
    return nodes;
}

@end

@implementation WOAngleNode

- (id) initWithBaseAngle:(float)newBaseAngle andMaxOffset:(float)newMaxOffset
{
    self = [super init];
    if (self) {
        
        baseAngle = newBaseAngle;
        maxOffset = newMaxOffset;
    }
    return self;
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    float angle = baseAngle + maxOffset * randomOffset;
    [stack rotateBy:angle];
}

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* newNodes = [NSMutableArray array];
    WOAngleNode* newNode = [[[WOAngleNode alloc] initWithBaseAngle:baseAngle andMaxOffset:maxOffset] autorelease];
    newNode.growthPercent = self.growthPercent;
    newNode.offset = offset;
    newNode.randomOffset = randomOffset;
    [newNodes addObject:newNode];
    return newNodes;
}

@end

@implementation WOLeftBracketNode

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    [stack push];
}

@end

@implementation WORightBracketNode

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    [stack pop];
    CGPoint newPoint = [stack getPoint];
    CGPathMoveToPoint(system.path, NULL, newPoint.x, newPoint.y);
}

@end
