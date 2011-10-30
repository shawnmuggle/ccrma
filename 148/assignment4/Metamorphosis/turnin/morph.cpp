/****************************************************************************
 * THE GRAND METAMORPHOSIS
 * CS148 Assignment #4 - Fall 2010, Stanford University
 ****************************************************************************/

#include "st.h"
#include "stglut.h"
#include "parseConfig.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

// --------------------------------------------------------------------------
// Structure to contain an image feature for a morph. A feature is a directed
// line segment from P to Q, with coordinates in pixel units relative to the
// lower-left corner of the image.
// --------------------------------------------------------------------------

struct Feature
{
    STPoint2 P, Q;
    Feature(const STPoint2 &p, const STPoint2 &q) : P(p), Q(q) { }
};

// --------------------------------------------------------------------------
// Constants, a few global variables, and function prototypes
// --------------------------------------------------------------------------

const int kWindowWidth  = 512;
const int kWindowHeight = 512;
const int kFrames       = 30;   // number of frames to generate

STImage *gDisplayedImage = 0;   // an image to display (for testing/debugging)

std::vector<Feature> gSourceFeatures;   // feature set on source image
std::vector<Feature> gTargetFeatures;   // corresponding features on target

// Copies an image into the global image for display
void DisplayImage(STImage *image);

// --------------------------------------------------------------------------
// CS148 TODO: Implement the functions below to compute the morph
// --------------------------------------------------------------------------

void scaleColor(STImage::Pixel &color, float scale)
{
    assert(scale >= 0.0f && scale <= 1.0f);
    color.r *= scale;
    color.g *= scale;
    color.b *= scale;
}

STImage::Pixel addColors(STImage::Pixel a, STImage::Pixel b)
{
    return STImage::Pixel(a.r + b.r, a.g + b.g, a.b + b.b);
}

/**
 * Compute a linear blend of the pixel colors in two provided images according
 * to a parameter t.
 */
STImage *BlendImages(STImage *image1, STImage *image2, float t)
{
    assert(image1->GetWidth() == image2->GetWidth());
    assert(image1->GetHeight() == image2->GetHeight());
    
    STImage *result = new STImage(image1->GetWidth(), image1->GetWidth());
    
    for (int i = 0; i < result->GetHeight(); i++)
    {
        for (int j = 0; j < result->GetWidth(); j++)
        {
            STImage::Pixel a = image1->GetPixel(j, i);
            scaleColor(a, 1.0f - t);
            STImage::Pixel b = image2->GetPixel(j, i);
            scaleColor(b, t);
            STImage::Pixel c = addColors(a, b);
            result->SetPixel(j, i, c);
        }
    }
    
    return result;
}

STColor4f lerpColor(float t, STColor4f v0, STColor4f v1)
{
    STVector3 a(v0.r, v0.g, v0.b);
    STVector3 b(v1.r, v1.g, v1.b);
    STVector3 lerped = STVector3::Lerp(a, b, t);
    return STColor4f(lerped.x, lerped.y, lerped.z);
}

STColor4f bilerpColor(float s, float t, STColor4f v0, STColor4f v1, STColor4f v2, STColor4f v3)
{
    STColor4f v01 = lerpColor(s, v0, v1);
    STColor4f v23 = lerpColor(s, v2, v3);
    STColor4f v = lerpColor(t, v01, v23);
    return v;
}

bool checkBounds(int x, int y, STImage *image)
{
    return x >= 0 && x < image->GetWidth() && y >= 0 && y < image->GetHeight();
}

STImage::Pixel GetBilerpedColorAroundPoint(STPoint2 point, STImage *image)
{
    STColor4f p0(1.0f, 1.0f, 1.0f);
    STColor4f p1(1.0f, 1.0f, 1.0f);
    STColor4f p2(1.0f, 1.0f, 1.0f);
    STColor4f p3(1.0f, 1.0f, 1.0f);

    float minX = floor(point.x);
    float minY = floor(point.y);
    float maxX = ceil(point.x);
    float maxY = ceil(point.y);
     
    if ( checkBounds(minX, minY, image))
        p0 = STColor4f(image->GetPixel(minX, minY));
    
    if ( checkBounds(maxX, minY, image))
        p1 = STColor4f(image->GetPixel(maxX, minY));
    
    if ( checkBounds(minX, maxY, image))
        p2 = STColor4f(image->GetPixel(minX, maxY));
    
    if ( checkBounds(maxX, maxY, image))
        p3 = STColor4f(image->GetPixel(maxX, maxY));

    STImage::Pixel bilerpedPixel = STImage::Pixel(bilerpColor((point.x - minX) / (maxX - minX), 
                                                              (point.y - minY) / (maxY - minY), 
                                                              p0, p1, p2, p3));
    
    return bilerpedPixel;
}

