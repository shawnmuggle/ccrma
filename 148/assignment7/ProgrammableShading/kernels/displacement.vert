// displacement.vert

/*
 This GLSL vertex shader performs displacement mapping
 using an analytical displacement function.
 */

// This 'varying' vertex output can be read as an input
// by a fragment shader that makes the same declaration.
varying vec2 texPos;
varying vec3 modelPos;
varying vec3 lightSource;
varying vec3 normal;

varying float displacement;

uniform float time;

float h(float u, float v)
{
    float PI = 3.14159265358979323846264;
    float s = 0.03;
    float a = 10.0;
    
    
    float circular_ripple = 0.;
    float r = 0.;
    r = length(vec2(u - 0.15, v - 0.05));
    circular_ripple = circular_ripple + pow(r, 2.8) * s * cos(a * 2. * PI * pow(r, 1.) - time);
    r = length(vec2(u - 0.23, v - 0.3));
    circular_ripple = circular_ripple + pow(r, 4.95) * s * sin(a * 2. * PI * pow(r, 1.051) - time);
    r = length(vec2(u - 0.79, v - 0.14));
    circular_ripple = circular_ripple + pow(r, 1.4) * s * cos(a * 2. * PI * pow(r, 1.24) - time);
    r = length(vec2(u - 0.59, v - 0.03));
    circular_ripple = circular_ripple + pow(r, 1.5) * s * cos(a * 2. * PI * pow(r, 1.12) - time);
    r = length(vec2(u - 0.13, v - 0.91));
    circular_ripple = circular_ripple + pow(r, 1.71) * s * cos(a * 2. * PI * pow(r, 1.585) - time);
    
    float perturbance = 0.;
    perturbance = perturbance + 0.0029 * sin(time * 0.1) * sin(5.2 * 2. * PI * u - time / 2. + 0.4);
    perturbance = perturbance + 0.0019 * cos(time * 0.12) * cos(7.8 * 2. * PI * u - time / 5.8 - 0.1);
    perturbance = perturbance + 0.0028 * sin(time * 0.04) * sin(3.56 * 2. * PI * v - time / 1.4 + 8.4);
    perturbance = perturbance + 0.0024 * cos(time * 0.16) * cos(9.79 * 2. * PI * v - time / 1.9 - 0.56);
    perturbance = perturbance + 0.0022 * sin(time * 0.01) * sin(2.87 * 2. * PI * u - time / 4.3 + 0.4);
    perturbance = perturbance + 0.0026 * cos(time * 0.07) * cos(13.3 * 2. * PI * v - time / 3.1 - 5.74);
    
    return circular_ripple + perturbance;
}

void main()
{
    // Tell the fragment shader we have done vertex displacement
    displacement = 1.0;
    
	normal = gl_Normal.xyz;
	modelPos = gl_Vertex.xyz;
    
    // Copy the standard OpenGL texture coordinate to the output.
    texPos = gl_MultiTexCoord0.xy;
    
	/* CS 148 TODO: Modify 'modelPos' and 'normal' using your displacment function */
    
    float delta = 0.00001;
    float displacementValue = h(texPos.x, texPos.y);
    
    float dh_du = (h(texPos.x + delta, texPos.y) - h(texPos.x, texPos.y)) / delta;
    float dh_dv = (h(texPos.x, texPos.y + delta) - h(texPos.x, texPos.y)) / delta;
    
    vec3 T1 = normalize(vec3(1, dh_du, 0));
    vec3 T2 = normalize(vec3(0, -dh_dv, -1));
    vec3 displacementNormal = cross(T1, T2);
    vec3 divergence = displacementNormal - vec3(0.0, 1.0, 0.0);

    vec3 displacementVector = displacementValue * normal;    
    modelPos = modelPos + displacementVector;
    normal = normalize(normal + divergence);
    
    // Render the shape using modified position.
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix *  vec4(modelPos,1);
    
    // we may need this in the fragment shader...
    modelPos = (gl_ModelViewMatrix * vec4(modelPos,1)).xyz;
    
	// send the normal to the shader
	normal = (gl_NormalMatrix * normal);
    
    // push light source through modelview matrix to find its position
    // in model space and pass it to the fragment shader.
    lightSource = gl_LightSource[0].position.xyz;
}
