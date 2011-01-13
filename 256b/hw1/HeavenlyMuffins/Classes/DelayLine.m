//
//  DelayLine.m
//  HeavenlyMuffins
//
//  Created by Michael Rotondo on 1/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "DelayLine.h"


@implementation DelayLine

- (id) initWithMaxDelay:(int)max_delay andDelay:(int)delay
{
    self = [super init];
    if (self) {
        max_delay_length = max_delay;
        delay_length = delay;
        
        delay_line = malloc(sizeof(float) * max_delay_length);
        
        read_index = 0;
        write_index = delay_length;
    }
    return self;
}

- (float) writeAndRead:(float)new_sample
{
    delay_line[write_index++] = new_sample;
    float read_sample = delay_line[read_index++];
    
    if (write_index == max_delay_length)
        write_index = 0;
    if (read_index == max_delay_length)
        read_index = 0;
    
    return read_sample;
}

- (void) dealloc
{
    free(delay_line);
    [super dealloc];
}

@end
