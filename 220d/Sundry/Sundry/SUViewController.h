//
//  SUViewController.h
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@class SUSpace;
@class SUPlayer;

@interface SUViewController : GLKViewController

@property (nonatomic, strong) SUSpace *space;
@property (nonatomic, strong) SUPlayer *player;

@end
