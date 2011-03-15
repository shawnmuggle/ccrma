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
        

        int numPlanetSegments;
        float baseRadius, maxHeight, centerX, centerY;
        // TODO: slew the zoom instead of jumping straight to each level
        switch (state.zoomLevel) {
            case 3: // Zoomed in to the surface
                numPlanetSegments = 500;
                baseRadius = 900;
                maxHeight = 300;
                centerX = screen.size.width / 2.0;
                centerY = screen.size.height / 2.0 + state.zoomedPlanetYOffset;
                
                break;
            
            case 2: // Zoomed in so the planet is centered on the screen
                numPlanetSegments = 200;
                baseRadius = 300;
                maxHeight = 100;
                centerX = screen.size.width / 2.0;
                centerY = screen.size.height / 2.0;
                                
            default:
                break;
        }
        
        CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
        CGFloat components[] = {0.2, 0.8, 0.3, 1.0};
        CGColorRef color = CGColorCreate(colorspace, components);
        CGContextSetStrokeColorWithColor(context, color);
        CGContextSetLineWidth(context, 2.0);
        
        for (WOPlanet* planet in state.planets) {
//            float planetSurface = [planet surfaceFunction:0];
//            float x = (baseRadius + maxHeight * planetSurface) * cos(0 + state.rotationAngle);
//            float y = (baseRadius + maxHeight * planetSurface) * sin(0 + state.rotationAngle);
//            CGContextMoveToPoint(context, centerX + x, centerY + y);
//            
//            float angle;
//            
//            // TODO: When zoomed in on the surface, only the line segments that are on-screen should be drawn
//            for (int i = 1; i <= numPlanetSegments; i++) {
//                angle = (i / (float)numPlanetSegments) * 2 * M_PI;
//                planetSurface = [planet surfaceFunction:-angle];
//                x = (baseRadius + maxHeight * planetSurface) * cos(-angle + state.rotationAngle);
//                y = (baseRadius + maxHeight * planetSurface) * sin(-angle + state.rotationAngle);
//                CGContextAddLineToPoint(context, centerX + x, centerY + y);
//            }
//            CGContextStrokePath(context);
            
            CGAffineTransform translate = CGAffineTransformMakeTranslation(centerX, centerY);
            CGAffineTransform rotate = CGAffineTransformMakeRotation(state.rotationAngle);
            CGAffineTransform transform = CGAffineTransformConcat(rotate, translate);
            // TODO: Change this to retrieve specifically the planet sublayer!
            [UIView beginAnimations:nil context:nil];
            [UIView setAnimationDuration:0.5];
            [UIView setAnimationCurve:UIViewAnimationCurveEaseIn];
            
            [[[view layer].sublayers lastObject] setAffineTransform:transform];
            
            [UIView commitAnimations];
            
            

            float planetSurface;
            // TODO: The trees should be scaled appropriately to the current zoom level
            for (WOTree* tree in planet.trees) {

                planetSurface = [planet surfaceFunction:tree.angle];
                float treeX = centerX + (baseRadius + maxHeight * planetSurface) * cos(-tree.angle - state.rotationAngle);
                float treeY = centerY - (baseRadius + maxHeight * planetSurface) * sin(-tree.angle - state.rotationAngle);
//                float treeTopX = centerX + (baseRadius + maxHeight * planetSurface + 100) * cos(-tree.angle - state.rotationAngle);
//                float treeTopY = centerY - (baseRadius + maxHeight * planetSurface + 100) * sin(-tree.angle - state.rotationAngle);
                
                CGContextAddArc(context, treeX, treeY, 10, 0, 2 * M_PI, 0);
                CGContextMoveToPoint(context, treeX, treeY);
                [tree renderAtPoint:CGPointMake(treeX, treeY) withAngle:(-tree.angle - state.rotationAngle)];
                //CGContextAddLineToPoint(context, treeTopX, treeTopY);
                CGContextStrokePath(context);
            }

        }
        
        CGColorSpaceRelease(colorspace);
        CGColorRelease(color);
    }
}

@end
