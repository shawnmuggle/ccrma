//
//  MatrixEvents.h
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Event.h"

@protocol MatrixManipulator
- (void)modifyMatrix:(float *)inMatrix;
@end

@interface MatrixModeEvent : Event {
	struct MatrixModePacked *data;
}
@property uint32_t mode;
@end

@interface LoadIdentityEvent : Event<MatrixManipulator> {
}
@end

@interface TranslatefEvent : Event<MatrixManipulator> {
	struct TranslatefPacked *data;
}
@property float x, y, z;
@end

@interface OrthoEvent : Event<MatrixManipulator> {
	struct OrthoPacked *data;
}
@property float left,right, bottom, top, zNear, zFar;
@end

@interface PerspectiveEvent : Event<MatrixManipulator> {
	struct PerspectivePacked *data;
}
@property float fovy, aspect, zNear, zFar;
@end