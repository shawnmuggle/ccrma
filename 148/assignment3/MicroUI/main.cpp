// main.cpp
//
// Starter code for CS 148 Assignment 3.
//

//
// Include libst headers for using OpenGL and GLUT.
//
#include "st.h"
#include "stgl.h"
#include "stglut.h"

//
// Include headers for UI clases.
//
#include "UIBox.h"
#include "UIDraggableBox.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIWidget.h"

//
// Include header for parsing config and line editor files
//
#include "parseConfig.h"

#include <set>

#define WIN_WIDTH 1024
#define WIN_HEIGHT 600

//
// Globals used by this application.
// As a rule, globals are Evil, but this is a small application
// and the design of GLUT makes it hard to avoid them.
//

// Font to use for text in the UI.
static STFont* gFont = NULL;

// List of widgets being displayed.
static std::set<UIWidget *> gWidgets;

// Widgets that have captured mouse input
static UIWidget * gCapturedWidget = NULL;

// Widget that the mouse is currently over
static UIWidget *gCurrentMouseOverWidget = NULL;

// Window size so we can properly update the UI.
static int gWindowSizeX = 0;
static int gWindowSizeY = 0;

// Background images
static STImage* gBgIm1;
static STImage* gBgIm2;

// Buttons
static UIButton *quitButton;
static UIButton *deleteButton;

#ifndef BUFSIZ
#define BUFSIZ  512
#endif
// Background image filenames parsed from config file
static char gImage1Fname[BUFSIZ];
static char gImage2Fname[BUFSIZ];

// Line editor load/save filenames parsed from config file
static char gSaveFname[BUFSIZ];
static char gLoadFname[BUFSIZ];

//
// Add a widget to the list of widgets in the window.
// Also sets the rectangle of the widget to the one specified.
//
void AddWidget(UIWidget* widget, const UIRectangle& rectangle)
{
    widget->SetRectangle(rectangle);
    gWidgets.insert(widget);
}

void RemoveWidget(UIWidget* widget)
{
    gWidgets.erase(widget);
}

void CaptureMouseActivity(UIWidget *widget)
{
    // Allow the button to capture future mouse events, until it 
    gCapturedWidget = widget;    
}

void UncaptureMouseActivity()
{
    gCapturedWidget = NULL;
}

class LinePair;
static LinePair *selectedLinePair;
static std::set<LinePair *> linePairs;

void ButtonCallback( UIButton* whichButton );

class Line {
public:
    Line(const STPoint2 &p1, const STPoint2 &p2, STColor4f c, bool clickedToCreate)
    {
        endPoint1 = new UIDraggableBox(STColor4f(0.0, 1.0, 0.0));
        endPoint2 = new UIDraggableBox(STColor4f(1.0, 0.0, 0.0));
        color = c;
        
        static float endBoxSize = 10.0f;
        AddWidget(endPoint1, UIRectangle(STPoint2(p1.x - endBoxSize / 2, p1.y - endBoxSize / 2),
                                         STPoint2(p1.x + endBoxSize / 2, p1.y + endBoxSize / 2)));
        AddWidget(endPoint2, UIRectangle(STPoint2(p2.x - endBoxSize / 2, p2.y - endBoxSize / 2),
                                         STPoint2(p2.x + endBoxSize / 2, p2.y + endBoxSize / 2)));
        
        selected = true;
        
        if (clickedToCreate)
        {
            // Indicates that this is the line that was created by clicking, as opposed to the one on the other image that is a duplicate
            endPoint2->HandleMouseEnter();
            endPoint2->HandleMouseDown(p2);
            CaptureMouseActivity(endPoint2);
        }
    }
    
    ~Line()
    {
        RemoveWidget(endPoint1);
        RemoveWidget(endPoint2);
        if (gCurrentMouseOverWidget == endPoint1 || gCurrentMouseOverWidget == endPoint2)
        {
            gCurrentMouseOverWidget = NULL;
        }
        if (gCapturedWidget == endPoint1 || gCapturedWidget == endPoint2)
        {
            UncaptureMouseActivity();
        }
        delete endPoint1;
        delete endPoint2;
    }
    
    void Display()
    {
        glColor3f(color.r, color.g, color.b);
        if (selected)
            glColor3f(1.0f, 1.0f, 1.0f);
        STPoint2 p1 = endPoint1->GetCenter();
        STPoint2 p2 = endPoint2->GetCenter();
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
    }
    
