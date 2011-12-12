//
//  ROGLProgram.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "ROGLProgram.h"

#warning These uniforms should be made PER-PROGRAM/PER-SHADER instead of globally! Ignoring for now.

// Uniform index.
enum
{
    UNIFORM_MODELVIEWPROJECTION_MATRIX,
    UNIFORM_NORMAL_MATRIX,
    UNIFORM_COLOR,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum
{
    ATTRIB_POSITION,
    ATTRIB_NORMAL,
    NUM_ATTRIBUTES
};

@implementation ROGLGouraudProgram

+ (ROGLGouraudProgram *)sharedInstance
{
    static ROGLGouraudProgram *sharedInstance;
    if (sharedInstance == nil)
    {
        sharedInstance = [[self alloc] init];
    }
    return sharedInstance;
}

- (id)init {
    self = [super initWithVertexShader:@"Gouraud" fragmentShader:@"Gouraud"];
    if (self) {
    }
    return self;
}

- (void)setModelViewProjectionMatrix:(GLKMatrix4)modelViewProjectionMatrix normalMatrix:(GLKMatrix3)normalMatrix
{
    glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, modelViewProjectionMatrix.m);
    glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, normalMatrix.m);
}

- (void)setColor:(GLKVector3)color
{
    glUniform3fv(uniforms[UNIFORM_COLOR], 1, color.v);
}

@end

@interface ROGLProgram ()
{
    GLuint program;
}
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;

@end

@implementation ROGLProgram

- (id)initWithVertexShader:(NSString *)vertexShaderFilename fragmentShader:(NSString *)fragmentShaderFilename
{
    self = [super init];
    if (self) {
        GLuint vertShader, fragShader;
        NSString *vertShaderPath, *fragShaderPath;
        
        // Create shader program.
        program = glCreateProgram();
        
        // Create and compile vertex shader.
        vertShaderPath = [[NSBundle mainBundle] pathForResource:vertexShaderFilename ofType:@"vsh"];
        if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPath]) {
            NSLog(@"Failed to compile vertex shader");
            self = nil;
            return nil;
        }
        
        // Create and compile fragment shader.
        fragShaderPath = [[NSBundle mainBundle] pathForResource:fragmentShaderFilename ofType:@"fsh"];
        if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPath]) {
            NSLog(@"Failed to compile fragment shader");
            self = nil;
            return nil;
        }
        
        // Attach vertex shader to program.
        glAttachShader(program, vertShader);
        
        // Attach fragment shader to program.
        glAttachShader(program, fragShader);
        
        // Bind attribute locations.
        // This needs to be done prior to linking.
        glBindAttribLocation(program, ATTRIB_POSITION, "position");
        glBindAttribLocation(program, ATTRIB_NORMAL, "normal");
        
        // Link program.
        if (![self linkProgram:program]) {
            NSLog(@"Failed to link program: %d", program);
            
            if (vertShader) {
                glDeleteShader(vertShader);
                vertShader = 0;
            }
            if (fragShader) {
                glDeleteShader(fragShader);
                fragShader = 0;
            }
            if (program) {
                glDeleteProgram(program);
                program = 0;
            }
            
            self = nil;
            return nil;
        }
        
        // Get uniform locations.
        uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(program, "modelViewProjectionMatrix");
        uniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(program, "normalMatrix");
        uniforms[UNIFORM_COLOR] = glGetUniformLocation(program, "color");
        
        // Release vertex and fragment shaders.
        if (vertShader) {
            glDetachShader(program, vertShader);
            glDeleteShader(vertShader);
        }
        if (fragShader) {
            glDetachShader(program, fragShader);
            glDeleteShader(fragShader);
        }
    }
    return self;
}

- (void)dealloc {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}

- (void)use
{
    glUseProgram(program);
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source) {
        NSLog(@"Failed to load vertex shader");
        return NO;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return NO;
    }
    
    return YES;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

@end
