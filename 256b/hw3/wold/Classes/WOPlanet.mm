//
//  WOPlanet.mm
//  wold
//
//  Created by Michael Rotondo on 2/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOPlanet.h"
#import "WOLSystem.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "WOGeometry.h"

#import "ArcBall.h"

GLfloat g_sun_angle = 0;
GLfloat g_sun_dist = 30;

Matrix4fT Transform = {
    1.0f,  0.0f,  0.0f,  0.0f,
    0.0f,  1.0f,  0.0f,  0.0f,
    0.0f,  0.0f,  1.0f,  0.0f,
    0.0f,  0.0f,  0.0f,  1.0f };

Matrix3fT LastRot   = {  
    1.0f,  0.0f,  0.0f,					// Last Rotation
    0.0f,  1.0f,  0.0f,
    0.0f,  0.0f,  1.0f };

Matrix3fT ThisRot   = {  
    1.0f,  0.0f,  0.0f,					// This Rotation
    0.0f,  1.0f,  0.0f,
    0.0f,  0.0f,  1.0f };

ArcBallT    ArcBall(768.0f * 2, 1024.0f);
Point2fT    MousePt;

@implementation WOPlanet
@synthesize position, radius, lines, trees;

- (void) processDrag:(UIPanGestureRecognizer *)gesture
{
    if (gesture.state == UIGestureRecognizerStateBegan) {
        NSLog(@"Starting a drag!");
        LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
        
        CGPoint loc = [gesture locationInView:gesture.view];
        NSLog(@"Drag started at %f, %f", loc.x, loc.y);
        MousePt.s.X = loc.x + 768; // Accounting for screen offset in "spherical" dragging centered (roughly) on left edge of screen
        MousePt.s.Y = loc.y;
              
        ArcBall.click(&MousePt);
    } else if (gesture.state == UIGestureRecognizerStateChanged) {
        NSLog(@"Continuing a drag!");    
        
        CGPoint loc = [gesture locationInView:gesture.view];
        NSLog(@"Drag started at %f, %f", loc.x, loc.y);
        MousePt.s.X = loc.x + 768; // Accounting for screen offset
        MousePt.s.Y = loc.y;
        
        Quat4fT     ThisQuat;
        
        ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
        Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
        Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
    }
}

- (id) initWithPosition:(Vector3D)pos andRadius:(float)rad andTexture:(GLuint)texture_id andTreeTexture:(GLuint)tree_texture_id
{
    self = [super init];
    if (self) {
        self.position = pos;
        self.radius = rad;
        
        self.lines = [[[NSMutableSet alloc] init] autorelease];
        self.trees = [[[NSMutableSet alloc] init] autorelease];

        planet_texture = texture_id;
        tree_texture = tree_texture_id;
        
        Vector3D* line = new Vector3D(1, 0, 0);
        [self.lines addObject:[NSValue valueWithPointer:line]];
        
        [self addTreeAtPoint:line];
    }
    return self;
}

- (void) dealloc
{
    [lines release];
    [trees release];
    
    [super dealloc];
}

- (void) addPoint:(Vector3D)contactPoint
{
    NSLog(@"Contact point: %f, %f, %f", contactPoint.x, contactPoint.y, contactPoint.z);
    
    
    Vector3D* newPoint = new Vector3D(position - contactPoint);
    newPoint->normalize();
    *newPoint *= -1;
    NSLog(@"New point: %f, %f, %f", newPoint->x, newPoint->y, newPoint->z);
    
    [self addTreeAtPoint:newPoint];
    
    [self.lines addObject:[NSValue valueWithPointer:newPoint]];
}

- (void) addTreeAtPoint:(Vector3D*)point
{
    NSLog(@"This Rot: \n%f %f %f;\n%f %f %f;\n%f %f %f", ThisRot.s.XX, ThisRot.s.XY, ThisRot.s.XZ, ThisRot.s.YX, ThisRot.s.YY, ThisRot.s.YZ, ThisRot.s.ZX, ThisRot.s.ZY, ThisRot.s.ZZ);
    
    Matrix3fT pointMatrix;
    pointMatrix.s.XX = point->x;
    pointMatrix.s.YX = point->y;
    pointMatrix.s.ZX = point->z;
    Matrix3fMulMatrix3f(&pointMatrix, &ThisRot);
    
    NSLog(@"PointMatrix: \n%f %f %f;\n%f %f %f;\n%f %f %f", pointMatrix.s.XX, pointMatrix.s.XY, pointMatrix.s.XZ, pointMatrix.s.YX, pointMatrix.s.YY, pointMatrix.s.YZ, pointMatrix.s.ZX, pointMatrix.s.ZY, pointMatrix.s.ZZ);
    
    point->x = pointMatrix.s.XX;
    point->y = pointMatrix.s.YX;
    point->z = pointMatrix.s.ZX;
    
    WOLSystem* tree = [[[WOLSystem alloc] initWithMaxGeneration:4 atPoint:*point] autorelease];
//    WOXNode* xNode = [[[WOXNode alloc] init] autorelease];
//    [tree.nodes addObject:xNode];
    
    WOLittleFNode* fNode = [[[WOLittleFNode alloc] init] autorelease];
    [tree.nodes addObject:fNode];
    fNode = [[[WOLittleFNode alloc] init] autorelease];
    [tree.nodes addObject:fNode];
//    fNode = [[[WOLittleFNode alloc] init] autorelease];
//    [tree.nodes addObject:fNode];
//    fNode = [[[WOLittleFNode alloc] init] autorelease];
//    [tree.nodes addObject:fNode];
//    fNode = [[[WOLittleFNode alloc] init] autorelease];
//    [tree.nodes addObject:fNode];
    
    WOANode* aNode = [[[WOANode alloc] init] autorelease];
    [tree.nodes addObject:aNode];
    
    [self.trees addObject:tree];
    
    GLfloat xy_angle = tree.origin.angleXY();
    GLfloat xz_angle = tree.origin.angleXZ();
    GLfloat yz_angle = tree.origin.angleYZ();
    
    NSLog(@"ORIGIN: %f, %f, %f", tree.origin.x, tree.origin.y, tree.origin.z);
    NSLog(@"ANGLES: XY %f, XZ %f, YZ %f", xy_angle, xz_angle, yz_angle);
    
}

