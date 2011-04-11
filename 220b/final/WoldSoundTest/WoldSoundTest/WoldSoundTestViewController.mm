//
//  WoldSoundTestViewController.m
//  WoldSoundTest
//
//  Created by Michael Rotondo on 3/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WoldSoundTestViewController.h"

#import "SoundGrain.h"

#import "mo_audio.h"

#import "FileRead.h"
#import "Stk.h"

#import <string>

#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2

stk::StkFrames data;

SoundGrain* grain;

// TODO: Make this actually have an effect
float density = 1.0;

// TODO: Paramaterize length, attack and release of grains
// TODO: Add polyphony (and make density use it)

void audioCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    for ( int i = 0; i < frameSize; i++ )
    {
        Float32 sample = [grain tick];
        
        buffer[i * 2] = buffer[i * 2 + 1] = sample;
        
        if ([grain done]) {
            // TODO: Make this a property to handle releasing
            grain = [[SoundGrain alloc] initWithRandomParamsAndFrames:&data];
        }
    } 
}

@implementation WoldSoundTestViewController

- (void)dealloc
{
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"potato_hell" ofType:@"wav"];
    std::string fileName = [filePath UTF8String];
    bool typeRaw = false;
    
    // Attempt to load the soundfile data.
    stk::FileRead file( fileName, typeRaw );
    data.resize( file.fileSize(), file.channels() );
    NSLog(@"File size: %lu", file.fileSize());
    file.read( data );
    //lastFrame_.resize( 1, file.channels(), 0.0 );
    
    grain = [[SoundGrain alloc] initWithRandomParamsAndFrames:&data];
    
    // init the audio layer
    bool result = MoAudio::init( SRATE, FRAMESIZE, NUMCHANNELS );
    if( !result )
    {
        // something went wrong
        NSLog( @"cannot initialize real-time audio!" );
        // bail out
        return;
    }
    
    // start the audio layer, registering a callback method
    result = MoAudio::start( audioCallback, NULL);
    if( !result )
    {
        // something went wrong
        NSLog( @"cannot start real-time audio!" );
        // bail out
        return;
    }
}


- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return YES;
}

@end
