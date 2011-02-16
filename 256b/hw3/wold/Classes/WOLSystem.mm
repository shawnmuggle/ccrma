//
//  WOLSystem.m
//  wold
//
//  Created by Michael Rotondo on 2/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOLSystem.h"

#import "ES1Renderer.h"

#import "WOGeometry.h"

#import <cstdlib>
#import <ctime>


float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

@implementation WOLSystemRenderState
@synthesize widthStack;

- (id) init
{
    self = [super init];
    if (self) {
        self.widthStack = [NSMutableArray array];
        [self.widthStack addObject:[NSNumber numberWithFloat:0.1]];
    }
    return self;
}

- (float) oldWidth
{
    NSNumber* top = [self.widthStack lastObject];
    return [top floatValue];
}

- (float) newWidth
{
    NSNumber* top = [self.widthStack lastObject];
    [self.widthStack removeLastObject];
    GLfloat newWidth = [top floatValue] * 0.65;
    [self.widthStack addObject:[NSNumber numberWithFloat:newWidth]];
    return newWidth;

}

- (void) push
{
    NSNumber* top = [self.widthStack lastObject];
    [self.widthStack addObject:[NSNumber numberWithFloat:[top floatValue]]];
}

- (void) pop
{
    [self.widthStack removeLastObject];
}

- (void) dealloc
{
    [widthStack release];
    
    [super dealloc];
}

@end



@implementation WOLSystem

@synthesize nodes;
@synthesize maxGeneration;
@synthesize currentGeneration;
@synthesize ticksPerGeneration;
@synthesize generationTickCount;
@synthesize origin;
@synthesize offset;
@synthesize numPoints;

- (id) initWithMaxGeneration:(int)maxGen atPoint:(Vector3D)pos
{
    self = [super init];
    if (self) {
        self.nodes = [[NSMutableArray alloc] init];
        self.maxGeneration = maxGen;
        self.currentGeneration = 0;
        self.generationTickCount = 0;
        self.ticksPerGeneration = 30;

        ticksReceived = 0;
        advanceInterval = 2;
        lineVertices = NULL;
        
        self.origin = pos;
        self.offset = CGPointMake(0, 0);
        
        self.numPoints = 1;

        square = new stk::BlitSquare();
        square->setFrequency(80 + 300 * -origin.y / 1024.0);
        square->setHarmonics(0);
        
        srand((unsigned)time(0));
    }
    return self;
}

- (void) advanceGeneration
{
    NSMutableArray* newNodes = [[NSMutableArray alloc] init];
    for (WONode* node in self.nodes) {
        [newNodes addObjectsFromArray:[node expandInLSystem:self isLastGeneration:self.currentGeneration == self.maxGeneration]];
    }

    self.nodes = newNodes;
}

- (void) tick
{
    if (self.generationTickCount == self.ticksPerGeneration && self.currentGeneration < self.maxGeneration) {
        self.currentGeneration++;
        self.generationTickCount = 0;
        [self advanceGeneration];
        square->setHarmonics(self.currentGeneration);
    }
    self.generationTickCount++;
    float generationPercent = self.generationTickCount / (float)self.ticksPerGeneration;
    for (WONode* node in self.nodes) {
        node.growthPercent = generationPercent;
        node.offset = self.offset;
    }
    
//    if (ticksReceived++ % advanceInterval == 0) {
//        [self generateNewVertices];
//    }
}

- (void) generateNewVertices
{
    delete lineVertices;
    
    lineVertices = new GLfloat[(numPoints - 1) * 2 * 3]; // 2 points for every line segment if using GL_LINES, and 3 values per 3D point

    WOTurtle* turtle = [[WOTurtle alloc] initWithPoints:lineVertices];
    Vector3D position;
    [turtle addPoint:position];
    
    for (WONode* node in nodes) {
        [node modifyTurtle:turtle];
    }
}

- (void) render
{
//    glPushMatrix();
//    glVertexPointer(3, GL_FLOAT, 0, lineVertices);
//    glDrawArrays(GL_LINES, 0, numPoints);
//    glPopMatrix();

    WOLSystemRenderState* state = [[[WOLSystemRenderState alloc] init] autorelease];
    
    for (WONode* node in self.nodes) {
        [node renderWithState:state];
    }
}

- (float) tickAudio
{
    float generationPercent = self.generationTickCount / (float)self.ticksPerGeneration;
    return 0.1 * ((self.currentGeneration + generationPercent) / self.maxGeneration) * square->tick();
}

