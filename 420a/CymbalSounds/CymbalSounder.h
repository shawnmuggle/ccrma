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
- (IBAction)setSize:(id)sender;
- (IBAction)setAttackFalloff:(id)sender;

- (IBAction)setLeftPole:(id)sender;
- (IBAction)setLeftDamping:(id)sender;
- (IBAction)setLeftA1:(id)sender;
- (IBAction)setLeftA2:(id)sender;

- (IBAction)setRightPole:(id)sender;
- (IBAction)setRightDamping:(id)sender;
- (IBAction)setRightA1:(id)sender;
- (IBAction)setRightA2:(id)sender;

@end
