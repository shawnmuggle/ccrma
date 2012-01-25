//
//  SAWorldViewController.m
//  Soundaround
//
//  Created by Michael Rotondo on 11/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SAWorldViewController.h"
#import "SAWorld.h"
#import "SAAvatar.h"
#import "SAWorldView.h"
#import "SABallGuide.h"

@interface SAWorldViewController ()
{
    SABallGuide *currentBallGuide;
}
@end

@implementation SAWorldViewController
@synthesize world;

-(BOOL)acceptsFirstResponder { return YES; }
-(BOOL)becomeFirstResponder  { return YES; }
-(BOOL)resignFirstResponder  { return YES; }

- (void)mouseDown:(NSEvent *)theEvent
{
    if (!self.world.editMode)
    {
        [self.world.avatar throwBallTowardsPoint:[(SAWorldView *)self.view worldPosForEvent:theEvent]];
    }
    else
    {
        [self.world startBallGuide];
    }
}

- (void)mouseUp:(NSEvent *)theEvent
{
    if (self.world.editMode)
    {
        [self.world finishBallGuide];
    }
}

//- (void)mouseMoved:(NSEvent *)theEvent
//{
//}

- (void)mouseDragged:(NSEvent *)theEvent
{
    [self.world addBallGuidePoint:[(SAWorldView *)self.view worldPosForEvent:theEvent]];    
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    [self.world.avatar grabBall];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
    NSLog(@"Button number: %ld", theEvent.buttonNumber);
}

-(void)keyUp:(NSEvent*)event
{
    CGPoint movementVector;
    
    switch( [event keyCode] ) {
        case 2:
        case 124:       // right arrow
            movementVector = CGPointMake(1, 0);
            [self.world.avatar stopMoving:movementVector];
            break;
        case 0:
        case 123:       // left arrow
            movementVector = CGPointMake(-1, 0);
            [self.world.avatar stopMoving:movementVector];
            break;
        case 126:       // up arrow
        case 125:       // down arrow
        default:
            //            NSLog(@"Key pressed: %@", event);
            break;
    }    
}

-(void)keyDown:(NSEvent*)event
{   
//    NSLog(@"Key pressed: %@", event);
    CGPoint movementVector;
    switch( [event keyCode] ) {
        case 14: // E key, toggle edit mode
            [self.world toggleEditMode];
            break;
        case 2:
        case 124:       // right arrow
            movementVector = CGPointMake(1, 0);
            [self.world.avatar startMoving:movementVector];
            break;
        case 0:
        case 123:       // left arrow
            movementVector = CGPointMake(-1, 0);
            [self.world.avatar startMoving:movementVector];
            break;
        case 3:
            ((SAWorldView *)self.view).followingBall = !((SAWorldView *)self.view).followingBall;
            break;
        case 126:       // up arrow
        case 125:       // down arrow
        default:
            break;
    }
}

@end
