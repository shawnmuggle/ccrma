#include "Framework.h"
#include "Shader.h"
#include "DepthRenderTarget.h"

#define CATHEDRAL_MODEL_PATH "models/cathedral.3ds"
//#define ARMADILLO_MODEL_PATH "models/armadillo.3ds"
#define ARMADILLO_MODEL_PATH "models/sphere.3ds"

#define RENDER_WIDTH 800
#define RENDER_HEIGHT 600
#define SHADOW_MAP_RATIO 5

// Note: See the SMFL documentation for info on setting up fullscreen mode
// and using rendering settings
// http://www.sfml-dev.org/tutorials/1.6/window-window.php
sf::WindowSettings settings(24, 8, 2);
sf::Window window(sf::VideoMode(RENDER_WIDTH, RENDER_HEIGHT), "CS248 Rules!", sf::Style::Close, settings);
 
// This is a clock you can use to control animation.  For more info, see:
// http://www.sfml-dev.org/tutorials/1.6/window-time.php
sf::Clock clck;

// This creates an asset importer using the Open Asset Import library.
// It automatically manages resources for you, and frees them when the program
// exits.
Assimp::Importer cathedralImporter;
const aiScene* cathedralScene;
Assimp::Importer armadilloImporter;
const aiScene* armadilloScene;
std::map<aiMesh *, std::vector<unsigned>*> indexBuffers;
sf::Image whiteImage;

Shader *phongShader;
Shader *simpleShader;
Shader *envMapShader;

std::map<aiMaterial *, sf::Image *> diffuseMaps;
std::map<aiMaterial *, sf::Image *> specularMaps;
std::map<aiMaterial *, sf::Image *> normalMaps;

bool left, right, forward, backward;
bool lightLeft, lightRight, lightUp, lightDown;
float yaw = 0.0, pitch = 0.0;
float lightYaw = 0.0f, lightPitch = M_PI / 9, lightDistance = 15.0f;
bool test;

aiVector3D position, velocity;
float friction = 0.9;

void initOpenGL();
void loadAssets();
void handleInput();
void renderFrame();

// ENVMAP STUFF

void generateCubeMap();
GLuint cubeMapTextureID;

// END ENVMAP STUFF

// SHADOW STUFF

DepthRenderTarget *depthRenderTarget;

// END SHADOW STUFF

void loadTextureMap(aiMaterial *material, std::map<aiMaterial *, sf::Image *> *textureMapMap, char file_suffix);
sf::Image *getTextureMap(aiMesh const * inMesh, std::map<aiMaterial *, sf::Image *> *textureMapMap);

int main(int argc, char** argv) {

    initOpenGL();
    loadAssets();

    // Put your game loop here (i.e., render with OpenGL, update animation)
    while (window.IsOpened()) {
        handleInput();
        renderFrame();
        window.Display();
    }

    return 0;
}

void initOpenGL() {
    // Initialize GLEW on Windows, to make sure that OpenGL 2.0 is loaded
#ifdef FRAMEWORK_USE_GLEW
    GLint error = glewInit();
    if (GLEW_OK != error) {
        std::cerr << glewGetErrorString(error) << std::endl;
        exit(-1);
    }
    if (!GLEW_VERSION_2_0 || !GL_EXT_framebuffer_object) {
        std::cerr << "This program requires OpenGL 2.0 and FBOs" << std::endl;
        exit(-1);
    }
#endif

    // This initializes OpenGL with some common defaults.  More info here:
    // http://www.sfml-dev.org/tutorials/1.6/window-opengl.php
    glClearDepth(1.0f);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glViewport(0, 0, window.GetWidth(), window.GetHeight());
    
    position = aiVector3D(35.0, -20.0, 0.0);
    yaw = M_PI * 3 / 2.0;
    pitch = -M_PI / 7;
    
    GLfloat light0_position[] = { 0.0, 30.0, 0.0, 0.0 };
    GLfloat light0_ambient[] = { 245/2550.0, 222/2550.0, 179/2550.0, 1 };
    GLfloat light0_diffuse[] = { 245/255.0, 232/255.0, 199/255.0, 1 };
    GLfloat light0_specular[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat shininess = 90;
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv( GL_LIGHT0, GL_AMBIENT, light0_ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light0_specular );
    glLightfv( GL_LIGHT0, GL_SHININESS, &shininess );
    glEnable(GL_LIGHT0);
    
    GLfloat light1_position[] = { 0, 11, 0, 0.0 };
    GLfloat light1_diffuse[] = { 145/2550.0, 222/2550.0, 149/2550.0, 1 };
    GLfloat light1_specular[] = { 0.1, 0.1, 0.1, 1 };
    glLightfv( GL_LIGHT1, GL_POSITION, light1_position );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, light1_diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, light1_specular );
    glEnable(GL_LIGHT1);
    
    depthRenderTarget = new DepthRenderTarget(RENDER_WIDTH * SHADOW_MAP_RATIO, RENDER_HEIGHT * SHADOW_MAP_RATIO);
}

