//
//  WOPlanet.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOPlanet.h"
#import "WOTree.h"
#import "ASIFormDataRequest.h"

@implementation WOPlanet
@synthesize trees, layer, baseRadius, loc, translation, rotation, rotationAngleIncrement, rotationAngle;

- (id) initAtPoint:(CGPoint)newLoc
{
    self = [super init];
    if (self) {
        self.trees = [NSMutableSet setWithCapacity:10];

        [self determineShape];
        
        self.layer = [CAShapeLayer layer];
        path = CGPathCreateMutable();
        [self generatePath];
        self.layer.path = path;
        self.layer.bounds = CGPathGetBoundingBox(path);
        
        self.translation = [CALayer layer];
        self.rotation = [CALayer layer];
        [self.translation addSublayer:self.rotation];
        [self.rotation addSublayer:self.layer];
        
        loc = newLoc;
        self.translation.transform = CATransform3DMakeTranslation(loc.x, loc.y, 0);
        
        [self setupLayer];
        
        rotationAngle = 0;
        self.rotationAngleIncrement = 0;
        
        soundFiles = [[NSArray arrayWithObjects:@"ice", @"fire", @"door", @"record", nil] retain];
        
    }
    return self;
}

- (id) initAtPoint:(CGPoint)newLoc withId:(int)newPlanetId
{
    self = [super init];
    if (self) {
        self.trees = [NSMutableSet setWithCapacity:10];
        
        if (newPlanetId == -1) { // CREATE NEW PLANET
            planetId = [self getNewPlanetIdFromServer];
            
            NSLog(@"Got planet id %d from the server...", planetId);
            
            [self determineShape];
 
            
            self.layer = [CAShapeLayer layer];
            path = CGPathCreateMutable();
            [self generatePath];
            self.layer.path = path;
            self.layer.bounds = CGPathGetBoundingBox(path);
            
            self.translation = [CALayer layer];
            self.rotation = [CALayer layer];
            [self.translation addSublayer:self.rotation];
            [self.rotation addSublayer:self.layer];
            
            loc = newLoc;
            self.translation.transform = CATransform3DMakeTranslation(loc.x, loc.y, 0);
            
            [self setupLayer];
            
            rotationAngle = 0;
            self.rotationAngleIncrement = 0;
            
            soundFiles = [[NSArray arrayWithObjects:@"ice", @"fire", @"door", @"record", nil] retain];
            
            [self submitPlanetInfoToServer];
        }

    }
    return self;
}

- (id) initAtPoint:(CGPoint)newLoc withPlist:(NSDictionary*)plist
{
    self = [super init];
    if (self) {
        self.trees = [NSMutableSet setWithCapacity:10];
        
        // HACK: this whole thing is a fucking hack. Collapse all this into shared code!!!
        
        planetId = [[plist objectForKey:@"id"] intValue];
        
        //Equivalent to a call to determineShape
        
        // TODO: Oops, store radius and maxHeight on the server too
        self.baseRadius = 800 + 200 * (rand() / (float)RAND_MAX);
        maxHeight = 200 + 200 * (rand() / (float)RAND_MAX);
        
        a1 = [[plist objectForKey:@"a1"] floatValue];
        a2 = [[plist objectForKey:@"a2"] floatValue];
        a3 = [[plist objectForKey:@"a3"] floatValue];
        a4 = [[plist objectForKey:@"a4"] floatValue];

        c1 = [[plist objectForKey:@"c1"] floatValue];
        c2 = [[plist objectForKey:@"c2"] floatValue];
        c3 = [[plist objectForKey:@"c3"] floatValue];
        c4 = [[plist objectForKey:@"c4"] floatValue];

        
        // Duplicated from the other init
        
        self.layer = [CAShapeLayer layer];
        path = CGPathCreateMutable();
        [self generatePath];
        self.layer.path = path;
        self.layer.bounds = CGPathGetBoundingBox(path);
        
        self.translation = [CALayer layer];
        self.rotation = [CALayer layer];
        [self.translation addSublayer:self.rotation];
        [self.rotation addSublayer:self.layer];
        
        loc = newLoc;
        self.translation.transform = CATransform3DMakeTranslation(loc.x, loc.y, 0);
        
        
        // Equivalent to a call to setupLayer
        self.layer.strokeColor = [UIColor colorWithRed:0.2 green:0.8 blue:0.3 alpha:1.0].CGColor;
        self.layer.lineWidth = 8.0;
        
        r = [[plist objectForKey:@"red"] floatValue];
        g = [[plist objectForKey:@"green"] floatValue];
        b = [[plist objectForKey:@"blue"] floatValue];
        float fill[3] = {r, g, b};
        
        self.layer.fillColor = [self generateSquaresPatternFromColorComponents:fill];
        
        rotationAngle = 0;
        self.rotationAngleIncrement = 0;
        
        NSLog(@"SOUND 1: %@", [plist objectForKey:@"sound1"]);
        
        soundFiles = [[NSArray arrayWithObjects:
                       [plist objectForKey:@"sound1"],
                       [plist objectForKey:@"sound2"],
                       [plist objectForKey:@"sound3"],
                       [plist objectForKey:@"sound4"], nil] retain];
        
        self.layer.shouldRasterize = YES;
        
        for (NSDictionary* treePlist in [self getTreesFromServer]) {
            [self addTreeAtAngle:[[treePlist valueForKey:@"angle"] floatValue] treeType:[[treePlist valueForKey:@"type"] intValue]];
            
            // HACK HACK HACK
            //tree.lSystem.angleOffset = [[treePlist valueForKey:@"angleOffset"] floatValue];
            //[tree.lSystem setAge:[[treePlist valueForKey:@"age"] floatValue]];
        }
        for (WOTree* tree in self.trees) {
            for (SoundGrain* grain in tree.lSystem.instrument.grainsArray) {
                grain.on = NO;
            }
        }
        
    }
    return self;
}

