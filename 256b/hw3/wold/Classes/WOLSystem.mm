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

#define SRATE 44100

float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

@implementation WOLSystemRenderState
//@synthesize widthStack;

- (id) init
{
    self = [super init];
    if (self) {
        widthStack = new GLfloat[1000];
        stackIndex = 0;
        widthStack[stackIndex] = 0.1;
    }
    return self;
}

- (float) oldWidth
{
    return widthStack[stackIndex];
}

- (float) newWidth
{
    widthStack[stackIndex] *= 0.65;
    return widthStack[stackIndex];
}

- (void) push
{
    widthStack[stackIndex + 1] = widthStack[stackIndex];
    stackIndex++;
}

- (void) pop
{
    stackIndex--;
}

- (void) dealloc
{
    delete widthStack;
    
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
@synthesize env;

- (id) initWithMaxGeneration:(int)maxGen atPoint:(Vector3D)pos
{
    self = [super init];
    if (self) {
        self.nodes = [[NSMutableArray alloc] init];
        self.maxGeneration = maxGen;
        self.currentGeneration = 0;
        self.generationTickCount = 0;
        self.ticksPerGeneration = 30;

        self.origin = pos;
        self.offset = CGPointMake(0, 0);
        
        phase = 0.0;
        env = 0.999999;
        //freq = 200 + (rand() / (float)RAND_MAX) * 200.0;
        freq = 1000 + (rand() / (float)RAND_MAX) * 400.0;
        freq_offset = 0.0;
        
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
        //freq *= 1.5;
        freq *= 0.666666;
        env = 0.99999;
    }
    self.generationTickCount++;
    float generationPercent = self.generationTickCount / (float)self.ticksPerGeneration;
    for (WONode* node in self.nodes) {
        node.growthPercent = generationPercent;
        node.offset = self.offset;
    }
}

- (void) render
{
    WOLSystemRenderState* state = [[[WOLSystemRenderState alloc] init] autorelease];
    
    for (WONode* node in self.nodes) {
        [node renderWithState:state];
    }
}

- (float) tickAudio
{  
    env *= 0.99995;
    phase += (freq + freq_offset)/(SRATE*1.0);
    return sin(2.0*M_PI*phase) * env;
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
    
    return nodes;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    float length = 0.1 * self.growthPercent;
    [WOGeometry drawFrustumWithBottomRadius:[state oldWidth] andTopRadius:[state newWidth] andHeight:length andSections:5];
    glTranslatef(0.0f, length, 0.0f);
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

@end

@implementation WOLNode

- (id) init
{
    self = [super init];
    if (self) {
        self.symbol = @"l";
        phase = 0;
        freq = 0.5 * rand() / (float)RAND_MAX;
    }
    return self;
}

- (void) renderWithState:(WOLSystemRenderState*)state
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, green);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
    
    GLfloat radius = 0.1;

    //glPushMatrix();
    glTranslatef(0.0f, radius, 0.0f);
    float flutter_offset = 10 * sin(2.0*M_PI*phase);
    phase += freq * 0.05;
    glRotatef(90 + flutter_offset, 1.0f, 0.0f, 0.0f);
    //glScalef(radius, 1.0, radius);
    //[WOGeometry drawDiskWithRadius:radius andSections:5];
    //glPopMatrix();
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

@end
