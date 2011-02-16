//
//  WOLSystem.h
//  wold
//
//  Created by Michael Rotondo on 2/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BlitSquare.h"
#import "BiQuad.h"

#import "WOTurtle.h"

@interface WOLSystemRenderState : NSObject {
    NSMutableArray* widthStack;
}

@property (nonatomic, retain) NSMutableArray* widthStack;

- (float) oldWidth;
- (float) newWidth;
- (void) push;
- (void) pop;

@end


@interface WOLSystem : NSObject {

    NSMutableArray* nodes;
    
    int maxGeneration;
    int currentGeneration;
    int ticksPerGeneration;
    int generationTickCount;
    
    int ticksReceived;
    int advanceInterval;
    GLfloat* lineVertices;
    
    Vector3D origin;
    CGPoint offset;
    
    int numPoints;
    
    float red, green, blue;
    float leaf_red, leaf_green, leaf_blue;
    
    stk::BlitSquare* square;
    
}

@property (nonatomic, retain) NSMutableArray* nodes;

@property int maxGeneration;
@property int currentGeneration;
@property int ticksPerGeneration;
@property int generationTickCount;
@property (nonatomic) Vector3D origin;
@property (nonatomic) CGPoint offset;
@property int numPoints;

- (id) initWithMaxGeneration:(int)maxGen atPoint:(Vector3D)pos;
- (void) advanceGeneration;
- (void) generateNewVertices;
- (void) tick;
- (void) render;
- (float) tickAudio;

@end

@interface WONode : NSObject {

    NSString* symbol; // For debugging
    
    float growthPercent;
    CGPoint offset;
    float randomOffset;

}

@property (nonatomic, retain) NSString* symbol;
@property (setter=setNewGrowthPercent:) float growthPercent;
@property CGPoint offset;
@property float randomOffset;

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration;
- (void) renderWithState:(WOLSystemRenderState*)state;
- (void)modifyTurtle:(WOTurtle*)turtle;
- (void) setNewGrowthPercent:(float)percent;

@end

// Fractal Plant from wikipedia: (http://en.wikipedia.org/wiki/L-system)
@interface WOXNode : WONode {}
@end

@interface WOANode : WONode {}
@end

@interface WOFNode : WONode {}
@end

@interface WOLittleFNode : WONode {}
@end

@interface WOLNode : WONode {}
@end

@interface WOPlusNode : WONode {}
@end

@interface WOMinusNode : WONode {}
@end

@interface WOGTNode : WONode {}
@end

@interface WOLTNode : WONode {}
@end

@interface WOUpNode : WONode {}
@end

@interface WODownNode : WONode {}
@end

@interface WOLeftBracketNode : WONode {}
@end

@interface WORightBracketNode : WONode {}
@end