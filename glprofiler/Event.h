//
//  Event.h
//
//  Created by handley on 7/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "events.h"

enum EventValidationErrors
{
	E_None,
	E_MissingData,
	E_Other
};

extern NSString *EventErrorMsgType;
extern NSString *EventErrorComponents;


@class MyDocument;
@interface Event : NSObject<NSCopying> {
	uint64_t time;
	uint64_t endTime;
	uint32_t msgID;
	uint32_t index;
	BOOL isFake;
	
	NSArray *originalData;
	
	MyDocument *document;
	Event *parent;
	NSMutableArray *children;
	BOOL isSecondaryReference;
	BOOL isValid;
}
+ (Class)classForID:(uint32_t)msgid;
+ (Event *)eventWithID:(uint32_t)msgid data:(NSArray *)data;
- (id)initWithID:(uint32_t)msgid data:(NSArray *)data;
- (void)endScopeWithEvent:(Event *)event;
- (void)drawRect:(NSRect)rect;
- (void)displayInRect:(NSRect)rect;
- (BOOL)mightCacheEvent:(Event *)e;
- (enum EventValidationErrors)validate:(NSError **)error;

@property uint64_t time,endTime;
@property(readonly) uint32_t msgID;
@property uint32_t index;
@property BOOL isFake;

@property(retain) NSArray *originalData;
@property(assign) MyDocument *document;
@property(readonly) NSMutableArray *children;
@property(readonly) Event *parent;
@property BOOL isSecondaryReference;

@property(readonly) NSAttributedString *name;
@property(readonly) float floatTime, floatEndTime, duration; // for UI
@property(readonly) BOOL isLeaf;

@property(readonly) BOOL isScoped, isEndScope;
@property(readonly) NSString *viewName;
@property(readonly) BOOL isDrawEvent;
@property BOOL isValid;
@property(readonly) Event *drawEventParent;
@end


