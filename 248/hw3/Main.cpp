#include "Framework.h"
#include "Shader.h"

#define MODEL_PATH "models/cathedral.3ds"
//#define MODEL_PATH "models/dragon.dae"

// Note: See the SMFL documentation for info on setting up fullscreen mode
// and using rendering settings
// http://www.sfml-dev.org/tutorials/1.6/window-window.php
sf::WindowSettings settings(24, 8, 2);
sf::Window window(sf::VideoMode(800, 600), "CS248 Rules!", sf::Style::Close, settings);
 
// This is a clock you can use to control animation.  For more info, see:
// http://www.sfml-dev.org/tutorials/1.6/window-time.php
sf::Clock clck;

// This creates an asset importer using the Open Asset Import library.
// It automatically manages resources for you, and frees them when the program
// exits.
Assimp::Importer importer;
const aiScene* scene;
std::map<aiMesh *, std::vector<unsigned>*> indexBuffers;
sf::Image whiteImage;

Shader *shader;

std::map<aiMaterial *, sf::Image *> diffuseMaps;
std::map<aiMaterial *, sf::Image *> specularMaps;
std::map<aiMaterial *, sf::Image *> normalMaps;

bool left, right, forward, backward;
float yaw = 0.0, pitch = 0.0;

aiVector3D position, velocity;
float friction = 0.9;

void initOpenGL();
void loadAssets();
void handleInput();
void renderFrame();

void loadTextureMap(aiMaterial *material, aiTextureType textureType, std::map<aiMaterial *, sf::Image *> *textureMapMap, char file_suffix);
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
    glViewport(0, 0, window.GetWidth(), window.GetHeight());
    
    position = aiVector3D(40.0, -5.0, 0.0);
    yaw = M_PI * 3 / 2.0;
}


void loadTextureMap(aiMaterial *material, aiTextureType textureType, std::map<aiMaterial *, sf::Image *> *textureMapMap, char file_suffix)
{
    aiString materialName;
    material->Get(AI_MATKEY_NAME, materialName);
    
    std::map<aiMaterial *, sf::Image *>::iterator imageItr = textureMapMap->find(material);
    
    if (imageItr == textureMapMap->end())
    {
        // Haven't loaded this image yet, load it
        aiString pathString;
        material->GetTexture(textureType, 0, &pathString);
        
        // Look for a diffuse texture
        sf::Image *map = new sf::Image();
        char path[1024];
        sprintf(path, "models/%s_%c.jpg", pathString.data, file_suffix);
        bool loaded = map->LoadFromFile(path);
        if (loaded)
        {
            // Found a file!
            textureMapMap->insert(std::pair<aiMaterial *, sf::Image *>(material, map));
        }
        else
        {
            delete map;
        }
    }
    else
    {
        // This is already loaded, so skip it
    }
}

void loadAssets() {
    // Read in an asset file, and do some post-processing.  There is much 
    // more you can do with this asset loader, including load textures.
    // More info is here:
    // http://assimp.sourceforge.net/lib_html/usage.html
    scene = importer.ReadFile(MODEL_PATH,  
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcessPreset_TargetRealtime_Quality);

    if (!scene || scene->mNumMeshes <= 0) {
        std::cerr << importer.GetErrorString() << std::endl;
        exit(-1);
    }
        
    //////////////////////////////////////////////////////////////////////////
    // TODO: LOAD YOUR SHADERS/TEXTURES
    //////////////////////////////////////////////////////////////////////////
    	
    // Load the vertex shader
    shader = new Shader("Shaders/phong");
	if (!shader->loaded()) {
		std::cerr << "Shader failed to load" << std::endl;
		std::cerr << shader->errors() << std::endl;
		exit(-1);
	}
    
    for (unsigned i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];
     
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
        loadTextureMap(material, aiTextureType_DIFFUSE, &diffuseMaps, 'd');
        loadTextureMap(material, aiTextureType_SPECULAR, &specularMaps, 's');
        loadTextureMap(material, aiTextureType_NORMALS, &normalMaps, 'n');
    }
    
    whiteImage = sf::Image(1, 1, sf::Color::White);
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

void setMatrices() {
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
    
    glRotatef(rad_to_deg(pitch), 1.0, 0.0, 0.0);
    glRotatef(rad_to_deg(yaw), 0.0, 1.0, 0.0);
    glTranslatef(position.x, position.y, position.z);
}

