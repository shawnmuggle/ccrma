//
//  AppController.h
//
//  Created by handley on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class EventController;
@interface AppController : NSObject<NSPortDelegate> {
	NSMachPort *localPort, *remotePort;
	
	EventController *currentEventController;
}
- (id)init;
+ (AppController *)sharedInstance;
- (void)sendMessageToTarget:(uint32_t)eventType data:(NSArray *)data;
- (void)closeConnection;

@property(retain) EventController *currentEventController;
@end
