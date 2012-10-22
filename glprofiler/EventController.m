//
//  MyTreeController.m
//
//  Created by handley on 7/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "EventController.h"
#import "AppController.h"
#import "Event.h"
#import "events.h"
#import "ProfileOpenGLView.h"
#import "MyDocument.h"
#import "utility.h"

@implementation EventController
- (IBAction)capture:(id)sender
{
	NSLog(@"Capturing!");
	
	[AppController sharedInstance].currentEventController = self;
	[[AppController sharedInstance] sendMessageToTarget:etCAPTURE data:nil];
}
- (IBAction)stopCapturing:(id)sender
{
	[[AppController sharedInstance] sendMessageToTarget:etSTOP_CAPTURE data:nil];
}
@end

