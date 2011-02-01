//
//  SettingsViewController.h
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GOState.h"

@protocol SettingsViewControllerDelegate;

@interface SettingsViewController : UIViewController {
    id <SettingsViewControllerDelegate> delegate;
    GOState* state;
    
    IBOutlet UISlider* dampingSlider;
    IBOutlet UISlider* reverbSlider;
    IBOutlet UISwitch* shakeSwitch;
    IBOutlet UISwitch* gravitySwitch;
}

@property (nonatomic, assign) id <SettingsViewControllerDelegate> delegate;
@property (nonatomic, assign) GOState* state;

- (IBAction)done:(id)sender;
- (IBAction)setDamping:(id)sender;
- (IBAction)setReverb:(id)sender;
- (IBAction)setShake:(id)sender;
- (IBAction)setGravity:(id)sender;
@end

@protocol SettingsViewControllerDelegate
- (void)settingsViewControllerDidFinish:(SettingsViewController *)controller;
- (void)setDamping:(float)damping;
- (void)setReverb:(float)reverb;
- (void)setShake:(BOOL)shake;
- (void)setGravity:(BOOL)gravity;
@end