//
//  SAAudioProducer.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/24/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol SAAudioProducer <NSObject>

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames;

@end
