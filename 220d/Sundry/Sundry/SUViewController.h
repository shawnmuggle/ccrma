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

@property (nonatomic, strong) IBOutlet UIView *editingView;

@property (nonatomic, strong) IBOutlet UISlider *scaleSlider;
@property (nonatomic, strong) IBOutlet UISlider *freqSlider;
@property (nonatomic, strong) IBOutlet UISlider *axisXSlider;
@property (nonatomic, strong) IBOutlet UISlider *axisYSlider;
@property (nonatomic, strong) IBOutlet UISlider *axisZSlider;
@property (nonatomic, strong) IBOutlet UISlider *posXSlider;
@property (nonatomic, strong) IBOutlet UISlider *posYSlider;
@property (nonatomic, strong) IBOutlet UISlider *posZSlider;
@property (nonatomic, strong) IBOutlet UISlider *colRSlider;
@property (nonatomic, strong) IBOutlet UISlider *colGSlider;
@property (nonatomic, strong) IBOutlet UISlider *colBSlider;

- (IBAction)scaleChanged:(UISlider *)sender;
- (IBAction)freqChanged:(UISlider *)sender;
- (IBAction)axisXChanged:(UISlider *)sender;
- (IBAction)axisYChanged:(UISlider *)sender;
- (IBAction)axisZChanged:(UISlider *)sender;
- (IBAction)posXChanged:(UISlider *)sender;
- (IBAction)posYChanged:(UISlider *)sender;
- (IBAction)posZChanged:(UISlider *)sender;
- (IBAction)colRChanged:(UISlider *)sender;
- (IBAction)colGChanged:(UISlider *)sender;
- (IBAction)colBChanged:(UISlider *)sender;
- (IBAction)createAnotherCube:(id)sender;
- (IBAction)doneCreatingCubes:(id)sender;


@end
