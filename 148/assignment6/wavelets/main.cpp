// main.cpp
//
// Starter code for CS 148 Assignment 5.
//

//
// Include libst headers for using OpenGL and GLUT.
//
#include "st.h"
#include "stgl.h"
#include "stglut.h"

#include "Wavelet.h"

float MY_GREAT_RANDOM_NUMBER();

Wavelet *wavelet;

//
// GLUT display callback
//
void display()
{
    glClearColor(MY_GREAT_RANDOM_NUMBER(), MY_GREAT_RANDOM_NUMBER(), MY_GREAT_RANDOM_NUMBER(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glutSwapBuffers();
}

//
// Reshape window
//
void reshape(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluOrtho2D(0., 1., 0., 1.);
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glutPostRedisplay();
}

// Keyboard event handler:
static void keyboard(unsigned char key, int x, int y) {
    switch (key) {
            // quit
        case '+':
        case '=':
            break;
        case char(27):
        case 'q':
        case 'Q':
            exit(0);
    }
    
    glutPostRedisplay();
}

static void usage() {
    printf("Usage:\n");
    printf("./Wavelet [optional filename of image]\n");
    exit(-1);
}

int main(int argc, char** argv)
{
    //
    // Initialize GLUT.
    //
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(512, 512);
    glutCreateWindow("CS148 Wavelet Compression!");
    
    std::string filename = "MonaLisa.png";
    
    if (argc > 2) {
        usage();
    }
    
    if ( argc == 2) {
        filename = argv[1];
    }

    wavelet = new Wavelet(filename);
        
    glutReshapeWindow(512, 512);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMainLoop();
    
    return 0;
}

float MY_GREAT_RANDOM_NUMBER()
{
    return (arc4random() / (float)0x100000000);
}
