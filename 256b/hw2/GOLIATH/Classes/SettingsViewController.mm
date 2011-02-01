    //
//  SettingsViewController.m
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "SettingsViewController.h"


@implementation SettingsViewController

@synthesize delegate;
@synthesize state;

 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
/*
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization.
    }
    return self;
}
*/

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
    
    dampingSlider.value = state.damping;
    reverbSlider.value = state.reverb;
    shakeSwitch.on = state.shake;
    gravitySwitch.on = state.gravity;
}

- (IBAction)done:(id)sender {
	[self.delegate settingsViewControllerDidFinish:self];	
}

- (IBAction)setDamping:(UISlider*)sender
{
    [self.delegate setDamping:sender.value];
}

- (IBAction)setReverb:(UISlider*)sender
{
    [self.delegate setReverb:sender.value];
}

- (IBAction)setShake:(UISwitch*)sender
{
    [self.delegate setShake:sender.on];
}

- (IBAction)setGravity:(UISwitch*)sender
{
    [self.delegate setGravity:sender.on];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}


- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}


- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
