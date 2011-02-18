//
//  WOGeometry.h
//  wold
//
//  Created by Michael Rotondo on 2/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "mo_gfx.h"

@interface WOGeometry : NSObject {

}

+ (void)drawSphereWithRadius:(GLfloat)r andNumLats:(GLint)lats andNumLongs:(GLint)longs;
+ (void)drawFrustumWithBottomRadius:(GLfloat)rBottom andTopRadius:(GLfloat)topRadius andHeight:(GLfloat)h andSections:(GLint)sections;

//+ (void)generateDisk;
+ (void)drawDiskWithRadius:(GLfloat)r andSections:(GLint)sections;

@end
