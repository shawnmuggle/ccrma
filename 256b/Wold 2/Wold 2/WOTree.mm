//
//  WOTree.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOTree.h"
#import "ASIFormDataRequest.h"

@implementation WOTree
@synthesize angle, lSystem, planetId;

- (id) initWithAngle:(float)newAngle andOrigin:(CGPoint)origin andType:(int)newType andFilename:(NSString*)filename
{
    self = [super init];
    if (self) {
        self.angle = newAngle;
        treeId = -1;
        
        type = newType;
        switch (type) {

                float range, branchAngle;
                int numBranches;
                WONode* n;
                
            case 1:
                // Tree 1 (normal looking, with leaves)
                
                // TODO: Change maxgeneration back to 5
                self.lSystem = [[WOLSystem alloc] initWithMaxGeneration:4 andAngle:newAngle andOrigin:origin andSoundFile:filename];
                self.lSystem.ticksPerGeneration = 30;
                self.lSystem.layer.strokeColor = [UIColor colorWithRed:45.0 / 255 green:115.0 / 255 blue:57.0 / 255 alpha:1.0].CGColor;
                self.lSystem.layer.fillColor = nil;
                self.lSystem.layer.lineWidth = 2.0;
                n = [[[WOLittleFNode alloc] initWithBaseLength:40 andMaxOffset:30] autorelease];
                [self.lSystem.nodes addObject:n];
                n = [[[WOANode alloc] init] autorelease];
                [self.lSystem.nodes addObject:n];
                
                break;

            case 2:
                // Tree 2 (curly bush)
                range = 60;
                numBranches = 10;
                self.lSystem = [[WOLSystem alloc] initWithMaxGeneration:10 andAngle:newAngle andOrigin:origin andSoundFile:filename];
                self.lSystem.ticksPerGeneration = 10;
                self.lSystem.layer.strokeColor = [UIColor colorWithRed:0.5 green:0.2 blue:0.1 alpha:1.0].CGColor;
                self.lSystem.layer.fillColor = nil;
                self.lSystem.layer.lineWidth = 1.0;
                for (int i = 0; i < numBranches; i++) {
                    branchAngle = range * ((2 * i / (float)(numBranches - 1)) - 1.0);
                    
                    n = [[[WOLeftBracketNode alloc] init] autorelease];
                    [self.lSystem.nodes addObject:n];
                    // TODO: Make angle's random offset vary around 0 instead of strictly positive
                    n = [[[WOAngleNode alloc] initWithBaseAngle:branchAngle andMaxOffset:10] autorelease];
                    [self.lSystem.nodes addObject:n];
                    n = [[[WOWiggleNode alloc] init] autorelease];
                    [self.lSystem.nodes addObject:n];
                    n = [[[WOLNode alloc] init] autorelease];
                    [self.lSystem.nodes addObject:n];
                    n = [[[WORightBracketNode alloc] init] autorelease];
                    [self.lSystem.nodes addObject:n];
                }
                
                break;
                
            case 3:
                // Tree 3 (pine-like cone tree)
                self.lSystem = [[WOLSystem alloc] initWithMaxGeneration:5 andAngle:newAngle andOrigin:origin andSoundFile:filename];
                self.lSystem.ticksPerGeneration = 30;
                self.lSystem.layer.strokeColor = [UIColor colorWithRed:242.0 / 255 green:141.0 / 255 blue:0.0 / 255 alpha:1.0].CGColor;
                self.lSystem.layer.fillColor = nil;
                self.lSystem.layer.lineWidth = 5.0;
                n = [[[WOPineNode alloc] init] autorelease];
                [self.lSystem.nodes addObject:n];
                
                break;
                
            case 4:
                // Tree 4 (curly side tree)
                self.lSystem = [[WOLSystem alloc] initWithMaxGeneration:4 andAngle:newAngle andOrigin:origin andSoundFile:filename];
                self.lSystem.ticksPerGeneration = 40;
                self.lSystem.layer.strokeColor = [UIColor colorWithRed:171.0 / 255 green:49.0 / 255 blue:117.0 / 255 alpha:1.0].CGColor;
                self.lSystem.layer.fillColor = nil;
                self.lSystem.layer.lineWidth = 1.0;
                n = [[[WOAngleNode alloc] initWithBaseAngle:-20 andMaxOffset:4] autorelease];
                [self.lSystem.nodes addObject:n];
                n = [[[WOCurlyNode alloc] init] autorelease];
                [self.lSystem.nodes addObject:n];

                break;
            default:
                break;
        }
    }
    return self;
}

- (void) tick
{
    [self.lSystem tick];
}

- (void) stopGrowing
{
    self.lSystem.growing = NO;
}

- (bool) isGrowing
{
    return self.lSystem.growing;
}

- (void) updateServer
{
    if (treeId == -1) {
        treeId = [self getNewTreeIdFromServer];
        NSLog(@"Got a new tree ID: %d", treeId);
    }
    
    NSLog(@"sending tree updates to server...");
	
	NSString *server = [[NSString alloc] initWithUTF8String:"http://mikerotondo.com/wold/"];
	
	NSURL *url = [NSURL URLWithString:[server stringByAppendingString:@"trees"]];
	
	ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
	
    // Setup the variables for the request
    [request setPostValue:[NSNumber numberWithFloat:treeId] forKey:@"id"];
    
    [request setPostValue:[NSNumber numberWithFloat:angle] forKey:@"angle"];
    [request setPostValue:[NSNumber numberWithInt:type] forKey:@"type"];
    [request setPostValue:[NSNumber numberWithFloat:lSystem.angleOffset] forKey:@"angleOffset"];
    [request setPostValue:[NSNumber numberWithFloat:lSystem.currentGeneration + (lSystem.generationTickCount / (float)lSystem.ticksPerGeneration)] forKey:@"age"];
    
	[request startSynchronous];
}

- (int) getNewTreeIdFromServer
{
	NSLog(@"getting new tree id to server...");
	
	NSString *server = [[NSString alloc] initWithUTF8String:"http://mikerotondo.com/wold/"];
	NSURL *url = [NSURL URLWithString:[server stringByAppendingFormat:@"newtree/%d", self.planetId]];
	
	ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request startSynchronous];
    
	NSError *error = [request error];
	if (!error) {
		NSString *response = [request responseString];
        return [response intValue];
	}
    return -1;
}

- (void) tickAudio:(stk::StkFrames*)frames;
{
    [self.lSystem tickAudio:frames];
}

- (CALayer*) layer
{
    return self.lSystem.layer;
}

- (void) handleTouch:(CGPoint)loc withVelocity:(CGPoint)vel
{
    [self.lSystem handleTouch:loc withVelocity:vel];
}

- (void) dealloc
{
    [lSystem release];
    
    [super dealloc];
}

@end
