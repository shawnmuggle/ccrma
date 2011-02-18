//
//  WOGeometry.mm
//  wold
//
//  Created by Michael Rotondo on 2/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOGeometry.h"

#import "mo_gfx.h"

GLuint sphereVBO;

// TODO: Use display lists to speed these bitches up.
@implementation WOGeometry

// TODO: Use triangle fans at the top and bottom instead of triangle strips, to have less redundancy?
+ (void)drawSphereWithRadius:(GLfloat)r andNumLats:(GLint)lats andNumLongs:(GLint)longs
{
    GLfloat psi, theta, theta_next, x1, y1, z1, x2, y2, z2;
    
    for (int i = 0; i < lats + 1; i++) {
        
        int numVertices = 2 * longs;
        
        GLfloat vertices[numVertices * 3];
        GLfloat normals[numVertices * 3];
        GLfloat texCoords[numVertices * 2];
        
        theta = M_PI * (i / (float)(lats + 1));
        theta_next = M_PI * ((i + 1) / (float)(lats + 1));
        for (int j = 0; j < longs; j++) {
            psi = 2 * M_PI * (j / (float)(longs - 1));
            
            x1 = r * sin(theta) * cos(psi);
            y1 = r * sin(theta) * sin(psi);
            z1 = r * cos(theta);
            
            x2 = r * sin(theta_next) * cos(psi);
            y2 = r * sin(theta_next) * sin(psi);
            z2 = r * cos(theta_next);
            
            vertices[/*i * longs * 3 * 2 +*/ j * 3 * 2] = x1;
            vertices[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 1] = y1;
            vertices[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 2] = z1;
            
            vertices[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 3] = x2;
            vertices[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 4] = y2;
            vertices[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 5] = z2;
            
            Vector3D norm1(x1, y1, z1);
            norm1.normalize();
            
            normals[/*i * longs * 3 * 2 +*/ j * 3 * 2] = norm1.x;
            normals[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 1] = norm1.y;
            normals[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 2] = norm1.z;
            
            Vector3D norm2(x2, y2, z2);
            norm2.normalize();
            
            normals[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 3] = norm2.x;
            normals[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 4] = norm2.y;
            normals[/*i * longs * 3 * 2 +*/ j * 3 * 2 + 5] = norm2.z;
            
            
            // SHITTY TEXTURE MAPPING
            texCoords[/*i * longs * 2 * 2 +*/ j * 2 * 2] = psi / (2 * M_PI);//sin(theta) * cos(psi);
            texCoords[/*i * longs * 2 * 2 +*/ j * 2 * 2 + 1] = theta / M_PI;//sin(theta) * sin(psi);
            
            texCoords[/*i * longs * 2 * 2 +*/ j * 2 * 2 + 2] = psi / (2 * M_PI);//sin(theta_next) * cos(psi);
            texCoords[/*i * longs * 2 * 2 +*/ j * 2 * 2 + 3] = theta_next / M_PI;//sin(theta_next) * sin(psi);
        }
        
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glNormalPointer(GL_FLOAT, 0, normals);
        glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices);
    }
}