/**
 * Compute a field morph on an image using two sets of corresponding features
 * according to a parameter t.  Arguments a, b, and p are weighting parameters
 * for the field morph, as described in Beier & Nelly 1992, section 3.
 */
STImage *FieldMorph(STImage *image,
                    const std::vector<Feature> &sourceFeatures,
                    const std::vector<Feature> &targetFeatures,
                    float t, float a, float b, float p)
{
    assert(sourceFeatures.size() == targetFeatures.size());
    
    STImage *result = new STImage(image->GetWidth(), image->GetWidth());
    
    for (int i = 0; i < result->GetHeight(); i++)
    {
        for (int j = 0; j < result->GetWidth(); j++)
        {
            STVector2 DSum;
            STPoint2 X(j, i);
            float weightsum = 0;
            bool print = false;
            for(int k = 0; k < targetFeatures.size(); k++)
            {
                Feature Fi = targetFeatures[k];
                STPoint2 Pi(Fi.P);
                STPoint2 Qi(Fi.Q);
                STVector2 line = Qi - Pi;
                float u = STVector2::Dot((X - Pi), line) / line.LengthSq();
                STVector2 perp(-line.y, line.x);
                float v = STVector2::Dot((X - Pi), perp) / line.Length();
                
                Feature Fip = sourceFeatures[k];
                STPoint2 Pip(Fip.P);
                STPoint2 Qip(Fip.Q);
                STVector2 newLine = Qip - Pip;
                                
                STVector2 newPerp(-newLine.y, newLine.x);
                newPerp.Normalize();
                STPoint2 Xip = Pip + u * newLine + v * newPerp;
                
                STVector2 Di = Xip - X;
                float dist;
                if ( u < 0.0f )
                {
                    dist = (Pi - X).Length();
                }
                else if ( u >= 0.0f && u <= 1.0f )
                {
                    dist = fabs(v);
                }
                else
                {
                    dist = (Qi - X).Length();
                }
                
                float weight = (pow(line.Length(), p) / (a + dist)) * b;
                
                DSum += Di * weight;
                weightsum += weight;
            }
            
            STPoint2 finalPosition = X + t * (DSum / weightsum);
            
            STImage::Pixel finalPixel = GetBilerpedColorAroundPoint(finalPosition, image);
            result->SetPixel(j, i, finalPixel);
        }
    }

    return result;
}

/**
 * Compute a morph between two images by first distorting each toward the
 * other, then combining the results with a blend operation.
 */
STImage *MorphImages(STImage *sourceImage, const std::vector<Feature> &sourceFeatures,
                     STImage *targetImage, const std::vector<Feature> &targetFeatures,
                     float t, float a, float b, float p)
{
    STImage *sourceMorph = FieldMorph(sourceImage, sourceFeatures, targetFeatures, t, a, b, p);
    STImage *targetMorph = FieldMorph(targetImage, targetFeatures, sourceFeatures, 1 - t, a, b, p);
    STImage *result = BlendImages(sourceMorph, targetMorph, t);
    
    return result;
}

/**
 * Compute a morph through time by generating appropriate values of t and
 * repeatedly calling MorphImages(). Saves the image sequence to disk.
 */
void GenerateMorphFrames(STImage *sourceImage, const std::vector<Feature> &sourceFeatures,
                         STImage *targetImage, const std::vector<Feature> &targetFeatures,
                         float a, float b, float p)
{
    // iterate and generate each required frame
    for (int i = 0; i < kFrames; ++i)
    {
        std::cout << "Metamorphosizing frame #" << i << "...";
        
        
        // **********
        // CS148 TODO: Compute a t value for the current frame and generate
        //             the morphed image here.
        // **********
        float t = (float)i / (kFrames - 1);
        float ease = -2 * pow(t, 3) + 3 * pow(t, 2);
        STImage *result = MorphImages(sourceImage, sourceFeatures, targetImage, targetFeatures, ease, a, b, p);
        
        // generate a file name to save
        std::ostringstream oss;
        oss << "frame" << std::setw(3) << std::setfill('0') << i << ".png";

        // write and deallocate the morphed image
        if (result) {
            result->Save(oss.str());
            delete result;
        }

        std::cout << " done." << std::endl;
    }
}