@end

@implementation WONode

@synthesize symbol;
@synthesize growthPercent;
@synthesize offset, randomOffset;

- (id) init
{
    self = [super init];
    if (self) {
        self.growthPercent = 0;
        self.offset = CGPointMake(0, 0);
        self.randomOffset = 1 - 2 * (rand() / (float)RAND_MAX);
    }
    return self;
}

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* newNodes = [NSMutableArray array];
    WONode* newNode = [[[self class] alloc] init];
    newNode.growthPercent = self.growthPercent;
    newNode.randomOffset = self.randomOffset;
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

- (void) renderWithState:(WOLSystemRenderState*)state
{}

- (void) modifyTurtle:(WOTurtle *)turtle
{}

@end

@implementation WOXNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"X";
    }
    return self;
}

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* nodes = [NSMutableArray array];
    [nodes addObject:[[WOFNode alloc] init]];
    [nodes addObject:[[WOMinusNode alloc] init]];
    
    if (!lastGeneration) {
        [nodes addObject:[[WOLeftBracketNode alloc] init]];
        [nodes addObject:[[WOLeftBracketNode alloc] init]];
        [nodes addObject:[[WOXNode alloc] init]];
        [nodes addObject:[[WORightBracketNode alloc] init]];
        [nodes addObject:[[WOPlusNode alloc] init]];
        [nodes addObject:[[WOXNode alloc] init]];
        [nodes addObject:[[WORightBracketNode alloc] init]];
    }
    
    [nodes addObject:[[WOPlusNode alloc] init]];
    [nodes addObject:[[WOFNode alloc] init]];
    [nodes addObject:[[WOLeftBracketNode alloc] init]];
    [nodes addObject:[[WOPlusNode alloc] init]];
    [nodes addObject:[[WOFNode alloc] init]];
    [nodes addObject:[[WOXNode alloc] init]];
    [nodes addObject:[[WORightBracketNode alloc] init]];
    [nodes addObject:[[WOMinusNode alloc] init]];
    [nodes addObject:[[WOXNode alloc] init]];
    
    lSystem.numPoints += 3;
    
    return nodes;
}

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

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* nodes = [NSMutableArray array];

    [nodes addObject:[[WOLittleFNode alloc] init]];

    [nodes addObject:[[WOLeftBracketNode alloc] init]];
    [nodes addObject:[[WOPlusNode alloc] init]];
    [nodes addObject:[[WOPlusNode alloc] init]];
    [nodes addObject:[[WOANode alloc] init]];
    [nodes addObject:[[WOLNode alloc] init]];
    [nodes addObject:[[WORightBracketNode alloc] init]];
    
    [nodes addObject:[[WOLeftBracketNode alloc] init]];
    [nodes addObject:[[WOMinusNode alloc] init]];
    [nodes addObject:[[WOMinusNode alloc] init]];
    [nodes addObject:[[WOANode alloc] init]];
    [nodes addObject:[[WOLNode alloc] init]];
    [nodes addObject:[[WORightBracketNode alloc] init]];
    
    [nodes addObject:[[WOLeftBracketNode alloc] init]];
    [nodes addObject:[[WOGTNode alloc] init]];
    [nodes addObject:[[WOGTNode alloc] init]];
    [nodes addObject:[[WOGTNode alloc] init]];
    [nodes addObject:[[WOANode alloc] init]];
    [nodes addObject:[[WORightBracketNode alloc] init]];
    
    [nodes addObject:[[WOLeftBracketNode alloc] init]];
    [nodes addObject:[[WOLTNode alloc] init]];
    [nodes addObject:[[WOLTNode alloc] init]];
    [nodes addObject:[[WOLTNode alloc] init]];
    [nodes addObject:[[WOANode alloc] init]];
    [nodes addObject:[[WORightBracketNode alloc] init]];
    
    lSystem.numPoints += 1;
    
    return nodes;
}

@end


@implementation WOFNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"F";
    }
    return self;
}

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* nodes = [NSMutableArray array];
    WOFNode* fNode = [[WOFNode alloc] init];
    fNode.growthPercent = 1.0;
    [nodes addObject:fNode];
    [nodes addObject:[[WOFNode alloc] init]];
    
    lSystem.numPoints += 2;
    
    return nodes;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    float length = 0.1 * self.growthPercent;
    [WOGeometry drawFrustumWithBottomRadius:[state oldWidth] andTopRadius:[state newWidth] andHeight:length andSections:5];
    glTranslatef(0.0f, length, 0.0f);
}

