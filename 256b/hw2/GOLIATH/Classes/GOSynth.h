//
//  GoliathSynth.h
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "GOState.h"

@interface GOSynth : NSObject {

    GOState* state;
    
}
- (id) initWithState:(GOState*) newState;

@property (nonatomic, retain) GOState* state;

@end
