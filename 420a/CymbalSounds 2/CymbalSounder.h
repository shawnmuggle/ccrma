//
//  CymbalSounder.h
//  CymbalSounds
//
//  Created by Michael Rotondo on 2/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "pthread.h"

@interface CymbalSounder : NSObject {

    pthread_t rtaudio_thread;
    
}
- (IBAction)playCymbal:(id)sender;
- (IBAction)setAttackFalloff:(id)sender;
- (IBAction)tick:(id)sender;

@end
