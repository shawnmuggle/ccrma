//
//  ClassFilteringArrayController.m
//
//  Created by handley on 11/10/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ClassFilteringArrayController.h"

@implementation ClassFilteringArrayController
- (void)awakeFromNib
{
	[self setFilterPredicate:[NSPredicate predicateWithFormat:@"self.class == %@",self.objectClass]];
}
@end
