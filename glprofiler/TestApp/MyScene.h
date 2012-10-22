//
//  MyScene.h
//
//  Created by handley on 7/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class MyController;

@interface MyScene : NSObject {
    IBOutlet MyController *controller;
	NSRect viewportRect;
	
	CFAbsoluteTime accTime;
	
	GLuint vertexVBO;
	
	NSPoint mouseLoc;
}
- (void)advanceTimeBy:(CFTimeInterval)dt;
- (void)render;

@property NSRect viewportRect;
@property NSPoint mouseLoc;
@end
