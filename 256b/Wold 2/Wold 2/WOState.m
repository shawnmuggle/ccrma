//
//  WOState.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOState.h"
#import "QuartzCore/QuartzCore.h"
#import "WOTree.h"

@implementation WOState
@synthesize solarSystems, currentSolarSystem, currentPlanet, zoomLevel, rotationAngle, zoomedPlanetYOffset, justZoomed;

- (id) init
{
    self = [super init];
    if (self) {
        srand((unsigned)time(0));
        
        self.solarSystems = [NSMutableSet setWithCapacity:1];
        [self.solarSystems addObject:[[[WOSolarSystem alloc] init] autorelease]];
        
        self.currentSolarSystem = [self.solarSystems anyObject];
        self.currentPlanet = nil;
        self.zoomLevel = 1;
        
        self.rotationAngle = 0;
        rotationAngleIncrement = 0;
        rotationDamping = 0.92;
        
        self.zoomedPlanetYOffset = 1000;
        
        
        self.justZoomed = YES;
    }
    return self;
}

- (void) awakeFromNib
{
    [view.layer addSublayer:self.currentSolarSystem.sky];

    UIPanGestureRecognizer* pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
    pan.minimumNumberOfTouches = 2;
    [view addGestureRecognizer:pan];
    
    UIPanGestureRecognizer* rub = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleRub:)];
    rub.maximumNumberOfTouches = 1;
    [view addGestureRecognizer:rub];
    
    UILongPressGestureRecognizer* press = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];
    [view addGestureRecognizer:press];
    
    UILongPressGestureRecognizer* twoPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];
    twoPress.numberOfTouchesRequired = 2;
    [view addGestureRecognizer:twoPress];
    
    UIPinchGestureRecognizer* pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinch:)];
    [view addGestureRecognizer:pinch];

    UITapGestureRecognizer* tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSolarSystemTap:)];
    [view addGestureRecognizer:tap];
}

- (void) tick:(id)sender
{
    [self.currentPlanet tick];
    
    [self render];
}

- (void) render
{
    screen = view.bounds;
    self.currentSolarSystem.sky.frame = screen;
    // TODO: Figure out how to make the background blue when zoomed in on planet, black when zoomed out with fade
    float scale, centerX, centerY;
    CGColorRef background;
    
    if (self.justZoomed) {
        NSLog(@"About to switch on zoomLevel %d", self.zoomLevel);
        switch (self.zoomLevel) {
            case 3: // Zoomed in to the surface
                self.currentPlanet.layer.shadowOpacity = 0.0;
                scale = 1.0; // TODO: Set this based on the baseRadius of the planet so the surface is in a consistent location
                centerX = -self.currentPlanet.loc.x * scale + screen.size.width / 2.0;
                centerY = -self.currentPlanet.loc.y * scale + screen.size.height / 2.0 + self.currentPlanet.baseRadius;
                // TODO: Figure out how to  set centerX and centerY to zoom in on the right planet!
                background = [UIColor colorWithRed:198.0 / 255 green:218.0 / 255 blue:232.0 / 255 alpha:1.0].CGColor;
                break;
                
            case 2: // Zoomed in so the planet is centered on the screen
                self.currentPlanet.layer.shadowOpacity = 0.0;
                scale = 0.25;
                centerX = -self.currentPlanet.loc.x * scale + screen.size.width / 2.0;
                centerY = -self.currentPlanet.loc.y * scale + screen.size.height / 2.0;
                background = [UIColor colorWithRed:46.0 / 255 green:51.0 / 255 blue:63.0 / 255 alpha:1.0].CGColor;
                break;
                
            case 1:
                self.currentPlanet.layer.shadowOpacity = 0.4;
                scale = 0.05;
                centerX = screen.size.width / 8.0;
                centerY = screen.size.height / 2.0;
                background = [UIColor colorWithRed:16.0 / 255 green:21.0 / 255 blue:33.0 / 255 alpha:1.0].CGColor;
                break;
                
            default:
                break;
        }
        
        [CATransaction begin];
        [CATransaction setValue:[NSNumber numberWithFloat:1.0f] forKey:kCATransactionAnimationDuration];
        
        CATransform3D scaleTransform = CATransform3DMakeScale(scale, scale, scale);
        CATransform3D translateTransform = CATransform3DMakeTranslation(centerX, centerY, 0);
        scaledAndTranslated = CATransform3DConcat(scaleTransform, translateTransform);
        
        self.currentSolarSystem.zoom.transform = scaledAndTranslated;
        self.currentSolarSystem.sky.backgroundColor = background;
        
        [CATransaction commit];
        
        self.justZoomed = NO;
    }
    
    [CATransaction begin];
    [CATransaction setValue:[NSNumber numberWithFloat:0.25f] forKey:kCATransactionAnimationDuration];
    
    CATransform3D rotateTransform = CATransform3DMakeRotation(self.currentPlanet.rotationAngle, 0, 0, 1);
    currentPlanet.rotation.transform = rotateTransform;
    
    [CATransaction commit];
    
}


