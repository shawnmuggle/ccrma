#include "Scene.h"
#include "Sphere.h"
#include "Triangle.h"
#include <fstream>
#include <sstream>


Scene::Scene(std::string sceneFilename)
{
	Parse(sceneFilename);
}

void Scene::Parse(std::string sceneFilename)
{
	BeganParsing();

	std::ifstream sceneFile(sceneFilename.c_str());
	char line[1024];
	while (!sceneFile.eof())
	{
		sceneFile.getline(line, 1023);
		std::stringstream ss;
		ss.str(line);
		std::string command;
		ss >> command;

		if (command == "Camera")
		{
			float ex, ey, ez, ux, uy, uz, lx, ly, lz, f, a;
			ss >> ex >> ey >> ez >> ux >> uy >> uz >> lx >> ly >> lz >> f >> a;
			STPoint3 eye(ex, ey, ez);
			STVector3 up(ux, uy, uz);
			STPoint3 lookAt(lx, ly, lz);
			ParsedCamera(eye, up, lookAt, f, a);
		}
		else
		if (command == "Output")
		{
			int w, h;
			std::string fname;
			ss >> w >> h >> fname;
			ParsedOutput(w, h, fname);
		}
		else
		if (command == "BounceDepth")
		{
			int depth;
			ss >> depth;
			ParsedBounceDepth(depth);
		}
		else if (command == "ShadowBias")
		{
			float bias;
			ss >> bias;
			ParsedShadowBias(bias);
		}
		else
		if (command == "PushMatrix")
		{
			ParsedPushMatrix();
		}
		else
		if (command == "PopMatrix")
		{
			ParsedPopMatrix();
		}
		else
		if (command == "Rotate")
		{
			float rx, ry, rz;
			ss >> rx >> ry >> rz;
			ParsedRotate(rx, ry, rz);
		}
		else
		if (command == "Scale")
		{
			float sx, sy, sz;
			ss >> sx >> sy >> sz;
			ParsedScale(sx, sy, sz);
		}
		else
		if (command == "Translate")
		{
			float tx, ty, tz;
			ss >> tx >> ty >> tz;
			ParsedTranslate(tx, ty, tz);
		}
		else
		if (command == "Sphere")
		{
			float cx, cy, cz, r;
			ss >> cx >> cy >> cz >> r;
			STPoint3 center(cx, cy, cz);
			ParsedSphere(center, r);
		}
		else
		if (command == "Triangle")
		{
			float x1, y1, z1, x2, y2, z2, x3, y3, z3;
			ss >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
			STPoint3 v[3];
			v[0] = STPoint3(x1, y1, z1);
			v[1] = STPoint3(x2, y2, z2);
			v[2] = STPoint3(x3, y3, z3);
			ParsedTriangle(v[0], v[1], v[2]);
		}
		else
		if (command == "AmbientLight")
		{
			float r, g, b;
			ss >> r >> g >> b;
			STColor3f col(r, g, b);
			ParsedAmbientLight(col);
		}
		else
		if (command == "PointLight")
		{
			float px, py, pz, r, g, b;
			ss >> px >> py >> pz >> r >> g >> b;
			STPoint3 pos(px, py, pz);
			STColor3f col(r, g, b);
			ParsedPointLight(pos, col);
		}
		else
		if (command == "DirectionalLight")
		{
			float dx, dy, dz, r, g, b;
			ss >> dx >> dy >> dz >> r >> g >> b;
			STVector3 dir(dx, dy, dz);
			STColor3f col(r, g, b);
			ParsedDirectionalLight(dir, col);
		}
		else
		if (command == "Material")
		{
			float ra, ga, ba, rd, gd, bd, rs, gs, bs, rr, gr, br, shine;
			ss >> ra >> ga >> ba >> rd >> gd >> bd >> rs >> gs >> bs >> rr >> gr >> br >> shine;
			STColor3f amb(ra, ga, ba);
			STColor3f diff(rd, gd, bd);
			STColor3f spec(rs, gs, bs);
			STColor3f mirr(rr, gr, br);
			ParsedMaterial(amb, diff, spec, mirr, shine);
		}
	}
	sceneFile.close();

	FinishedParsing();
}

