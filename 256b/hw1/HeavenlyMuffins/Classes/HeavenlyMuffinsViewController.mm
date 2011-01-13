//
//  HeavenlyMuffinsViewController.m
//  HeavenlyMuffins
//
//  Created by Michael Rotondo on 1/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HeavenlyMuffinsViewController.h"
#import "mo_audio.h"
#import "mo_accel.h"
#import "mo_touch.h"

#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2

// Sine parameters
float g_freq = 220;
float g_freq_offset = 0;
float g_freq_offset_target = 0;
float g_freq_offset_rate = 0.1;
float g_phase = 0;
int g_synth_style = 0;

// FM parameters
float g_c_freq = 440;
float g_c_freq_offset = 0;
float g_c_phase = 0;
float g_m_freq = 202; // TODO: Change to be calculated on the fly by a ratio
float g_m_freq_offset = 0;
float g_m_phase = 0;
float g_m_index = 100;
float g_fm_amplitude = 1.0;
bool g_fm_triggering = NO;

// Delay parameters
float g_feedback_coefficient = 1.0;


void sineCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    g_freq_offset += (g_freq_offset_target - g_freq_offset) * g_freq_offset_rate;
    
    float step = (g_freq + g_freq_offset) / SRATE;
    for ( int i = 0; i < frameSize; i++ )
    {
        g_phase += step;
        buffer[i * 2] = buffer[i * 2 + 1] = sin(g_phase * 2 * M_PI);
        if (g_phase > 1) {
            g_phase -= 1;
        }
    }   
}

void fmCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    
    for ( int i = 0; i < frameSize; i++ )
    {
        float m_step = (g_m_freq + g_m_freq_offset) / SRATE;
        if (m_step < 0)
            m_step = 0;
        g_m_phase += m_step;
        
        if (g_m_phase > 1) {
            g_m_phase -= 1;
        }
        
        float c_freq_offset = sin(g_m_phase * 2 * M_PI) * g_m_index;
        
        float c_step = (g_c_freq + c_freq_offset + g_c_freq_offset) / SRATE;
        
        g_c_phase += c_step;
        buffer[i * 2] = buffer[i * 2 + 1] = sin(g_c_phase * 2 * M_PI) * g_fm_amplitude;
        if (g_fm_triggering)
            g_fm_amplitude *= 0.9994;
        
        if (g_c_phase > 1) {
            g_c_phase -= 1;
        }
    }   
}

void delayCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    DelayLine* delay_line = (DelayLine*)userData;
    for ( int i = 0; i < frameSize; i++ )
    {
        buffer[i * 2] = buffer[i * 2 + 1] = [delay_line writeAndRead:buffer[i * 2] * g_feedback_coefficient];
    }   
}

// the audio callback
void audioCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    switch (g_synth_style) {
        case 0:
            sineCallback(buffer, frameSize, userData);
            break;
        case 1:
            fmCallback(buffer, frameSize, userData);
            break;
        case 2:
            delayCallback(buffer, frameSize, userData);
            break;
        default:
            break;
    }
}

void accelCallback( double x, double y, double z, void * data )
{
	switch (g_synth_style) {
        case 0:
            g_freq_offset_target = x * 100;
            break;
        case 1:
            g_m_freq_offset = x * 200;
            g_c_freq_offset = y * 200;
            break;
        case 2:
            break;
        default:
            break;
    }
}

void touchCallback( NSSet * touches, UIView * view, const std::vector<MoTouchTrack> & touchPts, void * data)
{
    if (g_synth_style == 2) {
        HeavenlyMuffinsViewController * view_controller = (HeavenlyMuffinsViewController *)data;
        long count = [touches count];
        // If there's one touch
        if(count==2)
        {
            view_controller.delay_line.delay_length = fabs(([touchPts[0].touch locationInView:view_controller.view].x - 
                                                            [touchPts[1].touch locationInView:view_controller.view].x) * 200);
            g_feedback_coefficient = fabs(([touchPts[0].touch locationInView:view_controller.view].y - 
                                           [touchPts[1].touch locationInView:view_controller.view].y)) / view_controller.view.bounds.size.height;
        }    
        view_controller.delay_length_label.text = [NSString stringWithFormat:@"%.1fs", (float)view_controller.delay_line.delay_length / SRATE];
        view_controller.feedback_coefficient_label.text = [NSString stringWithFormat:@"%.1f%", g_feedback_coefficient];
    }
}

@implementation HeavenlyMuffinsViewController

@synthesize delay_line;
@synthesize delay_length_label;
@synthesize feedback_coefficient_label;

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSLog(@"HELLO");
    
    // log
    NSLog( @"starting real-time audio..." );
    
    delay_line = [[DelayLine alloc] initWithMaxDelay:SRATE * 5 andDelay: SRATE];
    
    self.delay_length_label.text = [NSString stringWithFormat:@"%.1fs", (float)self.delay_line.delay_length / SRATE];
    self.feedback_coefficient_label.text = [NSString stringWithFormat:@"%.1f%", g_feedback_coefficient];
    
    MoAccel::addCallback( accelCallback, NULL);
    MoTouch::addCallback( touchCallback, self);
    
    // init the audio layer
    bool result = MoAudio::init( SRATE, 64, 2 );
    if( !result )
    {
        // something went wrong
        NSLog( @"cannot initialize real-time audio!" );
        // bail out
        return;
    }
    
    // start the audio layer, registering a callback method
    result = MoAudio::start( audioCallback, delay_line );
    if( !result )
    {
        // something went wrong
        NSLog( @"cannot start real-time audio!" );
        // bail out
        return;
    }
    
    [self toggleFmControlsHidden:YES];
    [self toggleDelayLabelsHidden:YES];
}

- (IBAction) synthStyleSwitched:(UISegmentedControl*)sender
{
    g_synth_style = sender.selectedSegmentIndex;
    [self toggleFmControlsHidden:sender.selectedSegmentIndex != 1];
    [self toggleDelayLabelsHidden:sender.selectedSegmentIndex != 2];
}

- (void) toggleDelayLabelsHidden:(bool)hidden
{
    delay_label.hidden = delay_length_label.hidden = feedback_coefficient_label.hidden = hidden;
}

- (void) toggleFmControlsHidden:(bool)hidden
{
     mod_index_label.hidden = mod_index_slider.hidden = fm_trigger_label.hidden = fm_trigger_switch.hidden = fm_trigger_button.hidden = hidden;
}

- (IBAction) fmIndexChanged:(UISlider*)sender
{
    g_m_index = sender.value;
}

- (IBAction) toggleFmTriggering:(UISwitch*)sender
{
    if (sender.on) {
        g_fm_amplitude = 0.0;
        g_fm_triggering = YES;
    }
    else {
        g_fm_amplitude = 1.0;
        g_fm_triggering = NO;
    }
}

- (IBAction) triggerFmNote:(UIButton*)sender
{
    g_fm_amplitude = 1.0;
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}

@end
