//
//  SUViewController.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUViewController.h"
#import "SUSpace.h"
#import "SUAudioManager.h"

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
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation SUViewController
@synthesize context = _context;
@synthesize space;
@synthesize player;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
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

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if ([touches containsObject:movementTouch])
    {
        CGPoint touchPoint = [movementTouch locationInView:movementTouch.view];
        GLKVector2 movementTouchPoint = GLKVector2Make(touchPoint.x, touchPoint.y);
        movementTouchOffset = GLKVector2Subtract(movementTouchPoint, movementTouchStartPoint);
    }    
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if ([touches containsObject:movementTouch])
    {
        movementTouch = nil;
        
        movingForward = NO;
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
        
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
        movementRate = MAX(0, movementRate - 0.3f);
        turnRate = MAX(0, turnRate - 0.00003f);
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
}

@end
