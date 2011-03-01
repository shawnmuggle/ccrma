//
//  WOPlanet.mm
//  wold
//
//  Created by Michael Rotondo on 2/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOPlanet.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "WOGeometry.h"

#import "ArcBall.h"

#import "ASIFormDataRequest.h"

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
@synthesize position, radius, lines, trees, growingTree, id;

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

- (void)processTap:(Vector3D)point
{
    NSLog(@"HEYO");
    
    Vector3D touchPoint(position - point);
    touchPoint.normalize();
    touchPoint *= -1;    
    
    Matrix3fT pointMatrix;
    pointMatrix.s.XX = touchPoint.x;
    pointMatrix.s.YX = touchPoint.y;
    pointMatrix.s.ZX = touchPoint.z;
    Matrix3fMulMatrix3f(&pointMatrix, &ThisRot);
    
    //NSLog(@"PointMatrix: \n%f %f %f;\n%f %f %f;\n%f %f %f", pointMatrix.s.XX, pointMatrix.s.XY, pointMatrix.s.XZ, pointMatrix.s.YX, pointMatrix.s.YY, pointMatrix.s.YZ, pointMatrix.s.ZX, pointMatrix.s.ZY, pointMatrix.s.ZZ);
    
    touchPoint.x = pointMatrix.s.XX;
    touchPoint.y = pointMatrix.s.YX;
    touchPoint.z = pointMatrix.s.ZX;
    
    touchPoint *= self.radius;
    
    float dist;
    for (WOLSystem* tree in self.trees) {
        Vector3D treePoint = [tree origin] * self.radius;
        dist = sqrt(pow(treePoint.x - touchPoint.x, 2) + pow(treePoint.y - touchPoint.y, 2) + pow(treePoint.z - touchPoint.z, 2));
        //NSLog(@"Dist: %f < %f ?", dist, self.radius / 5);
        if (dist < self.radius / 5) {
            tree.env = 1.0;
            break;
        }
    }
}

- (id) initWithID:(int)dbID 
      andPosition:(Vector3D)pos 
        andRadius:(float)rad 
       andTexture:(GLuint)texture_id 
   andTreeTexture:(GLuint)tree_texture_id
           andRed:(GLfloat)red
         andGreen:(GLfloat)green
          andBlue:(GLfloat)blue
{
    self = [super init];
    if (self) {
        self.id = dbID;
        
        self.position = pos;
        self.radius = rad;
        
        self.lines = [[[NSMutableSet alloc] init] autorelease];
        self.trees = [[[NSMutableSet alloc] init] autorelease];

        r = red;
        g = green;
        b = blue;
        
        planet_texture = texture_id;
        tree_texture = tree_texture_id;
        
        for (NSDictionary* treePlist in [self getTreesFromServer]) {
            Vector3D* pos = new Vector3D([[treePlist objectForKey:@"x"] floatValue], 
                                         [[treePlist objectForKey:@"y"] floatValue], 
                                         [[treePlist objectForKey:@"z"] floatValue]);
            [self addTreeAtPoint:pos];
            [self.growingTree setAge:[[treePlist objectForKey:@"age"] floatValue]];
            self.growingTree.freq = [[treePlist objectForKey:@"frequency"] floatValue];
            [self stopGrowingAndSendToServer:NO];
        }
    }
    return self;
}

