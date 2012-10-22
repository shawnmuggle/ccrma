//
//  ProfileOpenGLView.m
//
//  Created by handley on 8/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ProfileOpenGLView.h"
#import "Event.h"
#import "MetaEvents.h"
#import <OpenGL/CGLRenderers.h>

@interface ProfileOpenGLView()
- (void)reinit;
@end

@implementation ProfileOpenGLView
+ (void)initialize
{
	[self exposeBinding:@"cachedEvents"];
}
- (void)awakeFromNib
{
	[self bind:@"cachedEvents" toObject:controller withKeyPath:@"selectedObjects" options:[NSDictionary dictionary]];
}
- (void)drawRect:(NSRect)dirtyRect
{
	//rebuild the context, if necessary
	[[self openGLContext] makeCurrentContext];
	
	if (cachedEvents)
	{
		dirtyRect = [self bounds];
		glDisable(GL_SCISSOR_TEST);
		glViewport(0,0,(GLsizei)dirtyRect.size.width,(GLsizei)dirtyRect.size.height);
		glClearColor(.6f,.6f,.6f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (Event *e in cachedEvents)
		{
			//[e displayInRect:dirtyRect];
		}
	}
	else
	{
		glClearColor(1,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	[[self openGLContext] flushBuffer];
}

- (void)reinit
{
	//assume that we have our surface event
	assert(self.cachedSurfaceEvent);
	
	NSOpenGLPixelFormatAttribute attributes[] = {
		NSOpenGLPFAAllRenderers,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, self.cachedSurfaceEvent.color,
		NSOpenGLPFADepthSize, self.cachedSurfaceEvent.depth,
		NSOpenGLPFAStencilSize, self.cachedSurfaceEvent.stencil,
		0
	};
	
	NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
	
	assert(format);
	
	[[self openGLContext] clearDrawable];

	NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:format shareContext:0];
	[context setView:self];
	[self setPixelFormat:format];
	[self setOpenGLContext:context];
	[context update];
	[context release];
}

- (void)reshape
{
	[self setNeedsDisplay:YES];
}

- (BOOL)isOpaque
{
	return YES;
}

- (void)setCachedEvents:(NSArray *)ce
{
	if (ce != cachedEvents)
	{
		[cachedEvents release];
		
		if (ce.count)
		{
			Event *e = ce.lastObject;
			if (e.drawEventParent)
			{
				NSUInteger lastSameIndex = 0;
				for (Event *c in e.drawEventParent.children)
				{
					NSUInteger i = [ce indexOfObjectIdenticalTo:c];
					if (i == NSNotFound)
					{
						break;
					}
					lastSameIndex++;
				}
				
				//now, at this point, we know that all of the objects from i to the end of the drawelements array needs to be appended to ce and assigned to cachedEvents
				cachedEvents = [[[ce arrayByAddingObjectsFromArray:[e.drawEventParent.children subarrayWithRange:NSMakeRange(lastSameIndex,e.drawEventParent.children.count - lastSameIndex)]] arrayByAddingObject:e.drawEventParent] retain];
				
			}
			else
			{
				cachedEvents = [ce retain];
			}
		}
		else
		{
			cachedEvents = [ce retain];
		}

		
		for (Event *e in cachedEvents)
		{
			if ([e isKindOfClass:[SurfaceEvent class]])
			{
				self.cachedSurfaceEvent = (SurfaceEvent *)e;
				break;
			}
		}
		
		[self setNeedsDisplay:YES];
	}
}
- (void)setCachedSurfaceEvent:(SurfaceEvent *)se
{
	if (se != cachedSurfaceEvent)
	{
		[cachedSurfaceEvent release];
		cachedSurfaceEvent = [se retain];
		
		[self reinit];
	}
}

@synthesize cachedEvents, cachedSurfaceEvent, needsInit;
@end