    STVector2 Vector()
    {
        return endPoint2->GetCenter() - endPoint1->GetCenter();
    }
    
    float Length()
    {
        return Vector().Length();
    }
    
    void UpdateWithVector(STVector2 vector)
    {
        STPoint2 p2 = endPoint1->GetCenter() + vector;
        static float endBoxSize = 10.0f;
        endPoint2->SetRectangle(UIRectangle(STPoint2(p2.x - endBoxSize / 2, p2.y - endBoxSize / 2),
                                            STPoint2(p2.x + endBoxSize / 2, p2.y + endBoxSize / 2)));
    }
    
    bool CurrentlyBeingModified()
    {
        return endPoint1 == gCapturedWidget || endPoint2 == gCapturedWidget;
    }
    
    bool selectableWithMousePosition(STPoint2 mousePosition)
    {
        STVector2 mouseVector = mousePosition - endPoint1->GetCenter();
        float r = STVector2::Dot(Vector(), mouseVector) / Vector().LengthSq();
        STPoint2 projectedPoint = endPoint1->GetCenter() + Vector() * r;
        return r > -0.04 && r < 1.04 && (mousePosition - projectedPoint).LengthSq() < 500;
    }
    
    void Select()
    {
        selected = true;
    }
    
    void Deselect()
    {
        selected = false;
    }
    
private:
    STColor4f color;
    UIDraggableBox *endPoint1;
    UIDraggableBox *endPoint2;
    bool selected;
};

class LinePair {
public:
    LinePair(Line *l1, Line *l2)
    {
        img1Line = l1;
        img2Line = l2;
    }
    
    ~LinePair()
    {
        printf("DELETING LINE PAIR");
        linePairs.erase(this);
        delete img1Line;
        delete img2Line;
    }
    
    void Display()
    {
        img1Line->Display();
        glPushMatrix();
        glTranslated(512, 0, 0);
        img2Line->Display();
        glPopMatrix();
    }
    
    void MaybeUpdateZeroLengthLine()
    {
        if (img1Line->Length() < 0.01 && !img2Line->CurrentlyBeingModified())
        {
            img1Line->UpdateWithVector(img2Line->Vector());
        }
        else if (img2Line->Length() < 0.01 && !img1Line->CurrentlyBeingModified())
        {
            img2Line->UpdateWithVector(img1Line->Vector());
        }
    }
    
    bool MaybeSelectWithMousePosition(STPoint2 mousePosition)
    {
        if (!img1Line->CurrentlyBeingModified() && 
            !img2Line->CurrentlyBeingModified() && 
            (img1Line->selectableWithMousePosition(mousePosition) || img2Line->selectableWithMousePosition(mousePosition)))
        {
            Select();
            if (selectedLinePair != this)
            {
                selectedLinePair = this;
                const std::string deleteString = "DELETE";
                float deleteWidth = gFont->ComputeWidth(deleteString);
                float height = gFont->GetHeight();
                deleteButton = new UIButton(gFont, deleteString, ButtonCallback);
                AddWidget(deleteButton, UIRectangle(STPoint2(mousePosition.x - deleteWidth / 2, mousePosition.y - height / 2),
                                                    STPoint2(mousePosition.x + deleteWidth / 2, mousePosition.y + height / 2)));
            }

            return true;
        }
        else
        {
            Deselect();
            if (selectedLinePair == this)
            {
                selectedLinePair = NULL;
                RemoveWidget(deleteButton);
            }
            return false;
        }
    }
    
    void Select()
    {
        img2Line->Select();
        img1Line->Select();        
    }
    
    void Deselect()
    {
        img1Line->Deselect();
        img2Line->Deselect();        
    }

    void Delete()
    {
        RemoveWidget(deleteButton);
        delete this;
    }
    
private:
    Line *img1Line;
    Line *img2Line;
};


// Button callbacks (unbound global functions, gross)
void ButtonCallback( UIButton* whichButton )
{
    printf("The button fired!\n");
    if (whichButton == quitButton)
    {
        exit(0);
    }
    else if (whichButton == deleteButton)
    {
        if (selectedLinePair)
        {
            selectedLinePair->Delete();
        }
    }
}

