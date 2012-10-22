//
//  MyTreeController.h
//
//  Created by handley on 7/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class MyDocument;
@interface EventController : NSArrayController<NSOutlineViewDelegate,NSOutlineViewDataSource> {
}

- (IBAction)capture:(id)sender;
- (IBAction)stopCapturing:(id)sender;
@end
