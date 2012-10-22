//
//  Event.m
//
//  Created by handley on 7/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "Event.h"
#import "events.h"
#import "MyDocument.h"
#import "Utilities.h"
#import "MetaEvents.h"
#import "ClearEvents.h"
#import "MatrixEvents.h"
#import "ArrayEvents.h"
#import "VBOEvents.h"
#import "DrawEvents.h"
#import "ViewportEvent.h"

#import <objc/runtime.h>

NSString *EventErrorMsgType = @"msgType";
NSString *EventErrorComponents = @"components";

@interface NSAttributedString(SimpleExtensions)
+ (NSAttributedString *)stringWithString:(NSString *)str attributes:(NSDictionary *)dict;
@end
@implementation NSAttributedString(SimpleExtensions)
+ (NSAttributedString *)stringWithString:(NSString *)str attributes:(NSDictionary *)dict
{
	return [[[NSAttributedString alloc] initWithString:str attributes:dict] autorelease];
}
@end

@interface Event()
- (NSString *)_name;
- (NSDictionary *)textAttributes;
@end

@implementation Event
+ (Class)classForID:(uint32_t)msgid
{
	switch (msgid)
	{
	case etFRAME_START:
	case etFRAME_END:
		return [FrameEvent class];
	case etCAPTURE:
	case etSTOP_CAPTURE:
		return [CaptureEvent class];
	case etSURFACE_EVENT:
		return [SurfaceEvent class];
	case etCLEAR_COLOR:
		return [ClearColorEvent class];
	case etCLEAR:
		return [ClearEvent class];
	case etMATRIX_MODE:
		return [MatrixModeEvent class];
	case etLOAD_IDENTITY:
		return [LoadIdentityEvent class];
	case etTRANSLATEF:
		return [TranslatefEvent class];
	case etORTHO:
		return [OrthoEvent class];
	case etPERSPECTIVE:
		return [PerspectiveEvent class];
	case etENABLE_CLIENT_STATE:
		return [EnableClientStateEvent class];
	case etVERTEX_POINTER:
		return [VertexPointerEvent class];
	case etCOLOR_POINTER:
		return [ColorPointerEvent class];
	case etTEXCOORD_POINTER:
		return [TexCoordPointerEvent class];
	case etNORMAL_POINTER:
		return [NormalPointerEvent class];
	case etGEN_BUFFERS:
		return [GenBuffersEvent class];
	case etDELETE_BUFFERS:
		return [DeleteBuffersEvent class];
	case etBIND_BUFFER:
		return [BindBufferEvent class];
	case etBUFFER_DATA:
		return [BufferDataEvent class];
	case etBUFFER_SUB_DATA:
		return [BufferSubDataEvent class];
	case etDRAW_ARRAYS:
		return [DrawArraysEvent class];
	case etVIEPORT:
		return [ViewportEvent class];
	case etINFO_BUFFER:
		return [BufferInfoEvent class];
	default:
		return [Event class];
	}
}
+ (Event *)eventWithID:(uint32_t)msgid data:(NSArray *)inData
{
	return [[[[Event classForID:msgid] alloc] initWithID:msgid data:inData] autorelease];
}
- (id)initWithID:(uint32_t)msgid data:(NSArray *)inData
{
	if (self = [super init])
	{
		static uint32_t nextIndex = 0;
		index = nextIndex++;
		msgID = msgid;
		isValid = YES;
		
		self.originalData = inData;
		
		children = [[NSMutableArray alloc] init];
		
		if (inData)
		{
			NSData *dataHeader = [originalData objectAtIndex:0];
			struct EventHeader *header = (struct EventHeader *)[dataHeader bytes];
			isFake = header->fake;
			time = header->time;
			
			if (class_getInstanceSize([Event class]) < class_getInstanceSize([self class]))
			{
				*(void **)(self+1) = (void *)[dataHeader bytes];
			}
		}
	}
	return self;
}
- (id)copyWithZone:(NSZone *)zone
{
	//copy self, and put self into Event
	Event *event = [[[self class] allocWithZone:zone] init];
	
	event.time = self.time;
	event.endTime = self.endTime;
	event->msgID = self.msgID;
	event->index = self.index;
	
	event.document = self.document;
	event->children = nil; //things that are copied DO NOT copy their children.
	event.isSecondaryReference = self.isSecondaryReference;
	
	return event;
}
- (BOOL)isLeaf
{
	return !children || [children count] == 0;
}
- (NSArray *)children
{
	return children;
}
- (void)insertObject:(id)object inChildrenAtIndex:(NSUInteger)i
{
	[children insertObject:object atIndex:i];
	((Event *)object)->parent = self;
}
- (void)removeObjectFromChildrenAtIndex:(NSUInteger)i
{
	[children removeObjectAtIndex:i];
}
- (BOOL)isScoped
{
	return NO;
}
- (BOOL)isEndScope
{
	return NO;
}

- (NSString *)_name
{
	return [NSString stringWithFormat:@"MsgID: %u",msgID];
}
- (NSDictionary *)textAttributes
{
	NSColor *foregroundColor = !self.isValid ? [NSColor redColor] : (self.isSecondaryReference || self.isFake ? [NSColor grayColor] : [NSColor blackColor]);
	return [NSDictionary dictionaryWithObjectsAndKeys:foregroundColor, NSForegroundColorAttributeName, nil];
}
- (NSAttributedString *)name
{
	return [NSAttributedString stringWithString:[self _name] attributes:[self textAttributes]];
}
- (void)endScopeWithEvent:(Event *)event
{
	@throw [NSException exceptionWithName:@"BadEndScopeException" reason:@"End scope was called on an event without scope" userInfo:nil];
}
- (uint64_t)time
{
	return time;
}
- (void)setTime:(uint64_t)t
{
	self.endTime = time = t;
}
- (float)floatTime
{
	return (float)time;
}
- (float)floatEndTime
{
	return (float)endTime;
}
- (float)duration
{
	return (float)(endTime - time);
}
+ (NSSet *)keyPathsForValuesAffectingDuration:(NSString *)key
{
	return [NSSet setWithObjects:@"time","endTime",nil];
}
+ (NSSet *)keyPathsForValuesAffectingTextColor:(NSString *)key
{
	return [NSSet setWithObjects:@"isSecondaryReference",nil];
}
- (void)displayInRect:(NSRect)rect
{
	NSError *e;
	if ([self validate:&e] == E_None)
	{
		[self drawRect:rect];
	}
}
- (void)drawRect:(NSRect)rect
{
	//nada
}
- (NSString *)viewName
{
	return @"None";
}
- (BOOL)isDrawEvent
{
	return NO;
}
- (BOOL)mightCacheEvent:(Event *)e
{
	return YES;
}
- (enum EventValidationErrors)validate:(NSError **)error
{
	return E_None;
}

- (Event *)drawEventParent
{
	if (parent)
	{
		if (parent.isDrawEvent)
		{
			return parent;
		}
		return parent.drawEventParent;
	}
	return nil;
}
@synthesize msgID, index, endTime, document, isSecondaryReference, originalData, isFake, parent, isValid;
@end

