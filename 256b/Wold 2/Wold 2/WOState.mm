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
#import "ASIFormDataRequest.h"

@implementation WOState
@synthesize solarSystems, currentSolarSystem, currentPlanet, zoomLevel, rotationAngle, zoomedPlanetYOffset, justZoomed;

- (id) init
{
    self = [super init];
    if (self) {
        srand((unsigned)time(0));
        
        self.solarSystems = [NSMutableSet setWithCapacity:1];

        // NOTE: Right now this just creates a new solar system and stores it on the server but never retrieves it
        // TODO: Switch this over to grab the latest solar system from the server
        // TODO: Add a method to generate a new solar system
        [self.solarSystems addObject:[[[WOSolarSystem alloc] initWithId:[self getNewSolarSystemIdFromServer]] autorelease]];
        
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

- (int) getNewSolarSystemIdFromServer
{
	NSLog(@"getting new solar system to server...");
	
	NSString *server = [[NSString alloc] initWithUTF8String:"http://mikerotondo.com/wold/"];
	NSURL *url = [NSURL URLWithString:[server stringByAppendingString:@"newsystem"]];
	
	ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request startSynchronous];
    
	NSError *error = [request error];
	if (!error) {
		NSString *response = [request responseString];
        return [response intValue];
	}
    return -1;
}

- (void) awakeFromNib
{
    [view.layer addSublayer:self.currentSolarSystem.sky];

    pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
    pan.minimumNumberOfTouches = 2;
    [view addGestureRecognizer:pan];
    
    rub = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleRub:)];
    rub.maximumNumberOfTouches = 1;
    [view addGestureRecognizer:rub];
    rub.delegate = self;
    
    press = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];
    [view addGestureRecognizer:press];
    press.delegate = self;
    
    twoPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];
    twoPress.numberOfTouchesRequired = 2;
    [view addGestureRecognizer:twoPress];
    twoPress.delegate = self;
    
    threePress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];
    threePress.numberOfTouchesRequired = 3;
    [view addGestureRecognizer:threePress];
    threePress.delegate = self;
    
    fourPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];
    fourPress.numberOfTouchesRequired = 4;
    fourPress.numberOfTapsRequired = 0;
    [view addGestureRecognizer:fourPress];
    fourPress.delegate = self;
    
    pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinch:)];
    [view addGestureRecognizer:pinch];

    tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSolarSystemTap:)];
    [view addGestureRecognizer:tap];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    return YES;
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    if ((gestureRecognizer == rub || gestureRecognizer == press || gestureRecognizer == twoPress) && 
        (otherGestureRecognizer == rub || otherGestureRecognizer == press || otherGestureRecognizer == twoPress)) {
        return YES;
    }
    else {
        return NO;
    }
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
        switch (self.zoomLevel) {
            case 3: // Zoomed in to the surface
                self.currentPlanet.layer.shadowOpacity = 0.0;
                scale = 1.0; // TODO: Set this based on the baseRadius of the planet so the surface is in a consistent location
                centerX = -(self.currentSolarSystem.star.loc.x + self.currentPlanet.loc.x) * scale + screen.size.width / 2.0;
                centerY = -(self.currentSolarSystem.star.loc.y + self.currentPlanet.loc.y) * scale + screen.size.height / 2.0 + self.currentPlanet.baseRadius;
                // TODO: Figure out how to  set centerX and centerY to zoom in on the right planet!
                background = [UIColor colorWithRed:198.0 / 255 green:218.0 / 255 blue:232.0 / 255 alpha:1.0].CGColor;
                break;
                
            case 2: // Zoomed in so the planet is centered on the screen
                self.currentPlanet.layer.shadowOpacity = 0.0;
                scale = 0.25;
                centerX = -(self.currentSolarSystem.star.loc.x + self.currentPlanet.loc.x) * scale + screen.size.width / 2.0;
                centerY = -(self.currentSolarSystem.star.loc.y + self.currentPlanet.loc.y) * scale + screen.size.height / 2.0;
                background = [UIColor colorWithRed:46.0 / 255 green:51.0 / 255 blue:63.0 / 255 alpha:1.0].CGColor;
                break;
                
            case 1:
                self.currentPlanet.layer.shadowOpacity = 0.4;
                scale = 0.05;
                centerX = -self.currentSolarSystem.star.loc.x * scale + screen.size.width / 8.0;
                centerY = -self.currentSolarSystem.star.loc.y * scale + screen.size.height / 2.0;
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


- (void) handlePan:(UIPanGestureRecognizer*)sender
{
    self.currentPlanet.rotationAngleIncrement += [sender velocityInView:sender.view].x * 0.000005;
}

- (void) handleLongPress:(UILongPressGestureRecognizer*)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        self.currentPlanet.rotationAngleIncrement = 0;
        
        CGPoint loc = [sender locationInView:sender.view];
        
        float xDiff = loc.x - 512; // hacked magic number, we know the screen is sideways & 1024 px wide
        float yDiff = (768 - loc.y) + (self.currentPlanet.baseRadius - 384); // more hackass magic numbers
        
        int type = [sender numberOfTouches];
        if ([sender numberOfTapsRequired] == 1) {
            type += 3;
        }
        
        [self.currentPlanet addTreeAtAngle:atan2f(yDiff, xDiff) + self.currentPlanet.rotationAngle 
                                  treeType:type];
    } else if (sender.state == UIGestureRecognizerStateEnded) {
        [self.currentPlanet stopGrowingTree];
    }
}