void Scene::BeganParsing()
{
    printf("BEGAN PARSING\n");
    transformStack.push(STTransform4::Identity());
}

void Scene::cleanup()
{
    for (std::vector<SceneObject>::const_iterator itr = objects.begin(); itr != objects.end(); itr++)
    {
        SceneObject object = *itr;
        delete object.shape;
    }
    printf("ALL CLEANED UP\n");
}

void Scene::FinishedParsing()
{
    printf("FINISHED PARSING\n");
    
    imagePlane.generateRaysFromCamera(camera, this);    
    imagePlane.saveOutputImage();
    
    cleanup();
}

void Scene::ParsedCamera(const STPoint3& eye, const STVector3& up, const STPoint3& lookAt, float fovy, float aspect)
{
    camera.createOrthonormalBasis(eye, up, lookAt);
    imagePlane.createBoundaryPoints(camera, fovy, aspect);
#warning Why are my spheres rendering smaller than they do in the provided screenshots?!
}

void Scene::ParsedOutput(int imgWidth, int imgHeight, const std::string& outputFilename)
{
    imagePlane.createOutputImage(imgWidth, imgHeight, outputFilename);
}

void Scene::ParsedBounceDepth(int depth)
{
    bounceDepth = depth;
}

void Scene::ParsedShadowBias(float bias)
{
    shadowBias = bias;
}

void Scene::ParsedPushMatrix()
{
    transformStack.push(transformStack.top());
}

void Scene::ParsedPopMatrix()
{
    transformStack.pop();
    if (!transformStack.size())
    {
        transformStack.push(STTransform4::Identity());
    }
}

void Scene::ParsedRotate(float rx, float ry, float rz)
{
    STTransform4 ctm = transformStack.top();
    STTransform4 rotate = STTransform4::Rotation(rx, ry, rz);
    transformStack.pop();
    transformStack.push(ctm * rotate);
}

void Scene::ParsedScale(float sx, float sy, float sz)
{
    STTransform4 ctm = transformStack.top();
    STTransform4 scale = STTransform4::Scaling(sx, sy, sz);
    transformStack.pop();
    transformStack.push(ctm * scale);
}

void Scene::ParsedTranslate(float tx, float ty, float tz)
{
    STTransform4 ctm = transformStack.top();
    STTransform4 translation = STTransform4::Translation(tx, ty, tz);
    transformStack.pop();
    transformStack.push(ctm * translation);
}

void Scene::ParsedSphere(const STPoint3& center, float radius)
{
    Sphere *sphere = new Sphere(center, radius);
    SceneObject object(sphere, currentMaterial, transformStack.top());
    objects.push_back(object);
}

void Scene::ParsedTriangle(const STPoint3& v1, const STPoint3& v2, const STPoint3& v3)
{
    Triangle *triangle = new Triangle(v1, v2, v3);
    SceneObject object(triangle, currentMaterial, transformStack.top());
    objects.push_back(object);
}

void Scene::ParsedAmbientLight(const STColor3f& col)
{
    AmbientLight light(col);
    ambientLights.push_back(light);
}

void Scene::ParsedPointLight(const STPoint3& loc, const STColor3f& col)
{
    PointLight light(loc, col);
    pointLights.push_back(light);
}

void Scene::ParsedDirectionalLight(const STVector3& dir, const STColor3f& col)
{
    DirectionalLight light(dir, col);
    directionalLights.push_back(light);
}

void Scene::ParsedMaterial(const STColor3f& amb, const STColor3f& diff, const STColor3f& spec, const STColor3f& mirr, float shine)
{
    currentMaterial = Material(amb, diff, spec, mirr, shine);
}

