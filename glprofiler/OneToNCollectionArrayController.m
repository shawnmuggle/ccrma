//
//  OneToNCollectionArrayController.m
//
//  Created by handley on 11/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "OneToNCollectionArrayController.h"

@implementation OneToNCollectionArrayController
- (void)setContent:(id)content
{
	//get the index of the selected object in the event list
	NSArray *selectedEvents = (NSArray *)content;
	if (selectedEvents.count)
	{
		NSArray *allEvents = arrayController.arrangedObjects;		
		NSArray *eventsUpToCurrent = [allEvents subarrayWithRange:NSMakeRange(0,[allEvents indexOfObjectIdenticalTo:[content lastObject]]+1)];
	
		[super setContent:eventsUpToCurrent];
		//select all:
		[self setSelectionIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0,eventsUpToCurrent.count)]];
	}
	else
	{
		[super setContent:content];
		[self setSelectionIndex:NSNotFound];
	}
}
@end
