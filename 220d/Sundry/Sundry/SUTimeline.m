//
//  SUTimeline.m
//  Sundry
//
//  Created by Michael Rotondo on 12/14/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUTimeline.h"

@interface SUTimeline ()
{
    float length;
    NSMutableArray *events; //Contains NSValues which contain TimeEvents
}
@end

@implementation SUTimeline

- (id)initWithLength:(float)inLength
{
    self = [super init];
    if (self) {
        length = inLength;
        events = [NSMutableArray array];
    }
    return self;
}

- (void)addEvent:(float)value atTime:(float)time
{
    SUTimeEvent newEvent;
    newEvent.time = time;
    newEvent.value = value;
    NSValue *newEventValue = [NSValue valueWithBytes:&newEvent objCType:@encode(SUTimeEvent)];

    if (![events count])
    {
        // No events in the array yet, so just add the new one.
        [events addObject:newEventValue];
    }
    else
    {
        // Keep the events in order by time
        int i = 0;
        for (NSValue *timeEventValue in events)
        {
            SUTimeEvent event;
            [timeEventValue getValue:&event];
            if (event.time > newEvent.time)
            {
                // Found an event later than the new one, so add the new one before it.
                [events insertObject:newEventValue atIndex:i];
                break;
            }
            i++;
        }
        if (i == [events count])
        {
            // There are events in the array but the new event is later than all of them, so add it to the end.
            [events addObject:newEventValue];
        }
    }
}

- (SUTimeEvent)lastValueBetweenStartTime:(float)startTime endTime:(float)endTime
{
    while (startTime > length) 
    {
        startTime -= length;
    }
    while (endTime > length)
    {
        endTime -= length;
    }
    
    SUTimeEvent ret;
    ret.time = -1;
    ret.value = -1;
    BOOL foundAnything = NO;
    for (NSValue *timeEventValue in events)
    {
        SUTimeEvent event;
        [timeEventValue getValue:&event];
        // if startTime < endTime, look between them
        // if endTime < startTime, look outside them (we're wrapped around the end of the loop)
        if ((startTime <= endTime && event.time >= startTime && event.time <= endTime)||
            (endTime < startTime && (event.time <= endTime || event.time >= startTime)))
        {
            ret = event;
            foundAnything = YES;
        }
        
        if (endTime < startTime)
        {
            // If endTime < startTime, then only stop if we found something before endTime, because anything after startTime will be earlier.
            if (foundAnything && event.time > endTime)
            {
                break;
            }
        }
        else if (event.time > endTime)
        {
            // If startTime <= endTime, then stop as soon as we get past endTime
            break;
        }
    }
    return ret;
}

@end