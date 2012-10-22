#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
 
@class MyController;
 
@interface MyOpenGLView : NSView {
    
    NSOpenGLContext *openGLContext;
    NSOpenGLPixelFormat *pixelFormat;
    
    IBOutlet MyController *controller;
    
    CVDisplayLinkRef displayLink;
}
 
- (id) initWithFrame:(NSRect)frameRect;
- (id) initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context;
 
- (NSOpenGLContext*) openGLContext;
 
- (void) drawView;
@end