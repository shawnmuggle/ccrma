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

@property (nonatomic, strong) IBOutlet UIView *cubeEditingView;
@property (nonatomic, strong) IBOutlet UIView *percsEditingView;

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

@property (nonatomic, strong) IBOutlet UISlider *perc0StartFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc0EndFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc0LengthSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc1StartFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc1EndFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc1LengthSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc2StartFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc2EndFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc2LengthSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc3StartFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc3EndFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc3LengthSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc4StartFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc4EndFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc4LengthSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc5StartFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc5EndFreqSlider;
@property (nonatomic, strong) IBOutlet UISlider *perc5LengthSlider;

@property (nonatomic, strong) IBOutlet UIProgressView *percLoopProgressView;


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

- (IBAction)startFreqChanged:(UISlider *)sender;
- (IBAction)endFreqChanged:(UISlider *)sender;
- (IBAction)lengthChanged:(UISlider *)sender;
- (IBAction)percPadPressed:(UIButton *)sender;
- (IBAction)clearSprings:(UIButton *)sender;
- (IBAction)doneEditing:(UIButton *)sender;

@end