// Creates any widgets or other objects used for displaying lines.
// lineEndpt1 and lineEndpt2 should both be coordindates RELATIVE TO THE IMAGE
// THEY ARE DRAWN TO. This means that if the lower left of one image is at
// (15,15), if lineEndpt1 is (10,10), it would actually be drawn at window
// coordinates (25,25).
//
// The imageChoice parameter tells you whether a line is being drawn to both images,
// or to a single images. When the user is manually adding lines, this parameter
// should be BOTH_IMAGES. When this function is called by loadLineEditorFile
// (already implemented), imageChoice will take on the value of either IMAGE_1
// or IMAGE_2.
void AddNewLine(STPoint2 lineEndpt1, STPoint2 lineEndpt2, ImageChoice imageChoice)
{
    /** CS148 TODO : ONLY REQUIRED FOR EXTRA CREDIT
     *
     * Use this to create any widgets and objects you use to display and edit lines.
     */
}

void CreateNewLinePair(STPoint2 startPoint)
{
    STColor4f color(0.4 + 0.6 *(rand() / (float)RAND_MAX),
                    0.4 + 0.6 *(rand() / (float)RAND_MAX),
                    0.4 + 0.6 *(rand() / (float)RAND_MAX));
    
    bool clickedOnLeftImage = true;
    STPoint2 leftPoint, rightPoint;
    if (startPoint.x <= 512)
    {
        leftPoint = startPoint;
        rightPoint = STPoint2(startPoint.x + 512, startPoint.y);
    }
    else if (startPoint.x > 512)
    {
        clickedOnLeftImage = false;
        rightPoint = startPoint;
        leftPoint = STPoint2(startPoint.x - 512, startPoint.y);
    }

    Line *l1 = new Line(leftPoint, leftPoint, color, clickedOnLeftImage);
    Line *l2 = new Line(rightPoint, rightPoint, color, !clickedOnLeftImage);
    LinePair *lines = new LinePair(l1, l2);
    linePairs.insert(lines);
}

//
// Setup routine.
//
// As you progress with implementing the assignment,
// you will want to modify this routine to create and
// test your new widget types.
//
void CreateWidgets()
{
    /** CS 148 TODO:
    *
    * This function should create any widgets on display
    * when the line editor begins. You should also use
    * it during testing to play with new widget types.
    */
 
    // Title
    const std::string title = "1-800-LINES-4-U";
    float titleWidth = gFont->ComputeWidth(title);
    float height = gFont->GetHeight();
    AddWidget(new UILabel(gFont, title), UIRectangle(STPoint2(WIN_WIDTH / 2 - titleWidth / 2, WIN_HEIGHT - height),
                                                     STPoint2(WIN_WIDTH / 2 + titleWidth / 2, WIN_HEIGHT)));
    
    const std::string quit = "QUIT";
    float quitWidth = gFont->ComputeWidth(quit);
    quitButton = new UIButton(gFont, quit, ButtonCallback);
    AddWidget(quitButton, UIRectangle(STPoint2(WIN_WIDTH - quitWidth, WIN_HEIGHT - height),
                                      STPoint2(WIN_WIDTH, WIN_HEIGHT)));
}

//
// Display the UI, including all widgets.
//
void DisplayCallback()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Clear the window.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gBgIm1->Draw();

    glPushMatrix();
    glTranslated(512, 0, 0);
    gBgIm2->Draw();
    glPopMatrix();
    
    // Loop through all the widgets in the user
    // interface and tell each to display itself.
    for (std::set<UIWidget *>::iterator i = gWidgets.begin(); i!= gWidgets.end(); i++)
    {
        UIWidget* widget = *i;
        widget->Display();        
    }
    
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for (std::set<LinePair *>::iterator i = linePairs.begin(); i != linePairs.end(); i++)
    {
        LinePair *linePair = *i;
        linePair->Display();        
    }
    glEnd();

    glutSwapBuffers();
}

void update(int unused)
{
    glutPostRedisplay();
    glutTimerFunc(16.0f, update, 0);
    
    // For any line pair where one of the lines has its start & end points at the same location (that is, it was just created)
    // and the other point is NOT being dragged (that is, the user has completed their initial drag placement), we update the
    // former point to have same vector from its start point.
    for (std::set<LinePair *>::iterator i = linePairs.begin(); i != linePairs.end(); i++)
    {
        LinePair *linePair = *i;
        linePair->MaybeUpdateZeroLengthLine();
    }
}

