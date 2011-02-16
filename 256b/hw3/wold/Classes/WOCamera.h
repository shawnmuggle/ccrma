//
//  WOCamera.h
//  wold
//
//  Created by Michael Rotondo on 2/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "WOPlanet.h"

@interface WOCamera : NSObject {

    WOPlanet* planet;
    
}

@property (nonatomic, retain) WOPlanet* planet;

- (void) transform;

@end
