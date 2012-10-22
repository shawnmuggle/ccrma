//
//  MyDocument.m
//  GLProfiler
//
//  Created by handley on 7/20/10.
//  Copyright __MyCompanyName__ 2010 . All rights reserved.
//

#import "MyDocument.h"
#import "AppController.h"
#import "events.h"
#import "Event.h"
#import "EventController.h"
#import "MetaEvents.h"

@interface MyDocument()
@end

@implementation BufferInfo
@synthesize clientID, localID;
@end


@implementation MyDocument
- (id)init
{
	if (self = [super init])
	{
		events = [[NSMutableArray alloc] initWithCapacity:8];
		eventsForUI = [[NSMutableArray alloc] initWithCapacity:8];
		temporaryStateEventArray = [[NSMutableArray alloc] init];
		bufferMapping = [[NSMutableDictionary alloc] init];
	}
	return self;
}
- (void)dealloc
{
	[bufferMapping release];
	[temporaryStateEventArray release];
	[eventsForUI release];
	[events release];
	[super dealloc];
}

- (NSString *)windowNibName 
{
    return @"MyDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)windowController 
{
    [super windowControllerDidLoadNib:windowController];
    // user interface preparation code
}

//events array mutators
- (void)insertObject:(id)object inEventsAtIndex:(NSUInteger)index
{
	//the first thing the profiled app does data-wise is to send any surface events over. 
	//Next, the profiled app sends across any initial state setup; that is, the current GL state pre-frame
	//The frame begin event comes next, which signifies the beginning of a frame
	//next comes state change events, and then a draw call. We group these underneath the frame begin event, and the state changes underneath the draw call
	//A frame end event is next down the line
	//Finally, the profiled app holds the connection open for the profiler to send any final requests for data, or any results back to the app...
	//in the future, we may hijack the rendering loop here, and let the profiler render into the view of the app...., but that's probably overkill
	
	//we store the event in the event record in-order, regardless of the type or kind of the event.
	[events insertObject:object atIndex:index];
	
	//do any document-specific event setup
	Event *e = (Event *)object;
	e.index = index;
	e.document = self;

	//check to see if the event would like to cache any previous event
	for (Event *c in [events reverseObjectEnumerator])
	{
		if ([e mightCacheEvent:c])
		{
			break; //if might cache returns true, then we've cached everything we can cache
		}
	}

	if (e.msgID == etCAPTURE)
	{
		captureEvent = e;
		[eventsForUI addObject:e];
	}
	else if (e.msgID == etSTOP_CAPTURE)
	{
		//validate all of the events
		for (Event *e in events)
		{
			NSError *error;
			if ([e validate:&error] == E_MissingData)
			{
				if (error)
				{
					//this is a recoverable error, so we ask the app for information
					//generate an event to send to the app
					//all event requests are otf type, id					
					//remember that we're waiting on at least 1 more requested event
					requestedEvents++;
					
					NSDictionary *errorInfo = [error userInfo];
					
					//send event
					[[AppController sharedInstance] sendMessageToTarget:[[errorInfo objectForKey:EventErrorMsgType] unsignedIntValue] data:[errorInfo objectForKey:EventErrorComponents]];
				}
				else
				{
					//this is a non-recoverable error, which is bad, so we invalidate this event
					e.isValid = NO;
				}
			}
		}
		
		if (requestedEvents == 0)
		{
			//artificially refresh the UI
			[[self mutableArrayValueForKey:@"eventsForUI"] addObject:object];
			[[self mutableArrayValueForKey:@"eventsForUI"] removeLastObject];
			
			[[AppController sharedInstance] sendMessageToTarget:etFINISHED_CAPTURING data:nil];
		}
		else
		{
			//pop up a modal panel, with a scroll bar
			[NSApp beginSheet:progressIndicator modalForWindow:[NSApp keyWindow] modalDelegate:nil didEndSelector:nil contextInfo:0];
		}
	}
	else if (e.msgID == etFRAME_START)
	{
		beginFrameEvent = e;
		[[captureEvent mutableArrayValueForKey:@"children"] addObject:e];
	}
	else if (e.msgID == etFRAME_END)
	{
		beginFrameEvent = nil; //bookkeeping
	}
	else if (e.isDrawEvent)
	{
		for (Event *c in temporaryStateEventArray)
		{
			[[e mutableArrayValueForKey:@"children"] addObject:c];
		}
		[temporaryStateEventArray removeAllObjects];
		[[beginFrameEvent mutableArrayValueForKey:@"children"] addObject:e];
	}
	else if (!beginFrameEvent)
	{
		if (!glstateEvent)
		{
			glstateEvent = [[[GLStateEvent alloc] initWithID:0 data:nil] autorelease];
			[events addObject:glstateEvent];
			[[captureEvent mutableArrayValueForKey:@"children"] addObject:glstateEvent];
			glstateEvent.isFake = YES;
			glstateEvent.isValid = YES;
		}
		//e is a pre-frame state-change event
		//add the event to the current capture
		[[glstateEvent mutableArrayValueForKey:@"children"] addObject:e];
	}
	else
	{
		//e is a pre-draw-event state-change event
		[temporaryStateEventArray addObject:e];
	}
}
- (void)removeObjectFromEventsAtIndex:(NSUInteger)index
{
	[events removeObjectAtIndex:index];
}

- (BufferInfo *)bufferInfo:(uint32_t)bufferID
{
	BufferInfo *info = [bufferMapping objectForKey:[NSNumber numberWithUnsignedInt:bufferID]];
	if (!info)
	{
		info = [[BufferInfo alloc] init];
		info.clientID = bufferID;
		uint32_t localID;
		glGenBuffers(1, &localID);
		info.localID = localID;
		
		[bufferMapping setObject:info forKey:[NSNumber numberWithUnsignedInt:bufferID]];
	}
	
	return info;
}

- (void)registerBufferMappingFrom:(NSData *)clientBuffers to:(NSData *)displayBuffers
{
	assert(clientBuffers.length == displayBuffers.length);
	
	const GLuint *clientBuffer = [clientBuffers bytes];
	const GLuint *displayBuffer = [displayBuffers bytes];
	
	for (unsigned int i=0;i<clientBuffers.length / sizeof(GLuint);++i)
	{
		[bufferMapping setObject:[NSNumber numberWithUnsignedInt:displayBuffer[i]] forKey:[NSNumber numberWithUnsignedInt:clientBuffer[i]]];
	}
}
- (GLuint)displayBufferWithID:(GLuint)bufferID
{
	return [(NSNumber *)[bufferMapping objectForKey:[NSNumber numberWithUnsignedInt:bufferID]] unsignedIntValue];
}


@synthesize events, eventsForUI;
@end