- (void) handlePinch:(UIPinchGestureRecognizer*)sender
{
    if (sender.state == UIGestureRecognizerStateBegan) {
        if (sender.scale > 1) { 
            self.zoomLevel += 1;
        } else if (sender.scale < 1) {
            self.zoomLevel -= 1;
        }
        if (self.zoomLevel < 1) self.zoomLevel = 1;
        if (self.zoomLevel > 3) self.zoomLevel = 3; // TODO: Eventually use 4 as the max here for individual-tree style zoom
        
        self.justZoomed = YES;
    }
}

- (void) handleSolarSystemTap:(UITapGestureRecognizer*)sender
{
    if (sender.state == UIGestureRecognizerStateEnded) {
        CGPoint loc = [sender locationInView:tap.view];
        
        int i = 0;
        for (WOPlanet* planet in self.currentSolarSystem.planets) {
            CGPoint convertedLoc = [view.layer convertPoint:loc toLayer:planet.layer]; // TODO: Verify that we're still getting correct conversion
            convertedLoc = CGPointMake(convertedLoc.x, convertedLoc.y);
            
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

- (void) handleRub:(UIPanGestureRecognizer*)sender
{
    if (sender.state == UIGestureRecognizerStateBegan || sender.state == UIGestureRecognizerStateChanged) {
        CGPoint loc = [sender locationInView:sender.view];
        
        for (WOTree* tree in currentPlanet.trees) {
            CGPoint convertedLoc = [view.layer convertPoint:loc toLayer:tree.lSystem.layer]; // TODO: Verify that we're still getting correct conversion
            convertedLoc = CGPointMake(convertedLoc.y, 
                                       convertedLoc.x - tree.lSystem.layer.bounds.size.height / 2.0);
            
            if ([tree.lSystem.layer containsPoint:convertedLoc]) {
                
//                CGColorRef color = tree.lSystem.layer.strokeColor;
//                CABasicAnimation* flashAnimation = [CABasicAnimation
//                                                    animationWithKeyPath:@"strokeColor"];
//                flashAnimation.toValue = (id)[UIColor whiteColor].CGColor;
//                flashAnimation.duration = 0.25;
//                CABasicAnimation* fadeAnimation = [CABasicAnimation
//                                                   animationWithKeyPath:@"strokeColor"];
//                fadeAnimation.fromValue = (id)[UIColor whiteColor].CGColor;
//                fadeAnimation.toValue = (id)color;
//                fadeAnimation.duration = 1.0;
//                fadeAnimation.beginTime = 0.25;
//                
//                CAAnimationGroup *group = [CAAnimationGroup animation];
//                [group setDuration:1.25];
//                [group setAnimations:[NSArray arrayWithObjects:flashAnimation, fadeAnimation, nil]];
//                
//                [tree.lSystem.layer addAnimation:group forKey:nil];
                
                CGSize treeSize = tree.lSystem.layer.bounds.size;
                // This point goes from -.5, 0 in the lower left to 0.5, 1 in the top right
                CGPoint normalizedPoint = CGPointMake(convertedLoc.x / treeSize.width, (convertedLoc.y + treeSize.height * 0.5) / treeSize.height);
                [tree handleTouch:normalizedPoint withVelocity:[sender velocityInView:sender.view]];
            }
        }
    }
}

@end
