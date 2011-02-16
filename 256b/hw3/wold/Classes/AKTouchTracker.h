//
//  AKTouchTracker.h
//  Artikulator
//
//  Created by Luke Iannini on 6/5/10.
//  Copyright 2010 Hello, Chair Inc. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface AKTouchTracker : NSObject
{
    CFMutableDictionaryRef touches;
}
- (void)removeObjectForTouch:(UITouch *)aTouch;
- (void)setObject:(id)anObject forTouch:(UITouch *)aTouch;
- (id)objectForTouch:(UITouch *)aTouch;
- (void)removeAllObjects;
- (NSArray *)allTouches;
- (int)count;

@property (nonatomic, assign) CFMutableDictionaryRef touches;

@end