void setMaterial(aiMesh *mesh) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D color;
    
    // Get a handle to the diffuse, specular, and ambient variables
    // inside the shader.  Then set them with the diffuse, specular, and
    // ambient color.
    GLint diffuse = glGetUniformLocation(shader->programID(), "Kd");
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    glUniform3f(diffuse, color.r, color.g, color.b);
    
    // Specular material
    GLint specular = glGetUniformLocation(shader->programID(), "Ks");
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    glUniform3f(specular, color.r, color.g, color.b);
    
    // Ambient material
    GLint ambient = glGetUniformLocation(shader->programID(), "Ka");
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    glUniform3f(ambient, color.r, color.g, color.b);
    
    // Specular power
    GLint shininess = glGetUniformLocation(shader->programID(), "alpha");
    float value;
    if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, value)) {
        glUniform1f(shininess, value);
    } else {
        glUniform1f(shininess, 1);
    }
}

sf::Image *getTextureMap(aiMesh const * inMesh, std::map<aiMaterial *, sf::Image *> *textureMapMap)
{
    aiMaterial* material = scene->mMaterials[inMesh->mMaterialIndex];

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

void setTextures(aiMesh *mesh) {
    // Get a "handle" to the texture variables inside our shader.  Then 
    // pass two textures to the shader: one for diffuse, and the other for
    // transparency.
    GLint diffuse = glGetUniformLocation(shader->programID(), "diffuseMap");
    glUniform1i(diffuse, 0); // The diffuse map will be GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    sf::Image *diffuseMap = getTextureMap(mesh, &diffuseMaps);
    diffuseMap->Bind();
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    // Transparency
    GLint specular = glGetUniformLocation(shader->programID(), "specularMap");
    glUniform1i(specular, 1); // The transparency map will be GL_TEXTURE1
    glActiveTexture(GL_TEXTURE1);
    sf::Image *specularMap = getTextureMap(mesh, &specularMaps);
    specularMap->Bind();
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}

void setMeshData(aiMesh *mesh) {
    // Get a handle to the variables for the vertex data inside the shader.
    GLint position = glGetAttribLocation(shader->programID(), "positionIn");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mVertices);
    
    // Texture coords.  Note the [0] at the end, very important
    GLint texcoord = glGetAttribLocation(shader->programID(), "texcoordIn");
    glEnableVertexAttribArray(texcoord);
    glVertexAttribPointer(texcoord, 2, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mTextureCoords[0]);
    
    // Normals
    GLint normal = glGetAttribLocation(shader->programID(), "normalIn");
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mNormals);
}

void renderNode(aiNode *node)
{
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    aiMatrix4x4 m = node->mTransformation;
    m = m.Transpose();
    glMultMatrixf((float *)&m);

    for (int i = 0; i < node->mNumMeshes; i++)
    {
        int meshIndex = node->mMeshes[i];
        aiMesh *mesh = scene->mMeshes[meshIndex];
        
        if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
            continue;
        
        setMaterial(mesh);
        setTextures(mesh);
        setMeshData(mesh);
        
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
        renderNode(childNode);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void move()
{
    // TODO: This has the classic "diagonal movement is faster" bug
    // TODO: It also has the classic "movement speed is dependent on framerate" bug
    static float speed = 0.2;
    float rad_pitch_WHY = pitch + M_PI / 2.0;
    float rad_yaw = yaw;
    
    if (forward) {
        
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch_WHY) * sin(rad_yaw),
                                      -speed * cos(rad_pitch_WHY),
                                      speed * sin(rad_pitch_WHY) * cos(rad_yaw));
        velocity += force;
    }
    if (backward) {
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch_WHY) * sin(rad_yaw),
                                      -speed * cos(rad_pitch_WHY),
                                      speed * sin(rad_pitch_WHY) * cos(rad_yaw));
        velocity -= force;
    }
    if (left) {
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch_WHY) * sin(rad_yaw + M_PI / 2.0),
                                      0.0,
                                      speed * sin(rad_pitch_WHY) * cos(rad_yaw + M_PI / 2.0));
        velocity -= force;
    }
    if (right) {
        aiVector3D force = aiVector3D(-speed * sin(rad_pitch_WHY) * sin(rad_yaw + M_PI / 2.0),
                                      0.0,
                                      speed * sin(rad_pitch_WHY) * cos(rad_yaw + M_PI / 2.0));
        velocity += force;
    }
    
    position -= velocity;
    velocity *= (1.0 - friction);
}

void renderFrame() {
    //////////////////////////////////////////////////////////////////////////
    // TODO: ADD YOUR RENDERING CODE HERE.  You may use as many .cpp files 
    // in this assignment as you wish.
    //////////////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader->programID());
    
    setMatrices();
    
    move();
    
    renderNode(scene->mRootNode);
}