- (NSArray*) getTreesFromServer
{
    NSLog(@"Getting trees from server!");
    
    NSArray* treesPlist;
    
    NSString *server = [[NSString alloc] initWithString:@"http://mikerotondo.com/wold/"];
    
    NSURL *url = [NSURL URLWithString:[server stringByAppendingFormat:@"treesforplanet/%d", planetId]];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    
    // Start sending synchronously
    [request startSynchronous];
    
    // Display any error messages
    NSError *error = [request error];
    if (!error) {
        NSString *response = [request responseString];
        NSLog(@"Response: %@", response);
        
        NSData* data = [NSData dataWithBytes:[response cStringUsingEncoding:NSUTF8StringEncoding] 
                                      length:[response lengthOfBytesUsingEncoding:NSUTF8StringEncoding]];
        treesPlist = [NSPropertyListSerialization propertyListWithData:data
                                                               options:0
                                                                format:NULL
                                                                 error:NULL];
    }
    return treesPlist;
}


- (void) submitPlanetInfoToServer
{
	NSLog(@"sending planet updates to server...");
	
	NSString *server = [[NSString alloc] initWithUTF8String:"http://mikerotondo.com/wold/"];
	
	NSURL *url = [NSURL URLWithString:[server stringByAppendingString:@"planets"]];
	
	ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
	
    // Setup the variables for the request
    [request setPostValue:[NSNumber numberWithFloat:planetId] forKey:@"id"];
    
    [request setPostValue:[NSNumber numberWithFloat:r] forKey:@"r"];
    [request setPostValue:[NSNumber numberWithFloat:g] forKey:@"g"];
    [request setPostValue:[NSNumber numberWithFloat:b] forKey:@"b"];

    [request setPostValue:[NSNumber numberWithFloat:a1] forKey:@"a1"];
    [request setPostValue:[NSNumber numberWithFloat:a2] forKey:@"a2"];
    [request setPostValue:[NSNumber numberWithFloat:a3] forKey:@"a3"];
    [request setPostValue:[NSNumber numberWithFloat:a4] forKey:@"a4"];

    [request setPostValue:[NSNumber numberWithFloat:c1] forKey:@"c1"];
    [request setPostValue:[NSNumber numberWithFloat:c2] forKey:@"c2"];
    [request setPostValue:[NSNumber numberWithFloat:c3] forKey:@"c3"];
    [request setPostValue:[NSNumber numberWithFloat:c4] forKey:@"c4"];
    
    [request setPostValue:[soundFiles objectAtIndex:0] forKey:@"sound1"];
    [request setPostValue:[soundFiles objectAtIndex:1] forKey:@"sound2"];
    [request setPostValue:[soundFiles objectAtIndex:2] forKey:@"sound3"];
    [request setPostValue:[soundFiles objectAtIndex:3] forKey:@"sound4"];
    
	[request startSynchronous];
    
//	NSError *error = [request error];
//	if (!error) {
//		NSString *response = [request responseString];
//        NSLog(@"POSTED TREE: %@", response);
//	}	
}

