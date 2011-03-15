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
        WOLSystemTransformState* firstState = [[WOLSystemTransformState alloc] init];
        firstState.width = 0;
        firstState.angle = 0;
        firstState.x = 0;
        firstState.y = 0;
        [stack addObject:firstState];
    }
    return self;
}

- (id) initWithPoint:(CGPoint)pos andAngle:(float)angle
{
    self = [super init];
    if (self) {
        stack = [[NSMutableArray alloc] init];
        WOLSystemTransformState* firstState = [[WOLSystemTransformState alloc] init];
        firstState.width = 0;
        firstState.angle = angle;
        firstState.x = pos.x;
        firstState.y = pos.y;
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
    currentState.x = currentState.x + length * cos(currentState.angle);
    currentState.y = currentState.y - length * sin(currentState.angle); // Growing upwards towards negative Y
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

- (id) initWithMaxGeneration:(int)maxGen
{
    self = [super init];
    if (self) {
        self.nodes = [[NSMutableArray alloc] init];
        self.maxGeneration = maxGen;
        self.currentGeneration = 0;
        self.generationTickCount = 0;
        self.ticksPerGeneration = 30;
        
        self.growing = YES;
        
        path = CGPathCreateMutable();
        layer = [CAShapeLayer layer];
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
}

// TODO: Cache the everloving shit out of these points
- (void) renderAtPoint:(CGPoint)pos withAngle:(float)angle
{
    if (growing) {
        path = CGPathCreateMutable();
        CGPathMoveToPoint(path, NULL, pos.x, pos.y);
        WOLSystemTransformStack* stack = [[[WOLSystemTransformStack alloc] initWithPoint:pos andAngle:angle] autorelease];
        for (WONode* node in self.nodes) {
            [node renderWithStack:stack inLSystem:self];
        }
    }
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextAddPath(context, path);

}

- (void) setAge:(float)age
{
    int generation = 1;
    while (generation++ <= age) {
//        for (WOLNode* node in self.nodes) {
//            node.growthPercent = 1.0;
//        }
        [self advanceGeneration];
    }
    
    float percent = age - floor(age);
    self.generationTickCount = percent * self.ticksPerGeneration;
//    for (WOLNode* node in self.nodes) {
//        if (node.growthPercent != 1.0) {
//            node.growthPercent = percent;
//        }
//    }
}
@end

@implementation WONode

@synthesize symbol;
@synthesize growthPercent;

- (id) init
{
    self = [super init];
    if (self) {
        self.growthPercent = 0;
    }
    return self;
}

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* newNodes = [NSMutableArray array];
    WONode* newNode = [[[self class] alloc] init];
    newNode.growthPercent = self.growthPercent;
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

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{}

@end


@implementation WOANode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"A";
    }
    return self;
}


// TODO: Make LNodes (leaves) work again, with just little circles.
- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* nodes = [NSMutableArray array];

    [nodes addObject:[[WOLittleFNode alloc] init]];

    [nodes addObject:[[WOLeftBracketNode alloc] init]];
    [nodes addObject:[[WOPlusNode alloc] init]];
    [nodes addObject:[[WOPlusNode alloc] init]];
    [nodes addObject:[[WOANode alloc] init]];
    //[nodes addObject:[[WOLNode alloc] init]];
    [nodes addObject:[[WORightBracketNode alloc] init]];
    
    [nodes addObject:[[WOLeftBracketNode alloc] init]];
    [nodes addObject:[[WOMinusNode alloc] init]];
    [nodes addObject:[[WOMinusNode alloc] init]];
    [nodes addObject:[[WOANode alloc] init]];
    //[nodes addObject:[[WOLNode alloc] init]];
    [nodes addObject:[[WORightBracketNode alloc] init]];

    [nodes addObject:[[WOANode alloc] init]];

    return nodes;
}

@end

@implementation WOLittleFNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"f";
    }
    return self;
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    float length = 30 * self.growthPercent;
    [stack translateBy:length];
    CGPoint newPoint = [stack getPoint];
    CGPathAddLineToPoint(system.path, NULL, newPoint.x, newPoint.y);
}

@end

//@implementation WOLNode
//
//- (id) init
//{
//    self = [super init];
//    if (self) {
//        self.symbol = @"l";
//        phase = 0;
//        freq = 0.5 * rand() / (float)RAND_MAX;
//    }
//    return self;
//}
//
//- (void) renderWithState:(WOLSystemRenderState*)state
//{
//    float radius = 0.1;
//
//    glTranslatef(0.0f, radius, 0.0f);
//    mglTranslate(0.0f, radius, 0.0f);
//    float flutter_offset = 10 * sin(2.0*M_PI*phase);
//    phase += freq * 0.05;
//    glRotatef(90 + flutter_offset, 1.0f, 0.0f, 0.0f);
//    mglRotate(90 + flutter_offset, 1.0f, 0.0f, 0.0f);
//    //[WOGeometry drawDiskWithRadius:radius andSections:5];
//}
//
//@end


@implementation WOPlusNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"+";
    }
    return self;
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    float angle = 25;
    [stack rotateBy:angle];
}

@end

@implementation WOMinusNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"-";
    }
    return self;    
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    float angle = -25;
    [stack rotateBy:angle];
}

@end

@implementation WOLeftBracketNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"[";
    }
    return self;
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    [stack push];
}

@end

@implementation WORightBracketNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"]";
    }
    return self;
    
}

- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system
{
    [stack pop];
    CGPoint newPoint = [stack getPoint];
    CGPathMoveToPoint(system.path, NULL, newPoint.x, newPoint.y);
}

@end
