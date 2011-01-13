//
//  HeavenlyMuffinsAppDelegate.h
//  HeavenlyMuffins
//
//  Created by Michael Rotondo on 1/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class HeavenlyMuffinsViewController;

@interface HeavenlyMuffinsAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    HeavenlyMuffinsViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet HeavenlyMuffinsViewController *viewController;

@end

