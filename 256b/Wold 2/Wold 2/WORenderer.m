//
//  WORenderer.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WORenderer.h"

#import "WOPlanet.h"
#import "WOTree.h"

@implementation WORenderer

- (void) awakeFromNib
{
    [[view layer] addSublayer:state.currentPlanet.layer];
}

- (void) render
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGRect screen = CGContextGetClipBoundingBox(context);
    
    if (state.currentPlanet) {
        
        // TODO: Figure out how to make the background blue when zoomed in on planet, black when zoomed out with fade
        float scale, baseRadius, maxHeight, centerX, centerY;
        switch (state.zoomLevel) {
            case 3: // Zoomed in to the surface
                scale = 1.0;
                centerX = screen.size.width / 2.0;
                centerY = screen.size.height / 2.0 + state.zoomedPlanetYOffset;

                break;
            
            case 2: // Zoomed in so the planet is centered on the screen
                scale = 0.333;
                centerX = screen.size.width / 2.0;
                centerY = screen.size.height / 2.0;
                                
            default:
                break;
        }

        // HACK: these are duplicated from WOPlanet
        baseRadius = 900 * scale;
        maxHeight = 300 * scale;
        
        CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
        CGFloat components[] = {0.2, 0.8, 0.3, 1.0};
        CGColorRef color = CGColorCreate(colorspace, components);
        CGContextSetStrokeColorWithColor(context, color);
        CGContextSetLineWidth(context, 2.0);
        
        for (WOPlanet* planet in state.planets) {
            [UIView beginAnimations:nil context:nil];
            [UIView setAnimationDuration:0.0];
            [UIView setAnimationCurve:UIViewAnimationCurveLinear];
            
            CGAffineTransform scaleTransform = CGAffineTransformMakeScale(scale, scale);
            CGAffineTransform translateTransform = CGAffineTransformMakeTranslation(centerX, centerY);
            CGAffineTransform rotateTransform = CGAffineTransformMakeRotation(state.rotationAngle);
            CGAffineTransform transform = CGAffineTransformConcat(scaleTransform, rotateTransform);
            transform = CGAffineTransformConcat(transform, translateTransform);
            // TODO: Change this to retrieve specifically the planet sublayer!
            [[[view layer].sublayers lastObject] setAffineTransform:transform];
            
            [UIView commitAnimations];
            
            

//            float planetSurface;
//            // TODO: The trees should be scaled appropriately to the current zoom level
//            for (WOTree* tree in planet.trees) {
//
//                planetSurface = [planet surfaceFunction:tree.angle];
//                float treeX = centerX + (baseRadius + maxHeight * planetSurface) * cos(-tree.angle - state.rotationAngle);
//                float treeY = centerY - (baseRadius + maxHeight * planetSurface) * sin(-tree.angle - state.rotationAngle);
//                
//                CGContextAddArc(context, treeX, treeY, 10, 0, 2 * M_PI, 0);
//                CGContextMoveToPoint(context, treeX, treeY);
//                [tree renderAtPoint:CGPointMake(treeX, treeY) withAngle:(-tree.angle - state.rotationAngle)];
//                //CGContextAddLineToPoint(context, treeTopX, treeTopY);
//                CGContextStrokePath(context);
//            }

        }
        
        CGColorSpaceRelease(colorspace);
        CGColorRelease(color);
    }
}

@end
