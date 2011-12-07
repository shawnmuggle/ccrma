#ifndef SCENE_H
#define SCENE_H

#include "st.h"
#include "Camera.h"
#include "ImagePlane.h"
#include "Material.h"
#include "SceneObject.h"
#include "Light.h"
#include <vector>
#include <stack>

inline STVector3 reflect(STVector3 const& I, STVector3 const& N)
{
    return I - 2 * STVector3::Dot(I, N) * N;
}

class Scene
{
public:
	Scene(std::string sceneFilename);

	/* CS 148 TODO: Add methods for the scene to render itself, etc. **/
    void traceRay(Ray const& r, STColor3f *const outColor) const;

private:

	/* Parsing helpers **/
	void Parse(std::string sceneFilename);
	void BeganParsing();
	void FinishedParsing();
	void ParsedCamera(const STPoint3& eye, const STVector3& up, const STPoint3& lookAt, float fovy, float aspect);
	void ParsedOutput(int imgWidth, int imgHeight, const std::string& outputFilename);
	void ParsedBounceDepth(int depth);
	void ParsedShadowBias(float bias);
	void ParsedPushMatrix();
	void ParsedPopMatrix();
	void ParsedRotate(float rx, float ry, float rz);
	void ParsedScale(float sx, float sy, float sz);
	void ParsedTranslate(float tx, float ty, float tz);
	void ParsedSphere(const STPoint3& center, float radius);
	void ParsedTriangle(const STPoint3& v1, const STPoint3& v2, const STPoint3& v3);
	void ParsedAmbientLight(const STColor3f& col);
	void ParsedPointLight(const STPoint3& loc, const STColor3f& col);
	void ParsedDirectionalLight(const STVector3& dir, const STColor3f& col);
	void ParsedMaterial(const STColor3f& amb, const STColor3f& diff, const STColor3f& spec, const STColor3f& mirr, float shine);

    Camera camera;
    ImagePlane imagePlane;
    int bounceDepth;
    float shadowBias;
    Material currentMaterial;
    std::vector<SceneObject> objects;
    std::vector<AmbientLight> ambientLights;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::stack<STTransform4> transformStack;
    
    void cleanup();
    void traceRayRecursive(Ray const& r, STColor3f *const outColor, int const& recursionCount, STColor3f const& multiplier) const;
    bool intersect(Ray const& r, Intersection * const outIntersection, SceneObject * const outObject) const;
    void lightObjectAtIntersection(STColor3f const& lightColor, 
                                   float const& distanceToLight,
                                   Intersection const& intersection, 
                                   Material const& material, 
                                   STVector3 const& L,
                                   STColor3f *const outDiffuseColor,
                                   STColor3f *const outSpecularColor) const;
};

#endif //SCENE_H

