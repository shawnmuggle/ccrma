//
//  WOAudio.h
//  wold
//
//  Created by Michael Rotondo on 2/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WOState.h"
#import "mo_audio.h"


@interface WOAudio : NSObject {

    WOState* state;
    
}

@property (nonatomic, retain) WOState* state;

- (id) initWithState:(WOState*)state;

@end