bool Scene::intersect(Ray const& r, Intersection * const outIntersection, SceneObject * const outObject) const
{
    bool intersects = false;
    float minT = MAXFLOAT;
    Intersection minIntersection;
    SceneObject minObject;
    for (std::vector<SceneObject>::const_iterator itr = objects.begin(); itr != objects.end(); itr++)
    {
        SceneObject object = *itr;
        Intersection intersection;
        if (object.intersectionWithRay(r, &intersection))
        {
            intersects = true;
            if (intersection.t < minT)
            {
                minT = intersection.t;
                minIntersection = intersection;
                minObject = object;
            }
        }
    }
    *outIntersection = minIntersection;
    *outObject = minObject;
    return intersects;
}

STColor3f Scene::traceRay(Ray const& r) const
{
    STColor3f outputColor;
    Intersection intersection;
    SceneObject object;
    bool intersects = intersect(r, &intersection, &object);
        
    if (intersects)
    {
        STColor3f ambientColor = STColor3f();
        for (std::vector<AmbientLight>::const_iterator aItr = ambientLights.begin(); aItr != ambientLights.end(); aItr++)
        {
            AmbientLight aLight = *aItr;
            ambientColor += aLight.color;
        }
        ambientColor *= object.material.ambient;                    
        
#warning Do something about potential color overflow??
        STColor3f diffuseColor;
        STColor3f specularColor;
        for (std::vector<PointLight>::const_iterator pItr = pointLights.begin(); pItr != pointLights.end(); pItr++)
        {
            PointLight pLight = *pItr;
            
            STVector3 L = pLight.position - intersection.position;            
            L.Normalize();

            // Check for shadow
            Ray shadowRay(intersection.position, L, shadowBias, 100000);
            Intersection shadowIntersection;
            SceneObject shadowObject;
            if (intersect(shadowRay, &shadowIntersection, &shadowObject))
                continue;
                
            // Diffuse lighting
            float diffuseAmount = fmax(0, STVector3::Dot(L, intersection.normal));
            diffuseColor += object.material.diffuse * pLight.color * diffuseAmount;
            
            // Specular lighting
            L *= -1.0f;
            STVector3 R = L - 2 * STVector3::Dot(L, intersection.normal) * intersection.normal;
            R.Normalize();
            STVector3 V = camera.position - intersection.position;
            V.Normalize();
            float specularAmount = powf(fmaxf(0, STVector3::Dot(R, V)), object.material.shine);
            specularColor += object.material.specular * pLight.color * specularAmount;
        }
        
        for (std::vector<DirectionalLight>::const_iterator dItr = directionalLights.begin(); dItr != directionalLights.end(); dItr++)
        {
            DirectionalLight dLight = *dItr;
            
            // Diffuse lighting
            STVector3 L = -dLight.direction;
            L.Normalize();
            
            // Check for shadow
            Ray shadowRay(intersection.position, L, shadowBias, MAXFLOAT);
            Intersection shadowIntersection;
            SceneObject shadowObject;
            if (intersect(shadowRay, &shadowIntersection, &shadowObject))
                continue;
            
            float diffuseAmount = fmax(0, STVector3::Dot(L, intersection.normal));
            diffuseColor += object.material.diffuse * dLight.color * diffuseAmount;
            
            // Specular lighting
            L *= -1.0f;
            STVector3 R = L - 2 * STVector3::Dot(L, intersection.normal) * intersection.normal;
            R.Normalize();
            STVector3 V = camera.position - intersection.position;
            V.Normalize();
            float specularAmount = powf(fmaxf(0, STVector3::Dot(R, V)), object.material.shine);
            specularColor += object.material.specular * dLight.color * specularAmount;
        }
        
        outputColor = ambientColor + diffuseColor + specularColor;
    }
    
    return outputColor;
}
