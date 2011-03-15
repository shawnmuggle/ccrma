//
//  WOPlanetView.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "WORenderer.h"
#import "WOState.h"

@interface WOPlanetView : UIView {
    
    IBOutlet WORenderer* renderer;
    IBOutlet WOState* state;
    
}

- (void) tick:(id)sender;

@end