- (int) getNewPlanetIdFromServer
{
	NSLog(@"getting new planet id to server...");
	
	NSString *server = [[NSString alloc] initWithUTF8String:"http://mikerotondo.com/wold/"];
	NSURL *url = [NSURL URLWithString:[server stringByAppendingString:@"newplanet"]];
	
	ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    [request startSynchronous];
    
	NSError *error = [request error];
	if (!error) {
		NSString *response = [request responseString];
        return [response intValue];
	}
    return -1;
}

- (void) determineShape
{
    self.baseRadius = 800 + 200 * (rand() / (float)RAND_MAX);
    maxHeight = 200 + 200 * (rand() / (float)RAND_MAX);
    
    c1 = 0.08 * rand() / RAND_MAX;
    c2 = 0.07 * rand() / RAND_MAX;
    c3 = 0.05 * rand() / RAND_MAX;
    c4 = 0.03 * rand() / RAND_MAX;
    
    a1 = rand() % 10;
    a2 = rand() % 20;
    a3 = rand() % 30;
    a4 = rand() % 40;
    
}

- (CGColorRef) generateSquaresPatternFromColorComponents:(float*)components
{
    int w = 200;
    int h = 200;
    int squareDim = 10;
    
    CGContextRef bitmapContext = CGBitmapContextCreate(NULL, w, h, 8, 4 * w, CGColorSpaceCreateDeviceRGB(), 
                                                       kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrderDefault);
    
    float offset;
    CGRect square;
    for (int y = 0; y < h / squareDim; y++) {
        for (int x = 0; x < w / squareDim; x++) {
            offset = 0.05 * rand() / (float)RAND_MAX;
            square = CGRectMake(x * squareDim, y * squareDim, squareDim, squareDim);
            CGContextSetFillColorWithColor(bitmapContext, [UIColor colorWithRed:components[0] + offset 
                                                                          green:components[1] + offset 
                                                                           blue:components[2] + offset 
                                                                          alpha:1.0].CGColor);
            CGContextAddRect(bitmapContext, square);
            CGContextFillPath(bitmapContext);
        }
    }
    
    // Create a CGImage from context
    CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
    // Create a UIImage from CGImage
    UIImage *uiImage = [UIImage imageWithCGImage:cgImage];
    // Release the CGImage
    CGImageRelease(cgImage);
    // Release the bitmap context
    CGContextRelease(bitmapContext);
    // Create the patterned UIColor and set as background color
    
    //[targetView setBackgroundColor:[UIColor colorWithPatternImage:image]];
    return [UIColor colorWithPatternImage:uiImage].CGColor;
}

- (void) setupLayer
{
    self.layer.strokeColor = [UIColor colorWithRed:0.2 green:0.8 blue:0.3 alpha:1.0].CGColor;
    self.layer.lineWidth = 8.0;

    r =  0.7 * rand() / (float)RAND_MAX;
    g = 0.6 * rand() / (float)RAND_MAX;
    b = 0.8 * rand() / (float)RAND_MAX;
    float fill[3] = {r, g, b};
    
    self.layer.fillColor = [self generateSquaresPatternFromColorComponents:fill];
}

// TODO: Figure out if we should generate multiple paths for different zoom levels, or just scale one path
- (void) generatePath
{
    int numPlanetSegments = 500;
    
    float planetSurface = [self surfaceFunction:0];
    float x = (self.baseRadius + maxHeight * planetSurface) * cos(0);
    float y = (self.baseRadius + maxHeight * planetSurface) * sin(0);
    CGPathMoveToPoint(path, NULL, x, y);
    
    float angle;    
    for (int i = 1; i <= numPlanetSegments; i++) {
        angle = (i / (float)numPlanetSegments) * 2 * M_PI;
        planetSurface = [self surfaceFunction:angle];
        x = (self.baseRadius + maxHeight * planetSurface) * cos(-angle);
        y = (self.baseRadius + maxHeight * planetSurface) * sin(-angle);
        CGPathAddLineToPoint(path, NULL, x, y);
    }
}