- (NSArray*) getTreesFromServer
{
    NSArray* treesPlist;
    
    NSString *server = [[NSString alloc] initWithString:@"http://mikerotondo.com/wold/"];
    
    NSURL *url = [NSURL URLWithString:[server stringByAppendingFormat:@"planets/%d/trees",self.id]];
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

- (void) submitTreeToServer:(WOLSystem*)tree
{
	NSLog(@"about to send data to server...");
	
	NSString *server = [[NSString alloc] initWithUTF8String:"http://mikerotondo.com/wold/"];
	
	NSURL *url = [NSURL URLWithString:[server stringByAppendingFormat:@"planets/%d/trees", self.id]];
	
	ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
	
    // Setup the variables for the request
    [request setPostValue:[NSNumber numberWithFloat:tree.origin.x] forKey:@"x"];
    [request setPostValue:[NSNumber numberWithFloat:tree.origin.y] forKey:@"y"];
    [request setPostValue:[NSNumber numberWithFloat:tree.origin.z] forKey:@"z"];
    float age = tree.currentGeneration + ((float)tree.generationTickCount / tree.ticksPerGeneration);
    [request setPostValue:[NSNumber numberWithFloat:age] forKey:@"age"];
    [request setPostValue:[NSNumber numberWithFloat:tree.freq] forKey:@"frequency"];
    
	[request startSynchronous];  // TODO: Make this asynchronous to get rid of sound sputtering
    
	NSError *error = [request error];
	if (!error) {
		NSString *response = [request responseString];
        NSLog(@"POSTED TREE: %@", response);
	}	
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
}

- (void) addTreeAtPoint:(Vector3D*)point
{
    [self.lines addObject:[NSValue valueWithPointer:point]];
    
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
    
    WOLittleFNode* fNode = [[[WOLittleFNode alloc] init] autorelease];
    [tree.nodes addObject:fNode];
    fNode = [[[WOLittleFNode alloc] init] autorelease];
    [tree.nodes addObject:fNode];
    
    WOANode* aNode = [[[WOANode alloc] init] autorelease];
    [tree.nodes addObject:aNode];
    
    [self.trees addObject:tree];
    
    GLfloat xy_angle = tree.origin.angleXY();
    GLfloat xz_angle = tree.origin.angleXZ();
    GLfloat yz_angle = tree.origin.angleYZ();
    
    NSLog(@"ORIGIN: %f, %f, %f", tree.origin.x, tree.origin.y, tree.origin.z);
    NSLog(@"ANGLES: XY %f, XZ %f, YZ %f", xy_angle, xz_angle, yz_angle);
    
    self.growingTree = tree;
}

- (void) stopGrowingAndSendToServer:(BOOL)send
{
    if (send) {
        [self submitTreeToServer:self.growingTree];
    }
    self.growingTree = nil;
}

- (void) transform
{
    glTranslatef(self.position.x, self.position.y, self.position.z);
}

- (void) render
{
    glTranslatef(self.position.x, self.position.y, self.position.z);
    
    float lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    float lightSpecular[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    
    float matAmbient[] = { r, g, b, 1.0f };
    float matDiffuse[] = { r * 0.75f, g * 0.75f, b * 0.75f, 1.0f };
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
    
    glMultMatrixf(Transform.M);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);

    glBindTexture( GL_TEXTURE_2D, planet_texture );
    
    
    // enable texture mapping
    glEnable( GL_TEXTURE_2D ); // TODO: Move this around only the textured drawing!
    //[WOGeometry drawSphereWithRadius:self.radius andNumLats:20 andNumLongs:20];
    //[WOGeometry drawFrustumWithBottomRadius:self.radius / 2 andTopRadius:self.radius / 4 andHeight:self.radius / 2 andSections:40];
    glDisable(GL_TEXTURE_2D);
    
    //glBindTexture( GL_TEXTURE_2D, tree_texture);
    
    GLfloat xy_angle, xz_angle, yz_angle;
    for (NSValue* lineValue in self.lines) {
        Vector3D* line = (Vector3D*)[lineValue pointerValue];
        Vector3D endPoint(*line * self.radius);
        
        xy_angle = line->angleXY() * (180 / M_PI);
        yz_angle = line->angleYZ() * (180 / M_PI);
        
        glPushMatrix();
        glTranslatef(endPoint.x, endPoint.y, endPoint.z); // TODO: This is still wrong
        glRotatef(xy_angle, 0, 0, 1);

        //[WOGeometry drawSphereWithRadius:self.radius/10 andNumLats:6 andNumLongs:6];
        glPopMatrix();
    }
    
    for (WOLSystem* tree in self.trees) {

        xy_angle = tree.origin.angleXY();
        xz_angle = tree.origin.angleXZ();
        yz_angle = tree.origin.angleYZ();
        
        glPushMatrix();

        
        Vector3D treePoint = [tree origin] * self.radius;
        // TODO: This isn't quite right but is close enough for now. Fix it later.
        glTranslatef(treePoint.x, treePoint.y, treePoint.z);
        glRotatef(xy_angle * (180 / M_PI) - 90, 0.0f, 0.0f, 1.0f);        
        glRotatef(sin(yz_angle) * (180 / M_PI), 1.0, 0.0f, 0.0f);
        
        [tree render];
        glPopMatrix();
    }
    
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    
    glPopMatrix();
}

- (void) tick
{
    [growingTree tick];
}

@end