void loadTextureMap(aiMaterial *material, std::map<aiMaterial *, sf::Image *> *textureMapMap, char file_suffix)
{
    aiString materialName;
    material->Get(AI_MATKEY_NAME, materialName);
    
    std::map<aiMaterial *, sf::Image *>::iterator imageItr = textureMapMap->find(material);
    
    if (imageItr == textureMapMap->end())
    {
        // Haven't loaded this image yet, load it
        aiString pathString;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &pathString);
        
        // Look for a diffuse texture
        sf::Image *map = new sf::Image();
        char path[1024];
        sprintf(path, "models/%s_%c.jpg", pathString.data, file_suffix);
        printf("LOOKING FOR texture: %s\n", path);
        bool loaded = map->LoadFromFile(path);
        
        if (loaded)
        {
            // Found a file!
            printf("GOT ITTTT\n");
            textureMapMap->insert(std::pair<aiMaterial *, sf::Image *>(material, map));
            
            // set stuff up
            map->Bind();
            glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
            glGenerateMipmapEXT(GL_TEXTURE_2D);
        }
        else
        {
            printf("NOPE\n");
            delete map;
        }
    }
    else
    {
        // This is already loaded, so skip it
    }
}

void loadTexturesAndMeshIndices(aiScene const * scene)
{
    for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        aiMesh *mesh = scene->mMeshes[meshIndex];
        
        // Load index buffers
        // Set up the index buffer.  Each face should have 3 vertices since we
        // specified aiProcess_Triangulate
        std::vector<unsigned> *indexBuffer = new std::vector<unsigned>();
        indexBuffer->reserve(mesh->mNumFaces * 3);
        for (unsigned i = 0; i < mesh->mNumFaces; i++) {
            for (unsigned j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
                indexBuffer->push_back(mesh->mFaces[i].mIndices[j]);
            }
        }
        indexBuffers.insert(std::pair<aiMesh *, std::vector<unsigned>*>(mesh, indexBuffer));
        
        // Load textures
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        loadTextureMap(material, &diffuseMaps, 'd');
        loadTextureMap(material, &specularMaps, 's');
        loadTextureMap(material, &normalMaps, 'n');
    }
}

void loadAssets() {
    // Read in an asset file, and do some post-processing.  There is much 
    // more you can do with this asset loader, including load textures.
    // More info is here:
    // http://assimp.sourceforge.net/lib_html/usage.html
    cathedralScene = cathedralImporter.ReadFile(CATHEDRAL_MODEL_PATH,  
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcessPreset_TargetRealtime_Quality);

    if (!cathedralScene || cathedralScene->mNumMeshes <= 0) {
        std::cerr << cathedralImporter.GetErrorString() << std::endl;
        exit(-1);
    }
    
    armadilloScene = armadilloImporter.ReadFile(ARMADILLO_MODEL_PATH,  
                                       aiProcess_CalcTangentSpace |
                                       aiProcess_Triangulate |
                                       aiProcess_JoinIdenticalVertices |
                                       aiProcessPreset_TargetRealtime_Quality);
    
    if (!armadilloScene || armadilloScene->mNumMeshes <= 0) {
        std::cerr << armadilloImporter.GetErrorString() << std::endl;
        exit(-1);
    }
        
    //////////////////////////////////////////////////////////////////////////
    // TODO: LOAD YOUR SHADERS/TEXTURES
    //////////////////////////////////////////////////////////////////////////
    	
    // Load the vertex shader
    phongShader = new Shader("Shaders/phong");
	if (!phongShader->loaded()) {
		std::cerr << "Shader failed to load" << std::endl;
		std::cerr << phongShader->errors() << std::endl;
		exit(-1);
	}

    simpleShader = new Shader("Shaders/simple");
	if (!simpleShader->loaded()) {
		std::cerr << "Shader failed to load" << std::endl;
		std::cerr << simpleShader->errors() << std::endl;
		exit(-1);
	}

    envMapShader = new Shader("Shaders/envmap");
	if (!envMapShader->loaded()) {
		std::cerr << "Shader failed to load" << std::endl;
		std::cerr << envMapShader->errors() << std::endl;
		exit(-1);
	}
    
    loadTexturesAndMeshIndices(cathedralScene);
    loadTexturesAndMeshIndices(armadilloScene);
    
    whiteImage = sf::Image(1, 1, sf::Color::White);
    
    generateCubeMap();
}