// --------------------------------------------------------------------------
// Utility and support code below that you do not need to modify
// --------------------------------------------------------------------------

/**
 * Copies an image into the global image for display
 */
void DisplayImage(STImage *image)
{
    // clean up the previous image
    if (gDisplayedImage) {
        delete gDisplayedImage;
        gDisplayedImage = 0;
    }

    // allocate a new image and copy it over
    if (image) {
        gDisplayedImage = new STImage(image->GetWidth(), image->GetHeight());
        size_t bytes = image->GetWidth() * image->GetHeight() * sizeof(STImage::Pixel);
        memcpy(gDisplayedImage->GetPixels(), image->GetPixels(), bytes);
    }
}

/**
 * Display callback function draws a single image to help debug
 */
void DisplayCallback()
{
    glClearColor(.2f, 2.f, 2.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gDisplayedImage)
        gDisplayedImage->Draw();

    glutSwapBuffers();
}

/**
 * Window resize callback function
 */
void ReshapeCallback(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}

/**
 * Keyboard callback function
 */
void KeyboardCallback(unsigned char key, int x, int y)
{
    switch (key)
    {
        // exit program on escape press
        case 27:
            exit(0);
            break;
        // save the currently displayed image if S is pressed
        case 's':
        case 'S':
            if (gDisplayedImage)
                gDisplayedImage->Save("screenshot.png");
            break;
        default:
            break;
    }
}

/**
 * This function is called by the parsing functions to populate the feature sets
 */
void AddFeatureCallback(STPoint2 p, STPoint2 q, ImageChoice image)
{
    if (image == IMAGE_1 || image == BOTH_IMAGES)
        gSourceFeatures.push_back(Feature(p, q));
    if (image == IMAGE_2 || image == BOTH_IMAGES)
        gTargetFeatures.push_back(Feature(p, q));
}

/**
 * Program entry point
 */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("Metamorphosis: CS148 Assignment 4");

    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardCallback);

    //
    // load the configuration from config.txt, or other file as specified
    //
    std::string configFile = "config.txt";
    if (argc > 1) configFile = argv[1];

    char sourceName[64], targetName[64];
    char saveName[64], loadName[64];
    STImage *sourceImage, *targetImage;
    parseConfigFile(configFile.c_str(),
                    sourceName, targetName,
                    saveName, loadName,
                    &sourceImage, &targetImage);
    delete sourceImage;
    delete targetImage;

    //
    // load the features from the saved features file
    //
    loadLineEditorFile(loadName, AddFeatureCallback,
                       sourceName, targetName,
                       &sourceImage, &targetImage);

    //
    // run the full morphing algorithm before going into the main loop to
    // display an image
    //

    // these weighting parameters (Beier & Nelly 1992) can be changed if desired
    const float a = 1.5f, b = 3.f, p = 0.5f;

    GenerateMorphFrames(sourceImage, gSourceFeatures,
                        targetImage, gTargetFeatures,
                        a, b, p);
    
    //
    // display a test or debug image here if desired
    // (note: comment this out if you call DisplayImage from elsewhere)
    //
//    STImage *result = sourceImage;

    // use this to test your image blending
//    STImage *result = BlendImages(sourceImage, targetImage, 0.5f);

    // use this to test your field morph
    STImage *result = FieldMorph(sourceImage, gSourceFeatures, gTargetFeatures, 1.0f, a, b, p);
    

    // use this to test your image morphing
//    STImage *result = MorphImages(sourceImage, gSourceFeatures,
//                                  targetImage, gTargetFeatures,
//                                  0.99f, a, b, p);
    
    DisplayImage(result);

    // enter the GLUT main loop
    glutMainLoop();

    return 0;
}

// --------------------------------------------------------------------------
