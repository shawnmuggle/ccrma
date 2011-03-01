//
//  WOPlanet.h
//  wold
//
//  Created by Michael Rotondo on 2/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "mo_gfx.h"

#import "WOLSystem.h"

@interface WOPlanet : NSObject {

    int id;
    
    Vector3D position;
    float radius;
    GLuint planet_texture;
    GLuint tree_texture;
    
    GLfloat r,g,b;
    
    NSMutableSet* lines;
    NSMutableSet* trees;
    WOLSystem* growingTree;
}

@property Vector3D position;
@property float radius;
@property int id;
@property (nonatomic, retain) NSMutableSet* lines;
@property (nonatomic, retain) NSMutableSet* trees;
@property (nonatomic, retain) WOLSystem* growingTree;

- (id) initWithID:(int)dbID 
      andPosition:(Vector3D)pos 
        andRadius:(float)rad 
       andTexture:(GLuint)texture_id 
   andTreeTexture:(GLuint)tree_texture_id
           andRed:(GLfloat)red
         andGreen:(GLfloat)green
          andBlue:(GLfloat)blue;
- (void) addPoint:(Vector3D)contactPoint;
- (void) addTreeAtPoint:(Vector3D*)point;
- (void) stopGrowingAndSendToServer:(BOOL)send;
- (void) transform;
- (void) render;
- (void) processDrag:(UIPanGestureRecognizer*)gesture;
- (void) processTap:(Vector3D)point;
- (void) tick;
- (NSArray*) getTreesFromServer;
- (void) submitTreeToServer:(WOLSystem*)tree;

@end
