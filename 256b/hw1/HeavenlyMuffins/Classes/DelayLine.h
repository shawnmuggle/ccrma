//
//  DelayLine.h
//  HeavenlyMuffins
//
//  Created by Michael Rotondo on 1/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface DelayLine : NSObject {
    float* delay_line;
    int max_delay_length;
    int delay_length;
    int read_index;
    int write_index;
}
- (id) initWithMaxDelay:(int)max_delay andDelay:(int)delay;
- (void) setNewDelay:(int)delay;
- (float) writeAndRead:(float)new_sample;

@property (setter=setDelayLength) int delay_length;

@end
