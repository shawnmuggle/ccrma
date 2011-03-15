//
//  WORenderer.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "WOState.h"

@interface WORenderer : NSObject {
    
    IBOutlet WOState* state;
    IBOutlet UIView* view;

}

- (void) render;

@end
