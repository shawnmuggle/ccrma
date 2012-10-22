//
//  VBOEvents.h
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "Event.h"
@class BufferInfoEvent;
@interface BindBufferEvent : Event {
	struct BindBufferPacked *data;
//runtime
	BufferInfoEvent *bufferInfo;
}
@property uint32_t target, buffer;
@end

@interface GenBuffersEvent : Event {
	struct GenDeleteBuffersPacked *data;
	
	NSData *buffers;
}
@property GLsizei count;
@property(retain) NSData *buffers;
@end

@interface DeleteBuffersEvent : Event {
	struct GenDeleteBuffersPacked *data;
	
	NSData *buffers;
}
@property GLsizei count;
@property(retain) NSData *buffers;
@end

@interface BufferSubDataEvent : Event {
	struct BufferSubDataPacked *data;
	NSData *bufferData;
//runtime
	BindBufferEvent *bindBufferEvent;
}
@property uint32_t target, size, offset;
@property(retain) NSData *bufferData;
@property(assign) BindBufferEvent *bindBufferEvent;
@end

@interface BufferDataEvent : Event {
	struct BufferDataPacked *data;
	NSData *bufferData;
//runtime
	BindBufferEvent *bindBufferEvent;
}
@property uint32_t target, size, usage;
@property(retain) NSData *bufferData;
@property(assign) BindBufferEvent *bindBufferEvent;
@end

@interface BufferInfoEvent : Event {
	struct BufferInfoPacked *data;
	NSData *bufferData;
}
@property uint32_t buffer, target, size, usage;
@property(retain) NSData *bufferData;
@end