//
// Reshape the window and record the size so
// that we can use it in the mouse callbacks.
//
void ReshapeCallback(int w, int h)
{
    gWindowSizeX = w;
    gWindowSizeY = h;

    glViewport(0, 0, gWindowSizeX, gWindowSizeY);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0, gWindowSizeX, 0, gWindowSizeY, -1., 1.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

UIWidget *getMouseOverWidget(STPoint2 mousePosition)
{
    bool hit = false;
    UIWidget *widget;
    for (std::set<UIWidget *>::reverse_iterator i = gWidgets.rbegin(); i!= gWidgets.rend(); i++)
    {
        widget = *i;
        if (widget->HitTest(mousePosition))
        {
            hit = true;
            break;
        }
    }

    if (hit)
        return widget;
    else
        return NULL;
}

// Mouse callbacks
void mouseButtonCallback(int button, int state, int x, int y)
{
    STPoint2 mousePosition(x, gWindowSizeY - y);
    UIWidget *widget = getMouseOverWidget(mousePosition);
    
    // If a widget has captured mouse activity, then it must have previously gotten a mouse down
    // That means the mouse button is currently down, so if it goes up, alert the capturing widget.
    if (gCapturedWidget && state == 1)
    {
        gCapturedWidget->HandleMouseUp(mousePosition);
        if (gCapturedWidget == widget)
        {
            // The captured widget is the same one that the mouseup happened over, so don't process it twice.
            widget = NULL;
        }
        gCapturedWidget = NULL;
    }
    
    // If the mouse isn't over a widget (or we already processed the mouseup), create a line and then stop processing the button activity
    if (!widget)
    {
        if (state == 0)
        {
            CreateNewLinePair(mousePosition);
        }
        return;
    }
    
    if (state == 0)
    {
        // mousedown on a widget
        widget->HandleMouseDown(mousePosition);
        CaptureMouseActivity(widget);
    }
    else if (state == 1)
    {
        // mouseup on a widget
        widget->HandleMouseUp(mousePosition);
        UncaptureMouseActivity();
    }
}

void mouseMotionCallback(int x, int y)
{
    STPoint2 mousePosition(x, gWindowSizeY - y);
    UIWidget *widget = getMouseOverWidget(mousePosition);
    
    for (std::set<LinePair *>::iterator i = linePairs.begin(); i != linePairs.end(); i++)
    {
        LinePair *linePair = *i;
        linePair->Deselect();
    }

    for (std::set<LinePair *>::iterator i = linePairs.begin(); i != linePairs.end(); i++)
    {
        LinePair *linePair = *i;
        if (linePair->MaybeSelectWithMousePosition(mousePosition))
            break;
    }
    
    if (gCapturedWidget)
    {
        gCapturedWidget->HandleMouseMove(mousePosition);
    }
    
    if (!widget)
    {
        if (gCurrentMouseOverWidget)
        {
            // If we aren't over a widget, but there is a widget that we were previously over, tell it that the mouse left it
            gCurrentMouseOverWidget->HandleMouseLeave();
            gCurrentMouseOverWidget = NULL;
        }
        return;
    }
    
    if (widget != gCurrentMouseOverWidget)
    {
        // If the mouse moved 
        if (gCurrentMouseOverWidget)
            gCurrentMouseOverWidget->HandleMouseLeave();
        widget->HandleMouseEnter();
    }
    gCurrentMouseOverWidget = widget;
}

//
// Initialize the application, loading resources,
// setting state, and creating widgets.
//
void Initialize()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    gFont = new STFont("resources/arial.ttf", 24);

    CreateWidgets();
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    
    //
    // Initialize GLUT.
    //
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(
        WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("CS148 Assignment 3");
    
    glutTimerFunc(16.0f, update, 0);

    //
    // Initialize the UI.
    //
    Initialize();

    //
    // Parse config file
    //
    parseConfigFile(
        "config.txt",
        gImage1Fname,
        gImage2Fname,
        gSaveFname,
        gLoadFname,
        &gBgIm1,
        &gBgIm2);

    //
    // Register GLUT callbacks and enter main loop.
    //
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutMouseFunc(mouseButtonCallback);
    glutMotionFunc(mouseMotionCallback);
    glutPassiveMotionFunc(mouseMotionCallback);

    /** CS148 TODO:
    *
    *   Remember to set up any additional event
    *   callbacks that you create.
    */

    glutMainLoop();

    //
    // Cleanup code should be called here.
    //
    delete gBgIm1;
    delete gBgIm2;

    return 0;
}
