//
//  AKTouchTracker.m
//  Artikulator
//
//  Created by Luke Iannini on 6/5/10.
//  Copyright 2010 Hello, Chair Inc. All rights reserved.
//

#import "AKTouchTracker.h"

@implementation AKTouchTracker
@synthesize touches;

- (id)init
{
	self = [super init];
	if (self != nil)
	{
		self.touches = CFDictionaryCreateMutable(NULL,11,NULL,NULL);
	}
	return self;
}

- (void)dealloc
{
    for (id object in [(NSDictionary *)self.touches allValues])
    {
        [object release];
    }
	CFRelease(self.touches);
	[super dealloc];
}

- (void)removeObjectForTouch:(UITouch *)aTouch
{
    [[self objectForTouch:aTouch] release];
    CFDictionaryRemoveValue(self.touches, aTouch);
}

- (void)setObject:(id)anObject forTouch:(UITouch *)aTouch
{
    id currentObject = [self objectForTouch:aTouch];
    if (anObject != currentObject)
    {
        [currentObject release];
        CFDictionarySetValue(self.touches, aTouch, [anObject retain]);
    }
}

- (id)objectForTouch:(UITouch *)aTouch
{
    return (id)CFDictionaryGetValue(self.touches, aTouch);
}


- (void)removeAllObjects
{
    for (id object in [(NSDictionary *)self.touches allValues])
    {
        [object release];
    }
    CFDictionaryRemoveAllValues(self.touches);
}

- (NSArray *)allTouches
{
    return [(NSDictionary *)self.touches allKeys];
}

- (int)count
{
    return [(NSDictionary *)self.touches count];
}


@end
