#import "MyOpenGLView.h"
#import "MyController.h"
#import "MyScene.h"
#include "glwrapper.h"

@implementation MyOpenGLView
 
- (NSOpenGLContext*) openGLContext
{
    return openGLContext;
}
 
- (NSOpenGLPixelFormat*) pixelFormat
{
    return pixelFormat;
}
 
- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
    // There is no autorelease pool when this method is called because it will be called from a background thread
    // It's important to create one or you will leak objects
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
	if ([[self window] isVisible])
	{
	    // Make sure we draw to the right context
		[[self openGLContext] makeCurrentContext];
    
		// Update the animation
		CFAbsoluteTime currentTime = CFAbsoluteTimeGetCurrent();
		[[controller scene] advanceTimeBy:(currentTime - [controller renderTime])];
		[controller setRenderTime:currentTime];
		
		[self drawView];
	}
    
    [pool release];
    return kCVReturnSuccess;
}
 
// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(MyOpenGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}
 
- (void) setupDisplayLink
{
    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
    
    // Set the display link for the current renderer
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
}
 
- (id) initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context
{
    NSOpenGLPixelFormatAttribute attribs[] =
    {
        kCGLPFAAccelerated,
        kCGLPFANoRecovery,
        kCGLPFADoubleBuffer,
        kCGLPFAColorSize, 24,
        kCGLPFADepthSize, 16,
        0
    };
    
    pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    
    if (!pixelFormat)
        NSLog(@"No OpenGL pixel format");
    
    // NSOpenGLView does not handle context sharing, so we draw to a custom NSView instead
    openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:context];
    
    if (self = [super initWithFrame:frameRect]) {
        [[self openGLContext] makeCurrentContext];
        
		GLint color,depth,stencil;
		[pixelFormat getValues:&color forAttribute:NSOpenGLPFAColorSize forVirtualScreen:0];
		[pixelFormat getValues:&depth forAttribute:NSOpenGLPFADepthSize forVirtualScreen:0];
		[pixelFormat getValues:&stencil forAttribute:NSOpenGLPFAStencilSize forVirtualScreen:0];

		SurfacePROFILE(color,depth,stencil,[self frame].size.width,[self frame].size.height);
		
        // Synchronize buffer swaps with vertical refresh rate
        GLint swapInt = 1;
        [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 
        
        [self setupDisplayLink];
        
        // Look for changes in view size
        // Note, -reshape will not be called automatically on size changes because NSView does not export it to override 
        [[NSNotificationCenter defaultCenter] addObserver:self 
                                                 selector:@selector(reshape) 
                                                     name:NSViewGlobalFrameDidChangeNotification
                                                   object:self];
		CVDisplayLinkStart(displayLink);
    }
    
    return self;
}
 
- (id) initWithFrame:(NSRect)frameRect
{
    self = [self initWithFrame:frameRect shareContext:nil];
    return self;
}
 
- (void) lockFocus
{
    [super lockFocus];
    if ([[self openGLContext] view] != self)
        [[self openGLContext] setView:self];
}
 
- (void) reshape
{
    // This method will be called on the main thread when resizing, but we may be drawing on a secondary thread through the display link
    // Add a mutex around to avoid the threads accessing the context simultaneously
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    // Delegate to the scene object to update for a change in the view size
    [[controller scene] setViewportRect:[self bounds]];
    [[self openGLContext] update];
    
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}
 
- (void) drawRect:(NSRect)dirtyRect
{
    // Ignore if the display link is still running
    //if (!CVDisplayLinkIsRunning(displayLink))
    //    [self drawView];
}
 
- (void) drawView
{
    // This method will be called on both the main thread (through -drawRect:) and a secondary thread (through the display link rendering loop)
    // Also, when resizing the view, -reshape is called on the main thread, but we may be drawing on a secondary thread
    // Add a mutex around to avoid the threads accessing the context simultaneously
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    // Make sure we draw to the right context
    [[self openGLContext] makeCurrentContext];
    
    // Delegate to the scene object for rendering
    [[controller scene] render];
    [[self openGLContext] flushBuffer];
    
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}
 
- (BOOL) acceptsFirstResponder
{
    // We want this view to be able to receive key events
    return YES;
}
 
- (void)mouseDown:(NSEvent *)theEvent
{
    // Delegate to the controller object for handling mouse events
    [controller scene].mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
}

- (void) dealloc
{
    [openGLContext release];
    [pixelFormat release];
    
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    
    [[NSNotificationCenter defaultCenter] removeObserver:self 
                                                    name:NSViewGlobalFrameDidChangeNotification
                                                  object:self];
    [super dealloc];
}   
 
@end