- (void) transform
{
    glTranslatef(self.position.x, self.position.y, self.position.z);
}

- (void) render
{
    glTranslatef(self.position.x, self.position.y, self.position.z);
    
    //glRotatef(90, 1, 0, 0);

    //glEnable(GL_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    float lightSpecular[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    
    float matAmbient[] = { 0.5f, 1.0f, 0.7f, 1.0f };
    float matDiffuse[] = { 0.3f, 0.8f, 0.5f, 1.0f };
//    float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    GLfloat sun_x, sun_y, sun_z;
    
    sun_x = g_sun_dist;// * cos(g_sun_angle);
    sun_y = g_sun_dist;// * sin(g_sun_angle);
    sun_z = 0; //-1.5;
    g_sun_angle += 0.017;
    
    float lightPosition[] = { sun_x, sun_y, sun_z, 0.0f };
//    float lightDirection[] = { -sun_x, -sun_y, -sun_z };
    

//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    
//    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
//	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
//    
//    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
//    
//    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0f);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glPushMatrix();
    
    glMultMatrixf(Transform.M);										// NEW: Apply Dynamic Transform
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    //drawSphere(self.radius, 40, 40);
    glBindTexture( GL_TEXTURE_2D, planet_texture );
    
    [WOGeometry drawSphereWithRadius:self.radius andNumLats:40 andNumLongs:40];
    //[WOGeometry drawFrustumWithBottomRadius:self.radius andTopRadius:0 andHeight:self.radius * 2 andSections:40];
    
    glBindTexture( GL_TEXTURE_2D, tree_texture);
    
    GLfloat xy_angle, xz_angle, yz_angle;
    for (NSValue* lineValue in self.lines) {
        Vector3D* line = (Vector3D*)[lineValue pointerValue];
        Vector3D endPoint(*line * self.radius);
        
        xy_angle = line->angleXY() * (180 / M_PI);
        yz_angle = line->angleYZ() * (180 / M_PI);
        
        glPushMatrix();
        glTranslatef(endPoint.x, endPoint.y, endPoint.z);
        //glRotatef(xz_angle, 0, 1, 0);
        glRotatef(xy_angle, 0, 0, 1);
        //drawSphere(self.radius / 10, 10, 10);
        [WOGeometry drawSphereWithRadius:self.radius/10 andNumLats:10 andNumLongs:10];
        glPopMatrix();
    }
    
    for (WOLSystem* tree in self.trees) {
        [tree tick];
        
        xy_angle = tree.origin.angleXY();
        xz_angle = tree.origin.angleXZ();
        yz_angle = tree.origin.angleYZ();
        
        glPushMatrix();

        
        Vector3D treePoint = [tree origin] * self.radius;
        // TODO: This isn't quite right but is close enough for now. Fix it later.
        glTranslatef(treePoint.x, treePoint.y, treePoint.z);
        glRotatef(xy_angle * (180 / M_PI) - 90, 0.0f, 0.0f, 1.0f);        
        glRotatef(sin(yz_angle) * (180 / M_PI), 1.0, 0.0f, 0.0f);
        //glTranslatef(0, self.radius, 0);
        
        
        
        
        
//        // CODE TAKEN FROM http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/index.htm
//        // Assuming the angles are in radians.
//        float ch = cos(xz_angle);
//        float sh = sin(xz_angle);
//        float ca = cos(yz_angle);
//        float sa = sin(yz_angle);
//        float cb = cos(xy_angle);
//        float sb = sin(xy_angle);
//        
//        Matrix3fT rotationMatrix;
//        
//        rotationMatrix.s.XX = ch * ca;
//        rotationMatrix.s.YX = sh*sb - ch*sa*cb;
//        rotationMatrix.s.ZX = ch*sa*sb + sh*cb;
//        rotationMatrix.s.XY = sa;
//        rotationMatrix.s.YY = ca*cb;
//        rotationMatrix.s.ZY = -ca*sb;
//        rotationMatrix.s.XZ = -sh*ca;
//        rotationMatrix.s.YZ = sh*sa*cb + ch*sb;
//        rotationMatrix.s.ZZ = -sh*sa*sb + ch*cb;
//        
//        glTranslatef(tree.origin.x, tree.origin.y, tree.origin.z);
//        glMultMatrixf(rotationMatrix.M);
//        //glTranslatef(0, self.radius, 0);

        
        
        [tree render];
        glPopMatrix();
    }
    
    //glDisable(GL_ALPHA);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    
    glPopMatrix();
}

@end