+ (void)drawFrustumWithBottomRadius:(GLfloat)rBottom andTopRadius:(GLfloat)rTop andHeight:(GLfloat)h andSections:(GLint)sections
{
    int numTubeVertices = (sections + 1) * 2;
    int numFanVertices = sections + 2;
    
    GLfloat bottomFanVertices[numFanVertices * 3];
    GLfloat bottomFanNormals[numFanVertices * 3];
    GLfloat bottomFanTexCoords[numFanVertices * 2];
    bottomFanVertices[0] = 0.0f;
    bottomFanVertices[1] = 0.0f;
    bottomFanVertices[2] = 0.0f;
    
    bottomFanNormals[0] = 0.0f;
    bottomFanNormals[1] = -1.0f;
    bottomFanNormals[2] = 0.0f;
    
    bottomFanTexCoords[0] = 0.5;
    bottomFanTexCoords[1] = 0.5;
    
    GLfloat topFanVertices[numFanVertices * 3];
    GLfloat topFanNormals[numFanVertices * 3];
    GLfloat topFanTexCoords[numFanVertices * 2];
    topFanVertices[0] = 0.0f;
    topFanVertices[1] = h;
    topFanVertices[2] = 0.0f;
    
    topFanNormals[0] = 0.0f;
    topFanNormals[1] = 1.0f;
    topFanNormals[2] = 0.0f;

    topFanTexCoords[0] = 0.5;
    topFanTexCoords[1] = 0.5;
    
    GLfloat tubeVertices[numTubeVertices * 3];
    GLfloat tubeNormals[numTubeVertices * 3];
    GLfloat tubeTexCoords[numTubeVertices * 2];
    
    GLfloat percent, theta, bottomX, topX, bottomZ, topZ;
    Vector3D norm;
    for( int i = 0; i < numFanVertices - 1; i++) {
        percent = i / (GLfloat)(numFanVertices - 2);
        theta = 2 * M_PI * percent;
        
        bottomX = rBottom * cos(theta);
        bottomZ = rBottom * sin(theta);
        topX = rTop * cos(theta);
        topZ = rTop * sin(theta);
        
        bottomFanVertices[(i + 1) * 3] = bottomX;
        bottomFanVertices[(i + 1) * 3 + 1] = 0.0f;
        bottomFanVertices[(i + 1) * 3 + 2] = bottomZ;
        
        bottomFanNormals[(i + 1) * 3] = 0.0f;
        bottomFanNormals[(i + 1) * 3 + 1] = -1.0f;
        bottomFanNormals[(i + 1) * 3 + 2] = 0.0f;
        
        bottomFanTexCoords[(i + 1) * 2] = 0.5 + bottomX / 2;
        bottomFanTexCoords[(i + 1) * 2 + 1] = 0.5 + bottomZ / 2;
        
        // NOTE: This is written in reverse so that it winds correctly and OpenGL recognizes it as facing in the opposite direction from the bottom
        topFanVertices[(numFanVertices - (i + 1)) * 3] = topX;
        topFanVertices[(numFanVertices - (i + 1)) * 3 + 1] = h;
        topFanVertices[(numFanVertices - (i + 1)) * 3 + 2] = topZ;
        
        topFanNormals[(i + 1) * 3] = 0.0f;
        topFanNormals[(i + 1) * 3 + 1] = 1.0f;
        topFanNormals[(i + 1) * 3 + 2] = 0.0f;
        
        topFanTexCoords[(i + 1) * 2] = 0.5 + topX / 2;
        topFanTexCoords[(i + 1) * 2 + 1] = 0.5 + topZ / 2;
        
        tubeVertices[i * 3 * 2] = bottomX;
        tubeVertices[i * 3 * 2 + 1] = 0.0f;
        tubeVertices[i * 3 * 2 + 2] = bottomZ;
        tubeVertices[i * 3 * 2 + 3] = topX;
        tubeVertices[i * 3 * 2 + 4] = h;
        tubeVertices[i * 3 * 2 + 5] = topZ;
        
        norm.x = bottomX;
        norm.z = bottomZ;
        norm.normalize();
        tubeNormals[i * 3 * 2] = norm.x;
        tubeNormals[i * 3 * 2 + 1] = 0.0f;
        tubeNormals[i * 3 * 2 + 2] = norm.z;
        norm.x = topX;
        norm.z = topZ;
        norm.normalize();
        tubeNormals[i * 3 * 2 + 3] = norm.x;
        tubeNormals[i * 3 * 2 + 4] = 0.0f;
        tubeNormals[i * 3 * 2 + 5] = norm.z;
        
        tubeTexCoords[i * 2 * 2] = percent;
        tubeTexCoords[i * 2 * 2 + 1] = 0.0f;
        tubeTexCoords[i * 2 * 2 + 2] = percent;
        tubeTexCoords[i * 2 * 2 + 3] = 1.0f;
        
    }

    glVertexPointer(3, GL_FLOAT, 0, tubeVertices);
    glNormalPointer(GL_FLOAT, 0, tubeNormals);
    glTexCoordPointer( 2, GL_FLOAT, 0, tubeTexCoords );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numTubeVertices);

    glVertexPointer(3, GL_FLOAT, 0, bottomFanVertices);
    glNormalPointer(GL_FLOAT, 0, bottomFanNormals);
    glTexCoordPointer( 2, GL_FLOAT, 0, bottomFanTexCoords );
    glDrawArrays(GL_TRIANGLE_FAN, 0, numFanVertices);

    glVertexPointer(3, GL_FLOAT, 0, topFanVertices);
    glNormalPointer(GL_FLOAT, 0, topFanNormals);
    glTexCoordPointer( 2, GL_FLOAT, 0, topFanTexCoords );
    glDrawArrays(GL_TRIANGLE_FAN, 0, numFanVertices);
}