void handleInput() {
    //////////////////////////////////////////////////////////////////////////
    // TODO: ADD YOUR INPUT HANDLING HERE.
    //////////////////////////////////////////////////////////////////////////

    // Event loop, for processing user input, etc.  For more info, see:
    // http://www.sfml-dev.org/tutorials/1.6/window-events.php
    sf::Event evt;
    while (window.GetEvent(evt)) {
        switch (evt.Type) {
            case sf::Event::KeyPressed:
                switch(evt.Key.Code)
            {
                case sf::Key::A:
                    left = true;
                    break;
                case sf::Key::W:
                    forward = true;
                    break;
                case sf::Key::S:
                    backward = true;
                    break;
                case sf::Key::D:
                    right = true;
                    break;
                case sf::Key::Up:
                    lightUp = true;
                    break;
                case sf::Key::Left:
                    lightLeft = true;
                    break;                    
                case sf::Key::Down:
                    lightDown = true;
                    break;                    
                case sf::Key::Right:
                    lightRight = true;
                    break;
                case sf::Key::T:
                    test = !test;
                    break;
                default:
                    break;
            }
                break;
            case sf::Event::KeyReleased:
                switch(evt.Key.Code)
            {
                case sf::Key::A:
                    left = false;
                    break;
                case sf::Key::W:
                    forward = false;
                    break;
                case sf::Key::S:
                    backward = false;
                    break;
                case sf::Key::D:
                    right = false;
                    break;
                case sf::Key::Up:
                    lightUp = false;
                    break;
                case sf::Key::Left:
                    lightLeft = false;
                    break;                    
                case sf::Key::Down:
                    lightDown = false;
                    break;                    
                case sf::Key::Right:
                    lightRight = false;
                    break;
                default:
                    break;
            }
                break;
            case sf::Event::MouseMoved:
                static float rotation_sensitivity = 0.01;
                static float prev_mouse_x = evt.MouseMove.X;
                static float prev_mouse_y = evt.MouseMove.Y;
                yaw -= rotation_sensitivity * (prev_mouse_x - evt.MouseMove.X);
                pitch += rotation_sensitivity * (prev_mouse_y - evt.MouseMove.Y);
                
                if (pitch < -M_PI / 2.0f) pitch = -M_PI / 2.0f;
                if (pitch > M_PI / 2.0f) pitch = M_PI / 2.0f;
                
                prev_mouse_x = evt.MouseMove.X;
                prev_mouse_y = evt.MouseMove.Y;
                break;
            case sf::Event::Closed: 
                // Close the window.  This will cause the game loop to exit,
                // because the IsOpened() function will no longer return true.
                window.Close(); 
                break;
            case sf::Event::Resized: 
                // If the window is resized, then we need to change the perspective
                // transformation and viewport
                glViewport(0, 0, evt.Size.Width, evt.Size.Height);
                break;
            default: 
                break;
        }
    }
}

float rad_to_deg(float angle)
{
    return 180.0 * (angle / M_PI);
}

float deg_to_rad(float angle)
{
    return M_PI * (angle / 180.0);
}

