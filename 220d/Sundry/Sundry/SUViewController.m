//
//  SUViewController.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUViewController.h"
#import "SUSpace.h"
#import "SUWorld.h"
#import "SUWorldParts.h"
#import "SUSynths.h"
#import "SUAudioManager.h"
#import "SUTimeline.h"

static float TWO_PI = 2 * M_PI;

@interface SUViewController () {
    GLKMatrix4 projectionMatrix;
    
    UITouch *movementTouch;
    GLKVector2 movementTouchStartPoint;
    GLKVector2 movementTouchOffset;

    BOOL movingForward;
    float movementRate;
    float maxMovementRate;
    float turnRate;
    float maxTurnRate;
    
    BOOL editMode;
    SUWorld *editingWorld;
    SUWorldCube *editingWorldCube;
    NSArray *editingWorldSprings;
    float loopTime;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
- (void)createEditingCube;

@end

@implementation SUViewController
@synthesize context = _context;
@synthesize space;
@synthesize player;
@synthesize cubeEditingView;
@synthesize percsEditingView;

@synthesize scaleSlider;
@synthesize freqSlider;
@synthesize axisXSlider;
@synthesize axisYSlider;
@synthesize axisZSlider;
@synthesize posXSlider;
@synthesize posYSlider;
@synthesize posZSlider;
@synthesize colRSlider;
@synthesize colGSlider;
@synthesize colBSlider;
@synthesize perc0StartFreqSlider;
@synthesize perc0EndFreqSlider;
@synthesize perc0LengthSlider;
@synthesize perc1StartFreqSlider;
@synthesize perc1EndFreqSlider;
@synthesize perc1LengthSlider;
@synthesize perc2StartFreqSlider;
@synthesize perc2EndFreqSlider;
@synthesize perc2LengthSlider;
@synthesize perc3StartFreqSlider;
@synthesize perc3EndFreqSlider;
@synthesize perc3LengthSlider;
@synthesize perc4StartFreqSlider;
@synthesize perc4EndFreqSlider;
@synthesize perc4LengthSlider;
@synthesize perc5StartFreqSlider;
@synthesize perc5EndFreqSlider;
@synthesize perc5LengthSlider;
@synthesize percLoopProgressView;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (editMode)
    {
        
    }
    else
    {
        if (movementTouch == nil)
        {
            movementTouch = [touches anyObject];
            
            movingForward = YES;
            CGPoint touchPoint = [movementTouch locationInView:movementTouch.view];
            movementTouchStartPoint = GLKVector2Make(touchPoint.x, touchPoint.y);
            movementTouchOffset = GLKVector2Make(0.0f, 0.0f);
        }
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (editMode)
    {
        
    }
    else
    {
        if ([touches containsObject:movementTouch])
        {
            CGPoint touchPoint = [movementTouch locationInView:movementTouch.view];
            GLKVector2 movementTouchPoint = GLKVector2Make(touchPoint.x, touchPoint.y);
            movementTouchOffset = GLKVector2Subtract(movementTouchPoint, movementTouchStartPoint);
        }    
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (editMode)
    {
        
    }
    else
    {
        if ([touches containsObject:movementTouch])
        {
            movementTouch = nil;
            
            movingForward = NO;
        }
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [[UINib nibWithNibName:@"SUCubeEditingView" bundle:nil] instantiateWithOwner:self options:nil];
    [[UINib nibWithNibName:@"SUPercsEditingView" bundle:nil] instantiateWithOwner:self options:nil];
        
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self setupGL];

    self.space = [[SUSpace alloc] init];
    
    self.player = [[SUPlayer alloc] initWithPosition:GLKVector3Make(0.0f, 0.0f, -50.0f) orientation:GLKQuaternionIdentity ];
    maxMovementRate = 6.0f;
    maxTurnRate = 0.0003f;
    
    [SUAudioManager initAudioWithPlayer:self.player space:self.space]; // Get this audio party started
}

- (void)viewDidUnload
{    
    [super viewDidUnload];
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
	self.context = nil;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    glEnable(GL_DEPTH_TEST);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.1f, 10000.0f);
    
    if (movingForward)
    {
        movementRate = MIN(maxMovementRate, movementRate + 0.05f);
        turnRate = MIN(maxTurnRate, turnRate + 0.00005f);
    }
    else
    {
        if (editMode)
        {
            movementRate *= 0.94f;
        }
        else
        {
            movementRate = MAX(0, movementRate - 0.3f);
            turnRate = MAX(0, turnRate - 0.00003f);
        }
    }
    GLKVector3 xAxis = GLKVector3Make(1.0f, 0.0f, 0.0f);
    GLKVector3 rotatedXAxis = GLKQuaternionRotateVector3(self.player.orientation, xAxis);        
    GLKQuaternion xRotation = GLKQuaternionMakeWithAngleAndVector3Axis(-movementTouchOffset.y * turnRate, rotatedXAxis);
    self.player.orientation = GLKQuaternionMultiply(xRotation, self.player.orientation);
    
    GLKVector3 yAxis = GLKVector3Make(0.0f, 1.0f, 0.0f);
    GLKVector3 rotatedYAxis = GLKQuaternionRotateVector3(self.player.orientation, yAxis);
    GLKQuaternion yRotation = GLKQuaternionMakeWithAngleAndVector3Axis(-movementTouchOffset.x * turnRate, rotatedYAxis);
    self.player.orientation = GLKQuaternionMultiply(yRotation, self.player.orientation);
    
    GLKVector3 movement = GLKVector3Make(0.0f, 0.0f, movementRate);
    movement = GLKQuaternionRotateVector3(self.player.orientation, movement);        
    self.player.position = GLKVector3Add(self.player.position, movement);
    
    SUWorldSeed *activeWorldSeed = [self.space checkWorldSeedsForCollisionWithPlayer:self.player];
    if (activeWorldSeed)
    {
        movingForward = NO;
        movementRate = -10.0f;
        turnRate = 0.0f;
        movementTouch = nil;
        
        editMode = YES;
        editingWorld = [[SUWorld alloc] initWithPosition:activeWorldSeed.position];
        [self.space removeWorldSeed:activeWorldSeed];
        [SUAudioManager sharedAudioManager].editMode = YES;
        [SUAudioManager sharedAudioManager].editingWorld = editingWorld;
        [self.space addWorld:editingWorld];
        [self createEditingCube];
    }
}

- (void)createEditingCube
{
    if (editMode && editingWorld)
    {
        editingWorldCube = [[SUWorldCube alloc] init];
        [editingWorld addCube:editingWorldCube];
        
        [self.view addSubview:self.cubeEditingView];
        
        self.scaleSlider.value = ((editingWorldCube.scale - editingWorldCube.minScale) / 
                                  (editingWorldCube.maxScale - editingWorldCube.minScale));
        self.freqSlider.value = ((editingWorldCube.angleIncrement - editingWorldCube.minAngleIncrement) / 
                                 (editingWorldCube.maxAngleIncrement - editingWorldCube.minAngleIncrement));
        self.axisXSlider.value = editingWorldCube.axis.x;
        self.axisYSlider.value = editingWorldCube.axis.y;
        self.axisZSlider.value = editingWorldCube.axis.z;
        self.posXSlider.value = ((editingWorldCube.relativePosition.x - editingWorldCube.minRelativePosition) / 
                                 (editingWorldCube.maxRelativePosition - editingWorldCube.minRelativePosition));
        self.posYSlider.value = ((editingWorldCube.relativePosition.y - editingWorldCube.minRelativePosition) / 
                                 (editingWorldCube.maxRelativePosition - editingWorldCube.minRelativePosition));
        self.posZSlider.value = ((editingWorldCube.relativePosition.z - editingWorldCube.minRelativePosition) / 
                                 (editingWorldCube.maxRelativePosition - editingWorldCube.minRelativePosition));
        self.colRSlider.value = editingWorldCube.color.x;
        self.colGSlider.value = editingWorldCube.color.y;
        self.colBSlider.value = editingWorldCube.color.x;
    }
}

- (IBAction)scaleChanged:(UISlider *)sender
{
    editingWorldCube.scale = editingWorldCube.minScale + (editingWorldCube.maxScale - editingWorldCube.minScale) * sender.value;
}
- (IBAction)freqChanged:(UISlider *)sender
{
    editingWorldCube.angleIncrement = editingWorldCube.minAngleIncrement + (editingWorldCube.maxAngleIncrement - editingWorldCube.minAngleIncrement) * sender.value;
}
- (IBAction)axisXChanged:(UISlider *)sender
{
    editingWorldCube.axis = GLKVector3Make(sender.value, editingWorldCube.axis.y, editingWorldCube.axis.z);
}
- (IBAction)axisYChanged:(UISlider *)sender
{
    editingWorldCube.axis = GLKVector3Make(editingWorldCube.axis.x, sender.value, editingWorldCube.axis.z);
}
- (IBAction)axisZChanged:(UISlider *)sender
{
    editingWorldCube.axis = GLKVector3Make(editingWorldCube.axis.x, editingWorldCube.axis.y, sender.value);
}
- (IBAction)posXChanged:(UISlider *)sender
{
    editingWorldCube.relativePosition = GLKVector3Make(editingWorldCube.minRelativePosition + (editingWorldCube.maxRelativePosition - editingWorldCube.minRelativePosition) * sender.value, editingWorldCube.relativePosition.y, editingWorldCube.relativePosition.z);
}
- (IBAction)posYChanged:(UISlider *)sender
{
    editingWorldCube.relativePosition = GLKVector3Make(editingWorldCube.relativePosition.x, editingWorldCube.minRelativePosition + (editingWorldCube.maxRelativePosition - editingWorldCube.minRelativePosition) * sender.value, editingWorldCube.relativePosition.z);    
}
- (IBAction)posZChanged:(UISlider *)sender
{
    editingWorldCube.relativePosition = GLKVector3Make(editingWorldCube.relativePosition.x, editingWorldCube.relativePosition.y, editingWorldCube.minRelativePosition + (editingWorldCube.maxRelativePosition - editingWorldCube.minRelativePosition) * sender.value);
}
- (IBAction)colRChanged:(UISlider *)sender
{
    editingWorldCube.color = GLKVector4Make(sender.value, editingWorldCube.color.y, editingWorldCube.color.z, 1.0f);
}
- (IBAction)colGChanged:(UISlider *)sender
{
    editingWorldCube.color = GLKVector4Make(editingWorldCube.color.x, sender.value, editingWorldCube.color.z, 1.0f);
}
- (IBAction)colBChanged:(UISlider *)sender
{
    editingWorldCube.color = GLKVector4Make(editingWorldCube.color.x, editingWorldCube.color.y, sender.value, 1.0f);    
}

- (IBAction)createAnotherCube:(id)sender
{
    [self createEditingCube];
}
- (IBAction)doneCreatingCubes:(id)sender
{
    [self.cubeEditingView removeFromSuperview];
    [self.view addSubview:self.percsEditingView];
    
    editingWorldSprings = [NSArray arrayWithObjects:
                           [[SUWorldSpringThing alloc] init],
                           [[SUWorldSpringThing alloc] init],
                           [[SUWorldSpringThing alloc] init],
                           [[SUWorldSpringThing alloc] init],
                           [[SUWorldSpringThing alloc] init],
                           [[SUWorldSpringThing alloc] init], nil];
    for (SUWorldSpringThing *springThing in editingWorldSprings)
    {
        [editingWorld addSpringThing:springThing];
    }
    
    SUPercSynth *perc;
    // NOTE: The ranges on these synth params shouldn't be magic numbers over here, but in fact they are provided by the springthing so there's a bit of funkiness already. Ignoring for now.
    perc = ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:0]).synth;
    self.perc0StartFreqSlider.value = perc.startFreq / (300.0f * TWO_PI);
    self.perc0EndFreqSlider.value = perc.endFreq / (300.0f * TWO_PI);
    self.perc0LengthSlider.value = perc.length / 3.0f;
    perc = ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:1]).synth;
    self.perc1StartFreqSlider.value = perc.startFreq / (300.0f * TWO_PI);
    self.perc1EndFreqSlider.value = perc.endFreq / (300.0f * TWO_PI);
    self.perc1LengthSlider.value = perc.length / 3.0f;
    perc = ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:2]).synth;
    self.perc2StartFreqSlider.value = perc.startFreq / (300.0f * TWO_PI);
    self.perc2EndFreqSlider.value = perc.endFreq / (300.0f * TWO_PI);
    self.perc2LengthSlider.value = perc.length / 3.0f;
    perc = ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:3]).synth;
    self.perc3StartFreqSlider.value = perc.startFreq / (300.0f * TWO_PI);
    self.perc3EndFreqSlider.value = perc.endFreq / (300.0f * TWO_PI);
    self.perc3LengthSlider.value = perc.length / 3.0f;
    perc = ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:4]).synth;
    self.perc4StartFreqSlider.value = perc.startFreq / (300.0f * TWO_PI);
    self.perc4EndFreqSlider.value = perc.endFreq / (300.0f * TWO_PI);
    self.perc4LengthSlider.value = perc.length / 3.0f;
    perc = ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:5]).synth;
    self.perc5StartFreqSlider.value = perc.startFreq / (300.0f * TWO_PI);
    self.perc5EndFreqSlider.value = perc.endFreq / (300.0f * TWO_PI);
    self.perc5LengthSlider.value = perc.length / 3.0f;
}