//
//// SADNESS but I need these in class methods.
//// Disk geometry:
//GLfloat* diskBottomVertices;
//GLfloat* diskBottomNormals;
//GLfloat* diskBottomTexCoords;
//GLfloat* diskTopVertices;
//GLfloat* diskTopNormals;
//GLfloat* diskTopTexCoords;
//int numDiskVertices;
//
//+ (void) generateDisk
//{
//    // Assume 5 sections (like leaves have, duh)
//    int sections = 5;
//    GLfloat h = 0.0001;
//    GLfloat r = 1.0; // Scale it later!
//    numDiskVertices = sections + 2;
//    
//    diskBottomVertices = new GLfloat[numDiskVertices * 3];
//    diskBottomNormals = new GLfloat[numDiskVertices * 3];
//    diskBottomTexCoords = new GLfloat[numDiskVertices * 2];
//    diskBottomVertices[0] = 0.0f;
//    diskBottomVertices[1] = 0.0f;
//    diskBottomVertices[2] = 0.0f;
//    
//    diskBottomNormals[0] = 0.0f;
//    diskBottomNormals[1] = -1.0f;
//    diskBottomNormals[2] = 0.0f;
//    
//    diskBottomTexCoords[0] = 0.5;
//    diskBottomTexCoords[1] = 0.5;
//    
//    diskTopVertices = new GLfloat[numDiskVertices * 3];
//    diskTopNormals = new GLfloat[numDiskVertices * 3];
//    diskTopTexCoords = new GLfloat[numDiskVertices * 2];
//    diskTopVertices[0] = 0.0f;
//    diskTopVertices[1] = h;
//    diskTopVertices[2] = 0.0f;
//    
//    diskTopNormals[0] = 0.0f;
//    diskTopNormals[1] = 1.0f;
//    diskTopNormals[2] = 0.0f;
//    
//    diskTopTexCoords[0] = 0.5;
//    diskTopTexCoords[1] = 0.5;
//    
//    GLfloat percent, theta, x, z;
//    Vector3D norm;
//    for( int i = 0; i < numDiskVertices - 1; i++) {
//        percent = i / (GLfloat)(numDiskVertices - 2);
//        theta = 2 * M_PI * percent;
//        
//        x = r * cos(theta);
//        z = r * sin(theta);
//        
//        diskBottomVertices[(i + 1) * 3] = x;
//        diskBottomVertices[(i + 1) * 3 + 1] = 0.0f;
//        diskBottomVertices[(i + 1) * 3 + 2] = z;
//        
//        diskBottomNormals[(i + 1) * 3] = 0.0f;
//        diskBottomNormals[(i + 1) * 3 + 1] = -1.0f;
//        diskBottomNormals[(i + 1) * 3 + 2] = 0.0f;
//        
//        diskBottomTexCoords[(i + 1) * 2] = 0.5 + x / 2;
//        diskBottomTexCoords[(i + 1) * 2 + 1] = 0.5 + z / 2;
//        
//        // NOTE: This is written in reverse so that it winds correctly and OpenGL recognizes it as facing in the opposite direction from the bottom
//        diskTopVertices[(numDiskVertices - (i + 1)) * 3] = x;
//        diskTopVertices[(numDiskVertices - (i + 1)) * 3 + 1] = h;
//        diskTopVertices[(numDiskVertices - (i + 1)) * 3 + 2] = z;
//        
//        diskTopNormals[(i + 1) * 3] = 0.0f;
//        diskTopNormals[(i + 1) * 3 + 1] = 1.0f;
//        diskTopNormals[(i + 1) * 3 + 2] = 0.0f;
//        
//        diskTopTexCoords[(i + 1) * 2] = 0.5 + x / 2;
//        diskTopTexCoords[(i + 1) * 2 + 1] = 0.5 + z / 2;
//    }
//
//}
//
//+ (void)drawDiskWithRadius:(GLfloat)r andSections:(GLint)sections
//{
//    glVertexPointer(3, GL_FLOAT, 0, diskBottomVertices);
//    glNormalPointer(GL_FLOAT, 0, diskBottomNormals);
//    glTexCoordPointer( 2, GL_FLOAT, 0, diskBottomTexCoords );
//    glDrawArrays(GL_TRIANGLE_FAN, 0, numDiskVertices);
//     
//    glVertexPointer(3, GL_FLOAT, 0, diskTopVertices);
//    glNormalPointer(GL_FLOAT, 0, diskTopNormals);
//    glTexCoordPointer( 2, GL_FLOAT, 0, diskTopTexCoords );
//    glDrawArrays(GL_TRIANGLE_FAN, 0, numDiskVertices);
//}