void move()
{
    // TODO: This has the classic "diagonal movement is faster" bug
    // TODO: It also has the classic "movement speed is dependent on framerate" bug
    static float speed = 0.2;
    float rad_pitch = pitch + M_PI / 2.0;
    float rad_yaw = yaw;
    
    if (forward) {
        
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch) * sin(rad_yaw),
                                      -speed * cos(rad_pitch),
                                      speed * sin(rad_pitch) * cos(rad_yaw));
        velocity += force;
    }
    if (backward) {
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch) * sin(rad_yaw),
                                      -speed * cos(rad_pitch),
                                      speed * sin(rad_pitch) * cos(rad_yaw));
        velocity -= force;
    }
    if (left) {
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch) * sin(rad_yaw + M_PI / 2.0),
                                      0.0,
                                      speed * sin(rad_pitch) * cos(rad_yaw + M_PI / 2.0));
        velocity -= force;
    }
    if (right) {
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch) * sin(rad_yaw + M_PI / 2.0),
                                      0.0,
                                      speed * sin(rad_pitch) * cos(rad_yaw + M_PI / 2.0));
        velocity += force;
    }
    
    position -= velocity;
    velocity *= (1.0 - friction);
}

void updateLightPosition()
{
    if (lightUp) {
        lightPitch += 0.01;
    }
    if (lightDown) {
        lightPitch -= 0.01;
    }
    if (lightLeft) {
        lightYaw -= 0.01;
    }
    if (lightRight) {
        lightYaw += 0.01;
    }
    if (lightPitch > M_PI / 3.0f) lightPitch = M_PI / 3.0f;
    if (lightPitch < 0.05f) lightPitch = 0.05f;
}

// TODO: remove this?
void transNode(aiScene const *scene, aiNode const *node)
{
    if ( node == NULL ) return;
    aiMatrix4x4 m = node->mTransformation;
    
    float mat[16] = {m.a1,m.b1,m.c1,m.d1,m.a2,m.b2,m.c2,m.d2,m.a3,
        m.b3,m.c3,m.d3,m.a4,m.b4,m.c4,m.d4};
    
    glMultMatrixf(mat);
    
    for (int i = 0; i < (int)node->mNumChildren; i++) 
    {
        transNode(scene, node->mChildren[i]);
    }
}

void setMatrices()
{
    // Set up the projection and model-view matrices
    GLfloat aspectRatio = (GLfloat)window.GetWidth()/window.GetHeight();
    GLfloat nearClip = 0.1f;
    GLfloat farClip = 500.0f;
    GLfloat fieldOfView = 45.0f; // Degrees
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfView, aspectRatio, nearClip, farClip);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
        
    glPushMatrix(); // set the light in the scene correctly
    transNode(cathedralScene, cathedralScene->mRootNode);
    GLfloat light0_position[] = { 0, 30, 0, 0 };
    glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
    GLfloat light1_position[] = { 0, 11, 0, 0.0 };
    glLightfv( GL_LIGHT1, GL_POSITION, light1_position );
    glPopMatrix();
    
    glRotatef(rad_to_deg(pitch), 1.0, 0.0, 0.0);
    glRotatef(rad_to_deg(yaw), 0.0, 1.0, 0.0);
    glTranslatef(position.x, position.y, position.z);

    // We store the modelview matrix here, but it's actually the view matrix because we haven't done any model transforms yet
//    static double viewMatrix[16];
    GLfloat *viewMatrix = new GLfloat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
    GLint inverseViewMatrixUniform = glGetUniformLocation(envMapShader->programID(), "inverseViewMatrix");
    glUniformMatrix4fv(inverseViewMatrixUniform, 1, true, viewMatrix);
    delete [] viewMatrix;
}

void setMaterial(aiScene const * scene, aiMesh *mesh) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D color;
    
    // Get a handle to the diffuse, specular, and ambient variables
    // inside the shader.  Then set them with the diffuse, specular, and
    // ambient color.
    GLint diffuse = glGetUniformLocation(phongShader->programID(), "Kd");
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    glUniform3f(diffuse, color.r, color.g, color.b);
    
    // Specular material
    GLint specular = glGetUniformLocation(phongShader->programID(), "Ks");
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    glUniform3f(specular, color.r, color.g, color.b);
    
    // Ambient material
    GLint ambient = glGetUniformLocation(phongShader->programID(), "Ka");
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    glUniform3f(ambient, color.r, color.g, color.b);
    
    // Specular power
    GLint shininess = glGetUniformLocation(phongShader->programID(), "alpha");
    float value;
    if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, value)) {
        glUniform1f(shininess, value);
    } else {
        glUniform1f(shininess, 20);
    }
}

