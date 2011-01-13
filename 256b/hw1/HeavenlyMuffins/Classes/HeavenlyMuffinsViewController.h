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
    
}
- (IBAction) synthStyleSwitched:(UISegmentedControl*)sender;
@end