+ (void)drawDiskWithRadius:(GLfloat)r andSections:(GLint)sections
{
    GLfloat h = 0.0001;
    int numFanVertices = sections + 2;
    
    GLfloat bottomFanVertices[numFanVertices * 3];
    GLfloat bottomFanNormals[numFanVertices * 3];
    GLfloat bottomFanTexCoords[numFanVertices * 2];
    bottomFanVertices[0] = 0.0f;
    bottomFanVertices[1] = 0.0f;
    bottomFanVertices[2] = 0.0f;
    
    bottomFanNormals[0] = 0.0f;
    bottomFanNormals[1] = -1.0f;
    bottomFanNormals[2] = 0.0f;
    
    bottomFanTexCoords[0] = 0.5;
    bottomFanTexCoords[1] = 0.5;
    
    GLfloat topFanVertices[numFanVertices * 3];
    GLfloat topFanNormals[numFanVertices * 3];
    GLfloat topFanTexCoords[numFanVertices * 2];
    topFanVertices[0] = 0.0f;
    topFanVertices[1] = h;
    topFanVertices[2] = 0.0f;
    
    topFanNormals[0] = 0.0f;
    topFanNormals[1] = 1.0f;
    topFanNormals[2] = 0.0f;
    
    topFanTexCoords[0] = 0.5;
    topFanTexCoords[1] = 0.5;
    
    GLfloat percent, theta, x, z;
    Vector3D norm;
    for( int i = 0; i < numFanVertices - 1; i++) {
        percent = i / (GLfloat)(numFanVertices - 2);
        theta = 2 * M_PI * percent;
        
        x = r * cos(theta);
        z = r * sin(theta);
        
        bottomFanVertices[(i + 1) * 3] = x;
        bottomFanVertices[(i + 1) * 3 + 1] = 0.0f;
        bottomFanVertices[(i + 1) * 3 + 2] = z;
        
        bottomFanNormals[(i + 1) * 3] = 0.0f;
        bottomFanNormals[(i + 1) * 3 + 1] = -1.0f;
        bottomFanNormals[(i + 1) * 3 + 2] = 0.0f;
        
        bottomFanTexCoords[(i + 1) * 2] = 0.5 + x / 2;
        bottomFanTexCoords[(i + 1) * 2 + 1] = 0.5 + z / 2;
        
        // NOTE: This is written in reverse so that it winds correctly and OpenGL recognizes it as facing in the opposite direction from the bottom
        topFanVertices[(numFanVertices - (i + 1)) * 3] = x;
        topFanVertices[(numFanVertices - (i + 1)) * 3 + 1] = h;
        topFanVertices[(numFanVertices - (i + 1)) * 3 + 2] = z;
        
        topFanNormals[(i + 1) * 3] = 0.0f;
        topFanNormals[(i + 1) * 3 + 1] = 1.0f;
        topFanNormals[(i + 1) * 3 + 2] = 0.0f;
        
        topFanTexCoords[(i + 1) * 2] = 0.5 + x / 2;
        topFanTexCoords[(i + 1) * 2 + 1] = 0.5 + z / 2;
    }

    glVertexPointer(3, GL_FLOAT, 0, bottomFanVertices);
    glNormalPointer(GL_FLOAT, 0, bottomFanNormals);
    glTexCoordPointer( 2, GL_FLOAT, 0, bottomFanTexCoords );
    glDrawArrays(GL_TRIANGLE_FAN, 0, numFanVertices);
    
    glVertexPointer(3, GL_FLOAT, 0, topFanVertices);
    glNormalPointer(GL_FLOAT, 0, topFanNormals);
    glTexCoordPointer( 2, GL_FLOAT, 0, topFanTexCoords );
    glDrawArrays(GL_TRIANGLE_FAN, 0, numFanVertices);
}


@end