sf::Image *getTextureMap(aiScene const * scene, aiMesh const * mesh, std::map<aiMaterial *, sf::Image *> *textureMapMap)
{
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    aiString materialName;
    material->Get(AI_MATKEY_NAME, materialName);
    std::map<aiMaterial *, sf::Image *>::iterator itr = textureMapMap->find(material);
    if (itr != textureMapMap->end())
    {
        // texture found, return it
        return itr->second;
    }
    else
    {
        return &whiteImage;
    }
}

void setTextures(Shader *shader, aiScene const * scene, aiMesh const * mesh) {

    if (shader == envMapShader)
    {
        GLint env = glGetUniformLocation(shader->programID(), "cubeMap");
        glUniform1i(env, 4); // The env map will be GL_TEXTURE3
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    else if (shader == phongShader)
    {
        
        // Get a "handle" to the texture variables inside our shader.  Then 
        // pass two textures to the shader: one for diffuse, and the other for
        // transparency.
        GLint diffuse = glGetUniformLocation(phongShader->programID(), "diffuseMap");
        glUniform1i(diffuse, 0); // The diffuse map will be GL_TEXTURE0
        glActiveTexture(GL_TEXTURE0);
        sf::Image *diffuseMap = getTextureMap(scene, mesh, &diffuseMaps);
        diffuseMap->Bind();
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        
        
        // Transparency
        GLint specular = glGetUniformLocation(phongShader->programID(), "specularMap");
        glUniform1i(specular, 1); // The transparency map will be GL_TEXTURE1
        glActiveTexture(GL_TEXTURE1);
        sf::Image *specularMap = getTextureMap(scene, mesh, &specularMaps);
        specularMap->Bind();
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        
        // Normals
        GLint normal = glGetUniformLocation(phongShader->programID(), "normalMap");
        glUniform1i(normal, 2); // The normal map will be GL_TEXTURE2
        glActiveTexture(GL_TEXTURE2);
        sf::Image *normalMap = getTextureMap(scene, mesh, &normalMaps);
        normalMap->Bind();
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        
        GLint shadow = glGetUniformLocation(phongShader->programID(), "shadowMap");
        glUniform1i(shadow, 7);  // Why use texture 7? Who knows! Fabien knows best.
        glActiveTexture(GL_TEXTURE7);
        glBindTexture( GL_TEXTURE_2D, depthRenderTarget->textureID());
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }
}

void setMeshData(aiMesh *mesh, Shader *shader) {
    // Get a handle to the variables for the vertex data inside the shader.
    GLint position = glGetAttribLocation(shader->programID(), "positionIn");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mVertices);

    if (shader != simpleShader)
    {
        // Normals
        GLint normal = glGetAttribLocation(shader->programID(), "normalIn");
        glEnableVertexAttribArray(normal);
        glVertexAttribPointer(normal, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mNormals);
    }
    
    if (shader == phongShader)
    {
        // Texture coords.  Note the [0] at the end, very important
        GLint texcoord = glGetAttribLocation(shader->programID(), "texcoordIn");
        glEnableVertexAttribArray(texcoord);
        glVertexAttribPointer(texcoord, 2, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mTextureCoords[0]);

        // Tangents
        GLint tangent = glGetAttribLocation(shader->programID(), "tangentIn");
        glEnableVertexAttribArray(tangent);
        glVertexAttribPointer(tangent, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mTangents);
    }
}

void setLightMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-25, 25, -25, 25, -20, 40);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    aiVector3D lightPos;
    lightPos.x = lightDistance * -cosf(lightYaw) * cosf(lightPitch);
    lightPos.y = lightDistance * sinf(lightPitch);
    lightPos.z = lightDistance * sinf(lightYaw) * cosf(lightPitch);

    gluLookAt(lightPos.x, lightPos.y, lightPos.z,
              0.0f, 0.0f, 0.0f, 
              0.0f, 1.0f, 0.0f);
    
    // PACKAGE UP THE LIGHT MATRICES FOR THE SHADER
    // (so that we can do it once per frame on the CPU instead of a hojillion times on the GPU)
    static double modelView[16];
	static double projection[16];
    
	const GLdouble bias[16] =
    {
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    };
	
	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
    // store active texture to restore afterwards
    GLint activeTxtUnit;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTxtUnit);
    
    // change texture7's matrix
	glMatrixMode(GL_TEXTURE);
    glActiveTexture(GL_TEXTURE7);  // Store the light bias * projection * modelview matrix in texture 7 (??)
	
	glLoadIdentity();	
	glLoadMatrixd(bias);
    glMultMatrixd(projection);
	glMultMatrixd(modelView);
	
	// Go back to modelview matrix mode
	glMatrixMode(GL_MODELVIEW);
    
    // reload previous active texture
    glActiveTexture(activeTxtUnit);
}

void renderNode(Shader *shader, aiScene const * scene, aiNode *node, bool doShadows)
{    
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    aiMatrix4x4 m = node->mTransformation;
    m = m.Transpose();
    glMultMatrixf((float *)&m);

    if (shader == phongShader)
    {
        // store active texture to restore afterwards
        GLint activeTxtUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTxtUnit);
        
        glMatrixMode(GL_TEXTURE);
        glActiveTexture(GL_TEXTURE6);
        glPushMatrix();
        glMultMatrixf((float *)&m);
        
        glActiveTexture(activeTxtUnit);
    }
    
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        int meshIndex = node->mMeshes[i];
        aiMesh *mesh = scene->mMeshes[meshIndex];
        
        if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
            continue;
        
        if (shader == phongShader)
        {
            setMaterial(scene, mesh);
        }
        if (shader != simpleShader)
        {
            setTextures(shader, scene, mesh);
        }
        setMeshData(mesh, shader);
        
        // Draw the mesh
        std::map<aiMesh *, std::vector<unsigned>*>::iterator itr = indexBuffers.find(mesh);
        if (itr != indexBuffers.end())
        {
            std::vector<unsigned> indexBuffer = *itr->second;
            // Found an index buffer! Draw it
            glDrawElements(GL_TRIANGLES,  3*mesh->mNumFaces, GL_UNSIGNED_INT, &indexBuffer[0]);
        }
    }
    
    for (int i = 0; i < node->mNumChildren; i++)
    {
        aiNode *childNode = node->mChildren[i];
        renderNode(shader, scene, childNode, doShadows);
    }
    
    if (shader == phongShader)
    {
        // store active texture to restore afterwards
        GLint activeTxtUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTxtUnit);
        
        glMatrixMode(GL_TEXTURE);
        glActiveTexture(GL_TEXTURE6);
        glPopMatrix();
        
        glActiveTexture(activeTxtUnit);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

