//
//  WOCamera.mm
//  wold
//
//  Created by Michael Rotondo on 2/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOCamera.h"

@implementation WOCamera
@synthesize planet;

- (void) transform
{
    glTranslatef(-self.planet.position.x - self.planet.radius, -self.planet.position.y, -self.planet.position.z - self.planet.radius * 2.5);
    
    [self.planet transform];
}

- (void) dealloc
{
    [planet release];
    
    [super dealloc];
}

@end
