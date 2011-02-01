//
//  GOLIATHAppDelegate.h
//  GOLIATH
//
//  Created by Michael Rotondo on 1/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "MainViewController.h"

@class EAGLView;

@interface GOAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    MainViewController* view_controller;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet MainViewController* view_controller;

@end