- (void) handlePan:(id)sender
{
    UIPanGestureRecognizer* pan = (UIPanGestureRecognizer*)sender;
    self.currentPlanet.rotationAngleIncrement += [pan velocityInView:pan.view].x * 0.000005;
}

- (void) handleLongPress:(id)sender
{
    UILongPressGestureRecognizer* press = (UILongPressGestureRecognizer*)sender;
    if (press.state == UIGestureRecognizerStateBegan) {
        self.currentPlanet.rotationAngleIncrement = 0;
        
        CGPoint loc = [press locationInView:press.view];
        
        float xDiff = loc.x - 512; // hacked magic number, we know the screen is sideways & 1024 px wide
        float yDiff = (768 - loc.y) + (self.currentPlanet.baseRadius - 384); // more hackass magic numbers
        
        [self.currentPlanet addTreeAtAngle:atan2f(yDiff, xDiff) + self.currentPlanet.rotationAngle 
                                  treeType:[press numberOfTouches]];
    }
}

- (void) handlePinch:(id)sender
{
    UIPinchGestureRecognizer* pinch = (UIPinchGestureRecognizer*)sender;
    if (pinch.state == UIGestureRecognizerStateBegan) {
        if (pinch.scale > 1) { 
            self.zoomLevel += 1;
        } else if (pinch.scale < 1) {
            self.zoomLevel -= 1;
        }
        if (self.zoomLevel < 1) self.zoomLevel = 1;
        if (self.zoomLevel > 3) self.zoomLevel = 3; // TODO: Eventually use 4 as the max here for individual-tree style zoom
        
        self.justZoomed = YES;
    }
}

- (void) handleSolarSystemTap:(id)sender
{
    UITapGestureRecognizer* tap = (UITapGestureRecognizer*)sender;
    if (tap.state == UIGestureRecognizerStateEnded) {
        CGPoint loc = [tap locationInView:tap.view];
        
        int i = 0;
        for (WOPlanet* planet in self.currentSolarSystem.planets) {
            CGPoint convertedLoc = [view.layer convertPoint:loc toLayer:planet.layer]; // TODO: Verify that we're still getting correct conversion
            convertedLoc = CGPointMake(convertedLoc.x, 
                                       convertedLoc.y);
            
            if ([planet.layer containsPoint:convertedLoc]) {
                
                self.currentPlanet.layer.shadowOpacity = 0.0;
                
                self.currentPlanet = planet;
                
                self.currentPlanet.layer.shadowColor = [UIColor whiteColor].CGColor;
                self.currentPlanet.layer.shadowOpacity = 0.4;
                self.currentPlanet.layer.shadowOffset = CGSizeMake(0, 0);
                self.currentPlanet.layer.shadowRadius = 150;
            }
            i++;
        }
    }
}

- (void) handleRub:(id)sender
{
    UIPanGestureRecognizer* rub = (UIPanGestureRecognizer*)sender;
    if (rub.state == UIGestureRecognizerStateBegan || rub.state == UIGestureRecognizerStateChanged) {
        CGPoint loc = [rub locationInView:rub.view];
        
        for (WOTree* tree in currentPlanet.trees) {
            CGPoint convertedLoc = [view.layer convertPoint:loc toLayer:tree.lSystem.layer]; // TODO: Verify that we're still getting correct conversion
            convertedLoc = CGPointMake(convertedLoc.y, 
                                       convertedLoc.x - tree.lSystem.layer.bounds.size.height / 2.0);
            
            if ([tree.lSystem.layer containsPoint:convertedLoc]) {
                CGColorRef color = tree.lSystem.layer.strokeColor;
                
                CABasicAnimation* flashAnimation = [CABasicAnimation
                                                    animationWithKeyPath:@"strokeColor"];
                flashAnimation.toValue = (id)[UIColor whiteColor].CGColor;
                flashAnimation.duration = 0.25;
                CABasicAnimation* fadeAnimation = [CABasicAnimation
                                                   animationWithKeyPath:@"strokeColor"];
                fadeAnimation.fromValue = (id)[UIColor whiteColor].CGColor;
                fadeAnimation.toValue = (id)color;
                fadeAnimation.duration = 1.0;
                fadeAnimation.beginTime = 0.25;
                
                CAAnimationGroup *group = [CAAnimationGroup animation];
                [group setDuration:1.25];
                [group setAnimations:[NSArray arrayWithObjects:flashAnimation, fadeAnimation, nil]];
                
                [tree.lSystem.layer addAnimation:group forKey:nil];
            }
        }
    }
}

@end
