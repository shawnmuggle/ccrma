//
//  HeavenlyMuffinsViewController.m
//  HeavenlyMuffins
//
//  Created by Michael Rotondo on 1/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HeavenlyMuffinsViewController.h"
#import "mo_audio.h"

#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2

float g_freq = 220;
float g_phase = 0;
int g_synth_style = 0;

float g_c_freq = 220;
float g_c_phase = 0;
float g_m_freq = 2; // TODO: Change to be calculated on the fly by a ratio
float g_m_phase = 0;
float g_m_index = 10;


void sineCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    float step = g_freq / SRATE;
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
        float m_step = g_m_freq / SRATE;
        g_m_phase += m_step;
        
        if (g_m_phase > 1) {
            g_m_phase -= 1;
        }
        
        float c_freq_offset = sin(g_m_phase * 2 * M_PI) * g_m_index;
        
        float c_step = (g_c_freq + c_freq_offset) / SRATE;
        
        g_c_phase += c_step;
        buffer[i * 2] = buffer[i * 2 + 1] = sin(g_c_phase * 2 * M_PI);
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
        buffer[i * 2] = buffer[i * 2 + 1] = [delay_line writeAndRead:buffer[i * 2]];
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

@implementation HeavenlyMuffinsViewController



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
}

- (IBAction) synthStyleSwitched:(UISegmentedControl*)sender
{
    g_synth_style = sender.selectedSegmentIndex;
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
