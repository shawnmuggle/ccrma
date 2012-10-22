//
//  MyController.h
//
//  Created by handley on 7/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class MyOpenGLView, MyScene;

@interface MyController : NSController {
    IBOutlet MyOpenGLView *openGLView;
    IBOutlet MyScene *scene;
	
	CFAbsoluteTime renderTime;
}

@property(assign) MyScene *scene;
@property CFAbsoluteTime renderTime;
@end
