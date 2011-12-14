//
//  SUTimeline.h
//  Sundry
//
//  Created by Michael Rotondo on 12/14/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef struct SUTimeEvent_ {
    float time;
    float value;
} SUTimeEvent;

@interface SUTimeline : NSObject

- (id)initWithLength:(float)length;
- (void)addEvent:(float)value atTime:(float)time;
- (SUTimeEvent)lastValueBetweenStartTime:(float)startTime endTime:(float)endTime;

@end
