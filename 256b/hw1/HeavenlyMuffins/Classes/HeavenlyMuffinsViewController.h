//
//  HeavenlyMuffinsViewController.h
//  HeavenlyMuffins
//
//  Created by Michael Rotondo on 1/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DelayLine.h"

@interface HeavenlyMuffinsViewController : UIViewController {

    DelayLine* delay_line;
    
    IBOutlet UILabel* mod_index_label;
    IBOutlet UISlider* mod_index_slider;
    IBOutlet UILabel* fm_trigger_label;
    IBOutlet UISwitch* fm_trigger_switch;
    IBOutlet UIButton* fm_trigger_button;
    
    IBOutlet UILabel* delay_label;
    IBOutlet UILabel* delay_length_label;
    IBOutlet UILabel* feedback_coefficient_label;
}
- (IBAction) synthStyleSwitched:(UISegmentedControl*)sender;
- (IBAction) fmIndexChanged:(UISlider*)sender;
- (IBAction) toggleFmTriggering:(UISwitch*)sender;
- (IBAction) triggerFmNote:(UIButton*)sender;
- (void) toggleFmControlsHidden:(bool)hidden;
- (void) toggleDelayLabelsHidden:(bool)hidden;

@property (nonatomic, assign) DelayLine* delay_line;
@property (nonatomic, assign) UILabel* delay_length_label;
@property (nonatomic, assign) UILabel* feedback_coefficient_label;
@end