- (IBAction)startFreqChanged:(UISlider *)sender
{
    ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:sender.tag]).synth.startFreq = 300.0f * TWO_PI * sender.value;
}
- (IBAction)endFreqChanged:(UISlider *)sender
{
    ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:sender.tag]).synth.endFreq = 300.0f * TWO_PI * sender.value;
}
- (IBAction)lengthChanged:(UISlider *)sender
{
    ((SUWorldSpringThing *)[editingWorldSprings objectAtIndex:sender.tag]).synth.length = 3.0f * sender.value;
}
- (IBAction)percPadPressed:(UIButton *)sender
{
    SUWorldSpringThing *springThing = [editingWorldSprings objectAtIndex:sender.tag];
    [springThing springWithAmplitude:1.0f];
    [springThing.timeline addEvent:0.5f + 0.5f * (arc4random() / (float)0x100000000) atTime:loopTime];
    
}
- (IBAction)clearSprings:(UIButton *)sender
{
    for (SUWorldSpringThing *springThing in editingWorldSprings)
    {
        [springThing.timeline clear];
    }
}
- (IBAction)doneEditing:(UIButton *)sender
{
    [self.cubeEditingView removeFromSuperview];
    [self.percsEditingView removeFromSuperview];
    editMode = NO;
    [SUAudioManager sharedAudioManager].editMode = NO;
    [editingWorld makeTriangles];
    editingWorld = nil;
    editingWorldCube = nil;
    editingWorldSprings = nil;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClearColor(0.43f, 0.66f, 0.93f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GLKMatrix4 baseModelViewMatrix = GLKMatrix4Identity;
    baseModelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, 
                                             GLKMatrix4MakeWithQuaternion(GLKQuaternionInvert(self.player.orientation)));
    GLKVector3 position = self.player.position;
    baseModelViewMatrix = GLKMatrix4Translate(baseModelViewMatrix, 
                                              position.x, 
                                              position.y, 
                                              position.z);
    
    [self.space drawWithBaseModelViewMatrix:baseModelViewMatrix
                           projectionMatrix:projectionMatrix
                                timeElapsed:self.timeSinceLastDraw
                                  forPlayer:self.player];
    
    if (editMode && editingWorldSprings)
    {
        SUWorldSpringThing *springThing = [editingWorldSprings objectAtIndex:0];
        float loopLength = springThing.timeline.length;
        percLoopProgressView.progress = loopTime / loopLength;
        loopTime = fmodf(loopTime + self.timeSinceLastDraw, loopLength);
    }
}

@end
