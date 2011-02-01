//
//  GOLIATHAppDelegate.m
//  GOLIATH
//
//  Created by Michael Rotondo on 1/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "GOAppDelegate.h"
#import "EAGLView.h"

@implementation GOAppDelegate

@synthesize window;
@synthesize view_controller;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
    [self.window addSubview:view_controller.view];
    [self.window makeKeyAndVisible];
    
    [(EAGLView*) view_controller.view startAnimation];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [(EAGLView*) view_controller.view stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [(EAGLView*) view_controller.view startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [(EAGLView*) view_controller.view stopAnimation];
}

- (void)dealloc
{
    [window release];
    [view_controller.view release];

    [super dealloc];
}

@end