- (void)modifyTurtle:(WOTurtle *)turtle
{
    // TODO: Implement x angles as well.
    
    float length = 0.1 * self.growthPercent;
    
    TurtleState* state = [turtle currentState];
    Vector3D newPosition;
    newPosition.x = state->x + cos((M_PI / 180) * state->zAngle) * length;
    newPosition.y = state->y + sin((M_PI / 180) * state->zAngle) * length;
    newPosition.z = state->z + sin((M_PI / 180) * state->yAngle) * length;
    [turtle addPoint:newPosition];
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

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration
{
    NSMutableArray* newNodes = [super expandInLSystem:lSystem isLastGeneration:lastGeneration];
    lSystem.numPoints += 1;
    return newNodes;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
    
    GLfloat length = 0.3 * self.growthPercent;
    [WOGeometry drawFrustumWithBottomRadius:[state oldWidth] andTopRadius:[state newWidth] andHeight:length andSections:5];
    glTranslatef(0.0f, length, 0.0f);
}

- (void)modifyTurtle:(WOTurtle *)turtle
{
    // TODO: Implement x angles as well.
    
    float length = 0.1 * self.growthPercent;
    
    TurtleState* state = [turtle currentState];
    Vector3D newPosition;
    newPosition.x = state->x + cos((M_PI / 180) * state->zAngle) * length;
    newPosition.y = state->y + sin((M_PI / 180) * state->zAngle) * length;
    newPosition.z = state->z + sin((M_PI / 180) * state->yAngle) * length;
    [turtle addPoint:newPosition];
}

@end

@implementation WOLNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"l";
    }
    return self;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, green);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
    
    GLfloat radius = 0.1;
    glTranslatef(0.0f, radius, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    [WOGeometry drawFrustumWithBottomRadius:radius andTopRadius:radius andHeight:0.0001 andSections:5];
}

@end


@implementation WOPlusNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"+";
    }
    return self;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    GLfloat angle = 25 + 10 * self.randomOffset;
    glRotatef(angle * self.growthPercent, 1.0f, 0.0f, 0.0f);
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    float angleOffset = 0;
    if (fabs(self.offset.x) < 50) {
        angleOffset = -self.offset.y / 40.0;
    } else {
        angleOffset = self.offset.x / 50.0;
    }
    
    [turtle addZAngle:self.growthPercent * (25 + angleOffset)];
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

- (void) renderWithState:(WOLSystemRenderState*)state
{
    GLfloat angle = -25 + 10 * self.randomOffset;
    glRotatef(angle * self.growthPercent, 1.0f, 0.0f, 0.0f);
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    float angleOffset = 0;
    if (fabs(self.offset.x) < 50) {
        angleOffset = self.offset.y / 40.0;
    } else {
        angleOffset = self.offset.x / 50.0;
    }
    
    [turtle addZAngle:self.growthPercent * (-25 + angleOffset)];
}

@end

@implementation WOGTNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @">";
    }
    return self;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    GLfloat angle = 25 + 10 * self.randomOffset;
    glRotatef(angle * self.growthPercent, 0.0f, 0.0f, 1.0f);
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    [turtle addYAngle:self.growthPercent * 25];
}

@end

@implementation WOLTNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"<";
    }
    return self;    
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    GLfloat angle = -25 + 10 * self.randomOffset;
    glRotatef(angle * self.growthPercent, 0.0f, 0.0f, 1.0f);
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    [turtle addYAngle:self.growthPercent * -25];
}

@end

@implementation WOUpNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"^";
    }
    return self;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    GLfloat angle = 25 + 10 * self.randomOffset;
    glRotatef(angle * self.growthPercent, 0.0f, 1.0f, 0.0f);
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    [turtle addYAngle:self.growthPercent * 25];
}

@end

@implementation WODownNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"v";
    }
    return self;    
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    GLfloat angle = -25 + 10 * self.randomOffset;
    glRotatef(angle * self.growthPercent, 0.0f, 1.0f, 0.0f);
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    [turtle addYAngle:self.growthPercent * -25];
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

- (void) renderWithState:(WOLSystemRenderState*)state
{
    [state push];
    glPushMatrix();
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    [turtle pushState];
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

- (void) renderWithState:(WOLSystemRenderState*)state
{
    [state pop];
    glPopMatrix();
}

- (void) modifyTurtle:(WOTurtle *)turtle
{
    [turtle popState];
}

@end
