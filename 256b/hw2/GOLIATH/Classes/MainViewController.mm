    //
//  MainViewController.m
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "MainViewController.h"
#import "EAGLView.h"

@implementation MainViewController

- (void)settingsViewControllerDidFinish:(SettingsViewController *)controller {    
	[self dismissModalViewControllerAnimated:YES];
}

- (IBAction)showInfo:(id)sender {    
	
	SettingsViewController *controller = [[SettingsViewController alloc] initWithNibName:@"SettingsViewController" bundle:nil];
	controller.delegate = self;
    controller.state = ((EAGLView*) self.view).state;
	
	controller.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
	[self presentModalViewController:controller animated:YES];
	
	[controller release];
}

- (void)setDamping:(float)damping
{
    ((EAGLView*) self.view).state.damping = damping;
}

- (void)setReverb:(float)reverb
{
    ((EAGLView*) self.view).state.reverb = reverb;
}

- (void)setShake:(BOOL)shake
{
    ((EAGLView*) self.view).state.shake = shake;
}

- (void)setGravity:(BOOL)gravity
{
    ((EAGLView*) self.view).state.gravity = gravity;
}


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

    NSLog(@"HEEEEEYYYY");
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return NO;
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
