//
//  MyDocument.h
//  GLProfiler
//
//  Created by handley on 7/20/10.
//  Copyright __MyCompanyName__ 2010 . All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface BufferInfo : NSObject {
	GLuint clientID, localID;
}
@property GLuint clientID, localID;
@end

@class MyTreeController,Event;
@interface MyDocument : NSPersistentDocument {
	IBOutlet NSTextView *debugText;
	IBOutlet MyTreeController *eventController;
	IBOutlet NSPanel *progressIndicator;
	
	NSMutableArray *events;
	
	//transient:
	NSMutableArray *eventsForUI;
	Event *captureEvent;
	Event *beginFrameEvent;
	Event *glstateEvent;
	NSMutableArray *temporaryStateEventArray;
	
	NSMutableDictionary *bufferMapping;
	
	NSUInteger requestedEvents, satisfiedRequests;
}

- (BufferInfo *)bufferInfo:(uint32_t)bufferID; 
- (GLuint)displayBufferWithID:(GLuint)bufferID;

@property(readonly) NSMutableArray *events, *eventsForUI;
@end