#define CUBE_MAP_SIZE RENDER_HEIGHT
#include <sstream>
void generateCubeMap()
{
    glViewport(0, 0, CUBE_MAP_SIZE, CUBE_MAP_SIZE);

    glUseProgram(phongShader->programID());
    GLint drawShadows = glGetUniformLocation(phongShader->programID(), "drawShadows");
    glUniform1i(drawShadows, 0);

    glGenTextures(1, &cubeMapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);    
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (uint face = 0; face < 6; face++) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up the projection and model-view matrices
        GLfloat aspectRatio = 1.0f;
        GLfloat nearClip = 0.1f;
        GLfloat farClip = 500.0f;
        GLfloat fieldOfView = 90.0f; // Degrees
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fieldOfView, aspectRatio, nearClip, farClip);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        switch (face) {
            case 0:
                /// positive x
                gluLookAt(0.0f, 2.0f, 0.0f, 
                          1.0f, 2.0f, 0.0f, 
                          0.0f, 1.0f, 0.0f);
                break;
            case 1:
                // negative x
                gluLookAt(0.0f, 2.0f, 0.0f, 
                          -1.0f, 2.0f, 0.0f, 
                          0.0f, 1.0f, 0.0f);
                break;
            case 2:
                // positive y
                gluLookAt(0.0f, 2.0f, 0.0f, 
                          0.0f, 3.0f, 0.0f, 
                          0.0f, 0.0f, -1.0f);
                break;
            case 3:
                // negative y
                gluLookAt(0.0f, 2.0f, 0.0f, 
                          0.0f, 1.0f, 0.0f, 
                          0.0f, 0.0f, 1.0f);
                break;
            case 4:
                // positive z
                gluLookAt(0.0f, 2.0f, 0.0f, 
                          0.0f, 2.0f, 1.0f, 
                          0.0f, 1.0f, 0.0f);
                break;
            case 5:
                // negative z
                gluLookAt(0.0f, 2.0f, 0.0f, 
                          0.0f, 2.0f, -1.0f, 
                          0.0f, 1.0f, 0.0f);
                break;
            default:
                break;
        }
     

        
        // TODO: WHY SO DARK CUBEMAP
        glPushMatrix(); // set the light in the scene correctly
        
        transNode(cathedralScene, cathedralScene->mRootNode);  // TODO: STILL NEED TO FIGURE OUT WHY I DO THIS

        GLfloat light0_position[] = { 0, 30, 0, 0 };
        glLightfv( GL_LIGHT0, GL_POSITION, light0_position );

        GLfloat light1_position[] = { 0, 11, 0, 0.0 };
        glLightfv( GL_LIGHT1, GL_POSITION, light1_position );
        
        glPopMatrix();

        
        renderNode(phongShader, cathedralScene, cathedralScene->mRootNode, false);
        
        // Copy the back buffer into the current face of the cube map
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
                     CUBE_MAP_SIZE, CUBE_MAP_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
        glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 0, 0, 0, 0, CUBE_MAP_SIZE, CUBE_MAP_SIZE);
    }
    
    glUniform1i(drawShadows, 1);
    glViewport(0, 0, window.GetWidth(), window.GetHeight());
}

void renderFrame() {
    //////////////////////////////////////////////////////////////////////////
    // TODO: ADD YOUR RENDERING CODE HERE.  You may use as many .cpp files 
    // in this assignment as you wish.
    //////////////////////////////////////////////////////////////////////////

    // Move the player based on keyboard input (wasd)
    move();
    
    // Move the light based on keyboard input (arrows)
    updateLightPosition();
    
    // Generate a shadow map, and send the light matrix over to the shader in texture matrix 7
    depthRenderTarget->bind();
    glUseProgram(simpleShader->programID());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setLightMatrix();
    renderNode(simpleShader, cathedralScene, cathedralScene->mRootNode, true);
    renderNode(simpleShader, armadilloScene, armadilloScene->mRootNode, true);
    depthRenderTarget->unbind();

    // Render our scene, sending the model matrices over to the shader in texture matrix 6
    glUseProgram(phongShader->programID());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setMatrices();
    renderNode(phongShader, cathedralScene, cathedralScene->mRootNode, true);

    // Render the armadillo using the environment map shader.
    glUseProgram(envMapShader->programID());    
    setMatrices();
    renderNode(envMapShader, armadilloScene, armadilloScene->mRootNode, true);
    
    // Display a test quad on screen (press t key to toggle)
    if (test)
    {
        // Render test quad
        glDisable(GL_LIGHTING);
        glUseProgramObjectARB(0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-RENDER_WIDTH/2,RENDER_WIDTH/2,-RENDER_HEIGHT/2,RENDER_HEIGHT/2,1,20);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4f(1,1,1,1);
        glActiveTextureARB(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthRenderTarget->textureID());
        glEnable(GL_TEXTURE_2D);
        glTranslated(0,-RENDER_HEIGHT/2,-1);
        glBegin(GL_QUADS);
        glTexCoord2d(0,0);glVertex3f(0,0,0);
        glTexCoord2d(1,0);glVertex3f(RENDER_WIDTH/2,0,0);
        glTexCoord2d(1,1);glVertex3f(RENDER_WIDTH/2,RENDER_HEIGHT/2,0);
        glTexCoord2d(0,1);glVertex3f(0,RENDER_HEIGHT/2,0);
        glEnd();
        glEnable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        depthRenderTarget->unbind();
    }
}
