/*
 *  connection.c
 *  GLProfiler
 *
 *  Created by handley on 7/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "connection.h"
#include "events.h"

NSPort *remotePort, *localPort;
int shouldCapture = 0;
int shouldDelayDeletions = 0;

struct
{
	uint32_t n;
	GLuint *buffers;
} deleteBufferEvent[1024];
uint32_t nextDeleteBuffer;

GLuint idBuffer[1024], *currentIDBuffer = idBuffer;

void AddDeleteBuffer(uint32_t count, const GLuint *buffers)
{
	deleteBufferEvent[nextDeleteBuffer].n = count;
	deleteBufferEvent[nextDeleteBuffer].buffers = currentIDBuffer;
	memcpy(currentIDBuffer,buffers,sizeof(GLuint) * count);
	currentIDBuffer += count;
}

void Finish()
{
	//do any finishing events...
	for (uint32_t i=0;i<nextDeleteBuffer;++i)
	{
		glDeleteBuffers(deleteBufferEvent[i].n, deleteBufferEvent[i].buffers);
	}
	
	shouldDelayDeletions = 0;
}

@interface PortMessageDelegate : NSObject<NSPortDelegate> {

}
@end

@implementation PortMessageDelegate
- (void)handlePortMessage:(NSPortMessage *)m
{
	switch ([m msgid])
	{
		case etCAPTURE:
			shouldCapture = 1;
			shouldDelayDeletions = 1;
			remotePort = [[m sendPort] retain];
			SendData(0, 0, etCAPTURE);
			break;
		case etFINISHED_CAPTURING:
			Finish();
			break;
		default:
			NSLog(@"Unknown message recieved by client: %@",m);
	}
}
@end

PortMessageDelegate *messageDelegate = 0;

int InitProfilerConnection()
{
	if (!localPort)
	{
		localPort = [[NSMachPort alloc] init];
		if ([[NSMachBootstrapServer sharedInstance] registerPort:localPort name:@"glprofiler"])
		{
			//we're good!
			NSLog(@"Successfully registered port!");
			messageDelegate = [[PortMessageDelegate alloc] init];
			[localPort setDelegate:messageDelegate];
			[[NSRunLoop mainRunLoop] addPort:localPort forMode:NSDefaultRunLoopMode];
		}
		else
		{
			NSLog(@"FAILED TO REGISTER PORT!");
		}
	}
	
	return localPort && shouldCapture;
}

void CloseProfilerConnection()
{
	shouldCapture = 0;
	NSLog(@"Closed profiler connection");
	if (remotePort)
	{
		SendData(0, 0, etSTOP_CAPTURE);
		
		[[NSRunLoop mainRunLoop] removePort:localPort forMode:NSDefaultRunLoopMode];
		[localPort invalidate];
		[localPort release];
		localPort = 0;
		[remotePort invalidate];
		[remotePort release];
		remotePort = 0;
	}
}

uint32_t SendData(const void *data, unsigned int size, unsigned int msgID)
{
	if (remotePort)
	{
		NSArray *payload;
		if (data && size)
		{
			NSData *wrappedData = [NSData dataWithBytesNoCopy:(void *)data length:size freeWhenDone:NO];
			payload = [NSArray arrayWithObject:wrappedData];
		}
		else
		{
			payload = [NSArray array];
		}
		NSPortMessage *message = [[[NSPortMessage alloc] initWithSendPort:remotePort receivePort:localPort components:payload] autorelease];
		[message setMsgid:msgID];
		@try {
			[message sendBeforeDate:[NSDate distantFuture]];
		}
		@catch (NSException *e) {
			NSLog(@"Profiler app seems to have crashed... giving up sending any data.");
			//the profiler closed down... stop sending events!
			[localPort invalidate];
			[localPort release];
			localPort = 0;
			shouldCapture = 0;
			
			return false;
		}
		
		return true;
	}
	return false;
}

uint32_t SendDataWithExtra(const void *data, unsigned int size, unsigned int msgID, const void *extraData, uint32_t extraDataSize)
{
	if (remotePort)
	{
		NSArray *payload;
		if (data && size)
		{
			NSData *wrappedData = [NSData dataWithBytesNoCopy:(void *)data length:size freeWhenDone:NO];
			if (extraData && extraDataSize)
			{
				NSData *moreWrappedData = [NSData dataWithBytesNoCopy:(void *)extraData length:extraDataSize freeWhenDone:NO];
				payload = [NSArray arrayWithObjects:wrappedData, moreWrappedData, nil];
			}
			else
			{
				payload = [NSArray arrayWithObject:wrappedData];
			}

		}
		else
		{
			payload = [NSArray array];
		}
		NSPortMessage *message = [[[NSPortMessage alloc] initWithSendPort:remotePort receivePort:localPort components:payload] autorelease];
		[message setMsgid:msgID];
		[message sendBeforeDate:[NSDate distantFuture]];
		
		return true;
	}
	return false;

}