//
//  WOLSystem.h
//  wold
//
//  Created by Michael Rotondo on 2/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

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

- (id) initWithPoint:(CGPoint)pos andAngle:(float)angle;
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
}

@property (nonatomic, retain) NSMutableArray* nodes;

@property int maxGeneration;
@property int currentGeneration;
@property int ticksPerGeneration;
@property int generationTickCount;
@property BOOL growing;

@property CGMutablePathRef path;
@property (nonatomic, retain) CAShapeLayer* layer;

- (id) initWithMaxGeneration:(int)maxGen andAngle:(float)newAngle andOrigin:(CGPoint)origin;
- (void) advanceGeneration;
- (void) tick;
- (void) renderAtPoint:(CGPoint)pos withAngle:(float)angle;
- (void) setAge:(float)age;

@end

@interface WONode : NSObject {

    NSString* symbol; // For debugging
    
    float growthPercent;
}

@property (nonatomic, retain) NSString* symbol;
@property (setter=setNewGrowthPercent:) float growthPercent;

- (NSMutableArray*) expandInLSystem:(WOLSystem*)lSystem isLastGeneration:(BOOL)lastGeneration;
- (void) renderWithStack:(WOLSystemTransformStack*)stack inLSystem:(WOLSystem*)system;
- (void) setNewGrowthPercent:(float)percent;

@end

@interface WOANode : WONode {}
@end

@interface WOLittleFNode : WONode {}
@end

//@interface WOLNode : WONode {
//    float freq, phase;
//}
//@end

@interface WOPlusNode : WONode {}
@end

@interface WOMinusNode : WONode {}
@end

@interface WOLeftBracketNode : WONode {}
@end

@interface WORightBracketNode : WONode {}
@end