//
//  VBOEvents.m
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "VBOEvents.h"
#import "MyDocument.h"

@implementation BindBufferEvent
- (uint32_t)target
{
	return data->target;
}
- (void)setTarget:(uint32_t)target
{
	data->target = target;
}
- (uint32_t)buffer
{
	return data->buffer;
}
- (void)setBuffer:(uint32_t)buffer
{
	data->buffer = buffer;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Bind %s Buffer: %u",self.target == GL_ARRAY_BUFFER ? "Vertex" : "Index", self.buffer];
}
- (void)drawRect:(NSRect)rect
{
	GLuint displayBuffer = [document displayBufferWithID:self.buffer];
	glBindBuffer(self.target, displayBuffer);
}
- (BOOL)mightCacheEvent:(Event *)e
{
	if (e.msgID == etINFO_BUFFER)
	{
		BufferInfoEvent *b = (BufferInfoEvent *)e;
		if (self.target == b.target && self.buffer == b.buffer)
		{
			bufferInfo = b;
			return YES;
		}
	}
	return NO;
}
- (enum EventValidationErrors)validate:(NSError **)error
{
	if (bufferInfo)
		return E_None;
	
	if (error)
	{
		*error = nil;
	}
	
	return E_MissingData; //missing data, unrecoverable error here
}
@end

@implementation GenBuffersEvent
- (id)initWithID:(uint32_t)msgid data:(NSArray *)inData
{
	if (self = [super initWithID:msgid data:inData])
	{
		assert([inData count] == 2);
		self.buffers = [inData objectAtIndex:1];
	}
	return self;
}
- (GLsizei)count
{
	return data->count;
}
- (void)setCount:(GLsizei)count
{
	data->count = count;
}
- (NSString *)_name
{
	if (self.count == 1)
	{
		return [NSString stringWithFormat:@"Gen Buffers: (ID %u)",*(GLuint *)(self.buffers.bytes)];
	}
	else
	{
		return [NSString stringWithFormat:@"Gen Buffers: %u",self.count];
	}
}
- (void)drawRect:(NSRect)rect
{
	//genbuffers just makes sure that the buffers exist
	for (uint32_t i=0;i<self.count;++i)
	{
		uint32_t bufferID;
		[self.buffers getBytes:&bufferID range:NSMakeRange(sizeof(uint32_t)*i,sizeof(uint32_t))];
		[document bufferInfo:bufferID];
	}
}
@synthesize buffers;
@end

@implementation DeleteBuffersEvent
- (id)initWithID:(uint32_t)msgid data:(NSArray *)inData
{
	if (self = [super initWithID:msgid data:inData])
	{
		assert([inData count] == 2);
		self.buffers = [inData objectAtIndex:1];
	}
	return self;
}
- (GLsizei)count
{
	return data->count;
}
- (void)setCount:(GLsizei)count
{
	data->count = count;
}
- (NSString *)_name
{
	if (self.count == 1)
	{
		return [NSString stringWithFormat:@"Delete Buffers: (ID %u)",*(GLuint *)(self.buffers.bytes)];
	}
	else
	{
		return [NSString stringWithFormat:@"Delete Buffers: %u",self.count];
	}
}
- (void)drawRect:(NSRect)rect
{
	//we never actually delete any buffers, as we don't want to re-create them every frame.
}
@synthesize buffers;
@end

@implementation BufferDataEvent
- (id)initWithID:(uint32_t)msgid data:(NSArray *)inData
{
	if (self = [super initWithID:msgid data:inData])
	{
		assert([inData count] == 2);
		self.bufferData = [inData objectAtIndex:1];
	}
	return self;
}

- (uint32_t)target
{
	return data->target;
}
- (void)setTarget:(uint32_t)target
{
	data->target = target;
}
- (uint32_t)size
{
	return data->size;
}
- (void)setSize:(uint32_t)size
{
	data->size = size;
}
- (uint32_t)usage
{
	return data->usage;
}
- (void)setUsage:(uint32_t)usage
{
	data->usage = usage;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"%s Buffer Data: %u bytes%@",self.target == GL_ARRAY_BUFFER ? "Vertex" : "Index", self.size, self.bindBufferEvent ? 
		[NSString stringWithFormat:@" (→ %u)", self.bindBufferEvent.buffer] : @""];
}
- (void)drawRect:(NSRect)rect
{
	glBufferData(self.target, self.size, self.bufferData.bytes, self.usage);
}
- (BOOL)mightCacheEvent:(Event *)e
{
	if (e.msgID == etBIND_BUFFER)
	{
		BindBufferEvent *be = (BindBufferEvent *)e;
		if (be.target == self.target)
		{
			self.bindBufferEvent = be;
			return YES;
		}
	}
	return NO;
}
- (enum EventValidationErrors)validate:(NSError **)error
{
	if (self.bindBufferEvent)
		return E_None;
	
	if (error)
	{
		*error = nil;
	}
	
	return E_MissingData;
}
@synthesize bufferData, bindBufferEvent;
@end

@implementation BufferSubDataEvent
- (id)initWithID:(uint32_t)msgid data:(NSArray *)inData
{
	if (self = [super initWithID:msgid data:inData])
	{
		assert([inData count] == 2);
		self.bufferData = [inData objectAtIndex:1];
	}
	return self;
}

- (uint32_t)target
{
	return data->target;
}
- (void)setTarget:(uint32_t)target
{
	data->target = target;
}
- (uint32_t)size
{
	return data->size;
}
- (void)setSize:(uint32_t)size
{
	data->size = size;
}
- (uint32_t)offset
{
	return data->offset;
}
- (void)setOffset:(uint32_t)offset
{
	data->offset = offset;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"%s Buffer Sub Data: %u bytes @0x%X %@",self.target == GL_ARRAY_BUFFER ? "Vertex" : "Index", self.size, self.offset, self.bindBufferEvent ? 
		[NSString stringWithFormat:@" (→ %u)", self.bindBufferEvent.buffer] : @""];
}
- (void)drawRect:(NSRect)rect
{
	glBufferSubData(self.target, self.offset, self.size, [self.bufferData bytes]);
}
- (BOOL)mightCacheEvent:(Event *)e
{
	if (e.msgID == etBIND_BUFFER)
	{
		BindBufferEvent *be = (BindBufferEvent *)e;
		if (be.target == self.target)
		{
			self.bindBufferEvent = be;
			return YES;
		}
	}
	return NO;
}
@synthesize bufferData, bindBufferEvent;
@end

@implementation BufferInfoEvent
- (uint32_t)target
{
	return data->header.target;
}
- (void)setTarget:(uint32_t)target
{
	data->header.target = target;
}
- (uint32_t)size
{
	return data->header.size;
}
- (void)setSize:(uint32_t)size
{
	data->header.size = size;
}
- (uint32_t)usage
{
	return data->header.usage;
}
- (void)setUsage:(uint32_t)usage
{
	data->header.usage = usage;
}
- (uint32_t)buffer
{
	return data->buffer;
}
- (void)setBuffer:(uint32_t)buffer
{
	data->buffer = buffer;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"%s Buffer Info: ID %u, %u bytes",self.target == GL_ARRAY_BUFFER ? "Vertex" : "Index", self.buffer, self.size];
}
@synthesize bufferData;
@end