- (void) addTreeAtAngle:(float)angle treeType:(int)type
{
    float planetSurface = [self surfaceFunction:angle];
    float treeX = (self.baseRadius + maxHeight * planetSurface) * cos(-angle);
    float treeY = (self.baseRadius + maxHeight * planetSurface) * sin(-angle);

    WOTree* tree = [[WOTree alloc] initWithAngle:-angle andOrigin:CGPointMake(treeX, treeY) andType:type andFilename:[soundFiles objectAtIndex:type - 1]];
    tree.planetId = planetId;
    [self.trees addObject:tree];
    
    [self.layer addSublayer:[tree layer]];
}

- (float) surfaceFunction:(float)x
{
    return cos(a1 * x) * c1 + cos(a2 * x) * c2 + cos(a3 * x) * c3 + cos(a4 * x) * c4;
}

- (void) tick
{
    float rotationDamping = 0.92;
    self.rotationAngle += self.rotationAngleIncrement;
    self.rotationAngleIncrement *= rotationDamping;
    
    //NSLog(@"In planet tick, we have %d trees", [self.trees count]);
    
    for (WOTree* tree in self.trees) {
        [tree tick];
    }
}

- (void) stopGrowingTree
{
    // NOTE: This assumes that there is only one tree growing at any time!!
    for (WOTree* tree in self.trees) {
        if ([tree isGrowing]) {
            [tree stopGrowing];
            
            [tree updateServer];
        }
    }
}

@end

@implementation WOStar

- (void) setupLayer
{
    self.layer.lineWidth = 40.0;

    float stroke[3] = { 0.8  + 0.2 * rand() / (float)RAND_MAX, 0.9 + 0.1 * rand() / (float)RAND_MAX,  0.5 + 0.1 * rand() / (float)RAND_MAX };    
    self.layer.strokeColor = [self generateSquaresPatternFromColorComponents:stroke];

    float fill[3] = { 0.7  + 0.3 * rand() / (float)RAND_MAX, 0.8 + 0.2 * rand() / (float)RAND_MAX,  0.4 + 0.1 * rand() / (float)RAND_MAX };    
    self.layer.fillColor = [self generateSquaresPatternFromColorComponents:fill];
    
    // TODO: Draw sweet radiation lines around the sun
    float angle, lengthScale, outerDist;
    int numLines = 20;
    int baseLineLength = baseRadius * 0.22;
    float innerDist = (baseRadius + maxHeight) * 1.0;
    CGPoint innerPoint, outerPoint;
    CGMutablePathRef raysPath = CGPathCreateMutable();
    for (int i = 0; i < numLines; i++) {
        angle = (i / (float)numLines) * 2 * M_PI;
        lengthScale = 1.3 * (rand() / (float)RAND_MAX);
        outerDist = innerDist + baseLineLength + baseLineLength * lengthScale;
        innerPoint = CGPointMake(innerDist * cos(angle), innerDist * sin(angle));
        outerPoint = CGPointMake(outerDist * cos(angle), outerDist * sin(angle));
        CGPathMoveToPoint(raysPath, NULL, innerPoint.x, innerPoint.y);
        CGPathAddLineToPoint(raysPath, NULL, outerPoint.x, outerPoint.y);
    }
    CAShapeLayer* raysLayer = [CAShapeLayer layer];
    raysLayer.path = raysPath;
    raysLayer.lineWidth = 50;
    raysLayer.strokeColor = self.layer.strokeColor;
    [self.layer addSublayer: raysLayer];
    
    CATransform3D rotationTransform = CATransform3DMakeRotation(1.0f * M_PI, 0, 0, 1.0);
    
    CABasicAnimation* rotationAnimation;
    rotationAnimation = [CABasicAnimation animationWithKeyPath:@"transform"];
    
    rotationAnimation.toValue = [NSValue valueWithCATransform3D:rotationTransform];
    rotationAnimation.duration = 5.0f;
    rotationAnimation.cumulative = YES;
    rotationAnimation.repeatCount = 99999999; 
    
    [raysLayer addAnimation:rotationAnimation forKey:@"rotationAnimation"];
}

- (void) determineShape
{
    self.baseRadius = 1400 + 400 * (rand() / (float) RAND_MAX);
    maxHeight = 200 + 200 * (rand() / (float)RAND_MAX);    
}

- (float) surfaceFunction:(float)x
{
    return cos(28 * x) * 0.03;
}

@end
