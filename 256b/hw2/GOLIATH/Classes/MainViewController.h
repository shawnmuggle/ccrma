//
//  MainViewController.h
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SettingsViewController.h"

@interface MainViewController : UIViewController <SettingsViewControllerDelegate> {
}

- (IBAction)showInfo:(id)sender;
- (void)setDamping:(float)damping;
- (void)setReverb:(float)reverb;
- (void)setShake:(BOOL)shake;
- (void)setGravity:(BOOL)gravity;

@end
