//
//  AppController.m
//
//  Created by handley on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "AppController.h"
#import "EventController.h"
#import "Event.h"

static AppController *appController = 0;

@interface AppController()
- (BOOL)connectToRemotePort;
@end

@implementation AppController
+ (AppController *)sharedInstance
{
	if (!appController)
	{
		appController = [[AppController alloc] init];
	}
	return appController;
}
- (id)init
{
	NSAssert(appController == 0, @"Bad app controller!");
	if (self = [super init])
	{
		
	}
	return self;
}

- (void)handlePortMessage:(NSPortMessage *)message
{
	NSLog(@"Got event: %u",[message msgid]);
	//convert the message into an Event
	Event *newEvent = [Event eventWithID:[message msgid] data:[message components]];

	if (newEvent.msgID == etSTOP_CAPTURE)
	{
		[self closeConnection];
	}

	[currentEventController addObject:newEvent];
}

- (BOOL)connectToRemotePort
{
	if (!remotePort)
	{
		remotePort = (NSMachPort *)[[[NSMachBootstrapServer sharedInstance] portForName:@"glprofiler"] retain];
		if (remotePort)
		{
			localPort = [[NSMachPort alloc] init];
			
			NSPort *temp = localPort;
			[temp setDelegate:self];
			[[NSRunLoop mainRunLoop] addPort:temp forMode:NSDefaultRunLoopMode];
		}
		else
		{
			NSLog(@"Couldn't find any application to connect to!");
		}
	}
	return !!remotePort;
}

- (void)sendMessageToTarget:(uint32_t)eventType data:(NSArray *)data
{
	if ([self connectToRemotePort])
	{
		NSPortMessage *message = [[NSPortMessage alloc] initWithSendPort:remotePort receivePort:localPort components:data];
		[message setMsgid:eventType];
		[message sendBeforeDate:[NSDate distantFuture]];
		[message release];
	}
}

- (void)closeConnection
{
	[[NSRunLoop mainRunLoop] removePort:remotePort forMode:NSDefaultRunLoopMode];
	[remotePort invalidate];
	[remotePort release];
	remotePort = 0;
	[localPort invalidate];
	[localPort release];
	localPort = 0;
}

@synthesize currentEventController;
@end
